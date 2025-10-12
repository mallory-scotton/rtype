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
UInt32 FPacketManager::CalculateChecksum(const UInt8* data, SizeT size) const
{
    // Simple CRC32-like checksum implementation
    UInt32 checksum = 0xFFFFFFFF;

    for (SizeT i = 0; i < size; ++i)
    {
        checksum ^= data[i];
        for (int j = 0; j < 8; ++j)
        {
            if (checksum & 1) { checksum = (checksum >> 1) ^ 0xEDB88320; }
            else { checksum >>= 1; }
        }
    }

    return ~checksum;
}

///////////////////////////////////////////////////////////////////////////////
std::vector<UInt8>
    FPacketManager::SerializePacket(const IPacket& packet, EPacketFlags flags)
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
    header.flags = static_cast<UInt16>(flags);
    header.flags = static_cast<UInt16>(EPacketFlags::None);
    header.checksum = 0;   // Placeholder, will be calculated later

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

    std::vector<UInt8> checksumBuffer;
    FBinaryWriter checksumWriter(checksumBuffer);

    checksumWriter.Write(header.magic);
    checksumWriter.Write(header.protocolVersion);
    checksumWriter.Write(header.flags);
    checksumWriter.Write(header.packetType);
    checksumWriter.Write(header.packetSize);
    checksumWriter.Write(header.sequenceNumber);
    checksumWriter.Write(header.timestamp);

    const UInt8* packetDataStart = buffer.data() + FPacketHeader::SIZE;
    SizeT packetDataSize = buffer.size() - FPacketHeader::SIZE;
    checksumBuffer.insert(
        checksumBuffer.end(), packetDataStart, packetDataStart + packetDataSize
    );

    header.checksum =
        CalculateChecksum(checksumBuffer.data(), checksumBuffer.size());

    SizeT checksumOffset = sizeof(UInt32) * 4 + sizeof(UInt16) * 3;
    std::memcpy(
        buffer.data() + checksumOffset, &header.checksum, sizeof(UInt32)
    );

    return buffer;
}

///////////////////////////////////////////////////////////////////////////////
bool FPacketManager::ValidateChecksum(
    const UInt8* data, SizeT size, UInt32 expectedChecksum
) const
{
    std::vector<UInt8> checksumBuffer;

    SizeT checksumOffset = sizeof(UInt32) * 4 + sizeof(UInt16) * 3;

    checksumBuffer.insert(checksumBuffer.end(), data, data + checksumOffset);

    SizeT afterChecksumOffset = checksumOffset + sizeof(UInt32);
    checksumBuffer.insert(
        checksumBuffer.end(), data + afterChecksumOffset, data + size
    );

    UInt32 calculatedChecksum =
        CalculateChecksum(checksumBuffer.data(), checksumBuffer.size());
    return calculatedChecksum == expectedChecksum;
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

    // Verify checksum
    if (!ValidateChecksum(data, size, outHeader.checksum))
    {
        return nullptr;   // Checksum validation failed
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
