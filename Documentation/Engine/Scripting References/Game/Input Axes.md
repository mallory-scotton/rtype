# Input Axes Documentation

## Overview

The `UInputAxis` class represents continuous input axes in the TKD Game Engine. Unlike discrete actions, axes provide floating-point values representing input intensity, making them ideal for analog controls like joysticks, mouse movement, and pressure-sensitive inputs. Axes support dead zones, sensitivity scaling, inversion, and automatic value smoothing.

### Key Features

- **Continuous Values**: Floating-point input values (-1.0 to 1.0 range)
- **Dead Zone Support**: Configurable dead zones to prevent drift
- **Sensitivity Scaling**: Adjustable input sensitivity
- **Axis Inversion**: Ability to invert axis direction
- **Value Smoothing**: Optional smoothing for stable values
- **Multi-Input Aggregation**: Combine multiple inputs into single axis
- **Event System**: Emits events for value changes
- **Automatic Updates**: Real-time value computation
- **Thread-Safe Events**: Event emission is thread-safe
- **Dynamic Configuration**: Runtime parameter adjustment

### Architecture

```cpp
class UInputAxis : public TEventEmitter<TEvents<UInputAxis>::All>
{
private:
    FString m_name;                    // Axis identifier
    std::vector<EInput> m_inputs;      // Bound inputs
    float m_value;                     // Current axis value
    float m_rawValue;                  // Raw computed value
    float m_deadZone;                  // Dead zone threshold
    float m_sensitivity;               // Sensitivity multiplier
    bool m_inverted;                   // Axis inversion flag
    bool m_smooth;                     // Smoothing enabled
    float m_smoothFactor;              // Smoothing strength
    float m_previousValue;             // Previous frame value

public:
    // Event types
    struct ValueChanged { float oldValue; float newValue; };
    struct DeadZoneEntered { };
    struct DeadZoneExited { };

    // Value computation
    void UpdateValue();
    void SetRawValue(float value);
    void ApplyDeadZone();
    void ApplySensitivity();
    void ApplySmoothing();

    // Configuration
    bool Bind(EInput input);
    bool Unbind(EInput input);
    void SetDeadZone(float deadZone);
    void SetSensitivity(float sensitivity);
    void SetInverted(bool inverted);
    void SetSmoothing(bool enabled, float factor);

    // Queries
    const FString& GetName() const;
    const std::vector<EInput>& GetInputs() const;
    float GetValue() const;
    float GetRawValue() const;
    float GetDeadZone() const;
    float GetSensitivity() const;
    bool IsInverted() const;
    bool IsInDeadZone() const;
};
```

The `UInputAxis` operates as a real-time value processor, continuously computing axis values from bound inputs and applying configurable transformations.

## Core Components

### Value Processing Pipeline

```
Raw Input ──► Aggregation ──► Dead Zone ──► Sensitivity ──► Inversion ──► Smoothing ──► Final Value
     │            │             │             │             │             │             │
     ▼            ▼             ▼             ▼             ▼             ▼             ▼
Device Poll   Sum/Average   Threshold     Multiply     Negate      Interpolate   Output Value
(Keyboard/    (Multiple     (0.0-1.0)    (0.5-2.0)    (true/false) (0.0-1.0)     (-1.0 to 1.0)
 Mouse/       inputs)       range)        range)       toggle)      factor)       range)
 Gamepad)
```

### Dead Zone Implementation

Dead zones prevent small input values from registering, eliminating controller drift:

```cpp
float ApplyDeadZone(float input, float deadZone) {
    if (std::abs(input) < deadZone) {
        return 0.0f;  // Within dead zone, return zero
    } else {
        // Outside dead zone, scale to full range
        float sign = (input > 0.0f) ? 1.0f : -1.0f;
        float magnitude = std::abs(input);
        return sign * ((magnitude - deadZone) / (1.0f - deadZone));
    }
}
```

### Smoothing Algorithm

Exponential smoothing provides stable axis values:

```cpp
float ApplySmoothing(float current, float previous, float factor) {
    return previous + (current - previous) * factor;
    // factor = 1.0: No smoothing (immediate response)
    // factor = 0.1: Heavy smoothing (slow response)
}
```

### Multi-Input Aggregation

Multiple inputs are combined using configurable strategies:

```cpp
enum class AggregationMode {
    Sum,        // Add all input values
    Average,    // Average all input values
    Maximum,    // Use largest absolute value
    Minimum     // Use smallest absolute value
};
```

## API Reference

### Constructor & Initialization

#### `UInputAxis(const FString& name, const std::vector<EInput>& inputs, float deadZone, float sensitivity, bool inverted, bool smooth, float smoothFactor)`

Creates an input axis with specified configuration.

**Parameters:**
- `name`: Unique identifier for the axis (e.g., "MoveX", "LookY")
- `inputs`: Vector of input enumerations that contribute to this axis
- `deadZone`: Dead zone threshold (0.0 to 1.0, default 0.1)
- `sensitivity`: Sensitivity multiplier (default 1.0)
- `inverted`: Whether to invert axis direction (default false)
- `smooth`: Whether to enable value smoothing (default false)
- `smoothFactor`: Smoothing factor (0.0 to 1.0, default 0.5)

