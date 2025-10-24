# Input Manager Documentation

## Overview

The `FInputManager` class is the central hub for managing input devices and processing user interactions in the TKD Game Engine. It serves as a singleton manager that coordinates input actions, axes, and device states, providing a unified interface for handling keyboard, mouse, and gamepad inputs across different platforms.

### Key Features

- **Device Abstraction**: Unified interface for keyboard, mouse, and gamepad inputs
- **Action-Based Input**: Event-driven input actions with press/release/hold states
- **Analog Axis Support**: Continuous input values for joysticks and triggers
- **Multi-Device Support**: Simultaneous handling of multiple input devices
- **Configurable Mappings**: Runtime rebinding of inputs to actions and axes
- **Event System Integration**: Emits events for input state changes
- **Platform Independence**: Cross-platform input handling
- **Dead Zone Management**: Automatic dead zone handling for analog inputs
- **Input Inversion**: Configurable Y-axis inversion for different preferences

### Architecture

```cpp
class FInputManager
{
private:
    std::vector<UInputAction> m_actions;   // Action definitions
    std::vector<UInputAxis> m_axes;        // Axis definitions
    bool m_invertY;                        // Y-axis inversion flag
    bool m_enableGamepad;                  // Gamepad enable flag
    bool m_initialized;                    // Initialization state

public:
    // Initialization and configuration
    void Initialize(const FEngineSettings& settings);
    bool IsInitialized() const;

    // Device settings
    bool IsInvertY() const;
    bool IsGamepadEnabled() const;
    void SetInvertY(bool invert);
    void SetGamepadEnabled(bool enable);

    // Action management
    const std::vector<UInputAction>& GetActions() const;
    const UInputAction* GetAction(const FString& actionName) const;

    // Axis management
    const std::vector<UInputAxis>& GetAxes() const;
    const UInputAxis* GetAxis(const FString& axisName) const;
    Float32 GetAxisValue(const FString& axisName) const;

    // Update processing
    void Update(IWindow* window);
};
```

The `FInputManager` operates as the central coordinator, polling input devices each frame and updating action/axis states while emitting appropriate events.

## Core Components

### Input Device Abstraction

The manager abstracts different input devices into a unified `EInput` enumeration:

```cpp
enum class EInput {
    // Keyboard (0-95)
    Keyboard_A, Keyboard_B, ..., Keyboard_Pause,

    // Mouse (96-100)
    Mouse_Left, Mouse_Right, Mouse_Middle,
    Mouse_XButton1, Mouse_XButton2,

    // Gamepad Buttons (101-116)
    GamePad_A, GamePad_B, ..., GamePad_DPadRight,

    // Gamepad Axes (117-122)
    GamePadAxis_LeftX, GamePadAxis_LeftY,
    GamePadAxis_RightX, GamePadAxis_RightY,
    GamePadAxis_LeftTrigger, GamePadAxis_RightTrigger
};
```

### Action State Management

Actions track discrete input states with event emission:

```cpp
struct ActionState {
    bool pressed;      // Just pressed this frame
    bool held;         // Currently being held
    bool released;     // Just released this frame
    EInput input;      // Which input triggered the state
};
```

### Axis Value Processing

Axes combine multiple inputs into continuous values:

```cpp
struct AxisValue {
    float scale;       // Current axis value [-1.0, 1.0]
    float deadZone;    // Minimum value to register input
    bool inverted;     // Y-axis inversion flag
};
```

## API Reference

### Constructor & Initialization

#### `FInputManager(void)`

Creates an uninitialized input manager instance.

**Default State:**
- `m_actions`: Empty vector
- `m_axes`: Empty vector
- `m_invertY`: false
- `m_enableGamepad`: true
- `m_initialized`: false

#### `void Initialize(const FEngineSettings& settings)`

Initializes the input manager with engine configuration.

**Parameters:**
- `settings`: Engine settings containing input configuration

**Behavior:**
- Loads input action mappings from `settings.inputs.inputActions`
- Loads input axis mappings from `settings.inputs.inputAxes`
- Sets Y-axis inversion from `settings.inputs.invertYAxis`
- Sets gamepad enable flag from `settings.inputs.enableGamepad`
- Creates `UInputAction` and `UInputAxis` instances
- Sets initialization flag to true

**Example:**
```cpp
FEngineSettings settings;
// Configure jump action
settings.inputs.inputActions["Jump"] = {
    EInput::Keyboard_Space,
    EInput::GamePad_A
};

// Configure movement axis
settings.inputs.inputAxes["MoveForward"] = {
    {EInput::Keyboard_W, 1.0f},
    {EInput::Keyboard_S, -1.0f}
};

inputManager.Initialize(settings);
```

### State Queries

#### `bool IsInitialized(void) const`

Checks if the input manager has been initialized.

