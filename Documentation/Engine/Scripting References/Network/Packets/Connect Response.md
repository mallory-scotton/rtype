# ConnectResponse Packet

## Overview

The `ConnectResponse` packet is sent by the server in response to a client's `Connect` packet. It indicates whether the connection was accepted and provides the assigned client ID for successful connections.

This packet completes the connection handshake and is automatically sent by the `FNetworkServer` upon receiving a `Connect` packet.

## Fields

### `bool accepted`

Indicates whether the connection request was accepted.

- **Values:** `true` for accepted, `false` for rejected.
- **Use:** Determines if the client can proceed with the connection.

### `UInt32 clientID`

The server-assigned unique ID for the client.

- **Use:** Used for all subsequent communication with this client. Valid only if `accepted` is `true`.

## Usage

### Server Side

The server automatically sends `ConnectResponse` when handling connections:

```cpp
// In FNetworkServer::HandleConnectPacket
Packets::ConnectResponse response;
response.accepted = true;  // or false based on conditions
response.clientID = assignedId;
SendPacket(response, endpoint);
```

### Client Side

Clients should register a handler for `ConnectResponse`:

```cpp
networkClient.RegisterPacketHandler<Packets::ConnectResponse>(
    [](const Packets::ConnectResponse& packet, const FEndpoint& sender) {
        if (packet.accepted) {
            std::cout << "Connected with ID: " << packet.clientID << std::endl;
            // Proceed with game logic
        } else {
            std::cout << "Connection rejected" << std::endl;
            // Handle rejection
        }
    }
);
```

## Notes

- If `accepted` is `false`, the `clientID` field should be ignored.
- Successful connections result in the client being added to the server's connection list.
- This packet is part of the automatic connection process in `FNetworkServer`.
