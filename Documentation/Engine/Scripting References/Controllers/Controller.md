# AController

## Overview

The `AController` class in the TKD Game Engine serves as the base class for all controller types, providing the fundamental mechanism for possessing and controlling `APawn` objects. Controllers act as intermediaries between input sources (players, AI, network) and game entities (pawns), enabling modular control systems where the same pawn can be controlled by different types of controllers.

## Architecture

### Class Hierarchy

```
AActor (Base actor class)
    │
    └── AController (Abstract controller base)
        │
        ├── APlayerController (Player input controller)
        ├── AAIController (AI behavior controller)
        └── ANetworkController (Network synchronization controller)
```

### Core Components

The `AController` class combines several key systems:

- **Actor Inheritance**: Inherits from `AActor` for world presence and lifecycle management
- **Event System**: Uses `TEventEmitter` for possession state notifications
- **RPC System**: Network synchronization through remote procedure calls
- **Possession System**: Manages ownership and control of `APawn` objects

### Interface Design

```cpp
class AController
    : public AActor
    , public TEventEmitter<TEvents<AController>::All>
{
protected:
    APawn* m_pawn;   // Controlled pawn reference

public:
    // Possession management
    virtual void Possess(APawn* pawn);
    virtual void UnPossess(void);
    APawn* GetPawn(void) const;

    // Network synchronization
    UFunction<UUID> PossessRPC;
    UFunction<> UnPossessRPC;
};
```

## Event System

### Event Types

The `AController` emits events for possession state changes:

```cpp
template <>
class TEvents<AController>
{
public:
    struct Possess {
        APawn* pawn;   // The pawn that was possessed
    };

    struct UnPossess {
        APawn* pawn;   // The pawn that was unpossessed
    };

    using All = std::tuple<Possess, UnPossess>;
};
```

### Event Usage

```cpp
// Listen for possession events
controller->On<AController::Events::Possess>([](const auto& event) {
    std::cout << "Controller possessed pawn: " << event.pawn->GetName() << std::endl;
});

controller->On<AController::Events::UnPossess>([](const auto& event) {
    std::cout << "Controller unpossessed pawn: " << event.pawn->GetName() << std::endl;
});
```

## RPC System

### Remote Procedure Calls

`AController` provides network synchronization through RPC functions:

```cpp
// Network possession commands
UFunction<UUID> PossessRPC;   // Possess pawn by UUID
UFunction<> UnPossessRPC;     // Unpossess current pawn
```

### RPC Implementation

```cpp
// RPC handlers (called automatically by network system)
void RPC_Possess(const UUID& pawnID) {
    UObject* obj = UObject::FindByUUID(pawnID);
    if (obj == nullptr) return;

    APawn* pawn = obj->As<APawn>();
    if (pawn == nullptr) return;

    Possess(pawn);
}

void RPC_UnPossess(void) {
    UnPossess();
}
```

## Possession System

### Core Concept

Possession is the mechanism by which a controller gains control over a pawn:

- **Single Ownership**: One controller can possess only one pawn at a time
- **Exclusive Control**: A pawn can be possessed by only one controller
- **State Management**: Automatic cleanup and event notification
- **Network Sync**: Possession changes are synchronized across network

### Possession Lifecycle

```
Controller Creation
        │
        ▼
    Find Target Pawn
        │
        ▼
    Possess(Pawn)
        │
        ├─► Emit Possess Event
        ├─► Set m_pawn pointer
        └─► Network: Call PossessRPC
        │
        ▼
    Control Active
        │
        ▼
    UnPossess() or Controller Destruction
        │
        ├─► Emit UnPossess Event
        ├─► Clear m_pawn pointer
        └─► Network: Call UnPossessRPC
```

## API Reference

### Constructor and Destructor

#### `AController(const FString& name = "AController")`
```cpp
AController(const FString& name = "AController");
```
Creates a new controller instance.

