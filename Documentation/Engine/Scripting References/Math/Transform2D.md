# TTransform2D

## Overview

The `TTransform2D` template class represents a 2D transformation containing position, rotation, and scale components for 2D graphics and game development. It provides both component-based (object-oriented) and static utility methods for 2D transformation operations. The class supports transformation composition and comprehensive mathematical operations while maintaining type safety through template parameters.

## Basic Usage

### Creating 2D Transforms

```cpp
#include <Engine/Core/Math/TTransform2D.hpp>

// Default constructor - creates identity transform
tkd::Transform2D transform;  // Position: (0,0), Rotation: 0°, Scale: (1,1)

// Constructor with specific values
tkd::Transform2D transform(
    tkd::Vector2(100.0f, 200.0f),    // Position
    tkd::Rotator2D(45.0f),           // Rotation (degrees)
    tkd::Vector2(2.0f, 2.0f)         // Scale
);

// Copy constructor
tkd::Transform2D otherTransform = transform;

// Using identity transform
tkd::Transform2D identityTransform = tkd::Transform2D::Identity;
```

### Basic Operations

```cpp
tkd::Transform2D transform;

// Set individual components
transform.SetPosition(tkd::Vector2(50.0f, 100.0f));
transform.SetRotation(tkd::Rotator2D(90.0f));
transform.SetScale(tkd::Vector2(1.5f, 2.0f));

// Set uniform scale
transform.SetScale(2.0f);  // Both components become (2, 2)

// Get components
auto position = transform.GetPosition();
auto rotation = transform.GetRotation();
auto scale = transform.GetScale();

// Print transform information
std::cout << transform << std::endl;
// Output: Transform2D(Position: (50, 100), Rotation: 90, Scale: (1.5, 2))
```

## Component Methods

### Position Operations

#### `SetPosition(const TVector2<T>& position)`

Sets the absolute position of the 2D transform.

```cpp
transform.SetPosition(tkd::Vector2(100.0f, 200.0f));
```

#### `GetPosition() const`

Returns a const reference to the position vector.

```cpp
const auto& pos = transform.GetPosition();
std::cout << "X: " << pos.x << ", Y: " << pos.y << std::endl;
```

#### `Translate(const TVector2<T>& offset)` / `Translate(const T& offset)`

Moves the transform by the specified offset (relative movement).

```cpp
// Move by vector
transform.Translate(tkd::Vector2(10.0f, -5.0f));

// Move uniformly in both directions
transform.Translate(5.0f);  // Adds 5 to both x and y
```

### Rotation Operations

#### `SetRotation(const TRotator2D<T>& rotation)`

Sets the absolute rotation of the transform.

```cpp
transform.SetRotation(tkd::Rotator2D(45.0f));
```

#### `GetRotation() const`

Returns a const reference to the rotation component.

```cpp
const auto& rot = transform.GetRotation();
std::cout << "Angle: " << rot.GetAngle() << " degrees" << std::endl;
```

#### `Rotate(const TRotator2D<T>& rotation)` / `Rotate(const T& angle)`

Applies additional rotation to the current rotation (relative rotation).

```cpp
// Rotate by rotator
transform.Rotate(tkd::Rotator2D(30.0f));

// Rotate by angle directly
transform.Rotate(30.0f);  // Rotate 30 degrees
```

### Scale Operations

#### `SetScale(const TVector2<T>& scale)` / `SetScale(const T& uniformScale)`

Sets the absolute scale of the transform.

```cpp
// Non-uniform scale
transform.SetScale(tkd::Vector2(2.0f, 1.5f));

// Uniform scale
transform.SetScale(2.5f);  // Both components become (2.5, 2.5)
```

#### `GetScale() const`

Returns a const reference to the scale vector.

```cpp
const auto& scale = transform.GetScale();
std::cout << "Scale X: " << scale.x << ", Y: " << scale.y << std::endl;
```

#### `Scale(const TVector2<T>& factors)` / `Scale(const T& factor)`

Multiplies the current scale by the specified factors (relative scaling).

```cpp
// Scale by vector
transform.Scale(tkd::Vector2(1.5f, 0.8f));

// Uniform scale multiplication
transform.Scale(2.0f);  // Doubles both scale components
```