**Returns:** True if initialized, false otherwise

#### `bool IsInvertY(void) const`

Checks if Y-axis inversion is enabled.

**Returns:** True if Y-axis is inverted, false otherwise

#### `bool IsGamepadEnabled(void) const`

Checks if gamepad input is enabled.

**Returns:** True if gamepad input is enabled, false otherwise

### Configuration Methods

#### `void SetInvertY(bool invert)`

Sets Y-axis inversion for mouse and gamepad inputs.

**Parameters:**
- `invert`: True to invert Y-axis, false for normal

**Behavior:**
- Affects mouse Y-axis and gamepad left/right stick Y-axis
- Takes effect immediately on next update

#### `void SetGamepadEnabled(bool enable)`

Enables or disables gamepad input processing.

**Parameters:**
- `enable`: True to enable gamepad input, false to disable

**Behavior:**
- When disabled, gamepad inputs are ignored during update
- Existing gamepad actions/axes become inactive
- Can be toggled at runtime

### Action Management

#### `const std::vector<UInputAction>& GetActions(void) const`

Gets the list of all registered input actions.

**Returns:** Constant reference to vector of input actions

**Usage:**
```cpp
const auto& actions = inputManager.GetActions();
for (const auto& action : actions) {
    FLogger::Log(ELogLevel::Debug,
        FString::Format("Action: {}", action.GetName()));
}
```

#### `const UInputAction* GetAction(const FString& actionName) const`

Finds an input action by name.

**Parameters:**
- `actionName`: Name of the action to find

**Returns:** Pointer to the action if found, nullptr otherwise

**Usage:**
```cpp
const UInputAction* jumpAction = inputManager.GetAction("Jump");
if (jumpAction && jumpAction->IsPressed()) {
    player.Jump();
}
```

### Axis Management

#### `const std::vector<UInputAxis>& GetAxes(void) const`

Gets the list of all registered input axes.

**Returns:** Constant reference to vector of input axes

**Usage:**
```cpp
const auto& axes = inputManager.GetAxes();
for (const auto& axis : axes) {
    FLogger::Log(ELogLevel::Debug,
        FString::Format("Axis: {} = {:.2f}", axis.GetName(), axis.GetScale()));
}
```

#### `const UInputAxis* GetAxis(const FString& axisName) const`

Finds an input axis by name.

**Parameters:**
- `axisName`: Name of the axis to find

**Returns:** Pointer to the axis if found, nullptr otherwise

#### `Float32 GetAxisValue(const FString& axisName) const`

Gets the current value of an input axis.

**Parameters:**
- `axisName`: Name of the axis to query

**Returns:** Current axis value in range [-1.0, 1.0], or 0.0 if not found

**Usage:**
```cpp
float moveSpeed = inputManager.GetAxisValue("MoveForward") * maxSpeed;
player.Move(moveSpeed);
```

### Update Processing

#### `void Update(IWindow* window)`

Updates input states by polling all input devices.

**Parameters:**
- `window`: Pointer to window interface for device polling

**Behavior:**
- Polls keyboard state for all mapped keys
- Polls mouse state for buttons and position
- Polls gamepad state for buttons and axes (if enabled)
- Updates action states (pressed/held/released)
- Updates axis values with dead zone processing
- Emits events for state changes
- Applies Y-axis inversion if enabled

**Performance:** Called once per frame, processes all registered actions and axes

## Usage Examples

### Basic Input Manager Setup

