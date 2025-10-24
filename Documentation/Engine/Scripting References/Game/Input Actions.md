# Input Actions Documentation

## Overview

The `UInputAction` class represents discrete input actions in the TKD Game Engine. Unlike continuous axes, actions have distinct states (pressed, held, released) and are ideal for buttons, keys, and other binary inputs. Actions support multiple input bindings, event-driven programming, and automatic state management.

### Key Features

- **Discrete States**: Pressed, held, released, and idle states
- **Multi-Binding**: Single action can be triggered by multiple inputs
- **Event System**: Emits events for state transitions
- **Automatic State Management**: Handles press/hold/release logic
- **Input Validation**: Ensures valid input bindings
- **Thread-Safe Events**: Event emission is thread-safe
- **Dynamic Binding**: Runtime addition/removal of input bindings
- **State Querying**: Real-time state inspection

### Architecture

```cpp
class UInputAction : public TEventEmitter<TEvents<UInputAction>::All>
{
private:
    FString m_name;                 // Action identifier
    std::vector<EInput> m_inputs;   // Bound inputs
    bool m_pressed;                 // Current pressed state
    bool m_held;                    // Current held state
    bool m_released;                // Current released state
    EInput m_currentInput;          // Last active input

public:
    // Event types
    struct Pressed { EInput input; };
    struct Released { EInput input; };
    struct Held { EInput input; };

    // State management
    void Press(EInput input);
    void Release(EInput input);
    void Hold(EInput input);
    void Idle();

    // Configuration
    bool Bind(EInput input);
    bool Unbind(EInput input);
    void Reset();

    // Queries
    const FString& GetName() const;
    const std::vector<EInput>& GetInputs() const;
    EInput GetCurrentInput() const;
    bool IsPressed() const;
    bool IsHeld() const;
    bool IsReleased() const;
};
```

The `UInputAction` operates as an event-driven state machine, automatically managing transitions between input states and notifying listeners of changes.

## Core Components

### State Machine

Actions implement a finite state machine for input handling:

```cpp
enum class ActionState {
    Idle,      // No input active
    Pressed,   // Just pressed this frame
    Held,      // Being held down
    Released   // Just released this frame
};
```

### Event System

Actions emit events for all state transitions:

```cpp
struct ActionEvents {
    Pressed   // Emitted when action becomes pressed
    Released  // Emitted when action becomes released
    Held      // Emitted when action is held (after pressed)
};
```

### Input Binding

Actions support multiple input sources for the same logical action:

```cpp
struct InputBinding {
    std::vector<EInput> inputs;  // All bound inputs
    EInput activeInput;          // Currently active input
    bool allowMultiple;          // Allow multiple simultaneous inputs
};
```

## API Reference

### Constructor & Initialization

#### `UInputAction(const FString& name, const std::vector<EInput>& inputs)`

Creates an input action with specified name and input bindings.

**Parameters:**
- `name`: Unique identifier for the action (e.g., "Jump", "Attack")
- `inputs`: Vector of input enumerations that trigger this action

**Behavior:**
- Initializes action name and input bindings
- Sets initial state to idle (not pressed/held/released)
- Current input set to Unknown
- No events emitted during construction

**Example:**
```cpp
// Create jump action with keyboard and gamepad bindings
UInputAction jumpAction("Jump", {
    EInput::Keyboard_Space,
    EInput::GamePad_A
});

// Create attack action with mouse and gamepad bindings
UInputAction attackAction("Attack", {
    EInput::Mouse_Left,
    EInput::GamePad_X
});
```

### State Management Methods

#### `void Press(EInput input)`

Simulates pressing an input bound to this action.

**Parameters:**
- `input`: The input that was pressed

**Behavior:**
- Checks if input is bound to this action
- Sets pressed state to true
- Sets held state to false
- Sets released state to false
- Updates current input to the pressed input
- Emits `Pressed` event with input parameter

**Usage:**
```cpp
// Called by input manager when input is detected
jumpAction.Press(EInput::Keyboard_Space);
```

#### `void Release(EInput input)`

Simulates releasing an input bound to this action.

**Parameters:**
- `input`: The input that was released

**Behavior:**
- Checks if input is bound to this action
- Sets pressed state to false
- Sets held state to false
- Sets released state to true
- Updates current input to the released input
- Emits `Released` event with input parameter

#### `void Hold(EInput input)`

Simulates holding an input bound to this action.

**Parameters:**
- `input`: The input being held

**Behavior:**
- Checks if input is bound to this action
- Sets held state to true
- Updates current input to the held input
- Emits `Held` event with input parameter

**Note:** Typically called after `Press()` to indicate continuous holding

#### `void Idle(void)`

Resets the action to idle state (no active input).

**Behavior:**
- Sets pressed state to false
- Sets held state to false
- Sets released state to false
- Current input remains unchanged
- No events emitted

