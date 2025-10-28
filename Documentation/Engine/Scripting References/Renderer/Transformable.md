# Transformable System Documentation

## Overview

The TKD Engine `FTransformable` class provides a comprehensive transformation system for 3D objects in the game engine. It serves as the foundation for positioning, orienting, and scaling all renderable entities, implementing a hierarchical transformation model with lazy evaluation for optimal performance.

### Key Features

- **Complete 3D Transformations**: Position, rotation, and scale in 3D space
- **Origin Point Support**: Customizable transformation origin for pivot-based rotations
- **Euler Angle Rotations**: Pitch, yaw, and roll with ZYX rotation order
- **Lazy Matrix Evaluation**: Cached transformation matrices updated only when needed
- **Relative Transformations**: Move, rotate, and scale operations relative to current state
- **Matrix Access**: Direct access to forward and inverse transformation matrices
- **Performance Optimized**: Minimal recomputation through dirty flag system

### Architecture

The `FTransformable` class implements a component-based transformation system that integrates seamlessly with the engine's rendering pipeline:

```
┌─────────────────┐
│  FTransformable │ ← Base transformation class
│                 │
│ • Position      │
│ • Rotation      │
│ • Scale         │
│ • Origin        │
│ • Transform     │
└─────────────────┘
         ▲
         │
┌─────────────────┐    ┌─────────────────┐
│   UPrimitive    │    │   UObject       │ ← Inherits from FTransformable
│                 │    │                 │
│ • Vertex data   │    │ • Properties    │
│ • Materials     │    │ • Functions     │
│ • Rendering     │    │ • Networking    │
└─────────────────┘    └─────────────────┘
```

## Class Declaration

```cpp
namespace tkd
{
class FTransformable
{
private:
    FVector3 m_origin;                    // Origin point for transformations
    FVector3 m_position;                  // Position in world space
    FRotator m_rotation;                  // Rotation (Euler angles)
    FVector3 m_scale;                     // Scale factors
    mutable FTransform m_transform;       // Cached combined transform matrix
    mutable bool m_transformNeedUpdate;  // Dirty flag for transform
    mutable FTransform m_inverseTransform;        // Cached inverse matrix
    mutable bool m_inverseTransformNeedUpdate;   // Dirty flag for inverse

public:
    FTransformable(void);
    virtual ~FTransformable() = default;

    // Position setters
    void SetPosition(float x, float y, float z);
    void SetPosition(const FVector3& position);

    // Rotation setters
    void SetRotation(float roll, float pitch, float yaw);
    void SetRotation(const FRotator& rotation);

    // Scale setters
    void SetScale(float x, float y, float z);
    void SetScale(const FVector3& scale);

    // Origin setters
    void SetOrigin(float x, float y, float z);
    void SetOrigin(const FVector3& origin);

    // Relative transformations
    void Move(float x, float y, float z);
    void Move(const FVector3& offset);
    void Rotate(float roll, float pitch, float yaw);
    void Rotate(const FRotator& rotation);
    void Scale(float x, float y, float z);
    void Scale(const FVector3& scale);

    // Matrix access
    const FTransform& GetTransform(void) const;
    const FTransform& GetInverseTransform(void) const;

    // Property getters
    const FVector3& GetPosition(void) const;
    const FRotator& GetRotation(void) const;
    const FVector3& GetScale(void) const;
    const FVector3& GetOrigin(void) const;
};
}
```

## Member Variables

### Transformation Properties

- **`FVector3 m_origin`**
  - Defines the pivot point for rotations and scaling
  - Default: `FVector3::Zero` (origin at object center)
  - Affects how transformations are applied relative to the object

- **`FVector3 m_position`**
  - World-space position of the object
  - Default: `FVector3::Zero`
  - Translation component of the transformation

- **`FRotator m_rotation`**
  - Orientation using Euler angles (pitch, yaw, roll)
  - Default: `FRotator::Identity` (no rotation)
  - Applied in ZYX order: yaw → pitch → roll