## Static Utility Methods

### Static Transform Operations

All instance methods have corresponding static versions that take a transform reference as the first parameter:

```cpp
tkd::Transform2D myTransform;

// Static scaling
tkd::Transform2D::Scale(myTransform, 2.0f);
tkd::Transform2D::Scale(myTransform, tkd::Vector2(2.0f, 1.5f));

// Static translation
tkd::Transform2D::Translate(myTransform, 10.0f);
tkd::Transform2D::Translate(myTransform, tkd::Vector2(20.0f, -10.0f));

// Static rotation
tkd::Transform2D::Rotate(myTransform, tkd::Rotator2D(45.0f));
tkd::Transform2D::Rotate(myTransform, 45.0f);
```

## Comparison Operators

### Equality Operations

```cpp
tkd::Transform2D transform1;
tkd::Transform2D transform2;

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
tkd::Transform2D floatTransform;  // Same as TTransform2D<float>

// Double precision
tkd::Transform2Dd doubleTransform;  // Same as TTransform2D<double>

// Cross-type construction
tkd::Transform2D floatTransform;
tkd::Transform2Dd doubleTransform(floatTransform);  // Converts float to double
```

### Identity Transform

The static Identity member provides a convenient reference transform:

```cpp
// Reset transform to identity
myTransform = tkd::Transform2D::Identity;

// Check if transform is identity
if (myTransform == tkd::Transform2D::Identity) {
    std::cout << "Transform is at identity" << std::endl;
}
```

### Output and Debugging

```cpp
tkd::Transform2D transform;
transform.SetPosition(tkd::Vector2(10.0f, 20.0f));
transform.SetRotation(tkd::Rotator2D(45.0f));
transform.SetScale(tkd::Vector2(2.0f, 3.0f));

// Print complete transform
std::cout << transform << std::endl;
// Output: Transform2D(Position: (10, 20), Rotation: 45, Scale: (2, 3))

// Print individual components
std::cout << transform.GetRotation() << std::endl;  // Rotator2D(Angle: 45)
std::cout << transform.GetPosition() << std::endl;  // (10, 20)
std::cout << transform.GetScale() << std::endl;     // (2, 3)
```

## Integration with 2D Game Objects

### 2D Entity Component System

```cpp
class GameObject2D {
private:
    tkd::Transform2D m_transform;

public:
    void SetWorldPosition(const tkd::Vector2& position) {
        m_transform.SetPosition(position);
    }

    void MoveBy(const tkd::Vector2& offset) {
        m_transform.Translate(offset);
    }

    void RotateBy(float angle) {
        m_transform.Rotate(angle);
    }

    const tkd::Transform2D& GetTransform() const {
        return m_transform;
    }
};
```

### 2D Sprite System

```cpp
class Sprite {
private:
    tkd::Transform2D m_transform;
    tkd::Vector2 m_pivot;  // Pivot point for rotation/scaling

public:
    void SetPivot(const tkd::Vector2& pivot) {
        m_pivot = pivot;
    }

    void DrawSprite() {
        // Apply transform to sprite rendering
        auto pos = m_transform.GetPosition();
        auto angle = m_transform.GetRotation().GetAngle();
        auto scale = m_transform.GetScale();

        // Render sprite with transformation
        RenderSprite(pos, angle, scale, m_pivot);
    }
};
```

### UI Element Transforms

```cpp
class UIElement {
private:
    tkd::Transform2D m_transform;
    tkd::Vector2 m_anchor;  // Anchor point (0-1 range)

public:
    void SetAnchor(const tkd::Vector2& anchor) {
        m_anchor = anchor;  // (0,0) = top-left, (1,1) = bottom-right
    }

    void UpdateLayout(const tkd::Vector2& parentSize) {
        tkd::Vector2 anchoredPos = m_anchor * parentSize;
        m_transform.SetPosition(anchoredPos);
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

// Good: Cache frequently accessed values
const auto& pos = transform.GetPosition();
for (int i = 0; i < particles.size(); ++i) {
    particles[i].position = pos + offsets[i];
}
```

### Memory Layout

The 2D transform stores three components contiguously:

