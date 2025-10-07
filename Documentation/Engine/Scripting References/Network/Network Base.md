# FNetworkBase

## Overview

The `FNetworkBase` class is an abstract base class that provides the foundation for network services in the TKD Engine. It handles UDP socket management, asynchronous I/O operations using ASIO, packet serialization/deserialization, and network statistics. This class is designed to be inherited by specific network implementations, such as clients or servers, which must implement the pure virtual `Start` method.

Key features include:
- Asynchronous UDP communication
- Automatic packet handling and routing
- Network statistics tracking
- Thread-safe operations
- Support for custom packet types through the packet manager

## Public Members

### Destructor

#### `virtual ~FNetworkBase()`

Virtual destructor that ensures the network service is stopped when the object is destroyed.

### Methods

#### `virtual bool Start(void) = 0`

Pure virtual method that must be implemented by derived classes to start the network service. This typically involves binding the socket and starting the network thread.

- **Returns:** `true` if the network started successfully, `false` otherwise.

#### `virtual void Stop(void)`

Stops the network service, closes the socket, and joins the network thread.

#### `bool IsRunning(void) const`

Checks if the network service is currently running.

- **Returns:** `true` if the network service is running, `false` otherwise.

#### `const FNetworkStatistics& GetStatistics(void) const`

Retrieves the current network statistics, including packets sent/received, bytes transferred, and dropped packets.

- **Returns:** A const reference to the `FNetworkStatistics` object.

#### `template <typename T> void RegisterPacketHandler(std::function<void(const T&, const FEndpoint&)> handler)`

Registers a handler function for a specific packet type. The handler will be called whenever a packet of type `T` is received.

- **Template Parameters:**
  - `T`: The packet type, must derive from `IPacket`.
- **Parameters:**
  - `handler`: A function that takes a const reference to the packet and the sender's endpoint.

## Usage Examples

### Deriving from FNetworkBase

```cpp
#include <Engine/Network/FNetworkBase.hpp>

class MyNetworkService : public tkd::FNetworkBase {
public:
    bool Start() override {
        // Implement socket binding and start logic
        // e.g., bind to a port, start the network thread
        m_running = true;
        m_networkThread = std::make_unique<FThread>(
            [this]() { RunNetworkThread(); }
        );
        StartReceive();
        return true;
    }
};
```

### Registering Packet Handlers

```cpp
#include <Engine/Network/Packets.hpp>  // Assuming Connect packet exists

MyNetworkService service;

// Register a handler for Connect packets
service.RegisterPacketHandler<Packets::Connect>(
    [](const Packets::Connect& packet, const FEndpoint& sender) {
        // Handle the connect packet
        std::cout << "Received connect from " << sender << std::endl;
    }
);
```

### Sending Packets

```cpp
// Assuming derived class has access to SendPacket
Packets::HeartBeat heartbeat;
FEndpoint targetEndpoint(/* endpoint details */);
if (service.SendPacket(heartbeat, targetEndpoint)) {
    // Packet sent successfully
}
```

### Checking Network Status

```cpp
if (service.IsRunning()) {
    const auto& stats = service.GetStatistics();
    std::cout << "Packets received: " << stats.packetsReceived << std::endl;
}
```

### Stopping the Service

```cpp
service.Stop();  // Automatically called in destructor, but can be called manually
```

## Notes

- This class uses ASIO for asynchronous I/O, ensuring non-blocking network operations.
- Packet handling is automatic; received packets are deserialized and routed to registered handlers.
- Statistics are updated in real-time and can be queried at any time.
- Derived classes should call `InitializePacketManager()` in their constructor or `Start()` method to register built-in packet types.
