///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Network/FNetworkServer.hpp>
#include <Engine/Core.hpp>
#include <Engine/Core/Utils/FLogger.hpp>
#include <Engine/Network/FBinaryWriter.hpp>
#include <Engine/Network/Packets.hpp>
#include <Engine/Runtime/Actor/AActor.hpp>
#include <Engine/Static/FEngineInterface.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{
///////////////////////////////////////////////////////////////////////////////
FNetworkServer::FNetworkServer(UInt16 port)
    : FNetworkBase()
    , m_port(port)
    , m_lastUpdate(SteadyClock::now())
{
    InitializePacketManager();
    SetupDefaultHandlers();
}

///////////////////////////////////////////////////////////////////////////////
FNetworkServer::~FNetworkServer() { Cleanup(); }

///////////////////////////////////////////////////////////////////////////////
void FNetworkServer::DisconnectClient(
    UInt32 clientID, EDisconnectionReason reason
)
{
    std::lock_guard<std::mutex> lock(m_connectionsMutex);
    auto it = m_clientIDToEndpoint.find(clientID);
    if (it != m_clientIDToEndpoint.end())
    {
        // Send disconnect packet
        Packets::Disconnect disconnectPacket;
        disconnectPacket.clientID = clientID;
        disconnectPacket.reason = static_cast<UInt32>(reason);
        SendPacket(disconnectPacket, it->second);

        // Emit event
        EmitEvent(Events::ClientDisconnected{ clientID, reason });

        // Log disconnection
        FLogger::SetNamespace("Network");
        FLogger::Info("Client ID: {} disconnected", clientID);

        // Remove connection
        m_connections.erase(it->second);
        m_clientIDToEndpoint.erase(it);
    }
}

