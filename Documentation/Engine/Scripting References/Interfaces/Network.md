# Network Interface Documentation

## Overview

The `FNetworkInterface` provides comprehensive network communication capabilities for the TKD Engine, supporting both client-server and peer-to-peer architectures. It offers thread-safe operations for sending and receiving data, managing connections, and handling network statistics.

### Key Features

- **Dual Mode Support**: Client and server modes with automatic role detection
- **Multiple Transmission Types**: Reliable, unreliable, and broadcast messaging
- **Connection Management**: Automatic connection handling and state tracking
- **Thread-Safe Operations**: All methods protected by mutexes for cross-thread safety
- **Statistics Tracking**: Real-time network performance monitoring
- **RPC Integration**: Deferred remote procedure call processing

### Architecture

```cpp
class FNetworkInterface final
{
private:
    static std::mutex s_mutex;
    static __internal::FNetworkSubsystem* s_networkSubsystem;

public:
    // Setup and status
    static void Setup(__internal::FNetworkSubsystem* subsystem);
    static Bool IsInitialized(void);
    static Bool IsClient(void);
    static Bool IsServer(void);
    static Bool IsConnected(void);

    // Data transmission
    static Bool SendData(const std::vector<Byte>& data);
    static Bool SendData(const std::vector<Byte>& data, const FEndpoint& endpoint);
    static Bool SendData(const std::vector<Byte>& data, const std::vector<FEndpoint>& endpoints);

    // Packet transmission
    static Bool SendPacket(const IPacket& packet);
    static Bool SendPacket(const IPacket& packet, UInt32 clientID);
    static Bool SendPacket(const IPacket& packet, const FEndpoint& endpoint);
    static Bool SendPacket(const IPacket& packet, const std::vector<FEndpoint>& endpoints);

    // Reliable transmission
    static Bool SendReliablePacket(const IPacket& packet);
    static Bool SendReliablePacket(const IPacket& packet, UInt32 clientID);
    static Bool SendReliablePacket(const IPacket& packet, const FEndpoint& endpoint);
    static Bool SendReliablePacket(const IPacket& packet, const std::vector<FEndpoint>& endpoints);

    // Broadcasting
    static Bool BroadcastData(const std::vector<Byte>& data);
    static Bool BroadcastPacket(const IPacket& packet);

    // Connection management
    static Bool Connect(const FString& address, UInt16 port);
    static Bool Connect(const FEndpoint& endpoint);
    static Bool Disconnect(EDisconnectionReason reason = EDisconnectionReason::Unknown);

    // Client information
    static FConnectionInformation* GetClientInformation(UInt32 clientID);
    static UInt32 GetClientID(void);

    // Statistics
    static FNetworkStatistics GetStatistics(void);

    // RPC processing
    static void ProcessDeferredRPCs(UWorld& world);
};
```

## Setup and Initialization

### Network Subsystem Setup

#### `static void Setup(__internal::FNetworkSubsystem* subsystem)`

Sets the network subsystem instance for the interface.

**Parameters:**
- `subsystem`: Pointer to the initialized network subsystem

**Behavior:**
- Stores subsystem reference for static method access
- Must be called before using network functionality
- Typically called during engine initialization

**Example:**
```cpp
// During engine initialization
void InitializeNetworking() {
    // Create network subsystem with server configuration
    FNetworkSubsystem::Config config;
    config.mode = FNetworkSubsystem::Mode::Server;
    config.port = 8080;
    config.maxClients = 32;

    auto* networkSubsystem = new FNetworkSubsystem(config);
    if (networkSubsystem->Initialize()) {
        // Setup the static interface
        Network::Setup(networkSubsystem);

        // Start the subsystem
        networkSubsystem->Start();
    }
}
```

#### `static Bool IsInitialized(void)`

Checks if the network subsystem has been properly set up.

**Returns:** `true` if initialized, `false` otherwise

**Example:**
```cpp
if (!Network::IsInitialized()) {
    FLogger::Log(ELogLevel::Error, "Network subsystem not initialized");
    return;
}
// Safe to use network functions
```

### Network Mode Detection

#### `static Bool IsClient(void)`

Checks if running in client mode.

**Returns:** `true` if in client mode, `false` otherwise

#### `static Bool IsServer(void)`

Checks if running in server mode.

**Returns:** `true` if in server mode, `false` otherwise