**Behavior:**
- Initializes axis name and input bindings
- Sets configuration parameters
- Initializes value to 0.0
- No events emitted during construction

**Example:**
```cpp
// Create movement axes with different configurations
UInputAxis moveXAxis("MoveX", {
    EInput::Keyboard_A, EInput::Keyboard_D,
    EInput::GamePad_LeftStickX
}, 0.1f, 1.0f, false, true, 0.3f);

UInputAxis lookYAxis("LookY", {
    EInput::Mouse_Y
}, 0.05f, 0.8f, true, false, 0.0f); // Inverted mouse Y
```

### Value Update Methods

#### `void UpdateValue(void)`

Updates the axis value based on current input states.

**Behavior:**
- Polls all bound inputs from input manager
- Aggregates input values (sums by default)
- Applies dead zone filtering
- Applies sensitivity scaling
- Applies inversion if enabled
- Applies smoothing if enabled
- Updates final value
- Emits `ValueChanged` event if value changed
- Emits `DeadZoneEntered`/`DeadZoneExited` events as appropriate

**Usage:**
```cpp
// Called every frame by input manager
moveXAxis.UpdateValue();
lookYAxis.UpdateValue();
```

#### `void SetRawValue(float value)`

Directly sets the axis value, bypassing input polling.

**Parameters:**
- `value`: Raw value to set (-1.0 to 1.0)

**Behavior:**
- Sets raw value directly
- Applies dead zone, sensitivity, inversion, and smoothing
- Updates final value
- Emits events if value changed

**Usage:**
```cpp
// For programmatic control or testing
axis.SetRawValue(0.5f); // Set to half intensity
```

### Configuration Methods

#### `bool Bind(EInput input)`

Adds an input binding to this axis.

**Parameters:**
- `input`: The input to bind

**Returns:** True if binding was added, false if input was already bound

**Behavior:**
- Checks if input is already bound
- Adds input to bindings vector if not present
- Returns success status

#### `bool Unbind(EInput input)`

Removes an input binding from this axis.

**Parameters:**
- `input`: The input to remove

**Returns:** True if binding was removed, false if input was not found

#### `void SetDeadZone(float deadZone)`

Sets the dead zone threshold.

**Parameters:**
- `deadZone`: Dead zone value (0.0 to 1.0)

**Behavior:**
- Updates dead zone parameter
- Dead zone is applied on next value update

**Example:**
```cpp
// Adjust dead zone for different controller sensitivities
axis.SetDeadZone(0.15f); // Wider dead zone for noisy controller
```

#### `void SetSensitivity(float sensitivity)`

Sets the sensitivity multiplier.

**Parameters:**
- `sensitivity`: Sensitivity value (typically 0.1 to 5.0)

**Behavior:**
- Updates sensitivity parameter
- Sensitivity is applied on next value update

**Example:**
```cpp
// Make mouse more sensitive for precise aiming
lookAxis.SetSensitivity(1.5f);
```

#### `void SetInverted(bool inverted)`

Sets whether the axis should be inverted.

**Parameters:**
- `inverted`: True to invert axis direction, false for normal

**Behavior:**
- Updates inversion flag
- Inversion is applied on next value update

#### `void SetSmoothing(bool enabled, float factor)`

Configures value smoothing.

**Parameters:**
- `enabled`: Whether to enable smoothing
- `factor`: Smoothing factor (0.0 = no smoothing, 1.0 = immediate response)

**Behavior:**
- Updates smoothing parameters
- Smoothing is applied on next value update

**Example:**
```cpp
// Enable smoothing for camera movement
cameraAxis.SetSmoothing(true, 0.2f); // Smooth but responsive
```

### Query Methods

#### `const FString& GetName(void) const`

Gets the axis name identifier.

**Returns:** Constant reference to the axis name string

#### `const std::vector<EInput>& GetInputs(void) const`

Gets all input bindings for this axis.

**Returns:** Constant reference to vector of bound inputs

#### `float GetValue(void) const`

Gets the current processed axis value.

**Returns:** Current axis value (-1.0 to 1.0)

#### `float GetRawValue(void) const`

Gets the raw axis value before processing.

**Returns:** Raw axis value before dead zone/sensitivity/smoothing

#### `float GetDeadZone(void) const`

Gets the current dead zone threshold.

**Returns:** Dead zone value (0.0 to 1.0)

#### `float GetSensitivity(void) const`

Gets the current sensitivity multiplier.

**Returns:** Sensitivity value

#### `bool IsInverted(void) const`

Checks if axis inversion is enabled.

**Returns:** True if axis is inverted, false otherwise

#### `bool IsInDeadZone(void) const`

Checks if the current value is within the dead zone.

**Returns:** True if current value is in dead zone, false otherwise

## Usage Examples

### Basic Axis Setup and Movement Control

