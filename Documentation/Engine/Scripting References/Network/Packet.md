# Packet System

## Overview

The TKD Engine's packet system provides a structured way to serialize and deserialize data for network communication. It consists of an interface (`IPacket`), a template class (`TPacket`) for automatic type management, and a packet header (`FPacketHeader`) for metadata. This system ensures type safety, efficient serialization, and reliable packet routing.

Key components:
- `IPacket`: Abstract interface for all packets
- `TPacket<Derived>`: Template helper that assigns unique type IDs
- `FPacketHeader`: Structure containing packet metadata

## IPacket Interface

The `IPacket` class is the base interface that all network packets must implement.

### Methods

#### `virtual UInt16 GetType(void) const = 0`

Returns the unique type identifier for the packet.

- **Returns:** The packet type ID.

#### `virtual bool Serialize(FBinaryWriter& writer) const = 0`

Serializes the packet's data into a binary writer.

- **Parameters:**
  - `writer`: The binary writer to serialize data into.
- **Returns:** `true` if serialization succeeded, `false` otherwise.

#### `virtual bool Deserialize(FBinaryReader& reader) = 0`

Deserializes the packet's data from a binary reader.

- **Parameters:**
  - `reader`: The binary reader to deserialize data from.
- **Returns:** `true` if deserialization succeeded, `false` otherwise.

#### `virtual SizeT GetSize(void) const = 0`

Returns the size of the packet's data in bytes (excluding header).

- **Returns:** The size of the packet data in bytes.

## TPacket Template

The `TPacket<Derived>` template class inherits from `IPacket` and automatically assigns a unique type ID to each derived packet class.

### Usage

```cpp
class MyPacket : public TPacket<MyPacket> {
    // Implement Serialize, Deserialize, GetSize
};
```

This ensures that each packet type has a unique ID without manual management.

## FPacketHeader

The `FPacketHeader` struct contains metadata for each packet, used for validation, routing, and integrity checking.

### Fields

#### `UInt32 magic`

A magic number used to identify valid packets and detect corruption.

- **Value:** `0xDEADBEEF`
- **Use:** Checked upon receipt to ensure the data is a valid packet.

#### `UInt32 protocolVersion`

The version of the network protocol.

- **Value:** `1` (defined as `PROTOCOL_VERSION`)
- **Use:** Ensures compatibility between client and server versions.

#### `UInt16 flags`

Flags for additional packet information.

- **Default:** `0` (None)
- **Use:** Reserved for future extensions like compression or encryption flags.

#### `UInt16 packetType`

The type identifier of the packet.

- **Use:** Determines how to deserialize and handle the packet. Corresponds to `IPacket::GetType()`.

#### `UInt16 packetSize`

The size of the packet data in bytes (excluding header).

- **Use:** Used to allocate buffers and validate received data.

#### `UInt32 sequenceNumber`

A sequence number for ordering packets.

- **Use:** Helps detect lost packets and maintain order in unreliable protocols like UDP.

#### `UInt32 timestamp`

Timestamp when the packet was sent.

- **Use:** For latency calculation, debugging, and timeout detection.

#### `UInt32 checksum`

Checksum for data integrity verification.

- **Use:** Ensures the packet data hasn't been corrupted during transmission.

### Constants

- `SIZE`: Total size of the header in bytes (28 bytes).

## Usage Examples

### Creating a Custom Packet

```cpp
#include <Engine/Network/TPacket.hpp>
#include <Engine/Network/FBinaryWriter.hpp>
#include <Engine/Network/FBinaryReader.hpp>

class PlayerPosition : public TPacket<PlayerPosition> {
public:
    float x, y, z;

    bool Serialize(FBinaryWriter& writer) const override {
        writer.Write(x);
        writer.Write(y);
        writer.Write(z);
        return true;
    }

    bool Deserialize(FBinaryReader& reader) override {
        return reader.Read(x) && reader.Read(y) && reader.Read(z);
    }

    SizeT GetSize(void) const override {
        return sizeof(float) * 3;
    }
};
```

### Using Packets with Network Classes

```cpp
// On the server
PlayerPosition pos{1.0f, 2.0f, 3.0f};
server.SendPacketToClient(pos, clientID);

// Packet will be automatically serialized with header
```

### Packet Header Inspection

```cpp
// When receiving raw data
FPacketHeader header;
// Deserialize header first
if (reader.Read(header.magic) && header.magic == MAGIC_NUMBER) {
    // Valid packet
    if (header.protocolVersion == PROTOCOL_VERSION) {
        // Compatible version
        // Proceed to deserialize packet data
    }
}
```

## Notes

- All packets must inherit from `TPacket<Derived>` to get automatic type assignment.
- The header is always sent before packet data.
- Packet types are assigned sequentially starting from 1.
- Implementations should handle endianness if communicating across different architectures.
- The checksum field is currently unused but reserved for future integrity checks.