**Example:**
```cpp
if (Network::IsServer()) {
    // Server-specific logic
    BroadcastGameState();
} else if (Network::IsClient()) {
    // Client-specific logic
    SendPlayerInput();
} else {
    // Offline/single-player mode
    UpdateLocalGame();
}
```

## Connection Management

### Connecting to Servers

#### `static Bool Connect(const FString& address, UInt16 port)`

Connects to a remote server.

**Parameters:**
- `address`: Server address (IP or hostname)
- `port`: Server port number

**Returns:** `true` if connection initiated successfully, `false` otherwise

**Behavior:**
- Initiates asynchronous connection
- Returns immediately (non-blocking)
- Connection state can be checked with `IsConnected()`

**Example:**
```cpp
// Connect to local server
if (Network::Connect("127.0.0.1", 8080)) {
    FLogger::Log(ELogLevel::Info, "Connecting to server...");
} else {
    FLogger::Log(ELogLevel::Error, "Failed to initiate connection");
}

// Wait for connection (in a loop)
while (!Network::IsConnected()) {
    // Process other logic
    UpdateUI();

    // Small delay to avoid busy waiting
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

FLogger::Log(ELogLevel::Info, "Connected to server!");
```

#### `static Bool Connect(const FEndpoint& endpoint)`

Connects to a server using an endpoint structure.

**Parameters:**
- `endpoint`: `FEndpoint` containing address and port

**Returns:** `true` if connection initiated successfully, `false` otherwise

**Example:**
```cpp
// Create endpoint
FEndpoint serverEndpoint;
serverEndpoint.address = "game.example.com";
serverEndpoint.port = 7777;

// Connect using endpoint
if (Network::Connect(serverEndpoint)) {
    FLogger::Log(ELogLevel::Info, "Connecting to game server...");
}
```

### Connection Status

#### `static Bool IsConnected(void)`

Checks if currently connected to a server.

**Returns:** `true` if connected, `false` otherwise

**Example:**
```cpp
void UpdateNetworkStatus() {
    if (Network::IsConnected()) {
        statusText->SetText("Connected");
        statusText->SetColor(Color::Green);
    } else {
        statusText->SetText("Disconnected");
        statusText->SetColor(Color::Red);

        // Attempt reconnection
        AttemptReconnection();
    }
}
```

### Disconnection

#### `static Bool Disconnect(EDisconnectionReason reason = EDisconnectionReason::Unknown)`

Disconnects from the current connection.

**Parameters:**
- `reason`: Reason for disconnection (default: Unknown)

**Returns:** `true` if disconnection successful, `false` otherwise

**Disconnection Reasons:**
- `EDisconnectionReason::Unknown`
- `EDisconnectionReason::UserInitiated`
- `EDisconnectionReason::Timeout`
- `EDisconnectionReason::ServerShutdown`
- `EDisconnectionReason::ProtocolError`

**Example:**
```cpp
// User-initiated disconnect
void OnQuitButtonPressed() {
    if (Network::Disconnect(EDisconnectionReason::UserInitiated)) {
        FLogger::Log(ELogLevel::Info, "Disconnected from server");
    }
    ExitGame();
}

// Automatic disconnect on timeout
void CheckConnectionTimeout() {
    if (timeSinceLastMessage > CONNECTION_TIMEOUT) {
        Network::Disconnect(EDisconnectionReason::Timeout);
        ShowReconnectDialog();
    }
}
```

## Data Transmission

### Raw Data Transmission

#### `static Bool SendData(const std::vector<Byte>& data)`

Sends raw data to all connected clients (server) or to server (client).

**Parameters:**
- `data`: Raw bytes to send

**Returns:** `true` if sent successfully, `false` otherwise

**Example:**
```cpp
// Send raw binary data
std::vector<Byte> binaryData = { 0x01, 0x02, 0x03, 0x04 };
if (Network::SendData(binaryData)) {
    FLogger::Log(ELogLevel::Info, "Raw data sent");
}
```

#### `static Bool SendData(const std::vector<Byte>& data, const FEndpoint& endpoint)`

Sends raw data to a specific endpoint.

**Parameters:**
- `data`: Raw bytes to send
- `endpoint`: Target endpoint

**Returns:** `true` if sent successfully, `false` otherwise

#### `static Bool SendData(const std::vector<Byte>& data, const std::vector<FEndpoint>& endpoints)`

Sends raw data to multiple endpoints.

**Parameters:**
- `data`: Raw bytes to send
- `endpoints`: List of target endpoints

**Returns:** `true` if sent successfully, `false` otherwise

