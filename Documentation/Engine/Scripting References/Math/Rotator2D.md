# TRotator2D

## Overview

The `TRotator2D` template class represents 2D rotations using a single angle value in degrees. It provides simple yet comprehensive rotation operations, automatic angle normalization, and both component-based and static utility methods. The class handles angle wrapping, supports various rotation operations, and is optimized for 2D game development and UI transformations.

## Basic Usage

### Creating 2D Rotators

```cpp
#include <Engine/Core/Math/TRotator2D.hpp>

// Default constructor - no rotation (0°)
tkd::Rotator2D rotator;

// Constructor with angle in degrees
tkd::Rotator2D rotator(45.0f);    // 45 degrees
tkd::Rotator2D rotator(-90.0f);   // -90 degrees (270° clockwise)

// Copy constructor
tkd::Rotator2D otherRotator = rotator;

// Using identity rotator
tkd::Rotator2D identityRotator = tkd::Rotator2D::Identity;
```

### Basic Operations

```cpp
tkd::Rotator2D rotator;

// Set angle
rotator.SetAngle(90.0f);  // 90 degrees counter-clockwise

// Get angle
float angle = rotator.GetAngle();  // Returns 90.0f

// Print rotator information
std::cout << rotator << std::endl;
// Output: Rotator2D(Angle: 90)
```

## Angle Conventions

### Angle Direction

-   **Positive angles**: Counter-clockwise rotation
-   **Negative angles**: Clockwise rotation
-   **Range**: Automatically normalized to [-180°, +180°]
-   **Zero**: Points along positive X-axis (right direction)

### Common Angles

```cpp
tkd::Rotator2D right(0.0f);      // Points right (→)
tkd::Rotator2D up(90.0f);        // Points up (↑)
tkd::Rotator2D left(180.0f);     // Points left (←)
tkd::Rotator2D down(-90.0f);     // Points down (↓)
```

## Component Methods

### Getters

#### `GetAngle() const`

Returns the rotation angle in degrees.

```cpp
tkd::Rotator2D rotator(135.0f);
float angle = rotator.GetAngle();  // 135.0
```

### Setters

#### `SetAngle(const T& angle)`

Sets the rotation angle with automatic normalization.

```cpp
rotator.SetAngle(45.0f);    // 45 degrees
rotator.SetAngle(270.0f);   // Becomes -90.0f (normalized)
rotator.SetAngle(450.0f);   // Becomes 90.0f (normalized)
rotator.SetAngle(-200.0f);  // Becomes 160.0f (normalized)
```

### Rotation Operations

#### `Rotate(const TRotator2D<T>& rotation)`

Applies additional rotation to the current rotation (relative rotation).

```cpp
tkd::Rotator2D baseRotation(45.0f);
tkd::Rotator2D additionalRotation(30.0f);

baseRotation.Rotate(additionalRotation);
// Result: baseRotation is now 75.0° with normalization
```

#### `Rotate(const T& angle)`

Applies angle rotation directly.

```cpp
tkd::Rotator2D rotator(45.0f);
rotator.Rotate(30.0f);  // Now 75.0°
rotator.Rotate(-120.0f); // Now -45.0°
```

### Utility Methods

#### `IsIdentity() const`

Checks if the rotator represents no rotation (angle is zero).

```cpp
tkd::Rotator2D rotator;
if (rotator.IsIdentity()) {
    std::cout << "No rotation applied" << std::endl;
}

rotator.SetAngle(360.0f);  // Normalized to 0.0f
if (rotator.IsIdentity()) {
    std::cout << "Still identity after 360° rotation" << std::endl;
}
```

#### `Reset()`

Resets the rotation angle to zero.

```cpp
rotator.SetAngle(120.0f);
rotator.Reset();  // Angle becomes 0.0f
```

#### `Equals(const TRotator2D& other, const T& tolerance) const`

Compares two rotators with a specified tolerance.

```cpp
tkd::Rotator2D rot1(45.01f);
tkd::Rotator2D rot2(45.0f);

if (rot1.Equals(rot2, 0.1f)) {
    std::cout << "Rotators are approximately equal" << std::endl;
}
```

## Static Utility Methods

