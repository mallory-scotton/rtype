///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Network/FPacketManager.hpp>
#include <Engine/Network/FBinaryReader.hpp>
#include <Engine/Network/FBinaryWriter.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
std::vector<UInt8> FPacketManager::SerializePacket(const IPacket& packet)
{
    std::vector<UInt8> buffer;
    FBinaryWriter writer(buffer);

    // Prepare packet header
    FPacketHeader header;
    header.packetType = packet.GetType();
    header.packetSize =
        static_cast<UInt16>(FPacketHeader::SIZE + packet.GetSize());
    header.sequenceNumber = ++m_sequenceNumber;
    header.timestamp = GetCurrentTimestamp();
    header.flags = static_cast<UInt16>(EPacketFlags::None);
    header.checksum = 0;   // TODO: Implement checksum calculation

    // Write header fields
    writer.Write(header.magic);
    writer.Write(header.protocolVersion);
    writer.Write(header.flags);
    writer.Write(header.packetType);
    writer.Write(header.packetSize);
    writer.Write(header.sequenceNumber);
    writer.Write(header.timestamp);
    writer.Write(header.checksum);

    // Serialize packet data
    if (!packet.Serialize(writer)) { return {}; }

    return buffer;
}

///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<IPacket> FPacketManager::DeserializePacket(
    const UInt8* data, SizeT size, FPacketHeader& outHeader
)
{
    // Validate input size
    if (size < FPacketHeader::SIZE) { return nullptr; }

    // Read header
    FBinaryReader reader(data, size);

    // Read and validate header fields
    if (!reader.Read(outHeader.magic) || outHeader.magic != MAGIC_NUMBER)
    {
        return nullptr;
    }

    // Read remaining header fields
    if (!reader.Read(outHeader.protocolVersion) ||
        !reader.Read(outHeader.flags) || !reader.Read(outHeader.packetType) ||
        !reader.Read(outHeader.packetSize) ||
        !reader.Read(outHeader.sequenceNumber) ||
        !reader.Read(outHeader.timestamp) || !reader.Read(outHeader.checksum))
    {
        return nullptr;
    }

    // Validate protocol version and packet size
    if (outHeader.protocolVersion != PROTOCOL_VERSION ||
        outHeader.packetSize != size)
    {
        return nullptr;
    }

    // Find the appropriate factory for the packet type
    auto it = m_factories.find(outHeader.packetType);
    if (it == m_factories.end()) { return nullptr; }

    // Create packet instance and deserialize data
    auto packet = it->second();
    if (!packet->Deserialize(reader)) { return nullptr; }

    // Successful deserialization
    return packet;
}

///////////////////////////////////////////////////////////////////////////////
UInt32 FPacketManager::GetCurrentTimestamp(void) const
{
    auto now = SteadyClock::now();
    auto duration = now.time_since_epoch();
    return static_cast<UInt32>(
        std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()
    );
}

}   // namespace tkd
