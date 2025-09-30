///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include <array>
#include <chrono>
#include <compare>
#include <concepts>
#include <cstdint>
#include <cstring>
#include <Engine/Config.hpp>
#include <format>
#include <functional>
#include <iostream>
#include <optional>
#include <random>
#include <span>
#include <string>
#include <string_view>
#if defined(_WIN32)
    #include <rpc.h>
    #pragma comment(lib, "rpcrt4.lib")
#elif defined(__linux__) || defined(__APPLE__)
    #include <ifaddrs.h>
    #include <net/if.h>
    #include <sys/socket.h>
#endif

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief String-like concept
///
/// \tparam T Type to check
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
concept TStringLike = requires(T t) { std::string_view{ t }; };

///////////////////////////////////////////////////////////////////////////////
/// \brief Byte span concept
///
/// \tparam T Type to check
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
concept TByteSpan = requires(T t) { std::span<const UInt8>{ t }; };

///////////////////////////////////////////////////////////////////////////////
/// \brief Universally Unique Identifier (UUID) class version enumeration
///
///////////////////////////////////////////////////////////////////////////////
enum class EUUIDVersion : UInt8
{
    TimeBased = 1,      //<! Mac address + timestamp
    DCESecurity = 2,    //<! DCE Security (rarely used)
    NameBasedMD5 = 3,   //<! Namespace + name with MD5
    Random = 4,         //<! Random/pseudo-random
    NameBasedSHA1 = 5   //<! Namespace + name with SHA1
};

///////////////////////////////////////////////////////////////////////////////
/// \brief Universally Unique Identifier (UUID) variant enumeration
///
///////////////////////////////////////////////////////////////////////////////
enum class EUUIDVariant : UInt8
{
    NCS = 0,         //<! NCS backward compatibility
    RFC4122 = 1,     //<! RFC 4122 (Leach-Salz), used by this implementation
    Microsoft = 2,   //<! Microsoft Corporation backward compatibility
    Future = 3       //<! Future definition
};

