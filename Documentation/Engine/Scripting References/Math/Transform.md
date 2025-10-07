# TTransform

## Overview

The `TTransform` template class represents a 3D transformation containing position, rotation, and scale components. It provides both component-based (object-oriented) and static utility methods for transformation operations. The class supports transformation composition, interpolation, and comprehensive mathematical operations while maintaining type safety through template parameters.

## Basic Usage

### Creating Transforms

```cpp
#include <Engine/Core/Math/TTransform.hpp>

// Default constructor - creates identity transform
tkd::Transform transform;  // Position: (0,0,0), Rotation: (0,0,0), Scale: (1,1,1)

// Constructor with specific values
tkd::Transform transform(
    tkd::Vector3(10.0f, 20.0f, 30.0f),  // Position
    tkd::Rotator(45.0f, 90.0f, 0.0f),   // Rotation (pitch, yaw, roll)
    tkd::Vector3(2.0f, 2.0f, 2.0f)      // Scale
);

// Copy constructor
tkd::Transform otherTransform = transform;

// Using identity transform
tkd::Transform identityTransform = tkd::Transform::Identity;
```

### Basic Operations

```cpp
tkd::Transform transform;

// Set individual components
transform.SetPosition(tkd::Vector3(10.0f, 5.0f, 0.0f));
transform.SetRotation(tkd::Rotator(45.0f, 90.0f, 0.0f));
transform.SetScale(tkd::Vector3(2.0f, 2.0f, 2.0f));

// Set uniform scale
transform.SetScale(3.0f);  // All components become (3, 3, 3)

// Get components
auto position = transform.GetPosition();
auto rotation = transform.GetRotation();
auto scale = transform.GetScale();

// Print transform information
std::cout << transform << std::endl;
// Output: Transform(Position: (10, 5, 0), Rotation: (45, 90, 0), Scale: (2, 2, 2))
```

## Component Methods

### Position Operations

#### `SetPosition(const TVector3<T>& position)`

Sets the absolute position of the transform.

```cpp
transform.SetPosition(tkd::Vector3(100.0f, 200.0f, 300.0f));
```

#### `GetPosition() const`

Returns a const reference to the position vector.

```cpp
const auto& pos = transform.GetPosition();
std::cout << "X: " << pos.x << ", Y: " << pos.y << ", Z: " << pos.z << std::endl;
```

#### `Translate(const TVector3<T>& offset)` / `Translate(const T& offset)`

Moves the transform by the specified offset (relative movement).

```cpp
// Move by vector
transform.Translate(tkd::Vector3(10.0f, 0.0f, -5.0f));

// Move uniformly in all directions
transform.Translate(2.0f);  // Adds 2 to x, y, and z
```

### Rotation Operations

#### `SetRotation(const TRotator<T>& rotation)`

Sets the absolute rotation of the transform.

```cpp
transform.SetRotation(tkd::Rotator(45.0f, 90.0f, 30.0f));
```

#### `GetRotation() const`

Returns a const reference to the rotation component.

```cpp
const auto& rot = transform.GetRotation();
std::cout << "Pitch: " << rot.GetPitch() << ", Yaw: " << rot.GetYaw()
          << ", Roll: " << rot.GetRoll() << std::endl;
```

#### `Rotate(const TRotator<T>& rotation)` / `Rotate(const T& pitch, const T& yaw, const T& roll)`

Applies additional rotation to the current rotation (relative rotation).

```cpp
// Rotate by rotator
transform.Rotate(tkd::Rotator(15.0f, -30.0f, 45.0f));

// Rotate by individual angles
transform.Rotate(15.0f, -30.0f, 45.0f);
```

### Scale Operations

#### `SetScale(const TVector3<T>& scale)` / `SetScale(const T& uniformScale)`

Sets the absolute scale of the transform.

```cpp
// Non-uniform scale
transform.SetScale(tkd::Vector3(2.0f, 3.0f, 1.0f));

// Uniform scale
transform.SetScale(2.5f);  // All components become (2.5, 2.5, 2.5)
```

#### `GetScale() const`

Returns a const reference to the scale vector.

```cpp
const auto& scale = transform.GetScale();
std::cout << "Scale X: " << scale.x << ", Y: " << scale.y << ", Z: " << scale.z << std::endl;
```

