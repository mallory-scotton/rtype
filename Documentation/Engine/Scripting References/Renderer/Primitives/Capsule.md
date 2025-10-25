# UCapsulePrimitive - Capsule (Pill) Rendering Primitive

## Overview

The `UCapsulePrimitive` class represents a capsule primitive in the TKD Game Engine. A capsule is a cylinder with hemispherical ends, commonly known as a "pill" shape. It's the ideal primitive for character collision shapes and organic cylindrical objects.

Capsules are essential for:
- **Character Collision**: Player and NPC collision boundaries
- **Pills/Medicine**: Pharmaceutical objects and health items
- **Organic Shapes**: Smooth, rounded cylindrical objects
- **Projectiles**: Bullets and missile bodies
- **UI Elements**: Rounded buttons and sliders

## Architecture

### Class Hierarchy
```
UPrimitive → IDrawable, FTransformable → UCapsulePrimitive
```

### Key Components
- **m_sectorCount**: Smoothness around the circumference
- **m_stackCount**: Smoothness of the hemispherical ends
- **m_cylinderHeight**: Height of the cylindrical middle section
- **m_radius**: Radius of the capsule (both cylinder and hemispheres)

## API Reference

### Constructor
```cpp
UCapsulePrimitive(
    SizeT sectorCount = 32,
    SizeT stackCount = 16,
    float cylinderHeight = 1.0f,
    float radius = 0.5f,
    const FColor& color = FColor::White
);
```

**Parameters:**
- `sectorCount`: Smoothness around circumference (default: 32)
- `stackCount`: Hemisphere detail (default: 16)
- `cylinderHeight`: Height of middle cylinder part (default: 1.0)
- `radius`: Radius of capsule (default: 0.5)
- `color`: Capsule color

### Methods
```cpp
void SetSectorCount(SizeT sectorCount);
SizeT GetSectorCount(void) const;
void SetStackCount(SizeT stackCount);
SizeT GetStackCount(void) const;
void SetCylinderHeight(float height);
float GetCylinderHeight(void) const;
void SetRadius(float radius);
float GetRadius(void) const;
float GetTotalHeight(void) const;  // cylinder height + 2*radius
```

## Usage Examples

### Character Collision Shape
```cpp
UCapsulePrimitive* characterCollider = new UCapsulePrimitive(16, 12, 1.5f, 0.4f);
characterCollider->SetColor(FColor(0, 255, 0, 50)); // Translucent green
characterCollider->SetPosition(characterPosition);
// Typically not rendered in final game, used for collision detection
```

### Health Pill
```cpp
UCapsulePrimitive* healthPill = new UCapsulePrimitive(24, 16, 0.8f, 0.3f);
healthPill->SetColor(FColor::Red);
healthPill->SetPosition(itemPosition);
healthPill->SetRotation(FQuaternion::FromAxisAngle(FVector3(0, 0, 1), 90.0f));

// Floating animation
void AnimatePill(float time)
{
    float yOffset = std::sin(time * 2.0f) * 0.2f;
    healthPill->SetPosition(itemPosition + FVector3(0, yOffset, 0));
    
    FQuaternion rot = FQuaternion::FromAxisAngle(FVector3(0, 1, 0), time * 60.0f);
    healthPill->SetRotation(rot);
}
```

### Player Character Body
```cpp
// Simplified character body for physics
UCapsulePrimitive* playerBody = new UCapsulePrimitive(24, 16, 1.2f, 0.35f);
playerBody->SetColor(FColor(100, 150, 255));
playerBody->SetPosition(playerPosition);
```

### Bullet Projectile
```cpp
UCapsulePrimitive* bullet = new UCapsulePrimitive(12, 8, 0.4f, 0.15f);
bullet->SetColor(FColor::Yellow);
bullet->SetPosition(gunPosition);
bullet->SetRotation(FQuaternion::FromAxisAngle(FVector3(1, 0, 0), 90.0f));

// Move forward
void UpdateBullet(float deltaTime)
{
    FVector3 forward = bullet->GetForward();
    bullet->SetPosition(bullet->GetPosition() + forward * bulletSpeed * deltaTime);
}
```

### Medicine Capsule
```cpp
// Two-toned capsule
UCapsulePrimitive* medicine = new UCapsulePrimitive(32, 16, 1.0f, 0.4f);
// For two-tone effect, you'd need two half-capsules or texture mapping
medicine->SetColor(FColor(255, 0, 0)); // Red
medicine->SetPosition(FVector3(5, 1, 5));
medicine->SetRotation(FQuaternion::FromAxisAngle(FVector3(0, 0, 1), 90.0f));
```

### Finger/Limb Segment
```cpp
UCapsulePrimitive* finger = new UCapsulePrimitive(12, 8, 0.6f, 0.1f);
finger->SetColor(FColor(255, 220, 177)); // Skin tone
finger->SetPosition(handPosition);
finger->SetRotation(fingerRotation);
```

### Energy Cell
```cpp
UCapsulePrimitive* energyCell = new UCapsulePrimitive(24, 16, 2.0f, 0.5f);
energyCell->SetColor(FColor(0, 255, 255)); // Cyan
energyCell->SetPosition(batterySlot);

// Pulsating glow effect
void AnimateEnergyCell(float time)
{
    float pulse = 0.9f + 0.1f * std::sin(time * 4.0f);
    energyCell->SetScale(FVector3(pulse, pulse, pulse));
    
    int alpha = 200 + static_cast<int>(55.0f * std::sin(time * 4.0f));
    energyCell->SetColor(FColor(0, 255, 255, alpha));
}
```

