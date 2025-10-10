///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Network/FNetworkClient.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
FNetworkClient::FNetworkClient()
    : FNetworkBase()
    , m_connectionState(EConnectionState::Disconnected)
    , m_clientID(0)
    , m_lastUpdate(SteadyClock::now())
{
    InitializePacketManager();
    SetupDefaultHandlers();
}

///////////////////////////////////////////////////////////////////////////////
FNetworkClient::~FNetworkClient()
{
    std::cout << "[CLIENT] Destructor called" << std::endl;

    // Call cleanup first (sends disconnect packet)
    Cleanup();

    // ✅ CRITICAL: Wait for UDP packet to actually transmit
    std::cout << "[CLIENT] Waiting for packet transmission..." << std::endl;
    // std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "[CLIENT] Destructor completed" << std::endl;
}

///////////////////////////////////////////////////////////////////////////
bool FNetworkClient::Connect(const std::string& hostname, UInt16 port)
{
    std::lock_guard<std::mutex> lock(m_connectionMutex);
    std::cout << "we connecting typeshit" << std::endl;
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

        Packets::Connect connectPacket;
        std::cout << "sending connection packet to server" << std::endl;
        if (SendPacket(connectPacket, m_serverEndpoint))
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

    std::cout << "connection packet received" << std::endl;

    if (m_connectionState != EConnectionState::Connecting) { return; }

    if (packet.accepted)
    {
        m_connectionState = EConnectionState::Connected;
        m_clientID = packet.clientID;
        m_connection->connected = true;

        Emit<Events::Connected>({ packet.clientID, endpoint });
        std::cout << "Successfully connected to server! Client ID: "
                  << packet.clientID << std::endl;
    }
    else
    {
        m_connectionState = EConnectionState::Disconnected;

        // Emit connection failed event
        Emit<Events::ConnectionFailed>({ endpoint });
    }
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkClient::Disconnect(EDisconnectionReason reason)
{
    DisconnectInternal(reason, true);
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
        std::cout << "sending over packet in disconnect internals"
                  << std::endl;
        Packets::Disconnect disconnectPacket;
        disconnectPacket.clientID = m_clientID;
        disconnectPacket.reason = static_cast<UInt32>(reason);

        FNetworkBase::SendPacket(disconnectPacket, m_serverEndpoint);
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
void FNetworkClient::SetupDefaultHandlers()
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
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkClient::Update(TKD_MAYBE_UNUSED float deltaTime)
{
    if (!IsRunning()) { return; }

    // Don't continue updating if disconnected after timeout
    if (m_connectionState == EConnectionState::Disconnected)
    {
        std::cout << "[CLIENT] Not updating - disconnected" << std::endl;
        Stop();
        return;
    }

    auto now = SteadyClock::now();

    CheckConnectionAttemptTimeout(now);
    SendHeartbeat(now);
    CheckConnectionTimeout(now);
}

///////////////////////////////////////////////////////////////////////////////
EConnectionState FNetworkClient::GetConnectionState() const
{
    std::lock_guard<std::mutex> lock(m_connectionMutex);

    return m_connectionState;
}

///////////////////////////////////////////////////////////////////////////////
std::optional<UInt32> FNetworkClient::GetClientID() const
{
    std::lock_guard<std::mutex> lock(m_connectionMutex);

    if (m_connectionState == EConnectionState::Connected)
    {
        return m_clientID;
    }

    return std::nullopt;
}

///////////////////////////////////////////////////////////////////////////////
bool FNetworkClient::IsConnected() const
{
    std::lock_guard<std::mutex> lock(m_connectionMutex);

    return m_connectionState == EConnectionState::Connected;
}

///////////////////////////////////////////////////////////////////////////////
FEndpoint FNetworkClient::GetServerEndpoint() const
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
        std::cout << "[CLIENT] Connection timeout - no server activity for "
                  << CONNECTION_TIMEOUT.count() << " seconds" << std::endl;
        std::cout << "[CLIENT] Last activity was at: "
                  << std::chrono::duration_cast<std::chrono::seconds>(
                         m_connection->lastActivity.time_since_epoch()
                     )
                         .count()
                  << "s" << std::endl;

        // Disconnect due to timeout
        DisconnectInternal(EDisconnectionReason::Timeout, false);
    }
    else
    {
        // Optional: Log approaching timeout
        if (timeSinceLastActivity >= std::chrono::seconds(25))
        {   // Warn at 25s if timeout is 30s
            std::cout << "[CLIENT] Warning: No server activity for "
                      << timeSinceLastActivity.count() << " seconds"
                      << std::endl;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkClient::SendHeartbeat(const TimePoint& now)
{
    if (m_connectionState != EConnectionState::Connected) { return; }

    auto timeSinceLastUpdate =
        std::chrono::duration_cast<std::chrono::seconds>(now - m_lastUpdate);

    if (timeSinceLastUpdate < HEARTBEAT_INTERVAL) { return; }

    std::cout << "[CLIENT] Sending heartbeat to server (ID: " << m_clientID
              << ")" << std::endl;
    Packets::HeartBeat heartbeat;
    heartbeat.id = m_clientID;
    heartbeat.timestamp = GetCurrentTimestamp();

    if (SendPacketToServer(heartbeat)) { m_lastUpdate = now; }
    else { std::cout << "[CLIENT] Failed to send heartbeat" << std::endl; }
}

//////////////////////////////////////////////////////////////////////////////
void FNetworkClient::CheckConnectionAttemptTimeout(const TimePoint& now)
{
    if (m_connectionState != EConnectionState::Connecting) { return; }

    auto timeSinceConnectAttempt =
        std::chrono::duration_cast<std::chrono::seconds>(now - m_lastUpdate);

    // Timeout after 5 seconds if no ConnectResponse received
    if (timeSinceConnectAttempt >= std::chrono::seconds(5))
    {
        std::cout
            << "[CLIENT] Connection timeout - no server response after 5 seconds"
            << std::endl;

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
        std::cout
            << "[CLIENT] Server is shutting down - disconnecting gracefully"
            << std::endl;
    }
    else
    {
        std::cout
            << "[CLIENT] Received disconnect packet from server. Reason: "
            << static_cast<int>(reason) << std::endl;
    }

    DisconnectInternal(reason, false);
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkClient::HandleHeartbeatPacket(
    const Packets::HeartBeat& packet, const FEndpoint& endpoint
)
{
    // Only accept heartbeat packets from our server
    if (endpoint != m_serverEndpoint ||
        m_connectionState != EConnectionState::Connected)
    {
        return;
    }

    if (m_connection) { m_connection->lastActivity = SteadyClock::now(); }
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkClient::Cleanup(void)
{
    std::cout << "[CLIENT] Cleanup called" << std::endl;

    Disconnect(EDisconnectionReason::Shutdown);
    std::cout << "[CLIENT] Cleanup disconnected complete now waiting"
              << std::endl;

    std::cout << "[CLIENT] Cleanup completed" << std::endl;
}

}   // namespace tkd
