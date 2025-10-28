# UPrismPrimitive - Triangular Prism Rendering Primitive

## Overview

The `UPrismPrimitive` class represents a triangular prism primitive in the TKD Game Engine. A prism has two parallel triangular faces connected by three rectangular faces. Perfect for roofs, ramps, wedges, and architectural elements.

Prisms are essential for:
- **Architecture**: Roof structures, building elements
- **Ramps**: Sloped surfaces and inclines
- **Wedges**: Splitting tools and geometric shapes
- **Props**: Tents, arrow shapes, and triangular objects
- **Environment**: Rock formations and terrain features

## Architecture

### Class Hierarchy
```
UPrimitive → IDrawable, FTransformable → UPrismPrimitive
```

### Key Components
- **m_length**: Length along the prism axis (depth)
- **m_capEnds**: Whether triangular ends are capped
- Base: Isosceles triangle centered at origin
- Default size: 1.0 unit wide, 1.0 unit tall

## API Reference

### Constructor
```cpp
UPrismPrimitive(
    float length = 2.0f,
    bool capEnds = true,
    const FColor& color = FColor::White
);
```

**Parameters:**
- `length`: Length of the prism (default: 2.0)
- `capEnds`: Whether to cap triangular ends (default: true)
- `color`: Prism color

### Methods
```cpp
void SetLength(float length);
float GetLength(void) const;
void SetCapEnds(bool cap);
bool AreEndsCapped(void) const;
```

## Usage Examples

### Roof Structure
```cpp
UPrismPrimitive* roof = new UPrismPrimitive(10.0f, true);
roof->SetColor(FColor(139, 69, 19)); // Saddle brown
roof->SetPosition(buildingTop);
roof->SetScale(FVector3(5.0f, 2.0f, 1.0f));
roof->SetRotation(FQuaternion::FromAxisAngle(FVector3(0, 1, 0), 90.0f));
```

### Ramp
```cpp
UPrismPrimitive* ramp = new UPrismPrimitive(5.0f, true);
ramp->SetColor(FColor::Gray);
ramp->SetPosition(FVector3(0, 0, 0));
ramp->SetRotation(FQuaternion::FromAxisAngle(FVector3(0, 0, 1), -90.0f));
ramp->SetScale(FVector3(3.0f, 1.0f, 1.0f));
```

### Tent
```cpp
UPrismPrimitive* tent = new UPrismPrimitive(3.0f, true);
tent->SetColor(FColor(34, 139, 34)); // Forest green
tent->SetPosition(campgroundPosition);
tent->SetScale(FVector3(2.0f, 1.5f, 1.0f));
tent->SetRotation(FQuaternion::FromAxisAngle(FVector3(0, 1, 0), 90.0f));
```

### Arrow Shape
```cpp
UPrismPrimitive* arrow = new UPrismPrimitive(1.0f, true);
arrow->SetColor(FColor::Red);
arrow->SetPosition(targetPosition + FVector3(0, 2, 0));
arrow->SetRotation(FQuaternion::FromAxisAngle(FVector3(0, 0, 1), -90.0f));
arrow->SetScale(FVector3(0.5f, 1.0f, 0.5f));

// Point arrow at target
void PointArrow(const FVector3& direction)
{
    // Calculate rotation to point at direction
    FQuaternion rot = FQuaternion::LookRotation(direction, FVector3::Up);
    arrow->SetRotation(rot);
}
```

### Cheese Wedge
```cpp
UPrismPrimitive* cheese = new UPrismPrimitive(1.5f, true);
cheese->SetColor(FColor(255, 255, 0)); // Yellow
cheese->SetPosition(platePosition);
cheese->SetScale(FVector3(1.0f, 0.8f, 1.0f));
```

### Bookend
```cpp
UPrismPrimitive* bookend = new UPrismPrimitive(0.5f, true);
bookend->SetColor(FColor(139, 69, 19)); // Brown
bookend->SetPosition(shelfPosition);
bookend->SetRotation(FQuaternion::FromAxisAngle(FVector3(0, 0, 1), 90.0f));
bookend->SetScale(FVector3(1.2f, 1.0f, 1.0f));
```

### Triangular Beam
```cpp
UPrismPrimitive* beam = new UPrismPrimitive(10.0f, false);
beam->SetColor(FColor(160, 82, 45)); // Sienna
beam->SetPosition(FVector3(0, 5, 0));
beam->SetRotation(FQuaternion::FromAxisAngle(FVector3(0, 1, 0), 90.0f));
beam->SetScale(FVector3(0.5f, 0.5f, 1.0f));
```

### Rock Formation
```cpp
UPrismPrimitive* rock = new UPrismPrimitive(2.0f, true);
rock->SetColor(FColor(105, 105, 105)); // Dim gray
rock->SetPosition(terrainPosition);
rock->SetRotation(FQuaternion::FromEuler(15.0f, 30.0f, 10.0f));
rock->SetScale(FVector3(1.5f, 2.0f, 1.0f));
```

### Directional Sign
```cpp
// Create a directional arrow sign
UPrismPrimitive* signArrow = new UPrismPrimitive(0.3f, true);
signArrow->SetColor(FColor::Yellow);
signArrow->SetPosition(signPostTop);
signArrow->SetRotation(FQuaternion::FromAxisAngle(FVector3(0, 0, 1), -90.0f));
signArrow->SetScale(FVector3(0.5f, 1.0f, 0.8f));
// Rotate to point in direction
FQuaternion direction = FQuaternion::FromAxisAngle(FVector3(0, 1, 0), directionAngle);
signArrow->SetRotation(direction * signArrow->GetRotation());
```