-   `TVector2<T> m_position` - 2 × sizeof(T) bytes
-   `TRotator2D<T> m_rotation` - 1 × sizeof(T) bytes
-   `TVector2<T> m_scale` - 2 × sizeof(T) bytes

Total size: 5 × sizeof(T) bytes (20 bytes for float, 40 bytes for double)

## Best Practices

### 2D Game Development

```cpp
// Good: Use 2D-specific methods
transform.Rotate(45.0f);  // Simple angle rotation

// Good: Leverage 2D coordinate system
transform.SetPosition(tkd::Vector2(screenWidth * 0.5f, screenHeight * 0.7f));

// Good: Use uniform scaling for sprites
transform.SetScale(2.0f);  // Maintains aspect ratio
```

### Animation and Tweening

```cpp
class Animator2D {
public:
    tkd::Transform2D InterpolateTransforms(
        const tkd::Transform2D& from,
        const tkd::Transform2D& to,
        float t
    ) {
        tkd::Vector2 position = Lerp(from.GetPosition(), to.GetPosition(), t);

        // Handle angle wrapping for rotation
        float fromAngle = from.GetRotation().GetAngle();
        float toAngle = to.GetRotation().GetAngle();
        float angle = LerpAngle(fromAngle, toAngle, t);

        tkd::Vector2 scale = Lerp(from.GetScale(), to.GetScale(), t);

        return tkd::Transform2D(position, tkd::Rotator2D(angle), scale);
    }
};
```

## Common Use Cases

### 2D Camera System

```cpp
class Camera2D {
private:
    tkd::Transform2D m_transform;
    float m_zoom;

public:
    void Follow(const tkd::Vector2& target, float smoothing = 0.1f) {
        tkd::Vector2 currentPos = m_transform.GetPosition();
        tkd::Vector2 newPos = Lerp(currentPos, target, smoothing);
        m_transform.SetPosition(newPos);
    }

    void SetZoom(float zoom) {
        m_zoom = zoom;
        m_transform.SetScale(zoom);
    }

    tkd::Vector2 WorldToScreen(const tkd::Vector2& worldPos) {
        auto cameraPos = m_transform.GetPosition();
        auto scale = m_transform.GetScale();
        return (worldPos - cameraPos) * scale.x;
    }
};
```

### Particle System

```cpp
class Particle2D {
private:
    tkd::Transform2D m_transform;
    tkd::Vector2 m_velocity;
    float m_angularVelocity;

public:
    void Update(float deltaTime) {
        // Update position
        m_transform.Translate(m_velocity * deltaTime);

        // Update rotation
        m_transform.Rotate(m_angularVelocity * deltaTime);

        // Apply physics or other effects...
    }
};
```

### GUI Layout System

```cpp
class LayoutContainer {
private:
    std::vector<tkd::Transform2D> m_childTransforms;
    tkd::Transform2D m_containerTransform;

public:
    void ArrangeHorizontally(float spacing) {
        float currentX = 0;
        for (auto& childTransform : m_childTransforms) {
            childTransform.SetPosition(tkd::Vector2(currentX, 0));
            currentX += childTransform.GetScale().x + spacing;
        }
    }

    void CenterChildren() {
        if (m_childTransforms.empty()) return;

        float totalWidth = GetTotalWidth();
        float startX = -totalWidth * 0.5f;

        for (auto& childTransform : m_childTransforms) {
            auto pos = childTransform.GetPosition();
            childTransform.SetPosition(tkd::Vector2(startX + pos.x, pos.y));
        }
    }
};
```

## Type Aliases

The class provides convenient type aliases for common use cases:

```cpp
using Transform2D = TTransform2D<float>;    // Single precision
using Transform2Dd = TTransform2D<double>;  // Double precision

// Usage
Transform2D spriteTransform;        // Most common for 2D graphics
Transform2Dd precisionTransform;    // For high-precision calculations
```

## Related Classes

-   **TVector2**: Position and scale components for 2D space
-   **TRotator2D**: Single-angle rotation for 2D space
-   **TTransform**: 3D version for 3D games and applications
-   **TMatrix3**: 3×3 transformation matrix for 2D operations

The TTransform2D class provides a comprehensive, efficient, and easy-to-use solution for 2D transformations in game engines, graphics applications, and user interface systems.
