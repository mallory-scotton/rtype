# Disconnect Packet

## Overview

The `Disconnect` packet is used to notify disconnection from the network. It can be sent by either a client to gracefully disconnect from the server, or by the server to forcibly disconnect a client. The packet includes the client ID and a reason code for the disconnection.

This packet is automatically handled by the `FNetworkServer` for server-initiated disconnections and can be sent by clients for clean disconnections.

## Fields

### `UInt32 clientID`

The ID of the client being disconnected.

- **Use:** Identifies which client is disconnecting or being disconnected.

### `UInt32 reason`

A code indicating the reason for disconnection.

- **Use:** Provides context for why the disconnection occurred. Can correspond to `EDisconnectionReason` enum values.

## Usage

### Client Side (Graceful Disconnect)

```cpp
#include <Engine/Network/Packets/Disconnect.hpp>

tkd::Packets::Disconnect disconnectPacket;
disconnectPacket.clientID = myClientID;
disconnectPacket.reason = static_cast<UInt32>(EDisconnectionReason::Unknown);

// Send to server before closing connection
networkClient.SendPacket(disconnectPacket);
```

### Server Side (Forced Disconnect)

```cpp
// In FNetworkServer::DisconnectClient
Packets::Disconnect disconnectPacket;
disconnectPacket.clientID = clientID;
disconnectPacket.reason = static_cast<UInt32>(reason);
SendPacket(disconnectPacket, endpoint);
```

### Handling Disconnect Packets

Both clients and servers can register handlers:

```cpp
// On client
networkClient.RegisterPacketHandler<Packets::Disconnect>(
    [](const Packets::Disconnect& packet, const FEndpoint& sender) {
        if (packet.clientID == myClientID) {
            std::cout << "Server disconnected us: " << packet.reason << std::endl;
            // Handle disconnection
        }
    }
);

// On server
server.RegisterPacketHandler<Packets::Disconnect>(
    [this](const Packets::Disconnect& packet, const FEndpoint& endpoint) {
        std::cout << "Client " << packet.clientID << " disconnected: "
                  << packet.reason << std::endl;
        // Clean up connection
    }
);
```

## Notes

- The server automatically sends `Disconnect` packets when clients timeout or are kicked.
- Clients should send this packet before closing their connection for clean disconnections.
- The reason codes can be mapped to the `EDisconnectionReason` enum for type safety.