**Parameters**:
- `name`: `const FString&` - Name of the controller (default: "AController")

**Initializes**:
- `m_pawn = nullptr`
- RPC functions bound to handlers

#### `~AController()`
```cpp
~AController();
```
Destroys the controller and automatically unpossesses any controlled pawn.

### Possession Methods

#### `Possess(APawn* pawn)`
```cpp
virtual void Possess(APawn* pawn);
```
Takes possession of the specified pawn.

**Parameters**:
- `pawn`: `APawn*` - The pawn to possess (can be nullptr to unpossess)

**Behavior**:
- Unpossesses current pawn if different from new pawn
- Sets `m_pawn` to the new pawn
- Emits `Possess` event if pawn is not null
- Triggers network synchronization via `PossessRPC`

#### `UnPossess()`
```cpp
virtual void UnPossess(void);
```
Releases possession of the currently controlled pawn.

**Behavior**:
- Stores reference to current pawn
- Sets `m_pawn` to nullptr
- Emits `UnPossess` event with old pawn reference
- Triggers network synchronization via `UnPossessRPC`

#### `GetPawn()`
```cpp
APawn* GetPawn(void) const;
```
Retrieves the currently possessed pawn.

**Returns**: `APawn*` - The possessed pawn, or `nullptr` if none

### RPC Methods

#### `RPC_Possess(const UUID& pawnID)`
```cpp
void RPC_Possess(const UUID& pawnID);
```
Network RPC handler for possession commands.

**Parameters**:
- `pawnID`: `const UUID&` - UUID of the pawn to possess

**Behavior**:
- Looks up pawn by UUID using `UObject::FindByUUID()`
- Casts to `APawn` and possesses if valid

#### `RPC_UnPossess()`
```cpp
void RPC_UnPossess(void);
```
Network RPC handler for unpossession commands.

**Behavior**:
- Calls `UnPossess()` to release current pawn

### Member Variables

#### `m_pawn`
```cpp
APawn* m_pawn;
```
Reference to the currently possessed pawn. Protected member accessible to derived classes.

#### `PossessRPC`
```cpp
UFunction<UUID> PossessRPC;
```
RPC function for network possession synchronization.

#### `UnPossessRPC`
```cpp
UFunction<> UnPossessRPC;
```
RPC function for network unpossession synchronization.

## Usage Examples

### Basic Controller Setup

```cpp
#include <Engine/Runtime/Controllers/AController.hpp>

// Create a controller
tkd::AController* controller = new tkd::AController("PlayerController");

// Find a pawn to possess
tkd::APawn* targetPawn = FindPawnInWorld();

// Possess the pawn
controller->Possess(targetPawn);

// Check possession status
if (controller->GetPawn() != nullptr) {
    std::cout << "Controller is possessing: " << controller->GetPawn()->GetName() << std::endl;
}

// Later, unpossess
controller->UnPossess();

// Cleanup
delete controller;
```

### Event-Driven Controller

```cpp
class GameController : public tkd::AController {
public:
    GameController() : tkd::AController("GameController") {
        // Listen for possession events
        this->On<Events::Possess>([this](const auto& event) {
            OnPawnPossessed(event.pawn);
        });

        this->On<Events::UnPossess>([this](const auto& event) {
            OnPawnUnPossessed(event.pawn);
        });
    }

private:
    void OnPawnPossessed(tkd::APawn* pawn) {
        std::cout << "Controller took control of pawn" << std::endl;
        // Initialize pawn-specific logic
        SetupPawnControl(pawn);
    }

    void OnPawnUnPossessed(tkd::APawn* pawn) {
        std::cout << "Controller released control of pawn" << std::endl;
        // Cleanup pawn-specific logic
        CleanupPawnControl(pawn);
    }

    void SetupPawnControl(tkd::APawn* pawn) {
        // Configure pawn for controller input
        // e.g., enable input processing, set control modes
    }

    void CleanupPawnControl(tkd::APawn* pawn) {
        // Reset pawn to neutral state
        // e.g., disable input processing, clear control modes
    }
};
```

