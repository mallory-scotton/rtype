# Animation State Documentation

## Overview

The `FAnimationState` class is a core component of the TKD Game Engine's animation state machine system, representing individual states within an animation graph. Each state encapsulates an animation asset, transition rules, and playback parameters that define how animations behave and transition between different states in a controlled manner.

### Key Features

- **Animation Container**: Holds animation assets with type-safe variant storage
- **Transition Management**: Manages outgoing transitions to other states
- **Playback Control**: Configurable playback speed and interruption settings
- **State Identification**: Unique naming for state machine navigation
- **Type Safety**: Template-based animation storage with compile-time safety
- **Extensible Design**: Easy integration with state machine controllers

### Architecture

```cpp
class FAnimationState
{
public:
    using AnimationType = TVariant<FAnimation2D>;

    FString name;                           // State identifier
    AnimationType animation;                // Animation asset
    TVector<FAnimationTransition> transitions; // Outgoing transitions
    Float32 playbackSpeed;                  // Speed multiplier
    bool allowInterruption;                 // Interruption permission

    FAnimationState(void);
    FAnimationState(const FString& name, const AnimationType& animation);
};
```

The `FAnimationState` serves as a node in the animation state machine graph, containing all the information needed to play animations and determine when to transition to other states.

## Core Components

### State Identity

- **Name**: Unique identifier for the state within the state machine
- **String-Based**: Uses `FString` for human-readable identification
- **State Machine Scope**: Must be unique within the containing state machine

### Animation Storage

- **Type-Safe Variant**: `TVariant<FAnimation2D>` for animation storage
- **Extensible Types**: Can be extended to support additional animation types
- **Reference Semantics**: Stores animation by value or reference as needed

### Transition System

- **Outgoing Transitions**: `TVector<FAnimationTransition>` for state changes
- **Dynamic Addition**: Transitions can be added/removed at runtime
- **Evaluation Order**: Transitions evaluated in vector order

### Playback Parameters

- **Speed Multiplier**: `Float32` playback speed (1.0f = normal speed)
- **Interruption Control**: Boolean flag for interruption permission
- **Real-time Modification**: Parameters can be changed during playback

## API Reference

### Constructors

#### `FAnimationState(void)`

Creates a new animation state with default values.

**Behavior:**
- Initializes `name` to empty string
- Sets `animation` to default (empty) state
- Initializes `transitions` as empty vector
- Sets `playbackSpeed` to 1.0f (normal speed)
- Sets `allowInterruption` to true

#### `FAnimationState(const FString& name, const AnimationType& animation)`

Creates a new animation state with specified name and animation.

**Parameters:**
- `name`: The unique identifier for this state
- `animation`: The animation asset to associate with this state

**Behavior:**
- Sets `name` to the provided value
- Copies or moves the animation into the state
- Initializes `transitions` as empty vector
- Sets `playbackSpeed` to 1.0f
- Sets `allowInterruption` to true

### Public Members

#### `FString name`

The unique name identifier for this animation state.

**Type:** `FString`
**Default:** `""` (empty string)
**Constraints:** Must be unique within the state machine
**Usage:** Used for state machine navigation and debugging

#### `AnimationType animation`

The animation asset associated with this state.

**Type:** `TVariant<FAnimation2D>`
**Default:** Empty variant
**Usage:** Contains the actual animation data to be played

#### `TVector<FAnimationTransition> transitions`

Collection of possible transitions from this state to others.

**Type:** `TVector<FAnimationTransition>`
**Default:** Empty vector
**Usage:** Evaluated during state machine updates to determine state changes

#### `Float32 playbackSpeed`

Multiplier for animation playback speed.

**Type:** `Float32`
**Default:** `1.0f`
**Range:** `> 0.0f` (negative values may cause undefined behavior)
**Units:** Multiplier (1.0f = normal speed, 2.0f = double speed)

#### `bool allowInterruption`

