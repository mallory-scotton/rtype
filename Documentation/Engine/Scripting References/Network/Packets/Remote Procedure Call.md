# Remote Procedure Call Packet

## Overview

Remote Procedure Calls (RPCs) are a fundamental mechanism in distributed systems and multiplayer games for executing functions across network boundaries. The **Remote Procedure Call packet** (`RemoteProcedureCall`) in the TKD Engine enables seamless invocation of methods on remote actors, abstracting away the complexities of network communication and serialization.

RPCs allow game logic to be written as if all objects exist locally, while the underlying system handles network transmission, parameter serialization, and execution on the appropriate remote endpoints.

## Purpose

The `RemoteProcedureCall` packet is designed to:
- Enable distributed function execution across network peers
- Support different RPC patterns (Server, Client, Multicast)
- Handle parameter serialization and deserialization
- Maintain type safety and reliability
- Integrate with the actor system for object-oriented networking
- Support both reliable and unreliable transmission

## Class Description

```cpp
namespace tkd::Packets
{
    class RemoteProcedureCall : public TPacket<RemoteProcedureCall>
    {
    public:
        std::array<Byte, 16> actorID;          // The ID of the actor
        ERPCType rpcType = ERPCType::Server;   // The type of RPC
        FString functionName;                  // The name of the function
        std::vector<Byte> parameters;          // Serialized parameters

        RemoteProcedureCall(void) = default;
        RemoteProcedureCall(const FString& name, ERPCType type, const UUID& uuid = UUID::Nil);
        template <typename... Args>
        RemoteProcedureCall(const FString& name, ERPCType type, const UUID& uuid, Args&&... params);

        bool Serialize(FBinaryWriter& writer) const override;
        bool Deserialize(FBinaryReader& reader) override;
        SizeT GetSize(void) const override;
    };
}
```

### Inheritance Hierarchy

```
IPacket (abstract interface)
├── TPacket<RemoteProcedureCall> (template base)
    └── RemoteProcedureCall (concrete implementation)
```

## Member Variables

### actorID
```cpp
std::array<Byte, 16> actorID;
```
- **Type**: `std::array<Byte, 16>`
- **Description**: UUID of the target actor in raw byte form
- **Size**: 16 bytes (standard UUID size)
- **Default Value**: Uninitialized (use `UUID::Nil` for default)
- **Usage**: Identifies which actor should execute the RPC

### rpcType
```cpp
ERPCType rpcType = ERPCType::Server;
```
- **Type**: `ERPCType` (enumeration)
- **Description**: Specifies the direction and scope of the RPC call
- **Default Value**: `ERPCType::Server`
- **Possible Values**:
  - `ERPCType::None`: Not an RPC (local execution only)
  - `ERPCType::Server`: Client-to-server call
  - `ERPCType::Client`: Server-to-client call
  - `ERPCType::Multicast`: Server-to-all-clients broadcast

### functionName
```cpp
FString functionName;
```
- **Type**: `FString`
- **Description**: Name of the function to be executed on the remote actor
- **Default Value**: Empty string
- **Usage**: Function lookup and execution dispatch

### parameters
```cpp
std::vector<Byte> parameters;
```
- **Type**: `std::vector<Byte>`
- **Description**: Serialized function parameters in binary form
- **Default Value**: Empty vector
- **Usage**: Contains all function arguments serialized for network transmission

## Constructors

### Default Constructor
```cpp
RemoteProcedureCall(void) = default;
```
- **Description**: Creates an empty RPC packet with default values
- **Initialization**:
  - `actorID`: Uninitialized
  - `rpcType`: `ERPCType::Server`
  - `functionName`: Empty
  - `parameters`: Empty

### Parameter Constructor (No Arguments)
```cpp
RemoteProcedureCall(const FString& name, ERPCType type, const UUID& uuid = UUID::Nil);
```
- **Description**: Creates an RPC packet for a function with no parameters
- **Parameters**:
  - `name`: Function name to call
  - `type`: RPC type (Server, Client, Multicast)
  - `uuid`: Target actor UUID (defaults to nil UUID)
- **Usage**: For parameterless RPC functions

### Parameter Constructor (With Arguments)
```cpp
template <typename... Args>
RemoteProcedureCall(const FString& name, ERPCType type, const UUID& uuid, Args&&... params);
```
- **Description**: Creates an RPC packet with serialized parameters
- **Template Parameters**: `Args...` - Variadic template for any parameter types
- **Parameters**:
  - `name`: Function name to call
  - `type`: RPC type
  - `uuid`: Target actor UUID
  - `params`: Function arguments (automatically serialized)
- **Usage**: For RPC functions with parameters

## Methods

### Serialize
```cpp
bool Serialize(FBinaryWriter& writer) const override;
```
- **Description**: Serializes the RPC packet into binary format for network transmission
- **Parameters**:
  - `writer`: Binary writer for output serialization
- **Returns**: `true` if serialization succeeds, `false` otherwise
- **Serialization Order**:
  1. `actorID` (16 bytes)
  2. `rpcType` (1 byte, cast to UInt8)
  3. `functionName` (string)
  4. `parameters` (byte array)

