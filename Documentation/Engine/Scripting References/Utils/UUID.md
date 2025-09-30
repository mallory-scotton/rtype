# UUID Class Documentation

## Overview

The `UUID` class in the TKD Engine provides a comprehensive implementation of Universally Unique Identifiers (UUIDs) as defined by RFC 4122. This class supports all standard UUID versions (1, 3, 4, 5) and provides both generation and parsing capabilities. The implementation is designed for high performance, type safety, and ease of use within the TKD Engine ecosystem.

## Features

- **RFC 4122 Compliance**: Full compliance with the UUID specification
- **All Standard Versions**: Support for time-based (v1), name-based MD5 (v3), random (v4), and name-based SHA-1 (v5) UUIDs
- **Efficient Storage**: 16-byte internal representation
- **Type Safety**: Strong typing with concepts and constexpr where possible
- **String Conversion**: Bidirectional conversion between UUID and string representations
- **Comparison Operators**: Full comparison support including three-way comparison (C++20)
- **Hash Support**: Specialization of `std::hash` for use in unordered containers
- **Format Support**: Specialization of `std::formatter` for modern formatting
- **Stream Support**: Output stream operator for easy printing
- **Validation**: Built-in validation for UUID correctness
- **Nil UUID Support**: Detection of nil (all-zero) UUIDs

## Dependencies

The UUID class depends on the following standard library headers:
- `<algorithm>`, `<array>`, `<chrono>`, `<compare>`, `<concepts>`, `<cstdint>`, `<cstring>`
- `<Engine/Config.hpp>` (for type aliases)
- `<format>`, `<functional>`, `<iostream>`, `<optional>`, `<random>`, `<span>`, `<string>`, `<string_view>`

Platform-specific headers for MAC address retrieval:
- Windows: `<rpc.h>`
- Unix-like: `<ifaddrs.h>`, `<net/if.h>`, `<sys/socket.h>`

## Class Definition

```cpp
namespace tkd
{
class UUID
{
public:
    using DataType = std::array<UInt8, 16>;
    using SpanType = std::span<const UInt8, 16>;

    // ... (detailed members below)
};
}
```

### Type Aliases

- `DataType`: `std::array<UInt8, 16>` - The underlying byte array type
- `SpanType`: `std::span<const UInt8, 16>` - A read-only span over the UUID bytes

## Public Members

### Constructors

#### Default Constructor
```cpp
constexpr UUID(void) noexcept;
```
Creates a nil UUID (all bytes set to 0).

#### Byte Array Constructor
```cpp
explicit constexpr UUID(const DataType& bytes) noexcept;
explicit constexpr UUID(SpanType bytes) noexcept;
```
Constructs a UUID from raw byte data. The span version copies the data into internal storage.

### Accessors

#### Data Access
```cpp
constexpr const DataType& Data(void) const noexcept;
constexpr SpanType Bytes(void) const noexcept;
```
- `Data()`: Returns a const reference to the internal byte array
- `Bytes()`: Returns a span over the UUID bytes

#### Version and Variant
```cpp
constexpr EUUIDVersion Version(void) const noexcept;
constexpr EUUIDVariant Variant(void) const noexcept;
```
- `Version()`: Returns the UUID version (1-5)
- `Variant()`: Returns the UUID variant (always RFC4122 for valid UUIDs)

#### String Conversion
```cpp
std::string ToString(void) const;
std::string ToStringNoDashes(void) const;
```
- `ToString()`: Returns the standard string representation with dashes (e.g., "550e8400-e29b-41d4-a716-446655440000")
- `ToStringNoDashes()`: Returns the compact string representation without dashes (e.g., "550e8400e29b41d4a716446655440000")

#### Validation
```cpp
constexpr bool IsNil(void) const noexcept;
constexpr bool IsValid(void) const noexcept;
```
- `IsNil()`: Returns true if all bytes are zero
- `IsValid()`: Returns true if the UUID has correct version (1-5) and RFC4122 variant

### Comparison Operators

```cpp
constexpr auto operator<=>(const UUID& other) const noexcept = default;
constexpr bool operator==(const UUID& other) const noexcept = default;
```
Supports three-way comparison and equality comparison using default implementations.

## Static Functions

### Parsing

#### Parse from String
```cpp
static std::optional<UUID> Parse(std::string_view str) noexcept;
```
Parses a UUID from its string representation. Accepts both dashed and dashless formats. Returns `std::nullopt` on parsing failure.

### UUID Generation

#### Version 1 (Time-based)
```cpp
static UUID V1(void);
```
Generates a version 1 UUID using the current timestamp and MAC address. Thread-safe but not cryptographically secure.

#### Version 3 (Name-based MD5)
```cpp
static UUID V3(SpanType namespaceUUID, std::string_view name);
template <TStringLike T>
static UUID V3(const UUID& namespaceUUID, const T& name);
static UUID DNSV3(std::string_view name);
static UUID URLV3(std::string_view name);
```
Generates version 3 UUIDs using MD5 hashing:
- `V3()`: General name-based generation with custom namespace
- `DNSV3()`: Uses DNS namespace UUID
- `URLV3()`: Uses URL namespace UUID

#### Version 4 (Random)
```cpp
static UUID V4(void);
```
Generates a version 4 UUID using cryptographically secure random bytes.