- **`FVector3 m_scale`**
  - Scaling factors along each axis
  - Default: `FVector3::One` (no scaling)
  - Non-uniform scaling supported

### Cached Matrices

- **`FTransform m_transform`**
  - Combined transformation matrix (position × rotation × scale)
  - Cached to avoid recomputation
  - Row-major 4×4 matrix representation

- **`FTransform m_inverseTransform`**
  - Inverse of the combined transformation matrix
  - Cached for performance in collision detection and picking
  - Computed on-demand

### Dirty Flags

- **`bool m_transformNeedUpdate`**
  - Indicates if the forward transform matrix needs recomputation
  - Set to `true` when position, rotation, scale, or origin changes

- **`bool m_inverseTransformNeedUpdate`**
  - Indicates if the inverse transform matrix needs recomputation
  - Set to `true` when the forward transform is updated

## Public Methods

### Position Management

#### `void SetPosition(float x, float y, float z)`
Sets the absolute position of the transformable object.

**Parameters:**
- `x`: X-coordinate in world space
- `y`: Y-coordinate in world space
- `z`: Z-coordinate in world space

**Example:**
```cpp
transformable.SetPosition(10.0f, 5.0f, -3.0f);
```

#### `void SetPosition(const FVector3& position)`
Sets the absolute position using a vector.

**Parameters:**
- `position`: New position vector

**Example:**
```cpp
FVector3 newPos(10.0f, 5.0f, -3.0f);
transformable.SetPosition(newPos);
```

### Rotation Management

#### `void SetRotation(float roll, float pitch, float yaw)`
Sets the absolute rotation using Euler angles in degrees.

**Parameters:**
- `roll`: Rotation around Z-axis (degrees)
- `pitch`: Rotation around Y-axis (degrees)
- `yaw`: Rotation around X-axis (degrees)

**Note:** Rotation order is ZYX (yaw → pitch → roll)

**Example:**
```cpp
// Rotate 45° around Y-axis (yaw)
transformable.SetRotation(0.0f, 45.0f, 0.0f);
```

#### `void SetRotation(const FRotator& rotation)`
Sets the absolute rotation using a rotator object.

**Parameters:**
- `rotation`: New rotation angles

**Example:**
```cpp
FRotator rotation(0.0f, 45.0f, 30.0f); // roll, pitch, yaw
transformable.SetRotation(rotation);
```

### Scale Management

#### `void SetScale(float x, float y, float z)`
Sets the absolute scale factors.

**Parameters:**
- `x`: Scale factor along X-axis
- `y`: Scale factor along Y-axis
- `z`: Scale factor along Z-axis

**Example:**
```cpp
// Double size in X, half size in Y, normal in Z
transformable.SetScale(2.0f, 0.5f, 1.0f);
```

#### `void SetScale(const FVector3& scale)`
Sets the absolute scale using a vector.

**Parameters:**
- `scale`: New scale factors

**Example:**
```cpp
FVector3 scale(2.0f, 0.5f, 1.0f);
transformable.SetScale(scale);
```

### Origin Management

#### `void SetOrigin(float x, float y, float z)`
Sets the transformation origin point.

**Parameters:**
- `x`: X-coordinate of origin relative to object
- `y`: Y-coordinate of origin relative to object
- `z`: Z-coordinate of origin relative to object

**Example:**
```cpp
// Set origin to bottom-center of object
transformable.SetOrigin(0.0f, -1.0f, 0.0f);
```

#### `void SetOrigin(const FVector3& origin)`
Sets the transformation origin using a vector.

**Parameters:**
- `origin`: New origin point

**Example:**
```cpp
FVector3 origin(0.0f, -1.0f, 0.0f); // Bottom-center
transformable.SetOrigin(origin);
```

### Relative Transformations

#### `void Move(float x, float y, float z)`
Moves the object relative to its current position.

**Parameters:**
- `x`: Distance to move along X-axis
- `y`: Distance to move along Y-axis
- `z`: Distance to move along Z-axis

