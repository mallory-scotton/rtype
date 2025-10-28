# UCylinderPrimitive - Cylinder Rendering Primitive

## Overview

The `UCylinderPrimitive` class represents a cylinder primitive in the TKD Game Engine. A cylinder is a 3D geometric shape with circular top and bottom faces connected by a curved surface. It is one of the most versatile primitives in 3D graphics.

Cylinders are essential for:
- **Architecture**: Pillars, columns, and structural supports
- **Game Objects**: Barrels, pipes, cans, and containers
- **Character Design**: Limbs and body segments
- **Environment**: Trees, logs, and poles
- **Mechanical Objects**: Shafts, rods, and pistons

## Architecture

### Class Hierarchy
```
UPrimitive (base class)
├── IDrawable
├── FTransformable
└── UCylinderPrimitive
    └── Dynamic geometry with configurable properties
```

### Dependencies
- `UPrimitive` - Base primitive class
- `FVertex` - Vertex structure with position, color, and UVs
- `FVector3` - 3D vector mathematics
- `FColor` - Color representation
- `EPrimitiveType` - Primitive rendering type (QuadStrip)

### Memory Layout
The `UCylinderPrimitive` contains:
- **Inherited Members**: Vertices, primitive type, color, transform
- **m_strips**: std::vector of vertex strips for rendering
- **m_sectorCount**: Number of sectors around the cylinder
- **m_height**: Height of the cylinder
- **m_capTop**: Whether the top face is capped
- **m_capBottom**: Whether the bottom face is capped

## Core Components

### Geometry Structure
The cylinder consists of:
- **Side Surface**: Curved surface connecting top and bottom
- **Top Cap** (optional): Circular top face
- **Bottom Cap** (optional): Circular bottom face

### Vertex Distribution
- **Sectors**: Divisions around the circumference (affects smoothness)
- **Height**: Distance between top and bottom faces
- **Radius**: Fixed at 1.0 (scale with transform for different sizes)

## API Reference

### Constructor

```cpp
UCylinderPrimitive::UCylinderPrimitive(
    SizeT sectorCount = 32,
    float height = 2.0f,
    bool capTop = true,
    bool capBottom = true,
    const FColor& color = FColor::White
);
```

Creates a cylinder with specified properties.

**Parameters:**
- `sectorCount`: Number of sectors (smoothness) - default: 32
- `height`: Height of the cylinder - default: 2.0
- `capTop`: Whether to cap the top face - default: true
- `capBottom`: Whether to cap the bottom face - default: true
- `color`: Color of the cylinder - default: White

**Example:**
```cpp
// Create a standard cylinder
UCylinderPrimitive* cylinder1 = new UCylinderPrimitive();

// Create a smooth cylinder with custom height
UCylinderPrimitive* cylinder2 = new UCylinderPrimitive(64, 5.0f);

// Create an open-ended pipe
UCylinderPrimitive* pipe = new UCylinderPrimitive(32, 3.0f, false, false);
```

### Property Accessors

#### SetSectorCount
```cpp
void SetSectorCount(SizeT sectorCount);
```
Sets the number of sectors (smoothness).

**Parameters:**
- `sectorCount`: New sector count

**Example:**
```cpp
cylinder->SetSectorCount(48); // Make smoother
```

#### GetSectorCount
```cpp
TKD_NODISCARD SizeT GetSectorCount(void) const;
```
Returns the current sector count.

**Example:**
```cpp
SizeT sectors = cylinder->GetSectorCount();
```

#### SetHeight
```cpp
void SetHeight(float height);
```
Sets the height of the cylinder.

**Parameters:**
- `height`: New height

**Example:**
```cpp
cylinder->SetHeight(4.0f);
```

#### GetHeight
```cpp
TKD_NODISCARD float GetHeight(void) const;
```
Returns the current height.

**Example:**
```cpp
float height = cylinder->GetHeight();
```

#### SetCapTop
```cpp
void SetCapTop(bool cap);
```
Sets whether the top face is capped.

**Parameters:**
- `cap`: True to cap the top, false otherwise

**Example:**
```cpp
cylinder->SetCapTop(false); // Open top
```

#### IsCapTop
```cpp
TKD_NODISCARD bool IsCapTop(void) const;
```
Checks if the top face is capped.

**Example:**
```cpp
if (cylinder->IsCapTop()) { /* ... */ }
```

#### SetCapBottom
```cpp
void SetCapBottom(bool cap);
```
Sets whether the bottom face is capped.

**Parameters:**
- `cap`: True to cap the bottom, false otherwise

**Example:**
```cpp
cylinder->SetCapBottom(false); // Open bottom
```

#### IsCapBottom
```cpp
TKD_NODISCARD bool IsCapBottom(void) const;
```
Checks if the bottom face is capped.