### Static Rotation Operations

#### `Rotate(TRotator2D<T>& rotator, const TRotator2D<T>& rotation)`

Static method to apply rotation to a 2D rotator.

```cpp
tkd::Rotator2D myRotator(45.0f);
tkd::Rotator2D deltaRotation(30.0f);

// Static rotation
tkd::Rotator2D::Rotate(myRotator, deltaRotation);

// Equivalent to: myRotator.Rotate(deltaRotation);
```

#### `Rotate(TRotator2D<T>& rotator, const T& angle)`

Static method to apply angle rotation.

```cpp
tkd::Rotator2D myRotator(45.0f);

// Static rotation with angle
tkd::Rotator2D::Rotate(myRotator, 30.0f);

// Equivalent to: myRotator.Rotate(30.0f);
```

## Mathematical Operators

### Addition Operators

#### `operator+(const TRotator2D<T>& lhs, const TRotator2D<T>& rhs)`

Adds two rotators with normalization.

```cpp
tkd::Rotator2D rot1(45.0f);
tkd::Rotator2D rot2(60.0f);

tkd::Rotator2D result = rot1 + rot2;  // 105.0°

// With normalization
tkd::Rotator2D rot3(170.0f);
tkd::Rotator2D rot4(30.0f);
tkd::Rotator2D result2 = rot3 + rot4;  // -160.0° (normalized from 200°)
```

#### `operator+=(TRotator2D<T>& lhs, const TRotator2D<T>& rhs)`

Adds a rotator to the current rotator in place.

```cpp
tkd::Rotator2D rotator(45.0f);
tkd::Rotator2D delta(30.0f);

rotator += delta;  // rotator becomes 75.0°
```

### Comparison Operators

#### `operator==(const TRotator2D<T>& lhs, const TRotator2D<T>& rhs)`

Checks exact equality between rotators.

```cpp
tkd::Rotator2D rot1(45.0f);
tkd::Rotator2D rot2(45.0f);
tkd::Rotator2D rot3(405.0f);  // Normalized to 45.0f

if (rot1 == rot2) {
    std::cout << "rot1 equals rot2" << std::endl;
}

if (rot1 == rot3) {
    std::cout << "rot1 equals rot3 (after normalization)" << std::endl;
}
```

#### `operator!=(const TRotator2D<T>& lhs, const TRotator2D<T>& rhs)`

Checks inequality between rotators.

```cpp
if (rot1 != rot2) {
    std::cout << "Rotators are different" << std::endl;
}
```

## Angle Normalization

The TRotator2D class automatically normalizes angles to the range [-180°, +180°]:

```cpp
tkd::Rotator2D rotator;

rotator.SetAngle(270.0f);   // Internally stored as -90.0f
rotator.SetAngle(380.0f);   // Internally stored as 20.0f
rotator.SetAngle(-200.0f);  // Internally stored as 160.0f
rotator.SetAngle(180.0f);   // Stored as 180.0f (boundary case)
rotator.SetAngle(-180.0f);  // Stored as -180.0f (boundary case)

// Addition also normalizes results
tkd::Rotator2D rot1(170.0f);
tkd::Rotator2D rot2(20.0f);
tkd::Rotator2D result = rot1 + rot2;  // -170.0f (normalized from 190°)
```

## Advanced Usage

### Type Conversion

```cpp
// Float precision (default)
tkd::Rotator2D floatRotator;  // Same as TRotator2D<float>

// Double precision
tkd::Rotator2Dd doubleRotator;  // Same as TRotator2D<double>

// Cross-type construction
tkd::Rotator2D floatRotator(45.0f);
tkd::Rotator2Dd doubleRotator(floatRotator);  // Converts float to double
```

### Identity Rotator

```cpp
// Reset to identity
myRotator = tkd::Rotator2D::Identity;

// Check if identity
if (myRotator.IsIdentity()) {
    std::cout << "No rotation" << std::endl;
}

// Multiple ways to create identity
tkd::Rotator2D identity1;           // Default constructor
tkd::Rotator2D identity2(0.0f);     // Explicit zero
tkd::Rotator2D identity3(360.0f);   // Normalized to zero
```

### Output and Debugging

