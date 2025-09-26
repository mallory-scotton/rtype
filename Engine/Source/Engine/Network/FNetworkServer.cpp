///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Network/FNetworkServer.hpp>

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
}

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

        // Remove connection
        m_connections.erase(it->second);
        m_clientIDToEndpoint.erase(it);
    }
}

///////////////////////////////////////////////////////////////////////////////
bool FNetworkServer::Start(void)
{
    if (m_running.load()) { return true; }

    try
    {
        // Create and bind socket
        m_socket = std::make_unique<FSocket>(m_ioContext);
        m_socket->open(asio::ip::udp::v4());
        m_socket->bind(FEndpoint(asio::ip::udp::v4(), m_port));

        m_running = true;

        // Start network thread
        m_networkThread =
            std::make_unique<FThread>([this]() { RunNetworkThread(); });

        // Start receiving
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
void FNetworkServer::Update(TKD_MAYBE_UNUSED float deltaTime)
{
    auto now = SteadyClock::now();

    CheckConnectionTimeouts(now);
    SendHeartbeats(now);

    m_lastUpdate = now;
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkServer::BroadcastPacket(const IPacket& packet)
{
    std::lock_guard<std::mutex> lock(m_connectionsMutex);
    for (const auto& [endpoint, connection]: m_connections)
    {
        if (connection->connected) { SendPacket(packet, endpoint); }
    }
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
            EmitEvent(Events::ClientDisconnected{
              clientID, EDisconnectionReason::Timeout });

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

    // Create new connection
    auto connection = std::make_unique<FConnectionInformation>();
    connection->endpoint = endpoint;
    connection->clientID = m_nextClientID++;
    connection->lastActivity = SteadyClock::now();
    connection->connected = true;

    uint32_t assignedClientId = connection->clientID;

    // Store connection
    m_connections[endpoint] = std::move(connection);
    m_clientIDToEndpoint[assignedClientId] = endpoint;

    // Send response
    Packets::ConnectResponse response;
    response.clientID = assignedClientId;
    response.accepted = true;

    SendPacket(response, endpoint);
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

}   // namespace tkd