#### `Scale(const TVector3<T>& factors)` / `Scale(const T& factor)`

Multiplies the current scale by the specified factors (relative scaling).

```cpp
// Scale by vector
transform.Scale(tkd::Vector3(1.5f, 2.0f, 0.5f));

// Uniform scale multiplication
transform.Scale(2.0f);  // Doubles all scale components
```

## Static Utility Methods

### Static Transform Operations

All instance methods have corresponding static versions that take a transform reference as the first parameter:

```cpp
tkd::Transform myTransform;

// Static scaling
tkd::Transform::Scale(myTransform, 2.0f);
tkd::Transform::Scale(myTransform, tkd::Vector3(2.0f, 3.0f, 4.0f));

// Static translation
tkd::Transform::Translate(myTransform, 5.0f);
tkd::Transform::Translate(myTransform, tkd::Vector3(10.0f, 0.0f, -5.0f));

// Static rotation
tkd::Transform::Rotate(myTransform, tkd::Rotator(45.0f, 90.0f, 0.0f));
tkd::Transform::Rotate(myTransform, 45.0f, 90.0f, 0.0f);
```

## Transform Composition

### Matrix Multiplication Operator

#### `operator*(const TTransform<T>& lhs, const TTransform<T>& rhs)`

Combines two transforms using matrix multiplication rules (applies `rhs` first, then `lhs`).

```cpp
tkd::Transform parentTransform;
parentTransform.SetPosition(tkd::Vector3(100.0f, 0.0f, 0.0f));
parentTransform.SetRotation(tkd::Rotator(0.0f, 45.0f, 0.0f));

tkd::Transform childTransform;
childTransform.SetPosition(tkd::Vector3(10.0f, 0.0f, 0.0f));

// Combine transforms
tkd::Transform worldTransform = parentTransform * childTransform;

// In-place combination
parentTransform *= childTransform;
```

## Comparison Operators

### Equality Operations

```cpp
tkd::Transform transform1;
tkd::Transform transform2;

// Exact comparison
if (transform1 == transform2) {
    std::cout << "Transforms are identical" << std::endl;
}

// Inequality
if (transform1 != transform2) {
    std::cout << "Transforms are different" << std::endl;
}
```

## Advanced Usage

### Type Conversion

The template supports different numeric types:

```cpp
// Float precision (default)
tkd::Transform floatTransform;  // Same as TTransform<float>

// Double precision
tkd::Transformd doubleTransform;  // Same as TTransform<double>

// Cross-type construction
tkd::Transform floatTransform;
tkd::Transformd doubleTransform(floatTransform);  // Converts float to double
```

### Identity Transform

The static Identity member provides a convenient reference transform:

```cpp
// Reset transform to identity
myTransform = tkd::Transform::Identity;

// Check if transform is identity
if (myTransform == tkd::Transform::Identity) {
    std::cout << "Transform is at identity" << std::endl;
}
```

### Output and Debugging

```cpp
tkd::Transform transform;
transform.SetPosition(tkd::Vector3(10.0f, 20.0f, 30.0f));
transform.SetRotation(tkd::Rotator(45.0f, 90.0f, 0.0f));
transform.SetScale(tkd::Vector3(2.0f, 3.0f, 4.0f));

// Print complete transform
std::cout << transform << std::endl;
// Output: Transform(Position: (10, 20, 30), Rotation: (45, 90, 0), Scale: (2, 3, 4))

// Print individual components
std::cout << transform.GetRotation() << std::endl;  // Rotator(Pitch: 45, Yaw: 90, Roll: 0)
std::cout << transform.GetPosition() << std::endl;  // (10, 20, 30)
std::cout << transform.GetScale() << std::endl;     // (2, 3, 4)
```

## Integration with Game Objects

### Entity Component System

```cpp
class GameObject {
private:
    tkd::Transform m_transform;

public:
    void SetWorldPosition(const tkd::Vector3& position) {
        m_transform.SetPosition(position);
    }

    void MoveBy(const tkd::Vector3& offset) {
        m_transform.Translate(offset);
    }

    void RotateBy(float pitch, float yaw, float roll) {
        m_transform.Rotate(pitch, yaw, roll);
    }

    const tkd::Transform& GetTransform() const {
        return m_transform;
    }
};
```

### Hierarchical Transforms