```cpp
class APlayerController : public AController
{
public:
    virtual void BeginPlay() override
    {
        AController::BeginPlay();

        // Create movement axes
        SetupMovementAxes();

        // Connect event handlers
        ConnectAxisEvents();
    }

    virtual void Tick(Float32 deltaTime) override
    {
        AController::Tick(deltaTime);

        // Update axes (typically done by input manager)
        UpdateAxes();

        // Apply movement
        ApplyMovement(deltaTime);
    }

private:
    void SetupMovementAxes()
    {
        // Horizontal movement axis
        moveXAxis = std::make_unique<UInputAxis>("MoveX",
            std::vector<EInput>{EInput::Keyboard_A, EInput::Keyboard_D,
                               EInput::GamePad_LeftStickX},
            0.1f,   // Dead zone
            1.0f,   // Sensitivity
            false,  // Not inverted
            true,   // Smoothing enabled
            0.3f);  // Smooth factor

        // Vertical movement axis
        moveYAxis = std::make_unique<UInputAxis>("MoveY",
            std::vector<EInput>{EInput::Keyboard_W, EInput::Keyboard_S,
                               EInput::GamePad_LeftStickY},
            0.1f,   // Dead zone
            1.0f,   // Sensitivity
            true,   // Inverted (WASD: W is up, but axis positive should be up)
            true,   // Smoothing enabled
            0.3f);  // Smooth factor

        // Camera look axes
        lookXAxis = std::make_unique<UInputAxis>("LookX",
            std::vector<EInput>{EInput::Mouse_X, EInput::GamePad_RightStickX},
            0.0f,   // No dead zone for mouse
            0.5f,   // Less sensitive for camera
            false,  // Not inverted
            false,  // No smoothing for responsive camera
            0.0f);

        lookYAxis = std::make_unique<UInputAxis>("LookY",
            std::vector<EInput>{EInput::Mouse_Y, EInput::GamePad_RightStickY},
            0.0f,   // No dead zone for mouse
            0.5f,   // Less sensitive for camera
            true,   // Invert mouse Y (screen down = look down)
            false,  // No smoothing for responsive camera
            0.0f);
    }

    void ConnectAxisEvents()
    {
        // Connect value change events
        moveXAxis->On<UInputAxis::Events::ValueChanged>()
            .Connect(this, &APlayerController::OnMoveXChanged);
        moveYAxis->On<UInputAxis::Events::ValueChanged>()
            .Connect(this, &APlayerController::OnMoveYChanged);

        // Connect dead zone events
        moveXAxis->On<UInputAxis::Events::DeadZoneEntered>()
            .Connect(this, &APlayerController::OnMoveXDeadZoneEntered);
        moveXAxis->On<UInputAxis::Events::DeadZoneExited>()
            .Connect(this, &APlayerController::OnMoveXDeadZoneExited);
    }

    void UpdateAxes()
    {
        // Update all axes
        moveXAxis->UpdateValue();
        moveYAxis->UpdateValue();
        lookXAxis->UpdateValue();
        lookYAxis->UpdateValue();
    }

    void ApplyMovement(Float32 deltaTime)
    {
        if (!playerPawn) return;

        // Get axis values
        float moveX = moveXAxis->GetValue();
        float moveY = moveYAxis->GetValue();
        float lookX = lookXAxis->GetValue();
        float lookY = lookYAxis->GetValue();

        // Calculate movement vector
        FVector2 movement(moveX, moveY);
        float moveSpeed = 200.0f; // Units per second
        movement *= moveSpeed * deltaTime;

        // Apply movement
        playerPawn->Move(movement);

        // Apply camera rotation
        float lookSensitivity = 2.0f;
        FVector2 lookDelta(lookX, lookY);
        lookDelta *= lookSensitivity * deltaTime;

        playerPawn->RotateCamera(lookDelta);

        // Debug logging
        if (movement.Length() > 0.01f) {
            FLogger::Log(ELogLevel::Debug,
                FString::Format("Movement: X={:.3f}, Y={:.3f}", moveX, moveY));
        }
    }

    void OnMoveXChanged(const UInputAxis::Events::ValueChanged& event)
    {
        FLogger::Log(ELogLevel::Debug,
            FString::Format("MoveX changed: {:.3f} -> {:.3f}",
                event.oldValue, event.newValue));
    }

    void OnMoveYChanged(const UInputAxis::Events::ValueChanged& event)
    {
        FLogger::Log(ELogLevel::Debug,
            FString::Format("MoveY changed: {:.3f} -> {:.3f}",
                event.oldValue, event.newValue));
    }

    void OnMoveXDeadZoneEntered(const UInputAxis::Events::DeadZoneEntered& event)
    {
        FLogger::Log(ELogLevel::Debug, "MoveX entered dead zone");
        // Could stop movement or play haptic feedback
    }

    void OnMoveXDeadZoneExited(const UInputAxis::Events::DeadZoneExited& event)
    {
        FLogger::Log(ELogLevel::Debug, "MoveX exited dead zone");
        // Could resume movement or provide feedback
    }

private:
    std::unique_ptr<UInputAxis> moveXAxis;
    std::unique_ptr<UInputAxis> moveYAxis;
    std::unique_ptr<UInputAxis> lookXAxis;
    std::unique_ptr<UInputAxis> lookYAxis;

    APawn* playerPawn;
};
```

### Advanced Axis Processing with Custom Curves

