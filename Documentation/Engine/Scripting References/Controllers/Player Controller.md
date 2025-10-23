# Player Controller Documentation

## Overview

The `APlayerController` class is a specialized controller implementation designed for handling player input and possession mechanics in the TKD Game Engine. As a client-side controller, it extends the base `AController` class with comprehensive input management capabilities, allowing players to interact with game entities through keyboard, mouse, gamepad, and other input devices.

### Key Features

- **Input Management**: Direct integration with the engine's input system for action and axis binding
- **Possession System**: Automatic setup and teardown of input bindings when possessing/unpossessing pawns
- **Event-Driven Architecture**: Callback-based input handling with support for pressed, released, and held states
- **Network-Aware**: Built-in support for client-server synchronization with proper net roles
- **Extensible Design**: Virtual methods for custom input binding setup in derived classes

### Architecture

```cpp
class APlayerController : public AController
```

The `APlayerController` inherits from `AController` and adds input-specific functionality:

- **Input Binding System**: Manages subscriptions to input actions and axes
- **Callback Storage**: Thread-safe storage of user-defined callback functions
- **Event Listener Management**: Automatic cleanup of event subscriptions
- **Possession Integration**: Seamless integration with the possession system

## Core Components

### Input Manager Integration

The player controller maintains a reference to an `FInputManager` instance, which provides access to:

- **Input Actions**: Discrete input events (button presses, key presses)
- **Input Axes**: Continuous input values (joystick positions, mouse movement)

### Callback System

The controller uses a callback-based system for input handling:

```cpp
using ActionCallback = TFunction<void(EInput)>;
using AxisCallback = TFunction<void(float)>;
```

- **Action Callbacks**: Receive input type information when actions trigger
- **Axis Callbacks**: Receive normalized float values (-1.0 to 1.0) for continuous inputs

### Binding Storage

Internal storage maintains:
- **Callback Maps**: Store user-defined callback functions by input name
- **Listener ID Maps**: Track event subscription handles for cleanup

## API Reference

### Constructor & Destructor

#### `APlayerController(const FString& name = "APlayerController")`

Creates a new player controller instance.

**Parameters:**
- `name`: Optional controller name (defaults to "APlayerController")

**Behavior:**
- Sets UUID to 1 (player controller identifier)
- Automatically retrieves input manager from Window on client builds
- Initializes input binding storage

#### `~APlayerController()`

Destroys the player controller and cleans up all input bindings.

### Possession Methods

#### `virtual void Possess(APawn* pawn) override final`

Possesses a pawn and sets up input bindings.

**Parameters:**
- `pawn`: The pawn to possess

**Behavior:**
- Unpossesses current pawn if any
- Sets pawn reference
- Calls `SetupInputBindings()` for input configuration
- Sets pawn's net role to `ENetRole::AutonomousProxy`
- Sets pawn's owning client ID
- Emits `Events::Possess` event

#### `virtual void UnPossess() override final`

Unpossesses the current pawn and clears input bindings.

**Behavior:**
- Calls `ClearInputBindings()` to remove all input subscriptions
- Clears pawn reference
- Resets pawn's net role to `ENetRole::SimulatedProxy`
- Resets pawn's owning client ID to 0
- Emits `Events::UnPossess` event

### Input Management

#### `void SetInputManager(FInputManager* inputManager)`

Sets the input manager for this controller.

**Parameters:**
- `inputManager`: Pointer to the input manager instance

**Behavior:**
- Clears existing bindings if input manager changes
- Stores new input manager reference
- Calls `SetupInputBindings()` if input manager is valid

#### `FInputManager* GetInputManager() const`

Retrieves the current input manager.

**Returns:** Pointer to the input manager, or `nullptr` if not set

### Input Binding Methods

#### `bool BindActionPressed(const FString& actionName, ActionCallback callback)`

Binds a callback to an action's pressed event.

**Parameters:**
- `actionName`: Name of the input action to bind to
- `callback`: Function to call when action is pressed

**Returns:** `true` if binding succeeded, `false` otherwise

