///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Network/FNetworkBase.hpp>
#include <Engine/Core/Utils.hpp>
#include <Engine/Runtime/World/UWorld.hpp>
#include <Engine/Static/FWorldInterface.hpp>
#include <sstream>
#if TKD_ENGINE_CLIENT
    #include <Engine/Debug/FNetworkDebug.hpp>
#endif

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
#if TKD_ENGINE_CLIENT
debug::FNetworkDebug* FNetworkBase::s_networkDebug = nullptr;
#endif

///////////////////////////////////////////////////////////////////////////////
FNetworkBase::FNetworkBase(void) { RegisterBasePacketHandlers(); }

///////////////////////////////////////////////////////////////////////////////
FNetworkBase::~FNetworkBase(void) { Stop(); }

///////////////////////////////////////////////////////////////////////////////
void FNetworkBase::Stop(void)
{
    if (m_running.exchange(false))
    {
        m_ioContext.stop();
        if (m_networkThread && m_networkThread->Joinable())
        {
            m_networkThread->Join();
        }
        if (m_socket) { m_socket->close(); }
    }
}

///////////////////////////////////////////////////////////////////////////////
bool FNetworkBase::IsRunning(void) const { return m_running.load(); }

///////////////////////////////////////////////////////////////////////////////
const FNetworkStatistics& FNetworkBase::GetStatistics(void) const
{
    return m_statistics;
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkBase::StartReceive(void)
{
    if (!m_socket || !m_running) { return; }

    m_socket->async_receive_from(
        asio::buffer(m_receiveBuffer),
        m_senderEndpoint,
        [this](const asio::error_code& error, SizeT bytesReceived)
        { HandleReceive(error, bytesReceived); }
    );
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkBase::InitializePacketManager(void)
{
    m_packetManager.RegisterPacket<Packets::Connect>();
    m_packetManager.RegisterPacket<Packets::ConnectResponse>();
    m_packetManager.RegisterPacket<Packets::Disconnect>();
    m_packetManager.RegisterPacket<Packets::HeartBeat>();
    m_packetManager.RegisterPacket<Packets::Replication>();
    m_packetManager.RegisterPacket<Packets::RemoteProcedureCall>();
    m_packetManager.RegisterPacket<Packets::Acknowledgment>();
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkBase::RegisterBasePacketHandlers(void)
{
    // Register handler for acknowledgment packets
    RegisterPacketHandler<Packets::Acknowledgment>(
        [this](
            const Packets::Acknowledgment& packet, const FEndpoint& endpoint
        ) { HandleAcknowledgmentPacket(packet, endpoint); }
    );

    // Register handler for RPC packets
    RegisterPacketHandler<Packets::RemoteProcedureCall>(
        [this](
            const Packets::RemoteProcedureCall& packet,
            const FEndpoint& endpoint
        ) { HandleRPCPacket(packet, endpoint); }
    );
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkBase::
    HandleAcknowledgmentPacket(const Packets::Acknowledgment& packet, const FEndpoint&)
{
    // Remove the acknowledged sequence number from pending ACKs
    m_pendingAcks.erase(
        std::remove_if(
            m_pendingAcks.begin(),
            m_pendingAcks.end(),
            [&packet](const FAcknowledgment& ack)
            { return ack.header.sequenceNumber == packet.ackedSequenceNumber; }
        ),
        m_pendingAcks.end()
    );
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkBase::HandleRPCPacket(
    const Packets::RemoteProcedureCall& packet, const FEndpoint& endpoint
)
{
    // Queue the RPC for deferred execution to avoid deadlock
    // The network thread queues RPCs here without blocking on m_worldMutex
    // The world thread will process them via ProcessDeferredRPCs()
    std::lock_guard<std::mutex> lock(m_rpcQueueMutex);
    m_deferredRPCs.push({ packet, endpoint });
}

///////////////////////////////////////////////////////////////////////////////
bool FNetworkBase::SendData(
    const std::vector<Byte>& data, const FEndpoint& endpoint
)
{
    if (!m_socket || !m_running || data.empty()) { return false; }

    try
    {
        SizeT bytesSent = m_socket->send_to(asio::buffer(data), endpoint);
        m_statistics.packetsSent++;
        m_statistics.bytesOutgoing += bytesSent;
        return bytesSent == data.size();
    }
    catch (const std::exception&)
    {
        m_statistics.packetsDropped++;
        return false;
    }
}

///////////////////////////////////////////////////////////////////////////////
bool FNetworkBase::SendPacket(const IPacket& packet, const FEndpoint& endpoint)
{
    if (!m_socket || !m_running) { return false; }

    auto data = m_packetManager.SerializePacket(packet);
    if (data.empty()) { return false; }

#if TKD_ENGINE_CLIENT
    // Log packet for debugging
    if (s_networkDebug)
    {
        auto header =
            m_packetManager.DeserializeHeader(data.data(), data.size());
        if (header)
        {
            std::ostringstream oss;
            oss << endpoint.address().to_string() << ":" << endpoint.port();
            const Byte* payload = data.data() + FPacketHeader::SIZE;
            SizeT payloadSize = data.size() - FPacketHeader::SIZE;
            s_networkDebug->LogPacket(
                *header, oss.str(), true, payload, payloadSize
            );
        }
    }
#endif

    try
    {
        SizeT bytesSent = m_socket->send_to(asio::buffer(data), endpoint);
        m_statistics.packetsSent++;
        m_statistics.bytesOutgoing += bytesSent;
        return bytesSent == data.size();
    }
    catch (const std::exception&)
    {
        m_statistics.packetsDropped++;
        return false;
    }
}

///////////////////////////////////////////////////////////////////////////////
bool FNetworkBase::SendReliablePacket(
    const IPacket& packet, const FEndpoint& endpoint
)
{
    // Check if socket is valid and network is running
    if (!m_socket || !m_running) { return false; }

    // Serialize packet with Reliable flag
    auto data =
        m_packetManager.SerializePacket(packet, EPacketFlags::Reliable);
    if (data.empty()) { return false; }

    // Deserialize header to get sequence number
    auto header = m_packetManager.DeserializeHeader(data.data(), data.size());
    if (!header) { return false; }

    // Store the sequence number for ACK tracking
    FAcknowledgment ack = { .header = *header,
                            .data = data,
                            .endpoint = endpoint };
    m_pendingAcks.push_back(ack);

#if TKD_ENGINE_CLIENT
    // Log packet for debugging
    if (s_networkDebug)
    {
        std::ostringstream oss;
        oss << endpoint.address().to_string() << ":" << endpoint.port();
        const Byte* payload = data.data() + FPacketHeader::SIZE;
        SizeT payloadSize = data.size() - FPacketHeader::SIZE;
        s_networkDebug->LogPacket(
            *header, oss.str(), true, payload, payloadSize
        );
    }
#endif

    // Send the packet
    try
    {
        SizeT bytesSent = m_socket->send_to(asio::buffer(data), endpoint);
        m_statistics.packetsSent++;
        m_statistics.bytesOutgoing += bytesSent;
        return bytesSent == data.size();
    }
    catch (const std::exception&)
    {
        // On failure, remove the sequence number from pending ACKs
        m_statistics.packetsDropped++;
        // Remove sequence number from pending ACKs
        m_pendingAcks.erase(
            std::remove_if(
                m_pendingAcks.begin(),
                m_pendingAcks.end(),
                [&header](const FAcknowledgment& ack)
                { return ack.header.sequenceNumber == header->sequenceNumber; }
            ),
            m_pendingAcks.end()
        );
        return false;
    }
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkBase::HandleReceive(
    const asio::error_code& error, SizeT bytesReceived
)
{
    if (!m_running) { return; }

    if (!error && bytesReceived > 0)
    {
        m_statistics.packetsReceived++;
        m_statistics.bytesIncoming += bytesReceived;

        ProcessReceivedData(
            m_receiveBuffer.data(), bytesReceived, m_senderEndpoint
        );
    }
    else if (error && error != asio::error::operation_aborted)
    {
        m_statistics.packetsDropped++;
    }

    StartReceive();
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkBase::ProcessReceivedData(
    const UInt8* data, SizeT size, const FEndpoint& sender
)
{
    // Deserialize packet header
    FPacketHeader header;
    auto packet = m_packetManager.DeserializePacket(data, size, header);

    // Failed to deserialize packet
    if (!packet)
    {
        m_statistics.packetsDropped++;
        return;
    }

#if TKD_ENGINE_CLIENT
    // Log packet for debugging
    if (s_networkDebug)
    {
        std::ostringstream oss;
        oss << sender.address().to_string() << ":" << sender.port();
        const Byte* payload = data + FPacketHeader::SIZE;
        SizeT payloadSize = size - FPacketHeader::SIZE;
        s_networkDebug->LogPacket(
            header, oss.str(), false, payload, payloadSize
        );
    }
#endif

    // Dispatch to registered handler
    auto it = m_packetHandlers.find(header.packetType);
    if (it != m_packetHandlers.end()) { it->second(*packet, sender); }
    else
    {
        FLogger::SetNamespace("Network");
        FLogger::Warn(
            "No handler registered for packet type {}", header.packetType
        );
    }

    // Handle reliable packet acknowledgment
    if (header.flags == static_cast<UInt16>(EPacketFlags::Reliable))
    {
        // Send acknowledgment for reliable packets
        Packets::Acknowledgment ackPacket;
        ackPacket.ackedSequenceNumber = header.sequenceNumber;
        SendPacket(ackPacket, sender);
    }

    // Call the virtual method for further processing
    OnPacketReceived(header, sender);
}

#if TKD_ENGINE_CLIENT
///////////////////////////////////////////////////////////////////////////////
void FNetworkBase::SetNetworkDebug(debug::FNetworkDebug* debugInstance)
{
    s_networkDebug = debugInstance;
}
#endif

///////////////////////////////////////////////////////////////////////////////
void FNetworkBase::OnPacketReceived(const FPacketHeader&, const FEndpoint&) {}

///////////////////////////////////////////////////////////////////////////////
void FNetworkBase::RunNetworkThread(void)
{
    while (m_running)
    {
        try
        {
            m_ioContext.run();
            break;
        }
        catch (const std::exception&)
        {
            if (!m_running) { break; }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
UInt32 FNetworkBase::GetCurrentTimestamp(void) const
{
    auto now = SteadyClock::now();
    auto epoch = now.time_since_epoch();
    return static_cast<UInt32>(
        std::chrono::duration_cast<std::chrono::milliseconds>(epoch).count()
    );
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkBase::FlushPackets(void)
{
    if (m_socket && m_ioContext.stopped()) { m_ioContext.restart(); }

    // Process any pending send operations
    m_ioContext.poll();

    // Give network stack time to send
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkBase::ProcessDeferredRPCs(UWorld& world)
{
    // Process all queued RPCs
    // IMPORTANT: This is called from UWorld::Tick(), which already holds
    // m_worldMutex The world is passed as a parameter to avoid re-locking
    std::queue<FDeferredRPC> localQueue;

    {
        // Quickly swap the queue to minimize lock contention
        std::lock_guard<std::mutex> lock(m_rpcQueueMutex);
        std::swap(localQueue, m_deferredRPCs);
    }

    // Process RPCs outside the lock
    while (!localQueue.empty())
    {
        const auto& deferredRPC = localQueue.front();
        const auto& packet = deferredRPC.packet;

        // Execute the RPC directly on the world (already locked by caller)
        auto actors = world.GetActors();
        for (auto& actor: actors)
        {
            if (actor && actor->GetClass()->GetName() == "BP_Player")
            {
                auto rpc = actor->GetFunction(packet.functionName);
                if (rpc) { rpc->Execute(packet.parameters); }
            }
        }

        localQueue.pop();
    }
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkBase::Update(Float32)
{
    UInt32 currentTime = GetCurrentTimestamp();

    // Check for pending ACKs to resend
    for (auto& ack: m_pendingAcks)
    {
        static const UInt32 TIMEOUT = static_cast<UInt32>(ACK_TIMEOUT * 1000);

        if (currentTime - ack.header.timestamp >= TIMEOUT)
        {
            // Resend the packet
            SendData(ack.data, ack.endpoint);
            // Update timestamp
            ack.header.timestamp = currentTime;
        }
    }
}

}   // namespace tkd