**Usage:**
```cpp
// Reset action when no inputs are active
if (!anyInputActive) {
    action.Idle();
}
```

### Configuration Methods

#### `bool Bind(EInput input)`

Adds an input binding to this action.

**Parameters:**
- `input`: The input to bind

**Returns:** True if binding was added, false if input was already bound

**Behavior:**
- Checks if input is already bound
- Adds input to bindings vector if not present
- Returns success status

**Example:**
```cpp
// Add additional binding for jump action
bool success = jumpAction.Bind(EInput::GamePad_Y);
if (success) {
    FLogger::Log(ELogLevel::Info, "Added gamepad Y binding to jump");
}
```

#### `bool Unbind(EInput input)`

Removes an input binding from this action.

**Parameters:**
- `input`: The input to remove

**Returns:** True if binding was removed, false if input was not found

**Behavior:**
- Searches for input in bindings vector
- Removes input if found
- Returns success status

**Example:**
```cpp
// Remove keyboard binding
bool success = jumpAction.Unbind(EInput::Keyboard_Space);
if (success) {
    FLogger::Log(ELogLevel::Info, "Removed spacebar binding from jump");
}
```

#### `void Reset(void)`

Resets the action to its default state.

**Behavior:**
- Sets all state flags to false
- Current input set to Unknown
- No events emitted
- Bindings remain unchanged

**Usage:**
```cpp
// Reset action for cleanup or reinitialization
action.Reset();
```

### Query Methods

#### `const FString& GetName(void) const`

Gets the action's name identifier.

**Returns:** Constant reference to the action name string

#### `const std::vector<EInput>& GetInputs(void) const`

Gets all input bindings for this action.

**Returns:** Constant reference to vector of bound inputs

**Example:**
```cpp
const auto& inputs = jumpAction.GetInputs();
FLogger::Log(ELogLevel::Debug,
    FString::Format("Jump action has {} bindings", inputs.size()));

for (EInput input : inputs) {
    FLogger::Log(ELogLevel::Debug,
        FString::Format("  Bound to input: {}", (int)input));
}
```

#### `EInput GetCurrentInput(void) const`

Gets the currently active input for this action.

**Returns:** The input that last triggered a state change, or `EInput::Unknown` if none

#### `bool IsPressed(void) const`

Checks if the action was pressed this frame.

**Returns:** True if action is in pressed state, false otherwise

#### `bool IsHeld(void) const`

Checks if the action is currently being held.

**Returns:** True if action is in held state, false otherwise

#### `bool IsReleased(void) const`

Checks if the action was released this frame.

**Returns:** True if action is in released state, false otherwise

## Usage Examples

### Basic Action Setup and Event Handling