///////////////////////////////////////////////////////////////////////////////
// Namespace __internal::uuid
///////////////////////////////////////////////////////////////////////////////
namespace __internal::uuid
{

///////////////////////////////////////////////////////////////////////////////
// Predefined namespace UUIDs
///////////////////////////////////////////////////////////////////////////////
inline constexpr std::array<UInt8, 16> DNS = { 0x6b, 0xa7, 0xb8, 0x10,
                                               0x9d, 0xad, 0x11, 0xd1,
                                               0x80, 0xb4, 0x00, 0xc0,
                                               0x4f, 0xd4, 0x30, 0xc8 };

///////////////////////////////////////////////////////////////////////////////
// Predefined namespace UUIDs
///////////////////////////////////////////////////////////////////////////////
inline constexpr std::array<UInt8, 16> URL = { 0x6b, 0xa7, 0xb8, 0x11,
                                               0x9d, 0xad, 0x11, 0xd1,
                                               0x80, 0xb4, 0x00, 0xc0,
                                               0x4f, 0xd4, 0x30, 0xc8 };

///////////////////////////////////////////////////////////////////////////////
// Predefined namespace UUIDs
///////////////////////////////////////////////////////////////////////////////
inline constexpr std::array<UInt8, 16> OID = { 0x6b, 0xa7, 0xb8, 0x12,
                                               0x9d, 0xad, 0x11, 0xd1,
                                               0x80, 0xb4, 0x00, 0xc0,
                                               0x4f, 0xd4, 0x30, 0xc8 };

///////////////////////////////////////////////////////////////////////////////
// Predefined namespace UUIDs
///////////////////////////////////////////////////////////////////////////////
inline constexpr std::array<UInt8, 16> X500 = { 0x6b, 0xa7, 0xb8, 0x14,
                                                0x9d, 0xad, 0x11, 0xd1,
                                                0x80, 0xb4, 0x00, 0xc0,
                                                0x4f, 0xd4, 0x30, 0xc8 };

}   // namespace __internal::uuid

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
class UUID
{
public:
    ///////////////////////////////////////////////////////////////////////////
    // Class aliases
    ///////////////////////////////////////////////////////////////////////////
    using DataType = std::array<UInt8, 16>;
    using SpanType = std::span<const UInt8, 16>;

private:
    ///////////////////////////////////////////////////////////////////////////
    // Static Class Member
    ///////////////////////////////////////////////////////////////////////////
    static std::mt19937_64 s_rng;   //<! Random number generator

private:
    ///////////////////////////////////////////////////////////////////////////
    // Class Member
    ///////////////////////////////////////////////////////////////////////////
    DataType m_data;   //<! Raw UUID data

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor
    ///
    ///////////////////////////////////////////////////////////////////////////
    constexpr UUID(void) noexcept
        : m_data{ 0 }
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor from raw byte array
    ///
    /// \param bytes Raw byte array to initialize the UUID from
    ///
    ///////////////////////////////////////////////////////////////////////////
    explicit constexpr UUID(const DataType& bytes) noexcept
        : m_data{ bytes }
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor from raw byte array
    ///
    /// \param bytes Raw byte array to initialize the UUID from
    ///
    ///////////////////////////////////////////////////////////////////////////
    explicit constexpr UUID(SpanType bytes) noexcept
        : m_data{ 0 }
    {
        std::copy(bytes.begin(), bytes.end(), m_data.begin());
    }

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Three-way comparison operator
    ///
    /// \param other The other UUID to compare with
    ///
    /// \return The result of the comparison
    ///
    ///////////////////////////////////////////////////////////////////////////
    constexpr auto operator<=>(const UUID& other) const noexcept = default;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Equality operator
    ///
    /// \param other The other UUID to compare with
    ///
    /// \return True if the UUIDs are equal, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    constexpr bool operator==(const UUID& other) const noexcept = default;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Parse a UUID from its string representation
    ///
    /// \param str String representation of the UUID
    ///
    /// \return Optional UUID object if parsing was successful, std::nullopt
    /// otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    static std::optional<UUID> Parse(std::string_view str) noexcept;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the raw byte array of the UUID
    ///
    /// \return The raw byte array of the UUID
    ///
    ///////////////////////////////////////////////////////////////////////////
    constexpr const DataType& Data(void) const noexcept { return m_data; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the raw byte array of the UUID
    ///
    /// \return The raw byte array of the UUID
    ///
    ///////////////////////////////////////////////////////////////////////////
    constexpr SpanType Bytes(void) const noexcept
    {
        return SpanType(m_data.data(), m_data.size());
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the version of the UUID
    ///
    /// \return The version of the UUID
    ///
    ///////////////////////////////////////////////////////////////////////////
    constexpr EUUIDVersion Version(void) const noexcept
    {
        return static_cast<EUUIDVersion>((m_data[6] & 0x0F) >> 4);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the variant of the UUID
    ///
    /// \return The variant of the UUID
    ///
    ///////////////////////////////////////////////////////////////////////////
    constexpr EUUIDVariant Variant(void) const noexcept
    {
        UInt8 byte8 = m_data[8];
        if ((byte8 & 0x80) == 0) { return EUUIDVariant::NCS; }
        if ((byte8 & 0xC0) == 0x80) { return EUUIDVariant::RFC4122; }
        if ((byte8 & 0xE0) == 0xC0) { return EUUIDVariant::Microsoft; }
        return EUUIDVariant::Future;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Convert the UUID to its string representation with dashes
    ///
    /// \return The string representation of the UUID
    ///
    ///////////////////////////////////////////////////////////////////////////
    std::string ToString(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Convert the UUID to its string representation without dashes
    ///
    /// \return The string representation of the UUID without dashes
    ///
    ///////////////////////////////////////////////////////////////////////////
    std::string ToStringNoDashes(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Check if the UUID is nil (all bits are zero)
    ///
    /// \return True if the UUID is nil, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    constexpr bool IsNil(void) const noexcept
    {
        return std::all_of(
            m_data.begin(), m_data.end(), [](UInt8 byte) { return byte == 0; }
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Check if the UUID is valid (correct version and variant)
    ///
    /// \return True if the UUID is valid, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    constexpr bool IsValid(void) const noexcept
    {
        return Variant() == EUUIDVariant::RFC4122 &&
               static_cast<UInt8>(Version()) >= 1 &&
               static_cast<UInt8>(Version()) <= 5;
    }

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the version and variant fields of the UUID
    ///
    /// \param version The UUID version to set
    ///
    ///////////////////////////////////////////////////////////////////////////
    constexpr void SetVersionVariant(EUUIDVersion version) noexcept
    {
        m_data[6] = (m_data[6] & 0x0F) | (static_cast<UInt8>(version) << 4);
        m_data[8] = (m_data[8] & 0x3F) | 0x80;
    }

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the MAC address of the current machine
    ///
    /// \return The MAC address of the current machine
    ///
    ///////////////////////////////////////////////////////////////////////////
    static std::array<UInt8, 6> GetMACAddress(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the current timestamp in 100-nanosecond intervals since
    ///
    /// \return The current timestamp in 100-nanosecond intervals since
    ///
    ///////////////////////////////////////////////////////////////////////////
    static UInt64 GetTimestamp100ns(void) noexcept;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Compute the MD5 hash of the given data
    ///
    /// \param data Data to hash
    ///
    /// \return The MD5 hash of the data
    ///
    ///////////////////////////////////////////////////////////////////////////
    static std::array<UInt8, 16> MD5Hash(std::span<const UInt8> data);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Compute the SHA-1 hash of the given data
    ///
    /// \param data Data to hash
    ///
    /// \return The SHA-1 hash of the data
    ///
    ///////////////////////////////////////////////////////////////////////////
    static std::array<UInt8, 20> SHA1Hash(std::span<const UInt8> data);

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Generate a version 1 (time-based) UUID
    ///
    /// \return The generated version 1 UUID
    ///
    ///////////////////////////////////////////////////////////////////////////
    static UUID V1(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Generate a version 3 (name-based) UUID
    ///
    /// \param namespaceUUID The namespace UUID
    /// \param name The name from which to generate the UUID
    ///
    /// \return The generated version 3 UUID
    ///
    ///////////////////////////////////////////////////////////////////////////
    static UUID V3(SpanType namespaceUUID, std::string_view name);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Generate a version 3 (name-based) UUID
    ///
    /// \param namespaceUUID The namespace UUID
    /// \param name The name from which to generate the UUID
    ///
    /// \return The generated version 3 UUID
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <TStringLike T>
    static UUID V3(const UUID& namespaceUUID, const T& name)
    {
        return V3(namespaceUUID.Bytes(), std::string_view{ name });
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Generate a version 3 (name-based) UUID
    ///
    /// \param name The name from which to generate the UUID
    ///
    /// \return The generated version 3 UUID
    ///
    ///////////////////////////////////////////////////////////////////////////
    static UUID DNSV3(std::string_view name);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Generate a version 3 (name-based) UUID
    ///
    /// \param name The name from which to generate the UUID
    ///
    /// \return The generated version 3 UUID
    ///
    ///////////////////////////////////////////////////////////////////////////
    static UUID URLV3(std::string_view name);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Generate a version 4 (random) UUID
    ///
    /// \return The generated version 4 UUID
    ///
    ///////////////////////////////////////////////////////////////////////////
    static UUID V4(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Generate a version 5 (name-based) UUID
    ///
    /// \param namespaceUUID The namespace UUID
    /// \param name The name from which to generate the UUID
    ///
    /// \return The generated version 5 UUID
    ///
    ///////////////////////////////////////////////////////////////////////////
    static UUID V5(SpanType namespaceUUID, std::string_view name);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Generate a version 5 (name-based) UUID
    ///
    /// \param namespaceUUID The namespace UUID
    /// \param name The name from which to generate the UUID
    ///
    /// \return The generated version 5 UUID
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <TStringLike T>
    static UUID V5(const UUID& namespaceUUID, const T& name)
    {
        return V5(namespaceUUID.Bytes(), std::string_view{ name });
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Generate a version 5 (name-based) UUID
    ///
    /// \param name The name from which to generate the UUID
    ///
    /// \return The generated version 5 UUID
    ///
    ///////////////////////////////////////////////////////////////////////////
    static UUID DNSV5(std::string_view name);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Generate a version 5 (name-based) UUID
    ///
    /// \param name The name from which to generate the UUID
    ///
    /// \return The generated version 5 UUID
    ///
    ///////////////////////////////////////////////////////////////////////////
    static UUID URLV5(std::string_view name);
};

}   // namespace tkd

///////////////////////////////////////////////////////////////////////////////
/// \brief Specialization of std::hash for tkd::UUID
///
///////////////////////////////////////////////////////////////////////////////
template <>
struct std::hash<tkd::UUID>
{
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Hash function for tkd::UUID
    ///
    ///////////////////////////////////////////////////////////////////////////
    std::size_t operator()(const tkd::UUID& uuid) const noexcept
    {
        const auto& data = uuid.Data();
        uint64_t hash_val;
        std::memcpy(&hash_val, data.data(), sizeof(hash_val));
        return std::hash<uint64_t>{}(hash_val);
    }
};

///////////////////////////////////////////////////////////////////////////////
/// \brief Specialization of std::formatter for tkd::UUID
///
///////////////////////////////////////////////////////////////////////////////
template <>
struct std::formatter<tkd::UUID> : std::formatter<std::string>
{
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Format function for tkd::UUID
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename FormatContext>
    auto format(const tkd::UUID& uuid, FormatContext& ctx) const
    {
        return std::formatter<std::string>::format(uuid.ToString(), ctx);
    }
};

///////////////////////////////////////////////////////////////////////////////
/// \brief Stream insertion operator for tkd::UUID
///
/// \param os Output stream
/// \param uuid UUID to insert into the stream
///
/// \return Reference to the output stream
///
///////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& os, const tkd::UUID& uuid);
