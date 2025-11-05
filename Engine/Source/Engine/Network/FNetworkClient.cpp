///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Network/FNetworkClient.hpp>
#include <Engine/Core/Utils/FLogger.hpp>
#include <Engine/Runtime/Actor/AActor.hpp>
#include <Engine/Static/FEngineInterface.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
FNetworkClient::FNetworkClient(void)
    : FNetworkBase()
    , m_connectionState(EConnectionState::Disconnected)
    , m_clientID(0)
    , m_lastUpdate(SteadyClock::now())
{
    InitializePacketManager();
    SetupDefaultHandlers();
}

///////////////////////////////////////////////////////////////////////////////
FNetworkClient::~FNetworkClient() { Cleanup(); }

///////////////////////////////////////////////////////////////////////////
bool FNetworkClient::Connect(const std::string& hostname, UInt16 port)
{
    std::lock_guard<std::mutex> lock(m_connectionMutex);

    // Don't allow connecting if already connected or connecting
    if (m_connectionState == EConnectionState::Connected ||
        m_connectionState == EConnectionState::Connecting)
    {
        return false;
    }
    try
    {
        // Resolve hostname to endpoint
        asio::ip::udp::resolver resolver(m_ioContext);
        auto endpoints = resolver.resolve(hostname, std::to_string(port));

        if (endpoints.empty()) { return false; }

        // Get the first resolved endpoint
        m_serverEndpoint = *endpoints.begin();

        // Start the network if not already started
        if (!IsRunning())
        {
            if (!Start()) { return false; }
        }

        m_connectionState = EConnectionState::Connecting;

        // Create connection information
        m_connection = std::make_unique<FConnectionInformation>();
        m_connection->endpoint = m_serverEndpoint;
        m_connection->lastActivity = SteadyClock::now();
        m_connection->connected = false;

        FLogger::SetNamespace("Network");
        FLogger::Info(
            "Attempting to connect to server at {}:{}", hostname, port
        );

        Packets::Connect connectPacket;
        connectPacket.gameName = m_settings->game.title;
        connectPacket.gameVersion = m_settings->game.version;

        FLogger::Info(
            "[CLIENT] Sending Connect packet - gameName: '{}', gameVersion: '{}'",
            connectPacket.gameName,
            connectPacket.gameVersion
        );

        if (SendReliablePacket(connectPacket, m_serverEndpoint))
        {
            m_lastUpdate = SteadyClock::now();
            return true;   // Connection attempt initiated successfully
        }
        else
        {
            m_connectionState = EConnectionState::Disconnected;
            return false;   // Failed to send connection packet
        }
    }
    catch (const std::exception&)
    {
        m_connectionState = EConnectionState::Disconnected;
        return false;
    }
}

