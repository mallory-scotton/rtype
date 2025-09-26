///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Core/Utils/FUUID.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
std::mt19937_64 UUID::s_rng{ std::random_device{}() };

///////////////////////////////////////////////////////////////////////////////
std::optional<UUID> UUID::Parse(std::string_view str) noexcept
{
    // Remove dashes and convert to lowercase
    std::string clean;
    clean.reserve(32);

    for (char c: str)
    {
        if (c != '-')
        {
            if (std::isxdigit(c)) { clean.push_back(std::tolower(c)); }
            else
            {
                return std::nullopt;   // Invalid character
            }
        }
    }

    if (clean.length() != 32)
    {
        return std::nullopt;   // Wrong length
    }

    DataType bytes;
    for (size_t i = 0; i < 16; ++i)
    {
        char high = clean[i * 2];
        char low = clean[i * 2 + 1];

        auto HexToByte = [](char c) -> uint8_t
        {
            if (c >= '0' && c <= '9') { return c - '0'; }
            if (c >= 'a' && c <= 'f') { return c - 'a' + 10; }
            return 0;   // Should not happen due to isxdigit check above
        };

        bytes[i] = (HexToByte(high) << 4) | HexToByte(low);
    }

    return UUID{ bytes };
}

///////////////////////////////////////////////////////////////////////////////
std::string UUID::ToString(void) const
{
    return std::format(
        "{:02x}{:02x}{:02x}{:02x}-{:02x}{:02x}-{:02x}{:02x}-{:02x}{:02x}-{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}",
        m_data[0],
        m_data[1],
        m_data[2],
        m_data[3],
        m_data[4],
        m_data[5],
        m_data[6],
        m_data[7],
        m_data[8],
        m_data[9],
        m_data[10],
        m_data[11],
        m_data[12],
        m_data[13],
        m_data[14],
        m_data[15]
    );
}

///////////////////////////////////////////////////////////////////////////////
std::string UUID::ToStringNoDashes(void) const
{
    return std::format(
        "{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}",
        m_data[0],
        m_data[1],
        m_data[2],
        m_data[3],
        m_data[4],
        m_data[5],
        m_data[6],
        m_data[7],
        m_data[8],
        m_data[9],
        m_data[10],
        m_data[11],
        m_data[12],
        m_data[13],
        m_data[14],
        m_data[15]
    );
}

///////////////////////////////////////////////////////////////////////////////
std::array<UInt8, 6> UUID::GetMACAddress(void)
{
    std::array<UInt8, 6> mac = {};

#if defined(_WIN32)
    // Windows implementation would go here
    // For now, generate random MAC with local bit set
    std::uniform_int_distribution<UInt8> dist;
    for (auto& byte: mac) { byte = dist(s_rng); }
    mac[0] |= 0x01;
#elif defined(__linux__) || defined(__APPLE__)
    // Unix implementation would go here
    // For now, generate random MAC with local bit set
    std::uniform_int_distribution<UInt8> dist;
    for (auto& byte: mac) { byte = dist(s_rng); }
    mac[0] |= 0x01;
#else
    // Fallback: random MAC with local bit set
    std::uniform_int_distribution<UInt8> dist;
    for (auto& byte: mac) { byte = dist(s_rng); }
    mac[0] |= 0x01;
#endif
    return mac;
}

///////////////////////////////////////////////////////////////////////////////
UInt64 UUID::GetTimestamp100ns(void) noexcept
{
    // UUID timestamp is 100ns intervals since 1582-10-15 00:00:00
    // Unix timestamp is seconds since 1970-01-01 00:00:00
    // Difference is 122192928000000000 100ns intervals
    constexpr UInt64 UUID_EPOCH_OFFSET = 122192928000000000ULL;

    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto ns =
        std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();

    return (ns / 100) + UUID_EPOCH_OFFSET;
}

