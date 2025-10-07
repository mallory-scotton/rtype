# TQuaternion

## Overview

The `TQuaternion` template class represents 3D rotations using quaternions, a mathematical system that extends complex numbers. Quaternions provide a robust alternative to Euler angles and rotation matrices, offering smooth interpolation, no gimbal lock, and efficient rotation composition. The class stores four components (x, y, z, w) where (x, y, z) represents the vector part and w represents the scalar part.

## Basic Usage

### Creating Quaternions

```cpp
#include <Engine/Core/Math/TQuaternion.hpp>

// Default constructor - identity quaternion (no rotation)
tkd::Quaternion quat;

// Constructor with explicit components (x, y, z, w)
tkd::Quaternion quat(0.0f, 0.0f, 0.0f, 1.0f);  // Identity

// From axis-angle representation
tkd::Vector3 axis(0.0f, 1.0f, 0.0f);  // Y-axis
tkd::Quaternion yawRotation(axis, 90.0f);  // 90 degrees around Y-axis

// From Euler angles (pitch, yaw, roll)
tkd::Quaternion eulerRotation(45.0f, 90.0f, 0.0f);

// Copy constructor
tkd::Quaternion otherQuat = quat;

// Using identity quaternion
tkd::Quaternion identity = tkd::Quaternion::Identity;
```

### Basic Operations

```cpp
tkd::Quaternion quat;

// Set individual components
quat.SetX(0.5f);
quat.SetY(0.0f);
quat.SetZ(0.0f);
quat.SetW(0.866f);

// Get individual components
float x = quat.GetX();
float y = quat.GetY();
float z = quat.GetZ();
float w = quat.GetW();

// Print quaternion information
std::cout << quat << std::endl;
// Output: Quaternion(X: 0.5, Y: 0, Z: 0, W: 0.866)
```

## Quaternion Mathematics

### Component Structure

-   **X, Y, Z**: Vector part representing the axis of rotation scaled by sin(θ/2)
-   **W**: Scalar part representing cos(θ/2) where θ is the rotation angle
-   **Unit Quaternions**: Magnitude = 1, used for representing rotations
-   **Identity**: (0, 0, 0, 1) represents no rotation

### Rotation Representation

```cpp
// A rotation of θ degrees around axis (ax, ay, az) becomes:
// x = ax * sin(θ/2)
// y = ay * sin(θ/2)
// z = az * sin(θ/2)
// w = cos(θ/2)

tkd::Vector3 axis(0, 1, 0);  // Y-axis
float angle = 90.0f;         // 90 degrees
tkd::Quaternion rotation(axis, angle);
```

## Component Methods

### Getters and Setters

#### `GetX() const` / `GetY() const` / `GetZ() const` / `GetW() const`

Returns individual quaternion components.

```cpp
tkd::Quaternion quat(0.5f, 0.0f, 0.5f, 0.707f);

float x = quat.GetX();  // 0.5
float y = quat.GetY();  // 0.0
float z = quat.GetZ();  // 0.5
float w = quat.GetW();  // 0.707
```

#### `SetX(value)` / `SetY(value)` / `SetZ(value)` / `SetW(value)`

Sets individual quaternion components.

```cpp
quat.SetX(0.0f);
quat.SetY(0.707f);
quat.SetZ(0.0f);
quat.SetW(0.707f);  // 90° rotation around Y-axis
```

### Part Access Methods

#### `GetVectorPart() const`

Returns the vector part (x, y, z) as a TVector3.

```cpp
tkd::Quaternion quat(0.5f, 0.0f, 0.5f, 0.707f);
tkd::Vector3 vectorPart = quat.GetVectorPart();  // (0.5, 0.0, 0.5)
```

#### `GetScalarPart() const`

Returns the scalar part (w component).

```cpp
float scalarPart = quat.GetScalarPart();  // 0.707
```

## Quaternion Operations

### Magnitude and Normalization

#### `Magnitude() const`

Calculates the length/magnitude of the quaternion.

```cpp
tkd::Quaternion quat(1.0f, 1.0f, 1.0f, 1.0f);
float magnitude = quat.Magnitude();  // 2.0
```

