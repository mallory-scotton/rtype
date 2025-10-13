///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Static/FNetworkSubsystem.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::__internal
///////////////////////////////////////////////////////////////////////////////
namespace tkd::__internal
{

///////////////////////////////////////////////////////////////////////////////
FNetworkSubsystem::FNetworkSubsystem(Config config)
    : m_config(std::move(config))
{}

///////////////////////////////////////////////////////////////////////////////
bool FNetworkSubsystem::Initialize(void)
{
    if (m_initialized.load(std::memory_order_acquire)) { return false; }

    if (m_config.mode == Mode::Disabled)
    {
        m_initialized.store(true, std::memory_order_release);
        return true;
    }

    try
    {
        if (m_config.mode == Mode::Server)
        {
            m_server = std::make_unique<FNetworkServer>(m_config.port);
            if (!m_server->Start()) { return false; }
        }
        else if (m_config.mode == Mode::Client)
        {
            m_client = std::make_unique<FNetworkClient>();
            if (!m_client->Connect(m_config.host, m_config.port))
            {
                return false;
            }
        }

        m_initialized.store(true, std::memory_order_release);
        return true;
    }
    catch (const std::exception& e)
    {
        // Log error
        return false;
    }
}

///////////////////////////////////////////////////////////////////////////////
FNetworkServer* FNetworkSubsystem::GetServer(void) noexcept
{
    std::shared_lock lock(m_networkMutex);
    return m_server.get();
}

///////////////////////////////////////////////////////////////////////////////
FNetworkClient* FNetworkSubsystem::GetClient(void) noexcept
{
    std::shared_lock lock(m_networkMutex);
    return m_client.get();
}

///////////////////////////////////////////////////////////////////////////////
FNetworkSubsystem::Mode FNetworkSubsystem::GetMode(void) const noexcept
{
    return m_config.mode;
}

///////////////////////////////////////////////////////////////////////////////
SizeT FNetworkSubsystem::GetConnectedClients(void) const noexcept
{
    return m_connectedClients.load(std::memory_order_acquire);
}

///////////////////////////////////////////////////////////////////////////////
UInt64 FNetworkSubsystem::GetBytesSentPerSecond(void) const noexcept
{
    return m_bytesSentPerSecond.load(std::memory_order_acquire);
}

///////////////////////////////////////////////////////////////////////////////
UInt64 FNetworkSubsystem::GetBytesReceivedPerSecond(void) const noexcept
{
    return m_bytesReceivedPerSecond.load(std::memory_order_acquire);
}

///////////////////////////////////////////////////////////////////////////////
FNetworkStatistics FNetworkSubsystem::GetStatistics(void) const noexcept
{
    std::shared_lock lock(m_networkMutex);

    if (m_server) { return m_server->GetStatistics(); }
    else if (m_client) { return m_client->GetStatistics(); }

    return FNetworkStatistics{};
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkSubsystem::ThreadLoop(void)
{
    TimePoint lastStatsUpdate = SteadyClock::now();
    TimePoint lastUpdateTime = SteadyClock::now();
    UInt64 totalBytesSent = 0;
    UInt64 totalBytesReceived = 0;

    while (m_running.load(std::memory_order_acquire))
    {
        TimePoint now = SteadyClock::now();

        float deltaTime =
            std::chrono::duration<float>(now - lastUpdateTime).count();
        lastUpdateTime = now;   // Update for next frame

        if (m_config.mode == Mode::Server && m_server)
        {
            std::unique_lock lock(m_networkMutex);

            // Process server updates
            m_server->Update(deltaTime);
        }
        else if (m_config.mode == Mode::Client && m_client)
        {
            std::unique_lock lock(m_networkMutex);

            // Process client updates
            m_client->Update(deltaTime);
        }

        // Update statistics every second
        float elapsed = TDuration<float>(now - lastStatsUpdate).count();
        if (elapsed >= 1.0f)
        {
            m_bytesSentPerSecond.store(
                static_cast<UInt64>(totalBytesSent / elapsed),
                std::memory_order_release
            );
            m_bytesReceivedPerSecond.store(
                static_cast<UInt64>(totalBytesReceived / elapsed),
                std::memory_order_release
            );

            totalBytesSent = 0;
            totalBytesReceived = 0;
            lastStatsUpdate = now;
        }

        // Network tick rate: ~30Hz is usually sufficient
        WaitFor(Milliseconds(33));
    }

    // Cleanup
    {
        std::unique_lock lock(m_networkMutex);

        if (m_server)
        {
            m_server->Stop();
            m_server.reset();
        }

        if (m_client)
        {
            m_client->Disconnect();
            m_client.reset();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
Bool FNetworkSubsystem::SendData(const std::vector<Byte>& data)
{
    std::shared_lock lock(m_networkMutex);

    if (m_config.mode == Mode::Server && m_server)
    {
        return m_server->BroadcastData(data);
    }
    else if (m_config.mode == Mode::Client && m_client &&
             m_client->IsConnected())
    {
        return m_client->SendData(data, m_client->GetServerEndpoint());
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////
Bool FNetworkSubsystem::SendData(
    const std::vector<Byte>& data, const FEndpoint& endpoint
)
{
    std::shared_lock lock(m_networkMutex);

    if (m_config.mode == Mode::Server && m_server)
    {
        return m_server->SendData(data, endpoint);
    }
    else if (m_config.mode == Mode::Client && m_client &&
             m_client->IsConnected())
    {
        return m_client->SendData(data, endpoint);
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////
Bool FNetworkSubsystem::SendData(
    const std::vector<Byte>& data, const std::vector<FEndpoint>& endpoints
)
{
    std::shared_lock lock(m_networkMutex);

    if (m_config.mode == Mode::Server && m_server)
    {
        bool allSent = true;
        for (const auto& endpoint: endpoints)
        {
            if (!m_server->SendData(data, endpoint)) { allSent = false; }
        }
        return allSent;
    }
    else if (m_config.mode == Mode::Client && m_client &&
             m_client->IsConnected())
    {
        bool allSent = true;
        for (const auto& endpoint: endpoints)
        {
            if (!m_client->SendData(data, endpoint)) { allSent = false; }
        }
        return allSent;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////
Bool FNetworkSubsystem::SendPacket(const IPacket& packet)
{
    std::shared_lock lock(m_networkMutex);

    if (m_config.mode == Mode::Server && m_server)
    {
        return m_server->BroadcastPacket(packet);
    }
    else if (m_config.mode == Mode::Client && m_client &&
             m_client->IsConnected())
    {
        return m_client->SendPacket(packet, m_client->GetServerEndpoint());
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////
Bool FNetworkSubsystem::SendPacket(
    const IPacket& packet, const FEndpoint& endpoint
)
{
    std::shared_lock lock(m_networkMutex);

    if (m_config.mode == Mode::Server && m_server)
    {
        return m_server->SendPacket(packet, endpoint);
    }
    else if (m_config.mode == Mode::Client && m_client &&
             m_client->IsConnected())
    {
        return m_client->SendPacket(packet, endpoint);
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////
Bool FNetworkSubsystem::SendPacket(
    const IPacket& packet, const std::vector<FEndpoint>& endpoints
)
{
    std::shared_lock lock(m_networkMutex);

    if (m_config.mode == Mode::Server && m_server)
    {
        bool allSent = true;
        for (const auto& endpoint: endpoints)
        {
            if (!m_server->SendPacket(packet, endpoint)) { allSent = false; }
        }
        return allSent;
    }
    else if (m_config.mode == Mode::Client && m_client &&
             m_client->IsConnected())
    {
        bool allSent = true;
        for (const auto& endpoint: endpoints)
        {
            if (!m_client->SendPacket(packet, endpoint)) { allSent = false; }
        }
        return allSent;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////
Bool FNetworkSubsystem::SendReliablePacket(const IPacket& packet)
{
    std::shared_lock lock(m_networkMutex);

    if (m_config.mode == Mode::Server && m_server)
    {
        return m_server->BroadcastReliablePacket(packet);
    }
    else if (m_config.mode == Mode::Client && m_client &&
             m_client->IsConnected())
    {
        return m_client->SendReliablePacket(
            packet, m_client->GetServerEndpoint()
        );
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////
Bool FNetworkSubsystem::SendReliablePacket(
    const IPacket& packet, const FEndpoint& endpoint
)
{
    std::shared_lock lock(m_networkMutex);

    if (m_config.mode == Mode::Server && m_server)
    {
        return m_server->SendReliablePacket(packet, endpoint);
    }
    else if (m_config.mode == Mode::Client && m_client &&
             m_client->IsConnected())
    {
        return m_client->SendReliablePacket(packet, endpoint);
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////
Bool FNetworkSubsystem::SendReliablePacket(
    const IPacket& packet, const std::vector<FEndpoint>& endpoints
)
{
    std::shared_lock lock(m_networkMutex);

    if (m_config.mode == Mode::Server && m_server)
    {
        bool allSent = true;
        for (const auto& endpoint: endpoints)
        {
            if (!m_server->SendReliablePacket(packet, endpoint))
            {
                allSent = false;
            }
        }
        return allSent;
    }
    else if (m_config.mode == Mode::Client && m_client &&
             m_client->IsConnected())
    {
        bool allSent = true;
        for (const auto& endpoint: endpoints)
        {
            if (!m_client->SendReliablePacket(packet, endpoint))
            {
                allSent = false;
            }
        }
        return allSent;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////
Bool FNetworkSubsystem::BroadcastData(const std::vector<Byte>& data)
{
    std::shared_lock lock(m_networkMutex);

    if (m_config.mode == Mode::Server && m_server)
    {
        return m_server->BroadcastData(data);
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////
Bool FNetworkSubsystem::BroadcastPacket(const IPacket& packet)
{
    std::shared_lock lock(m_networkMutex);

    if (m_config.mode == Mode::Server && m_server)
    {
        return m_server->BroadcastPacket(packet);
    }

    return false;
}

}   // namespace tkd::__internal
