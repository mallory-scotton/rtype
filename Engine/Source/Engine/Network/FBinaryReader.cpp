///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Network/FBinaryReader.hpp>
#include <Engine/Runtime/World/ULevel.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
FBinaryReader::FBinaryReader(const UInt8* data, SizeT size)
    : m_data(data)
    , m_size(size)
    , m_offset(0)
{}

///////////////////////////////////////////////////////////////////////////////
bool FBinaryReader::Read(std::string& value)
{
    SizeT size = 0;
    if (!Read(size)) { return false; }
    value.resize(size);
    if (!ReadBytes(value.data(), size)) { return false; }
    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool FBinaryReader::Read(FString& value)
{
    SizeT size = 0;
    if (!Read(size)) { return false; }
    value.Resize(size);
    if (!ReadBytes(value.Data(), size)) { return false; }
    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool FBinaryReader::Read(FVector3& value)
{
    return Read(value.x) && Read(value.y) && Read(value.z);
}

///////////////////////////////////////////////////////////////////////////////
bool FBinaryReader::Read(FRotator& value)
{
    float pitch, yaw, roll;
    if (!Read(pitch) || !Read(yaw) || !Read(roll)) { return false; }
    value = FRotator(pitch, yaw, roll);
    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool FBinaryReader::ReadBytes(void* buffer, SizeT size)
{
    if (m_offset + size > m_size) { return false; }
    std::memcpy(buffer, m_data + m_offset, size);
    m_offset += size;
    return true;
}

///////////////////////////////////////////////////////////////////////////////
SizeT FBinaryReader::GetOffset(void) const { return m_offset; }

///////////////////////////////////////////////////////////////////////////////
SizeT FBinaryReader::GetRemaining(void) const { return m_size - m_offset; }

}   // namespace tkd