### Network Synchronized Controller

```cpp
class NetworkController : public tkd::AController {
public:
    NetworkController() : tkd::AController("NetworkController") {}

    // Server-side possession request
    void RequestPossession(tkd::APawn* pawn) {
        if (pawn == nullptr) return;

        // Get pawn's network UUID
        tkd::UUID pawnID = pawn->GetUUID();

        // Send possession command to client
        // (This would use your network system)
        SendRPCToClient(PossessRPC, pawnID);
    }

    // Client-side unpossession request
    void RequestUnPossession() {
        // Send unpossession command to client
        SendRPCToClient(UnPossessRPC);
    }
};
```

### Controller Manager System

```cpp
class ControllerManager {
private:
    std::vector<tkd::AController*> m_controllers;
    std::map<tkd::APawn*, tkd::AController*> m_pawnToController;

public:
    tkd::AController* CreateController(const std::string& type) {
        tkd::AController* controller = nullptr;

        if (type == "Player") {
            controller = new tkd::APlayerController();
        } else if (type == "AI") {
            controller = new tkd::AAIController();
        }

        if (controller) {
            m_controllers.push_back(controller);

            // Listen for possession changes
            controller->On<tkd::AController::Events::Possess>(
                [this](const auto& event) {
                    m_pawnToController[event.pawn] = controller;
                }
            );

            controller->On<tkd::AController::Events::UnPossess>(
                [this](const auto& event) {
                    m_pawnToController.erase(event.pawn);
                }
            );
        }

        return controller;
    }

    tkd::AController* GetControllerForPawn(tkd::APawn* pawn) {
        auto it = m_pawnToController.find(pawn);
        return (it != m_pawnToController.end()) ? it->second : nullptr;
    }

    void TransferPossession(tkd::APawn* pawn, tkd::AController* newController) {
        // Find current controller
        tkd::AController* currentController = GetControllerForPawn(pawn);

        // Unpossess from current controller
        if (currentController) {
            currentController->UnPossess();
        }

        // Possess with new controller
        if (newController) {
            newController->Possess(pawn);
        }
    }
};
```

### Advanced Possession Logic

```cpp
class PossessionManager {
public:
    // Check if possession is allowed
    bool CanPossess(tkd::AController* controller, tkd::APawn* pawn) {
        if (!controller || !pawn) return false;

        // Check if pawn is already possessed
        if (pawn->GetController() != nullptr) return false;

        // Check if controller already possesses someone
        if (controller->GetPawn() != nullptr) return false;

        // Check game-specific rules
        return IsPossessionAllowed(controller, pawn);
    }

    // Safe possession with validation
    bool SafePossess(tkd::AController* controller, tkd::APawn* pawn) {
        if (!CanPossess(controller, pawn)) {
            return false;
        }

        controller->Possess(pawn);
        return true;
    }

    // Force possession (unpossesses existing controller)
    void ForcePossess(tkd::AController* controller, tkd::APawn* pawn) {
        if (!controller || !pawn) return;

        // Unpossess any existing controller
        tkd::AController* existingController = GetControllerForPawn(pawn);
        if (existingController) {
            existingController->UnPossess();
        }

        // Possess with new controller
        controller->Possess(pawn);
    }

private:
    bool IsPossessionAllowed(tkd::AController* controller, tkd::APawn* pawn) {
        // Implement game-specific possession rules
        // e.g., team checks, distance limits, etc.
        return true;
    }

    tkd::AController* GetControllerForPawn(tkd::APawn* pawn) {
        // Implementation would search controller registry
        return nullptr;
    }
};
```

## Network Synchronization

### RPC Flow

