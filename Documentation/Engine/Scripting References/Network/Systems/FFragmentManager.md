# FFragmentManager

## Overview

`FFragmentManager` is a singleton class that manages the fragmentation and reassembly of large network packets. It automatically splits packets exceeding the maximum UDP size, handles reliable transmission with selective retransmission, and reassembles fragments back into complete packets.

This system enables reliable transmission of large data structures (snapshots, RPCs, assets) over UDP without manual packet splitting or loss recovery logic.

---

## Architecture

### Key Components

```
┌─────────────────────────────────────────────────────────────┐
│                    FFragmentManager                         │
│  (Singleton - Manages all fragmentation/reassembly)         │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  ┌──────────────────┐         ┌──────────────────┐         │
│  │ Outgoing Entries │         │ Incoming Entries │         │
│  │ ────────────────-│         │ ────────────────-│         │
│  │ - Original data  │         │ - Received chunks│         │
│  │ - Destinations   │         │ - Sender endpoint│         │
│  │ - ACK tracking   │         │ - Completion flag│         │
│  └──────────────────┘         └──────────────────┘         │
│                                                              │
│  ┌──────────────────────────────────────────────┐          │
│  │            Chunk Management                   │          │
│  │  - Sequence ordering                          │          │
│  │  - Per-endpoint status tracking               │          │
│  │  - Retransmission timers                      │          │
│  └──────────────────────────────────────────────┘          │
└─────────────────────────────────────────────────────────────┘
```

---

## Core Types

### FragmentEntry

Represents a complete fragmented message (either being sent or received).

```cpp
struct FragmentEntry
{
    UUID id;                          // Unique ID for this fragmented message
    UInt32 timestamp;                 // Creation timestamp
    EFragmentDestination destination; // Outgoing or Incoming
    SizeT chunkCount;                 // Total number of chunks
    std::vector<Chunk> chunks;        // The fragments
    
    // Outgoing-specific fields
    std::vector<Byte> original;       // Original serialized data
    std::vector<FEndpoint> destinations; // Where to send
    
    // Incoming-specific fields
    FEndpoint sender;                 // Who sent it
    Bool fullyReceived;               // All chunks received?
};
```

### Chunk

Represents a single fragment within a package.

```cpp
struct Chunk
{
    struct Status
    {
        Bool received;                // Has this chunk been ACK'd?
        UInt32 lastSentTimestamp;     // When was it last sent?
    };
    
    SizeT sequenceOrder;              // Position in the sequence
    std::vector<Byte> data;           // Fragment payload
    std::map<FEndpoint, Status> statuses; // Per-endpoint tracking
};
```

### EFragmentDestination

Indicates whether a fragment entry is being sent or received.

```cpp
enum class EFragmentDestination
{
    Outgoing,  // We are sending this fragmented message
    Incoming   // We are receiving this fragmented message
};
```

---

## Public API

### Sending Large Packets

#### `SendFullTransmission`

Fragments and sends a large packet to one or more destinations.

```cpp
UUID SendFullTransmission(
    const std::vector<Byte>& serializedData,
    const std::vector<FEndpoint>& destinations,
    FNetworkBase* networkBase
);
```

**Parameters:**
- `serializedData` - Already serialized packet data
- `destinations` - List of endpoints to send to
- `networkBase` - Network interface to send fragments through

**Returns:**
- UUID of the created fragment entry

**Example:**

```cpp
// Serialize a large snapshot
Packets::Snapshot snapshot;
snapshot.SetupFromWorld(*world);

std::vector<Byte> data;
FBinaryWriter writer(data);
snapshot.Serialize(writer);

// Send with automatic fragmentation
std::vector<FEndpoint> clients = { endpoint1, endpoint2, endpoint3 };
UUID fragmentID = FragmentManager::Get().SendFullTransmission(
    data,
    clients,
    networkBase
);
```

---

### Processing Incoming Fragments