Flag indicating whether this state can be interrupted by transitions.

**Type:** `bool`
**Default:** `true`
**Usage:** When false, prevents transitions until animation completes

## Usage Examples

### Basic State Creation

```cpp
// Create a simple idle state
FAnimation2D idleAnimation = LoadAnimation("Character_Idle.anim");
FAnimationState idleState("Idle", idleAnimation);

// Create a walking state with custom speed
FAnimation2D walkAnimation = LoadAnimation("Character_Walk.anim");
FAnimationState walkState("Walk", walkAnimation);
walkState.playbackSpeed = 1.2f; // Slightly faster walking
```

### State with Transitions

```cpp
FAnimationState jumpState("Jump", LoadAnimation("Character_Jump.anim"));
jumpState.allowInterruption = false; // Don't interrupt jump animation

// Add transition to fall state when jump peaks
FAnimationTransition toFall;
toFall.targetState = "Fall";
toFall.blendTime = 0.1f;
toFall.condition = [](void* userData) -> bool {
    CharacterController* character = static_cast<CharacterController*>(userData);
    return character->IsAtJumpPeak();
};
toFall.userData = characterController;

jumpState.transitions.push_back(toFall);

// Add transition to land state when grounded
FAnimationTransition toLand;
toLand.targetState = "Land";
toLand.blendTime = 0.2f;
toLand.condition = [](void* userData) -> bool {
    CharacterController* character = static_cast<CharacterController*>(userData);
    return character->IsGrounded();
};
toLand.userData = characterController;

jumpState.transitions.push_back(toLand);
```

### Complex Character State Machine

```cpp
class CharacterAnimationController {
private:
    std::unordered_map<FString, FAnimationState> states;
    FString currentState;
    CharacterController* character;

public:
    void InitializeStates() {
        // Idle state
        FAnimationState idleState("Idle", LoadAnimation("Idle.anim"));
        idleState.playbackSpeed = 0.8f; // Slightly slower idle

        FAnimationTransition idleToWalk;
        idleToWalk.targetState = "Walk";
        idleToWalk.blendTime = 0.15f;
        idleToWalk.condition = [this](void*) {
            return character->GetMovementSpeed() > 0.1f;
        };
        idleState.transitions.push_back(idleToWalk);

        states["Idle"] = idleState;

        // Walk state
        FAnimationState walkState("Walk", LoadAnimation("Walk.anim"));

        FAnimationTransition walkToIdle;
        walkToIdle.targetState = "Idle";
        walkToIdle.blendTime = 0.2f;
        walkToIdle.condition = [this](void*) {
            return character->GetMovementSpeed() < 0.1f;
        };
        walkState.transitions.push_back(walkToIdle);

        FAnimationTransition walkToRun;
        walkToRun.targetState = "Run";
        walkToRun.blendTime = 0.1f;
        walkToRun.condition = [this](void*) {
            return character->GetMovementSpeed() > 5.0f;
        };
        walkState.transitions.push_back(walkToRun);

        states["Walk"] = walkState;

        // Run state
        FAnimationState runState("Run", LoadAnimation("Run.anim"));
        runState.playbackSpeed = 1.5f; // Faster playback for run

        FAnimationTransition runToWalk;
        runToWalk.targetState = "Walk";
        runToWalk.blendTime = 0.3f;
        runToWalk.condition = [this](void*) {
            return character->GetMovementSpeed() <= 5.0f;
        };
        runState.transitions.push_back(runToWalk);

        states["Run"] = runState;
    }
};
```

### Dynamic State Modification