```cpp
class AAdvancedController : public APlayerController
{
public:
    virtual void BeginPlay() override
    {
        AController::BeginPlay();

        // Create advanced axes with custom response curves
        SetupAdvancedAxes();
    }

    virtual void Tick(Float32 deltaTime) override
    {
        APlayerController::Tick(deltaTime);

        // Apply custom processing
        ProcessAdvancedAxes(deltaTime);
    }

private:
    void SetupAdvancedAxes()
    {
        // Racing game steering axis with exponential response
        steeringAxis = std::make_unique<UInputAxis>("Steering",
            std::vector<EInput>{EInput::Keyboard_A, EInput::Keyboard_D,
                               EInput::GamePad_LeftStickX},
            0.05f,  // Small dead zone
            1.0f,   // Base sensitivity
            false,  // Not inverted
            true,   // Smoothing
            0.4f);  // Moderate smoothing

        // Throttle axis with custom curve
        throttleAxis = std::make_unique<UInputAxis>("Throttle",
            std::vector<EInput>{EInput::Keyboard_W, EInput::Keyboard_S,
                               EInput::GamePad_RightTrigger, EInput::GamePad_LeftTrigger},
            0.1f,   // Dead zone
            1.0f,   // Sensitivity
            false,  // Not inverted
            true,   // Smoothing
            0.2f);  // Light smoothing

        // Camera zoom axis
        zoomAxis = std::make_unique<UInputAxis>("Zoom",
            std::vector<EInput>{EInput::Mouse_Wheel},
            0.0f,   // No dead zone for scroll
            0.1f,   // Low sensitivity
            false,  // Not inverted
            false,  // No smoothing for immediate response
            0.0f);
    }

    void ProcessAdvancedAxes(Float32 deltaTime)
    {
        // Process steering with exponential curve
        float rawSteering = steeringAxis->GetValue();
        float processedSteering = ApplySteeringCurve(rawSteering);
        ApplySteering(processedSteering);

        // Process throttle with custom curve
        float rawThrottle = throttleAxis->GetValue();
        float processedThrottle = ApplyThrottleCurve(rawThrottle);
        ApplyThrottle(processedThrottle);

        // Process zoom
        float zoomDelta = zoomAxis->GetValue();
        ApplyZoom(zoomDelta * deltaTime);
    }

    float ApplySteeringCurve(float input)
    {
        // Exponential curve: gentle at low inputs, aggressive at high inputs
        float absInput = std::abs(input);
        float sign = (input > 0.0f) ? 1.0f : -1.0f;

        if (absInput < 0.3f) {
            // Linear response for fine control
            return input * 0.5f;
        } else {
            // Exponential response for quick turns
            float expInput = std::pow(absInput, 1.5f);
            return sign * (0.5f + expInput * 0.5f);
        }
    }

    float ApplyThrottleCurve(float input)
    {
        // S-curve: slow acceleration/deceleration at low speeds,
        // fast at medium speeds, slow again at high speeds
        if (input > 0.0f) {
            // Acceleration curve
            return std::pow(input, 0.7f); // Easier to reach high speeds
        } else {
            // Braking curve
            return -std::pow(-input, 1.3f); // Harder to brake at low speeds
        }
    }

    void ApplySteering(float steering)
    {
        if (vehicle) {
            // Apply steering to vehicle
            float maxSteerAngle = 30.0f; // Degrees
            float steerAngle = steering * maxSteerAngle;
            vehicle->SetSteeringAngle(steerAngle);

            FLogger::Log(ELogLevel::Debug,
                FString::Format("Steering: {:.3f} -> {:.1f}°", steering, steerAngle));
        }
    }

    void ApplyThrottle(float throttle)
    {
        if (vehicle) {
            // Apply throttle to vehicle
            float acceleration = throttle * 10.0f; // m/s²
            vehicle->SetAcceleration(acceleration);

            FLogger::Log(ELogLevel::Debug,
                FString::Format("Throttle: {:.3f} -> {:.1f} m/s²", throttle, acceleration));
        }
    }

    void ApplyZoom(float zoomDelta)
    {
        if (camera) {
            // Apply zoom to camera
            float zoomSpeed = 5.0f;
            float newZoom = camera->GetZoom() + zoomDelta * zoomSpeed;
            newZoom = std::clamp(newZoom, 0.5f, 10.0f); // Clamp zoom range
            camera->SetZoom(newZoom);

            FLogger::Log(ELogLevel::Debug,
                FString::Format("Zoom: {:.3f} -> {:.1f}x", zoomDelta, newZoom));
        }
    }

private:
    std::unique_ptr<UInputAxis> steeringAxis;
    std::unique_ptr<UInputAxis> throttleAxis;
    std::unique_ptr<UInputAxis> zoomAxis;

    AVehicle* vehicle;
    ACamera* camera;
};
```

### Dynamic Axis Configuration and Calibration