```cpp
class APlayerController : public AController
{
public:
    virtual void BeginPlay() override
    {
        AController::BeginPlay();

        // Create input actions
        SetupInputActions();

        // Connect event handlers
        ConnectActionEvents();
    }

    virtual void Tick(Float32 deltaTime) override
    {
        AController::Tick(deltaTime);

        // Actions are updated by the input manager
        // Event handlers will be called automatically
    }

private:
    void SetupInputActions()
    {
        // Create movement actions
        moveUpAction = std::make_unique<UInputAction>("MoveUp",
            std::vector<EInput>{EInput::Keyboard_W, EInput::GamePad_DPadUp});

        moveDownAction = std::make_unique<UInputAction>("MoveDown",
            std::vector<EInput>{EInput::Keyboard_S, EInput::GamePad_DPadDown});

        moveLeftAction = std::make_unique<UInputAction>("MoveLeft",
            std::vector<EInput>{EInput::Keyboard_A, EInput::GamePad_DPadLeft});

        moveRightAction = std::make_unique<UInputAction>("MoveRight",
            std::vector<EInput>{EInput::Keyboard_D, EInput::GamePad_DPadRight});

        // Create game actions
        jumpAction = std::make_unique<UInputAction>("Jump",
            std::vector<EInput>{EInput::Keyboard_Space, EInput::GamePad_A});

        attackAction = std::make_unique<UInputAction>("Attack",
            std::vector<EInput>{EInput::Mouse_Left, EInput::GamePad_X});

        pauseAction = std::make_unique<UInputAction>("Pause",
            std::vector<EInput>{EInput::Keyboard_Escape, EInput::GamePad_Start});
    }

    void ConnectActionEvents()
    {
        // Connect movement events
        moveUpAction->On<UInputAction::Events::Pressed>()
            .Connect(this, &APlayerController::OnMoveUpPressed);
        moveUpAction->On<UInputAction::Events::Released>()
            .Connect(this, &APlayerController::OnMoveUpReleased);

        moveDownAction->On<UInputAction::Events::Pressed>()
            .Connect(this, &APlayerController::OnMoveDownPressed);
        moveDownAction->On<UInputAction::Events::Released>()
            .Connect(this, &APlayerController::OnMoveDownReleased);

        moveLeftAction->On<UInputAction::Events::Pressed>()
            .Connect(this, &APlayerController::OnMoveLeftPressed);
        moveLeftAction->On<UInputAction::Events::Released>()
            .Connect(this, &APlayerController::OnMoveLeftReleased);

        moveRightAction->On<UInputAction::Events::Pressed>()
            .Connect(this, &APlayerController::OnMoveRightPressed);
        moveRightAction->On<UInputAction::Events::Released>()
            .Connect(this, &APlayerController::OnMoveRightReleased);

        // Connect game action events
        jumpAction->On<UInputAction::Events::Pressed>()
            .Connect(this, &APlayerController::OnJumpPressed);

        attackAction->On<UInputAction::Events::Pressed>()
            .Connect(this, &APlayerController::OnAttackPressed);
        attackAction->On<UInputAction::Events::Held>()
            .Connect(this, &APlayerController::OnAttackHeld);
        attackAction->On<UInputAction::Events::Released>()
            .Connect(this, &APlayerController::OnAttackReleased);

        pauseAction->On<UInputAction::Events::Pressed>()
            .Connect(this, &APlayerController::OnPausePressed);
    }

    // Movement event handlers
    void OnMoveUpPressed(const UInputAction::Events::Pressed& event)
    {
        movementInput.y = 1.0f;
        UpdateMovement();
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Move up pressed with input: {}", (int)event.input));
    }

    void OnMoveUpReleased(const UInputAction::Events::Released& event)
    {
        movementInput.y = 0.0f;
        UpdateMovement();
    }

    void OnMoveDownPressed(const UInputAction::Events::Pressed& event)
    {
        movementInput.y = -1.0f;
        UpdateMovement();
    }

    void OnMoveDownReleased(const UInputAction::Events::Released& event)
    {
        movementInput.y = 0.0f;
        UpdateMovement();
    }

    void OnMoveLeftPressed(const UInputAction::Events::Pressed& event)
    {
        movementInput.x = -1.0f;
        UpdateMovement();
    }

    void OnMoveLeftReleased(const UInputAction::Events::Released& event)
    {
        movementInput.x = 0.0f;
        UpdateMovement();
    }

    void OnMoveRightPressed(const UInputAction::Events::Pressed& event)
    {
        movementInput.x = 1.0f;
        UpdateMovement();
    }

    void OnMoveRightReleased(const UInputAction::Events::Released& event)
    {
        movementInput.x = 0.0f;
        UpdateMovement();
    }

    // Game action event handlers
    void OnJumpPressed(const UInputAction::Events::Pressed& event)
    {
        if (playerPawn) {
            playerPawn->Jump();
            FLogger::Log(ELogLevel::Debug, "Player jumped");
        }
    }

    void OnAttackPressed(const UInputAction::Events::Pressed& event)
    {
        if (playerPawn) {
            playerPawn->StartAttack();
            FLogger::Log(ELogLevel::Debug, "Attack started");
        }
    }

    void OnAttackHeld(const UInputAction::Events::Held& event)
    {
        if (playerPawn) {
            playerPawn->ChargeAttack();
        }
    }

    void OnAttackReleased(const UInputAction::Events::Released& event)
    {
        if (playerPawn) {
            playerPawn->ReleaseAttack();
            FLogger::Log(ELogLevel::Debug, "Attack released");
        }
    }

    void OnPausePressed(const UInputAction::Events::Pressed& event)
    {
        gamePaused = !gamePaused;
        if (gamePaused) {
            ShowPauseMenu();
            FLogger::Log(ELogLevel::Debug, "Game paused");
        } else {
            HidePauseMenu();
            FLogger::Log(ELogLevel::Debug, "Game resumed");
        }
    }

    void UpdateMovement()
    {
        if (playerPawn) {
            FVector2 movement = movementInput * moveSpeed;
            playerPawn->Move(movement);
        }
    }

private:
    // Input actions
    std::unique_ptr<UInputAction> moveUpAction;
    std::unique_ptr<UInputAction> moveDownAction;
    std::unique_ptr<UInputAction> moveLeftAction;
    std::unique_ptr<UInputAction> moveRightAction;
    std::unique_ptr<UInputAction> jumpAction;
    std::unique_ptr<UInputAction> attackAction;
    std::unique_ptr<UInputAction> pauseAction;

    // Player state
    APawn* playerPawn;
    FVector2 movementInput;
    float moveSpeed;
    bool gamePaused;
};
```

### Advanced Action Processing with Combo System