///////////////////////////////////////////////////////////////////////////////
std::array<UInt8, 16> UUID::MD5Hash(std::span<const UInt8> data)
{
    // This is a placeholder - in production, use a proper MD5 implementation
    // like OpenSSL, Crypto++, or similar
    std::array<UInt8, 16> result = {};
    std::hash<std::string_view> hasher;
    auto hash_val = hasher(std::string_view{
      reinterpret_cast<const char*>(data.data()), data.size() });

    // Distribute hash value across the array (not cryptographically secure)
    for (size_t i = 0; i < 16; ++i)
    {
        result[i] = static_cast<UInt8>((hash_val >> (i % 8 * 8)) & 0xFF);
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
std::array<UInt8, 20> UUID::SHA1Hash(std::span<const UInt8> data)
{
    // This is a placeholder - in production, use a proper SHA-1 implementation
    std::array<UInt8, 20> result = {};
    std::hash<std::string_view> hasher;
    auto hash_val = hasher(std::string_view{
      reinterpret_cast<const char*>(data.data()), data.size() });

    // Distribute hash value across the array (not cryptographically secure)
    for (size_t i = 0; i < 20; ++i)
    {
        result[i] = static_cast<UInt8>((hash_val >> (i % 8 * 8)) & 0xFF);
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
UUID UUID::V1(void)
{
    UUID::DataType bytes;

    // Get current timestamp in 100ns intervals since UUID epoch (1582-10-15)
    UInt64 timestamp = GetTimestamp100ns();

    // Time low (32 bits)
    bytes[0] = (timestamp >> 24) & 0xFF;
    bytes[1] = (timestamp >> 16) & 0xFF;
    bytes[2] = (timestamp >> 8) & 0xFF;
    bytes[3] = timestamp & 0xFF;

    // Time mid (16 bits)
    bytes[4] = (timestamp >> 40) & 0xFF;
    bytes[5] = (timestamp >> 32) & 0xFF;

    // Time high and version (16 bits)
    bytes[6] = (timestamp >> 56) & 0xFF;
    bytes[7] = (timestamp >> 48) & 0xFF;

    // Clock sequence (14 bits) + variant (2 bits)
    std::uniform_int_distribution<UInt16> clock_dist(0, 0x3FFF);
    UInt16 clock_seq = clock_dist(s_rng);
    bytes[8] = (clock_seq >> 8) & 0xFF;
    bytes[9] = clock_seq & 0xFF;

    // Node (MAC address, 48 bits)
    auto mac = GetMACAddress();
    std::copy(mac.begin(), mac.end(), bytes.begin() + 10);

    // Set version and variant directly in the bytes
    bytes[6] =
        (bytes[6] & 0x0F) | (static_cast<UInt8>(EUUIDVersion::TimeBased) << 4);
    bytes[8] = (bytes[8] & 0x3F) | 0x80;

    return UUID{ bytes };
}

///////////////////////////////////////////////////////////////////////////////
UUID UUID::V3(UUID::SpanType namespaceUUID, std::string_view name)
{
    // Concatenate namespace UUID and name
    std::vector<UInt8> data;
    data.reserve(16 + name.size());
    data.insert(data.end(), namespaceUUID.begin(), namespaceUUID.end());
    data.insert(data.end(), name.begin(), name.end());

    auto hash = MD5Hash(data);

    UUID::DataType bytes;
    std::copy(hash.begin(), hash.end(), bytes.begin());

    // Set version and variant directly in the bytes
    bytes[6] = (bytes[6] & 0x0F) |
               (static_cast<UInt8>(EUUIDVersion::NameBasedMD5) << 4);
    bytes[8] = (bytes[8] & 0x3F) | 0x80;

    return UUID{ bytes };
}

///////////////////////////////////////////////////////////////////////////////
UUID UUID::DNSV3(std::string_view name)
{
    return V3(std::span{ __internal::uuid::DNS }, name);
}

///////////////////////////////////////////////////////////////////////////////
UUID UUID::URLV3(std::string_view name)
{
    return V3(std::span{ __internal::uuid::URL }, name);
}

///////////////////////////////////////////////////////////////////////////////
UUID UUID::V4(void)
{
    UUID::DataType bytes;

    // Fill with random bytes
    std::uniform_int_distribution<UInt32> dist;
    UInt32* words = reinterpret_cast<UInt32*>(bytes.data());
    for (SizeT i = 0; i < 4; ++i) { words[i] = dist(s_rng); }

    // Set version and variant directly in the bytes
    bytes[6] =
        (bytes[6] & 0x0F) | (static_cast<UInt8>(EUUIDVersion::Random) << 4);
    bytes[8] = (bytes[8] & 0x3F) | 0x80;

    return UUID{ bytes };
}

///////////////////////////////////////////////////////////////////////////////
UUID UUID::V5(UUID::SpanType namespaceUUID, std::string_view name)
{
    // Concatenate namespace UUID and name
    std::vector<UInt8> data;
    data.reserve(16 + name.size());
    data.insert(data.end(), namespaceUUID.begin(), namespaceUUID.end());
    data.insert(data.end(), name.begin(), name.end());

    auto hash = SHA1Hash(data);

    UUID::DataType bytes;
    std::copy_n(
        hash.begin(), 16, bytes.begin()
    );   // Take first 16 bytes of SHA-1

    // Set version and variant directly in the bytes
    bytes[6] = (bytes[6] & 0x0F) |
               (static_cast<UInt8>(EUUIDVersion::NameBasedSHA1) << 4);
    bytes[8] = (bytes[8] & 0x3F) | 0x80;

    return UUID{ bytes };
}

///////////////////////////////////////////////////////////////////////////////
UUID UUID::DNSV5(std::string_view name)
{
    return V5(std::span{ __internal::uuid::DNS }, name);
}

///////////////////////////////////////////////////////////////////////////////
UUID UUID::URLV5(std::string_view name)
{
    return V5(std::span{ __internal::uuid::URL }, name);
}

}   // namespace tkd

///////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& os, const tkd::UUID& uuid)
{
    os << uuid.ToString();
    return os;
}