**Behavior:**
- Retrieves action from input manager
- Stores callback in internal map
- Subscribes to action's `Pressed` event
- Stores listener ID for cleanup

#### `bool BindActionReleased(const FString& actionName, ActionCallback callback)`

Binds a callback to an action's released event.

**Parameters:**
- `actionName`: Name of the input action to bind to
- `callback`: Function to call when action is released

**Returns:** `true` if binding succeeded, `false` otherwise

#### `bool BindActionHeld(const FString& actionName, ActionCallback callback)`

Binds a callback to an action's held event.

**Parameters:**
- `actionName`: Name of the input action to bind to
- `callback`: Function to call when action is held

**Returns:** `true` if binding succeeded, `false` otherwise

#### `bool BindAxis(const FString& axisName, AxisCallback callback)`

Binds a callback to an axis change event.

**Parameters:**
- `axisName`: Name of the input axis to bind to
- `callback`: Function to call when axis value changes

**Returns:** `true` if binding succeeded, `false` otherwise

**Behavior:**
- Retrieves axis from input manager
- Stores callback in internal map
- Subscribes to axis's `Changed` event
- Stores listener ID for cleanup

### Virtual Methods

#### `virtual void SetupInputBindings()`

Virtual method for setting up input bindings in derived classes.

**Override Behavior:**
- Called automatically after input manager is set
- Called during possession of a pawn
- Implement custom input binding logic here

#### `virtual void EndPlay() override`

Called when play ends for this controller.

**Behavior:**
- Clears all input bindings
- Resets input manager reference
- Calls parent `EndPlay()` method

### Utility Methods

#### `void ClearInputBindings()`

Clears all input bindings and unsubscribes from events.

**Behavior:**
- Unsubscribes from all action events
- Unsubscribes from all axis events
- Clears all callback storage
- Clears all listener ID storage

## Usage Examples

### Basic Player Controller Setup

```cpp
class MyPlayerController : public APlayerController
{
public:
    MyPlayerController() : APlayerController("MyPlayerController") {}

    virtual void SetupInputBindings() override
    {
        // Bind movement actions
        BindActionPressed("MoveForward", [this](EInput input) {
            if (GetPawn()) {
                // Move pawn forward
                GetPawn()->MoveForward(1.0f);
            }
        });

        BindActionPressed("Jump", [this](EInput input) {
            if (GetPawn()) {
                // Make pawn jump
                GetPawn()->Jump();
            }
        });

        // Bind look axis
        BindAxis("LookX", [this](float value) {
            if (GetPawn()) {
                // Rotate pawn horizontally
                GetPawn()->AddRotation(FRotator(0.0f, value * 2.0f, 0.0f));
            }
        });

        BindAxis("LookY", [this](float value) {
            if (GetPawn()) {
                // Rotate pawn vertically
                GetPawn()->AddRotation(FRotator(value * 2.0f, 0.0f, 0.0f));
            }
        });
    }
};
```

### Advanced Input Handling

```cpp
class AdvancedPlayerController : public APlayerController
{
private:
    bool m_isSprinting = false;
    float m_moveSpeed = 1.0f;

public:
    virtual void SetupInputBindings() override
    {
        // Sprint toggle
        BindActionPressed("Sprint", [this](EInput input) {
            m_isSprinting = !m_isSprinting;
            m_moveSpeed = m_isSprinting ? 2.0f : 1.0f;
        });

        // Movement with sprint multiplier
        BindAxis("MoveForward", [this](float value) {
            if (GetPawn()) {
                GetPawn()->MoveForward(value * m_moveSpeed);
            }
        });

        BindAxis("MoveRight", [this](float value) {
            if (GetPawn()) {
                GetPawn()->MoveRight(value * m_moveSpeed);
            }
        });

        // Ability system
        BindActionHeld("PrimaryAbility", [this](EInput input) {
            if (GetPawn()) {
                GetPawn()->UsePrimaryAbility();
            }
        });

        BindActionReleased("PrimaryAbility", [this](EInput input) {
            if (GetPawn()) {
                GetPawn()->StopPrimaryAbility();
            }
        });
    }
};
```

