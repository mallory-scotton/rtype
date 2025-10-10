///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Static/FNetworkInterface.hpp>
#include <Engine/Static/Engine.hpp>
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
TUniquePtr<__internal::FNetworkSubsystem>
    FNetworkInterface::s_networkSubsystem = nullptr;
std::mutex FNetworkInterface::s_mutex;
bool FNetworkInterface::s_isConnected = false;

///////////////////////////////////////////////////////////////////////////////
bool FNetworkInterface::Connect(const std::string& host, UInt16 port)
{
    std::lock_guard<std::mutex> lock(s_mutex);

    // Return false if already connected
    if (s_isConnected)
    {
        std::cout << "[NETWORK] Already connected to a server" << std::endl;
        return false;
    }

    try
    {
        // Create network subsystem if not exists
        if (!s_networkSubsystem)
        {
            if (!Initialize(host, port))
            {
                std::cerr << "[NETWORK] Failed to initialize network subsystem"
                          << std::endl;
                return false;
            }
            s_networkSubsystem->Start();
        }
        s_isConnected = true;
        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "[NETWORK] Exception during connection: " << e.what()
                  << std::endl;
        if (s_networkSubsystem) { s_networkSubsystem.reset(); }
        s_isConnected = false;
        return false;
    }
}

///////////////////////////////////////////////////////////////////////////////
bool FNetworkInterface::Disconnect(void)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    std::cout << "[NETWORK INTERFACE] Disconnect called" << std::endl;

    if (!s_isConnected)
    {
        std::cout << "[NETWORK] Not connected to any server" << std::endl;
        return false;
    }

    try
    {
        if (s_networkSubsystem)
        {
            s_networkSubsystem->GetClient()->Disconnect(
                EDisconnectionReason::Shutdown
            );
            std::cout
                << "[NETWORK INTERFACE] Waiting for disconnect packet transmission..."
                << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        s_isConnected = false;
        std::cout << "[NETWORK] Successfully disconnected from server"
                  << std::endl;
        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "[NETWORK] Exception during disconnection: " << e.what()
                  << std::endl;
        s_isConnected = false;
        return false;
    }
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD bool FNetworkInterface::IsConnected(void)
{
    std::lock_guard<std::mutex> lock(s_mutex);

    // Check if subsystem exists first
    if (!s_networkSubsystem) { return false; }

    // Get the client
    auto client = s_networkSubsystem->GetClient();
    if (!client) { return false; }

    // Check if connection state is Connected
    return client->GetConnectionState() == EConnectionState::Connected;
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD __internal::FNetworkSubsystem&
    FNetworkInterface::GetNetworkSubsystem(void)
{
    std::lock_guard<std::mutex> lock(s_mutex);

    if (!s_networkSubsystem)
    {
        throw std::runtime_error("Network subsystem is not initialized.");
    }

    return *s_networkSubsystem;
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD FNetworkStatistics FNetworkInterface::GetStatistics(void)
{
    std::lock_guard<std::mutex> lock(s_mutex);

    if (s_networkSubsystem)
    {
        return s_networkSubsystem->GetClient()->GetStatistics();
    }

    // Return default statistics object
    return FNetworkStatistics{};
}

///////////////////////////////////////////////////////////////////////////////
bool FNetworkInterface::SendPacket(const IPacket& packet)
{
    TKD_UNUSED(packet);

    std::lock_guard<std::mutex> lock(s_mutex);

    if (!s_networkSubsystem)
    {
        std::cerr << "[NETWORK] Network subsystem not initialized"
                  << std::endl;
        return false;
    }

    if (!s_isConnected)
    {
        std::cerr << "[NETWORK] Not connected to server" << std::endl;
        return false;
    }
    // own thing
    // shit to do typeshit
    return true;
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkInterface::ClearPacketHandlers(void)
{
    std::lock_guard<std::mutex> lock(s_mutex);

    if (s_networkSubsystem)
    {
        // its own thing
        // s_networkSubsystem->ClearUserPacketHandlers();
    }
}

///////////////////////////////////////////////////////////////////////////////
bool FNetworkInterface::Initialize(const std::string& host, UInt16 port)
{
    __internal::FNetworkSubsystem::Config networkConfig;

    networkConfig.mode = __internal::FNetworkSubsystem::Mode::Client;
    networkConfig.host = host;
    networkConfig.port = port;

    // Initialize network subsystem (client mode)
    s_networkSubsystem =
        std::make_unique<__internal::FNetworkSubsystem>(networkConfig);
    if (!s_networkSubsystem->Initialize())
    {
        std::cerr << "[NETWORK] Failed to initialize network subsystem"
                  << std::endl;
        s_networkSubsystem.reset();
        return false;
    }
    std::cout << "[NETWORK] initialized network subsystem properly"
              << std::endl;
    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool FNetworkInterface::Shutdown(void)
{
    std::cout << "[NETWORK INTERFACE] Shutdown called" << std::endl;

    if (IsConnected())
    {
        std::cout
            << "[NETWORK INTERFACE] Still connected, disconnecting first..."
            << std::endl;
        Disconnect();
    }

    std::lock_guard<std::mutex> lock(s_mutex);

    if (!s_networkSubsystem)
    {
        std::cout << "[NETWORK INTERFACE] Subsystem already cleaned up"
                  << std::endl;
        return false;
    }

    try
    {
        // Stop the network thread and close socket
        std::cout
            << "[NETWORK INTERFACE] Stopping network thread and closing socket..."
            << std::endl;
        s_networkSubsystem->GetClient()->Stop();
        s_networkSubsystem.reset();
        s_isConnected = false;

        std::cout
            << "[NETWORK INTERFACE] Network interface shut down successfully"
            << std::endl;
        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "[NETWORK] Exception during shutdown: " << e.what()
                  << std::endl;
        s_networkSubsystem.reset();
        s_isConnected = false;
        return false;
    }
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD bool FNetworkInterface::IsInitialized(void)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    return s_networkSubsystem != nullptr;
}

}   // namespace tkd
