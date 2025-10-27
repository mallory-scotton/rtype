///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <atomic>
#include <Engine/Config.hpp>
#include <Engine/Core/Concurrency/FThread.hpp>
#include <Engine/Network/Asio.hpp>
#include <Engine/Network/FNetworkStatistics.hpp>
#include <Engine/Network/FPacketHeader.hpp>
#include <Engine/Network/FPacketManager.hpp>
#include <Engine/Network/IPacket.hpp>
#include <Engine/Network/Packets.hpp>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <unordered_map>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

#if TKD_ENGINE_CLIENT

///////////////////////////////////////////////////////////////////////////////
// Namespace debug
///////////////////////////////////////////////////////////////////////////////
namespace debug
{

///////////////////////////////////////////////////////////////////////////////
// Forward declaration
///////////////////////////////////////////////////////////////////////////////
class FNetworkDebug;

}   // namespace debug

#endif

///////////////////////////////////////////////////////////////////////////////
// Pre-declarations
///////////////////////////////////////////////////////////////////////////////
class UWorld;

///////////////////////////////////////////////////////////////////////////////
/// \brief Disconnection reason codes
///
///////////////////////////////////////////////////////////////////////////////
enum class EDisconnectionReason : UInt32
{
    Unknown = 0,           //<! No specific reason
    Timeout = 1,           //<! Disconnected due to timeout
    Kicked = 2,            //<! Disconnected by server (kicked)
    Shutdown = 3,          //<! Disconnected due to server shutdown
    Error = 4,             //<! Disconnected due to an error
    ClientRequested = 5,   //<! Disconnected at client's request
};

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
class FNetworkBase
{
public:
    ///////////////////////////////////////////////////////////////////////////
    // Class Constants
    ///////////////////////////////////////////////////////////////////////////
    static constexpr SizeT MAX_PACKET_SIZE = 1472;   //<! Max UDP packet size
    static constexpr Float32 ACK_TIMEOUT = 0.5f;     //<! 500 ms

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Acknowledgment packet structure
    ///
    ///////////////////////////////////////////////////////////////////////////
    struct FAcknowledgment
    {
        FPacketHeader header;      //<! Packet header
        std::vector<UInt8> data;   //<! Raw packet data
        FEndpoint endpoint;        //<! Sender endpoint
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Deferred RPC structure for thread-safe execution
    ///
    ///////////////////////////////////////////////////////////////////////////
    struct FDeferredRPC
    {
        Packets::RemoteProcedureCall packet;   //<! RPC packet data
        FEndpoint endpoint;                    //<! Sender endpoint
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Queued packet structure for thread-safe sending
    ///
    ///////////////////////////////////////////////////////////////////////////
    struct FQueuedPacket
    {
        std::vector<UInt8> data;   //<! Serialized packet data
        FEndpoint endpoint;        //<! Destination endpoint
        bool reliable;             //<! Whether this is a reliable packet
        FPacketHeader header;      //<! Packet header (for reliable packets)
    };

protected:
    ///////////////////////////////////////////////////////////////////////////
    // Class Member
    ///////////////////////////////////////////////////////////////////////////
    asio::io_context m_ioContext;               //<! ASIO IO context
    std::unique_ptr<FSocket> m_socket;          //<! ASIO UDP socket
    std::unique_ptr<FThread> m_networkThread;   //<! Network thread
    std::atomic<bool> m_running;       //<! true if the network is running
    FNetworkStatistics m_statistics;   //<! Network statistics
    FPacketManager m_packetManager;    //<! Packet manager
    std::array<UInt8, MAX_PACKET_SIZE>
        m_receiveBuffer;               //<! Buffer for receiving data
    tkd::FEndpoint m_senderEndpoint;   //<! Endpoint of the sender
    std::unordered_map<
        UInt16,
        std::function<void(const IPacket&, const FEndpoint&)>>
        m_packetHandlers;                         //<! Map of packet handlers
    std::vector<FAcknowledgment> m_pendingAcks;   //<! List of pending ACKs
    std::queue<FDeferredRPC> m_deferredRPCs;      //<! Queue of deferred RPCs
    std::mutex m_rpcQueueMutex;                   //<! Mutex for RPC queue
    std::queue<FQueuedPacket> m_sendQueue;        //<! Queue of packets to send
    std::mutex m_sendQueueMutex;                  //<! Mutex for send queue

private:
    ///////////////////////////////////////////////////////////////////////////
    // Static class member
    ///////////////////////////////////////////////////////////////////////////
#if TKD_ENGINE_CLIENT
    static debug::FNetworkDebug* s_networkDebug;   //<! Network debug instance
#endif

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor
    ///
    ///////////////////////////////////////////////////////////////////////////
    FNetworkBase(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Virtual destructor
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual ~FNetworkBase();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Delete copy constructor
    ///
    ///////////////////////////////////////////////////////////////////////////
    FNetworkBase(const FNetworkBase&) = delete;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Delete move constructor
    ///
    ///////////////////////////////////////////////////////////////////////////
    FNetworkBase(FNetworkBase&&) = delete;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Delete copy assignment operator
    ///
    ///////////////////////////////////////////////////////////////////////////
    FNetworkBase& operator=(const FNetworkBase&) = delete;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Delete move assignment operator
    ///
    ///////////////////////////////////////////////////////////////////////////
    FNetworkBase& operator=(FNetworkBase&&) = delete;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Start the network service, binding the socket and starting
    /// the network thread
    ///
    /// \return true if the network started successfully
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual bool Start(void) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Stop the network service, stopping the network thread and
    /// closing the socket
    ///
    /// \return true if the network stopped successfully
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void Stop(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Update the network service, processing incoming packets and
    ///
    /// \param deltaTime Time since last update in seconds
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void Update(Float32 deltaTime);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Check if the network service is running
    ///
    /// \return true if the network service is running
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool IsRunning(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the network statistics
    ///
    /// \return const reference to the network statistics
    ///
    ///////////////////////////////////////////////////////////////////////////
    const FNetworkStatistics& GetStatistics(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Process deferred RPCs from the queue
    ///
    /// This should be called from the world thread to safely execute RPCs
    /// without causing deadlocks. It processes all queued RPCs.
    ///
    /// \param world Reference to the world (already locked by caller)
    ///
    ///////////////////////////////////////////////////////////////////////////
    void ProcessDeferredRPCs(UWorld& world);

    /// \brief Register a packet handler for a specific packet type
    ///
    /// \param handler Function to call when a packet of the specified type is
    /// received
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    void RegisterPacketHandler(
        std::function<void(const T&, const FEndpoint&)> handler
    )
    {
        static_assert(
            std::is_base_of<IPacket, T>::value, "T must derive from IPacket"
        );

        m_packetHandlers[T::GetStaticType()] =
            [handler = std::move(handler
             )](const IPacket& packet, const FEndpoint& endpoint)
        {
            const T* typedPacket = dynamic_cast<const T*>(&packet);
            if (typedPacket) { handler(*typedPacket, endpoint); }
        };
    }

public:
#if TKD_ENGINE_CLIENT
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the network debug instance for logging
    ///
    /// \param debugInstance Pointer to network debug instance
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void SetNetworkDebug(debug::FNetworkDebug* debugInstance);
#endif

protected:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Initialize the packet manager with known packet types
    ///
    ///////////////////////////////////////////////////////////////////////////
    void InitializePacketManager(void);

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Send raw data to a specific endpoint
    ///
    /// \param data Vector of raw packet data
    /// \param endpoint The endpoint of the sender
    ///
    /// \return true if the data was sent successfully
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool SendData(const std::vector<UInt8>& data, const FEndpoint& endpoint);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Send a packet to a specific endpoint
    ///
    /// \param packet The packet to process
    /// \param endpoint The endpoint of the sender
    ///
    /// \return true if the packet was sent successfully
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool SendPacket(const IPacket& packet, const FEndpoint& endpoint);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Send a packet to a specific endpoint
    ///
    /// \param packet The packet to process
    /// \param endpoint The endpoint of the sender
    ///
    /// \return true if the packet was sent successfully
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool SendReliablePacket(const IPacket& packet, const FEndpoint& endpoint);

protected:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Flushes remaining packets to not cause udp issues at shutdown
    ///
    ///////////////////////////////////////////////////////////////////////////
    void FlushPackets(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Process queued packets and send them
    ///
    /// This is called from Update() to safely send all queued packets
    ///
    ///////////////////////////////////////////////////////////////////////////
    void ProcessSendQueue(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Start receiving data asynchronously
    ///
    ///////////////////////////////////////////////////////////////////////////
    void StartReceive(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Handle received data
    ///
    /// \param error Error code from ASIO
    /// \param bytesReceived Number of bytes received
    ///
    ///////////////////////////////////////////////////////////////////////////
    void HandleReceive(const asio::error_code& error, SizeT bytesReceived);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Process received data, deserialize the packet and call the
    /// appropriate handler
    ///
    /// \param data Pointer to the received data
    /// \param size Size of the received data
    /// \param sender Endpoint of the sender
    ///
    ///////////////////////////////////////////////////////////////////////////
    void ProcessReceivedData(
        const UInt8* data, SizeT size, const FEndpoint& sender
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Called when a packet is received
    ///
    /// \param header Header of the received packet
    /// \param sender Endpoint of the sender
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void
        OnPacketReceived(const FPacketHeader& header, const FEndpoint& sender);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Network thread function
    ///
    ///////////////////////////////////////////////////////////////////////////
    void RunNetworkThread(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the current timestamp in milliseconds since epoch
    ///
    /// \return current timestamp in milliseconds
    ///
    ///////////////////////////////////////////////////////////////////////////
    UInt32 GetCurrentTimestamp(void) const;

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Register base packet handlers
    ///
    ///////////////////////////////////////////////////////////////////////////
    void RegisterBasePacketHandlers(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Handle an acknowledgment packet
    ///
    /// \param packet Acknowledgment packet
    /// \param endpoint Endpoint of the sender
    ///
    ///////////////////////////////////////////////////////////////////////////
    void HandleAcknowledgmentPacket(
        const Packets::Acknowledgment& packet, const FEndpoint& endpoint
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Handle a remote procedure call (RPC) packet
    ///
    /// \param packet RPC packet
    /// \param endpoint Endpoint of the sender
    ///
    ///////////////////////////////////////////////////////////////////////////
    void HandleRPCPacket(
        const Packets::RemoteProcedureCall& packet, const FEndpoint& endpoint
    );
};

}   // namespace tkd