```cpp
class SceneNode {
private:
    tkd::Transform m_localTransform;
    tkd::Transform m_worldTransform;
    SceneNode* m_parent;

public:
    void UpdateWorldTransform() {
        if (m_parent) {
            m_worldTransform = m_parent->GetWorldTransform() * m_localTransform;
        } else {
            m_worldTransform = m_localTransform;
        }
    }

    void SetLocalPosition(const tkd::Vector3& position) {
        m_localTransform.SetPosition(position);
        UpdateWorldTransform();
    }
};
```

## Performance Considerations

### Efficient Usage Patterns

```cpp
// Good: Batch operations
transform.SetPosition(newPosition);
transform.SetRotation(newRotation);
transform.SetScale(newScale);

// Good: Use references to avoid copies
const auto& position = transform.GetPosition();
const auto& rotation = transform.GetRotation();

// Avoid: Frequent getter calls in loops
for (int i = 0; i < 1000; ++i) {
    // Bad: Gets position every iteration
    if (transform.GetPosition().x > someValue) { /* ... */ }
}

// Better: Cache the reference
const auto& pos = transform.GetPosition();
for (int i = 0; i < 1000; ++i) {
    if (pos.x > someValue) { /* ... */ }
}
```

### Memory Layout

The transform stores three components contiguously:

-   `TVector3<T> m_position` - 3 × sizeof(T) bytes
-   `TRotator<T> m_rotation` - 3 × sizeof(T) bytes
-   `TVector3<T> m_scale` - 3 × sizeof(T) bytes

Total size: 9 × sizeof(T) bytes (36 bytes for float, 72 bytes for double)

## Best Practices

### Initialization

```cpp
// Good: Use constructor for initialization
tkd::Transform transform(position, rotation, scale);

// Good: Use Identity for reset
transform = tkd::Transform::Identity;

// Avoid: Unnecessary default construction
tkd::Transform transform;
transform.SetPosition(position);
transform.SetRotation(rotation);
transform.SetScale(scale);
```

### Naming Conventions

```cpp
// Good: Clear variable names
tkd::Transform playerTransform;
tkd::Transform cameraTransform;
tkd::Transform worldTransform;

// Avoid: Generic names
tkd::Transform t1, t2, transform;
```

### Component Access

```cpp
// Good: Use const references when not modifying
void ProcessTransform(const tkd::Transform& transform) {
    const auto& pos = transform.GetPosition();
    const auto& rot = transform.GetRotation();
    // Process without copying...
}

// Good: Modify components through transform methods
transform.Translate(offset);  // Instead of: pos = transform.GetPosition(); pos += offset; transform.SetPosition(pos);
```

## Common Use Cases

### Camera Systems

```cpp
class Camera {
private:
    tkd::Transform m_transform;

public:
    void LookAt(const tkd::Vector3& target, const tkd::Vector3& up) {
        // Calculate rotation to look at target
        // ... rotation calculation logic ...
        m_transform.SetRotation(calculatedRotation);
    }

    void MoveForward(float distance) {
        tkd::Vector3 forward = m_transform.GetRotation().GetForwardVector();
        m_transform.Translate(forward * distance);
    }
};
```

### Animation Systems

```cpp
class AnimationSystem {
public:
    tkd::Transform InterpolateTransforms(
        const tkd::Transform& from,
        const tkd::Transform& to,
        float t
    ) {
        tkd::Vector3 position = Lerp(from.GetPosition(), to.GetPosition(), t);
        tkd::Rotator rotation = Slerp(from.GetRotation(), to.GetRotation(), t);
        tkd::Vector3 scale = Lerp(from.GetScale(), to.GetScale(), t);

        return tkd::Transform(position, rotation, scale);
    }
};
```

## Type Aliases

The class provides convenient type aliases for common use cases:

```cpp
using Transform = TTransform<float>;    // Single precision
using Transformd = TTransform<double>;  // Double precision

// Usage
Transform gameObjectTransform;      // Most common
Transformd precisionTransform;      // For high-precision calculations
```

## Related Classes

-   **TVector3**: Position and scale components
-   **TRotator**: Rotation component with Euler angles
-   **TTransform2D**: 2D version for 2D games
-   **TMatrix4**: 4×4 transformation matrix representation

The TTransform class provides a comprehensive, type-safe, and efficient way to handle 3D transformations in game engines and graphics applications.