///////////////////////////////////////////////////////////////////////////////
bool FNetworkClient::Start(void)
{
    if (m_running.load()) { return true; }

    try
    {
        // Create socket (but don't bind - clients don't need to listen on
        // specific port)
        m_socket = std::make_unique<FSocket>(m_ioContext);
        m_socket->open(asio::ip::udp::v4());

        m_running = true;

        // Start network thread
        m_networkThread =
            std::make_unique<FThread>([this]() { RunNetworkThread(); });
        m_networkThread->Start();

        StartReceive();
        return true;
    }
    catch (const std::exception&)
    {
        Stop();
        return false;
    }
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkClient::HandleConnectResponsePacket(
    const Packets::ConnectResponse& packet, const FEndpoint& endpoint
)
{
    std::lock_guard<std::mutex> lock(m_connectionMutex);

    if (m_connectionState != EConnectionState::Connecting) { return; }

    FLogger::SetNamespace("Network");
    FLogger::Info("Received connection response from server");

    if (packet.accepted)
    {
        m_connectionState = EConnectionState::Connected;
        m_clientID = packet.clientID;
        m_connection->connected = true;

        Emit<Events::Connected>({ packet.clientID, endpoint });
        FLogger::SetNamespace("Network");
        FLogger::Info(
            "Successfully connected to server! Client ID: {}", packet.clientID
        );
    }
    else
    {
        // Connection rejected by server
        m_connectionState = EConnectionState::Disconnected;

        FLogger::SetNamespace("Network");
        FLogger::Warn("Connection to server was rejected");

        // Emit connection failed event
        Emit<Events::ConnectionFailed>({ endpoint });
    }
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkClient::Disconnect(EDisconnectionReason reason)
{
    // SEGFAULT FIX: Only disconnect if not already stopped
    // This prevents double-disconnect during shutdown
    if (!m_running.load())
    {
        return;   // Already stopped, just perform internal cleanup
    }

    // Perform disconnection
    DisconnectInternal(reason, true);
    // Stop network thread if running
    Stop();
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkClient::DisconnectInternal(
    EDisconnectionReason reason, bool sendPacket
)
{
    std::lock_guard<std::mutex> lock(m_connectionMutex);

    // Only send disconnect packet if we're connected and requested to do so
    if (sendPacket && m_connectionState == EConnectionState::Connected)
    {
        Packets::Disconnect disconnectPacket;
        disconnectPacket.clientID = m_clientID;
        disconnectPacket.reason = static_cast<UInt32>(reason);
        SendPacket(disconnectPacket, m_serverEndpoint);
    }

    // Update connection state
    if (m_connectionState != EConnectionState::Disconnected)
    {
        m_connectionState = EConnectionState::Disconnected;
        EmitEvent(Events::Disconnected{ reason });
    }

    // Reset connection data
    m_clientID = 0;
    m_connection.reset();
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkClient::SetupDefaultHandlers(void)
{
    // Register handler for connect response
    RegisterPacketHandler<Packets::ConnectResponse>(
        [this](
            const Packets::ConnectResponse& packet, const FEndpoint& endpoint
        ) { HandleConnectResponsePacket(packet, endpoint); }
    );

    // Register handler for disconnect packets
    RegisterPacketHandler<Packets::Disconnect>(
        [this](const Packets::Disconnect& packet, const FEndpoint& endpoint)
        { HandleDisconnectPacket(packet, endpoint); }
    );

    // Register handler for heartbeat packets
    RegisterPacketHandler<Packets::HeartBeat>(
        [this](const Packets::HeartBeat& packet, const FEndpoint& endpoint)
        { HandleHeartbeatPacket(packet, endpoint); }
    );

    // Register handler for Snapshot packets
    RegisterPacketHandler<Packets::Snapshot>(
        [this](const Packets::Snapshot& packet, const FEndpoint& endpoint)
        { HandleSnapshotPacket(packet, endpoint); }
    );

    // Register handler for property replication packets
    RegisterPacketHandler<Packets::Replication>(
        [this](const Packets::Replication& packet, const FEndpoint& endpoint)
        { HandlePropertyReplicationPacket(packet, endpoint); }
    );
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkClient::Update(TKD_MAYBE_UNUSED float deltaTime)
{
    // Don't update if not running
    if (!IsRunning()) { return; }

    // Update Superclass (process incoming packets)
    Super::Update(deltaTime);

    // Don't continue updating if disconnected after timeout
    if (m_connectionState == EConnectionState::Disconnected)
    {
        FLogger::SetNamespace("Network");
        FLogger::Info("Disconnected - stopping network");
        Stop();
        return;
    }

    auto now = SteadyClock::now();

    CheckConnectionAttemptTimeout(now);
    SendHeartbeat(now);
    CheckConnectionTimeout(now);

    // Replicate dirty properties to server if connected
    if (m_connectionState == EConnectionState::Connected)
    {
        // ReplicateDirtyProperties();
    }
}

///////////////////////////////////////////////////////////////////////////////
EConnectionState FNetworkClient::GetConnectionState(void) const
{
    std::lock_guard<std::mutex> lock(m_connectionMutex);
    return m_connectionState;
}

///////////////////////////////////////////////////////////////////////////////
std::optional<UInt32> FNetworkClient::GetClientID(void) const
{
    std::lock_guard<std::mutex> lock(m_connectionMutex);

    if (m_connectionState == EConnectionState::Connected)
    {
        return m_clientID;
    }

    return std::nullopt;
}

///////////////////////////////////////////////////////////////////////////////
bool FNetworkClient::IsConnected(void) const
{
    std::lock_guard<std::mutex> lock(m_connectionMutex);

    return m_connectionState == EConnectionState::Connected;
}

///////////////////////////////////////////////////////////////////////////////
FEndpoint FNetworkClient::GetServerEndpoint(void) const
{
    return m_serverEndpoint;
}

///////////////////////////////////////////////////////////////////////////////
bool FNetworkClient::SendPacketToServer(const IPacket& packet)
{
    std::lock_guard<std::mutex> lock(m_connectionMutex);

    if (m_connectionState == EConnectionState::Connected ||
        m_connectionState == EConnectionState::Connecting)
    {
        return FNetworkBase::SendPacket(packet, m_serverEndpoint);
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkClient::CheckConnectionTimeout(const TimePoint& now)
{
    // Only check timeout if we're connected
    if (m_connectionState != EConnectionState::Connected || !m_connection)
    {
        return;
    }

    // Calculate time since last activity from server
    auto timeSinceLastActivity =
        std::chrono::duration_cast<std::chrono::seconds>(
            now - m_connection->lastActivity
        );

    // If no activity from server for CONNECTION_TIMEOUT seconds, disconnect
    if (timeSinceLastActivity >= CONNECTION_TIMEOUT)
    {
        // Timeout reached - disconnect
        FLogger::SetNamespace("Network");
        FLogger::Warn(
            "Connection timeout - no server activity for {} seconds",
            CONNECTION_TIMEOUT.count()
        );
        FLogger::Info("Disconnecting due to timeout");

        // Disconnect due to timeout
        DisconnectInternal(EDisconnectionReason::Timeout, false);
    }
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkClient::SendHeartbeat(const TimePoint& now)
{
    if (m_connectionState != EConnectionState::Connected) { return; }

    auto timeSinceLastUpdate =
        std::chrono::duration_cast<std::chrono::seconds>(now - m_lastUpdate);

    if (timeSinceLastUpdate < HEARTBEAT_INTERVAL) { return; }

    Packets::HeartBeat heartbeat;
    heartbeat.id = m_clientID;
    heartbeat.timestamp = GetCurrentTimestamp();

    if (SendPacketToServer(heartbeat)) { m_lastUpdate = now; }
    else { FLogger::Warn("Failed to send heartbeat packet to server"); }
}

//////////////////////////////////////////////////////////////////////////////
void FNetworkClient::CheckConnectionAttemptTimeout(const TimePoint& now)
{
    if (m_connectionState != EConnectionState::Connecting) { return; }

    auto timeSinceConnectAttempt =
        std::chrono::duration_cast<std::chrono::seconds>(now - m_lastUpdate);

    // Timeout after 5 seconds if no ConnectResponse received
    if (timeSinceConnectAttempt >= CONNECTION_RETRY_TIMEOUT)
    {
        FLogger::SetNamespace("Network");
        FLogger::Warn(
            "Connection attempt timed out after {} seconds",
            CONNECTION_RETRY_TIMEOUT.count()
        );

        std::lock_guard<std::mutex> lock(m_connectionMutex);
        m_connectionState = EConnectionState::Disconnected;

        // Emit connection failed event
        Emit<Events::ConnectionFailed>({ m_serverEndpoint });
    }
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkClient::HandleDisconnectPacket(
    const Packets::Disconnect& packet, const FEndpoint& endpoint
)
{
    if (endpoint != m_serverEndpoint) { return; }

    EDisconnectionReason reason =
        static_cast<EDisconnectionReason>(packet.reason);

    if (reason == EDisconnectionReason::Shutdown)
    {
        FLogger::SetNamespace("Network");
        FLogger::Info("Server is shutting down - disconnecting");
    }
    else
    {
        FLogger::SetNamespace("Network");
        FLogger::Info(
            "Received disconnect packet from server. Reason: {}",
            static_cast<int>(reason)
        );
    }

    DisconnectInternal(reason, false);
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkClient::HandleHeartbeatPacket(
    const Packets::HeartBeat& packet, const FEndpoint& endpoint
)
{
    // We don't use the packet data for anything currently
    TKD_UNUSED(packet);

    // Only accept heartbeat packets from our server
    if (endpoint != m_serverEndpoint ||
        m_connectionState != EConnectionState::Connected)
    {
        return;
    }

    if (m_connection) { m_connection->lastActivity = SteadyClock::now(); }
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkClient::HandleSnapshotPacket(
    const Packets::Snapshot& packet, const FEndpoint& endpoint
)
{
    // Only accept snapshot packets from our server
    if (endpoint != m_serverEndpoint ||
        m_connectionState != EConnectionState::Connected)
    {
        return;
    }

    {
        // Prepare a buffer to serialize the snapshot packet
        std::vector<Byte> packetData;
        FBinaryWriter writer(packetData);

        // Serialize the packet into the buffer
        if (!packet.Serialize(writer)) { return; }

        // Create a RemoteProcedureCall packet to handle the snapshot
        Packets::RemoteProcedureCall rpc(
            "SyncSnapshot", ERPCType::Client, UUID::Nil, packetData
        );

        // Enqueue the RPC for deferred execution
        std::lock_guard lock(m_rpcQueueMutex);
        m_deferredRPCs.push({ rpc, endpoint });
    }

    // Update last activity time
    if (m_connection) { m_connection->lastActivity = SteadyClock::now(); }
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkClient::HandlePropertyReplicationPacket(
    const Packets::Replication& packet, const FEndpoint& endpoint
)
{
    // Only accept replication packets from our server
    if (endpoint != m_serverEndpoint ||
        m_connectionState != EConnectionState::Connected)
    {
        return;
    }

    // Queue the property replication for deferred execution to avoid deadlock
    // The network thread queues replications here without blocking on
    // m_worldMutex The world thread will process them via
    // ProcessDeferredPropertyReplications()
    std::lock_guard<std::mutex> lock(m_propertyQueueMutex);
    m_deferredPropertyReplications.push({ packet, endpoint });
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkClient::ReplicateDirtyProperties(void)
{
    // Get the world subsystem from the engine
    auto* worldSubsystem = Engine::GetInstance().GetWorld();
    if (!worldSubsystem) { return; }

    // Collect properties to replicate
    std::vector<IProperty*> toReplicate;

    worldSubsystem->WithWorld(
        [&toReplicate](UWorld& world)
        {
            // RACE CONDITION FIX: Copy actors to avoid iterator invalidation
            auto actors = world.GetActors();

            for (const auto& actor: actors)
            {
                if (!actor) { continue; }

                TVector<IProperty*> properties;
                actor->GetLifetimeReplicatedProperties(properties);

                for (auto* property: properties)
                {
                    // Only replicate properties that have the Replicated flag
                    // and are dirty
                    if (property && property->IsDirty() &&
                        property->HasFlag(EPropertyFlags::Replicated))
                    {
                        // Then push the modified property to the vector
                        std::cout << "[CLIENT] who is it bum: "
                                  << property->GetName().CStr() << std::endl;
                        toReplicate.push_back(property);
                    }
                }
            }
        }
    );
    SendReplication(toReplicate);
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkClient::SendReplication(std::vector<IProperty*> toReplicate)
{
    // Send all collected properties
    for (auto* property: toReplicate)
    {
        if (!property) { continue; }

        auto& owner = property->GetOwner();

        // Create replication packet
        Packets::Replication replicationPacket;

        // Set actor ID from UUID
        replicationPacket.actorID = owner.GetUUID().Data();

        replicationPacket.propertyName = property->GetName();
        replicationPacket.timestamp = GetCurrentTimestamp();

        // Serialize the property to binary data
        replicationPacket.data = property->Serialize();

        // Send the replication packet to the server
        if (SendPacketToServer(replicationPacket)) { property->ClearDirty(); }
        else
        {
            FLogger::SetNamespace("Network");
            FLogger::Warn(
                "Failed to replicate property '{}' of actor '{}'",
                property->GetName().CStr(),
                owner.GetUUID()
            );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkClient::Cleanup(void)
{
    // SEGFAULT FIX: Only perform cleanup if still running
    // During shutdown, Stop() is already called by subsystem
    if (!m_running.load())
    {
        FLogger::SetNamespace("Network");
        FLogger::Info("Client already stopped, skipping cleanup");
        return;
    }

    FLogger::SetNamespace("Network");
    FLogger::Info("Cleaning up client resources");
    Disconnect(EDisconnectionReason::Shutdown);
}

}   // namespace tkd