#### `MagnitudeSquared() const`

Returns the squared magnitude (more efficient than Magnitude()).

```cpp
float magSquared = quat.MagnitudeSquared();  // 4.0 (avoids sqrt calculation)
```

#### `Normalize()`

Normalizes the quaternion to unit length in place.

```cpp
tkd::Quaternion quat(1.0f, 1.0f, 1.0f, 1.0f);
quat.Normalize();  // Now has magnitude 1.0
```

#### `Normalized() const`

Returns a normalized copy without modifying the original.

```cpp
tkd::Quaternion normalized = quat.Normalized();
// Original quat unchanged, normalized is unit length
```

### Quaternion Algebra

#### `Conjugate() const`

Returns the conjugate (negates vector part: x, y, z).

```cpp
tkd::Quaternion quat(0.5f, 0.0f, 0.5f, 0.707f);
tkd::Quaternion conjugate = quat.Conjugate();  // (-0.5, 0.0, -0.5, 0.707)
```

#### `Inverse() const`

Returns the multiplicative inverse of the quaternion.

```cpp
tkd::Quaternion quat(0.5f, 0.0f, 0.5f, 0.707f);
tkd::Quaternion inverse = quat.Inverse();

// For unit quaternions: inverse = conjugate
// For non-unit quaternions: inverse = conjugate / magnitude²
```

## Rotation Creation

### From Axis-Angle

#### `FromAxisAngle(const TVector3<T>& axis, const T& angleDegrees)`

Creates rotation from axis-angle representation.

```cpp
tkd::Quaternion quat;
tkd::Vector3 axis(0.0f, 1.0f, 0.0f);  // Y-axis (up)
quat.FromAxisAngle(axis, 90.0f);       // 90° rotation around Y-axis

// Or use constructor
tkd::Quaternion rotation(axis, 90.0f);
```

### From Euler Angles

#### `FromEulerAngles(const T& pitch, const T& yaw, const T& roll)`

Creates rotation from Euler angles (ZYX order).

```cpp
tkd::Quaternion quat;
quat.FromEulerAngles(45.0f, 90.0f, 30.0f);  // pitch, yaw, roll in degrees

// Or use constructor
tkd::Quaternion rotation(45.0f, 90.0f, 30.0f);
```

### To Euler Angles

#### `ToEulerAngles() const`

Converts quaternion back to Euler angles.

```cpp
tkd::Quaternion quat(45.0f, 90.0f, 30.0f);  // From Euler angles
tkd::Vector3 euler = quat.ToEulerAngles();   // Returns (45, 90, 30)

float pitch = euler.x;  // 45.0
float yaw = euler.y;    // 90.0
float roll = euler.z;   // 30.0
```

## Vector Rotation

### Rotating 3D Vectors

#### `RotateVector(const TVector3<T>& vector) const`

Applies the quaternion rotation to a 3D vector.

```cpp
tkd::Quaternion yaw90(tkd::Vector3(0, 1, 0), 90.0f);  // 90° around Y
tkd::Vector3 forward(0.0f, 0.0f, 1.0f);               // Forward vector
tkd::Vector3 rotated = yaw90.RotateVector(forward);    // Now pointing right (1, 0, 0)

// Rotate multiple vectors
tkd::Vector3 up(0, 1, 0);
tkd::Vector3 right(1, 0, 0);
tkd::Vector3 rotatedUp = yaw90.RotateVector(up);       // Still (0, 1, 0)
tkd::Vector3 rotatedRight = yaw90.RotateVector(right); // Now (-1, 0, 0)
```

## Static Utility Methods

### Spherical Linear Interpolation

#### `Slerp(const TQuaternion& from, const TQuaternion& to, const T& t)`

Performs spherical linear interpolation between two quaternions.

```cpp
tkd::Quaternion start = tkd::Quaternion::Identity;
tkd::Quaternion end(tkd::Vector3(0, 1, 0), 180.0f);   // 180° rotation

// Interpolate halfway (90° rotation)
tkd::Quaternion halfway = tkd::Quaternion::Slerp(start, end, 0.5f);

// Animation example
for (float t = 0.0f; t <= 1.0f; t += 0.1f) {
    tkd::Quaternion current = tkd::Quaternion::Slerp(start, end, t);
    // Use current for smooth animation
}
```