## Packet Transmission

### Standard Packet Transmission

#### `static Bool SendPacket(const IPacket& packet)`

Sends a packet to all connected clients (server) or to server (client).

**Parameters:**
- `packet`: Packet implementing `IPacket` interface

**Returns:** `true` if sent successfully, `false` otherwise

**Example:**
```cpp
// Create and send a chat message packet
FChatMessagePacket chatPacket;
chatPacket.message = "Hello, world!";
chatPacket.senderID = Network::GetClientID();

if (Network::SendPacket(chatPacket)) {
    FLogger::Log(ELogLevel::Info, "Chat message sent");
}
```

#### `static Bool SendPacket(const IPacket& packet, UInt32 clientID)`

Sends a packet to a specific client by ID.

**Parameters:**
- `packet`: Packet to send
- `clientID`: Target client ID

**Returns:** `true` if sent successfully, `false` otherwise

**Example:**
```cpp
// Send private message to specific player
FPrivateMessagePacket privateMsg;
privateMsg.message = "This is a private message";
privateMsg.recipientID = targetPlayerID;

if (Network::SendPacket(privateMsg, targetPlayerID)) {
    FLogger::Log(ELogLevel::Info, "Private message sent");
}
```

#### `static Bool SendPacket(const IPacket& packet, const FEndpoint& endpoint)`

Sends a packet to a specific endpoint.

**Parameters:**
- `packet`: Packet to send
- `endpoint`: Target endpoint

**Returns:** `true` if sent successfully, `false` otherwise

#### `static Bool SendPacket(const IPacket& packet, const std::vector<FEndpoint>& endpoints)`

Sends a packet to multiple endpoints.

**Parameters:**
- `packet`: Packet to send
- `endpoints`: List of target endpoints

**Returns:** `true` if sent successfully, `false` otherwise

### Reliable Packet Transmission

#### `static Bool SendReliablePacket(const IPacket& packet)`

Sends a packet with guaranteed delivery.

**Parameters:**
- `packet`: Packet to send reliably

**Returns:** `true` if queued for sending, `false` otherwise

**Behavior:**
- Guaranteed delivery (acknowledgment-based)
- Higher latency than unreliable transmission
- Automatic retransmission on failure

**Example:**
```cpp
// Send critical game state reliably
FGameStatePacket statePacket;
statePacket.playerPosition = player->GetPosition();
statePacket.gameTime = World::GetTime();

if (Network::SendReliablePacket(statePacket)) {
    FLogger::Log(ELogLevel::Info, "Game state sent reliably");
}
```

#### `static Bool SendReliablePacket(const IPacket& packet, UInt32 clientID)`

Sends a reliable packet to a specific client.

**Parameters:**
- `packet`: Packet to send reliably
- `clientID`: Target client ID

**Returns:** `true` if queued for sending, `false` otherwise

#### `static Bool SendReliablePacket(const IPacket& packet, const FEndpoint& endpoint)`

Sends a reliable packet to a specific endpoint.

**Parameters:**
- `packet`: Packet to send reliably
- `endpoint`: Target endpoint

**Returns:** `true` if queued for sending, `false` otherwise

#### `static Bool SendReliablePacket(const IPacket& packet, const std::vector<FEndpoint>& endpoints)`

Sends a reliable packet to multiple endpoints.

**Parameters:**
- `packet`: Packet to send reliably
- `endpoints`: List of target endpoints

**Returns:** `true` if queued for sending, `false` otherwise

## Broadcasting

### Data Broadcasting

#### `static Bool BroadcastData(const std::vector<Byte>& data)`

Broadcasts raw data to all connected clients.

**Parameters:**
- `data`: Raw bytes to broadcast

**Returns:** `true` if broadcast successfully, `false` otherwise

**Requirements:** Must be running in server mode

**Example:**
```cpp
// Broadcast server announcement
std::string message = "Server maintenance in 5 minutes";
std::vector<Byte> data(message.begin(), message.end());

if (Network::BroadcastData(data)) {
    FLogger::Log(ELogLevel::Info, "Server announcement broadcasted");
}
```

### Packet Broadcasting

#### `static Bool BroadcastPacket(const IPacket& packet)`

Broadcasts a packet to all connected clients.

**Parameters:**
- `packet`: Packet to broadcast

**Returns:** `true` if broadcast successfully, `false` otherwise

**Requirements:** Must be running in server mode