#### Version 5 (Name-based SHA-1)
```cpp
static UUID V5(SpanType namespaceUUID, std::string_view name);
template <TStringLike T>
static UUID V5(const UUID& namespaceUUID, const T& name);
static UUID DNSV5(std::string_view name);
static UUID URLV5(std::string_view name);
```
Generates version 5 UUIDs using SHA-1 hashing:
- `V5()`: General name-based generation with custom namespace
- `DNSV5()`: Uses DNS namespace UUID
- `URLV5()`: Uses URL namespace UUID

## Enumerations

### EUUIDVersion
```cpp
enum class EUUIDVersion : UInt8
{
    TimeBased = 1,      // MAC address + timestamp
    DCESecurity = 2,    // DCE Security (rarely used)
    NameBasedMD5 = 3,   // Namespace + name with MD5
    Random = 4,         // Random/pseudo-random
    NameBasedSHA1 = 5   // Namespace + name with SHA-1
};
```

### EUUIDVariant
```cpp
enum class EUUIDVariant : UInt8
{
    NCS = 0,         // NCS backward compatibility
    RFC4122 = 1,     // RFC 4122 (Leach-Salz), used by this implementation
    Microsoft = 2,   // Microsoft Corporation backward compatibility
    Future = 3       // Future definition
};
```

## Standard Library Specializations

### std::hash Specialization
```cpp
template <>
struct std::hash<tkd::UUID>
{
    std::size_t operator()(const tkd::UUID& uuid) const noexcept;
};
```
Provides hashing support for use in `std::unordered_map`, `std::unordered_set`, etc.

### std::formatter Specialization
```cpp
template <>
struct std::formatter<tkd::UUID> : std::formatter<std::string>
{
    template <typename FormatContext>
    auto format(const tkd::UUID& uuid, FormatContext& ctx) const;
};
```
Enables formatting with `std::format()` and related functions.

### Stream Operator
```cpp
std::ostream& operator<<(std::ostream& os, const tkd::UUID& uuid);
```
Enables output to streams using `operator<<`.

## Implementation Details

### Internal Storage
The UUID is stored as a 16-byte array (`std::array<UInt8, 16>`), providing efficient storage and access.

### Random Number Generation
- Uses `std::mt19937_64` seeded with `std::random_device` for version 4 UUIDs
- Thread-safe static initialization

### Hashing Implementation
- MD5 and SHA-1 implementations are placeholders using `std::hash<std::string_view>`
- **Warning**: Current hashing is not cryptographically secure; production code should use proper cryptographic libraries

### MAC Address Retrieval
- Windows: Uses RPC functions (placeholder implementation)
- Unix-like: Uses `getifaddrs()` (placeholder implementation)
- Fallback: Generates random MAC with locally administered bit set

### Timestamp Handling
- Version 1 UUIDs use 100-nanosecond intervals since UUID epoch (1582-10-15 00:00:00 UTC)
- Conversion from Unix timestamp with proper epoch offset

### Predefined Namespaces
The implementation includes predefined namespace UUIDs for DNS, URL, OID, and X.500 as per RFC 4122.

## Usage Examples

### Basic Usage
```cpp
#include <Engine/Core/Utils/FUUID.hpp>

// Create a random UUID
tkd::UUID randomUUID = tkd::UUID::V4();

// Parse from string
auto parsed = tkd::UUID::Parse("550e8400-e29b-41d4-a716-446655440000");
if (parsed) {
    tkd::UUID uuid = *parsed;
}

// Convert to string
std::string str = uuid.ToString();  // "550e8400-e29b-41d4-a716-446655440000"

// Check properties
if (uuid.IsValid() && !uuid.IsNil()) {
    // Valid non-nil UUID
}
```

### Name-based UUIDs
```cpp
// Generate name-based UUIDs
tkd::UUID dnsUUID = tkd::UUID::DNSV3("example.com");
tkd::UUID urlUUID = tkd::UUID::URLV3("https://example.com");

// Custom namespace
tkd::UUID customNS = tkd::UUID::V4();  // Use as namespace
tkd::UUID nameUUID = tkd::UUID::V5(customNS, "my-identifier");
```

### Comparison and Hashing
```cpp
tkd::UUID uuid1 = tkd::UUID::V4();
tkd::UUID uuid2 = tkd::UUID::V4();

if (uuid1 == uuid2) {
    // Unlikely for random UUIDs
}

if (uuid1 < uuid2) {
    // Lexicographical comparison
}

// Use in unordered containers
std::unordered_map<tkd::UUID, std::string> uuidMap;
uuidMap[uuid1] = "value";
```

### Formatting
```cpp
tkd::UUID uuid = tkd::UUID::V4();

// Modern formatting
std::string formatted = std::format("UUID: {}", uuid);

// Stream output
std::cout << "Generated UUID: " << uuid << std::endl;
```

## Performance Considerations

- UUID generation is fast for all versions
- String parsing and formatting involve memory allocation
- Comparison operations are efficient (byte-wise comparison)
- Hashing uses only the first 8 bytes for speed (may have collisions)

## Thread Safety

- Static generation functions are thread-safe
- Instance methods are const and thread-safe
- No internal mutable state

## Limitations

- MD5/SHA-1 implementations are placeholders and not cryptographically secure
- MAC address retrieval is not fully implemented on all platforms
- No support for version 2 (DCE Security) UUIDs

## Future Improvements

- Integrate proper cryptographic libraries for MD5/SHA-1
- Implement platform-specific MAC address retrieval
- Add version 2 UUID support if needed
- Consider SIMD optimizations for bulk operations