```cpp
class AMyGame : public UGame
{
public:
    AMyGame(const FEngineSettings& settings)
        : UGame(settings)
    {
        // Custom input configuration
        ConfigureInputs();
    }

    virtual void BeginPlay() override
    {
        UGame::BeginPlay();

        // Initialize input manager
        inputManager = std::make_unique<FInputManager>();
        inputManager->Initialize(GetEngineSettings());

        // Set up input event handlers
        SetupInputHandlers();
    }

    virtual void Tick(Float32 deltaTime) override
    {
        UGame::Tick(deltaTime);

        // Update input manager
        inputManager->Update(GetWindow());

        // Process input
        ProcessInput(deltaTime);
    }

private:
    void ConfigureInputs()
    {
        auto& inputs = GetEngineSettings().inputs;

        // Configure movement actions
        inputs.inputActions["MoveUp"] = {EInput::Keyboard_W, EInput::GamePad_DPadUp};
        inputs.inputActions["MoveDown"] = {EInput::Keyboard_S, EInput::GamePad_DPadDown};
        inputs.inputActions["MoveLeft"] = {EInput::Keyboard_A, EInput::GamePad_DPadLeft};
        inputs.inputActions["MoveRight"] = {EInput::Keyboard_D, EInput::GamePad_DPadRight};

        // Configure game actions
        inputs.inputActions["Jump"] = {EInput::Keyboard_Space, EInput::GamePad_A};
        inputs.inputActions["Attack"] = {EInput::Mouse_Left, EInput::GamePad_X};
        inputs.inputActions["Pause"] = {EInput::Keyboard_Escape, EInput::GamePad_Start};

        // Configure movement axes
        inputs.inputAxes["MoveX"] = {
            {EInput::Keyboard_A, -1.0f},
            {EInput::Keyboard_D, 1.0f},
            {EInput::GamePadAxis_LeftX, 1.0f}
        };
        inputs.inputAxes["MoveY"] = {
            {EInput::Keyboard_S, -1.0f},
            {EInput::Keyboard_W, 1.0f},
            {EInput::GamePadAxis_LeftY, 1.0f}
        };

        // Configure camera/look axes
        inputs.inputAxes["LookX"] = {
            {EInput::Mouse_MoveX, 1.0f},
            {EInput::GamePadAxis_RightX, 1.0f}
        };
        inputs.inputAxes["LookY"] = {
            {EInput::Mouse_MoveY, 1.0f},
            {EInput::GamePadAxis_RightY, 1.0f}
        };
    }

    void SetupInputHandlers()
    {
        // Get action references
        const UInputAction* jumpAction = inputManager->GetAction("Jump");
        const UInputAction* attackAction = inputManager->GetAction("Attack");
        const UInputAction* pauseAction = inputManager->GetAction("Pause");

        // Bind event handlers
        if (jumpAction) {
            jumpAction->On<UInputAction::Events::Pressed>()
                .Connect(this, &AMyGame::OnJumpPressed);
            jumpAction->On<UInputAction::Events::Released>()
                .Connect(this, &AMyGame::OnJumpReleased);
        }

        if (attackAction) {
            attackAction->On<UInputAction::Events::Pressed>()
                .Connect(this, &AMyGame::OnAttackPressed);
        }

        if (pauseAction) {
            pauseAction->On<UInputAction::Events::Pressed>()
                .Connect(this, &AMyGame::OnPausePressed);
        }
    }

    void ProcessInput(Float32 deltaTime)
    {
        // Handle movement
        float moveX = inputManager->GetAxisValue("MoveX");
        float moveY = inputManager->GetAxisValue("MoveY");

        if (moveX != 0.0f || moveY != 0.0f) {
            player.Move(moveX, moveY, deltaTime);
        }

        // Handle camera look
        float lookX = inputManager->GetAxisValue("LookX");
        float lookY = inputManager->GetAxisValue("LookY");

        if (lookX != 0.0f || lookY != 0.0f) {
            camera.Rotate(lookX, lookY, deltaTime);
        }
    }

    void OnJumpPressed(const UInputAction::Events::Pressed& event)
    {
        player.Jump();
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Jump pressed with input: {}", (int)event.input));
    }

    void OnJumpReleased(const UInputAction::Events::Released& event)
    {
        // Handle variable jump height or other release logic
        player.StopJumping();
    }

    void OnAttackPressed(const UInputAction::Events::Pressed& event)
    {
        player.Attack();
    }

    void OnPausePressed(const UInputAction::Events::Pressed& event)
    {
        gamePaused = !gamePaused;
        if (gamePaused) {
            ShowPauseMenu();
        } else {
            HidePauseMenu();
        }
    }

private:
    std::unique_ptr<FInputManager> inputManager;
    APlayer player;
    ACamera camera;
    bool gamePaused;
};
```

### Advanced Input Processing with Combo System