### Deserialize
```cpp
bool Deserialize(FBinaryReader& reader) override;
```
- **Description**: Deserializes the RPC packet from binary format
- **Parameters**:
  - `reader`: Binary reader for input deserialization
- **Returns**: `true` if deserialization succeeds, `false` otherwise
- **Deserialization Order**: Matches serialization order
- **Error Handling**: Returns `false` if any read operation fails

### GetSize
```cpp
SizeT GetSize(void) const override;
```
- **Description**: Calculates the total size of the packet in bytes
- **Returns**: Total packet size
- **Calculation**:
  - `actorID.size()` (16)
  - `sizeof(UInt8)` for rpcType (1)
  - `functionName.Size()` (string length + overhead)
  - `parameters.size()` (parameter data)
  - Additional size fields (2 × `sizeof(SizeT)`)

## RPC Types and Execution Model

### ERPCType::Server
- **Direction**: Client → Server
- **Execution**: Server validates and executes the RPC
- **Use Case**: Client requests server-side actions (movement validation, item pickup)
- **Security**: Server validates all parameters and permissions

### ERPCType::Client
- **Direction**: Server → Specific Client
- **Execution**: Target client executes the RPC
- **Use Case**: Server sending updates to individual clients (private messages, client-specific events)
- **Targeting**: Uses `actorID` to identify recipient

### ERPCType::Multicast
- **Direction**: Server → All Clients
- **Execution**: All connected clients execute the RPC
- **Use Case**: Broadcast events (game state changes, global announcements)
- **Efficiency**: Single packet sent, replicated to all clients

## Packet Structure

```
Remote Procedure Call Packet
├── Packet Header (16 bytes)
│   ├── Type ID: RPC packet type
│   ├── Sequence: Packet sequence number
│   ├── Timestamp: Send timestamp
│   └── Size: Variable (calculated)
└── RPC Data (variable size)
    ├── actorID: 16 bytes (UUID)
    ├── rpcType: 1 byte (ERPCType)
    ├── functionName: variable (FString)
    └── parameters: variable (serialized args)
```

## Integration with UFunction

The RPC system integrates deeply with the `UFunction` class for seamless remote execution:

### UFunction RPC Declaration
```cpp
class APlayer : public AActor {
public:
    UFunction<void(int, float)> TakeDamage;  // Server RPC
    UFunction<void()> Respawn;               // Multicast RPC
};
```

### RPC Execution Flow
```
Local Call                    Network Transmission              Remote Execution
    │                                │                                │
    │  player.TakeDamage(50, 1.0f)  │                                │
    │ ─────────────────────────────► │  Serialize → Send RPC Packet   │
    │                                │ ─────────────────────────────► │
    │                                │                                │  Deserialize → Execute
    │                                │                                │ ─────────────────────►
```

## Usage Examples

### Basic Server RPC
```cpp
// Define RPC function
UFunction<void(int)> TakeDamage(player, "TakeDamage", ERPCType::Server);

// Client calls server function
player.TakeDamage(50);  // Automatically sends RPC to server
```

### RPC with Parameters
```cpp
// Server-side function
void APlayer::TakeDamage(int damage, float multiplier) {
    int actualDamage = damage * multiplier;
    health -= actualDamage;
}

// Client calls with parameters
player.TakeDamage(25, 1.5f);  // damage=25, multiplier=1.5
```

### Multicast RPC
```cpp
// Define multicast function
UFunction<void()> PlayerDied(player, "PlayerDied", ERPCType::Multicast);

// Server broadcasts to all clients
player.PlayerDied();  // All clients execute PlayerDied()
```

### Manual RPC Creation
```cpp
// Create RPC packet manually
Packets::RemoteProcedureCall rpc(
    "FireWeapon",                    // Function name
    ERPCType::Server,                // RPC type
    player.GetUUID(),                // Target actor
    weaponID,                        // Parameter 1
    ammoCount                        // Parameter 2
);

// Send reliably
Network::SendReliablePacket(rpc);
```

### Handling RPC Parameters
```cpp
// Function with complex parameters
UFunction<void(FVector3, FRotator)> SetTransform(
    actor, "SetTransform", ERPCType::Client
);

// Usage
FVector3 position(100.0f, 200.0f, 0.0f);
FRotator rotation(0.0f, 90.0f, 0.0f);
actor.SetTransform(position, rotation);
```

## Serialization and Type Safety

### Automatic Parameter Serialization
```cpp
// Template constructor handles any serializable types
RemoteProcedureCall rpc("MoveTo", ERPCType::Server, actorID,
                       position, rotation, speed);
```

### Supported Parameter Types
- **Primitive Types**: `int`, `float`, `bool`, `char`
- **Engine Types**: `FVector3`, `FRotator`, `FColor`, `UUID`
- **Containers**: `std::vector<T>`, `std::array<T, N>`
- **Strings**: `FString`, `std::string`
- **Custom Types**: Any type with `Serialize`/`Deserialize` methods

### Type Safety Guarantees
- Compile-time parameter checking
- Runtime deserialization validation
- Automatic size calculation
- Endianness handling