```cpp
class AComboFighterController : public APlayerController
{
public:
    virtual void BeginPlay() override
    {
        APlayerController::BeginPlay();

        // Create basic attack actions
        lightAttackAction = std::make_unique<UInputAction>("LightAttack",
            std::vector<EInput>{EInput::Keyboard_J, EInput::GamePad_X});

        heavyAttackAction = std::make_unique<UInputAction>("HeavyAttack",
            std::vector<EInput>{EInput::Keyboard_K, EInput::GamePad_Y});

        specialAttackAction = std::make_unique<UInputAction>("SpecialAttack",
            std::vector<EInput>{EInput::Keyboard_L, EInput::GamePad_B});

        // Connect combo system
        ConnectComboEvents();
    }

    virtual void Tick(Float32 deltaTime) override
    {
        APlayerController::Tick(deltaTime);

        // Update combo system
        UpdateCombos(deltaTime);
    }

private:
    void ConnectComboEvents()
    {
        // Connect attack events to combo processor
        lightAttackAction->On<UInputAction::Events::Pressed>()
            .Connect(this, &AComboFighterController::OnAttackInput);
        heavyAttackAction->On<UInputAction::Events::Pressed>()
            .Connect(this, &AComboFighterController::OnAttackInput);
        specialAttackAction->On<UInputAction::Events::Pressed>()
            .Connect(this, &AComboFighterController::OnAttackInput);
    }

    void OnAttackInput(const UInputAction::Events::Pressed& event)
    {
        // Add input to combo buffer
        ComboInput input;
        input.attackType = GetAttackTypeFromInput(event.input);
        input.timestamp = GetWorld()->GetTime();

        comboBuffer.push_back(input);

        // Limit buffer size
        if (comboBuffer.size() > 10) {
            comboBuffer.erase(comboBuffer.begin());
        }

        // Try to match combos
        CheckComboMatches();
    }

    void UpdateCombos(Float32 deltaTime)
    {
        // Update combo timers
        for (auto& combo : activeCombos) {
            combo.timer -= deltaTime;
            if (combo.timer <= 0.0f) {
                // Combo failed
                combo.currentStep = 0;
                combo.timer = 0.0f;
                FLogger::Log(ELogLevel::Debug,
                    FString::Format("Combo '{}' timed out", combo.name));
            }
        }

        // Remove expired combos
        activeCombos.erase(
            std::remove_if(activeCombos.begin(), activeCombos.end(),
                [](const ActiveCombo& c) { return c.timer <= 0.0f; }),
            activeCombos.end());
    }

    void CheckComboMatches()
    {
        float currentTime = GetWorld()->GetTime();

        // Check each combo pattern
        for (const auto& [comboName, pattern] : comboPatterns) {
            if (TryMatchCombo(comboName, pattern, currentTime)) {
                ExecuteCombo(comboName);
                break; // Only execute one combo at a time
            }
        }
    }

    bool TryMatchCombo(const FString& comboName, const ComboPattern& pattern, float currentTime)
    {
        // Find or create active combo
        auto it = std::find_if(activeCombos.begin(), activeCombos.end(),
            [comboName](const ActiveCombo& c) { return c.name == comboName; });

        if (it == activeCombos.end()) {
            // Start new combo attempt
            activeCombos.push_back({comboName, 0, pattern[0].timeWindow});
            it = activeCombos.end() - 1;
        }

        // Check if current input matches expected input
        if (comboBuffer.back().attackType == pattern[it->currentStep].attackType) {
            it->currentStep++;

            // Check if combo completed
            if (it->currentStep >= pattern.size()) {
                activeCombos.erase(it); // Remove completed combo
                return true;
            } else {
                // Set timer for next input
                it->timer = pattern[it->currentStep].timeWindow;
            }
        }

        return false;
    }

    void ExecuteCombo(const FString& comboName)
    {
        FLogger::Log(ELogLevel::Info,
            FString::Format("Combo executed: {}", comboName));

        if (comboName == "Hadouken") {
            playerPawn->CastHadouken();
        } else if (comboName == "Shoryuken") {
            playerPawn->CastShoryuken();
        } else if (comboName == "Super") {
            playerPawn->CastSuperMove();
        }

        // Clear combo buffer after successful combo
        comboBuffer.clear();
    }

    void InitializeComboPatterns()
    {
        // Define combo patterns
        comboPatterns["Hadouken"] = {
            {EAttackType::Light, 0.3f},   // Light attack
            {EAttackType::Light, 0.3f},   // Light attack
            {EAttackType::Heavy, 0.2f}    // Heavy attack (finish)
        };

        comboPatterns["Shoryuken"] = {
            {EAttackType::Heavy, 0.4f},   // Heavy attack
            {EAttackType::Light, 0.2f},   // Light attack
            {EAttackType::Special, 0.1f}  // Special attack (finish)
        };

        comboPatterns["Super"] = {
            {EAttackType::Light, 0.2f},   // Light
            {EAttackType::Heavy, 0.2f},   // Heavy
            {EAttackType::Light, 0.2f},   // Light
            {EAttackType::Heavy, 0.2f},   // Heavy
            {EAttackType::Special, 0.1f}  // Special (finish)
        };
    }

    EAttackType GetAttackTypeFromInput(EInput input) const
    {
        if (input == EInput::Keyboard_J || input == EInput::GamePad_X) {
            return EAttackType::Light;
        } else if (input == EInput::Keyboard_K || input == EInput::GamePad_Y) {
            return EAttackType::Heavy;
        } else if (input == EInput::Keyboard_L || input == EInput::GamePad_B) {
            return EAttackType::Special;
        }
        return EAttackType::None;
    }

private:
    enum class EAttackType { None, Light, Heavy, Special };

    struct ComboStep {
        EAttackType attackType;
        float timeWindow; // Time allowed for this step
    };

    struct ComboInput {
        EAttackType attackType;
        float timestamp;
    };

    struct ActiveCombo {
        FString name;
        size_t currentStep;
        float timer;
    };

    using ComboPattern = std::vector<ComboStep>;

    // Combo system
    std::unordered_map<FString, ComboPattern> comboPatterns;
    std::vector<ComboInput> comboBuffer;
    std::vector<ActiveCombo> activeCombos;

    // Input actions
    std::unique_ptr<UInputAction> lightAttackAction;
    std::unique_ptr<UInputAction> heavyAttackAction;
    std::unique_ptr<UInputAction> specialAttackAction;
};
```

