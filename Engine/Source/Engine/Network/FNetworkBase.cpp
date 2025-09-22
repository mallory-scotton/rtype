///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Network/FNetworkBase.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

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
void FNetworkBase::InitializePacketManager(void) {}

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
    FPacketHeader header;
    auto packet = m_packetManager.DeserializePacket(data, size, header);

    if (!packet)
    {
        m_statistics.packetsDropped++;
        return;
    }

    auto it = m_packetHandlers.find(header.packetType);
    if (it != m_packetHandlers.end()) { it->second(*packet, sender); }

    OnPacketReceived(header, sender);
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkBase::OnPacketReceived(
    TKD_MAYBE_UNUSED const FPacketHeader& header,
    TKD_MAYBE_UNUSED const FEndpoint& sender
)
{}

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

}   // namespace tkd