```cpp
class ACalibratableController : public APlayerController
{
public:
    virtual void BeginPlay() override
    {
        APlayerController::BeginPlay();

        // Create calibratable axes
        SetupCalibratableAxes();

        // Load saved calibration
        LoadCalibration();
    }

    void StartCalibration(const FString& axisName)
    {
        auto it = axes.find(axisName);
        if (it == axes.end()) return;

        UInputAxis* axis = it->second.get();
        calibrationState = CalibrationState::WaitingForMin;
        calibratingAxis = axis;
        calibrationData = AxisCalibration();

        ShowCalibrationPrompt("Move axis to MINIMUM position and press Enter");
        FLogger::Log(ELogLevel::Info,
            FString::Format("Starting calibration for axis: {}", axisName));
    }

    void UpdateCalibration()
    {
        if (calibrationState == CalibrationState::Idle) return;

        float currentValue = calibratingAxis->GetRawValue();

        switch (calibrationState) {
            case CalibrationState::WaitingForMin:
                if (IsEnterPressed()) {
                    calibrationData.minValue = currentValue;
                    calibrationState = CalibrationState::WaitingForMax;
                    ShowCalibrationPrompt("Move axis to MAXIMUM position and press Enter");
                    FLogger::Log(ELogLevel::Debug,
                        FString::Format("Min value recorded: {:.3f}", currentValue));
                }
                break;

            case CalibrationState::WaitingForMax:
                if (IsEnterPressed()) {
                    calibrationData.maxValue = currentValue;
                    calibrationState = CalibrationState::WaitingForCenter;
                    ShowCalibrationPrompt("Move axis to CENTER position and press Enter");
                    FLogger::Log(ELogLevel::Debug,
                        FString::Format("Max value recorded: {:.3f}", currentValue));
                }
                break;

            case CalibrationState::WaitingForCenter:
                if (IsEnterPressed()) {
                    calibrationData.centerValue = currentValue;
                    calibrationState = CalibrationState::Complete;
                    ApplyCalibration();
                    SaveCalibration();
                    ShowCalibrationComplete();
                    FLogger::Log(ELogLevel::Info,
                        FString::Format("Calibration complete for axis: {}",
                            calibratingAxis->GetName()));
                }
                break;

            case CalibrationState::Complete:
                calibrationState = CalibrationState::Idle;
                calibratingAxis = nullptr;
                break;
        }
    }

    void ApplyCalibration()
    {
        if (!calibratingAxis || calibrationState != CalibrationState::Complete) return;

        // Calculate calibration parameters
        float range = calibrationData.maxValue - calibrationData.minValue;
        float centerOffset = calibrationData.centerValue - calibrationData.minValue;

        // Adjust sensitivity to normalize range
        float sensitivity = 2.0f / range; // Map to -1.0 to 1.0
        calibratingAxis->SetSensitivity(sensitivity);

        // Adjust dead zone based on center position
        float deadZone = std::abs(centerOffset - range * 0.5f) / range;
        calibratingAxis->SetDeadZone(deadZone * 0.1f); // Small dead zone around center

        FLogger::Log(ELogLevel::Debug,
            FString::Format("Applied calibration - Sensitivity: {:.3f}, DeadZone: {:.3f}",
                sensitivity, deadZone));
    }

    void LoadCalibration()
    {
        // Load calibration data from file
        FJsonObject calibrationJson = LoadJsonFile("AxisCalibration.json");

        if (calibrationJson.IsValid()) {
            for (const auto& axis : axes) {
                FString axisName = axis.first;
                if (calibrationJson.HasField(axisName)) {
                    FJsonObject axisCal = calibrationJson.GetObjectField(axisName);
                    AxisCalibration cal;
                    cal.minValue = axisCal.GetNumberField("min");
                    cal.maxValue = axisCal.GetNumberField("max");
                    cal.centerValue = axisCal.GetNumberField("center");

                    // Apply loaded calibration
                    calibratingAxis = axis.second.get();
                    calibrationData = cal;
                    ApplyCalibration();
                }
            }
        }
    }

    void SaveCalibration()
    {
        if (!calibratingAxis) return;

        FJsonObject calibrationJson;

        // Save current calibration data
        FJsonObject axisCal;
        axisCal.SetNumberField("min", calibrationData.minValue);
        axisCal.SetNumberField("max", calibrationData.maxValue);
        axisCal.SetNumberField("center", calibrationData.centerValue);

        calibrationJson.SetObjectField(calibratingAxis->GetName(), axisCal);

        SaveJsonFile("AxisCalibration.json", calibrationJson);
    }

    void ResetCalibration(const FString& axisName)
    {
        auto it = axes.find(axisName);
        if (it == axes.end()) return;

        UInputAxis* axis = it->second.get();

        // Reset to default values
        axis->SetSensitivity(1.0f);
        axis->SetDeadZone(0.1f);
        axis->SetInverted(false);
        axis->SetSmoothing(false, 0.0f);

        FLogger::Log(ELogLevel::Info,
            FString::Format("Reset calibration for axis: {}", axisName));
    }

private:
    void SetupCalibratableAxes()
    {
        // Create axes that can be calibrated
        axes["Throttle"] = std::make_unique<UInputAxis>("Throttle",
            std::vector<EInput>{EInput::GamePad_RightTrigger}, 0.1f, 1.0f, false, false, 0.0f);

        axes["Brake"] = std::make_unique<UInputAxis>("Brake",
            std::vector<EInput>{EInput::GamePad_LeftTrigger}, 0.1f, 1.0f, false, false, 0.0f);

        axes["Steering"] = std::make_unique<UInputAxis>("Steering",
            std::vector<EInput>{EInput::GamePad_LeftStickX}, 0.1f, 1.0f, false, false, 0.0f);
    }

    enum class CalibrationState {
        Idle,
        WaitingForMin,
        WaitingForMax,
        WaitingForCenter,
        Complete
    };

    struct AxisCalibration {
        float minValue = 0.0f;
        float maxValue = 0.0f;
        float centerValue = 0.0f;
    };

    std::unordered_map<FString, std::unique_ptr<UInputAxis>> axes;
    CalibrationState calibrationState = CalibrationState::Idle;
    UInputAxis* calibratingAxis = nullptr;
    AxisCalibration calibrationData;
};
```