**Example:**
```cpp
// Move 5 units forward
transformable.Move(0.0f, 0.0f, 5.0f);
```

#### `void Move(const FVector3& offset)`
Moves the object using a vector offset.

**Parameters:**
- `offset`: Movement vector

**Example:**
```cpp
FVector3 forward(0.0f, 0.0f, 5.0f);
transformable.Move(forward);
```

#### `void Rotate(float roll, float pitch, float yaw)`
Rotates the object relative to its current orientation.

**Parameters:**
- `roll`: Additional roll rotation (degrees)
- `pitch`: Additional pitch rotation (degrees)
- `yaw`: Additional yaw rotation (degrees)

**Example:**
```cpp
// Rotate 90° around Y-axis
transformable.Rotate(0.0f, 90.0f, 0.0f);
```

#### `void Rotate(const FRotator& rotation)`
Rotates the object using a rotator.

**Parameters:**
- `rotation`: Additional rotation to apply

**Example:**
```cpp
FRotator turn(0.0f, 90.0f, 0.0f);
transformable.Rotate(turn);
```

#### `void Scale(float x, float y, float z)`
Scales the object relative to its current scale.

**Parameters:**
- `x`: Additional scale factor for X-axis
- `y`: Additional scale factor for Y-axis
- `z`: Additional scale factor for Z-axis

**Example:**
```cpp
// Grow object by 10%
transformable.Scale(1.1f, 1.1f, 1.1f);
```

#### `void Scale(const FVector3& scale)`
Scales the object using a vector.

**Parameters:**
- `scale`: Additional scale factors

**Example:**
```cpp
FVector3 growth(1.1f, 1.1f, 1.1f);
transformable.Scale(growth);
```

### Matrix Access

#### `const FTransform& GetTransform(void) const`
Returns the combined transformation matrix.

**Returns:** Reference to the 4×4 transformation matrix

**Note:** Matrix is computed on-demand and cached

**Example:**
```cpp
const FTransform& worldMatrix = transformable.GetTransform();
// Use matrix for rendering or physics
renderer->SetTransform(worldMatrix);
```

#### `const FTransform& GetInverseTransform(void) const`
Returns the inverse of the combined transformation matrix.

**Returns:** Reference to the inverse 4×4 transformation matrix

**Note:** Useful for collision detection, picking, and coordinate conversion

**Example:**
```cpp
const FTransform& invMatrix = transformable.GetInverseTransform();
// Convert world point to local space
FVector3 localPoint = invMatrix.TransformPoint(worldPoint);
```

### Property Getters

#### `const FVector3& GetPosition(void) const`
Returns the current position.

**Returns:** Current position vector

#### `const FRotator& GetRotation(void) const`
Returns the current rotation.

**Returns:** Current rotation angles

#### `const FVector3& GetScale(void) const`
Returns the current scale.

**Returns:** Current scale factors

#### `const FVector3& GetOrigin(void) const`
Returns the current origin.

**Returns:** Current origin point

## Mathematical Background

### Transformation Order

Transformations are applied in the following order:
1. **Scale** (relative to origin)
2. **Rotation** (around origin, ZYX Euler angles)
3. **Translation** (position)

The combined transformation matrix is computed as:
```
M = Translation × Rotation × Scale
```

### Euler Angle Convention

The `FRotator` uses Euler angles with the following convention:
- **Yaw (Y)**: Rotation around Y-axis (left/right turning)
- **Pitch (X)**: Rotation around X-axis (up/down tilting)
- **Roll (Z)**: Rotation around Z-axis (rolling)

Rotation order: **ZYX** (Yaw → Pitch → Roll)

### Matrix Construction

The transformation matrix is constructed using the following algorithm:

```cpp
// Convert Euler angles to radians
float pitchRad = pitch * π/180;
float yawRad = yaw * π/180;
float rollRad = roll * π/180;

// Compute trigonometric values
float cp = cos(pitchRad), sp = sin(pitchRad);
float cy = cos(yawRad), sy = sin(yawRad);
float cr = cos(rollRad), sr = sin(rollRad);

// Build rotation matrix (ZYX order)
float m00 = cy * cp;
float m01 = cy * sp * sr - sy * cr;
float m02 = cy * sp * cr + sy * sr;

float m10 = sy * cp;
float m11 = sy * sp * sr + cy * cr;
float m12 = sy * sp * cr - cy * sr;

float m20 = -sp;
float m21 = cp * sr;
float m22 = cp * cr;

// Apply scaling
m00 *= scale.x; m01 *= scale.y; m02 *= scale.z;
m10 *= scale.x; m11 *= scale.y; m12 *= scale.z;
m20 *= scale.x; m21 *= scale.y; m22 *= scale.z;

// Apply translation (accounting for origin)
tx = -origin.x * m00 - origin.y * m01 - origin.z * m02 + position.x;
ty = -origin.x * m10 - origin.y * m11 - origin.z * m12 + position.y;
tz = -origin.x * m20 - origin.y * m21 - origin.z * m22 + position.z;
```

## Usage Examples

### Basic Object Positioning

```cpp
// Create a transformable object
FTransformable object;

// Set initial position
object.SetPosition(100.0f, 50.0f, 0.0f);

// Set rotation (face north-east, slight tilt)
object.SetRotation(0.0f, 45.0f, 15.0f);

// Scale to double size
object.SetScale(2.0f, 2.0f, 2.0f);

// Set origin to bottom of object for ground-based rotation
object.SetOrigin(0.0f, -1.0f, 0.0f);
```

### Animation Loop

```cpp
// In game loop
void Update(float deltaTime)
{
    // Rotate object around Y-axis
    transformable.Rotate(0.0f, 90.0f * deltaTime, 0.0f);

    // Move forward
    FVector3 forward = transformable.GetTransform().GetForwardVector();
    transformable.Move(forward * speed * deltaTime);

    // Get final transform for rendering
    const FTransform& worldTransform = transformable.GetTransform();
    renderer->Draw(mesh, worldTransform);
}
```

### Hierarchical Transformations

```cpp
// Parent object (car body)
FTransformable carBody;
carBody.SetPosition(0.0f, 0.0f, 0.0f);

// Child object (wheel)
FTransformable wheel;
wheel.SetPosition(2.0f, -1.0f, 1.5f); // Local position relative to car

// Compute world transform
FTransform wheelWorldTransform = carBody.GetTransform() * wheel.GetTransform();
```

### Coordinate System Conversion

```cpp
// Convert world point to object local space
FVector3 worldPoint(10.0f, 5.0f, 0.0f);
FVector3 localPoint = transformable.GetInverseTransform().TransformPoint(worldPoint);

// Convert local point to world space
FVector3 worldPoint2 = transformable.GetTransform().TransformPoint(localPoint);
```

### Camera Setup

```cpp
FCamera camera;
camera.position = transformable.GetPosition();
camera.yaw = transformable.GetRotation().GetYaw();
camera.pitch = transformable.GetRotation().GetPitch();
camera.UpdateCameraVectors();
```

## Performance Considerations

### Lazy Evaluation

The `FTransformable` class uses lazy evaluation to minimize computational overhead:

- **Dirty Flags**: Transform matrices are only recomputed when properties change
- **Caching**: Both forward and inverse matrices are cached
- **Minimal Updates**: Only modified components trigger recomputation

### Best Practices

1. **Batch Updates**: Group multiple property changes together
2. **Cache Matrices**: Store transform references locally when used multiple times
3. **Minimize Queries**: Avoid frequent `GetTransform()` calls in tight loops

```cpp
// Good: Cache transform reference
const FTransform& transform = object.GetTransform();
for (int i = 0; i < 1000; ++i) {
    renderer->Draw(mesh, transform);
}

// Bad: Query transform repeatedly
for (int i = 0; i < 1000; ++i) {
    renderer->Draw(mesh, object.GetTransform()); // Recomputes every time
}
```