### Dynamic Action Binding and Rebinding

```cpp
class ARebindableController : public APlayerController
{
public:
    virtual void BeginPlay() override
    {
        APlayerController::BeginPlay();

        // Create actions with initial bindings
        InitializeActions();

        // Load custom bindings
        LoadCustomBindings();

        // Set up rebinding UI
        CreateRebindingInterface();
    }

    void InitializeActions()
    {
        // Create actions with default bindings
        jumpAction = std::make_unique<UInputAction>("Jump",
            std::vector<EInput>{EInput::Keyboard_Space});
        attackAction = std::make_unique<UInputAction>("Attack",
            std::vector<EInput>{EInput::Mouse_Left});
        interactAction = std::make_unique<UInputAction>("Interact",
            std::vector<EInput>{EInput::Keyboard_E});

        // Connect events
        jumpAction->On<UInputAction::Events::Pressed>()
            .Connect(this, &ARebindableController::OnJump);
        attackAction->On<UInputAction::Events::Pressed>()
            .Connect(this, &ARebindableController::OnAttack);
        interactAction->On<UInputAction::Events::Pressed>()
            .Connect(this, &ARebindableController::OnInteract);
    }

    void LoadCustomBindings()
    {
        // Load custom keybindings from file
        FJsonObject bindings = LoadJsonFile("KeyBindings.json");

        if (bindings.IsValid()) {
            // Apply custom bindings
            if (bindings.HasField("jump")) {
                RebindAction(jumpAction.get(), bindings.GetArrayField("jump"));
            }
            if (bindings.HasField("attack")) {
                RebindAction(attackAction.get(), bindings.GetArrayField("attack"));
            }
            if (bindings.HasField("interact")) {
                RebindAction(interactAction.get(), bindings.GetArrayField("interact"));
            }
        }
    }

    void RebindAction(UInputAction* action, const FJsonArray& inputArray)
    {
        // Clear existing bindings
        const auto& currentInputs = action->GetInputs();
        for (EInput input : currentInputs) {
            action->Unbind(input);
        }

        // Add new bindings
        for (const auto& inputValue : inputArray) {
            FString inputStr = inputValue.GetStringField();
            EInput input = ParseInputFromString(inputStr);
            if (input != EInput::Unknown) {
                action->Bind(input);
            }
        }
    }

    void StartRebindingAction(const FString& actionName)
    {
        // Find the action
        UInputAction* action = nullptr;
        if (actionName == "Jump") action = jumpAction.get();
        else if (actionName == "Attack") action = attackAction.get();
        else if (actionName == "Interact") action = interactAction.get();

        if (!action) return;

        // Start rebinding process
        rebindingAction = action;
        rebindingInProgress = true;

        // Show rebinding prompt
        ShowRebindingPrompt(actionName);

        FLogger::Log(ELogLevel::Info,
            FString::Format("Started rebinding action: {}", actionName));
    }

    void UpdateRebinding()
    {
        if (!rebindingInProgress || !rebindingAction) return;

        // Check for any input press
        // (This would be called from input polling)
        for (int key = (int)EInput::Keyboard_A; key <= (int)EInput::Keyboard_Pause; ++key) {
            EInput input = (EInput)key;
            if (IsInputPressed(input)) {
                CompleteRebinding(input);
                return;
            }
        }

        for (int button = (int)EInput::Mouse_Left; button <= (int)EInput::Mouse_XButton2; ++button) {
            EInput input = (EInput)button;
            if (IsInputPressed(input)) {
                CompleteRebinding(input);
                return;
            }
        }
    }

    void CompleteRebinding(EInput newInput)
    {
        if (!rebindingAction) return;

        // Clear existing bindings
        const auto& currentInputs = rebindingAction->GetInputs();
        for (EInput input : currentInputs) {
            rebindingAction->Unbind(input);
        }

        // Add new binding
        rebindingAction->Bind(newInput);

        // Save bindings
        SaveCustomBindings();

        // End rebinding
        rebindingInProgress = false;
        rebindingAction = nullptr;

        HideRebindingPrompt();

        FLogger::Log(ELogLevel::Info,
            FString::Format("Rebound action to input: {}", (int)newInput));
    }

    void SaveCustomBindings()
    {
        FJsonObject bindings;

        // Serialize action bindings
        bindings.SetArrayField("jump", SerializeActionBindings(jumpAction.get()));
        bindings.SetArrayField("attack", SerializeActionBindings(attackAction.get()));
        bindings.SetArrayField("interact", SerializeActionBindings(interactAction.get()));

        SaveJsonFile("KeyBindings.json", bindings);
    }

    FJsonArray SerializeActionBindings(const UInputAction* action)
    {
        FJsonArray inputArray;
        const auto& inputs = action->GetInputs();

        for (EInput input : inputs) {
            inputArray.Add(InputToString(input));
        }

        return inputArray;
    }

    EInput ParseInputFromString(const FString& inputStr)
    {
        // Parse input string to EInput enum
        // (Implementation would map strings like "Keyboard_Space" to EInput::Keyboard_Space)
        return EInput::Unknown; // Placeholder
    }

    FString InputToString(EInput input)
    {
        // Convert EInput enum to string
        // (Implementation would map EInput::Keyboard_Space to "Keyboard_Space")
        return "Unknown"; // Placeholder
    }

    bool IsInputPressed(EInput input)
    {
        // Check if input is currently pressed
        // (This would interface with the input manager)
        return false; // Placeholder
    }

private:
    std::unique_ptr<UInputAction> jumpAction;
    std::unique_ptr<UInputAction> attackAction;
    std::unique_ptr<UInputAction> interactAction;

    UInputAction* rebindingAction;
    bool rebindingInProgress;
};
```

