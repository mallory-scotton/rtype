# FPacketManager

## Overview

The `FPacketManager` class is responsible for managing the serialization, deserialization, and registration of network packets. It handles packet factories, assigns sequence numbers, calculates checksums for integrity, and validates incoming packets. This class is central to the network communication system, ensuring reliable and type-safe packet handling.

Key features:
- Automatic packet type registration
- Serialization with header generation
- Deserialization with validation
- Checksum calculation for data integrity
- Sequence number management

## Public Members

### Methods

#### `template <typename T> void RegisterPacket(void)`

Registers a packet type with the manager, allowing it to create instances of that packet during deserialization.

- **Template Parameters:**
  - `T`: The packet type to register, must derive from `IPacket`.
- **Requirements:** `T` must have a static `GetStaticType()` method (provided by `TPacket`).

#### `std::vector<UInt8> SerializePacket(const IPacket& packet)`

Serializes a packet into a byte buffer, including the packet header with metadata.

- **Parameters:**
  - `packet`: The packet to serialize.
- **Returns:** A vector containing the serialized packet data, or empty vector on failure.
- **Process:**
  - Creates and populates `FPacketHeader`
  - Serializes header fields
  - Serializes packet data
  - Calculates and inserts checksum

#### `std::unique_ptr<IPacket> DeserializePacket(const UInt8* data, SizeT size, FPacketHeader& outHeader)`

Deserializes a packet from raw byte data, validating the header and checksum.

- **Parameters:**
  - `data`: Pointer to the raw packet data.
  - `size`: Size of the data in bytes.
  - `outHeader`: Reference to store the deserialized header.
- **Returns:** A unique pointer to the deserialized packet, or nullptr on failure.
- **Validation:**
  - Magic number
  - Protocol version
  - Packet size
  - Checksum

## Usage Examples

### Registering Packets

```cpp
#include <Engine/Network/FPacketManager.hpp>
#include <Engine/Network/Packets.hpp>  // Assuming predefined packets

FPacketManager manager;

// Register built-in packets
manager.RegisterPacket<Packets::Connect>();
manager.RegisterPacket<Packets::Disconnect>();
manager.RegisterPacket<Packets::HeartBeat>();

// Register custom packet
manager.RegisterPacket<MyCustomPacket>();
```

### Serializing a Packet

```cpp
Packets::Connect connectPacket;
// Fill packet data

std::vector<UInt8> serializedData = manager.SerializePacket(connectPacket);
if (!serializedData.empty()) {
    // Send serializedData over network
    socket.send_to(asio::buffer(serializedData), endpoint);
}
```

### Deserializing a Packet

```cpp
// Received data
const UInt8* receivedData = buffer.data();
SizeT receivedSize = buffer.size();

FPacketHeader header;
auto packet = manager.DeserializePacket(receivedData, receivedSize, header);
if (packet) {
    // Handle the packet based on type
    switch (header.packetType) {
        case Packets::Connect::GetStaticType():
            // Handle connect
            break;
        // etc.
    }
} else {
    // Invalid packet
}
```

### Integration with Network Classes

```cpp
// In FNetworkBase or derived classes
void InitializePacketManager() {
    m_packetManager.RegisterPacket<Packets::Connect>();
    m_packetManager.RegisterPacket<Packets::ConnectResponse>();
    // etc.
}

// When sending
bool SendPacket(const IPacket& packet, const FEndpoint& endpoint) {
    auto data = m_packetManager.SerializePacket(packet);
    if (data.empty()) return false;
    // Send data...
}

// When receiving
void ProcessReceivedData(const UInt8* data, SizeT size, const FEndpoint& sender) {
    FPacketHeader header;
    auto packet = m_packetManager.DeserializePacket(data, size, header);
    if (packet) {
        // Route to handler
        auto it = m_packetHandlers.find(header.packetType);
        if (it != m_packetHandlers.end()) {
            it->second(*packet, sender);
        }
    }
}
```

## Notes

- Packets must be registered before they can be deserialized.
- The manager automatically assigns sequence numbers to outgoing packets.
- Checksum validation ensures data integrity but may impact performance for large packets.
- The checksum calculation uses a CRC32-like algorithm.
- Timestamps are in milliseconds since epoch.
- Failed serialization/deserialization returns empty/null results for error handling.