```cpp
class AdaptiveAnimationController {
private:
    FAnimationState combatState;

public:
    void AdjustCombatSpeed(float intensity) {
        // Adjust playback speed based on combat intensity
        combatState.playbackSpeed = 1.0f + (intensity * 0.5f);

        // Clamp to reasonable range
        combatState.playbackSpeed = std::clamp(combatState.playbackSpeed, 0.5f, 2.0f);
    }

    void EnableCombatInterruption(bool enable) {
        combatState.allowInterruption = enable;
    }

    void AddDynamicTransition(const FString& targetState,
                             TFunction<bool(void*)> condition) {
        FAnimationTransition dynamicTransition;
        dynamicTransition.targetState = targetState;
        dynamicTransition.blendTime = 0.2f;
        dynamicTransition.condition = condition;
        dynamicTransition.userData = this;

        combatState.transitions.push_back(dynamicTransition);
    }
};
```

### State Machine Implementation

```cpp
class AnimationStateMachine {
private:
    std::unordered_map<FString, FAnimationState> states;
    FString currentStateName;
    Float32 blendTime;
    Float32 blendProgress;
    FAnimationState* currentState;
    FAnimationState* targetState;

public:
    void AddState(const FAnimationState& state) {
        states[state.name] = state;
    }

    void SetInitialState(const FString& stateName) {
        currentStateName = stateName;
        currentState = &states[stateName];
        targetState = nullptr;
        blendProgress = 1.0f;
    }

    void Update(Float32 deltaTime) {
        if (!currentState) return;

        // Update current animation
        if (currentState->animation.IsValid()) {
            // Apply playback speed
            Float32 adjustedDelta = deltaTime * currentState->playbackSpeed;
            currentState->animation.Update(adjustedDelta);
        }

        // Check transitions if interruption allowed or animation complete
        bool canTransition = currentState->allowInterruption ||
                           IsAnimationComplete(*currentState);

        if (canTransition && !IsBlending()) {
            EvaluateTransitions();
        }

        // Update blending
        UpdateBlending(deltaTime);
    }

private:
    void EvaluateTransitions() {
        for (const auto& transition : currentState->transitions) {
            if (transition.condition(transition.userData)) {
                TransitionToState(transition.targetState, transition.blendTime);
                break; // Only one transition per update
            }
        }
    }

    void TransitionToState(const FString& newStateName, Float32 blendDuration) {
        auto it = states.find(newStateName);
        if (it != states.end()) {
            targetState = &it->second;
            blendTime = blendDuration;
            blendProgress = 0.0f;

            if (blendTime <= 0.0f) {
                // Instant transition
                CompleteTransition();
            }
        }
    }

    void UpdateBlending(Float32 deltaTime) {
        if (IsBlending()) {
            blendProgress += deltaTime / blendTime;
            if (blendProgress >= 1.0f) {
                CompleteTransition();
            }
        }
    }

    void CompleteTransition() {
        currentState = targetState;
        currentStateName = currentState->name;
        targetState = nullptr;
        blendProgress = 1.0f;
    }

    bool IsBlending() const {
        return targetState != nullptr;
    }

    bool IsAnimationComplete(const FAnimationState& state) const {
        // Implementation depends on animation type
        return state.animation.IsComplete();
    }
};
```

## State Machine Integration

### State Evaluation Flow

The animation state integrates with the state machine through a well-defined evaluation cycle:

1. **Animation Update**: Current state's animation is updated with speed multiplier
2. **Transition Check**: Evaluate outgoing transitions if allowed
3. **Condition Evaluation**: Check each transition's condition function
4. **State Transition**: Execute first valid transition
5. **Blend Management**: Handle smooth transitions between states

### Playback Speed Application

```cpp
// Speed multiplier affects animation timing
Float32 adjustedDeltaTime = deltaTime * state.playbackSpeed;
animation.Update(adjustedDeltaTime);
```

### Interruption Logic

```cpp
bool canTransition = state.allowInterruption || animation.IsComplete();
if (canTransition) {
    // Evaluate transitions
}
```

## Performance Considerations

### Memory Usage

- **Per State**: Moderate memory footprint (animation + transitions)
- **Animation Storage**: Depends on animation type and complexity
- **Transition Vector**: Scales with number of outgoing transitions