### Hotdog
```cpp
UCapsulePrimitive* hotdog = new UCapsulePrimitive(24, 16, 1.5f, 0.3f);
hotdog->SetColor(FColor(210, 105, 30)); // Chocolate (brown)
hotdog->SetPosition(platePosition);
hotdog->SetRotation(FQuaternion::FromAxisAngle(FVector3(0, 0, 1), 90.0f));
```

### Rounded Button
```cpp
UCapsulePrimitive* button = new UCapsulePrimitive(24, 16, 0.1f, 0.3f);
button->SetColor(FColor(100, 100, 255));
button->SetPosition(uiPosition);
button->SetRotation(FQuaternion::FromAxisAngle(FVector3(1, 0, 0), 90.0f));
```

### NPC Collision
```cpp
// Simple AI character collision
UCapsulePrimitive* npcCollider = new UCapsulePrimitive(16, 12, 1.0f, 0.3f);
npcCollider->SetColor(FColor(255, 0, 0, 50)); // Debug red
npcCollider->SetPosition(npcPosition);
```

## Best Practices

### Sector/Stack Guidelines
- **Low Detail**: 12 sectors, 8 stacks - distant objects
- **Medium Detail**: 24 sectors, 16 stacks - standard objects
- **High Detail**: 32+ sectors, 24+ stacks - close-up objects

### Collision Shapes
- **Character Colliders**: Use medium detail (16-24 sectors)
- **Radius**: Slightly smaller than visual model
- **Height**: Match character center mass height
- **Origin**: Position at ground level or center mass

### Visual Design
- **Pills**: Horizontal orientation (rotate around Z-axis)
- **Characters**: Vertical orientation (default)
- **Fingers/Limbs**: Thin radius, appropriate length
- **Buttons**: Very short cylinder height, larger radius

### Performance Tips
1. **LOD System**: Reduce sectors/stacks for distant capsules
2. **Collision Only**: Use low detail for non-visible colliders
3. **Instancing**: Reuse for similar character shapes
4. **Batch Rendering**: Group similar capsules

### Common Use Cases
1. **Character Physics**: Collision boundaries for players/NPCs
2. **Pickups**: Pills, batteries, and collectibles
3. **Projectiles**: Bullets, missiles, and energy blasts
4. **UI**: Rounded buttons and progress bars
5. **Organic Objects**: Fingers, limbs, and body parts

## Advanced Techniques

### Character Controller
```cpp
class CharacterController
{
public:
    UCapsulePrimitive* collider;
    FVector3 velocity;
    
    void Update(float deltaTime)
    {
        // Apply gravity
        velocity.y -= 9.8f * deltaTime;
        
        // Move capsule
        FVector3 newPos = collider->GetPosition() + velocity * deltaTime;
        
        // Check collisions...
        // Resolve collisions...
        
        collider->SetPosition(newPos);
    }
};
```

### Compound Shape
```cpp
// Create a dumbbell using two capsules
UCapsulePrimitive* bar = new UCapsulePrimitive(12, 8, 2.0f, 0.1f);
bar->SetColor(FColor::Gray);
bar->SetRotation(FQuaternion::FromAxisAngle(FVector3(0, 0, 1), 90.0f));

UCapsulePrimitive* weight1 = new UCapsulePrimitive(16, 12, 0.3f, 0.4f);
weight1->SetColor(FColor::Black);
weight1->SetPosition(bar->GetPosition() - FVector3(1.5f, 0, 0));

UCapsulePrimitive* weight2 = new UCapsulePrimitive(16, 12, 0.3f, 0.4f);
weight2->SetColor(FColor::Black);
weight2->SetPosition(bar->GetPosition() + FVector3(1.5f, 0, 0));
```

### Animated Growth
```cpp
void GrowCapsule(UCapsulePrimitive* capsule, float deltaTime, float growthRate)
{
    float currentHeight = capsule->GetCylinderHeight();
    capsule->SetCylinderHeight(currentHeight + growthRate * deltaTime);
}
```

## Mathematical Properties

### Surface Area
```
A = 2πrh + 4πr²
where r = radius, h = cylinder height
```

### Volume
```
V = πr²h + (4/3)πr³
```

### Total Height
```
H = h + 2r
where h = cylinder height, r = radius
```

## Tips and Tricks

1. **Perfect Collision**: Capsule is ideal for character collision (no edge catching)
2. **Orientation**: Vertical for characters, horizontal for pills
3. **Radius Sizing**: Make slightly smaller than visual mesh for smooth movement
4. **Debug Rendering**: Render collider with transparency to verify alignment
5. **Performance**: Use lower detail for collision-only capsules

## Comparison

### Capsule vs Cylinder
- ✅ Capsule: Smooth collision, no sharp edges
- ✅ Cylinder: Flat ends, architectural uses

### Capsule vs Sphere
- ✅ Capsule: Better for tall objects (characters)
- ✅ Sphere: Better for rolling objects (balls)

## See Also

- [UCylinderPrimitive](Cylinder.md) - Cylinder with flat ends
- [USpherePrimitive](Sphere.md) - Spherical primitive
- [UConePrimitive](Cone.md) - Tapered shape
- [FTransformable](../Transformable.md) - 3D transformation