**Example:**
```cpp
// Broadcast game start event
FGameStartPacket startPacket;
startPacket.countdownTime = 30.0f; // 30 second countdown

if (Network::BroadcastPacket(startPacket)) {
    FLogger::Log(ELogLevel::Info, "Game start broadcasted to all clients");
}
```

## Client Information

#### `static FConnectionInformation* GetClientInformation(UInt32 clientID)`

Gets connection information for a specific client.

**Parameters:**
- `clientID`: ID of the client to query

**Returns:** Pointer to client information, or `nullptr` if not found

**Example:**
```cpp
// Get information about a specific client
FConnectionInformation* clientInfo = Network::GetClientInformation(clientID);
if (clientInfo) {
    FLogger::Log(ELogLevel::Info, FString::Format(
        "Client %d connected from %s:%d",
        clientID,
        clientInfo->endpoint.address.c_str(),
        clientInfo->endpoint.port
    ));

    // Check connection quality
    if (clientInfo->ping > 100) { // High ping
        SendWarningToClient(clientID, "High latency detected");
    }
} else {
    FLogger::Log(ELogLevel::Warning, "Client information not found");
}
```

#### `static UInt32 GetClientID(void)`

Gets the client ID of the current connection.

**Returns:** Client ID, or 0 if not connected

**Example:**
```cpp
// Include client ID in packets
FPlayerUpdatePacket updatePacket;
updatePacket.clientID = Network::GetClientID();
updatePacket.position = player->GetPosition();
updatePacket.rotation = player->GetRotation();

Network::SendPacket(updatePacket);
```

## Network Statistics

#### `static FNetworkStatistics GetStatistics(void)`

Gets current network statistics.

**Returns:** `FNetworkStatistics` structure with network metrics

**Statistics Include:**
- Bytes sent/received per second
- Packet loss rate
- Latency information
- Connection count
- Bandwidth usage

**Example:**
```cpp
// Monitor network performance
FNetworkStatistics stats = Network::GetStatistics();

FLogger::Log(ELogLevel::Info, FString::Format(
    "Network Stats - Sent: %.1f KB/s, Received: %.1f KB/s, Ping: %d ms",
    stats.bytesSentPerSecond / 1024.0f,
    stats.bytesReceivedPerSecond / 1024.0f,
    stats.averagePingMs
));

// Check for network issues
if (stats.packetLossRate > 0.05f) { // 5% packet loss
    FLogger::Log(ELogLevel::Warning, "High packet loss detected");
    EnableNetworkOptimization();
}
```

## RPC Processing

#### `static void ProcessDeferredRPCs(UWorld& world)`

Processes deferred remote procedure calls from the network queue.

**Parameters:**
- `world`: Reference to the world (must be locked by caller)

**Behavior:**
- Executes queued RPCs safely during world tick
- Should be called from the world thread
- Prevents deadlocks by processing RPCs at safe points

**Example:**
```cpp
// In world subsystem tick
void FWorldSubsystem::Tick(float deltaTime) {
    // Update world
    m_world->Tick(deltaTime);

    // Process deferred RPCs safely
    Network::ProcessDeferredRPCs(*m_world);
}
```

## Usage Examples

### Client-Server Chat System

```cpp
class AChatSystem : public AActor {
public:
    // Client: Send chat message
    void SendChatMessage(const FString& message) {
        if (!Network::IsConnected()) return;

        FChatMessagePacket packet;
        packet.message = message;
        packet.senderID = Network::GetClientID();
        packet.timestamp = World::GetTime();

        Network::SendPacket(packet);
    }

    // Server: Broadcast chat message
    void Server_BroadcastChatMessage(const FChatMessagePacket& packet) {
        if (!Network::IsServer()) return;

        // Validate message (anti-spam, etc.)
        if (ValidateChatMessage(packet)) {
            // Broadcast to all clients
            Network::BroadcastPacket(packet);

            // Also log on server
            FLogger::Log(ELogLevel::Info, FString::Format(
                "Chat: Player %d said: %s",
                packet.senderID,
                packet.message.c_str()
            ));
        }
    }

private:
    bool ValidateChatMessage(const FChatMessagePacket& packet) {
        // Check message length
        if (packet.message.length() > 200) return false;

        // Check for spam
        if (IsSpamming(packet.senderID)) return false;

        return true;
    }

    bool IsSpamming(UInt32 clientID) {
        // Implement spam detection logic
        return false;
    }
};
```

### Player Synchronization