**Example:**
```cpp
if (cylinder->IsCapBottom()) { /* ... */ }
```

### Rendering

#### Draw
```cpp
virtual void Draw(IRenderer& renderer, FRenderStates states = FRenderStates()) const override;
```
Renders the cylinder using the specified renderer.

#### SetColor
```cpp
virtual void SetColor(const FColor& color) override;
```
Sets the color of all vertices in the cylinder.

**Example:**
```cpp
cylinder->SetColor(FColor::Blue);
```

## Usage Examples

### Basic Cylinder
```cpp
// Create a standard pillar
UCylinderPrimitive* pillar = new UCylinderPrimitive(32, 5.0f);
pillar->SetColor(FColor(139, 69, 19)); // Brown
pillar->SetPosition(FVector3(0, 0, 0));
pillar->SetScale(FVector3(0.5f, 1.0f, 0.5f)); // Thin pillar
```

### Tree Trunk
```cpp
// Create a tree trunk
UCylinderPrimitive* trunk = new UCylinderPrimitive(24, 8.0f);
trunk->SetColor(FColor(101, 67, 33));
trunk->SetPosition(FVector3(10, 0, 10));
trunk->SetScale(FVector3(0.8f, 1.0f, 0.8f));
trunk->SetRotation(FQuaternion::FromAxisAngle(FVector3(0, 0, 1), 5.0f));
```

### Barrel
```cpp
// Create a barrel
UCylinderPrimitive* barrel = new UCylinderPrimitive(32, 2.0f);
barrel->SetColor(FColor(160, 82, 45));
barrel->SetPosition(FVector3(5, 1, 5));
barrel->SetScale(FVector3(1.0f, 1.2f, 1.0f)); // Slightly bulged
```

### Pipe
```cpp
// Create a pipe (open-ended)
UCylinderPrimitive* pipe = new UCylinderPrimitive(32, 10.0f, false, false);
pipe->SetColor(FColor(192, 192, 192)); // Silver
pipe->SetPosition(FVector3(0, 2, 0));
pipe->SetScale(FVector3(0.3f, 1.0f, 0.3f));
pipe->SetRotation(FQuaternion::FromAxisAngle(FVector3(1, 0, 0), 90.0f));
```

### Column
```cpp
// Create an architectural column
UCylinderPrimitive* column = new UCylinderPrimitive(48, 12.0f);
column->SetColor(FColor(245, 245, 245)); // White smoke
column->SetPosition(FVector3(-5, 0, -5));
column->SetScale(FVector3(0.6f, 1.0f, 0.6f));
```

### Can
```cpp
// Create a beverage can
UCylinderPrimitive* can = new UCylinderPrimitive(32, 1.5f);
can->SetColor(FColor::Red);
can->SetPosition(FVector3(2, 0.75f, 2));
can->SetScale(FVector3(0.35f, 0.75f, 0.35f));
```

### Wheel
```cpp
// Create a wheel (short, wide cylinder)
UCylinderPrimitive* wheel = new UCylinderPrimitive(32, 0.3f);
wheel->SetColor(FColor::Black);
wheel->SetPosition(carPosition);
wheel->SetScale(FVector3(1.5f, 1.0f, 1.5f));
wheel->SetRotation(FQuaternion::FromAxisAngle(FVector3(1, 0, 0), 90.0f));
```

### Smokestack
```cpp
// Create a factory smokestack
UCylinderPrimitive* smokestack = new UCylinderPrimitive(24, 15.0f, true, false);
smokestack->SetColor(FColor(70, 70, 70));
smokestack->SetPosition(factoryPosition);
smokestack->SetScale(FVector3(0.8f, 1.0f, 0.8f));
```

### Character Limb
```cpp
// Create a character arm segment
UCylinderPrimitive* arm = new UCylinderPrimitive(16, 2.0f);
arm->SetColor(FColor(255, 220, 177)); // Skin tone
arm->SetPosition(shoulderPosition);
arm->SetScale(FVector3(0.15f, 1.0f, 0.15f));
// Apply bone rotation for animation
```

### Level Design Elements
```cpp
// Create multiple pillars for a temple
for (int i = 0; i < 8; ++i)
{
    float angle = i * (2.0f * M_PI / 8.0f);
    float x = 10.0f * std::cos(angle);
    float z = 10.0f * std::sin(angle);
    
    UCylinderPrimitive* pillar = new UCylinderPrimitive(32, 8.0f);
    pillar->SetColor(FColor(222, 184, 135)); // Burlywood
    pillar->SetPosition(FVector3(x, 0, z));
    pillar->SetScale(FVector3(0.5f, 1.0f, 0.5f));
}
```