```cpp
tkd::Rotator2D rotator(120.0f);

std::cout << rotator << std::endl;
// Output: Rotator2D(Angle: 120)

tkd::Rotator2D normalized(450.0f);
std::cout << normalized << std::endl;
// Output: Rotator2D(Angle: 90) - shows normalized value
```

## Integration with Game Systems

### 2D Character Movement

```cpp
class Character2D {
private:
    tkd::Vector2 m_position;
    tkd::Rotator2D m_rotation;
    float m_speed;

public:
    void MoveForward(float deltaTime) {
        // Convert angle to direction vector
        float angleRad = m_rotation.GetAngle() * (PI / 180.0f);
        tkd::Vector2 direction(std::cos(angleRad), std::sin(angleRad));

        m_position += direction * m_speed * deltaTime;
    }

    void Turn(float angleDelta) {
        m_rotation.Rotate(angleDelta);
    }
};
```

### 2D Sprite Rotation

```cpp
class Sprite {
private:
    tkd::Rotator2D m_rotation;
    float m_rotationSpeed;  // degrees per second

public:
    void Update(float deltaTime) {
        // Continuous rotation
        float deltaAngle = m_rotationSpeed * deltaTime;
        m_rotation.Rotate(deltaAngle);
    }

    void FaceDirection(const tkd::Vector2& targetDirection) {
        float targetAngle = std::atan2(targetDirection.y, targetDirection.x) * (180.0f / PI);
        m_rotation.SetAngle(targetAngle);
    }

    void FacePosition(const tkd::Vector2& targetPosition, const tkd::Vector2& currentPosition) {
        tkd::Vector2 direction = (targetPosition - currentPosition).Normalized();
        FaceDirection(direction);
    }
};
```

### 2D Camera System

```cpp
class Camera2D {
private:
    tkd::Rotator2D m_rotation;
    float m_rotationSpeed;

public:
    void RotateCamera(float input, float deltaTime) {
        float rotationDelta = input * m_rotationSpeed * deltaTime;
        m_rotation.Rotate(rotationDelta);
    }

    void LookAt(const tkd::Vector2& target, const tkd::Vector2& cameraPos) {
        tkd::Vector2 direction = (target - cameraPos).Normalized();
        float angle = std::atan2(direction.y, direction.x) * (180.0f / PI);
        m_rotation.SetAngle(angle);
    }

    tkd::Vector2 GetForwardVector() const {
        float angleRad = m_rotation.GetAngle() * (PI / 180.0f);
        return tkd::Vector2(std::cos(angleRad), std::sin(angleRad));
    }
};
```

### UI Element Rotation

```cpp
class UIElement {
private:
    tkd::Rotator2D m_rotation;
    bool m_isAnimating;
    float m_targetAngle;
    float m_animationSpeed;

public:
    void SetRotation(float angle) {
        m_rotation.SetAngle(angle);
    }

    void AnimateToAngle(float targetAngle, float animationSpeed = 180.0f) {
        m_targetAngle = targetAngle;
        m_animationSpeed = animationSpeed;
        m_isAnimating = true;
    }

    void Update(float deltaTime) {
        if (!m_isAnimating) return;

        float currentAngle = m_rotation.GetAngle();
        float angleDifference = m_targetAngle - currentAngle;

        // Handle angle wrapping for shortest path
        if (angleDifference > 180.0f) angleDifference -= 360.0f;
        if (angleDifference < -180.0f) angleDifference += 360.0f;

        float rotationStep = m_animationSpeed * deltaTime;

        if (std::abs(angleDifference) <= rotationStep) {
            m_rotation.SetAngle(m_targetAngle);
            m_isAnimating = false;
        } else {
            float deltaAngle = std::copysign(rotationStep, angleDifference);
            m_rotation.Rotate(deltaAngle);
        }
    }
};
```

### Input Handling for 2D Games