```cpp
class APlayerSynchronization : public AActor {
private:
    std::unordered_map<UInt32, APlayerCharacter*> m_players;
    float m_syncInterval = 0.1f; // 10Hz sync
    float m_lastSyncTime = 0.0f;

public:
    virtual void Tick(float deltaTime) override {
        float currentTime = World::GetTime();

        if (currentTime - m_lastSyncTime >= m_syncInterval) {
            SynchronizePlayers();
            m_lastSyncTime = currentTime;
        }
    }

private:
    void SynchronizePlayers() {
        if (Network::IsServer()) {
            // Server: Send all player states
            for (const auto& pair : m_players) {
                UInt32 clientID = pair.first;
                APlayerCharacter* player = pair.second;

                FPlayerStatePacket statePacket;
                statePacket.position = player->GetPosition();
                statePacket.rotation = player->GetRotation();
                statePacket.health = player->GetHealth();
                statePacket.velocity = player->GetVelocity();

                Network::SendPacket(statePacket, clientID);
            }
        } else {
            // Client: Send local player state
            APlayerCharacter* localPlayer = GetLocalPlayer();
            if (localPlayer) {
                FPlayerInputPacket inputPacket;
                inputPacket.clientID = Network::GetClientID();
                inputPacket.position = localPlayer->GetPosition();
                inputPacket.inputDirection = GetInputDirection();

                Network::SendReliablePacket(inputPacket);
            }
        }
    }

    void OnPlayerStateReceived(const FPlayerStatePacket& packet) {
        // Update remote player state
        UInt32 clientID = packet.clientID;
        if (m_players.count(clientID)) {
            APlayerCharacter* player = m_players[clientID];
            player->SetPosition(packet.position);
            player->SetRotation(packet.rotation);
            player->SetHealth(packet.health);
            player->SetVelocity(packet.velocity);
        }
    }

    APlayerCharacter* GetLocalPlayer() {
        // Return the local player character
        return nullptr;
    }

    FVector3 GetInputDirection() {
        // Return current input direction
        return FVector3::Zero;
    }
};
```

### Connection Management System

```cpp
class AConnectionManager : public AActor {
private:
    enum class ConnectionState {
        Disconnected,
        Connecting,
        Connected,
        Reconnecting
    };

    ConnectionState m_connectionState = ConnectionState::Disconnected;
    float m_reconnectTimer = 0.0f;
    const float RECONNECT_INTERVAL = 5.0f;
    FEndpoint m_serverEndpoint;

public:
    void ConnectToServer(const FString& address, UInt16 port) {
        m_serverEndpoint.address = address;
        m_serverEndpoint.port = port;
        m_connectionState = ConnectionState::Connecting;

        if (Network::Connect(m_serverEndpoint)) {
            FLogger::Log(ELogLevel::Info, "Connecting to server...");
        } else {
            FLogger::Log(ELogLevel::Error, "Failed to initiate connection");
            ScheduleReconnect();
        }
    }

    virtual void Tick(float deltaTime) override {
        switch (m_connectionState) {
            case ConnectionState::Connecting:
                UpdateConnecting();
                break;
            case ConnectionState::Connected:
                UpdateConnected();
                break;
            case ConnectionState::Reconnecting:
                UpdateReconnecting(deltaTime);
                break;
            case ConnectionState::Disconnected:
                // Do nothing
                break;
        }
    }

private:
    void UpdateConnecting() {
        if (Network::IsConnected()) {
            m_connectionState = ConnectionState::Connected;
            OnConnected();
        } else {
            // Check for connection timeout
            static float connectionStartTime = World::GetTime();
            if (World::GetTime() - connectionStartTime > 10.0f) {
                FLogger::Log(ELogLevel::Warning, "Connection timeout");
                ScheduleReconnect();
            }
        }
    }

    void UpdateConnected() {
        if (!Network::IsConnected()) {
            FLogger::Log(ELogLevel::Warning, "Lost connection to server");
            OnDisconnected();
            ScheduleReconnect();
        }
    }

    void UpdateReconnecting(float deltaTime) {
        m_reconnectTimer -= deltaTime;
        if (m_reconnectTimer <= 0.0f) {
            AttemptReconnect();
        }
    }

    void ScheduleReconnect() {
        m_connectionState = ConnectionState::Reconnecting;
        m_reconnectTimer = RECONNECT_INTERVAL;
        FLogger::Log(ELogLevel::Info, "Scheduling reconnection...");
    }

    void AttemptReconnect() {
        FLogger::Log(ELogLevel::Info, "Attempting to reconnect...");
        m_connectionState = ConnectionState::Connecting;

        if (!Network::Connect(m_serverEndpoint)) {
            FLogger::Log(ELogLevel::Error, "Reconnection failed");
            ScheduleReconnect();
        }
    }

    void OnConnected() {
        FLogger::Log(ELogLevel::Info, "Successfully connected to server");

        // Send join packet
        FJoinPacket joinPacket;
        joinPacket.clientID = Network::GetClientID();
        joinPacket.playerName = GetPlayerName();

        Network::SendReliablePacket(joinPacket);
    }

    void OnDisconnected() {
        FLogger::Log(ELogLevel::Info, "Disconnected from server");

        // Clean up game state
        CleanupGameState();
    }

    FString GetPlayerName() {
        // Return player name
        return "Player";
    }

    void CleanupGameState() {
        // Clean up network-related game objects
    }
};
```