### Memory Layout

- **Compact Storage**: 4×4 matrix (64 bytes) + vectors and flags
- **Cache Alignment**: Matrices are aligned for optimal SIMD operations
- **Minimal Overhead**: Only essential data stored

## Integration with Engine Systems

### Rendering Pipeline

`FTransformable` integrates directly with the rendering system:

```cpp
// In UPrimitive::Draw()
void UPrimitive::Draw(IRenderer& renderer, FRenderStates states)
{
    // Apply local transformation
    FTransform worldTransform = GetWorldTransform() * m_localTransform.GetTransform();
    renderer.Draw(vertices, vertexCount, primitiveType, worldTransform);
}
```

### Physics System

Used for collision detection and rigid body transformations:

```cpp
// Convert between coordinate systems
FVector3 physicsToWorld(const FVector3& physicsPos) {
    return transformable.GetTransform().TransformPoint(physicsPos);
}

FVector3 worldToPhysics(const FVector3& worldPos) {
    return transformable.GetInverseTransform().TransformPoint(worldPos);
}
```

### Networking

Transform state is synchronized across network:

```cpp
// Serialize transform state
void Serialize(FArchive& archive)
{
    archive << m_position;
    archive << m_rotation;
    archive << m_scale;
    archive << m_origin;
}
```

## Architecture Diagrams

### Transformation Pipeline

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Properties    │───▶│  Matrix Build   │───▶│   Rendering     │
│                 │    │                 │    │                 │
│ • Position      │    │ • Scale Matrix  │    │ • GPU Upload    │
│ • Rotation      │    │ • Rotation Mat  │    │ • Vertex Trans  │
│ • Scale         │    │ • Translation   │    │ • Draw Call     │
│ • Origin        │    │ • Combine       │    │                 │
└─────────────────┘    └─────────────────┘    └─────────────────┘
```

### Class Hierarchy

```
FTransformable
├── UObject (base class for all objects)
│   ├── UActor (scene objects)
│   │   ├── UCamera (viewpoints)
│   │   ├── ULight (lighting)
│   │   └── UPrimitive (renderables)
│   │       ├── USpherePrimitive
│   │       ├── UBoxPrimitive
│   │       └── UPlanePrimitive
│   └── UComponent (attached objects)
│       ├── USpriteComponent
│       ├── UCollisionComponent
│       └── UAudioComponent
└── FTransformable2D (2D variant)
    └── UIElement (UI components)
```

### Matrix Computation Flow

```
Property Change
       │
       ▼
   Set Dirty Flag
       │
       ▼
   GetTransform() Called
       │
       ▼
   Recompute Matrix?
   ┌───Yes────┴───No──┐
   │                  │
   ▼                  ▼
Compute Matrix    Return Cached
   │                  │
   ▼                  ▼
Return Result    Return Result
```

## Error Handling

The `FTransformable` class includes built-in safeguards:

- **Scale Validation**: Prevents zero scale values that could cause division by zero
- **Matrix Decomposition**: Robust algorithm for extracting properties from matrices
- **Gimbal Lock Handling**: Proper handling of singularities in Euler angle conversion

## Threading Considerations

- **Not Thread-Safe**: Property modifications should be synchronized
- **Read-Only Access**: Multiple threads can safely read transform matrices
- **Cache Invalidation**: Dirty flags must be properly synchronized

## Future Extensions

The transformable system supports future enhancements:

- **Quaternion Rotation**: Planned upgrade to quaternion-based rotation
- **Animation Blending**: Support for skeletal animation transforms
- **Transform Constraints**: Parent-child relationship constraints
- **Interpolation**: Smooth transform interpolation for networking

---

*This documentation covers the complete TKD Engine FTransformable system as of the current version. For the latest updates and additional examples, refer to the engine's source code and test suites.*
