# UTorusPrimitive - Torus (Donut) Rendering Primitive

## Overview

The `UTorusPrimitive` class represents a torus (donut shape) primitive in the TKD Game Engine. A torus is defined by a major radius (distance from center to tube center) and a minor radius (tube radius). It's perfect for creating rings, wheels, portals, and decorative elements.

Torus primitives are essential for:
- **Rings**: Wedding rings, magical rings, and power rings
- **Wheels**: Tires, gears, and mechanical wheels
- **Portals**: Teleportation gates and dimensional doorways
- **Decorations**: Wreaths, donuts, and circular ornaments
- **Architecture**: Circular moldings and trim

## Architecture

### Class Hierarchy
```
UPrimitive → IDrawable, FTransformable → UTorusPrimitive
```

### Key Components
- **m_majorSectors**: Divisions around the major circle
- **m_minorSectors**: Divisions around the tube cross-section
- **m_majorRadius**: Distance from center to tube center
- **m_minorRadius**: Radius of the tube itself

## API Reference

### Constructor
```cpp
UTorusPrimitive(
    SizeT majorSectors = 48,
    SizeT minorSectors = 24,
    float majorRadius = 1.0f,
    float minorRadius = 0.3f,
    const FColor& color = FColor::White
);
```

**Parameters:**
- `majorSectors`: Smoothness around the ring
- `minorSectors`: Smoothness of the tube cross-section
- `majorRadius`: Distance from center to tube center
- `minorRadius`: Tube thickness
- `color`: Torus color

### Methods
```cpp
void SetMajorSectors(SizeT sectors);
SizeT GetMajorSectors(void) const;
void SetMinorSectors(SizeT sectors);
SizeT GetMinorSectors(void) const;
void SetMajorRadius(float radius);
float GetMajorRadius(void) const;
void SetMinorRadius(float radius);
float GetMinorRadius(void) const;
```

## Usage Examples

### Wedding Ring
```cpp
UTorusPrimitive* ring = new UTorusPrimitive(48, 24, 0.8f, 0.1f);
ring->SetColor(FColor(255, 215, 0)); // Gold
ring->SetPosition(fingerPosition);
ring->SetScale(FVector3(0.5f, 0.5f, 0.5f));
```

### Tire
```cpp
UTorusPrimitive* tire = new UTorusPrimitive(48, 24, 1.5f, 0.5f);
tire->SetColor(FColor::Black);
tire->SetPosition(wheelPosition);
tire->SetRotation(FQuaternion::FromAxisAngle(FVector3(1, 0, 0), 90.0f));
```

### Portal
```cpp
UTorusPrimitive* portal = new UTorusPrimitive(64, 32, 3.0f, 0.3f);
portal->SetColor(FColor(0, 191, 255, 200)); // Translucent cyan
portal->SetPosition(FVector3(0, 2, 0));

// Animate rotation
void UpdatePortal(float deltaTime)
{
    FQuaternion rot = FQuaternion::FromAxisAngle(FVector3(0, 1, 0), deltaTime * 90.0f);
    portal->SetRotation(rot * portal->GetRotation());
}
```

### Lifebuoy
```cpp
UTorusPrimitive* lifebuoy = new UTorusPrimitive(32, 16, 1.2f, 0.3f);
lifebuoy->SetColor(FColor(255, 69, 0)); // Orange red
lifebuoy->SetPosition(waterSurfacePosition);
```

### Power Ring
```cpp
UTorusPrimitive* powerRing = new UTorusPrimitive(48, 24, 2.0f, 0.2f);
powerRing->SetColor(FColor::Yellow);
powerRing->SetPosition(collectiblePosition);

// Floating animation
void AnimatePowerRing(float time)
{
    float yOffset = std::sin(time * 2.0f) * 0.5f;
    powerRing->SetPosition(collectiblePosition + FVector3(0, yOffset, 0));
    
    FQuaternion rot = FQuaternion::FromAxisAngle(FVector3(0, 1, 0), time * 60.0f);
    powerRing->SetRotation(rot);
}
```

### Donut
```cpp
UTorusPrimitive* donut = new UTorusPrimitive(48, 24, 0.8f, 0.4f);
donut->SetColor(FColor(210, 105, 30)); // Chocolate
donut->SetPosition(platePosition + FVector3(0, 0.5f, 0));
```

### Spaceship Ring
```cpp
UTorusPrimitive* shipRing = new UTorusPrimitive(64, 32, 10.0f, 2.0f);
shipRing->SetColor(FColor(192, 192, 192)); // Silver
shipRing->SetPosition(spaceshipCenter);
```

## Best Practices

### Sector Guidelines
- **Major Sectors**: 32-64 for smooth rings
- **Minor Sectors**: 16-32 for smooth tubes
- **Performance**: Reduce sectors for distant tori

### Visual Tips
- **Thin Rings**: Small minor radius (< 0.2 * major radius)
- **Thick Tires**: Large minor radius (> 0.4 * major radius)
- **Smooth Appearance**: Use 48+ major sectors, 24+ minor sectors

### Common Use Cases
1. **Collectibles**: Rings, hoops, and power-ups
2. **Vehicles**: Tires, wheels, and steering wheels
3. **Portals**: Teleportation gates and wormholes
4. **Decorations**: Wreaths, garlands, and ornaments
5. **UI**: 3D loading rings and progress indicators

## Mathematical Properties

### Surface Area
```
A = 4π²Rr
where R = major radius, r = minor radius
```

### Volume
```
V = 2π²Rr²
```

## See Also

- [UCylinderPrimitive](Cylinder.md)
- [USpherePrimitive](Sphere.md)
- [UCapsulePrimitive](Capsule.md)