### Network Statistics Monitor

```cpp
class ANetworkMonitor : public AActor {
private:
    FNetworkStatistics m_previousStats;
    float m_updateInterval = 1.0f;
    float m_lastUpdateTime = 0.0f;

public:
    virtual void Tick(float deltaTime) override {
        float currentTime = World::GetTime();

        if (currentTime - m_lastUpdateTime >= m_updateInterval) {
            UpdateNetworkStats();
            m_lastUpdateTime = currentTime;
        }
    }

private:
    void UpdateNetworkStats() {
        FNetworkStatistics currentStats = Network::GetStatistics();

        // Calculate rates of change
        float sentRateKBps = (currentStats.bytesSentPerSecond - m_previousStats.bytesSentPerSecond) / 1024.0f;
        float receivedRateKBps = (currentStats.bytesReceivedPerSecond - m_previousStats.bytesReceivedPerSecond) / 1024.0f;

        // Log statistics
        FLogger::Log(ELogLevel::Info, FString::Format(
            "Network - Sent: %.1f KB/s, Received: %.1f KB/s, Ping: %d ms, Loss: %.2f%%",
            sentRateKBps,
            receivedRateKBps,
            currentStats.averagePingMs,
            currentStats.packetLossRate * 100.0f
        ));

        // Check for network issues
        if (currentStats.averagePingMs > 200) {
            FLogger::Log(ELogLevel::Warning, "High latency detected");
            OnHighLatencyDetected();
        }

        if (currentStats.packetLossRate > 0.1f) { // 10% loss
            FLogger::Log(ELogLevel::Warning, "High packet loss detected");
            OnHighPacketLossDetected();
        }

        m_previousStats = currentStats;
    }

    void OnHighLatencyDetected() {
        // Enable latency compensation
        EnableInterpolation();
        ReduceUpdateRate();
    }

    void OnHighPacketLossDetected() {
        // Enable loss compensation
        EnableExtrapolation();
        IncreaseReliablePackets();
    }

    void EnableInterpolation() { /* Implementation */ }
    void ReduceUpdateRate() { /* Implementation */ }
    void EnableExtrapolation() { /* Implementation */ }
    void IncreaseReliablePackets() { /* Implementation */ }
};
```

## Performance Considerations

### Transmission Optimization

- **Choose Appropriate Reliability**: Use reliable transmission only for critical data
- **Batch Small Packets**: Combine multiple small updates into larger packets
- **Compress Data**: Use compression for large data transfers
- **Rate Limiting**: Limit transmission frequency to reduce bandwidth

### Connection Management

- **Connection Pooling**: Reuse connections when possible
- **Timeout Handling**: Implement appropriate connection timeouts
- **Reconnection Logic**: Automatic reconnection with exponential backoff

### Memory Management

- **Packet Pooling**: Reuse packet objects to reduce allocations
- **Buffer Management**: Efficient buffer allocation and reuse
- **Queue Sizing**: Appropriate queue sizes to prevent memory bloat

## Error Handling

### Transmission Failures

```cpp
bool SendPacketSafely(const IPacket& packet, UInt32 clientID) {
    try {
        if (!Network::SendPacket(packet, clientID)) {
            FLogger::Log(ELogLevel::Warning, "Failed to send packet to client " + std::to_string(clientID));

            // Check if client is still connected
            FConnectionInformation* clientInfo = Network::GetClientInformation(clientID);
            if (!clientInfo) {
                FLogger::Log(ELogLevel::Error, "Client " + std::to_string(clientID) + " not found");
                return false;
            }

            // Retry with reliable transmission
            if (!Network::SendReliablePacket(packet, clientID)) {
                FLogger::Log(ELogLevel::Error, "Reliable transmission also failed");
                return false;
            }
        }
        return true;
    } catch (const std::exception& e) {
        FLogger::Log(ELogLevel::Error, "Exception during packet transmission: " + std::string(e.what()));
        return false;
    }
}
```