```
Server Decision
      │
      ▼
  Possess(Pawn) Called
      │
      ▼
  Emit Possess Event (Local)
      │
      ▼
  Send PossessRPC to Clients
      │
      ├─► Client Receives RPC
      │         │
      │         ▼
      │    RPC_Possess Handler
      │         │
      │         ▼
      └─►   Possess(Pawn) Called
                │
                ▼
          Emit Possess Event (Client)
```

### Synchronization Considerations

- **UUID-based**: Uses UUIDs for network-safe pawn identification
- **Event-driven**: Local events emitted before network synchronization
- **Idempotent**: Multiple possession calls with same pawn are safe
- **Automatic cleanup**: Destructor ensures proper unpossession

## Integration with Game Systems

### Player Controller Integration

```cpp
class PlayerController : public tkd::AController {
public:
    void SetupPlayerInput() {
        // Bind input actions
        BindAction("MoveForward", [this](float value) {
            if (tkd::APawn* pawn = GetPawn()) {
                // Move pawn forward
                pawn->MoveForward(value);
            }
        });

        BindAction("Jump", [this]() {
            if (tkd::APawn* pawn = GetPawn()) {
                // Make pawn jump
                pawn->Jump();
            }
        });
    }

    void Update(float deltaTime) {
        // Process input
        ProcessInput(deltaTime);

        // Update pawn if possessed
        if (tkd::APawn* pawn = GetPawn()) {
            UpdatePawnControl(pawn, deltaTime);
        }
    }
};
```

### AI Controller Integration

```cpp
class AIController : public tkd::AController {
private:
    tkd::AIBehavior* m_behavior;

public:
    void InitializeAI(tkd::APawn* pawn) {
        Possess(pawn);
        m_behavior = CreateBehaviorForPawn(pawn);
    }

    void Update(float deltaTime) {
        if (tkd::APawn* pawn = GetPawn()) {
            // Update AI behavior
            m_behavior->Update(pawn, deltaTime);

            // Execute AI decisions
            ExecuteAIDecisions(pawn);
        }
    }

    void OnBehaviorComplete() {
        // Handle behavior completion
        // e.g., select new behavior, unpossess, etc.
    }
};
```

### Game Mode Integration

```cpp
class GameMode {
private:
    std::vector<tkd::AController*> m_activeControllers;

public:
    tkd::AController* SpawnPlayerController() {
        auto controller = new tkd::APlayerController();
        m_activeControllers.push_back(controller);

        // Find or spawn player pawn
        tkd::APawn* playerPawn = FindPlayerStart();
        if (playerPawn) {
            controller->Possess(playerPawn);
        }

        return controller;
    }

    tkd::AController* SpawnAIController(tkd::APawn* aiPawn) {
        auto controller = new tkd::AAIController();
        m_activeControllers.push_back(controller);

        if (aiPawn) {
            controller->Possess(aiPawn);
        }

        return controller;
    }

    void RestartPlayer(tkd::AController* controller) {
        // Find respawn point
        tkd::APawn* newPawn = FindRespawnPawn();

        // Transfer possession
        controller->Possess(newPawn);
    }
};
```

## Performance Considerations

### Memory Management
- Minimal memory footprint (single pawn pointer)
- Event system uses efficient delegate storage
- RPC functions are lightweight

### CPU Overhead
- Possession operations are O(1)
- Event emission has minimal overhead
- Network synchronization depends on RPC system performance

### Thread Safety
- Not inherently thread-safe
- Possession should be performed on main thread
- RPC handlers called on network thread (be careful with pawn access)

## Platform Considerations

### Client-Server Architecture
- Controllers exist on both client and server
- Server authoritative for possession decisions
- Clients receive possession state via RPC

### Reflection System Integration
```cpp
IMPLEMENT_CLASS_WITH_SUPER(AController, AActor)
```
Enables runtime type information and serialization.

## Diagrams

### Controller Possession Flow

