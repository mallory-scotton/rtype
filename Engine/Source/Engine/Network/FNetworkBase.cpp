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
        // CRITICAL FIX: Process any remaining queued packets before stopping
        // This ensures disconnect packets are sent before the socket closes
        FLogger::SetNamespace("Network");
        FLogger::Debug("Processing final packet queue before stopping...");
        ProcessSendQueue();

        // Stop the io_context and close socket
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
    m_packetManager.RegisterPacket<Packets::Snapshot>();
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
void FNetworkBase::SetEngineSettings(const FEngineSettings& settings)
{
    m_settings = std::make_unique<FEngineSettings>(settings);
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkBase::
    HandleAcknowledgmentPacket(const Packets::Acknowledgment& packet, const FEndpoint&)
{
    // Remove the acknowledged sequence number from pending ACKs
    std::lock_guard<std::mutex> lock(m_pendingAcksMutex);
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

    FLogger::SetNamespace("Network");
    FLogger::Debug(
        "Queued RPC '{}' from {} for deferred execution on Actor ID {}",
        packet.functionName.CStr(),
        endpoint.address().to_string().c_str(),
        UUID(packet.actorID).ToString().c_str()
    );
}

///////////////////////////////////////////////////////////////////////////////
bool FNetworkBase::SendData(
    const std::vector<Byte>& data, const FEndpoint& endpoint
)
{
    if (!m_socket || !m_running || data.empty()) { return false; }

    // Queue the packet for sending
    FQueuedPacket queuedPacket;
    queuedPacket.data = data;
    queuedPacket.endpoint = endpoint;
    queuedPacket.reliable = false;

    {
        std::lock_guard<std::mutex> lock(m_sendQueueMutex);
        m_sendQueue.push(std::move(queuedPacket));
    }

    return true;
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

    // Queue the packet for sending
    FQueuedPacket queuedPacket;
    queuedPacket.data = std::move(data);
    queuedPacket.endpoint = endpoint;
    queuedPacket.reliable = false;

    {
        std::lock_guard<std::mutex> lock(m_sendQueueMutex);
        m_sendQueue.push(std::move(queuedPacket));
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool FNetworkBase::SendReliablePacket(
    const IPacket& packet, const FEndpoint& endpoint
)
{
    // Check if socket is valid and network is running
    if (!m_socket || !m_running) { return false; }

    // Serialize packet with Reliable flag
    auto data = m_packetManager.SerializePacket(
        packet, static_cast<UInt8>(EPacketFlags::Reliable)
    );
    if (data.empty()) { return false; }

    // Deserialize header to get sequence number
    auto header = m_packetManager.DeserializeHeader(data.data(), data.size());
    if (!header) { return false; }

    // Store the sequence number for ACK tracking
    FAcknowledgment ack = { .header = *header,
                            .data = data,
                            .endpoint = endpoint };
    {
        std::lock_guard<std::mutex> lock(m_pendingAcksMutex);
        m_pendingAcks.push_back(ack);
    }

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

    // Queue the packet for sending
    FQueuedPacket queuedPacket;
    queuedPacket.data = std::move(data);
    queuedPacket.endpoint = endpoint;
    queuedPacket.reliable = true;
    queuedPacket.header = *header;

    {
        std::lock_guard<std::mutex> lock(m_sendQueueMutex);
        m_sendQueue.push(std::move(queuedPacket));
    }

    return true;
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
    if (header.HasFlag(EPacketFlags::Reliable))
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
    // Process any remaining queued packets
    ProcessSendQueue();

    if (m_socket && m_ioContext.stopped()) { m_ioContext.restart(); }

    // Process any pending send operations
    m_ioContext.poll();

    // Give network stack time to send
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkBase::ProcessDeferredRPCs(UWorld& world)
{
    TKD_UNUSED(world);

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
        auto object = UObject::FindByUUID(UUID(packet.actorID));
        if (object)
        {
            auto rpc = object->GetFunction(packet.functionName);
            if (rpc) { rpc->ExecuteSerialized(packet.parameters); }
        }

        localQueue.pop();
    }
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkBase::ProcessDeferredPropertyReplications(UWorld& world)
{
    TKD_UNUSED(world);

    // Process all queued property replications
    // IMPORTANT: This is called from UWorld::Tick(), which already holds
    // m_worldMutex The world is passed as a parameter to avoid re-locking
    std::queue<FDeferredPropertyReplication> localQueue;

    {
        // Quickly swap the queue to minimize lock contention
        std::lock_guard<std::mutex> lock(m_propertyQueueMutex);
        std::swap(localQueue, m_deferredPropertyReplications);
    }

    // Process property replications outside the lock
    while (!localQueue.empty())
    {
        const auto& deferredReplication = localQueue.front();
        const auto& packet = deferredReplication.packet;

        // Convert actorID array to UUID
        UUID actorID(packet.actorID);

        // RACE CONDITION FIX: Copy actors to avoid iterator invalidation
        auto actors = world.GetActors();
        std::shared_ptr<AActor> targetActorPtr = nullptr;

        for (const auto& actor: actors)
        {
            if (!actor) { continue; }

            if (actor->GetUUID() == actorID)
            {
                targetActorPtr = actor;
                break;
            }
        }

        if (targetActorPtr)
        {
            // Get the property
            IProperty* property =
                targetActorPtr->GetProperty(packet.propertyName);

            if (property)
            {
                // Deserialize the property value from byte array
                try
                {
                    // Update the property value with the received binary data
                    property->SetValue(packet.data.data(), packet.data.size());

                    // Clear dirty flag to avoid re-replicating this change
                    property->ClearDirty();
                }
                catch (const std::exception&)
                {
                    // Silently ignore malformed property data
                }
            }
        }

        localQueue.pop();
    }
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkBase::ProcessSendQueue(void)
{
    // CRITICAL FIX: Allow processing send queue even during shutdown
    // Only check if socket is valid, not m_running status
    // This ensures disconnect packets can be sent during cleanup
    if (!m_socket) { return; }

    std::queue<FQueuedPacket> localQueue;

    // Quickly swap the queue to minimize lock contention
    {
        std::lock_guard<std::mutex> lock(m_sendQueueMutex);
        std::swap(localQueue, m_sendQueue);
    }

    // Process all queued packets outside the lock
    while (!localQueue.empty())
    {
        const auto& queuedPacket = localQueue.front();

        try
        {
            SizeT bytesSent = m_socket->send_to(
                asio::buffer(queuedPacket.data), queuedPacket.endpoint
            );
            m_statistics.packetsSent++;
            m_statistics.bytesOutgoing += bytesSent;

            // If this was a reliable packet and sending failed, remove from
            // pending ACKs
            if (queuedPacket.reliable && bytesSent != queuedPacket.data.size())
            {
                std::lock_guard<std::mutex> lock(m_pendingAcksMutex);
                m_pendingAcks.erase(
                    std::remove_if(
                        m_pendingAcks.begin(),
                        m_pendingAcks.end(),
                        [&queuedPacket](const FAcknowledgment& ack)
                        {
                            return ack.header.sequenceNumber ==
                                   queuedPacket.header.sequenceNumber;
                        }
                    ),
                    m_pendingAcks.end()
                );
            }
        }
        catch (const std::exception&)
        {
            m_statistics.packetsDropped++;

            // If this was a reliable packet, remove from pending ACKs on
            // failure
            if (queuedPacket.reliable)
            {
                std::lock_guard<std::mutex> lock(m_pendingAcksMutex);
                m_pendingAcks.erase(
                    std::remove_if(
                        m_pendingAcks.begin(),
                        m_pendingAcks.end(),
                        [&queuedPacket](const FAcknowledgment& ack)
                        {
                            return ack.header.sequenceNumber ==
                                   queuedPacket.header.sequenceNumber;
                        }
                    ),
                    m_pendingAcks.end()
                );
            }
        }

        localQueue.pop();
    }
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkBase::Update(Float32)
{
    // Process all queued packets first
    ProcessSendQueue();

    UInt32 currentTime = GetCurrentTimestamp();

    // Check for pending ACKs to resend
    {
        std::lock_guard<std::mutex> lock(m_pendingAcksMutex);
        for (auto& ack: m_pendingAcks)
        {
            static const UInt32 TIMEOUT =
                static_cast<UInt32>(ACK_TIMEOUT * 1000);

            if (currentTime - ack.header.timestamp >= TIMEOUT)
            {
                // Re-queue the packet for resending
                FQueuedPacket queuedPacket;
                queuedPacket.data = ack.data;
                queuedPacket.endpoint = ack.endpoint;
                queuedPacket.reliable = true;
                queuedPacket.header = ack.header;

                {
                    std::lock_guard<std::mutex> sendLock(m_sendQueueMutex);
                    m_sendQueue.push(std::move(queuedPacket));
                }

                // Update timestamp
                ack.header.timestamp = currentTime;
            }
        }
    }
}

}   // namespace tkd