### Connection Issues

```cpp
bool HandleConnectionError() {
    if (!Network::IsConnected()) {
        FLogger::Log(ELogLevel::Warning, "Connection lost");

        // Attempt graceful disconnect
        if (!Network::Disconnect(EDisconnectionReason::Timeout)) {
            FLogger::Log(ELogLevel::Error, "Failed to disconnect gracefully");
        }

        // Notify UI
        ShowConnectionLostDialog();

        // Attempt reconnection
        return AttemptReconnection();
    }
    return true;
}

bool AttemptReconnection() {
    static int reconnectAttempts = 0;
    const int maxAttempts = 5;

    if (reconnectAttempts >= maxAttempts) {
        FLogger::Log(ELogLevel::Error, "Max reconnection attempts reached");
        ShowReconnectFailedDialog();
        return false;
    }

    reconnectAttempts++;

    // Exponential backoff
    float delay = std::pow(2.0f, reconnectAttempts) * 1.0f; // 2, 4, 8, 16, 32 seconds

    FLogger::Log(ELogLevel::Info, "Attempting reconnection in " + std::to_string(delay) + " seconds");

    // Schedule reconnection
    ScheduleCallback([this]() {
        if (Network::Connect(m_lastEndpoint)) {
            FLogger::Log(ELogLevel::Info, "Reconnection initiated");
            reconnectAttempts = 0; // Reset on success
        } else {
            FLogger::Log(ELogLevel::Warning, "Reconnection failed");
        }
    }, delay);

    return true;
}
```

## Architecture Diagrams

### Network Interface Flow

```
Game Code
    │
    ▼
FNetworkInterface (Static Methods)
    │
    ▼
std::mutex (Thread Safety)
    │
    ▼
FNetworkSubsystem (Internal)
    │
    ├── FNetworkServer (Server Mode)
    │   ├── Connection Management
    │   ├── Client Handling
    │   └── Broadcasting
    ├── FNetworkClient (Client Mode)
    │   ├── Server Connection
    │   ├── Packet Transmission
    │   └── State Synchronization
    └── Statistics Tracking
```

### Packet Transmission Flow

```
Packet Created
    │
    ▼
FNetworkInterface::SendPacket()
    │
    ▼
Reliability Check
    │
    ├── Unreliable ──► Immediate Send
    │
    └── Reliable ──► Queue for ACK
    │
    ▼
Endpoint Resolution
    │
    ├── Single Client ──► Direct Send
    │
    ├── Multiple Clients ──► Broadcast
    │
    └── All Clients ──► Server Broadcast
```

### Connection State Machine

```
Disconnected
    │
    ▼
Connect() Called
    │
    ▼
Connecting
    │
    ├── Success ──► Connected
    │
    └── Failure ──► Disconnected
    │
Connected
    │
    ├── Network Error ──► Reconnecting
    │
    └── Disconnect() ──► Disconnecting
    │
Reconnecting
    │
    ├── Success ──► Connected
    │
    └── Max Retries ──► Disconnected
```

## Troubleshooting

### Common Issues

#### Connection Failures

**Symptoms:** `Connect()` returns `false` or `IsConnected()` never becomes `true`

**Possible Causes:**
- Invalid server address/port
- Firewall blocking connections
- Server not running
- Network configuration issues

**Debug:**
```cpp
// Test connection
FEndpoint testEndpoint("127.0.0.1", 8080);
if (!Network::Connect(testEndpoint)) {
    FLogger::Log(ELogLevel::Error, "Failed to initiate connection");

    // Check if network is initialized
    if (!Network::IsInitialized()) {
        FLogger::Log(ELogLevel::Error, "Network subsystem not initialized");
    }

    // Check if already connected
    if (Network::IsConnected()) {
        FLogger::Log(ELogLevel::Warning, "Already connected");
    }
}
```

#### Packet Transmission Failures

**Symptoms:** Send methods return `false`

**Possible Causes:**
- Not connected to server
- Invalid client ID
- Network congestion
- Packet serialization errors