### CPU Performance

- **Update Cost**: Animation update + transition evaluation
- **Transition Checks**: Function call overhead per transition
- **Blend Calculations**: Minimal math operations

### Optimization Strategies

1. **Transition Ordering**: Place most likely transitions first
2. **Condition Caching**: Cache expensive condition results
3. **State Pooling**: Reuse common states across state machines

### Best Practices

- **Limit Transitions**: Keep transition count reasonable (2-5 per state)
- **Simple Conditions**: Avoid complex logic in condition functions
- **Speed Ranges**: Keep playback speeds in reasonable ranges (0.1f - 3.0f)
- **State Naming**: Use consistent, descriptive state names

## Architecture Diagrams

### Class Structure

```
FAnimationState
├── name: FString                    // State identifier
├── animation: TVariant<FAnimation2D> // Animation asset
├── transitions: TVector<FAnimationTransition> // Outgoing transitions
├── playbackSpeed: Float32           // Speed multiplier
└── allowInterruption: bool          // Interruption flag
```

### State Machine Flow

```
State Machine ──► Current State ──► Update Animation ──► Check Transitions
      ▲                    │                    │              │
      │                    ▼                    ▼              ▼
      │           Apply Speed Multiplier ──► Evaluate Conditions ──► Valid Transition?
      │                    ▲                    │              │
      │                    │                    ▼              ▼
      └───── Set State ────┴───────────► No Transition ──► Stay in State
                                               │
                                               ▼
                                       Transition Found ──► Start Blend ──► Target State
```

### Animation Update Sequence

```
Game Loop ──► State Machine Update ──► Animation Update ──► Apply Speed ──► Advance Time
      │                    │                    │              │              │
      ▼                    ▼                    ▼              ▼              ▼
Delta Time ──► Current State ──► Animation Asset ──► Speed Multiplier ──► Frame Advance
```

## Troubleshooting

### Common Issues

#### State Transitions Not Working

**Symptoms:** Animation stays in current state despite conditions being met

**Possible Causes:**
- Transitions not added to state
- Condition functions return false
- allowInterruption is false and animation not complete
- State machine not calling Update()

**Solutions:**
```cpp
// Debug transition evaluation
void DebugStateTransitions(const FAnimationState& state) {
    FLogger::Log(ELogLevel::Debug,
        FString::Format("State: {}, Transitions: {}",
            state.name, state.transitions.size()));

    for (size_t i = 0; i < state.transitions.size(); ++i) {
        const auto& transition = state.transitions[i];
        bool conditionResult = transition.condition(transition.userData);
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Transition {}: {} -> {}, Condition: {}",
                i, state.name, transition.targetState, conditionResult));
    }
}

// Check interruption settings
if (!state.allowInterruption && !IsAnimationComplete(state)) {
    FLogger::Log(ELogLevel::Warning,
        FString::Format("State {} cannot transition - interruption disabled", state.name));
}
```

#### Animation Playback Issues

**Symptoms:** Animation plays too fast/slow or doesn't play

**Possible Causes:**
- playbackSpeed is 0 or negative
- Animation asset is invalid
- Update not called with proper delta time

**Solutions:**
```cpp
// Validate playback speed
if (state.playbackSpeed <= 0.0f) {
    FLogger::Log(ELogLevel::Warning,
        FString::Format("Invalid playback speed: {}", state.playbackSpeed));
    state.playbackSpeed = 1.0f;
}

// Check animation validity
if (!state.animation.IsValid()) {
    FLogger::Log(ELogLevel::Error,
        FString::Format("Invalid animation in state: {}", state.name));
}

// Debug delta time
FLogger::Log(ELogLevel::Debug,
    FString::Format("Delta time: {}, Adjusted: {}",
        deltaTime, deltaTime * state.playbackSpeed));
```

#### Memory Issues

**Symptoms:** Crashes or undefined behavior with state management