### Action State Monitoring and Debugging

```cpp
class ActionDebugger
{
public:
    void MonitorAction(UInputAction* action)
    {
        monitoredActions.push_back(action);

        // Connect to all events
        action->On<UInputAction::Events::Pressed>()
            .Connect(this, &ActionDebugger::OnActionPressed);
        action->On<UInputAction::Events::Held>()
            .Connect(this, &ActionDebugger::OnActionHeld);
        action->On<UInputAction::Events::Released>()
            .Connect(this, &ActionDebugger::OnActionReleased);
    }

    void LogActionStates()
    {
        FLogger::Log(ELogLevel::Debug, "=== Action States ===");

        for (UInputAction* action : monitoredActions) {
            FLogger::Log(ELogLevel::Debug,
                FString::Format("Action '{}': Pressed={}, Held={}, Released={}, CurrentInput={}",
                    action->GetName(),
                    action->IsPressed(),
                    action->IsHeld(),
                    action->IsReleased(),
                    (int)action->GetCurrentInput()));

            const auto& inputs = action->GetInputs();
            FLogger::Log(ELogLevel::Debug,
                FString::Format("  Bound inputs: {}", inputs.size()));

            for (size_t i = 0; i < inputs.size(); ++i) {
                FLogger::Log(ELogLevel::Debug,
                    FString::Format("    [{}]: {}", i, (int)inputs[i]));
            }
        }
    }

    void LogActionStatistics()
    {
        FLogger::Log(ELogLevel::Debug, "=== Action Statistics ===");

        for (const auto& [actionName, stats] : actionStats) {
            FLogger::Log(ELogLevel::Debug,
                FString::Format("Action '{}': Presses={}, Holds={}, Releases={}",
                    actionName, stats.pressCount, stats.holdCount, stats.releaseCount));
        }

        // Reset statistics
        actionStats.clear();
    }

private:
    void OnActionPressed(const UInputAction::Events::Pressed& event)
    {
        const FString& actionName = GetActionName(event.input);
        actionStats[actionName].pressCount++;

        FLogger::Log(ELogLevel::Debug,
            FString::Format("Action '{}' pressed with input {}",
                actionName, (int)event.input));
    }

    void OnActionHeld(const UInputAction::Events::Held& event)
    {
        const FString& actionName = GetActionName(event.input);
        actionStats[actionName].holdCount++;
    }

    void OnActionReleased(const UInputAction::Events::Released& event)
    {
        const FString& actionName = GetActionName(event.input);
        actionStats[actionName].releaseCount++;

        FLogger::Log(ELogLevel::Debug,
            FString::Format("Action '{}' released with input {}",
                actionName, (int)event.input));
    }

    const FString& GetActionName(EInput input) const
    {
        // Find action name by input (simplified)
        for (UInputAction* action : monitoredActions) {
            const auto& inputs = action->GetInputs();
            if (std::find(inputs.begin(), inputs.end(), input) != inputs.end()) {
                return action->GetName();
            }
        }
        static FString unknown = "Unknown";
        return unknown;
    }

private:
    std::vector<UInputAction*> monitoredActions;

    struct ActionStatistics {
        size_t pressCount = 0;
        size_t holdCount = 0;
        size_t releaseCount = 0;
    };

    std::unordered_map<FString, ActionStatistics> actionStats;
};
```

