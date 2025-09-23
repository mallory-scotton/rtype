# HeartBeat Packet

## Overview

The `HeartBeat` packet is used to maintain and monitor network connections in UDP-based communication. Since UDP is a connectionless protocol, heartbeats serve as keep-alive signals to detect connection health, measure latency, and prevent timeouts. The server sends heartbeats periodically to all connected clients, and clients can respond or simply acknowledge receipt to update their last activity timestamp.

This packet is automatically managed by the `FNetworkServer` class, which sends heartbeats every 5 seconds and uses them to track connection liveness.

## Fields

### `UInt32 id`

A unique identifier for the heartbeat packet.

- **Use:** Allows tracking of specific heartbeat instances, useful for debugging or measuring round-trip times.

### `UInt32 timestamp`

The timestamp when the heartbeat was sent.

- **Use:** Enables latency calculation by comparing send and receive times.

## Usage in UDP

UDP does not provide built-in connection state management, so heartbeats are crucial for:

### Connection Monitoring

- **Server sends heartbeats** to clients at regular intervals (default: 5 seconds).
- **Clients receive heartbeats** and update their last activity time.
- **Server detects timeouts** if no response/activity within the timeout period (default: 30 seconds).

### Latency Measurement

```cpp
// On client
UInt32 sendTime = GetCurrentTimestamp();
server.SendPacket(heartbeat, serverEndpoint);

// When received back (if echoed)
UInt32 receiveTime = GetCurrentTimestamp();
UInt32 latency = receiveTime - heartbeat.timestamp;
```

### Keep-Alive Mechanism

```cpp
// Server side (automatic in FNetworkServer)
void SendHeartbeats() {
    for (auto& connection : m_connections) {
        if (connection.connected) {
            Packets::HeartBeat heartbeat;
            heartbeat.id = ++pingId;
            heartbeat.timestamp = GetCurrentTimestamp();
            SendPacket(heartbeat, connection.endpoint);
        }
    }
}
```

### Client Response (Optional)

Clients can respond to heartbeats to confirm liveness:

```cpp
networkClient.RegisterPacketHandler<Packets::HeartBeat>(
    [this](const Packets::HeartBeat& packet, const FEndpoint& sender) {
        // Update last server contact time
        lastServerHeartbeat = GetCurrentTimestamp();

        // Optional: Echo back for latency measurement
        SendPacket(packet, sender);
    }
);
```

## Notes

- Heartbeats are sent by the server only; clients typically don't send them.
- The `id` field increments with each heartbeat, useful for detecting lost packets.
- Timestamps use milliseconds since epoch for precision.
- In high-latency networks, adjust heartbeat intervals to prevent false timeouts.
- Heartbeats contribute to network statistics (packets sent/received).
