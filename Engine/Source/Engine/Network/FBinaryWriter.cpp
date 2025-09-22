///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Network/FBinaryWriter.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
FBinaryWriter::FBinaryWriter(std::vector<UInt8>& buffer)
    : m_buffer(buffer)
{}

///////////////////////////////////////////////////////////////////////////////
void FBinaryWriter::Write(const std::vector<std::string>& array)
{
    SizeT arraySize = array.size();
    Write(arraySize);
    for (const auto& str: array) { Write(str); }
}

///////////////////////////////////////////////////////////////////////////////
void FBinaryWriter::Write(const std::vector<FString>& array)
{
    SizeT arraySize = array.size();
    Write(arraySize);
    for (const auto& str: array) { Write(str); }
}

///////////////////////////////////////////////////////////////////////////////
void FBinaryWriter::Write(const std::string& str)
{
    SizeT strSize = str.size();
    Write(strSize);
    if (strSize > 0)
    {
        SizeT needed = m_offset + strSize;
        if (m_buffer.size() < needed) { m_buffer.resize(needed); }
        std::memcpy(m_buffer.data() + m_offset, str.data(), strSize);
        m_offset += strSize;
    }
}

///////////////////////////////////////////////////////////////////////////////
void FBinaryWriter::Write(const FString& str)
{
    SizeT strSize = str.Size();
    Write(strSize);
    if (strSize > 0)
    {
        SizeT needed = m_offset + strSize;
        if (m_buffer.size() < needed) { m_buffer.resize(needed); }
        std::memcpy(m_buffer.data() + m_offset, str.CStr(), strSize);
        m_offset += strSize;
    }
}

///////////////////////////////////////////////////////////////////////////////
void FBinaryWriter::WriteBytes(const UInt8* data, SizeT size)
{
    SizeT needed = m_offset + size;
    if (m_buffer.size() < needed) { m_buffer.resize(needed); }
    std::memcpy(m_buffer.data() + m_offset, data, size);
    m_offset += size;
}

///////////////////////////////////////////////////////////////////////////////
SizeT FBinaryWriter::GetOffset(void) const { return m_offset; }

///////////////////////////////////////////////////////////////////////////////
void FBinaryWriter::SetOffset(SizeT offset) { m_offset = offset; }

}   // namespace tkd
