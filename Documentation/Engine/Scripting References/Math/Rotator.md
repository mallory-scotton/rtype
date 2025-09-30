# TRotator

## Overview

The `TRotator` template class represents 3D rotations using Euler angles (pitch, yaw, roll). It provides comprehensive rotation operations, angle normalization, and both component-based and static utility methods. The class handles angle wrapping, supports various rotation operations, and integrates seamlessly with 3D transformation systems.

## Basic Usage

### Creating Rotators

```cpp
#include <Engine/Core/Math/TRotator.hpp>

// Default constructor - identity rotation (0, 0, 0)
tkd::Rotator rotator;

// Constructor with pitch, yaw, and roll
tkd::Rotator rotator(45.0f, 90.0f, 30.0f);  // pitch, yaw, roll in degrees

// Copy constructor
tkd::Rotator otherRotator = rotator;

// Using identity rotator
tkd::Rotator identityRotator = tkd::Rotator::Identity;
```

### Basic Operations

```cpp
tkd::Rotator rotator;

// Set individual components
rotator.SetPitch(45.0f);
rotator.SetYaw(90.0f);
rotator.SetRoll(30.0f);

// Get individual components
float pitch = rotator.GetPitch();
float yaw = rotator.GetYaw();
float roll = rotator.GetRoll();

// Print rotator information
std::cout << rotator << std::endl;
// Output: Rotator(Pitch: 45, Yaw: 90, Roll: 30)
```

## Euler Angle Components

### Pitch (X-axis rotation)

-   **Range**: -180° to +180°
-   **Positive**: Looking up
-   **Negative**: Looking down

### Yaw (Z-axis rotation)

-   **Range**: -180° to +180°
-   **Positive**: Turning left
-   **Negative**: Turning right

### Roll (Y-axis rotation)

-   **Range**: -180° to +180°
-   **Positive**: Rolling right
-   **Negative**: Rolling left

## Component Methods

### Getters

#### `GetPitch() const` / `GetYaw() const` / `GetRoll() const`

Returns the individual Euler angle components in degrees.

```cpp
tkd::Rotator rotator(30.0f, 45.0f, 60.0f);

float pitch = rotator.GetPitch();  // 30.0
float yaw = rotator.GetYaw();      // 45.0
float roll = rotator.GetRoll();    // 60.0
```

### Setters

#### `SetPitch(const T& pitch)` / `SetYaw(const T& yaw)` / `SetRoll(const T& roll)`

Sets individual Euler angle components with automatic normalization.

```cpp
rotator.SetPitch(45.0f);
rotator.SetYaw(-90.0f);
rotator.SetRoll(180.0f);

// Angles are automatically normalized to [-180, 180] range
rotator.SetYaw(270.0f);   // Becomes -90.0f
rotator.SetPitch(200.0f); // Becomes -160.0f
```

### Rotation Operations

#### `Rotate(const TRotator<T>& rotation)`

Applies additional rotation to the current rotation (relative rotation).

```cpp
tkd::Rotator baseRotation(0.0f, 45.0f, 0.0f);
tkd::Rotator additionalRotation(15.0f, 30.0f, -10.0f);

baseRotation.Rotate(additionalRotation);
// Result: baseRotation is now (15, 75, -10) with normalization
```

### Utility Methods

#### `IsIdentity() const`

Checks if the rotator represents no rotation (all angles are zero).

```cpp
tkd::Rotator rotator;
if (rotator.IsIdentity()) {
    std::cout << "No rotation applied" << std::endl;
}
```

#### `Reset()`

Resets all rotation components to zero.

```cpp
rotator.Reset();  // Sets pitch, yaw, roll to 0
```

#### `Equals(const TRotator& other, const T& tolerance) const`

Compares two rotators with a specified tolerance.

```cpp
tkd::Rotator rot1(45.01f, 90.0f, 30.0f);
tkd::Rotator rot2(45.0f, 90.0f, 30.0f);

if (rot1.Equals(rot2, 0.1f)) {
    std::cout << "Rotators are approximately equal" << std::endl;
}
```

## Static Utility Methods

### Static Rotation Operations

#### `Rotate(TRotator<T>& rotator, const TRotator<T>& rotation)`

Static method to apply rotation to a rotator.

```cpp
tkd::Rotator myRotator(45.0f, 90.0f, 0.0f);
tkd::Rotator deltaRotation(15.0f, -30.0f, 45.0f);

// Static rotation
tkd::Rotator::Rotate(myRotator, deltaRotation);

// Equivalent to: myRotator.Rotate(deltaRotation);
```