```cpp
class AComboSystemGame : public UGame
{
public:
    virtual void Tick(Float32 deltaTime) override
    {
        UGame::Tick(deltaTime);

        inputManager->Update(GetWindow());

        // Update combo system
        UpdateCombos(deltaTime);

        // Process buffered inputs
        ProcessInputBuffer(deltaTime);
    }

private:
    void SetupComboSystem()
    {
        // Define combo sequences
        comboSequences["Hadouken"] = {
            {EInput::Keyboard_D, 0.2f},
            {EInput::Keyboard_S, 0.2f},
            {EInput::Keyboard_D, 0.2f},
            {EInput::Keyboard_Space, 0.1f}
        };

        comboSequences["Shoryuken"] = {
            {EInput::Keyboard_S, 0.3f},
            {EInput::Keyboard_D, 0.1f},
            {EInput::Keyboard_Space, 0.1f}
        };
    }

    void UpdateCombos(Float32 deltaTime)
    {
        // Update combo timers
        for (auto& combo : activeCombos) {
            combo.timer -= deltaTime;
            if (combo.timer <= 0.0f) {
                combo.currentStep = 0;
                combo.timer = 0.0f;
            }
        }

        // Remove expired combos
        activeCombos.erase(
            std::remove_if(activeCombos.begin(), activeCombos.end(),
                [](const ComboState& c) { return c.timer <= 0.0f; }),
            activeCombos.end());
    }

    void ProcessInputBuffer(Float32 deltaTime)
    {
        // Check for new inputs
        for (const auto& action : inputManager->GetActions()) {
            if (action.IsPressed()) {
                // Add to input buffer
                inputBuffer.push_back({
                    action.GetCurrentInput(),
                    currentTime
                });

                // Limit buffer size
                if (inputBuffer.size() > 10) {
                    inputBuffer.erase(inputBuffer.begin());
                }

                // Check for combo matches
                CheckComboMatches(action.GetCurrentInput());
            }
        }
    }

    void CheckComboMatches(EInput input)
    {
        for (const auto& [comboName, sequence] : comboSequences) {
            // Find active combo or create new one
            auto it = std::find_if(activeCombos.begin(), activeCombos.end(),
                [comboName](const ComboState& c) { return c.name == comboName; });

            if (it == activeCombos.end()) {
                // Start new combo
                activeCombos.push_back({comboName, 0, sequence[0].timeWindow});
                it = activeCombos.end() - 1;
            }

            // Check if input matches current step
            if (sequence[it->currentStep].input == input) {
                it->currentStep++;

                // Check if combo completed
                if (it->currentStep >= sequence.size()) {
                    ExecuteCombo(comboName);
                    activeCombos.erase(it);
                } else {
                    // Reset timer for next step
                    it->timer = sequence[it->currentStep].timeWindow;
                }
            }
        }
    }

    void ExecuteCombo(const FString& comboName)
    {
        if (comboName == "Hadouken") {
            player.CastHadouken();
        } else if (comboName == "Shoryuken") {
            player.CastShoryuken();
        }

        FLogger::Log(ELogLevel::Info,
            FString::Format("Combo executed: {}", comboName));
    }

private:
    struct ComboStep {
        EInput input;
        float timeWindow;
    };

    struct ComboState {
        FString name;
        size_t currentStep;
        float timer;
    };

    std::unordered_map<FString, std::vector<ComboStep>> comboSequences;
    std::vector<ComboState> activeCombos;

    struct BufferedInput {
        EInput input;
        float timestamp;
    };
    std::vector<BufferedInput> inputBuffer;

    float currentTime;
};
```

### Input Remapping System

```cpp
class ARemappableInputGame : public UGame
{
public:
    virtual void BeginPlay() override
    {
        UGame::BeginPlay();

        inputManager = std::make_unique<FInputManager>();
        inputManager->Initialize(GetEngineSettings());

        LoadInputMappings();
        CreateRemappingUI();
    }

    void LoadInputMappings()
    {
        // Load custom mappings from file
        FJsonObject mappings = LoadJsonFile("InputMappings.json");

        if (mappings.IsValid()) {
            // Apply custom mappings
            auto& inputs = GetEngineSettings().inputs;

            // Load action mappings
            if (mappings.HasField("actions")) {
                auto actionsObj = mappings.GetObjectField("actions");
                for (const auto& [actionName, inputsArray] : actionsObj) {
                    std::vector<EInput> actionInputs;
                    for (const auto& inputStr : inputsArray.GetArrayField()) {
                        EInput input = ParseInputString(inputStr.GetStringField());
                        if (input != EInput::Unknown) {
                            actionInputs.push_back(input);
                        }
                    }
                    inputs.inputActions[actionName] = actionInputs;
                }
            }

            // Load axis mappings
            if (mappings.HasField("axes")) {
                auto axesObj = mappings.GetObjectField("axes");
                for (const auto& [axisName, inputsArray] : axesObj) {
                    std::vector<std::pair<EInput, float>> axisInputs;
                    for (const auto& inputObj : inputsArray.GetArrayField()) {
                        auto inputPair = inputObj.GetObjectField();
                        EInput input = ParseInputString(inputPair["input"].GetStringField());
                        float scale = inputPair["scale"].GetNumberField();

                        if (input != EInput::Unknown) {
                            axisInputs.emplace_back(input, scale);
                        }
                    }
                    inputs.inputAxes[axisName] = axisInputs;
                }
            }

            // Reinitialize input manager with new mappings
            inputManager->Initialize(GetEngineSettings());
        }
    }

    void SaveInputMappings()
    {
        FJsonObject mappings;

        // Save action mappings
        FJsonObject actionsObj;
        for (const auto& action : inputManager->GetActions()) {
            FJsonArray inputsArray;
            for (EInput input : action.GetInputs()) {
                inputsArray.Add(InputToString(input));
            }
            actionsObj.SetArrayField(action.GetName(), inputsArray);
        }
        mappings.SetObjectField("actions", actionsObj);

        // Save axis mappings
        FJsonObject axesObj;
        for (const auto& axis : inputManager->GetAxes()) {
            FJsonArray inputsArray;
            for (const auto& [input, scale] : axis.GetInputs()) {
                FJsonObject inputObj;
                inputObj.SetStringField("input", InputToString(input));
                inputObj.SetNumberField("scale", scale);
                inputsArray.Add(inputObj);
            }
            axesObj.SetArrayField(axis.GetName(), inputsArray);
        }
        mappings.SetObjectField("axes", axesObj);

        SaveJsonFile("InputMappings.json", mappings);
    }

    void StartRemapping(const FString& actionName)
    {
        remappingAction = actionName;
        remappingInProgress = true;
        ShowRemappingPrompt(actionName);
    }

    void UpdateRemapping()
    {
        if (!remappingInProgress) return;

        // Check for any input
        for (const auto& action : inputManager->GetActions()) {
            if (action.IsPressed()) {
                // Found new input for action
                RemapAction(remappingAction, action.GetCurrentInput());
                remappingInProgress = false;
                HideRemappingPrompt();
                SaveInputMappings();
                break;
            }
        }

        for (const auto& axis : inputManager->GetAxes()) {
            // Check axis inputs (simplified)
            for (const auto& [input, scale] : axis.GetInputs()) {
                // If axis moved significantly, use that input
                if (std::abs(axis.GetScale()) > 0.5f) {
                    RemapAxis(remappingAction, input, axis.GetScale());
                    remappingInProgress = false;
                    HideRemappingPrompt();
                    SaveInputMappings();
                    break;
                }
            }
        }
    }

    void RemapAction(const FString& actionName, EInput newInput)
    {
        // Find the action and replace its inputs
        for (auto& action : const_cast<std::vector<UInputAction>&>(inputManager->GetActions())) {
            if (action.GetName() == actionName) {
                action.UnbindAll(); // Clear existing bindings
                action.Bind(newInput);
                break;
            }
        }
    }

    void RemapAxis(const FString& axisName, EInput newInput, float scale)
    {
        // Find the axis and replace its inputs
        for (auto& axis : const_cast<std::vector<UInputAxis>&>(inputManager->GetAxes())) {
            if (axis.GetName() == axisName) {
                axis.ClearBindings();
                axis.Bind(newInput, scale);
                break;
            }
        }
    }

private:
    std::unique_ptr<FInputManager> inputManager;
    FString remappingAction;
    bool remappingInProgress;
};
```