**Possible Causes:**
- State objects destroyed while in use
- Invalid animation references
- Transition userData pointing to invalid memory

**Solutions:**
```cpp
class SafeStateManager {
private:
    std::unordered_map<FString, std::unique_ptr<FAnimationState>> states;

public:
    FAnimationState* CreateState(const FString& name, const FAnimationState::AnimationType& anim) {
        auto state = std::make_unique<FAnimationState>(name, anim);
        auto* statePtr = state.get();
        states[name] = std::move(state);
        return statePtr;
    }

    FAnimationState* GetState(const FString& name) {
        auto it = states.find(name);
        return it != states.end() ? it->second.get() : nullptr;
    }
};
```

### Performance Issues

**Symptoms:** State machine updates are slow

**Possible Causes:**
- Too many states or transitions
- Expensive condition functions
- Frequent state changes

**Solutions:**
```cpp
// Profile state machine performance
class ProfiledStateMachine : public AnimationStateMachine {
public:
    void Update(Float32 deltaTime) {
        auto start = std::chrono::high_resolution_clock::now();

        AnimationStateMachine::Update(deltaTime);

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        if (duration.count() > 5000) { // > 5ms
            FLogger::Log(ELogLevel::Warning,
                FString::Format("Slow state machine update: {}us", duration.count()));
        }
    }
};
```

### Debug Tools

Enable state machine debugging:

```cpp
class StateMachineDebugger {
public:
    static void LogStateInfo(const FAnimationState& state) {
        FLogger::Log(ELogLevel::Debug, "=== Animation State Info ===");
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Name: {}", state.name));
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Playback Speed: {}", state.playbackSpeed));
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Allow Interruption: {}", state.allowInterruption));
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Transitions: {}", state.transitions.size()));
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Animation Valid: {}", state.animation.IsValid()));
    }

    static void LogStateTransition(const FString& fromState,
                                  const FString& toState,
                                  Float32 blendTime) {
        FLogger::Log(ELogLevel::Info,
            FString::Format("State Transition: {} -> {} (Blend: {}s)",
                fromState, toState, blendTime));
    }
};
```

## Future Enhancements

### Planned Features

1. **State Hierarchies**: Nested state machines within states
2. **State Parameters**: Runtime parameter binding for animations
3. **Transition Priorities**: Priority-based transition evaluation
4. **State Events**: Callbacks for state entry/exit events
5. **Blend Trees**: Advanced blending between multiple animations
6. **State Synchronization**: Multi-character state synchronization
7. **Animation Layers**: Layered animation system with masks

### Extension Points

The state system allows for easy extension:

```cpp
class AdvancedAnimationState : public FAnimationState {
public:
    // Add state parameters
    std::unordered_map<FString, Float32> parameters;

    // Add entry/exit callbacks
    TFunction<void()> onEnter;
    TFunction<void()> onExit;

    // Add state-specific logic
    virtual void OnEnter() {
        if (onEnter) onEnter();
    }

    virtual void OnExit() {
        if (onExit) onExit();
    }

    // Add parameter binding
    void SetParameter(const FString& name, Float32 value) {
        parameters[name] = value;
        // Update animation with new parameter
        UpdateAnimationParameters();
    }

private:
    void UpdateAnimationParameters() {
        // Apply parameters to animation
        for (const auto& param : parameters) {
            animation.SetParameter(param.first, param.second);
        }
    }
};
```

### Advanced Features

Future versions may include:
- **Motion Matching**: Data-driven state transitions
- **Procedural States**: Runtime-generated animation states
- **State Prediction**: Predictive state transitions
- **Multi-threaded Evaluation**: Parallel state machine updates
- **Visual Debugging**: Real-time state machine visualization

---

*This documentation covers the complete FAnimationState implementation as of TKD Engine v1.0.0. For the latest updates and additional examples, refer to the engine's source code and test suites.*
