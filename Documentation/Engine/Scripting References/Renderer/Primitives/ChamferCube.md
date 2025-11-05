# Chamfer Cube Primitive

## Overview

The **Chamfer Cube** is a 3D primitive that represents a cube with beveled/chamfered edges instead of sharp corners. This creates a smoother, more refined appearance compared to the standard cube primitive.

## Features

- **Parametizable Chamfer Size**: Control the size of the beveled edges (0.0 to 0.5)
- **Texture Support**: Full texture mapping with proper UV coordinates
- **Color Support**: Inherits color system from base primitive class
- **Transform Support**: Full 3D transformation capabilities (position, rotation, scale)
- **Component Integration**: `UChamferCubeComponent` for use with the actor/component system

## Classes

### UChamferCubePrimitive

The primitive class that handles the geometry generation and rendering.

**Location**: `Engine/Source/Engine/Renderer/Primitives/UChamferCubePrimitive.hpp`

**Key Methods**:
- `UChamferCubePrimitive(const FColor& color, Float chamferSize)` - Constructor
- `SetChamferSize(Float size)` - Set the chamfer size (0.0 to 0.5)
- `GetChamferSize()` - Get the current chamfer size
- `SetTexture(ITexture* texture)` - Apply a texture to the chamfer cube
- `GetTexture()` - Get the current texture
- `Draw(IRenderer& renderer, FRenderStates states)` - Render the chamfer cube

### UChamferCubeComponent

The component wrapper for integration with actors.

**Location**: `Engine/Source/Engine/Runtime/Components/UChamferCubeComponent.hpp`

**Key Methods**:
- `UChamferCubeComponent(const FString& name, Float chamferSize)` - Constructor
- `GetChamferCube()` - Access the underlying primitive
- `SetLocalTransform(const FTransform& transform)` - Set local transform
- `GetLocalTransform()` - Get local transform
- `SetTexture(ITexture* texture)` - Apply a texture
- `SetChamferSize(Float size)` - Set chamfer size
- `GetChamferSize()` - Get chamfer size

## Chamfer Size Parameter

The **chamfer size** parameter controls how much of each edge is beveled:

- **0.0**: No chamfer (approaches a regular cube, but uses triangle geometry)
- **0.1**: Small bevel (default value, subtle smoothing)
- **0.25**: Medium bevel (noticeable rounded edges)
- **0.5**: Maximum bevel (creates an octahedron-like shape)

Valid range: `0.0` to `0.5` (values outside this range are clamped)

## Usage Examples

### Example 1: Basic Chamfer Cube

```cpp
#include <Engine/Renderer/Primitives.hpp>

// Create a chamfer cube with default chamfer size (0.1)
UChamferCubePrimitive chamferCube(FColor::White);

// Draw the chamfer cube
chamferCube.Draw(renderer);
```

### Example 2: Custom Chamfer Size

```cpp
#include <Engine/Renderer/Primitives.hpp>

// Create a chamfer cube with larger bevel
UChamferCubePrimitive chamferCube(FColor::Blue, 0.25f);

// Or modify existing chamfer size
chamferCube.SetChamferSize(0.15f);
```

### Example 3: With Texture

```cpp
#include <Engine/Renderer/Primitives.hpp>

// Create chamfer cube
UChamferCubePrimitive chamferCube(FColor::White, 0.2f);

// Load and apply texture
ITexture* texture = LoadTexture("Assets/metal_texture.png");
chamferCube.SetTexture(texture);

// Set transform
FTransform transform;
transform.SetPosition(FVector3(0.0f, 0.0f, 5.0f));
transform.SetScale(FVector3(2.0f, 2.0f, 2.0f));
chamferCube.SetTransform(transform);

// Draw
chamferCube.Draw(renderer);
```

### Example 4: Using Component with Actor

```cpp
#include <Engine/Runtime/Components.hpp>

// In your actor setup
class MyActor : public AActor
{
public:
    MyActor()
    {
        // Create chamfer cube component
        auto* chamferCube = CreateComponent<UChamferCubeComponent>(
            "MyChamferCube", 0.15f
        );
        
        // Set texture
        ITexture* texture = LoadTexture("Assets/cube_texture.png");
        chamferCube->SetTexture(texture);
        
        // Set local transform
        FTransform localTransform;
        localTransform.SetPosition(FVector3(0.0f, 1.0f, 0.0f));
        chamferCube->SetLocalTransform(localTransform);
    }
};
```

### Example 5: Animating Chamfer Size

```cpp
// In your update loop
void UpdateChamferCube(UChamferCubeComponent* chamferCube, Float deltaTime)
{
    static Float time = 0.0f;
    time += deltaTime;
    
    // Oscillate chamfer size between 0.05 and 0.3
    Float chamferSize = 0.175f + 0.125f * std::sin(time);
    chamferCube->SetChamferSize(chamferSize);
}
```

## Geometry Details

The chamfer cube is generated using triangle primitives (not quads like the regular cube). The geometry includes:

- **6 main faces**: Each face has a center quad (split into 2 triangles)
- **12 edge chamfers**: Each edge has a beveled strip (split into 2 triangles)
- **8 corner chamfers**: Each corner has a triangular chamfer face

Total geometry varies based on chamfer size but includes approximately:
- **6 × 2 triangles** (main face centers)
- **12 × 2 triangles** (edge chamfers)  
- **12 × 2 triangles** (face edge chamfers)
- **8 × 1 triangles** (corner chamfers)

## Differences from Standard Cube

| Feature | Standard Cube | Chamfer Cube |
|---------|--------------|--------------|
| **Edges** | Sharp, 90° angles | Beveled/rounded |
| **Geometry** | Quads (6 faces, 24 vertices) | Triangles (many more vertices) |
| **Parametrization** | None | Chamfer size (0.0 to 0.5) |
| **Performance** | Faster (fewer vertices) | Slightly slower (more vertices) |
| **Visual** | Hard edges | Smooth, refined edges |
| **Use Case** | General purpose | Refined/polished objects |

## Performance Considerations

- The chamfer cube uses significantly more vertices than a standard cube
- Chamfer size modifications trigger geometry regeneration
- If chamfer size is static, performance impact is minimal during rendering
- For many small chamfer cubes, consider using instanced rendering

## Technical Notes

- The primitive uses `EPrimitiveType::Triangles` instead of `Quads`
- Texture coordinates are properly mapped to maintain consistent appearance
- Chamfer size is clamped to [0.0, 0.5] to prevent invalid geometry
- The geometry is regenerated whenever chamfer size changes
- UV mapping maintains proportional texture appearance across all faces

## See Also

- `UCubePrimitive` - Standard cube primitive
- `USpherePrimitive` - Sphere primitive (smooth alternative)
- `UPrimitive` - Base class for all primitives
- Component system documentation