### Performance Optimization
```cpp
// Use appropriate sector count based on distance
float distanceToCamera = (cylinder->GetPosition() - cameraPosition).Length();

if (distanceToCamera < 10.0f)
    cylinder->SetSectorCount(48);  // High detail
else if (distanceToCamera < 30.0f)
    cylinder->SetSectorCount(24);  // Medium detail
else
    cylinder->SetSectorCount(12);  // Low detail
```

## Best Practices

### Sector Count Guidelines
- **Low Detail** (8-16 sectors): Distant objects, background elements
- **Medium Detail** (24-32 sectors): Standard game objects
- **High Detail** (48-64 sectors): Close-up objects, important elements
- **Very High Detail** (96+ sectors): Special showcase objects

### Performance Tips
1. **Use Appropriate Detail**: Adjust sector count based on object importance
2. **LOD System**: Decrease sectors for distant cylinders
3. **Cap Management**: Disable caps for pipes and tubes (saves triangles)
4. **Instancing**: Use instanced rendering for many similar cylinders
5. **Reuse Objects**: Transform existing cylinders rather than creating new ones

### Visual Quality
- **Smooth Cylinders**: Use 32+ sectors for smooth appearance
- **Scaling**: Use non-uniform scaling for variety (barrels, columns)
- **Rotation**: Rotate for horizontal pipes and diagonal elements
- **Texturing**: Apply textures for realistic materials
- **Lighting**: Cylinders respond well to directional lighting

### Common Use Cases
1. **Architecture**: Columns, pillars, pipes
2. **Environment**: Trees, logs, posts
3. **Props**: Barrels, cans, containers
4. **Characters**: Limb segments, body parts
5. **Vehicles**: Wheels, axles, pipes
6. **UI**: 3D loading bars, progress indicators

## Advanced Techniques

### Tapered Cylinder
```cpp
// Create a cone-like tapered cylinder using non-uniform scaling
UCylinderPrimitive* tapered = new UCylinderPrimitive(32, 4.0f);
tapered->SetPosition(FVector3(0, 2, 0));
// Apply non-uniform scaling matrix for taper effect
FMatrix4x4 scaleMatrix = FMatrix4x4::CreateScale(FVector3(0.5f, 1.0f, 0.5f));
// Combine with shear or use custom mesh for true taper
```

### Animated Rotation
```cpp
void RotateCylinder(UCylinderPrimitive* cylinder, float deltaTime, float speed)
{
    FQuaternion currentRotation = cylinder->GetRotation();
    FQuaternion deltaRotation = FQuaternion::FromAxisAngle(
        FVector3(0, 1, 0), speed * deltaTime
    );
    cylinder->SetRotation(deltaRotation * currentRotation);
}
```

### Compound Shape - Telescope
```cpp
// Create a telescope using multiple cylinders
UCylinderPrimitive* mainTube = new UCylinderPrimitive(24, 5.0f);
mainTube->SetColor(FColor::Black);
mainTube->SetScale(FVector3(0.5f, 1.0f, 0.5f));

UCylinderPrimitive* eyepiece = new UCylinderPrimitive(16, 1.0f);
eyepiece->SetColor(FColor::Black);
eyepiece->SetPosition(mainTube->GetPosition() - FVector3(0, 3, 0));
eyepiece->SetScale(FVector3(0.3f, 0.5f, 0.3f));
```

### Dynamic Height
```cpp
// Animate cylinder height (extending pipe, growing tree)
void ExtendCylinder(UCylinderPrimitive* cylinder, float deltaTime, float speed)
{
    float currentHeight = cylinder->GetHeight();
    cylinder->SetHeight(currentHeight + speed * deltaTime);
}
```

## Mathematical Properties

### Surface Area (without caps)
```
A = 2πrh
where r = radius (1.0 * scale), h = height
```

### Surface Area (with caps)
```
A = 2πrh + 2πr²
```

### Volume
```
V = πr²h
```

## Tips and Tricks

1. **Horizontal Pipes**: Rotate 90° around X-axis
2. **Vertical Columns**: Use default orientation (Y-up)
3. **Wheels**: Short height, wide radius, rotate 90° around X
4. **Open Tubes**: Disable both caps for pipe interiors
5. **Scaling**: Use transform scale for radius, keep height in cylinder space

## Limitations

- **Fixed Radius**: Base radius is 1.0; use scale transform for different sizes
- **Uniform Caps**: Caps are always circular; no custom cap shapes
- **Straight Sides**: No built-in support for tapered or curved profiles

## See Also

- [UPrimitive](../Primitive.md) - Base primitive class
- [UConePrimitive](Cone.md) - Tapered cylindrical shape
- [UCapsulePrimitive](Capsule.md) - Cylinder with hemispherical ends
- [USpherePrimitive](Sphere.md) - Spherical primitive
- [UCubePrimitive](Cube.md) - Box primitive
- [FTransformable](../Transformable.md) - 3D transformation component