```
Input Source (Player/AI/Network)
              │
              ▼
        AController::Possess(Pawn)
              │
              ├─► Validate Possession
              │         │
              │         ▼
              │    Unpossess Old Pawn
              │         │
              │         ▼
              └─►   Set m_pawn = New Pawn
                        │
                        ├─► Emit Possess Event
                        └─► Send PossessRPC (Network)
```

### Controller Hierarchy

```
AActor
    │
    ├── Transform
    ├── Components
    └── Lifecycle
    │
    └── AController
        │
        ├── m_pawn (APawn*)
        ├── Event Emitter
        └── RPC Functions
        │
        ├── APlayerController
        │   ├── Input Processing
        │   ├── Camera Control
        │   └── Player-specific Logic
        │
        ├── AAIController
        │   ├── Behavior Trees
        │   ├── Pathfinding
        │   └── AI Decision Making
        │
        └── ANetworkController
            ├── State Synchronization
            ├── Prediction
            └── Network-specific Logic
```

### Event Flow

```
Possession Change
        │
        ▼
    Controller Method
        │
        ├─► Local State Update
        │         │
        │         ▼
        │    Emit Event (Immediate)
        │         │
        │         ▼
        └─►   Send RPC (Network)
                    │
                    ▼
              Remote Clients
                    │
                    ▼
              RPC Handler
                    │
                    ▼
              Emit Event (Remote)
```

## Troubleshooting

### Common Issues

1. **Multiple Possession**
   - Ensure controllers unpossess before possessing new pawns
   - Check for concurrent possession attempts

2. **Null Pawn References**
   - Validate pawn pointers before possession
   - Handle pawn destruction during possession

3. **Network Desynchronization**
   - Ensure RPC calls complete before local operations
   - Handle network latency in possession timing

4. **Event Handler Crashes**
   - Use weak pointers or check object validity in event handlers
   - Avoid deleting objects during event emission

### Debug Information

```cpp
// Debug controller state
void DebugController(tkd::AController* controller) {
    std::cout << "Controller: " << controller->GetName() << std::endl;
    std::cout << "  UUID: " << controller->GetUUID() << std::endl;

    tkd::APawn* pawn = controller->GetPawn();
    if (pawn) {
        std::cout << "  Possessed Pawn: " << pawn->GetName() << std::endl;
        std::cout << "  Pawn UUID: " << pawn->GetUUID() << std::endl;
    } else {
        std::cout << "  No Possessed Pawn" << std::endl;
    }

    // Check RPC function bindings
    std::cout << "  PossessRPC bound: " << (controller->PossessRPC.IsBound() ? "Yes" : "No") << std::endl;
    std::cout << "  UnPossessRPC bound: " << (controller->UnPossessRPC.IsBound() ? "Yes" : "No") << std::endl;
}
```

## Future Enhancements

### Planned Features
- **Controller Priorities**: Priority-based possession resolution
- **Partial Possession**: Control sharing between controllers
- **Controller Stacking**: Multiple controllers on one pawn
- **Possession Transitions**: Smooth handoff animations
- **Controller Replication**: Advanced network synchronization

### Extension Points
- **Custom Possession Logic**: Game-specific possession rules
- **Controller Plugins**: Modular controller components
- **Input Mapping**: Dynamic input-controller binding
- **AI Integration**: Advanced AI controller frameworks

## Dependencies

- **AActor**: Base actor functionality and world integration
- **APawn**: Pawn objects that controllers can possess
- **Event System**: TEventEmitter for possession notifications
- **RPC System**: UFunction for network synchronization
- **Reflection**: IMPLEMENT_CLASS_WITH_SUPER for runtime type information

## See Also

- [AActor](AActor.md) - Base actor class
- [APawn](APawn.md) - Pawn objects that can be possessed
- [APlayerController](APlayerController.md) - Player input controller
- [AAIController](AAIController.md) - AI behavior controller
- [Event System](Event System.md) - Event emission and handling
- [RPC System](RPC System.md) - Remote procedure calls
