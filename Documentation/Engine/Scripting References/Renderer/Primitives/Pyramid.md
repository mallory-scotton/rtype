# UPyramidPrimitive - Pyramid Rendering Primitive

## Overview

The `UPyramidPrimitive` class represents a pyramid primitive with a square base and four triangular faces meeting at an apex. Perfect for ancient structures, markers, and geometric designs.

Pyramids are essential for:
- **Ancient Structures**: Egyptian pyramids, Mayan temples
- **Markers**: Waypoints, objective indicators, and beacons
- **Buildings**: Roofs, caps, and architectural elements
- **Collectibles**: Power-ups, gems, and special items
- **Geometric Art**: Abstract designs and patterns

## Architecture

### Class Hierarchy
```
UPrimitive → IDrawable, FTransformable → UPyramidPrimitive
```

### Key Components
- **m_height**: Height of the pyramid
- **m_capBase**: Whether the base is capped
- Base: Square with side length 1.0 (centered at origin)
- Apex: Point at (0, height/2, 0)

## API Reference

### Constructor
```cpp
UPyramidPrimitive(
    float height = 2.0f,
    bool capBase = true,
    const FColor& color = FColor::White
);
```

**Parameters:**
- `height`: Height of the pyramid (default: 2.0)
- `capBase`: Whether to include base (default: true)
- `color`: Pyramid color

### Methods
```cpp
void SetHeight(float height);
float GetHeight(void) const;
void SetCapBase(bool cap);
bool IsCapBase(void) const;
```

## Usage Examples

### Egyptian Pyramid
```cpp
UPyramidPrimitive* pyramid = new UPyramidPrimitive(10.0f, true);
pyramid->SetColor(FColor(237, 201, 175)); // Sandy color
pyramid->SetPosition(FVector3(0, 0, 0));
pyramid->SetScale(FVector3(20.0f, 1.0f, 20.0f));
```

### Waypoint Marker
```cpp
UPyramidPrimitive* waypoint = new UPyramidPrimitive(1.5f, true);
waypoint->SetColor(FColor::Yellow);
waypoint->SetPosition(objectivePosition + FVector3(0, 2, 0));
waypoint->SetScale(FVector3(0.5f, 1.0f, 0.5f));

// Floating animation
void AnimateWaypoint(float time)
{
    float yOffset = std::sin(time * 2.0f) * 0.3f;
    waypoint->SetPosition(objectivePosition + FVector3(0, 2 + yOffset, 0));
    
    FQuaternion rot = FQuaternion::FromAxisAngle(FVector3(0, 1, 0), time * 45.0f);
    waypoint->SetRotation(rot);
}
```

### Building Roof
```cpp
UPyramidPrimitive* roof = new UPyramidPrimitive(3.0f, false);
roof->SetColor(FColor(139, 69, 19)); // Saddle brown
roof->SetPosition(buildingTop);
roof->SetScale(FVector3(5.0f, 1.0f, 5.0f));
```

### Power Crystal
```cpp
UPyramidPrimitive* crystal = new UPyramidPrimitive(2.0f, true);
crystal->SetColor(FColor(138, 43, 226)); // Blue violet
crystal->SetPosition(altarPosition);
crystal->SetScale(FVector3(0.8f, 1.5f, 0.8f));

// Glowing effect with rotation
void AnimateCrystal(float time)
{
    FQuaternion rot = FQuaternion::FromAxisAngle(FVector3(0, 1, 0), time * 60.0f);
    crystal->SetRotation(rot);
    
    float pulse = 0.8f + 0.2f * std::sin(time * 3.0f);
    crystal->SetScale(FVector3(0.8f * pulse, 1.5f, 0.8f * pulse));
}
```

### Spike Trap
```cpp
UPyramidPrimitive* spike = new UPyramidPrimitive(1.5f, true);
spike->SetColor(FColor::Gray);
spike->SetPosition(floorPosition);
spike->SetScale(FVector3(0.2f, 2.0f, 0.2f));
```

### Temple Structure
```cpp
// Create a step pyramid
std::vector<UPyramidPrimitive*> stepPyramid;
for (int i = 0; i < 5; ++i)
{
    UPyramidPrimitive* step = new UPyramidPrimitive(2.0f, true);
    step->SetColor(FColor(205, 175, 149)); // Tan
    step->SetPosition(FVector3(0, i * 2.0f, 0));
    step->SetScale(FVector3(10.0f - i * 1.5f, 1.0f, 10.0f - i * 1.5f));
    stepPyramid.push_back(step);
}
```

### Gem/Diamond
```cpp
// Create a diamond using two pyramids
UPyramidPrimitive* topHalf = new UPyramidPrimitive(1.5f, false);
topHalf->SetColor(FColor(0, 191, 255));
topHalf->SetPosition(FVector3(0, 0.75f, 0));

UPyramidPrimitive* bottomHalf = new UPyramidPrimitive(1.5f, false);
bottomHalf->SetColor(FColor(0, 191, 255));
bottomHalf->SetPosition(FVector3(0, -0.75f, 0));
bottomHalf->SetRotation(FQuaternion::FromAxisAngle(FVector3(1, 0, 0), 180.0f));
```

### Mountain Peak
```cpp
UPyramidPrimitive* peak = new UPyramidPrimitive(20.0f, true);
peak->SetColor(FColor(139, 137, 137)); // Gray
peak->SetPosition(FVector3(0, 10, 0));
peak->SetScale(FVector3(15.0f, 1.0f, 15.0f));
```

### Arrow Indicator
```cpp
UPyramidPrimitive* arrow = new UPyramidPrimitive(1.0f, false);
arrow->SetColor(FColor::Red);
arrow->SetPosition(targetPosition);
arrow->SetRotation(FQuaternion::FromAxisAngle(FVector3(1, 0, 0), -90.0f));
arrow->SetScale(FVector3(0.5f, 2.0f, 0.5f));
```

## Best Practices

### Visual Design
- **Ancient Pyramids**: Large base scale, moderate height
- **Sharp Markers**: Tall height, small base
- **Roof Caps**: Moderate height, match building size
- **Crystals**: Vertical scaling for elongated appearance

### Performance Tips
- **Static Geometry**: Pyramids have fixed vertex count (18 or 24)
- **Instancing**: Use for multiple similar pyramids
- **LOD**: No need for LOD (already low poly)

### Common Use Cases
1. **Architecture**: Pyramid buildings, roofs, caps
2. **Markers**: Waypoints, objectives, indicators
3. **Collectibles**: Gems, power-ups, crystals
4. **Environment**: Mountains, rock formations
5. **UI**: 3D direction arrows, pointers

## Mathematical Properties

### Surface Area (without base)
```
A = 2s√((s²/4) + h²)
where s = side length, h = height
```

### Surface Area (with base)
```
A = 2s√((s²/4) + h²) + s²
```

### Volume
```
V = (1/3)s²h
where s = side length, h = height
```

## Tips and Tricks

1. **Inverted Pyramid**: Rotate 180° for funnel shapes
2. **Flat Pyramid**: Small height for platform appearance
3. **Tall Spire**: Large height, small base for needle effect
4. **Diamond**: Combine two pyramids apex-to-apex
5. **Open Base**: Disable cap for hollow structures

## See Also

- [UConePrimitive](Cone.md) - Circular base alternative
- [UPrismPrimitive](Prism.md) - Triangular prism
- [UCubePrimitive](Cube.md) - Box primitive