## Input Processing Architecture

### Update Flow

```
Frame Start ──► Poll Input Devices ──► Process Actions ──► Process Axes ──► Emit Events
      │                │                      │                │              │
      ▼                ▼                      ▼                ▼              ▼
Keyboard State   Check Key States       Update Press/Hold  Combine Inputs  Notify Listeners
Mouse State      Check Button States    Update Release     Apply Dead Zone Action Callbacks
Gamepad State    Check Axis Values      Emit Events        Apply Inversion  Axis Callbacks
      │                │                      │                │              │
      └────────────────┼──────────────────────┼────────────────┼──────────────┘
                       ▼                      ▼                ▼              ▼
                 Device Filtering ◄───── State Transitions ◄─── Value Clamping ◄── Event Queue
                 (Gamepad Enabled)       (Pressed→Held)        ([-1,1] Range)  (Async Dispatch)
```

### Action State Machine

```
Idle ──► Pressed ──► Held ──► Released ──► Idle
  │         │          │         │         │
  │         ▼          ▼         ▼         ▼
  └──── No Input  Continue  Continue  No Input
      Pressed     Pressed   Pressed   Released
```

### Axis Processing Pipeline

```
Raw Input ──► Device Polling ──► Input Combination ──► Dead Zone ──► Inversion ──► Clamping ──► Output
     │              │                   │                │             │             │         │
     ▼              ▼                   ▼                ▼             ▼             ▼         ▼
Key/Button     Keyboard/Mouse       Sum All Inputs   < 0.05 → 0   Y-Axis * -1   [-1,1] Range  Final Value
Axis Value     Gamepad Axes         Weighted Sum     Threshold     If Enabled   Saturation   Event Emit
```

## Performance Considerations

### Update Frequency

- **Frame-Based**: Input manager updates once per frame
- **Device Polling**: Polls all input devices each update
- **Event Emission**: Emits events for all state changes
- **Memory Access**: Iterates through all actions and axes

### Optimization Strategies

1. **Lazy Evaluation**: Only process active input devices
2. **Event Batching**: Group similar events for batch processing
3. **Input Filtering**: Ignore inputs below dead zone thresholds
4. **State Caching**: Cache input states to avoid redundant polling

### Memory Usage

- **Action Storage**: Vector of UInputAction instances
- **Axis Storage**: Vector of UInputAxis instances
- **Input Bindings**: Vectors of EInput enumerations
- **Event Callbacks**: Stored function pointers for event handling