### Controller Possession

```cpp
// Create player controller
APlayerController* playerController = new MyPlayerController();

// Create pawn
APawn* playerPawn = new APlayerPawn("PlayerPawn");

// Possess the pawn (automatically sets up input bindings)
playerController->Possess(playerPawn);

// Later, unpossess
playerController->UnPossess();

// Or possess a different pawn
APawn* vehiclePawn = new AVehiclePawn("Vehicle");
playerController->Possess(vehiclePawn); // Input bindings automatically update
```

## Network Synchronization

### Client-Server Architecture

The player controller operates in a client-server model:

- **Client-Side**: Handles local input processing and prediction
- **Server-Side**: Authoritative state management and validation

### Net Roles

When possessing a pawn, the player controller sets appropriate net roles:

- **Autonomous Proxy**: Local player has full control
- **Simulated Proxy**: Remote players see simulated movement

### RPC Integration

Player controllers can use RPCs for server communication:

```cpp
class NetworkedPlayerController : public APlayerController
{
    UFUNCTION(Server, Reliable)
    void Server_RequestRespawn()
    {
        // Server-side respawn logic
        APawn* newPawn = SpawnPlayerPawn();
        Possess(newPawn);
    }

public:
    void RequestRespawn()
    {
        if (GetNetRole() == ENetRole::AutonomousProxy)
        {
            Server_RequestRespawn();
        }
    }
};
```

## Integration Patterns

### With Game Modes

```cpp
class MyGameMode : public AGameMode
{
public:
    virtual void PostLogin(APlayerController* newPlayer) override
    {
        AGameMode::PostLogin(newPlayer);

        // Create and possess initial pawn
        APawn* startingPawn = SpawnPlayerPawn();
        newPlayer->Possess(startingPawn);
    }
};
```

### With HUD Systems

```cpp
class MyPlayerController : public APlayerController
{
private:
    AHUD* m_hud;

public:
    virtual void Possess(APawn* pawn) override
    {
        APlayerController::Possess(pawn);

        // Create HUD for this player
        m_hud = new AHUD();
        m_hud->SetOwningPlayer(this);
    }

    virtual void UnPossess() override
    {
        // Clean up HUD
        if (m_hud)
        {
            delete m_hud;
            m_hud = nullptr;
        }

        APlayerController::UnPossess();
    }
};
```

## Performance Considerations

### Input Binding Efficiency

- **Callback Storage**: Uses `std::unordered_map` for O(1) lookup performance
- **Event Subscription**: Minimal overhead for event listener management
- **Memory Management**: Efficient cleanup prevents memory leaks

### Network Optimization

- **Client Prediction**: Local input processing reduces server load
- **Selective Replication**: Only relevant state changes are networked
- **Input Buffering**: Smooths out network jitter for responsive controls

### Best Practices

1. **Minimize Bindings**: Only bind to inputs you need
2. **Use Appropriate Callbacks**: Choose between Pressed/Released/Held based on use case
3. **Clean Up Properly**: Always clear bindings when done
4. **Profile Performance**: Monitor input processing in performance-critical scenarios

## Architecture Diagrams

### Class Hierarchy

```
AController (Base Controller)
    ├── UUID: 0
    ├── OwningClientID: 0
    ├── NetRole: None
    └── Pawn: nullptr
    └── Events: Possess, UnPossess
    └── RPC System
    └── Property System

APlayerController (Player Controller)
    ├── UUID: 1 (Fixed)
    ├── InputManager: FInputManager*
    ├── Action Callbacks: unordered_map<FString, TVector<ActionCallback>>
    ├── Axis Callbacks: unordered_map<FString, TVector<AxisCallback>>
    ├── Listener IDs: unordered_map<FString, TVector<SizeT>>
    └── Input Binding Methods
```

### Input Flow Diagram

```
Input Device ──► FInputManager ──► UInputAction/UInputAxis ──► APlayerController
       │                │                        │
       │                │                        │
       ▼                ▼                        ▼
   Raw Events     Action/Axis Events        Callback Execution
   (Key/Mouse)    (Pressed/Changed)         (User Functions)
```