#### `ProcessFragment`

Handles an incoming fragment packet, stores it, sends ACK, and triggers reassembly when complete.

```cpp
void ProcessFragment(
    const IPacket& packet,
    const FEndpoint& sender,
    FNetworkBase* networkBase
);
```

**Parameters:**
- `packet` - The received fragment packet
- `sender` - Endpoint that sent the fragment
- `networkBase` - Network interface for sending ACK

**Example:**

```cpp
// Typically called automatically by packet handlers
networkBase.RegisterPacketHandler<Packets::Fragment>(
    [](const Packets::Fragment& fragment, const FEndpoint& sender) {
        FragmentManager::Get().ProcessFragment(fragment, sender, networkBase);
    }
);
```

---

### Update Loop

#### `Update`

Handles retransmission of unacknowledged fragments and cleanup of completed entries.

```cpp
void Update(Float32 deltaTime, FNetworkBase* networkBase);
```

**Parameters:**
- `deltaTime` - Time elapsed since last update (seconds)
- `networkBase` - Network interface for retransmissions

**Example:**

```cpp
// In FNetworkServer::Update() or FNetworkClient::Update()
void Update(float deltaTime) {
    // ... other updates ...
    
    FragmentManager::Get().Update(deltaTime, this);
}
```

---

## Internal Operations

### Fragmentation Process

When `SendFullTransmission` is called:

1. **Create Entry**
   ```cpp
   FragmentEntry entry;
   entry.id = UUID::Generate();
   entry.destination = EFragmentDestination::Outgoing;
   entry.original = serializedData;
   entry.destinations = destinations;
   ```

2. **Split Data**
   ```cpp
   std::vector<std::vector<Byte>> chunks = FragmentPacket(serializedData);
   // Splits into MAX_FRAGMENT_SIZE (512 byte) chunks
   ```

3. **Create Chunks**
   ```cpp
   for (size_t i = 0; i < chunks.size(); ++i) {
       Chunk chunk;
       chunk.sequenceOrder = i;
       chunk.data = chunks[i];
       
       // Initialize status for each destination
       for (const auto& dest : destinations) {
           chunk.statuses[dest] = { false, 0 };
       }
       
       entry.chunks.push_back(chunk);
   }
   ```

4. **Send All Fragments**
   ```cpp
   for (const auto& dest : destinations) {
       for (size_t i = 0; i < entry.chunks.size(); ++i) {
           SendFragmentPacket(entry, i, dest, networkBase);
       }
   }
   ```

### Reassembly Process

When fragments are received:

1. **Store Fragment**
   ```cpp
   auto* entry = FindOrCreateEntry(fragment.PackageID, sender);
   entry->chunks[fragment.SequenceID].data = fragment.data;
   entry->chunks[fragment.SequenceID].statuses[sender].received = true;
   ```

2. **Send ACK**
   ```cpp
   Packets::FragmentAcknowledgment ack;
   ack.PackageID = fragment.PackageID;
   ack.FragmentID = fragment.SequenceID;
   networkBase->SendPacket(ack, sender);
   ```

3. **Check Completion**
   ```cpp
   if (IsFragmentComplete(entry)) {
       MergeFragments(entry, networkBase);
   }
   ```

4. **Merge & Deserialize**
   ```cpp
   void MergeFragments(FragmentEntry& entry, FNetworkBase* networkBase) {
       SortFragments(entry); // Sort by sequenceOrder
       
       // Concatenate all chunk data
       std::vector<Byte> completeData;
       for (const auto& chunk : entry.chunks) {
           completeData.insert(completeData.end(), 
                              chunk.data.begin(), 
                              chunk.data.end());
       }
       
       // Deserialize and process original packet
       FBinaryReader reader(completeData);
       IPacket* packet = DeserializePacket(reader);
       networkBase->ProcessPacket(*packet, entry.sender);
       
       // Clean up
       MarkFragmentForDeletion(entry.id);
   }
   ```