### Axis Monitoring and Debugging

```cpp
class AxisDebugger
{
public:
    void MonitorAxis(UInputAxis* axis)
    {
        monitoredAxes.push_back(axis);

        // Connect to all events
        axis->On<UInputAxis::Events::ValueChanged>()
            .Connect(this, &AxisDebugger::OnAxisValueChanged);
        axis->On<UInputAxis::Events::DeadZoneEntered>()
            .Connect(this, &AxisDebugger::OnAxisDeadZoneEntered);
        axis->On<UInputAxis::Events::DeadZoneExited>()
            .Connect(this, &AxisDebugger::OnAxisDeadZoneExited);
    }

    void LogAxisStates()
    {
        FLogger::Log(ELogLevel::Debug, "=== Axis States ===");

        for (UInputAxis* axis : monitoredAxes) {
            FLogger::Log(ELogLevel::Debug,
                FString::Format("Axis '{}': Value={:.3f}, Raw={:.3f}, InDeadZone={}",
                    axis->GetName(),
                    axis->GetValue(),
                    axis->GetRawValue(),
                    axis->IsInDeadZone()));

            FLogger::Log(ELogLevel::Debug,
                FString::Format("  Config - DeadZone={:.3f}, Sensitivity={:.3f}, Inverted={}, Smooth={}",
                    axis->GetDeadZone(),
                    axis->GetSensitivity(),
                    axis->IsInverted(),
                    axis->GetSmoothingEnabled()));

            const auto& inputs = axis->GetInputs();
            FLogger::Log(ELogLevel::Debug,
                FString::Format("  Bound inputs: {}", inputs.size()));

            for (size_t i = 0; i < inputs.size(); ++i) {
                FLogger::Log(ELogLevel::Debug,
                    FString::Format("    [{}]: {}", i, (int)inputs[i]));
            }
        }
    }

    void LogAxisStatistics()
    {
        FLogger::Log(ELogLevel::Debug, "=== Axis Statistics ===");

        for (const auto& [axisName, stats] : axisStats) {
            FLogger::Log(ELogLevel::Debug,
                FString::Format("Axis '{}': Changes={}, DeadZoneEntries={}, DeadZoneExits={}",
                    axisName, stats.valueChanges, stats.deadZoneEntries, stats.deadZoneExits));

            FLogger::Log(ELogLevel::Debug,
                FString::Format("  Value Range: Min={:.3f}, Max={:.3f}, Avg={:.3f}",
                    stats.minValue, stats.maxValue, stats.avgValue));
        }

        // Reset statistics
        axisStats.clear();
    }

    void PlotAxisValue(UInputAxis* axis, float timeWindow = 5.0f)
    {
        // Record axis values over time for plotting
        FString axisName = axis->GetName();
        auto& history = valueHistory[axisName];

        history.push_back({GetCurrentTime(), axis->GetValue()});

        // Remove old values outside time window
        float currentTime = GetCurrentTime();
        history.erase(
            std::remove_if(history.begin(), history.end(),
                [currentTime, timeWindow](const ValuePoint& p) {
                    return currentTime - p.time > timeWindow;
                }),
            history.end());

        // Log current plot data
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Axis '{}' plot (last {}s): {} points",
                axisName, timeWindow, history.size()));

        for (size_t i = 0; i < history.size(); ++i) {
            FLogger::Log(ELogLevel::Debug,
                FString::Format("  {:.2f}s: {:.3f}",
                    history[i].time - currentTime + timeWindow,
                    history[i].value));
        }
    }

private:
    void OnAxisValueChanged(const UInputAxis::Events::ValueChanged& event)
    {
        // Find axis name (simplified - would need better tracking)
        FString axisName = "Unknown";
        for (UInputAxis* axis : monitoredAxes) {
            if (axis->GetValue() == event.newValue) {
                axisName = axis->GetName();
                break;
            }
        }

        axisStats[axisName].valueChanges++;
        axisStats[axisName].minValue = std::min(axisStats[axisName].minValue, event.newValue);
        axisStats[axisName].maxValue = std::max(axisStats[axisName].maxValue, event.newValue);
        axisStats[axisName].avgValue = (axisStats[axisName].avgValue + event.newValue) * 0.5f;

        FLogger::Log(ELogLevel::Debug,
            FString::Format("Axis '{}' value changed: {:.3f} -> {:.3f}",
                axisName, event.oldValue, event.newValue));
    }

    void OnAxisDeadZoneEntered(const UInputAxis::Events::DeadZoneEntered& event)
    {
        axisStats["Unknown"].deadZoneEntries++; // Simplified
        FLogger::Log(ELogLevel::Debug, "Axis entered dead zone");
    }

    void OnAxisDeadZoneExited(const UInputAxis::Events::DeadZoneExited& event)
    {
        axisStats["Unknown"].deadZoneExits++; // Simplified
        FLogger::Log(ELogLevel::Debug, "Axis exited dead zone");
    }

private:
    std::vector<UInputAxis*> monitoredAxes;

    struct AxisStatistics {
        size_t valueChanges = 0;
        size_t deadZoneEntries = 0;
        size_t deadZoneExits = 0;
        float minValue = FLT_MAX;
        float maxValue = -FLT_MAX;
        float avgValue = 0.0f;
    };

    struct ValuePoint {
        float time;
        float value;
    };

    std::unordered_map<FString, AxisStatistics> axisStats;
    std::unordered_map<FString, std::vector<ValuePoint>> valueHistory;
};
```