### Best Practices

- **Minimal Bindings**: Only bind necessary inputs to reduce processing
- **Dead Zone Tuning**: Adjust dead zones for different input devices
- **Event Cleanup**: Properly disconnect event handlers to prevent leaks
- **Thread Safety**: Input manager is not thread-safe, update from main thread only

## Troubleshooting

### Input Not Responding

**Symptoms:** Input actions or axes not triggering

**Possible Causes:**
- Input manager not initialized
- Incorrect input mappings
- Device not connected/enabled
- Event handlers not connected

**Solutions:**
```cpp
// Debug input initialization
void DebugInputInitialization(FInputManager* inputManager) {
    FLogger::Log(ELogLevel::Debug,
        FString::Format("Input manager initialized: {}", inputManager->IsInitialized()));

    if (!inputManager->IsInitialized()) {
        FLogger::Log(ELogLevel::Error, "Input manager not initialized!");
        return;
    }

    FLogger::Log(ELogLevel::Debug,
        FString::Format("Gamepad enabled: {}", inputManager->IsGamepadEnabled()));
    FLogger::Log(ELogLevel::Debug,
        FString::Format("Y-axis inverted: {}", inputManager->IsInvertY()));

    // Check action registrations
    const auto& actions = inputManager->GetActions();
    FLogger::Log(ELogLevel::Debug,
        FString::Format("Registered actions: {}", actions.size()));

    for (const auto& action : actions) {
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Action '{}' has {} inputs",
                action.GetName(), action.GetInputs().size()));
    }

    // Check axis registrations
    const auto& axes = inputManager->GetAxes();
    FLogger::Log(ELogLevel::Debug,
        FString::Format("Registered axes: {}", axes.size()));

    for (const auto& axis : axes) {
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Axis '{}' has {} inputs",
                axis.GetName(), axis.GetInputs().size()));
    }
}

// Test input polling
void TestInputPolling(FInputManager* inputManager, IWindow* window) {
    FLogger::Log(ELogLevel::Debug, "Testing input device polling...");

    // Test keyboard
    if (window->IsKeyPressed(EKeyboardKeys::W)) {
        FLogger::Log(ELogLevel::Debug, "Keyboard W key pressed");
    }

    // Test mouse
    if (window->IsMouseButtonPressed(EMouseButtons::Left)) {
        FLogger::Log(ELogLevel::Debug, "Mouse left button pressed");
    }

    // Test gamepad (if enabled)
    if (inputManager->IsGamepadEnabled()) {
        if (window->IsGamepadButtonPressed(EGamepadButtons::A)) {
            FLogger::Log(ELogLevel::Debug, "Gamepad A button pressed");
        }

        float leftX = window->GetGamepadAxis(EGamepadAxes::LeftX);
        if (std::abs(leftX) > 0.1f) {
            FLogger::Log(ELogLevel::Debug,
                FString::Format("Gamepad left X axis: {:.2f}", leftX));
        }
    }
}
```

### Axis Values Incorrect

**Symptoms:** Axis values are wrong, inverted, or not responding

**Possible Causes:**
- Incorrect axis mappings
- Y-axis inversion not applied correctly
- Dead zone too large/small
- Multiple inputs conflicting

**Solutions:**
```cpp
// Debug axis values
void DebugAxisValues(FInputManager* inputManager) {
    FLogger::Log(ELogLevel::Debug, "Debugging axis values...");

    const auto& axes = inputManager->GetAxes();
    for (const auto& axis : axes) {
        float value = inputManager->GetAxisValue(axis.GetName());
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Axis '{}' = {:.3f}", axis.GetName(), value));

        // Show input contributions
        for (const auto& [input, scale] : axis.GetInputs()) {
            FLogger::Log(ELogLevel::Debug,
                FString::Format("  Input {} with scale {:.2f}",
                    (int)input, scale));
        }
    }
}

// Test axis calibration
void CalibrateAxis(FInputManager* inputManager, const FString& axisName) {
    FLogger::Log(ELogLevel::Debug,
        FString::Format("Calibrating axis: {}", axisName));

    const UInputAxis* axis = inputManager->GetAxis(axisName);
    if (!axis) {
        FLogger::Log(ELogLevel::Error,
            FString::Format("Axis '{}' not found", axisName));
        return;
    }

    // Test different input scenarios
    FLogger::Log(ELogLevel::Debug, "Testing positive input...");
    // (Would need to simulate input or wait for user input)

    FLogger::Log(ELogLevel::Debug, "Testing negative input...");
    // (Would need to simulate input or wait for user input)

    FLogger::Log(ELogLevel::Debug, "Testing dead zone...");
    // (Would need to simulate input or wait for user input)
}

// Fix axis inversion
void FixAxisInversion(FInputManager* inputManager, const FString& axisName, bool shouldInvert) {
    // Find axis and fix inversion by negating scales
    for (auto& axis : const_cast<std::vector<UInputAxis>&>(inputManager->GetAxes())) {
        if (axis.GetName() == axisName) {
            auto inputs = axis.GetInputs();
            axis.ClearBindings();

            for (const auto& [input, scale] : inputs) {
                float newScale = shouldInvert ? -scale : scale;
                axis.Bind(input, newScale);
            }

            FLogger::Log(ELogLevel::Info,
                FString::Format("Fixed inversion for axis '{}'", axisName));
            break;
        }
    }
}
```

