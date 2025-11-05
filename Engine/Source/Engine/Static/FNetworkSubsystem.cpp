///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Static/FNetworkSubsystem.hpp>
#include <Engine/Core/Utils/FLogger.hpp>
#include <Engine/Runtime/World/UWorld.hpp>

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
            // Create client first
            m_client = std::make_unique<FNetworkClient>();

            // DON'T auto-connect here yet - wait for SetEngineSettings()
        }

        m_initialized.store(true, std::memory_order_release);

        // Set engine settings on client/server if available
        if (m_settings)
        {
            SetEngineSettings(*m_settings);   // Dereference the pointer
        }

        return true;
    }
    catch (...)
    {
        // Log error
        return false;
    }
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkSubsystem::SetEngineSettings(const FEngineSettings& settings)
{
    std::shared_lock lock(m_networkMutex);

    if (!m_server && !m_client)
    {
        m_settings = std::make_unique<FEngineSettings>(settings);
    }
    if (m_server)
    {
        m_server->SetEngineSettings(settings);
        FLogger::SetNamespace("Network");
        FLogger::Info(
            "[SERVER] Engine settings updated - Title: '{}', Version: '{}'",
            settings.game.title,
            settings.game.version
        );
    }

    if (m_client)
    {
        m_client->SetEngineSettings(settings);
        FLogger::SetNamespace("Network");
        FLogger::Info(
            "[CLIENT] Engine settings updated - Title: '{}', Version: '{}'",
            settings.game.title,
            settings.game.version
        );

        // NOW auto-connect if configured, after settings are set
        if (m_config.autoConnect)
        {
            FLogger::Info(
                "Auto-connecting to {}:{}", m_config.host.CStr(), m_config.port
            );
            if (!m_client->Connect(m_config.host, m_config.port))
            {
                FLogger::Warn("Auto-connect failed, manual connection required"
                );
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
const FEngineSettings* FNetworkSubsystem::GetEngineSettings(void
) const noexcept
{
    return m_settings.get();
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
void FNetworkSubsystem::ThreadSetup(void)
{
    // No special setup needed for now
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkSubsystem::ThreadTeardown(void)
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

///////////////////////////////////////////////////////////////////////////////
void FNetworkSubsystem::ThreadLoop(void)
{
    static TimePoint lastStatsUpdate = SteadyClock::now();
    static TimePoint lastUpdateTime = SteadyClock::now();
    static UInt64 totalBytesSent = 0;
    static UInt64 totalBytesReceived = 0;

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
        if (m_server)
        {
            // CRITICAL FIX: Don't call Stop() here - let the server cleanup
            // handle the shutdown gracefully by flushing packets first
            FLogger::SetNamespace("Network");
            FLogger::Info("Cleaning up server...");
            m_server->Cleanup();   // Cleanup flushes packets before stopping
            m_server->Stop();      // Now stop the network thread
            m_server.reset();
        }

        if (m_client)
        {
            // Disconnect will stop the client properly
            m_client->Disconnect();
            m_client.reset();
        }
    }

#ifndef TKD_SYSTEM_WINDOWS
    // Network tick rate: ~30Hz is usually sufficient
    WaitFor(Milliseconds(33));
#endif
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

///////////////////////////////////////////////////////////////////////////////
void FNetworkSubsystem::ProcessDeferredRPCs(UWorld& world)
{
    // NO LOCK NEEDED HERE!
    // The server/client pointers are stable after initialization
    // and ProcessDeferredRPCs has its own internal locking (m_rpcQueueMutex)
    // Taking m_networkMutex here causes deadlock with the network thread

    if (m_config.mode == Mode::Server && m_server)
    {
        m_server->ProcessDeferredRPCs(world);
    }
    else if (m_config.mode == Mode::Client && m_client)
    {
        m_client->ProcessDeferredRPCs(world);
    }
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkSubsystem::ProcessDeferredPropertyReplications(UWorld& world)
{
    // NO LOCK NEEDED HERE!
    // The server/client pointers are stable after initialization
    // and ProcessDeferredPropertyReplications has its own internal locking
    // (m_propertyQueueMutex) Taking m_networkMutex here causes deadlock with
    // the network thread

    if (m_config.mode == Mode::Server && m_server)
    {
        m_server->ProcessDeferredPropertyReplications(world);
    }
    else if (m_config.mode == Mode::Client && m_client)
    {
        m_client->ProcessDeferredPropertyReplications(world);
    }
}

}   // namespace tkd::__internal

/*

in thread loop, uworld cause network::deferal that calls subsytem deferal that
calls the actual server/client deferal

deferal itself swaps the queue of itself and the current rpc queue



*/