## Value Processing Architecture

### Processing Pipeline Stages

```
1. Input Aggregation
   ├── Sum Mode: Add all input values
   ├── Average Mode: Average all input values
   └── Max Mode: Use largest absolute value

2. Dead Zone Application
   ├── Input < DeadZone → 0.0
   └── Input ≥ DeadZone → Scaled to full range

3. Sensitivity Scaling
   └── Value *= Sensitivity

4. Inversion
   ├── Inverted = true → Value *= -1
   └── Inverted = false → Value unchanged

5. Smoothing
   └── Value = Previous + (Current - Previous) * Factor

6. Event Generation
   ├── ValueChanged: When final value changes
   ├── DeadZoneEntered: When entering dead zone
   └── DeadZoneExited: When exiting dead zone
```

### Performance Characteristics

- **Update Frequency**: Called every frame (60-120 FPS)
- **Input Polling**: Multiple device queries per axis
- **Math Operations**: Floating-point calculations per axis
- **Event Emission**: Conditional callback execution
- **Memory Access**: Frequent axis state reads/writes

### Optimization Strategies

1. **Batch Updates**: Update multiple axes together
2. **Lazy Evaluation**: Only update when inputs change
3. **SIMD Processing**: Vectorize multiple axis computations
4. **Event Batching**: Group similar events
5. **Caching**: Cache computed values when possible

## Troubleshooting

### Axis Values Not Updating

**Symptoms:** Axis values remain at 0.0 or don't change

**Possible Causes:**
- No input bindings configured
- Input devices not working
- Dead zone too large
- Input manager not updating axes
- Axis not being polled

**Solutions:**
```cpp
// Debug axis configuration
void DebugAxisConfiguration(const UInputAxis* axis) {
    FLogger::Log(ELogLevel::Debug,
        FString::Format("Debugging axis: {}", axis->GetName()));

    const auto& inputs = axis->GetInputs();
    FLogger::Log(ELogLevel::Debug,
        FString::Format("Bound inputs: {}", inputs.size()));

    for (size_t i = 0; i < inputs.size(); ++i) {
        FLogger::Log(ELogLevel::Debug,
            FString::Format("  Input {}: {}", i, (int)inputs[i]));
    }

    FLogger::Log(ELogLevel::Debug,
        FString::Format("Configuration: DeadZone={:.3f}, Sensitivity={:.3f}, Inverted={}",
            axis->GetDeadZone(), axis->GetSensitivity(), axis->IsInverted()));

    FLogger::Log(ELogLevel::Debug,
        FString::Format("Current state: Value={:.3f}, RawValue={:.3f}, InDeadZone={}",
            axis->GetValue(), axis->GetRawValue(), axis->IsInDeadZone()));
}

// Test axis manually
void TestAxisManually(UInputAxis* axis, float testValue) {
    FLogger::Log(ELogLevel::Debug,
        FString::Format("Testing axis '{}' with value {:.3f}", axis->GetName(), testValue));

    axis->SetRawValue(testValue);

    FLogger::Log(ELogLevel::Debug,
        FString::Format("Result: Value={:.3f}, RawValue={:.3f}",
            axis->GetValue(), axis->GetRawValue()));
}
```

### Erratic Axis Values

**Symptoms:** Axis values jump around or are unstable

**Possible Causes:**
- No smoothing enabled
- Dead zone too small
- Input device noise
- Sensitivity too high
- Multiple conflicting inputs

**Solutions:**
```cpp
// Stabilize axis configuration
void StabilizeAxis(UInputAxis* axis) {
    // Increase dead zone to reduce noise
    axis->SetDeadZone(0.15f);

    // Enable smoothing
    axis->SetSmoothing(true, 0.3f);

    // Reduce sensitivity
    axis->SetSensitivity(0.8f);

    FLogger::Log(ELogLevel::Info,
        FString::Format("Stabilized axis: {}", axis->GetName()));
}

// Debug input conflicts
void DebugInputConflicts(const UInputAxis* axis) {
    const auto& inputs = axis->GetInputs();

    FLogger::Log(ELogLevel::Debug,
        FString::Format("Checking for input conflicts in axis: {}", axis->GetName()));

    // Check for conflicting inputs (simplified check)
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
```