### Linear Interpolation

#### `Lerp(const TQuaternion& from, const TQuaternion& to, const T& t)`

Performs linear interpolation (faster but less smooth than Slerp).

```cpp
tkd::Quaternion result = tkd::Quaternion::Lerp(from, to, 0.5f);

// Lerp is faster but may not maintain constant rotation speed
// Use for performance-critical applications where slight irregularity is acceptable
```

## Mathematical Operators

### Multiplication (Rotation Composition)

#### `operator*(const TQuaternion<T>& lhs, const TQuaternion<T>& rhs)`

Combines two rotations (note: non-commutative).

```cpp
tkd::Quaternion pitch45(tkd::Vector3(1, 0, 0), 45.0f);  // 45° pitch
tkd::Quaternion yaw90(tkd::Vector3(0, 1, 0), 90.0f);    // 90° yaw

// Order matters! Different results:
tkd::Quaternion combined1 = pitch45 * yaw90;  // Pitch first, then yaw
tkd::Quaternion combined2 = yaw90 * pitch45;  // Yaw first, then pitch

// Apply combined rotation
tkd::Vector3 vector(1, 0, 0);
tkd::Vector3 result1 = combined1.RotateVector(vector);
tkd::Vector3 result2 = combined2.RotateVector(vector);  // Different result!
```

### Comparison Operators

#### `operator==(const TQuaternion<T>& lhs, const TQuaternion<T>& rhs)`

Checks exact equality between quaternions.

```cpp
tkd::Quaternion quat1(0.0f, 0.0f, 0.0f, 1.0f);
tkd::Quaternion quat2 = tkd::Quaternion::Identity;

if (quat1 == quat2) {
    std::cout << "Quaternions are identical" << std::endl;
}
```

#### `operator!=(const TQuaternion<T>& lhs, const TQuaternion<T>& rhs)`

Checks inequality between quaternions.

```cpp
if (quat1 != quat2) {
    std::cout << "Quaternions are different" << std::endl;
}
```

## Advanced Usage

### Type Conversion

```cpp
// Float precision (default)
tkd::Quaternion floatQuat;    // Same as TQuaternion<float>

// Double precision
tkd::Quaterniond doubleQuat;  // Same as TQuaternion<double>

// Cross-type usage
tkd::Quaternion floatQuat(0.5f, 0.0f, 0.5f, 0.707f);
// Manual conversion needed for different precision types
```

### Identity Quaternion

```cpp
// Access identity quaternion
tkd::Quaternion identity = tkd::Quaternion::Identity;

// Reset to identity
myQuaternion = tkd::Quaternion::Identity;

// Check if quaternion represents no rotation
bool isIdentity = (myQuaternion == tkd::Quaternion::Identity);
```

### Output and Debugging

```cpp
tkd::Quaternion quat(0.5f, 0.0f, 0.5f, 0.707f);

std::cout << quat << std::endl;
// Output: Quaternion(X: 0.5, Y: 0, Z: 0.5, W: 0.707)

// Debug rotation information
tkd::Vector3 euler = quat.ToEulerAngles();
std::cout << "Euler: " << euler << std::endl;
float magnitude = quat.Magnitude();
std::cout << "Magnitude: " << magnitude << std::endl;
```

## Integration with Game Systems

### Character Controller

```cpp
class Character {
private:
    tkd::Quaternion m_rotation;
    float m_turnSpeed;

public:
    void Turn(float input, float deltaTime) {
        // Create rotation delta around Y-axis
        tkd::Vector3 upAxis(0.0f, 1.0f, 0.0f);
        float angle = input * m_turnSpeed * deltaTime;
        tkd::Quaternion delta(upAxis, angle);

        // Apply rotation
        m_rotation = m_rotation * delta;
        m_rotation.Normalize();  // Maintain unit length
    }

    void LookAt(const tkd::Vector3& target, const tkd::Vector3& currentPos) {
        tkd::Vector3 direction = (target - currentPos).Normalized();

        // Calculate target rotation to face direction
        tkd::Vector3 forward(0, 0, 1);
        if (direction.Dot(forward) < -0.99f) {
            // Handle 180° turn case
            m_rotation = tkd::Quaternion(tkd::Vector3(0, 1, 0), 180.0f);
        } else {
            tkd::Vector3 axis = forward.Cross(direction).Normalized();
            float angle = std::acos(forward.Dot(direction)) * (180.0f / PI);
            m_rotation = tkd::Quaternion(axis, angle);
        }
    }
};
```