#### `Rotate(TRotator<T>& rotator, const T& pitch, const T& yaw, const T& roll)`

Static method to apply individual angle rotations.

```cpp
tkd::Rotator myRotator;

// Static rotation with individual angles
tkd::Rotator::Rotate(myRotator, 45.0f, 90.0f, 0.0f);

// Equivalent to: myRotator.Rotate(tkd::Rotator(45.0f, 90.0f, 0.0f));
```

## Mathematical Operators

### Addition Operators

#### `operator+(const TRotator<T>& lhs, const TRotator<T>& rhs)`

Adds two rotators component-wise with normalization.

```cpp
tkd::Rotator rot1(45.0f, 90.0f, 30.0f);
tkd::Rotator rot2(15.0f, -45.0f, 60.0f);

tkd::Rotator result = rot1 + rot2;  // (60, 45, 90)
```

#### `operator+=(TRotator<T>& lhs, const TRotator<T>& rhs)`

Adds a rotator to the current rotator in place.

```cpp
tkd::Rotator rotator(45.0f, 90.0f, 30.0f);
tkd::Rotator delta(15.0f, -30.0f, 10.0f);

rotator += delta;  // rotator becomes (60, 60, 40)
```

### Comparison Operators

#### `operator==(const TRotator<T>& lhs, const TRotator<T>& rhs)`

Checks exact equality between rotators.

```cpp
tkd::Rotator rot1(45.0f, 90.0f, 30.0f);
tkd::Rotator rot2(45.0f, 90.0f, 30.0f);

if (rot1 == rot2) {
    std::cout << "Rotators are identical" << std::endl;
}
```

#### `operator!=(const TRotator<T>& lhs, const TRotator<T>& rhs)`

Checks inequality between rotators.

```cpp
if (rot1 != rot2) {
    std::cout << "Rotators are different" << std::endl;
}
```

## Angle Normalization

The TRotator class automatically normalizes angles to the range [-180°, +180°]:

```cpp
tkd::Rotator rotator;

rotator.SetYaw(270.0f);   // Internally stored as -90.0f
rotator.SetPitch(380.0f); // Internally stored as 20.0f
rotator.SetRoll(-200.0f); // Internally stored as 160.0f

// Addition also normalizes results
tkd::Rotator rot1(170.0f, 0.0f, 0.0f);
tkd::Rotator rot2(20.0f, 0.0f, 0.0f);
tkd::Rotator result = rot1 + rot2;  // Pitch becomes -170.0f (normalized)
```

## Advanced Usage

### Type Conversion

```cpp
// Float precision (default)
tkd::Rotator floatRotator;  // Same as TRotator<float>

// Double precision
tkd::Rotatord doubleRotator;  // Same as TRotator<double>

// Cross-type construction
tkd::Rotator floatRotator(45.0f, 90.0f, 30.0f);
tkd::Rotatord doubleRotator(floatRotator);  // Converts float to double
```

### Identity Rotator

```cpp
// Reset to identity
myRotator = tkd::Rotator::Identity;

// Check if identity
if (myRotator.IsIdentity()) {
    std::cout << "No rotation" << std::endl;
}
```

### Output and Debugging

```cpp
tkd::Rotator rotator(45.0f, -90.0f, 120.0f);

std::cout << rotator << std::endl;
// Output: Rotator(Pitch: 45, Yaw: -90, Roll: 120)
```

## Integration with Game Systems

### First-Person Camera

```cpp
class FPSCamera {
private:
    tkd::Rotator m_rotation;
    float m_mouseSensitivity;

public:
    void UpdateRotation(float mouseDeltaX, float mouseDeltaY) {
        float yawDelta = -mouseDeltaX * m_mouseSensitivity;
        float pitchDelta = -mouseDeltaY * m_mouseSensitivity;

        // Apply mouse input
        m_rotation.Rotate(tkd::Rotator(pitchDelta, yawDelta, 0.0f));

        // Clamp pitch to prevent over-rotation
        float pitch = m_rotation.GetPitch();
        pitch = std::clamp(pitch, -89.0f, 89.0f);
        m_rotation.SetPitch(pitch);
    }
};
```

### Character Controller

