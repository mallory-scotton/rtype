# FNetworkServer

## Overview

The `FNetworkServer` class is a concrete implementation of `FNetworkBase` designed to manage UDP-based server operations. It handles client connections, disconnections, heartbeats, and broadcasting packets to multiple clients. The server automatically manages connection timeouts, assigns unique client IDs, and emits events for connection state changes.

Key features include:
- Automatic client connection handling
- Heartbeat mechanism for connection monitoring
- Timeout detection and disconnection
- Packet broadcasting to all clients
- Event-driven architecture for connection events
- Thread-safe operations

## Public Members

### Constructor

#### `explicit FNetworkServer(UInt16 port)`

Constructs a network server that will listen on the specified port.

- **Parameters:**
  - `port`: The UDP port to bind the server to.

### Methods

#### `void DisconnectClient(UInt32 clientID, EDisconnectionReason reason = EDisconnectionReason::Unknown)`

Disconnects a client by their ID and sends a disconnect packet with the specified reason.

- **Parameters:**
  - `clientID`: The ID of the client to disconnect.
  - `reason`: The reason for disconnection (default: Unknown).

#### `virtual bool Start(void) override`

Starts the server by binding the socket to the specified port and beginning network operations.

- **Returns:** `true` if the server started successfully, `false` otherwise.

#### `void Update(float deltaTime)`

Updates the server state, checking for timeouts and sending heartbeats. This should be called regularly in the main loop.

- **Parameters:**
  - `deltaTime`: Time elapsed since the last update in seconds.

#### `void BroadcastPacket(const IPacket& packet)`

Sends a packet to all connected clients.

- **Parameters:**
  - `packet`: The packet to broadcast.

#### `bool SendPacketToClient(const IPacket& packet, UInt32 clientID)`

Sends a packet to a specific client by their ID.

- **Parameters:**
  - `packet`: The packet to send.
  - `clientID`: The ID of the client to send the packet to.
- **Returns:** `true` if the packet was sent successfully, `false` otherwise.

#### `SizeT GetConnectedClientCount(void) const`

Returns the number of currently connected clients.

- **Returns:** The count of connected clients.

#### `std::vector<UInt32> GetConnectedClients(void) const`

Returns a list of all connected client IDs.

- **Returns:** A vector containing the IDs of connected clients.

## Events

The `FNetworkServer` emits the following events (inherited from `TEventEmitter`):

- `ServerStarted`: Emitted when the server starts successfully.
  - `port`: The port the server is listening on.
- `ServerStopped`: Emitted when the server stops.
  - `port`: The port the server was listening on.
- `ClientConnected`: Emitted when a client connects.
  - `clientID`: The assigned ID of the connected client.
  - `endpoint`: The endpoint of the connected client.
- `ClientDisconnected`: Emitted when a client disconnects.
  - `clientID`: The ID of the disconnected client.
  - `reason`: The reason for disconnection.

## Enums

### `EDisconnectionReason`

- `Unknown`: No specific reason.
- `Timeout`: Disconnected due to inactivity timeout.
- `Kicked`: Disconnected by the server.
- `Shutdown`: Disconnected due to server shutdown.
- `Error`: Disconnected due to an error.

## Usage Examples

### Creating and Starting a Server

```cpp
#include <Engine/Network/FNetworkServer.hpp>

tkd::FNetworkServer server(12345);  // Listen on port 12345

if (server.Start()) {
    std::cout << "Server started successfully" << std::endl;
} else {
    std::cout << "Failed to start server" << std::endl;
}
```

### Handling Events

```cpp
// Register event handlers
server.On<tkd::FNetworkServer::Events::ClientConnected>(
    [](const auto& event) {
        std::cout << "Client " << event.clientID << " connected from "
                  << event.endpoint << std::endl;
    }
);

server.On<tkd::FNetworkServer::Events::ClientDisconnected>(
    [](const auto& event) {
        std::cout << "Client " << event.clientID << " disconnected: "
                  << static_cast<int>(event.reason) << std::endl;
    }
);
```

### Updating the Server

```cpp
// In main loop
float deltaTime = 0.016f;  // ~60 FPS
server.Update(deltaTime);
```

### Broadcasting Packets

```cpp
#include <Engine/Network/Packets.hpp>  // Assuming a custom packet

Packets::GameState gameState;
// Fill gameState with data
server.BroadcastPacket(gameState);
```

### Sending to Specific Client

```cpp
Packets::PrivateMessage msg;
// Fill msg
if (server.SendPacketToClient(msg, clientID)) {
    // Sent successfully
}
```

### Managing Clients

```cpp
// Get connected client count
SizeT count = server.GetConnectedClientCount();

// Get list of client IDs
auto clients = server.GetConnectedClients();

// Disconnect a client
server.DisconnectClient(clientID, tkd::EDisconnectionReason::Kicked);
```

### Registering Packet Handlers

```cpp
// In addition to default handlers (Connect, Disconnect, HeartBeat)
server.RegisterPacketHandler<Packets::CustomPacket>(
    [](const Packets::CustomPacket& packet, const FEndpoint& sender) {
        // Handle custom packet
    }
);
```

## Notes

- The server automatically handles connection timeouts after 30 seconds of inactivity.
- Heartbeats are sent every 5 seconds to monitor connections.
- Client IDs are assigned sequentially starting from 1.
- The server inherits all public methods from `FNetworkBase`, such as `IsRunning()`, `GetStatistics()`, etc.
