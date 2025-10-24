# Animation Transition Documentation

## Overview

The `FAnimationTransition` class is a fundamental component of the TKD Game Engine's animation state machine system, representing the transition logic between different animation states. It encapsulates the conditions, timing, and blending parameters required to move from one animation state to another in a controlled and predictable manner.

### Key Features

- **Conditional Transitions**: Function-based conditions for state changes
- **Blend Control**: Configurable blending time between states
- **User Data Support**: Flexible context passing to condition functions
- **Lightweight Design**: Minimal memory footprint with simple data structure
- **Type Safety**: Template-free design for maximum compatibility
- **State Machine Integration**: Designed for use within animation state machines

### Architecture

```cpp
class FAnimationTransition
{
public:
    FString targetState;                // Target state name
    TFunction<bool(void*)> condition;   // Transition condition function
    Float32 blendTime;                  // Blend time in seconds
    void* userData;                     // User data for condition function
};
```

The `FAnimationTransition` is a simple data structure that defines the rules for transitioning between animation states in a state machine.

## Core Components

### Transition Target

- **Target State**: The name of the destination animation state
- **String-Based**: Uses `FString` for state identification
- **State Machine Scope**: Valid within the containing state machine

### Condition System

- **Function Pointer**: `TFunction<bool(void*)>` for condition evaluation
- **Context Parameter**: `void* userData` passed to condition function
- **Boolean Result**: Returns true when transition should occur

### Blending Parameters

- **Blend Time**: Duration in seconds for smooth state transitions
- **Float Precision**: `Float32` for timing accuracy
- **Zero Allowed**: Instant transitions when blend time is 0.0f

## API Reference

### Constructor

#### `FAnimationTransition(void)`

Creates a new animation transition with default values.

**Behavior:**
- Initializes `targetState` to empty string
- Sets `condition` to `nullptr`
- Sets `blendTime` to 0.0f
- Sets `userData` to `nullptr`

### Public Members

#### `FString targetState`

The name of the target animation state to transition to.

**Type:** `FString`
**Default:** `""` (empty string)
**Usage:** Must match a valid state name in the state machine

#### `TFunction<bool(void*)> condition`

Function that determines when the transition should occur.

**Type:** `TFunction<bool(void*)>`
**Default:** `nullptr`
**Parameters:**
- `void*`: User data pointer passed during evaluation
**Returns:** `true` if transition should occur, `false` otherwise

#### `Float32 blendTime`

Time in seconds to blend between the current and target states.

**Type:** `Float32`
**Default:** `0.0f`
**Range:** `>= 0.0f`
**Units:** Seconds

#### `void* userData`

Pointer to user-defined data passed to the condition function.

**Type:** `void*`
**Default:** `nullptr`
**Usage:** Can point to any user-defined structure or context

## Usage Examples

### Basic Transition Setup

```cpp
// Create a transition to a "Run" state
FAnimationTransition runTransition;
runTransition.targetState = "Run";
runTransition.blendTime = 0.2f; // 200ms blend

// Simple condition: always true (immediate transition)
runTransition.condition = [](void* userData) -> bool {
    return true;
};
```

### Conditional Transitions

```cpp
struct PlayerContext {
    float speed;
    bool isGrounded;
    bool jumpPressed;
};

PlayerContext playerCtx;

// Transition to Jump when conditions met
FAnimationTransition jumpTransition;
jumpTransition.targetState = "Jump";
jumpTransition.blendTime = 0.1f;
jumpTransition.userData = &playerCtx;

jumpTransition.condition = [](void* userData) -> bool {
    PlayerContext* ctx = static_cast<PlayerContext*>(userData);
    return ctx->isGrounded && ctx->jumpPressed;
};

// Transition to Fall when leaving ground
FAnimationTransition fallTransition;
fallTransition.targetState = "Fall";
fallTransition.blendTime = 0.15f;
fallTransition.userData = &playerCtx;

fallTransition.condition = [](void* userData) -> bool {
    PlayerContext* ctx = static_cast<PlayerContext*>(userData);
    return !ctx->isGrounded;
};
```

### Speed-Based Transitions