## Network Transmission Modes

### Reliable RPCs
```cpp
// Mark function as reliable
UFunction<void()> CriticalAction(player, "CriticalAction",
                                ERPCType::Server, nullptr, true);

// Reliable transmission ensures delivery
player.CriticalAction();  // Uses ACK system
```

### Unreliable RPCs
```cpp
// Default unreliable transmission
UFunction<void()> UpdatePosition(player, "UpdatePosition",
                                ERPCType::Multicast);

// Fast but potentially lossy
player.UpdatePosition();  // No retransmission
```

## Error Handling and Validation

### RPC Validation
```cpp
void ExecuteRPC(const RemoteProcedureCall& rpc) {
    // Validate actor exists
    AActor* actor = FindActorByID(rpc.actorID);
    if (!actor) return;

    // Validate function exists
    IFunction* function = actor->FindFunction(rpc.functionName);
    if (!function) return;

    // Validate RPC permissions
    if (!ValidateRPCPermissions(rpc, actor)) return;

    // Execute with deserialized parameters
    function->ExecuteSerialized(rpc.parameters);
}
```

### Network Error Handling
- **Connection Loss**: RPCs queued for reconnection
- **Timeout**: Reliable RPCs retransmitted
- **Deserialization Failure**: RPC discarded with logging
- **Invalid Actor ID**: RPC ignored
- **Permission Denied**: RPC rejected

## Performance Considerations

- **Serialization Overhead**: Parameter serialization cost
- **Network Bandwidth**: Packet size impacts transmission
- **Execution Latency**: Network round-trip time
- **Memory Usage**: Parameter buffers and queues
- **CPU Usage**: Serialization/deserialization overhead

### Optimization Strategies
- **Parameter Packing**: Minimize data types
- **Frequent Updates**: Use unreliable RPCs for non-critical data
- **Batching**: Combine multiple RPCs when possible
- **Compression**: Compress large parameter sets
- **Caching**: Cache serialized parameters for repeated calls

## Security Considerations

### Server Authority
- Server validates all `ERPCType::Server` RPCs
- Client input sanitization
- Rate limiting and spam prevention
- Permission checking per actor/function

### Client Validation
- Server can reject invalid RPCs
- Parameter range checking
- Anti-cheat measures
- Session validation

## Debugging and Monitoring

### RPC Logging
```cpp
// Enable RPC debug logging
FLogger::SetNamespace("RPC");
FLogger::Debug("Executing RPC '{}' on actor {}",
              rpc.functionName.CStr(), UUID(rpc.actorID).ToString());
```

### Network Profiler Integration
```cpp
// Track RPC statistics
struct RPCStatistics {
    UInt32 rpcsSent;
    UInt32 rpcsReceived;
    UInt32 rpcsFailed;
    Float32 averageLatency;
    // ... detailed metrics
};
```

## Related Classes and Systems

- **`UFunction<T...>`**: Function wrapper with RPC support
- **`AActor`**: Base class for networked objects
- **`FNetworkBase`**: Core networking system
- **`IPacket`**: Packet interface
- **`FBinaryWriter/FBinaryReader`**: Serialization utilities
- **`ERPCType`**: RPC type enumeration

## Diagram: RPC Execution Flow

```mermaid
sequenceDiagram
    participant Client
    participant Server
    participant Actor

    Client->>Client: Local RPC Call
    Client->>Client: Serialize Parameters
    Client->>Server: Send RPC Packet
    Server->>Server: Deserialize Packet
    Server->>Server: Validate Permissions
    Server->>Actor: Find Target Actor
    Server->>Actor: Execute Function
    Actor->>Server: Function Result
    Server->>Client: Send Response (if needed)
```

## Diagram: RPC Packet Memory Layout

```
RemoteProcedureCall Packet
├── Packet Header (16 bytes)
│   ├── Type ID (2B)
│   ├── Sequence (4B)
│   ├── Timestamp (4B)
│   └── Data Size (4B+)
└── RPC Payload (variable)
    ├── Actor ID (16B)
    │   └── UUID bytes
    ├── RPC Type (1B)
    │   └── ERPCType enum
    ├── Function Name (variable)
    │   ├── Length (SizeT)
    │   └── String data
    └── Parameters (variable)
        ├── Count (SizeT)
        └── Serialized args
```

## Best Practices

1. **RPC Type Selection**: Choose appropriate RPC type for security and performance
2. **Parameter Design**: Keep parameters simple and serializable
3. **Reliability**: Use reliable RPCs only when necessary
4. **Validation**: Always validate server-side RPCs
5. **Error Handling**: Implement robust error handling
6. **Performance**: Profile and optimize frequent RPCs
7. **Security**: Validate all client-initiated RPCs
8. **Documentation**: Document RPC functions and parameters
9. **Testing**: Test RPCs under various network conditions
10. **Monitoring**: Monitor RPC usage and performance

This documentation provides a comprehensive overview of the Remote Procedure Call system in the TKD Engine, covering its implementation, usage patterns, and integration with the broader networking architecture.