## State Management Architecture

### Action State Machine

```
Idle ──► Pressed ──► Held ──► Released ──► Idle
  │         │          │         │         │
  │         ▼          ▼         ▼         ▼
  └──── No Input  Continue  Continue  No Input
      Pressed     Pressed   Pressed   Released
```

### Event Emission Flow

```
Input Detected ──► State Change ──► Event Created ──► Listeners Notified
      │                 │                │              │
      ▼                 ▼                ▼              ▼
  Device Poll      State Update     Event Object    Callback Execution
  (Keyboard/       (Pressed=true)   (Pressed{input}) (OnPressed handlers)
   Mouse/Gamepad)  (Held=false)     (With metadata)  (Synchronous)
```

### Multi-Input Handling

```
Multiple Inputs ──► Any Active ──► State Update ──► Single Event
      │                │              │              │
      ▼                ▼              ▼              ▼
  Keyboard W      W pressed      Pressed=true    Pressed{W}
  GamePad Up      Up pressed     CurrentInput=W  Single event
  (Others idle)   (Others idle)  (No conflicts)  (No duplicates)
```

## Performance Considerations

### Memory Usage

- **Action Storage**: Each action stores name string and input vector
- **Event Callbacks**: Function pointers for event handlers
- **State Tracking**: Boolean flags for state management
- **Input Validation**: Vector searches for binding operations

### CPU Performance

- **State Updates**: Called once per frame per action
- **Event Emission**: Synchronous callback execution
- **Input Validation**: Vector searches for binding checks
- **Memory Access**: Frequent access to action state

### Optimization Strategies

1. **Event Batching**: Group similar events for batch processing
2. **Lazy Evaluation**: Only process active actions
3. **Callback Caching**: Cache frequently called event handlers
4. **State Caching**: Cache computed state values

## Troubleshooting

### Actions Not Triggering

**Symptoms:** Input actions not responding to input

**Possible Causes:**
- Actions not bound to correct inputs
- Input manager not updating actions
- Event handlers not connected
- Input device not working

**Solutions:**
```cpp
// Debug action binding
void DebugActionBinding(const UInputAction* action) {
    FLogger::Log(ELogLevel::Debug,
        FString::Format("Debugging action: {}", action->GetName()));

    const auto& inputs = action->GetInputs();
    FLogger::Log(ELogLevel::Debug,
        FString::Format("Bound inputs: {}", inputs.size()));

    for (size_t i = 0; i < inputs.size(); ++i) {
        FLogger::Log(ELogLevel::Debug,
            FString::Format("  Input {}: {}", i, (int)inputs[i]));
    }

    // Check state
    FLogger::Log(ELogLevel::Debug,
        FString::Format("State - Pressed: {}, Held: {}, Released: {}",
            action->IsPressed(), action->IsHeld(), action->IsReleased()));

    FLogger::Log(ELogLevel::Debug,
        FString::Format("Current input: {}", (int)action->GetCurrentInput()));
}

// Test action manually
void TestActionManually(UInputAction* action, EInput testInput) {
    FLogger::Log(ELogLevel::Debug,
        FString::Format("Testing action '{}' with input {}",
            action->GetName(), (int)testInput));

    // Simulate press
    action->Press(testInput);
    FLogger::Log(ELogLevel::Debug,
        FString::Format("After press - Pressed: {}", action->IsPressed()));

    // Simulate hold
    action->Hold(testInput);
    FLogger::Log(ELogLevel::Debug,
        FString::Format("After hold - Held: {}", action->IsHeld()));

    // Simulate release
    action->Release(testInput);
    FLogger::Log(ELogLevel::Debug,
        FString::Format("After release - Released: {}", action->IsReleased()));
}
```

### Event Handlers Not Called

**Symptoms:** Action events not triggering callbacks

**Possible Causes:**
- Event handlers not connected
- Wrong event type being listened to
- Handler function signature mismatch
- Object lifetime issues

