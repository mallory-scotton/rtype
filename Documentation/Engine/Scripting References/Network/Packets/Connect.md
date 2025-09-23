# Connect Packet

## Overview

The `Connect` packet is sent by a client to request a connection to the server. It includes the client's proposed ID and name, allowing the server to identify and potentially validate the connecting client.

This packet is part of the core networking protocol and is automatically handled by the `FNetworkServer` class.

## Fields

### `UInt32 clientID`

The client's proposed or requested ID.

- **Use:** May be 0 for new connections, or a specific ID for reconnection attempts.

### `std::array<char, 32> name`

The name of the connecting client.

- **Size:** Fixed 32 bytes.
- **Use:** Client identifier, may be used for logging or display purposes.

## Usage

### Client Side

```cpp
#include <Engine/Network/Packets/Connect.hpp>

tkd::Packets::Connect connectPacket;
connectPacket.clientID = 0;  // Request new ID
std::strncpy(connectPacket.name.data(), "PlayerName", connectPacket.name.size());

// Send to server
networkClient.SendPacket(connectPacket);
```

### Server Side

The server automatically handles `Connect` packets when registered:

```cpp
// In FNetworkServer::SetupDefaultHandlers
RegisterPacketHandler<Packets::Connect>(
    [this](const Packets::Connect& packet, const FEndpoint& endpoint) {
        // Handle connection request
        std::cout << "Connect request from: " << std::string(packet.name.data())
                  << " at " << endpoint << std::endl;
    }
);
```

## Notes

- The `clientID` field may be ignored by the server, which assigns its own IDs.
- The name is null-terminated within the array.
- This packet triggers the server to send a `ConnectResponse`.
