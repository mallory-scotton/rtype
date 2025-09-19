///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Core/Archives/FArchive.hpp>
#include <Engine/Config.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
FArchive::FArchive(EArchiveMode mode, bool allowSeek)
    : m_data()
    , m_version(0)
    , m_mode(mode)
    , m_allowSeek(allowSeek)
    , m_position(0)
{}

///////////////////////////////////////////////////////////////////////////////
FArchive& FArchive::operator<<(FString& value)
{
    SizeT length = value.Length();

    *this << length;
    if (m_mode == EArchiveMode::Loading)
    {
        value.Resize(length + 1, '\0');
        if (length > 0) { Serialize(&value[0], length); }
    }
    else if (m_mode == EArchiveMode::Saving)
    {
        if (length > 0) { Serialize(&value[0], length); }
    }

    return *this;
}

///////////////////////////////////////////////////////////////////////////////
FArchive& FArchive::operator<<(std::string& value)
{
    SizeT length = value.length();

    *this << length;
    if (m_mode == EArchiveMode::Loading)
    {
        value.resize(length + 1, '\0');
        if (length > 0) { Serialize(value.data(), length); }
    }
    else if (m_mode == EArchiveMode::Saving)
    {
        if (length > 0) { Serialize(value.data(), length); }
    }

    return *this;
}

///////////////////////////////////////////////////////////////////////////////
void FArchive::Serialize(void* data, SizeT size)
{
    if (m_mode == EArchiveMode::Loading)
    {
        // Ensure we don't read past the end of the buffer
        if (size == 0 || m_position >= m_data.size())
        {
            throw std::runtime_error(
                "Attempt to read past the end of the archive buffer"
            );
        }

        SizeT bytesToRead = Math<SizeT>::Min(size, m_data.size() - m_position);
        std::memcpy(data, m_data.data() + m_position, bytesToRead);
        m_position += bytesToRead;

        // If we couldn't read the full amount, zero out the rest
        if (bytesToRead < size)
        {
            std::memset(
                static_cast<UInt8*>(data) + bytesToRead, 0, size - bytesToRead
            );
        }
    }
    else if (m_mode == EArchiveMode::Saving)
    {
        if (size == 0)
        {
            return;   // Nothing to serialize
        }

        // Ensure the buffer is large enough
        if (m_position + size > m_data.size())
        {
            m_data.resize(m_position + size);
        }

        // Copy data from the provided pointer to the archive buffer
        std::memcpy(m_data.data() + m_position, data, size);
        m_position += size;
    }
}

///////////////////////////////////////////////////////////////////////////////
SizeT FArchive::Tell(void) const { return m_position; }

///////////////////////////////////////////////////////////////////////////////
void FArchive::Seek(SizeT position)
{
    if (!m_allowSeek)
    {
        throw std::runtime_error("Seeking is not allowed in this archive");
    }

    m_position = Math<SizeT>::Min(position, m_data.size());
}

///////////////////////////////////////////////////////////////////////////////
SizeT FArchive::TotalSize(void) const { return m_data.size(); }

///////////////////////////////////////////////////////////////////////////////
Int32 FArchive::GetVersion(void) const { return m_version; }

///////////////////////////////////////////////////////////////////////////////
void FArchive::SetVersion(Int32 version) { m_version = version; }

///////////////////////////////////////////////////////////////////////////////
EArchiveMode FArchive::GetMode(void) const { return m_mode; }

///////////////////////////////////////////////////////////////////////////////
const std::vector<UInt8>& FArchive::GetData(void) const { return m_data; }

///////////////////////////////////////////////////////////////////////////////
void FArchive::SetData(const std::vector<UInt8>& data) { m_data = data; }

///////////////////////////////////////////////////////////////////////////////
void FArchive::SetData(std::vector<UInt8>&& data) { m_data = std::move(data); }

///////////////////////////////////////////////////////////////////////////////
void FArchive::Clear(void)
{
    m_data.clear();
    m_position = 0;
}

}   // namespace tkd