### Gamepad Not Working

**Symptoms:** Gamepad inputs ignored or not responding

**Possible Causes:**
- Gamepad not enabled in settings
- Gamepad not connected
- Incorrect gamepad mappings
- Driver issues

**Solutions:**
```cpp
// Debug gamepad support
void DebugGamepadSupport(FInputManager* inputManager, IWindow* window) {
    FLogger::Log(ELogLevel::Debug, "Debugging gamepad support...");

    bool gamepadEnabled = inputManager->IsGamepadEnabled();
    FLogger::Log(ELogLevel::Debug,
        FString::Format("Gamepad enabled: {}", gamepadEnabled));

    if (!gamepadEnabled) {
        FLogger::Log(ELogLevel::Warning, "Gamepad support is disabled");
        inputManager->SetGamepadEnabled(true);
        FLogger::Log(ELogLevel::Info, "Gamepad support enabled");
    }

    // Test gamepad connection
    bool gamepadConnected = window->IsGamepadConnected();
    FLogger::Log(ELogLevel::Debug,
        FString::Format("Gamepad connected: {}", gamepadConnected));

    if (!gamepadConnected) {
        FLogger::Log(ELogLevel::Warning, "No gamepad detected");
        return;
    }

    // Test button polling
    for (int button = (int)EGamepadButtons::A;
         button <= (int)EGamepadButtons::DPadRight; ++button) {
        if (window->IsGamepadButtonPressed((EGamepadButtons)button)) {
            FLogger::Log(ELogLevel::Debug,
                FString::Format("Gamepad button {} pressed", button));
        }
    }

    // Test axis polling
    for (int axis = (int)EGamepadAxes::LeftX;
         axis <= (int)EGamepadAxes::RightTrigger; ++axis) {
        float value = window->GetGamepadAxis((EGamepadAxes)axis);
        if (std::abs(value) > 0.1f) {
            FLogger::Log(ELogLevel::Debug,
                FString::Format("Gamepad axis {} = {:.3f}", axis, value));
        }
    }
}

// Test gamepad mappings
void TestGamepadMappings(FInputManager* inputManager) {
    FLogger::Log(ELogLevel::Debug, "Testing gamepad mappings...");

    // Check if any actions use gamepad inputs
    const auto& actions = inputManager->GetActions();
    for (const auto& action : actions) {
        bool hasGamepadInput = false;
        for (EInput input : action.GetInputs()) {
            if (input >= EInput::GamePad_A && input <= EInput::GamePadAxis_RightTrigger) {
                hasGamepadInput = true;
                break;
            }
        }

        if (hasGamepadInput) {
            FLogger::Log(ELogLevel::Debug,
                FString::Format("Action '{}' has gamepad inputs", action.GetName()));
        }
    }

    // Check if any axes use gamepad inputs
    const auto& axes = inputManager->GetAxes();
    for (const auto& axis : axes) {
        bool hasGamepadInput = false;
        for (const auto& [input, scale] : axis.GetInputs()) {
            if (input >= EInput::GamePad_A && input <= EInput::GamePadAxis_RightTrigger) {
                hasGamepadInput = true;
                break;
            }
        }

        if (hasGamepadInput) {
            FLogger::Log(ELogLevel::Debug,
                FString::Format("Axis '{}' has gamepad inputs", axis.GetName()));
        }
    }
}
```

### Performance Issues

**Symptoms:** Input processing causing frame rate drops

**Possible Causes:**
- Too many input actions/axes
- Excessive event emission
- Inefficient input polling
- Memory leaks in event handlers

