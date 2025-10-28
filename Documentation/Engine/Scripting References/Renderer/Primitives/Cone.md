# UConePrimitive - Cone Rendering Primitive

## Overview

The `UConePrimitive` class represents a cone primitive in the TKD Game Engine. A cone is a 3D geometric shape with a circular base that tapers uniformly to a single point (apex). It is essential for creating pointed objects, directional indicators, and natural formations.

Cones are essential for:
- **Markers**: Direction indicators and waypoint markers
- **Projectiles**: Missiles, rockets, and bullets
- **Environment**: Trees, mountains, and natural formations
- **Traffic**: Road cones and safety markers
- **Effects**: Spotlight cones, laser beams, and targeting reticles

## Architecture

### Class Hierarchy
```
UPrimitive (base class)
├── IDrawable
├── FTransformable
└── UConePrimitive
    └── Dynamic geometry with configurable properties
```

### Dependencies
- `UPrimitive` - Base primitive class
- `FVertex` - Vertex structure
- `FVector3` - 3D vector mathematics
- `FColor` - Color representation
- `EPrimitiveType` - Primitive rendering type (TriangleStrip)

### Memory Layout
The `UConePrimitive` contains:
- **Inherited Members**: Vertices, primitive type, color, transform
- **m_strips**: std::vector of vertex strips for rendering
- **m_sectorCount**: Number of sectors around the base
- **m_height**: Height of the cone
- **m_capBase**: Whether the base is capped

## Core Components

### Geometry Structure
- **Side Surface**: Triangular surface from base to apex
- **Base Cap** (optional): Circular bottom face
- **Apex**: Single point at the top

### Vertex Distribution
- Apex at (0, height/2, 0)
- Base circle at y = -height/2
- Radius: 1.0 at base (scale with transform)

## API Reference

### Constructor

```cpp
UConePrimitive::UConePrimitive(
    SizeT sectorCount = 32,
    float height = 2.0f,
    bool capBase = true,
    const FColor& color = FColor::White
);
```

Creates a cone with specified properties.

**Parameters:**
- `sectorCount`: Number of sectors (smoothness) - default: 32
- `height`: Height of the cone - default: 2.0
- `capBase`: Whether to cap the base - default: true
- `color`: Color of the cone - default: White

**Example:**
```cpp
// Standard cone
UConePrimitive* cone1 = new UConePrimitive();

// Smooth cone with custom height
UConePrimitive* cone2 = new UConePrimitive(48, 3.0f);

// Open-base cone (for effects)
UConePrimitive* spotlight = new UConePrimitive(24, 5.0f, false);
```

### Property Accessors

#### SetSectorCount / GetSectorCount
```cpp
void SetSectorCount(SizeT sectorCount);
TKD_NODISCARD SizeT GetSectorCount(void) const;
```

#### SetHeight / GetHeight
```cpp
void SetHeight(float height);
TKD_NODISCARD float GetHeight(void) const;
```

#### SetCapBase / IsCapBase
```cpp
void SetCapBase(bool cap);
TKD_NODISCARD bool IsCapBase(void) const;
```

### Rendering Methods

```cpp
virtual void Draw(IRenderer& renderer, FRenderStates states = FRenderStates()) const override;
virtual void SetColor(const FColor& color) override;
```

## Usage Examples

### Traffic Cone
```cpp
UConePrimitive* trafficCone = new UConePrimitive(16, 1.5f);
trafficCone->SetColor(FColor(255, 140, 0)); // Orange
trafficCone->SetPosition(FVector3(5, 0.75f, 5));
trafficCone->SetScale(FVector3(0.6f, 1.0f, 0.6f));
```

### Pine Tree
```cpp
UConePrimitive* treeTop = new UConePrimitive(12, 8.0f);
treeTop->SetColor(FColor(34, 139, 34)); // Forest green
treeTop->SetPosition(FVector3(10, 4, 10));
treeTop->SetScale(FVector3(2.0f, 1.0f, 2.0f));
```

### Rocket/Missile
```cpp
UConePrimitive* noseCone = new UConePrimitive(24, 2.0f);
noseCone->SetColor(FColor::Red);
noseCone->SetPosition(rocketPosition + FVector3(0, 3, 0));
noseCone->SetScale(FVector3(0.5f, 1.0f, 0.5f));
```

### Mountain Peak
```cpp
UConePrimitive* mountainPeak = new UConePrimitive(8, 50.0f);
mountainPeak->SetColor(FColor(139, 137, 137));
mountainPeak->SetPosition(FVector3(0, 25, 0));
mountainPeak->SetScale(FVector3(20.0f, 1.0f, 20.0f));
```

### Spotlight Cone
```cpp
UConePrimitive* spotlight = new UConePrimitive(32, 10.0f, false);
spotlight->SetColor(FColor(255, 255, 200, 50)); // Transparent yellow
spotlight->SetPosition(lightPosition);
spotlight->SetRotation(FQuaternion::FromAxisAngle(FVector3(1, 0, 0), 180.0f));
spotlight->SetScale(FVector3(3.0f, 1.0f, 3.0f));
```