### Camera System

```cpp
class Camera {
private:
    tkd::Quaternion m_rotation;
    float m_mouseSensitivity;

public:
    void UpdateRotation(float mouseDeltaX, float mouseDeltaY, float deltaTime) {
        // Create pitch and yaw rotations
        tkd::Quaternion pitchDelta(tkd::Vector3(1, 0, 0), -mouseDeltaY * m_mouseSensitivity);
        tkd::Quaternion yawDelta(tkd::Vector3(0, 1, 0), -mouseDeltaX * m_mouseSensitivity);

        // Apply rotations (yaw first, then pitch to avoid roll)
        m_rotation = yawDelta * m_rotation * pitchDelta;
        m_rotation.Normalize();

        // Clamp pitch to prevent over-rotation
        tkd::Vector3 euler = m_rotation.ToEulerAngles();
        euler.x = std::clamp(euler.x, -89.0f, 89.0f);
        m_rotation.FromEulerAngles(euler.x, euler.y, euler.z);
    }

    tkd::Vector3 GetForward() const {
        return m_rotation.RotateVector(tkd::Vector3(0, 0, 1));
    }

    tkd::Vector3 GetRight() const {
        return m_rotation.RotateVector(tkd::Vector3(1, 0, 0));
    }

    tkd::Vector3 GetUp() const {
        return m_rotation.RotateVector(tkd::Vector3(0, 1, 0));
    }
};
```

### Animation System

```cpp
class RotationAnimator {
private:
    tkd::Quaternion m_startRotation;
    tkd::Quaternion m_targetRotation;
    float m_duration;
    float m_currentTime;

public:
    void StartRotationTo(const tkd::Quaternion& target, float duration) {
        m_startRotation = m_currentRotation;
        m_targetRotation = target;
        m_duration = duration;
        m_currentTime = 0.0f;
    }

    tkd::Quaternion Update(float deltaTime) {
        m_currentTime += deltaTime;
        float t = std::clamp(m_currentTime / m_duration, 0.0f, 1.0f);

        // Use easing function for smooth animation
        float easedT = EaseInOutCubic(t);

        return tkd::Quaternion::Slerp(m_startRotation, m_targetRotation, easedT);
    }

private:
    float EaseInOutCubic(float t) {
        return t < 0.5f ? 4.0f * t * t * t :
               1.0f - std::pow(-2.0f * t + 2.0f, 3.0f) / 2.0f;
    }
};
```

### Physics Integration

```cpp
class RigidBody {
private:
    tkd::Quaternion m_rotation;
    tkd::Vector3 m_angularVelocity;  // radians per second

public:
    void UpdateRotation(float deltaTime) {
        // Convert angular velocity to quaternion rotation
        float angularSpeed = m_angularVelocity.Magnitude();

        if (angularSpeed > 0.0f) {
            tkd::Vector3 axis = m_angularVelocity / angularSpeed;
            float angle = angularSpeed * deltaTime * (180.0f / PI);  // Convert to degrees

            tkd::Quaternion deltaRotation(axis, angle);
            m_rotation = m_rotation * deltaRotation;
            m_rotation.Normalize();
        }
    }

    void ApplyTorque(const tkd::Vector3& torque, float deltaTime) {
        // Simplified: add to angular velocity (real physics would use inertia tensor)
        m_angularVelocity += torque * deltaTime;

        // Apply damping
        m_angularVelocity *= 0.99f;
    }
};
```

## Performance Considerations

### Efficient Usage