**Debug:**
```cpp
// Debug packet sending
FTestPacket packet;
packet.data = "test";

if (!Network::SendPacket(packet)) {
    FLogger::Log(ELogLevel::Error, "Failed to send packet");

    // Check connection
    if (!Network::IsConnected()) {
        FLogger::Log(ELogLevel::Error, "Not connected to server");
    }

    // Check client mode
    if (Network::IsClient()) {
        UInt32 clientID = Network::GetClientID();
        FLogger::Log(ELogLevel::Info, "Client ID: " + std::to_string(clientID));
    }
}
```

#### High Latency/Packet Loss

**Symptoms:** Poor network performance, lag, desynchronization

**Possible Causes:**
- Network congestion
- Distance to server
- Inefficient packet structure
- Too many small packets

**Debug:**
```cpp
// Monitor network stats
FNetworkStatistics stats = Network::GetStatistics();

if (stats.averagePingMs > 100) {
    FLogger::Log(ELogLevel::Warning, "High ping: " + std::to_string(stats.averagePingMs) + "ms");
}

if (stats.packetLossRate > 0.01f) { // 1% loss
    FLogger::Log(ELogLevel::Warning, "Packet loss: " + std::to_string(stats.packetLossRate * 100.0f) + "%");
}

// Enable debugging
EnableNetworkDebugging();
```

#### Memory Issues

**Symptoms:** Memory usage grows over time

**Possible Causes:**
- Packet queues not being processed
- Circular references in packet data
- Large packet accumulation

**Debug:**
```cpp
// Monitor queue sizes (if available)
FNetworkStatistics stats = Network::GetStatistics();

// Check for queue buildup
if (stats.queuedPackets > 1000) {
    FLogger::Log(ELogLevel::Warning, "Large packet queue: " + std::to_string(stats.queuedPackets));
}

// Force queue processing
World::WithWorld([](UWorld& world) {
    Network::ProcessDeferredRPCs(world);
});
```

### Platform-Specific Issues

#### Windows Networking

- **Firewall**: Ensure application is allowed through Windows Firewall
- **Port Binding**: Check for port conflicts
- **IPv6**: Ensure proper IPv4/IPv6 handling

#### Linux Networking

- **iptables**: Check firewall rules
- **SELinux**: Verify security policy allows networking
- **Network Manager**: Ensure proper network configuration

#### Network Address Translation (NAT)

- **Port Forwarding**: Required for server hosting behind NAT
- **UPnP**: Automatic port forwarding if supported
- **STUN/TURN**: For NAT traversal in P2P scenarios

## Future Enhancements

### Planned Features

1. **WebSocket Support**: Browser-based client support
2. **IPv6 Support**: Full IPv6 networking stack
3. **DTLS Encryption**: Secure network communication
4. **Network Simulation**: Built-in lag/loss simulation for testing
5. **Advanced QoS**: Quality of Service for different packet types
6. **Cloud Integration**: Cloud-based relay servers

### Extension Points

```cpp
// Custom network interface extensions
class FExtendedNetworkInterface {
public:
    static Bool SendCompressedPacket(const IPacket& packet) {
        // Compress packet data before sending
        std::vector<Byte> compressedData = CompressPacket(packet);
        return Network::SendData(compressedData);
    }

    static Bool SendPrioritizedPacket(const IPacket& packet, ENetworkPriority priority) {
        // Route packets based on priority
        switch (priority) {
            case ENetworkPriority::High:
                return Network::SendReliablePacket(packet);
            case ENetworkPriority::Normal:
                return Network::SendPacket(packet);
            case ENetworkPriority::Low:
                return SendLowPriorityPacket(packet);
        }
        return false;
    }

    static FNetworkQuality GetNetworkQuality() {
        FNetworkStatistics stats = Network::GetStatistics();

        FNetworkQuality quality;
        quality.latency = stats.averagePingMs;
        quality.packetLoss = stats.packetLossRate;
        quality.bandwidth = stats.bytesSentPerSecond + stats.bytesReceivedPerSecond;

        return quality;
    }

private:
    static std::vector<Byte> CompressPacket(const IPacket& packet) {
        // Implementation of packet compression
        return std::vector<Byte>();
    }

    static Bool SendLowPriorityPacket(const IPacket& packet) {
        // Implementation of low-priority sending
        return Network::SendPacket(packet);
    }
};
```

---

*This documentation covers the complete FNetworkInterface as of TKD Engine v1.0.0. The interface provides comprehensive networking capabilities with support for both client-server and peer-to-peer architectures.*