```cpp
class Character {
private:
    tkd::Rotator m_rotation;
    float m_turnSpeed;

public:
    void TurnTowards(const tkd::Vector3& target, float deltaTime) {
        // Calculate desired yaw to face target
        tkd::Vector3 direction = (target - GetPosition()).Normalized();
        float targetYaw = std::atan2(direction.x, direction.z) * (180.0f / PI);

        // Smoothly rotate towards target
        float currentYaw = m_rotation.GetYaw();
        float yawDelta = AngleDifference(targetYaw, currentYaw);
        float rotationStep = m_turnSpeed * deltaTime;

        if (std::abs(yawDelta) > rotationStep) {
            yawDelta = std::copysign(rotationStep, yawDelta);
        }

        m_rotation.Rotate(tkd::Rotator(0.0f, yawDelta, 0.0f));
    }
};
```

### Animation System

```cpp
class RotationAnimator {
public:
    tkd::Rotator InterpolateRotation(
        const tkd::Rotator& from,
        const tkd::Rotator& to,
        float t
    ) {
        // Spherical linear interpolation for smoother rotation
        float pitch = LerpAngle(from.GetPitch(), to.GetPitch(), t);
        float yaw = LerpAngle(from.GetYaw(), to.GetYaw(), t);
        float roll = LerpAngle(from.GetRoll(), to.GetRoll(), t);

        return tkd::Rotator(pitch, yaw, roll);
    }

private:
    float LerpAngle(float from, float to, float t) {
        float delta = to - from;

        // Handle angle wrapping
        if (delta > 180.0f) delta -= 360.0f;
        if (delta < -180.0f) delta += 360.0f;

        return from + delta * t;
    }
};
```

## Performance Considerations

### Efficient Usage

```cpp
// Good: Batch angle modifications
rotator.SetPitch(newPitch);
rotator.SetYaw(newYaw);
rotator.SetRoll(newRoll);

// Good: Use const references
const tkd::Rotator& rotation = transform.GetRotation();
float yaw = rotation.GetYaw();

// Avoid: Frequent temporary rotator creation in loops
for (int i = 0; i < objects.size(); ++i) {
    objects[i].Rotate(tkd::Rotator(deltaTime, 0, 0));  // Creates temporary each iteration
}

// Better: Create once, reuse
tkd::Rotator deltaRotation(deltaTime, 0, 0);
for (int i = 0; i < objects.size(); ++i) {
    objects[i].Rotate(deltaRotation);
}
```

### Memory Layout

The rotator stores three float/double values:

-   `T m_pitch` - sizeof(T) bytes
-   `T m_yaw` - sizeof(T) bytes
-   `T m_roll` - sizeof(T) bytes

Total size: 3 × sizeof(T) bytes (12 bytes for float, 24 bytes for double)

## Best Practices

### Angle Handling

```cpp
// Good: Use normalized angles for consistency
rotator.SetYaw(angle);  // Automatically normalizes

// Good: Handle angle wrapping in interpolation
float LerpAngle(float from, float to, float t);

// Avoid: Manual angle clamping
float yaw = rotator.GetYaw();
if (yaw > 180.0f) yaw -= 360.0f;  // Already handled internally
```

### Gimbal Lock Awareness

```cpp
// Be aware: Euler angles can experience gimbal lock at ±90° pitch
// Consider quaternions for complex 3D rotations
class AdvancedRotation {
private:
    tkd::Rotator m_eulerRotation;    // For simple rotations
    // Add quaternion support for complex cases
};
```

### Input Handling

```cpp
// Good: Clamp pitch for FPS cameras
void UpdateCameraRotation(float pitchDelta, float yawDelta) {
    m_rotation.Rotate(tkd::Rotator(pitchDelta, yawDelta, 0.0f));

    // Prevent over-rotation
    float pitch = m_rotation.GetPitch();
    pitch = std::clamp(pitch, -89.0f, 89.0f);
    m_rotation.SetPitch(pitch);
}
```

## Type Aliases

```cpp
using Rotator = TRotator<float>;    // Single precision
using Rotatord = TRotator<double>;  // Double precision

// Usage
Rotator cameraRotation;      // Most common
Rotatord precisionRotation;  // For high-precision calculations
```

## Related Classes

-   **TTransform**: Contains TRotator for 3D transformations
-   **TRotator2D**: 2D version with single angle
-   **TVector3**: Can be used with rotation operations
-   **TQuaternion**: Alternative rotation representation (if available)

The TRotator class provides a robust and intuitive way to handle 3D rotations using Euler angles, with automatic normalization and comprehensive mathematical operations for game engines and 3D applications.
