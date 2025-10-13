///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Config.hpp>
#include <Engine/Core.hpp>
#include <Engine/Network.hpp>
#include <Engine/Static/FThreadedSubsystem.hpp>
#include <shared_mutex>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::__internal
///////////////////////////////////////////////////////////////////////////////
namespace tkd::__internal
{

///////////////////////////////////////////////////////////////////////////////
/// \brief Network subsystem for managing server and client network operations
///
///////////////////////////////////////////////////////////////////////////////
class FNetworkSubsystem final : public FThreadedSubsystem
{
public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Network mode
    ///
    ///////////////////////////////////////////////////////////////////////////
    enum class Mode
    {
        Server,    //<! Server mode
        Client,    //<! Client mode
        Disabled   //<! Disabled mode
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Configuration structure
    ///
    ///////////////////////////////////////////////////////////////////////////
    struct Config
    {
        Mode mode = Mode::Disabled;   //<! Network mode
        UInt16 port = 8080;           //<! Network port
        FString host = "localhost";   //<! Network host
        SizeT maxClients = 32;        //<! Maximum number of clients
    };

private:
    ///////////////////////////////////////////////////////////////////////////
    // Class Member
    ///////////////////////////////////////////////////////////////////////////
    Config m_config;
    TUniquePtr<FNetworkServer> m_server;
    TUniquePtr<FNetworkClient> m_client;
    mutable std::shared_mutex m_networkMutex;
    TAtomic<SizeT> m_connectedClients{ 0 };
    TAtomic<UInt64> m_bytesSentPerSecond{ 0 };
    TAtomic<UInt64> m_bytesReceivedPerSecond{ 0 };

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor from configuration
    ///
    /// \param config The configuration of the network subsystem
    ///
    ///////////////////////////////////////////////////////////////////////////
    FNetworkSubsystem(Config config);

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Initialize the network subsystem
    ///
    /// \return True if the initialization was successful, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool Initialize(void) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the server instance
    ///
    /// \return The server instance, or nullptr if not in server mode
    ///
    ///////////////////////////////////////////////////////////////////////////
    TKD_NODISCARD FNetworkServer* GetServer(void) noexcept;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the client instance
    ///
    /// \return The client instance, or nullptr if not in client mode
    ///
    ///////////////////////////////////////////////////////////////////////////
    TKD_NODISCARD FNetworkClient* GetClient(void) noexcept;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the current network mode
    ///
    /// \return The current network mode
    ///
    ///////////////////////////////////////////////////////////////////////////
    TKD_NODISCARD Mode GetMode(void) const noexcept;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the number of connected clients
    ///
    /// \return The number of connected clients
    ///
    ///////////////////////////////////////////////////////////////////////////
    TKD_NODISCARD SizeT GetConnectedClients(void) const noexcept;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the number of bytes sent per second
    ///
    /// \return The number of bytes sent per second
    ///
    ///////////////////////////////////////////////////////////////////////////
    TKD_NODISCARD UInt64 GetBytesSentPerSecond(void) const noexcept;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the number of bytes received per second
    ///
    /// \return The number of bytes received per second
    ///
    ///////////////////////////////////////////////////////////////////////////
    TKD_NODISCARD UInt64 GetBytesReceivedPerSecond(void) const noexcept;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the network statistics
    ///
    /// \return The network statistics
    ///
    ///////////////////////////////////////////////////////////////////////////
    TKD_NODISCARD FNetworkStatistics GetStatistics(void) const noexcept;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Send raw data or packets over the network
    ///
    /// \param data  The data to send
    ///
    /// \return True if the data was sent successfully, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    Bool SendData(const std::vector<Byte>& data);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Send raw data or packets over the network
    ///
    /// \param data  The data to send
    /// \param endpoint The endpoint to send the data to
    ///
    /// \return True if the data was sent successfully, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    Bool SendData(const std::vector<Byte>& data, const FEndpoint& endpoint);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Send raw data to multiple endpoints over the network
    ///
    /// \param data  The data to send
    /// \param endpoints The list of endpoints to send the data to
    ///
    /// \return True if the data was sent successfully, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    Bool SendData(
        const std::vector<Byte>& data, const std::vector<FEndpoint>& endpoints
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Send a packet over the network
    ///
    /// \param packet The packet to send
    ///
    /// \return True if the packet was sent successfully, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    Bool SendPacket(const IPacket& packet);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Send a packet to a specific endpoint over the network
    ///
    /// \param packet The packet to send
    /// \param endpoint The endpoint to send the packet to
    ///
    /// \return True if the packet was sent successfully, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    Bool SendPacket(const IPacket& packet, const FEndpoint& endpoint);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Send a packet to multiple endpoints over the network
    ///
    /// \param packet The packet to send
    /// \param endpoints The list of endpoints to send the packet to
    ///
    /// \return True if the packet was sent successfully, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    Bool SendPacket(
        const IPacket& packet, const std::vector<FEndpoint>& endpoints
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Send a reliable packet over the network
    ///
    /// \param packet The packet to send reliably
    ///
    /// \return True if the packet was sent successfully, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    Bool SendReliablePacket(const IPacket& packet);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Send a reliable packet to a specific endpoint over the network
    ///
    /// \param packet The packet to send reliably
    /// \param endpoint The endpoint to send the packet to
    ///
    /// \return True if the packet was sent successfully, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    Bool SendReliablePacket(const IPacket& packet, const FEndpoint& endpoint);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Send a reliable packet to multiple endpoints over the network
    ///
    /// \param packet The packet to send reliably
    /// \param endpoints The list of endpoints to send the packet to
    ///
    /// \return True if the packet was sent successfully, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    Bool SendReliablePacket(
        const IPacket& packet, const std::vector<FEndpoint>& endpoints
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Broadcast raw data to all connected clients
    ///
    /// \param data The data to broadcast
    ///
    /// \return True if the data was broadcast successfully, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    Bool BroadcastData(const std::vector<Byte>& data);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Broadcast a packet to all connected clients
    ///
    /// \param packet The packet to broadcast
    ///
    /// \return True if the packet was broadcast successfully, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    Bool BroadcastPacket(const IPacket& packet);

protected:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Thread loop
    ///
    ///////////////////////////////////////////////////////////////////////////
    void ThreadLoop(void) override;
};

}   // namespace tkd::__internal
