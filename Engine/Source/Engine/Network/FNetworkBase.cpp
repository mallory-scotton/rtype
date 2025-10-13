///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Network/FNetworkBase.hpp>
#include <Engine/Core/Utils.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

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