### Sloped Platform
```cpp
UPrismPrimitive* platform = new UPrismPrimitive(8.0f, true);
platform->SetColor(FColor(70, 130, 180)); // Steel blue
platform->SetPosition(FVector3(0, 0, 0));
platform->SetRotation(FQuaternion::FromAxisAngle(FVector3(0, 0, 1), -90.0f));
platform->SetScale(FVector3(4.0f, 1.0f, 1.0f));
```

## Best Practices

### Orientation Guidelines
- **Roof**: Point triangle up, length along building
- **Ramp**: Triangle on side, length as ramp direction
- **Arrow**: Point thin edge forward
- **Tent**: Triangle as front/back, length as depth

### Scaling Tips
- **X-axis**: Width of the base
- **Y-axis**: Height of the triangle
- **Z-axis**: Length/depth of the prism

### Visual Design
- **Sharp Roofs**: Tall Y-scale, normal X-scale
- **Gentle Slopes**: Low Y-scale, large X-scale
- **Long Beams**: Large Z-scale, small X and Y

### Performance
- **Fixed Geometry**: Prisms have constant vertex count
- **No LOD Needed**: Already low-poly (6 or 8 faces)
- **Instancing**: Use for repeated elements (roof tiles)

### Common Use Cases
1. **Architecture**: Roofs, gables, and structural beams
2. **Gameplay**: Ramps, slopes, and wedges
3. **Props**: Tents, signs, and markers
4. **Environment**: Rock formations, terrain features
5. **UI**: 3D arrows and directional indicators

## Advanced Techniques

### Compound Roof
```cpp
// Create a peaked roof with two prisms
UPrismPrimitive* roofLeft = new UPrismPrimitive(10.0f, true);
roofLeft->SetColor(FColor(139, 69, 19));
roofLeft->SetPosition(buildingTop);
roofLeft->SetRotation(FQuaternion::FromAxisAngle(FVector3(0, 1, 0), 90.0f));
roofLeft->SetScale(FVector3(5.0f, 2.0f, 1.0f));

UPrismPrimitive* roofRight = new UPrismPrimitive(10.0f, true);
roofRight->SetColor(FColor(139, 69, 19));
roofRight->SetPosition(buildingTop);
roofRight->SetRotation(FQuaternion::FromAxisAngle(FVector3(0, 1, 0), -90.0f));
roofRight->SetScale(FVector3(5.0f, 2.0f, 1.0f));
```

### Rotating Arrow Indicator
```cpp
void UpdateArrowIndicator(UPrismPrimitive* arrow, const FVector3& targetPos)
{
    FVector3 direction = (targetPos - arrow->GetPosition()).Normalized();
    float angle = std::atan2(direction.x, direction.z) * 180.0f / M_PI;
    
    FQuaternion rot1 = FQuaternion::FromAxisAngle(FVector3(0, 0, 1), -90.0f);
    FQuaternion rot2 = FQuaternion::FromAxisAngle(FVector3(0, 1, 0), angle);
    arrow->SetRotation(rot2 * rot1);
}
```

### Terrain Feature
```cpp
// Create jagged rock formation with multiple prisms
std::vector<UPrismPrimitive*> rocks;
for (int i = 0; i < 5; ++i)
{
    UPrismPrimitive* rock = new UPrismPrimitive(
        1.0f + (rand() % 100) / 50.0f, true
    );
    rock->SetColor(FColor(105, 105, 105));
    
    FVector3 offset(
        (rand() % 200 - 100) / 50.0f,
        0,
        (rand() % 200 - 100) / 50.0f
    );
    rock->SetPosition(formationCenter + offset);
    
    FQuaternion rot = FQuaternion::FromEuler(
        rand() % 360, rand() % 360, rand() % 360
    );
    rock->SetRotation(rot);
    
    float scale = 0.5f + (rand() % 100) / 100.0f;
    rock->SetScale(FVector3(scale, scale, scale));
    
    rocks.push_back(rock);
}
```

## Mathematical Properties

### Surface Area (with caps)
```
A = base_perimeter * length + 2 * triangle_area
```

### Volume
```
V = triangle_area * length
```

### Triangle Vertices (Default)
```
Front: (-0.5, -0.5, L/2), (0.5, -0.5, L/2), (0.0, 0.5, L/2)
Back:  (-0.5, -0.5, -L/2), (0.5, -0.5, -L/2), (0.0, 0.5, -L/2)
where L = length
```

## Tips and Tricks

1. **Roof Angle**: Adjust Y-scale for roof pitch
2. **Ramp Slope**: Rotate 90° around Z-axis, then adjust scale
3. **Arrow Direction**: Use look-at rotation for dynamic pointing
4. **Open Ends**: Disable caps for tunnel/tube effects
5. **Compound Shapes**: Combine multiple prisms for complex roofs

## Comparison

### Prism vs Pyramid
- ✅ Prism: Constant cross-section, two parallel triangular faces
- ✅ Pyramid: Tapers to a point, one triangular base

### Prism vs Cylinder
- ✅ Prism: Triangular cross-section, angular
- ✅ Cylinder: Circular cross-section, smooth

## See Also

- [UPyramidPrimitive](Pyramid.md) - Pyramid with square base
- [UCylinderPrimitive](Cylinder.md) - Cylindrical primitive
- [UCubePrimitive](Cube.md) - Box primitive
- [UConePrimitive](Cone.md) - Tapered cone shape
- [FTransformable](../Transformable.md) - 3D transformation