**Solutions:**
```cpp
// Profile input performance
class InputProfiler {
public:
    void ProfileInputUpdate(FInputManager* inputManager, IWindow* window) {
        auto start = std::chrono::high_resolution_clock::now();

        inputManager->Update(window);

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        updateTimes.push_back(duration.count());
        if (updateTimes.size() > 60) {
            updateTimes.erase(updateTimes.begin());
        }

        // Calculate average
        double avgTime = 0.0;
        for (double time : updateTimes) {
            avgTime += time;
        }
        avgTime /= updateTimes.size();

        FLogger::Log(ELogLevel::Debug,
            FString::Format("Input update: {:.1f}μs avg", avgTime));

        if (avgTime > 2000.0) { // > 2ms at 60fps
            FLogger::Log(ELogLevel::Warning, "Input update performance is poor");
            OptimizeInputPerformance(inputManager);
        }
    }

private:
    void OptimizeInputPerformance(FInputManager* inputManager) {
        // Count registered inputs
        size_t actionCount = inputManager->GetActions().size();
        size_t axisCount = inputManager->GetAxes().size();

        FLogger::Log(ELogLevel::Debug,
            FString::Format("Actions: {}, Axes: {}", actionCount, axisCount));

        if (actionCount > 50) {
            FLogger::Log(ELogLevel::Warning, "Large number of actions registered");
        }

        if (axisCount > 20) {
            FLogger::Log(ELogLevel::Warning, "Large number of axes registered");
        }

        // Suggest optimizations:
        // 1. Reduce number of registered actions/axes
        // 2. Use axis polling instead of multiple actions
        // 3. Implement input buffering for combo systems
        // 4. Cache input states when possible
    }

private:
    std::vector<double> updateTimes;
};

// Monitor input event frequency
class InputEventMonitor {
public:
    void MonitorEvents(FInputManager* inputManager) {
        // Connect to all action events
        for (const auto& action : inputManager->GetActions()) {
            action.On<UInputAction::Events::Pressed>()
                .Connect(this, &InputEventMonitor::OnActionPressed);
            action.On<UInputAction::Events::Released>()
                .Connect(this, &InputEventMonitor::OnActionReleased);
        }

        // Connect to all axis events
        for (const auto& axis : inputManager->GetAxes()) {
            axis.On<UInputAxis::Events::Changed>()
                .Connect(this, &InputEventMonitor::OnAxisChanged);
        }
    }

    void OnActionPressed(const UInputAction::Events::Pressed& event) {
        actionPressCount++;
    }

    void OnActionReleased(const UInputAction::Events::Released& event) {
        actionReleaseCount++;
    }

    void OnAxisChanged(const UInputAxis::Events::Changed& event) {
        axisChangeCount++;
    }

    void LogEventStats() {
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Input Events - Presses: {}, Releases: {}, Axis Changes: {}",
                actionPressCount, actionReleaseCount, axisChangeCount));

        // Reset counters
        actionPressCount = 0;
        actionReleaseCount = 0;
        axisChangeCount = 0;
    }

private:
    size_t actionPressCount = 0;
    size_t actionReleaseCount = 0;
    size_t axisChangeCount = 0;
};
```

## Future Enhancements

### Planned Features

1. **Advanced Input Processing**: Gesture recognition and multi-touch support
2. **Input Recording/Playback**: Record and replay input sequences for testing
3. **Dynamic Rebinding**: Runtime input remapping with conflict resolution
4. **Haptic Feedback**: Force feedback and vibration support
5. **Accessibility Features**: Alternative input methods and customization
6. **Network Input**: Synchronized input for multiplayer games
7. **Input Analytics**: Usage statistics and player behavior analysis
8. **Mobile Input**: Touch controls and mobile-specific optimizations

### Extension Points

The input manager allows for easy extension:

```cpp
class AdvancedInputManager : public FInputManager {
public:
    // Add gesture recognition
    virtual void Update(IWindow* window) override {
        FInputManager::Update(window);

        // Process gestures
        ProcessGestures();
    }

    // Add input recording
    void StartRecording() {
        recording = true;
        recordedInputs.clear();
    }

    void StopRecording() {
        recording = false;
    }

    void PlaybackRecording() {
        playback = true;
        playbackIndex = 0;
    }

private:
    void ProcessGestures() {
        // Detect swipe gestures, pinches, etc.
        // Update gesture state and emit events
    }

    struct RecordedInput {
        float timestamp;
        EInput input;
        float value; // For axes
    };

    std::vector<RecordedInput> recordedInputs;
    bool recording = false;
    bool playback = false;
    size_t playbackIndex = 0;
};

// Custom input device support
class CustomInputManager : public FInputManager {
public:
    void RegisterCustomDevice(ICustomInputDevice* device) {
        customDevices.push_back(device);
    }

    virtual void Update(IWindow* window) override {
        FInputManager::Update(window);

        // Poll custom devices
        for (ICustomInputDevice* device : customDevices) {
            device->Update();

            // Process custom inputs
            ProcessCustomInputs(device);
        }
    }

private:
    void ProcessCustomInputs(ICustomInputDevice* device) {
        // Map custom device inputs to engine inputs
        // Update actions and axes accordingly
    }

private:
    std::vector<ICustomInputDevice*> customDevices;
};
```

---

*This documentation covers the complete FInputManager implementation as of TKD Engine v1.0.0. For the latest updates and additional examples, refer to the engine's source code and test suites.*