**Solutions:**
```cpp
// Debug event connections
void DebugEventConnections(const UInputAction* action) {
    FLogger::Log(ELogLevel::Debug,
        FString::Format("Checking event connections for action: {}", action->GetName()));

    // Check if events are connected (this would require internal access)
    // For now, just log that we're checking
    FLogger::Log(ELogLevel::Debug, "Event connection debugging not fully implemented");
}

// Test event emission
void TestEventEmission(UInputAction* action) {
    FLogger::Log(ELogLevel::Debug,
        FString::Format("Testing event emission for action: {}", action->GetName()));

    // Create test event handler
    bool eventReceived = false;
    auto connection = action->On<UInputAction::Events::Pressed>()
        .Connect([&eventReceived](const UInputAction::Events::Pressed& event) {
            eventReceived = true;
            FLogger::Log(ELogLevel::Debug, "Test event received!");
        });

    // Trigger event
    action->Press(EInput::Keyboard_Space);

    // Check if event was received
    if (eventReceived) {
        FLogger::Log(ELogLevel::Debug, "Event emission test passed");
    } else {
        FLogger::Log(ELogLevel::Error, "Event emission test failed");
    }

    // Disconnect test handler
    connection.Disconnect();
}
```

### Multiple Inputs Conflict

**Symptoms:** Multiple bound inputs causing unexpected behavior

**Possible Causes:**
- Conflicting input mappings
- Input priority issues
- Simultaneous input handling problems

**Solutions:**
```cpp
// Debug input conflicts
void DebugInputConflicts(const UInputAction* action) {
    const auto& inputs = action->GetInputs();

    FLogger::Log(ELogLevel::Debug,
        FString::Format("Checking for input conflicts in action: {}", action->GetName()));

    // Check for duplicate inputs
    std::unordered_set<EInput> uniqueInputs;
    bool hasDuplicates = false;

    for (EInput input : inputs) {
        if (uniqueInputs.count(input) > 0) {
            FLogger::Log(ELogLevel::Warning,
                FString::Format("Duplicate input binding: {}", (int)input));
            hasDuplicates = true;
        }
        uniqueInputs.insert(input);
    }

    if (!hasDuplicates) {
        FLogger::Log(ELogLevel::Debug, "No duplicate input bindings found");
    }
}

// Resolve input conflicts
void ResolveInputConflicts(UInputAction* action) {
    const auto& inputs = action->GetInputs();
    std::unordered_set<EInput> seenInputs;

    // Remove duplicates, keeping first occurrence
    for (auto it = inputs.begin(); it != inputs.end(); ) {
        if (seenInputs.count(*it) > 0) {
            FLogger::Log(ELogLevel::Info,
                FString::Format("Removing duplicate input binding: {}", (int)*it));
            it = inputs.erase(it);
        } else {
            seenInputs.insert(*it);
            ++it;
        }
    }
}
```

## Future Enhancements

### Planned Features

1. **Advanced State Machines**: Complex action state transitions
2. **Input Modifiers**: Shift, Ctrl, Alt combinations
3. **Chord Support**: Multi-key simultaneous presses
4. **Gesture Recognition**: Pattern-based input recognition
5. **Haptic Feedback**: Action-based vibration patterns
6. **Input Prediction**: Predictive input handling
7. **Accessibility**: Alternative input methods
8. **Recording/Playback**: Input sequence recording

### Extension Points

The action system allows for easy extension:

```cpp
class AdvancedInputAction : public UInputAction {
public:
    // Add timing support
    void SetPressTimeWindow(float minTime, float maxTime) {
        minPressTime = minTime;
        maxPressTime = maxTime;
    }

    virtual void Press(EInput input) override {
        pressStartTime = GetCurrentTime();
        UInputAction::Press(input);
    }

    virtual void Release(EInput input) override {
        float pressDuration = GetCurrentTime() - pressStartTime;

        if (pressDuration >= minPressTime && pressDuration <= maxPressTime) {
            UInputAction::Release(input);
        } else {
            // Invalid press duration, ignore
            Idle();
        }
    }

private:
    float minPressTime = 0.0f;
    float maxPressTime = FLT_MAX;
    float pressStartTime = 0.0f;
};

// Contextual action system
class ContextualInputAction : public UInputAction {
public:
    void SetContextEnabled(const FString& context, bool enabled) {
        contextStates[context] = enabled;
    }

    bool IsContextActive(const FString& context) const {
        auto it = contextStates.find(context);
        return it != contextStates.end() ? it->second : false;
    }

    virtual void Press(EInput input) override {
        // Check if any active context allows this action
        bool contextAllows = false;
        for (const auto& [context, enabled] : contextStates) {
            if (enabled) {
                contextAllows = true;
                break;
            }
        }

        if (contextAllows) {
            UInputAction::Press(input);
        }
    }

private:
    std::unordered_map<FString, bool> contextStates;
};
```

---

*This documentation covers the complete UInputAction implementation as of TKD Engine v1.0.0. For the latest updates and additional examples, refer to the engine's source code and test suites.*