```cpp
class InputController {
public:
    void HandleMouseLook(const tkd::Vector2& mousePos, const tkd::Vector2& playerPos,
                        tkd::Rotator2D& playerRotation) {
        tkd::Vector2 direction = (mousePos - playerPos).Normalized();
        float targetAngle = std::atan2(direction.y, direction.x) * (180.0f / PI);
        playerRotation.SetAngle(targetAngle);
    }

    void HandleKeyboardRotation(float leftRightInput, tkd::Rotator2D& rotation,
                               float rotationSpeed, float deltaTime) {
        float rotationDelta = leftRightInput * rotationSpeed * deltaTime;
        rotation.Rotate(rotationDelta);
    }
};
```

## Performance Considerations

### Efficient Usage

```cpp
// Good: Reuse rotator objects
tkd::Rotator2D rotationDelta(deltaTime * rotationSpeed);
for (auto& sprite : sprites) {
    sprite.GetRotation().Rotate(rotationDelta);
}

// Good: Use const references
void ProcessRotation(const tkd::Rotator2D& rotation) {
    float angle = rotation.GetAngle();
    // Process angle...
}

// Avoid: Frequent angle conversions in tight loops
for (int i = 0; i < 1000; ++i) {
    float rad = rotators[i].GetAngle() * (PI / 180.0f);  // Repeated conversion
    float cos_val = std::cos(rad);
    float sin_val = std::sin(rad);
}

// Better: Batch conversions or cache results
std::vector<tkd::Vector2> directions;
directions.reserve(rotators.size());
for (const auto& rotator : rotators) {
    float rad = rotator.GetAngle() * (PI / 180.0f);
    directions.emplace_back(std::cos(rad), std::sin(rad));
}
```

### Memory Layout

The 2D rotator stores one value:

-   `T m_angle` - sizeof(T) bytes

Total size: sizeof(T) bytes (4 bytes for float, 8 bytes for double)

This makes TRotator2D very lightweight and cache-friendly for 2D applications.

## Best Practices

### Angle Handling

```cpp
// Good: Use normalized angles for consistency
rotator.SetAngle(angle);  // Automatically normalizes

// Good: Handle shortest rotation path in animations
float AngleDifference(float target, float current) {
    float diff = target - current;
    if (diff > 180.0f) diff -= 360.0f;
    if (diff < -180.0f) diff += 360.0f;
    return diff;
}

// Avoid: Manual angle clamping
float angle = rotator.GetAngle();
if (angle > 180.0f) angle -= 360.0f;  // Already handled internally
```

### Direction Vector Conversion

```cpp
// Good: Create utility functions for common conversions
tkd::Vector2 AngleToDirection(float angleDegrees) {
    float angleRad = angleDegrees * (PI / 180.0f);
    return tkd::Vector2(std::cos(angleRad), std::sin(angleRad));
}

float DirectionToAngle(const tkd::Vector2& direction) {
    return std::atan2(direction.y, direction.x) * (180.0f / PI);
}

// Usage
tkd::Rotator2D rotator(45.0f);
tkd::Vector2 forward = AngleToDirection(rotator.GetAngle());
```

### Smooth Rotation

```cpp
// Good: Implement smooth rotation for better user experience
class SmoothRotator {
private:
    tkd::Rotator2D m_current;
    float m_rotationSpeed;

public:
    void SetTargetAngle(float target, float deltaTime) {
        float current = m_current.GetAngle();
        float difference = AngleDifference(target, current);
        float step = m_rotationSpeed * deltaTime;

        if (std::abs(difference) > step) {
            float delta = std::copysign(step, difference);
            m_current.Rotate(delta);
        } else {
            m_current.SetAngle(target);
        }
    }
};
```

## Type Aliases

```cpp
using Rotator2D = TRotator2D<float>;    // Single precision
using Rotator2Dd = TRotator2D<double>;  // Double precision

// Usage
Rotator2D spriteRotation;       // Most common for 2D games
Rotator2Dd precisionRotation;   // For high-precision calculations
```

## Related Classes

-   **TTransform2D**: Contains TRotator2D for 2D transformations
-   **TRotator**: 3D version with pitch, yaw, and roll
-   **TVector2**: Often used with 2D rotations for direction calculations
-   **TMatrix2D**: Matrix representation of 2D transformations (if available)

The TRotator2D class provides a simple yet powerful way to handle 2D rotations with automatic angle normalization and comprehensive mathematical operations, making it ideal for 2D games, UI systems, and sprite-based applications.