### Possession Flow

```
Possess(pawn) ──► UnPossess Current ──► Set New Pawn ──► SetupInputBindings()
       │                                        │
       │                                        │
       ▼                                        ▼
   Set NetRole                              Bind Actions/Axes
   (AutonomousProxy)                        (Pressed/Released/Held)
                                            (Axis Changed)

UnPossess() ──► ClearInputBindings() ──► Clear Pawn ──► Reset NetRole
       │                                        │            (SimulatedProxy)
       │                                        │
       ▼                                        ▼
   Unsubscribe Events                      Emit Events
   (All Listeners)                        (Possess/UnPossess)
```

## Troubleshooting

### Common Issues

#### Input Not Working

**Symptoms:** Input callbacks not triggering

**Possible Causes:**
- Input manager not set
- Action/axis names don't match input configuration
- Callbacks not properly bound

**Solutions:**
```cpp
// Check input manager
if (GetInputManager() == nullptr) {
    SetInputManager(Window::GetInputManager());
}

// Verify action exists
const UInputAction* action = GetInputManager()->GetAction("MyAction");
if (action == nullptr) {
    // Action not configured in input settings
}

// Check binding success
bool success = BindActionPressed("MyAction", callback);
if (!success) {
    // Binding failed - check action name and input manager
}
```

#### Memory Leaks

**Symptoms:** Memory usage increases over time

**Possible Causes:**
- Callbacks not properly cleaned up
- Circular references in lambda captures

**Solutions:**
```cpp
// Use weak pointers for captured objects
BindActionPressed("MyAction", [this, weakPawn = TWeakPtr<APawn>(pawn)](EInput input) {
    if (auto strongPawn = weakPawn.lock()) {
        // Safe to use pawn
    }
});

// Always clear bindings
virtual void EndPlay() override {
    ClearInputBindings();
    APlayerController::EndPlay();
}
```

#### Network Desynchronization

**Symptoms:** Client and server states don't match

**Possible Causes:**
- Client prediction not implemented
- Input not properly replicated

**Solutions:**
```cpp
// Implement client prediction
BindAxis("MoveForward", [this](float value) {
    // Local prediction
    if (GetPawn()) {
        GetPawn()->MoveForward(value);
    }

    // Send to server
    Server_MoveForward(value);
});

UFUNCTION(Server, Unreliable)
void Server_MoveForward(float value) {
    // Server validation and correction
    if (GetPawn()) {
        GetPawn()->MoveForward(value);
    }
}
```

### Debug Information

Enable debug logging for input binding:

```cpp
// In SetupInputBindings
bool success = BindActionPressed("Jump", callback);
if (!success) {
    FLogger::Log(ELogLevel::Warning, "Failed to bind Jump action");
}
```

## Future Enhancements

### Planned Features

1. **Input Profiles**: Multiple input configurations for different game modes
2. **Gesture Recognition**: Advanced input patterns (swipes, multi-touch)
3. **Haptic Feedback**: Force feedback integration
4. **Accessibility**: Customizable input sensitivity and mappings
5. **Recording/Replay**: Input recording for gameplay analysis

### Extension Points

The player controller design allows for easy extension:

```cpp
class VRPlayerController : public APlayerController
{
public:
    virtual void SetupInputBindings() override
    {
        APlayerController::SetupInputBindings();

        // Add VR-specific bindings
        BindActionPressed("Teleport", [this](EInput input) {
            PerformTeleport();
        });
    }

private:
    void PerformTeleport() {
        // VR teleportation logic
    }
};
```

### Performance Optimizations

Future versions may include:
- **Input Prediction**: Client-side prediction algorithms
- **Binding Caching**: Pre-compiled input binding tables
- **Multithreaded Input**: Parallel input processing
- **GPU-Accelerated Input**: Hardware-accelerated input processing

---

*This documentation covers the complete APlayerController implementation as of TKD Engine v1.0.0. For the latest updates and additional examples, refer to the engine's source code and test suites.*