### Retransmission Logic

In `Update()`, for each outgoing entry:

```cpp
for (auto& entry : m_fragments) {
    if (entry.destination != EFragmentDestination::Outgoing) continue;
    
    UInt32 currentTime = GetCurrentTimestamp();
    
    for (size_t i = 0; i < entry.chunks.size(); ++i) {
        auto& chunk = entry.chunks[i];
        
        for (auto& [endpoint, status] : chunk.statuses) {
            // Check if ACK timeout expired
            if (!status.received && 
                (currentTime - status.lastSentTimestamp) > ACK_TIMEOUT * 1000) {
                
                // Retransmit only this chunk to this endpoint
                SendFragmentPacket(entry, i, endpoint, networkBase);
                status.lastSentTimestamp = currentTime;
                
                FLogger::Debug(
                    "Retransmitting fragment {} chunk {} to {}",
                    entry.id.ToString(),
                    i,
                    endpoint.address().to_string()
                );
            }
        }
    }
}
```

---

## Configuration

### Constants

```cpp
class FragmentManager {
public:
    // Maximum payload size per fragment
    static constexpr SizeT MAX_FRAGMENT_SIZE = 512;
    
    // Time to wait for ACK before retransmission (500ms)
    static constexpr Float32 ACK_TIMEOUT = 0.5f;
    
    // Time to keep completed fragments before cleanup (1 second)
    static constexpr Float32 DELETION_TIMEOUT = 1.0f;
};
```

### Tuning Guidelines

| Network Condition | MAX_FRAGMENT_SIZE | ACK_TIMEOUT | DELETION_TIMEOUT |
|-------------------|-------------------|-------------|------------------|
| LAN (< 5ms) | 1024 bytes | 0.1s | 0.5s |
| Good Internet | 512 bytes | 0.5s | 1.0s |
| High Latency | 512 bytes | 1.0s | 2.0s |
| Poor Connection | 256 bytes | 2.0s | 5.0s |

**Reasoning:**
- **MAX_FRAGMENT_SIZE**: Smaller = more overhead but less loss per packet
- **ACK_TIMEOUT**: Higher = less retransmissions but slower recovery
- **DELETION_TIMEOUT**: Higher = more memory but safer for debugging

---

## Usage Examples

### Example 1: Sending Large Snapshot

```cpp
void FNetworkServer::SendWorldSnapshot(UInt32 clientID) {
    FWorldInterface::WithWorld([this, clientID](UWorld& world) {
        // Create snapshot
        Packets::Snapshot snapshot;
        snapshot.SetupFromWorld(world);
        
        // Serialize
        std::vector<Byte> data;
        FBinaryWriter writer(data);
        snapshot.Serialize(writer);
        
        // Get client endpoint
        auto endpoint = GetClientEndpoint(clientID);
        
        // Send with automatic fragmentation if needed
        if (data.size() > 1400) { // Likely needs fragmentation
            FragmentManager::Get().SendFullTransmission(
                data,
                { endpoint },
                this
            );
        } else {
            SendPacket(snapshot, endpoint); // Send directly
        }
    });
}
```

### Example 2: Multicast to Multiple Clients

```cpp
void FNetworkServer::BroadcastSnapshot() {
    // Get all connected clients
    std::vector<FEndpoint> clients = GetAllClientEndpoints();
    
    // Serialize once
    Packets::Snapshot snapshot(*GetWorld());
    std::vector<Byte> data;
    FBinaryWriter writer(data);
    snapshot.Serialize(writer);
    
    // Send to all clients with ONE fragment entry
    // (FragmentManager tracks ACKs per-client)
    UUID fragmentID = FragmentManager::Get().SendFullTransmission(
        data,
        clients,
        this
    );
    
    FLogger::Info("Broadcasting snapshot {} to {} clients",
                  fragmentID.ToString(),
                  clients.size());
}
```