///////////////////////////////////////////////////////////////////////////////
bool FNetworkServer::Start(void)
{
    if (m_running.load())
    {
        FLogger::SetNamespace("Network");
        FLogger::Warn("Server is already running");
        return true;
    }

    FLogger::SetNamespace("Network");
    FLogger::Info("Starting server on port {}", m_port);

    try
    {
        // Create and bind socket
        m_socket = std::make_unique<FSocket>(m_ioContext);
        m_socket->open(asio::ip::udp::v4());

        m_socket->bind(FEndpoint(asio::ip::udp::v4(), m_port));
        FLogger::Info("Socket bound successfully");

        m_running = true;

        // Start network thread
        FLogger::Info("Starting network thread");
        m_networkThread = std::make_unique<FThread>(
            [this]()
            {
                FLogger::SetNamespace("Network");
                FLogger::Info("Network thread started");
                RunNetworkThread();
            }
        );
        m_networkThread->Start();

        EmitEvent(Events::ServerStarted{ m_port });
        FLogger::Info("Server started successfully on port {}", m_port);
        StartReceive();

        return true;
    }
    catch (const std::exception& e)
    {
        FLogger::SetNamespace("Network");
        FLogger::Error("Failed to start server: {}", e.what());
        Stop();
        return false;
    }
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkServer::Update(TKD_MAYBE_UNUSED float deltaTime)
{
    // Call base class update (if any)
    Super::Update(deltaTime);

    // Get current time
    auto now = SteadyClock::now();

    CheckConnectionTimeouts(now);
    SendHeartbeats(now);

    // ReplicateDirtyProperties();

    m_lastUpdate = now;
}

///////////////////////////////////////////////////////////////////////////////
Bool FNetworkServer::BroadcastPacket(const IPacket& packet)
{
    std::lock_guard<std::mutex> lock(m_connectionsMutex);
    Bool result = true;
    for (const auto& [endpoint, connection]: m_connections)
    {
        if (connection->connected)
        {
            Bool current = SendPacket(packet, endpoint);
            if (!current) { result = false; }
        }
    }
    return result;
}

///////////////////////////////////////////////////////////////////////////////
Bool FNetworkServer::BroadcastReliablePacket(const IPacket& packet)
{
    std::lock_guard<std::mutex> lock(m_connectionsMutex);
    Bool result = true;
    for (const auto& [endpoint, connection]: m_connections)
    {
        if (connection->connected)
        {
            Bool current = SendReliablePacket(packet, endpoint);
            if (!current) { result = false; }
        }
    }
    return result;
}

///////////////////////////////////////////////////////////////////////////////
Bool FNetworkServer::BroadcastData(const std::vector<Byte>& data)
{
    std::lock_guard<std::mutex> lock(m_connectionsMutex);
    Bool result = true;
    for (const auto& [endpoint, connection]: m_connections)
    {
        if (connection->connected)
        {
            Bool current = SendData(data, endpoint);
            if (!current) { result = false; }
        }
    }
    return result;
}

///////////////////////////////////////////////////////////////////////////////
bool FNetworkServer::SendPacketToClient(const IPacket& packet, UInt32 clientID)
{
    std::lock_guard<std::mutex> lock(m_connectionsMutex);
    auto it = m_clientIDToEndpoint.find(clientID);
    if (it != m_clientIDToEndpoint.end())
    {
        return SendPacket(packet, it->second);
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
SizeT FNetworkServer::GetConnectedClientCount(void) const
{
    std::lock_guard<std::mutex> lock(m_connectionsMutex);
    SizeT count = 0;
    for (const auto& [endpoint, connection]: m_connections)
    {
        if (connection->connected) { count++; }
    }
    return count;
}

///////////////////////////////////////////////////////////////////////////////
std::vector<UInt32> FNetworkServer::GetConnectedClients(void) const
{
    std::lock_guard<std::mutex> lock(m_connectionsMutex);
    std::vector<UInt32> clients;

    for (const auto& [clientID, endpoint]: m_clientIDToEndpoint)
    {
        auto connIt = m_connections.find(endpoint);
        if (connIt != m_connections.end() && connIt->second->connected)
        {
            clients.push_back(clientID);
        }
    }
    return clients;
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkServer::OnPacketReceived(
    const FPacketHeader& header, const FEndpoint& sender
)
{
    std::lock_guard<std::mutex> lock(m_connectionsMutex);

    auto it = m_connections.find(sender);
    if (it != m_connections.end())
    {
        it->second->lastActivity = SteadyClock::now();
        it->second->lastSequenceReceived = header.sequenceNumber;
    }
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkServer::SetupDefaultHandlers(void)
{
    RegisterPacketHandler<Packets::Connect>(
        [this](const Packets::Connect& packet, const FEndpoint& endpoint)
        { HandleConnectPacket(packet, endpoint); }
    );

    RegisterPacketHandler<Packets::Disconnect>(
        [this](const Packets::Disconnect& packet, const FEndpoint& endpoint)
        { HandleDisconnectPacket(packet, endpoint); }
    );

    RegisterPacketHandler<Packets::HeartBeat>(
        [this](const Packets::HeartBeat& packet, const FEndpoint& endpoint)
        { HandleHeartbeatPacket(packet, endpoint); }
    );

    RegisterPacketHandler<Packets::Replication>(
        [this](const Packets::Replication& packet, const FEndpoint& endpoint)
        { HandlePropertyReplicationPacket(packet, endpoint); }
    );
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkServer::CheckConnectionTimeouts(const TimePoint& now)
{
    std::lock_guard<std::mutex> lock(m_connectionsMutex);

    std::vector<FEndpoint> toDisconnect;

    for (const auto& [endpoint, connection]: m_connections)
    {
        if (connection->connected &&
            (now - connection->lastActivity) > CONNECTION_TIMEOUT)
        {
            toDisconnect.push_back(endpoint);
        }
    }

    // Disconnect timed out clients
    for (const auto& endpoint: toDisconnect)
    {
        auto it = m_connections.find(endpoint);
        if (it != m_connections.end())
        {
            uint32_t clientID = it->second->clientID;

            // Send disconnect packet
            Packets::Disconnect disconnectPacket;
            disconnectPacket.clientID = clientID;
            disconnectPacket.reason =
                static_cast<UInt32>(EDisconnectionReason::Timeout);
            SendPacket(disconnectPacket, endpoint);

            // Emit event
            EmitEvent(
                Events::ClientDisconnected{ clientID,
                                            EDisconnectionReason::Timeout }
            );

            // Remove connection
            m_connections.erase(it);
            m_clientIDToEndpoint.erase(clientID);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkServer::SendHeartbeats(const TimePoint& now)
{
    static auto lastHeartbeat = SteadyClock::now();

    if ((now - lastHeartbeat) < HEARTBEAT_INTERVAL) { return; }

    std::lock_guard<std::mutex> lock(m_connectionsMutex);

    static uint32_t pingId = 0;

    for (const auto& [endpoint, connection]: m_connections)
    {
        if (connection->connected)
        {
            Packets::HeartBeat heartbeat;
            heartbeat.id = ++pingId;
            heartbeat.timestamp = GetCurrentTimestamp();
            SendPacket(heartbeat, endpoint);
        }
    }

    lastHeartbeat = now;
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkServer::HandleDisconnectPacket(
    const Packets::Disconnect& packet, const FEndpoint& endpoint
)
{
    TKD_UNUSED(packet);
    std::lock_guard<std::mutex> lock(m_connectionsMutex);

    auto it = m_connections.find(endpoint);
    if (it != m_connections.end())
    {
        uint32_t clientID = it->second->clientID;
        m_connections.erase(it);
        m_clientIDToEndpoint.erase(clientID);

        FLogger::SetNamespace("Network");
        FLogger::Info("Client ID: {} disconnected", clientID);
    }
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkServer::HandleConnectPacket(
    const Packets::Connect& packet, const FEndpoint& endpoint
)
{
    TKD_UNUSED(packet);
    std::lock_guard<std::mutex> lock(m_connectionsMutex);

    // Check if client is already connected
    auto it = m_connections.find(endpoint);
    if (it != m_connections.end() && it->second->connected)
    {
        return;   // Already connected
    }

    // Log new connection
    FLogger::SetNamespace("Network");
    FLogger::Info(
        "New connection from {}:{}",
        endpoint.address().to_string(),
        endpoint.port()
    );

    // Create new connection
    auto connection = std::make_unique<FConnectionInformation>();
    connection->endpoint = endpoint;
    connection->clientID = m_nextClientID++;
    connection->lastActivity = SteadyClock::now();
    connection->connected = true;

    UInt32 assignedClientId = connection->clientID;

    // Store connection
    m_connections[endpoint] = std::move(connection);
    m_clientIDToEndpoint[assignedClientId] = endpoint;

    // Send response
    Packets::ConnectResponse response;
    response.clientID = assignedClientId;
    response.accepted = true;

    // Emit event
    EmitEvent(Events::ClientConnected{ assignedClientId, endpoint });

    SendReliablePacket(response, endpoint);

    {
        // Add a new RPC deferred call to spawn the client entity
        Packets::RemoteProcedureCall rpc(
            "SpawnClient", ERPCType::Server, UUID::World, assignedClientId
        );

        // Add parameters to the RPC
        std::lock_guard<std::mutex> lock(m_rpcQueueMutex);
        m_deferredRPCs.push({ rpc, endpoint });
    }
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkServer::HandleHeartbeatPacket(
    const Packets::HeartBeat& packet, const FEndpoint& endpoint
)
{
    TKD_UNUSED(packet);
    std::lock_guard<std::mutex> lock(m_connectionsMutex);

    auto it = m_connections.find(endpoint);
    if (it != m_connections.end())
    {
        it->second->lastActivity = SteadyClock::now();
    }
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkServer::HandlePropertyReplicationPacket(
    const Packets::Replication& packet, const FEndpoint& endpoint
)
{
    TKD_UNUSED(endpoint);

    // Queue the property replication for deferred execution to avoid deadlock
    // The network thread queues replications here without blocking on
    // m_worldMutex The world thread will process them via
    // ProcessDeferredPropertyReplications()
    std::lock_guard<std::mutex> lock(m_propertyQueueMutex);
    m_deferredPropertyReplications.push({ packet, endpoint });
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkServer::ReplicateDirtyProperties(void)
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
                        std::cout << "[SERVER] who is it bum: "
                                  << property->GetName().CStr() << std::endl;
                        toReplicate.push_back(property);
                    }
                }
            }
        }
    );

    // Send all collected properties
    for (auto* property: toReplicate)
    {
        if (!property) { continue; }

        auto& owner = property->GetOwner();

        // Cast to AActor to get the owning client ID
        auto* actor = dynamic_cast<AActor*>(&owner);
        if (!actor) { continue; }

        UInt32 owningClientID = actor->GetOwningClientID();

        // Create replication packet
        Packets::Replication replicationPacket;

        // Set actor ID from UUID
        replicationPacket.actorID = owner.GetUUID().Data();

        replicationPacket.propertyName = property->GetName();
        replicationPacket.timestamp = GetCurrentTimestamp();

        // Serialize the property to binary data
        replicationPacket.data = property->Serialize();

        // Send the replication packet to the owning client
        if (SendPacketToClient(replicationPacket, owningClientID))
        {
            property->ClearDirty();

            FLogger::SetNamespace("Network");
            FLogger::Info(
                "Server replicated property '{}' of actor '{}' to client {}",
                property->GetName().CStr(),
                owner.GetUUID(),
                owningClientID
            );
        }
        else
        {
            FLogger::SetNamespace("Network");
            FLogger::Warn(
                "Failed to replicate property '{}' of actor '{}' to client {}",
                property->GetName().CStr(),
                owner.GetUUID(),
                owningClientID
            );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkServer::Cleanup(void)
{
    FLogger::SetNamespace("Network");
    FLogger::Info("Server destructor called, notifying clients of shutdown");

    // Send disconnect notifications to all connected clients
    {
        std::lock_guard<std::mutex> lock(m_connectionsMutex);

        FLogger::Info(
            "Sending shutdown notifications to {} clients",
            m_connections.size()
        );

        for (const auto& [endpoint, connection]: m_connections)
        {
            if (connection->connected)
            {
                // Send disconnect packet with ServerShutdown reason
                Packets::Disconnect disconnectPacket;
                disconnectPacket.clientID = connection->clientID;
                disconnectPacket.reason =
                    static_cast<UInt32>(EDisconnectionReason::Shutdown);

                FLogger::Info(
                    "Notifying client ID: {} at {}:{}",
                    connection->clientID,
                    endpoint.address().to_string(),
                    endpoint.port()
                );

                SendPacket(disconnectPacket, endpoint);
            }
        }
    }

    // Give packets time to be sent before stopping
    FLogger::Info("Waiting for disconnect packets to be sent...");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Stop the server
    if (m_running.load())
    {
        FLogger::Info("Stopping server network services...");
        Stop();
        EmitEvent(Events::ServerStopped{ m_port });
    }

    // Clear all connections
    {
        std::lock_guard<std::mutex> lock(m_connectionsMutex);
        m_connections.clear();
        m_clientIDToEndpoint.clear();
    }
}

///////////////////////////////////////////////////////////////////////////////
FConnectionInformation*
    FNetworkServer::GetClientInformation(UInt32 clientID) const
{
    std::lock_guard<std::mutex> lock(m_connectionsMutex);
    auto it = m_clientIDToEndpoint.find(clientID);
    if (it != m_clientIDToEndpoint.end())
    {
        auto connIt = m_connections.find(it->second);
        if (connIt != m_connections.end()) { return connIt->second.get(); }
    }
    return nullptr;
}

}   // namespace tkd