```cpp
// Good: Reuse quaternion objects
tkd::Quaternion deltaRotation(tkd::Vector3(0, 1, 0), 1.0f);
for (auto& object : objects) {
    object.rotation = object.rotation * deltaRotation;
    object.rotation.Normalize();  // Periodic normalization
}

// Good: Use const references
void ProcessRotation(const tkd::Quaternion& rotation) {
    tkd::Vector3 forward = rotation.RotateVector(tkd::Vector3(0, 0, 1));
    // Process forward vector...
}

// Avoid: Frequent conversions
for (int i = 0; i < 1000; ++i) {
    tkd::Vector3 euler = quaternions[i].ToEulerAngles();  // Expensive conversion
    // Process euler...
}

// Better: Batch process or avoid conversions
std::vector<tkd::Vector3> eulers;
eulers.reserve(quaternions.size());
for (const auto& quat : quaternions) {
    eulers.push_back(quat.ToEulerAngles());
}
```

### Normalization Management

```cpp
class RotationManager {
private:
    tkd::Quaternion m_rotation;
    int m_operationCount;

public:
    void ApplyRotation(const tkd::Quaternion& delta) {
        m_rotation = m_rotation * delta;

        // Normalize periodically to prevent drift
        if (++m_operationCount >= 10) {
            m_rotation.Normalize();
            m_operationCount = 0;
        }
    }

    void ForceNormalization() {
        m_rotation.Normalize();
        m_operationCount = 0;
    }
};
```

### Memory Layout

The TQuaternion stores one TVector4:

-   `TVector4<T> m_quaternion` - 4 × sizeof(T) bytes

Total size: 4 × sizeof(T) bytes (16 bytes for float, 32 bytes for double)

## Best Practices

### Rotation Composition

```cpp
// Good: Be mindful of multiplication order
tkd::Quaternion parentRotation = GetParentRotation();
tkd::Quaternion localRotation = GetLocalRotation();
tkd::Quaternion worldRotation = parentRotation * localRotation;

// Good: Use meaningful names for rotation order
tkd::Quaternion ApplyLocalThenParent(const tkd::Quaternion& local,
                                    const tkd::Quaternion& parent) {
    return parent * local;  // Clear intention
}
```

### Interpolation Choice

```cpp
// Use Slerp for smooth, constant-speed rotation
tkd::Quaternion smoothRotation = tkd::Quaternion::Slerp(from, to, t);

// Use Lerp for performance-critical applications
tkd::Quaternion fastRotation = tkd::Quaternion::Lerp(from, to, t);

// For very small angles, Lerp ≈ Slerp but faster
float angle = std::acos(std::abs(from.GetW() * to.GetW() +
                                from.GetX() * to.GetX() +
                                from.GetY() * to.GetY() +
                                from.GetZ() * to.GetZ()));
if (angle < 0.1f) {  // Small angle threshold
    return tkd::Quaternion::Lerp(from, to, t);
} else {
    return tkd::Quaternion::Slerp(from, to, t);
}
```

### Gimbal Lock Avoidance

```cpp
// Good: Use quaternions for complex 3D rotations
class ComplexRotator {
    tkd::Quaternion m_rotation;

public:
    void RotateAroundAxis(const tkd::Vector3& axis, float angle) {
        tkd::Quaternion delta(axis, angle);
        m_rotation = m_rotation * delta;
    }

    // No gimbal lock issues with any rotation sequence
};

// Avoid: Converting to Euler angles for intermediate calculations
// (causes gimbal lock and precision loss)
```

## Type Aliases

```cpp
using Quaternion = TQuaternion<float>;    // Single precision
using Quaterniond = TQuaternion<double>;  // Double precision

// Usage
Quaternion cameraRotation;        // Most common
Quaterniond precisionRotation;    // For high-precision calculations
```

## Related Classes

-   **TTransform**: Contains TQuaternion for 3D transformations
-   **TRotator**: Euler angle alternative (simpler but has gimbal lock)
-   **TVector3**: Used for rotation axes and Euler angle representation
-   **TVector4**: Internal storage for quaternion components
-   **TMatrix4**: Can be constructed from quaternions for rendering

The TQuaternion class provides a mathematically robust and efficient way to handle 3D rotations, offering superior interpolation and composition capabilities compared to Euler angles, making it ideal for game engines, animation systems, and 3D applications requiring smooth, reliable rotations.