### Example 3: Custom Packet Handler Setup

```cpp
void FNetworkBase::InitializePacketManager() {
    // Fragment handler
    RegisterPacketHandler<Packets::Fragment>(
        [this](const Packets::Fragment& packet, const FEndpoint& sender) {
            FragmentManager::Get().ProcessFragment(packet, sender, this);
        }
    );
    
    // Fragment ACK handler
    RegisterPacketHandler<Packets::FragmentAcknowledgment>(
        [this](const Packets::FragmentAcknowledgment& ack, const FEndpoint& sender) {
            auto* entry = FragmentManager::Get().FindFragmentEntry(ack.PackageID);
            if (entry && ack.FragmentID < entry->chunks.size()) {
                entry->chunks[ack.FragmentID].statuses[sender].received = true;
            }
        }
    );
}
```

---

## Performance & Optimization

### Memory Usage

**Per Fragment Entry:**
- Fixed: ~64 bytes (UUID, timestamps, counts)
- Variable: `original.size()` + `chunkCount × (512 + 64)` bytes
- Example: 3000-byte packet = 3000 + (6 × 576) = ~6500 bytes

**Optimization Tips:**
1. Call `DestroyFragments()` regularly to clean up completed entries
2. Set appropriate `DELETION_TIMEOUT` based on your game's needs
3. Monitor `m_fragments.size()` to detect memory leaks

### Bandwidth Usage

**Overhead Calculation:**
```
Per Fragment:
  - Packet header: ~20 bytes
  - Fragment fields: 10 bytes (PackageID + SequenceID + FragmentCount)
  - Total overhead: ~30 bytes

Per ACK:
  - Packet header: ~20 bytes
  - ACK fields: 8 bytes
  - Total overhead: ~28 bytes

Example (3000-byte packet, 6 fragments):
  - Data: 3000 bytes
  - Fragment overhead: 6 × 30 = 180 bytes
  - ACK overhead: 6 × 28 = 168 bytes
  - Total: 3348 bytes (11.6% overhead)
```

### CPU Usage

**Hotspots:**
- `Update()`: O(fragments × chunks × endpoints) per frame
- `MergeFragments()`: O(chunks) on completion
- `SortFragments()`: O(chunks log chunks) on completion

**Optimization:**
- Limit maximum concurrent fragment entries
- Use deferred deletion to avoid mid-frame cleanup
- Batch ACK processing

---

## Debugging

### Logging

Enable fragment debugging:

```cpp
#define FRAGMENT_DEBUG_LOGGING 1

// In FFragmentManager.cpp
FLogger::SetNamespace("FragmentManager");
FLogger::Debug("Fragment {} chunk {} sent to {} (attempt {})",
               entry.id.ToString(),
               chunkIndex,
               endpoint.address().to_string(),
               retransmitCount);
```

### Metrics

Track fragment performance:

```cpp
struct FragmentMetrics {
    UInt32 totalFragmentsSent = 0;
    UInt32 totalFragmentsReceived = 0;
    UInt32 totalRetransmissions = 0;
    Float32 averageReassemblyTime = 0.0f;
    UInt32 activeEntries = 0;
};

FragmentMetrics GetMetrics() const;
```

### Common Issues

| Issue | Symptom | Solution |
|-------|---------|----------|
| Memory leak | `m_fragments` grows infinitely | Ensure `Update()` is called regularly |
| High retransmissions | Excessive bandwidth usage | Increase `ACK_TIMEOUT` or reduce packet size |
| Stuck fragments | Never completes | Check network connectivity, increase timeouts |
| Out of order | Incorrect reassembly | `SortFragments()` should be called before merge |

---

## See Also

- [Fragment Packet Documentation](../Packets/fragmentation.md) - Packet format details
- [FNetworkBase](./FNetworkBase.md) - Underlying network system
- [Snapshot System](../Packets/Snapshot.md) - Common use case for fragmentation