```cpp
class CharacterAnimationController {
private:
    FAnimationTransition walkToRun;
    FAnimationTransition runToWalk;
    float speedThreshold;

public:
    void InitializeTransitions() {
        speedThreshold = 5.0f;

        // Walk to Run transition
        walkToRun.targetState = "Run";
        walkToRun.blendTime = 0.3f;
        walkToRun.userData = this;

        walkToRun.condition = [](void* userData) -> bool {
            CharacterAnimationController* controller =
                static_cast<CharacterAnimationController*>(userData);
            return controller->GetCurrentSpeed() > controller->speedThreshold;
        };

        // Run to Walk transition
        runToWalk.targetState = "Walk";
        runToWalk.blendTime = 0.5f; // Slower blend back to walk
        runToWalk.userData = this;

        runToWalk.condition = [](void* userData) -> bool {
            CharacterAnimationController* controller =
                static_cast<CharacterAnimationController*>(userData);
            return controller->GetCurrentSpeed() <= controller->speedThreshold;
        };
    }

    float GetCurrentSpeed() const {
        // Return character's current movement speed
        return m_character->GetVelocity().Length();
    }
};
```

### Complex State Machines

```cpp
class AnimationStateMachine {
private:
    std::vector<FAnimationTransition> transitions;
    FString currentState;

public:
    void AddTransition(const FString& fromState, const FAnimationTransition& transition) {
        // Store transition with its source state
        m_transitions[fromState].push_back(transition);
    }

    void Update(Float32 deltaTime) {
        // Check all transitions from current state
        auto it = m_transitions.find(currentState);
        if (it != m_transitions.end()) {
            for (const auto& transition : it->second) {
                if (transition.condition && transition.condition(transition.userData)) {
                    // Execute transition
                    TransitionToState(transition.targetState, transition.blendTime);
                    break; // Only one transition per frame
                }
            }
        }
    }

private:
    void TransitionToState(const FString& newState, Float32 blendTime) {
        if (blendTime > 0.0f) {
            // Start blending animation
            StartBlendAnimation(currentState, newState, blendTime);
        } else {
            // Instant transition
            currentState = newState;
        }
    }
};
```

### Input-Based Transitions

```cpp
class InputDrivenAnimator {
private:
    FAnimationTransition attackTransition;
    FAnimationTransition defendTransition;
    FInputManager* inputManager;

public:
    void SetupInputTransitions() {
        // Attack transition
        attackTransition.targetState = "Attack";
        attackTransition.blendTime = 0.1f;
        attackTransition.userData = inputManager;

        attackTransition.condition = [](void* userData) -> bool {
            FInputManager* input = static_cast<FInputManager*>(userData);
            return input->IsActionPressed("Attack");
        };

        // Defend transition (hold-based)
        defendTransition.targetState = "Defend";
        defendTransition.blendTime = 0.2f;
        defendTransition.userData = inputManager;

        defendTransition.condition = [](void* userData) -> bool {
            FInputManager* input = static_cast<FInputManager*>(userData);
            return input->IsActionHeld("Defend");
        };
    }
};
```

## State Machine Integration

### Transition Evaluation

Transitions are evaluated in the context of a state machine:

```cpp
class AnimationStateMachine {
public:
    void EvaluateTransitions() {
        for (const auto& transition : currentStateTransitions) {
            if (transition.condition(transition.userData)) {
                ExecuteTransition(transition);
                break;
            }
        }
    }

private:
    void ExecuteTransition(const FAnimationTransition& transition) {
        // 1. Start blend if blendTime > 0
        if (transition.blendTime > 0.0f) {
            StartBlending(currentAnimation, transition.targetState, transition.blendTime);
        }

        // 2. Update current state
        currentState = transition.targetState;

        // 3. Trigger transition events
        OnStateTransition(currentState, transition.targetState);
    }
};
```

### Blend Timing

The blend time controls smooth interpolation between states:

- **blendTime = 0.0f**: Instant transition (no blending)
- **blendTime > 0.0f**: Smooth interpolation over specified duration
- **Blend Factor**: `currentTime / blendTime` (0.0 to 1.0)

### Condition Context

The `userData` parameter allows conditions to access relevant context:

```cpp
struct AnimationContext {
    float deltaTime;
    FVector3 velocity;
    bool isGrounded;
    std::unordered_map<FString, bool> flags;
};

AnimationContext context;

// Condition can access all context data
transition.condition = [](void* userData) -> bool {
    AnimationContext* ctx = static_cast<AnimationContext*>(userData);
    return ctx->velocity.Length() > 10.0f && ctx->isGrounded;
};
```

## Performance Considerations

### Memory Usage

- **Per Transition**: Minimal memory footprint (~32 bytes)
- **Function Pointer**: Small overhead for condition storage
- **String Storage**: `FString` for target state names

### CPU Performance

- **Evaluation Cost**: Function call overhead per transition check
- **Condition Complexity**: Depends on user-defined condition logic
- **Transition Frequency**: Typically evaluated every frame

### Optimization Strategies

1. **Condition Caching**: Cache expensive condition results
2. **Transition Ordering**: Order transitions by likelihood
3. **State Grouping**: Group related transitions for batch evaluation

### Best Practices

- **Simple Conditions**: Keep condition functions lightweight
- **Context Reuse**: Share context objects between related transitions
- **Blend Optimization**: Use appropriate blend times (0.1-0.5 seconds typical)
- **Memory Management**: Properly manage userData lifetime

## Architecture Diagrams

### Class Structure

```
FAnimationTransition
├── targetState: FString          // Destination state name
├── condition: TFunction<bool(void*)>  // Transition condition
├── blendTime: Float32            // Blend duration in seconds
└── userData: void*               // Context data pointer
```

### State Machine Flow

```
Current State ──► Evaluate Transitions ──► Condition Check ──► True ──► Execute Transition
     │                     │                        │
     │                     │                        ▼
     │                     │                   False ──► Stay in State
     │                     ▼
     │            [Transition List]
     │                     │
     ▼                     ▼
Update Animation ──◄─── Blend Animation ──► Target State
     ▲                     ▲
     │                     │
     └───── blendTime ─────┘
```

### Transition Evaluation Sequence

```
Animation Update ──► Check Transition Conditions ──► Evaluate userData
         │                        │                          │
         ▼                        ▼                          ▼
    Delta Time ──► Condition Function ──► Context Access ──► Boolean Result
         ▲                        ▲                          ▲
         │                        │                          │
         └───── Frame Loop ───────┴───── State Machine ──────┘
```

## Troubleshooting

### Common Issues

#### Transitions Not Triggering

**Symptoms:** Animation stays in current state despite conditions being met

**Possible Causes:**
- Condition function returns false
- userData is null or invalid
- Condition function has bugs
- Transition not added to state machine

**Solutions:**
```cpp
// Debug condition evaluation
bool debugCondition = transition.condition(transition.userData);
FLogger::Log(ELogLevel::Debug,
    FString::Format("Transition condition result: {}", debugCondition));

// Verify userData
if (transition.userData == nullptr) {
    FLogger::Log(ELogLevel::Warning, "Transition userData is null");
}

// Test condition in isolation
bool testResult = transition.condition(testContext);
FLogger::Log(ELogLevel::Debug,
    FString::Format("Isolated condition test: {}", testResult));
```

#### Invalid Transitions

**Symptoms:** Crashes or undefined behavior during transition evaluation

**Possible Causes:**
- targetState is empty or invalid
- condition function pointer is null
- userData points to invalid memory

**Solutions:**
```cpp
// Validate transition before use
bool IsValidTransition(const FAnimationTransition& transition) {
    return !transition.targetState.IsEmpty() &&
           transition.condition != nullptr &&
           transition.blendTime >= 0.0f;
}

// Safe condition evaluation
bool SafeEvaluateCondition(const FAnimationTransition& transition) {
    if (transition.condition == nullptr) {
        FLogger::Log(ELogLevel::Error, "Transition condition is null");
        return false;
    }

    try {
        return transition.condition(transition.userData);
    } catch (const std::exception& e) {
        FLogger::Log(ELogLevel::Error,
            FString::Format("Condition evaluation failed: {}", e.what()));
        return false;
    }
}
```

#### Blend Issues

**Symptoms:** Jerky or incorrect blending between states

**Possible Causes:**
- blendTime is too short or too long
- Blend calculation errors
- Animation data incompatibility