### Dead Zone Issues

**Symptoms:** Axis doesn't respond to small inputs or has dead spots

**Possible Causes:**
- Dead zone too large
- Dead zone too small
- Dead zone calculation incorrect

**Solutions:**
```cpp
// Test dead zone behavior
void TestDeadZone(UInputAxis* axis) {
    FLogger::Log(ELogLevel::Debug,
        FString::Format("Testing dead zone for axis: {}", axis->GetName()));

    float deadZone = axis->GetDeadZone();
    FLogger::Log(ELogLevel::Debug,
        FString::Format("Dead zone threshold: {:.3f}", deadZone));

    // Test various input values
    std::vector<float> testValues = {0.0f, 0.05f, 0.1f, 0.15f, 0.5f, 1.0f};

    for (float testValue : testValues) {
        axis->SetRawValue(testValue);
        float result = axis->GetValue();
        bool inDeadZone = axis->IsInDeadZone();

        FLogger::Log(ELogLevel::Debug,
            FString::Format("  Input {:.3f} -> Output {:.3f} (InDeadZone: {})",
                testValue, result, inDeadZone));
    }
}

// Calibrate dead zone
void CalibrateDeadZone(UInputAxis* axis) {
    FLogger::Log(ELogLevel::Debug,
        FString::Format("Calibrating dead zone for axis: {}", axis->GetName()));

    // Sample input noise
    std::vector<float> samples;
    for (int i = 0; i < 100; ++i) {
        // Sample input over time
        axis->UpdateValue();
        samples.push_back(std::abs(axis->GetRawValue()));
        Sleep(10); // Small delay
    }

    // Calculate noise threshold
    float maxNoise = *std::max_element(samples.begin(), samples.end());
    float recommendedDeadZone = maxNoise * 1.2f; // 20% margin

    axis->SetDeadZone(std::clamp(recommendedDeadZone, 0.01f, 0.5f));

    FLogger::Log(ELogLevel::Info,
        FString::Format("Calibrated dead zone: {:.3f} (max noise: {:.3f})",
            axis->GetDeadZone(), maxNoise));
}
```

## Future Enhancements

### Planned Features

1. **Advanced Response Curves**: Customizable input response curves
2. **Input Prediction**: Predictive axis value estimation
3. **Haptic Feedback**: Axis-based vibration patterns
4. **Multi-Point Calibration**: Advanced calibration procedures
5. **Input Blending**: Smooth transitions between input devices
6. **Context-Aware Processing**: Different processing based on game state
7. **Performance Profiling**: Built-in axis performance monitoring
8. **Accessibility**: Alternative input mapping for accessibility

### Extension Points

The axis system allows for easy extension:

```cpp
class AdvancedInputAxis : public UInputAxis {
public:
    // Add response curve support
    void SetResponseCurve(const std::function<float(float)>& curve) {
        responseCurve = curve;
    }

    virtual void UpdateValue() override {
        UInputAxis::UpdateValue();

        // Apply custom response curve
        if (responseCurve) {
            float rawValue = GetRawValue();
            float curvedValue = responseCurve(rawValue);
            SetRawValue(curvedValue);
        }
    }

    // Add input prediction
    void EnablePrediction(bool enabled, float predictionTime = 0.1f) {
        predictionEnabled = enabled;
        this->predictionTime = predictionTime;
    }

    virtual void UpdateValue() override {
        // Store previous values for prediction
        previousValues.push_back(GetValue());
        if (previousValues.size() > 10) {
            previousValues.erase(previousValues.begin());
        }

        UInputAxis::UpdateValue();

        if (predictionEnabled && previousValues.size() >= 3) {
            float predictedValue = PredictNextValue();
            // Blend current and predicted values
            float blendedValue = GetValue() * 0.8f + predictedValue * 0.2f;
            SetRawValue(blendedValue);
        }
    }

private:
    float PredictNextValue() {
        // Simple linear prediction based on recent values
        if (previousValues.size() < 2) return GetValue();

        float velocity = previousValues.back() - previousValues[previousValues.size() - 2];
        return GetValue() + velocity * predictionTime;
    }

    std::function<float(float)> responseCurve;
    bool predictionEnabled = false;
    float predictionTime = 0.1f;
    std::vector<float> previousValues;
};

// Context-aware axis
class ContextualInputAxis : public UInputAxis {
public:
    void SetContextMultiplier(const FString& context, float multiplier) {
        contextMultipliers[context] = multiplier;
    }

    void SetActiveContext(const FString& context) {
        activeContext = context;
    }

    virtual void UpdateValue() override {
        UInputAxis::UpdateValue();

        // Apply context multiplier
        auto it = contextMultipliers.find(activeContext);
        if (it != contextMultipliers.end()) {
            float multiplier = it->second;
            SetRawValue(GetValue() * multiplier);
        }
    }

private:
    std::unordered_map<FString, float> contextMultipliers;
    FString activeContext;
};
```

---

*This documentation covers the complete UInputAxis implementation as of TKD Engine v1.0.0. For the latest updates and additional examples, refer to the engine's source code and test suites.*