### Direction Marker
```cpp
UConePrimitive* marker = new UConePrimitive(16, 2.0f);
marker->SetColor(FColor::Yellow);
marker->SetPosition(targetPosition + FVector3(0, 3, 0));
marker->SetRotation(FQuaternion::FromAxisAngle(FVector3(1, 0, 0), 180.0f));
```

### Layered Christmas Tree
```cpp
std::vector<UConePrimitive*> treeLayers;
for (int i = 0; i < 4; ++i)
{
    UConePrimitive* layer = new UConePrimitive(16, 2.0f);
    layer->SetColor(FColor(34, 139, 34));
    layer->SetPosition(FVector3(0, i * 1.5f, 0));
    layer->SetScale(FVector3(3.0f - i * 0.5f, 1.0f, 3.0f - i * 0.5f));
    treeLayers.push_back(layer);
}
```

### Tornado Effect
```cpp
UConePrimitive* tornado = new UConePrimitive(24, 20.0f, false);
tornado->SetColor(FColor(169, 169, 169, 100)); // Translucent gray
tornado->SetPosition(groundPosition);
tornado->SetScale(FVector3(5.0f, 1.0f, 5.0f));

// Animate rotation
void UpdateTornado(float deltaTime)
{
    FQuaternion rot = FQuaternion::FromAxisAngle(FVector3(0, 1, 0), deltaTime * 180.0f);
    tornado->SetRotation(rot * tornado->GetRotation());
}
```

### Targeting Reticle
```cpp
UConePrimitive* reticle = new UConePrimitive(8, 0.5f, false);
reticle->SetColor(FColor::Red);
reticle->SetPosition(targetPosition);
reticle->SetRotation(FQuaternion::FromAxisAngle(FVector3(1, 0, 0), 180.0f));
reticle->SetScale(FVector3(2.0f, 1.0f, 2.0f));
```

### Wizard Hat
```cpp
UConePrimitive* hat = new UConePrimitive(24, 3.0f);
hat->SetColor(FColor(75, 0, 130)); // Indigo
hat->SetPosition(characterHeadPosition + FVector3(0, 1.5f, 0));
hat->SetScale(FVector3(1.2f, 1.0f, 1.2f));
```

## Best Practices

### Sector Count Guidelines
- **Low Detail** (6-12): Stylized objects, distant markers
- **Medium Detail** (16-24): Standard game objects
- **High Detail** (32-48): Close-up objects, smooth cones

### Performance Tips
1. **LOD System**: Reduce sectors for distant cones
2. **Cap Management**: Disable base cap for light cones and effects
3. **Instancing**: Use for multiple similar cones (trees, markers)
4. **Batch Rendering**: Group cones with similar properties

### Visual Quality
- **Sharp Points**: Lower sector count for angular appearance
- **Smooth Cones**: 24+ sectors for smooth, rounded look
- **Inverted Cones**: Rotate 180° around X-axis for funnels
- **Wide Base**: Scale X and Z for broad cones

### Common Use Cases
1. **Direction Indicators**: Waypoints, objective markers
2. **Natural Objects**: Trees, mountains, icicles
3. **Props**: Traffic cones, party hats, ice cream cones
4. **Effects**: Light cones, targeting overlays
5. **Projectiles**: Rocket nose cones, arrow tips

## Advanced Techniques

### Animated Scaling
```cpp
void PulsateCone(UConePrimitive* cone, float time)
{
    float scale = 1.0f + 0.2f * std::sin(time * 3.0f);
    cone->SetScale(FVector3(scale, 1.0f, scale));
}
```

### Stacked Cones
```cpp
// Create a double cone (diamond shape)
UConePrimitive* topCone = new UConePrimitive(24, 2.0f);
topCone->SetPosition(FVector3(0, 1, 0));

UConePrimitive* bottomCone = new UConePrimitive(24, 2.0f);
bottomCone->SetPosition(FVector3(0, -1, 0));
bottomCone->SetRotation(FQuaternion::FromAxisAngle(FVector3(1, 0, 0), 180.0f));
```

### Rotating Beacon
```cpp
void UpdateBeacon(UConePrimitive* beacon, float deltaTime)
{
    // Rotate around Y-axis
    FQuaternion rotation = FQuaternion::FromAxisAngle(
        FVector3(0, 1, 0), deltaTime * 90.0f
    );
    beacon->SetRotation(rotation * beacon->GetRotation());
}
```

## Mathematical Properties

### Surface Area (without base)
```
A = πr√(r² + h²)
where r = radius, h = height
```

### Surface Area (with base)
```
A = πr√(r² + h²) + πr²
```

### Volume
```
V = (1/3)πr²h
```

### Slant Height
```
l = √(r² + h²)
```

## Tips and Tricks

1. **Inverted Cone**: Rotate 180° around X for funnel shapes
2. **Flat Cone**: Small height, large radius for disc shapes
3. **Tall Cone**: Large height, small radius for spires
4. **Open Base**: Disable cap for light cone effects
5. **Compound Shapes**: Combine with cylinders for complex objects

## See Also

- [UPrimitive](../Primitive.md) - Base primitive class
- [UCylinderPrimitive](Cylinder.md) - Cylindrical primitive
- [UPyramidPrimitive](Pyramid.md) - Pyramid with square base
- [UCapsulePrimitive](Capsule.md) - Rounded cylinder
- [FTransformable](../Transformable.md) - 3D transformation component