**Solutions:**
```cpp
// Validate blend time
if (transition.blendTime < 0.0f) {
    FLogger::Log(ELogLevel::Warning,
        FString::Format("Invalid blend time: {}", transition.blendTime));
    transition.blendTime = 0.0f;
}

// Debug blend progress
float blendProgress = currentBlendTime / transition.blendTime;
FLogger::Log(ELogLevel::Debug,
    FString::Format("Blend progress: {}%", blendProgress * 100.0f));
```

### Memory Management Issues

**Symptoms:** Crashes due to invalid userData pointers

**Possible Causes:**
- userData object destroyed before transition evaluation
- Incorrect casting of userData
- Lifetime management issues

**Solutions:**
```cpp
class SafeTransitionManager {
private:
    std::vector<std::unique_ptr<FAnimationTransition>> transitions;
    std::shared_ptr<AnimationContext> sharedContext;

public:
    void AddTransition(const FString& targetState,
                      TFunction<bool(void*)> condition,
                      Float32 blendTime) {
        auto transition = std::make_unique<FAnimationTransition>();
        transition->targetState = targetState;
        transition->condition = condition;
        transition->blendTime = blendTime;
        transition->userData = sharedContext.get();

        transitions.push_back(std::move(transition));
    }
};
```

### Performance Issues

**Symptoms:** State machine evaluation is slow

**Possible Causes:**
- Too many transitions per state
- Expensive condition functions
- Frequent transition evaluations

**Solutions:**
```cpp
// Profile condition performance
class ProfiledTransition : public FAnimationTransition {
public:
    bool EvaluateCondition() {
        auto start = std::chrono::high_resolution_clock::now();
        bool result = condition(userData);
        auto end = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        if (duration.count() > 1000) { // > 1ms
            FLogger::Log(ELogLevel::Warning,
                FString::Format("Slow condition evaluation: {}us", duration.count()));
        }

        return result;
    }
};
```

### Debug Tools

Enable transition debugging:

```cpp
class TransitionDebugger {
public:
    static void LogTransitionEvaluation(const FAnimationTransition& transition,
                                       const FString& fromState) {
        FLogger::Log(ELogLevel::Debug, "=== Transition Evaluation ===");
        FLogger::Log(ELogLevel::Debug,
            FString::Format("From: {} -> To: {}", fromState, transition.targetState));
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Blend Time: {}s", transition.blendTime));
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Has Condition: {}", transition.condition != nullptr));
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Has UserData: {}", transition.userData != nullptr));
    }

    static void LogTransitionResult(bool conditionResult, bool executed) {
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Condition Result: {}, Executed: {}",
                conditionResult, executed));
    }
};
```

## Future Enhancements

### Planned Features

1. **Transition Priorities**: Priority-based transition evaluation
2. **Transition Events**: Callbacks for transition lifecycle
3. **Blend Curves**: Non-linear blending functions (ease-in, ease-out)
4. **Transition Groups**: Logical grouping of related transitions
5. **Transition History**: Tracking of recent state changes
6. **Conditional Dependencies**: Transitions that depend on other transitions
7. **Transition Validation**: Runtime validation of transition parameters

### Extension Points

The transition system allows for easy extension:

```cpp
class AdvancedTransition : public FAnimationTransition {
public:
    // Add priority system
    int priority = 0;

    // Add blend curve
    enum class BlendCurve { Linear, EaseIn, EaseOut, EaseInOut };
    BlendCurve blendCurve = BlendCurve::Linear;

    // Add transition events
    TFunction<void()> onTransitionStart;
    TFunction<void()> onTransitionComplete;

    // Add validation
    bool IsValid() const {
        return !targetState.IsEmpty() &&
               condition != nullptr &&
               blendTime >= 0.0f &&
               priority >= 0;
    }
};
```

### Advanced Features

Future versions may include:
- **Hierarchical States**: Transitions between state groups
- **Concurrent Transitions**: Multiple simultaneous state changes
- **Transition Queues**: Queued transition execution
- **Transition Cancellation**: Ability to cancel ongoing transitions
- **Transition Debugging**: Visual debugging tools for state machines

---

*This documentation covers the complete FAnimationTransition implementation as of TKD Engine v1.0.0. For the latest updates and additional examples, refer to the engine's source code and test suites.*
