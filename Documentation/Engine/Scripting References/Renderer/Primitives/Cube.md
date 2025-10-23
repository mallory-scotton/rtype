# UCubePrimitive - Cube Rendering Primitive

## Overview

The `UCubePrimitive` class represents a cube primitive in the TKD Game Engine. A cube is a fundamental 3D geometric shape consisting of six square faces, commonly used for boxes, buildings, obstacles, and basic 3D modeling.

Cubes are essential for:
- **Architecture**: Buildings, walls, and structural elements
- **Game Objects**: Crates, boxes, and containers
- **Debugging**: Visual placeholders and bounding volumes
- **Prototyping**: Quick 3D object creation and testing
- **Level Design**: Basic geometric building blocks

## Architecture

### Class Hierarchy
```
UPrimitive (base class)
├── IDrawable
├── FTransformable
└── UCubePrimitive
    └── Static Geometry (24 vertices, 6 faces)
```

### Dependencies
- `UPrimitive` - Base primitive class providing core functionality
- `FVertex` - Vertex structure with position, color, and UV coordinates
- `FVector3` - 3D vector mathematics
- `FColor` - Color representation
- `EPrimitiveType` - Primitive rendering type enumeration

### Memory Layout
The `UCubePrimitive` contains:
- **Inherited Members**: Vertex array, primitive type, color, transform
- **Static Geometry**: 24 vertices defining 6 faces (4 vertices per face)
- **No Additional Members**: All geometry is generated in constructor

## Core Components

### Vertex Structure
The cube is defined by 24 vertices arranged in 6 faces:
- **Front Face**: Z = +0.5 (positive Z direction)
- **Back Face**: Z = -0.5 (negative Z direction)
- **Top Face**: Y = +0.5 (positive Y direction)
- **Bottom Face**: Y = -0.5 (negative Y direction)
- **Right Face**: X = +0.5 (positive X direction)
- **Left Face**: X = -0.5 (negative X direction)

### Face Definitions
Each face consists of 4 vertices arranged in counter-clockwise order for proper backface culling:

```
Front Face (Z+):   Back Face (Z-):
(-0.5,-0.5,0.5)   (-0.5,-0.5,-0.5)
(0.5,-0.5,0.5)    (-0.5,0.5,-0.5)
(0.5,0.5,0.5)     (0.5,0.5,-0.5)
(-0.5,0.5,0.5)    (0.5,-0.5,-0.5)

Top Face (Y+):    Bottom Face (Y-):
(-0.5,0.5,-0.5)   (-0.5,-0.5,-0.5)
(-0.5,0.5,0.5)    (0.5,-0.5,-0.5)
(0.5,0.5,0.5)     (0.5,-0.5,0.5)
(0.5,0.5,-0.5)    (-0.5,-0.5,0.5)

Right Face (X+):  Left Face (X-):
(0.5,-0.5,-0.5)   (-0.5,-0.5,-0.5)
(0.5,0.5,-0.5)    (-0.5,-0.5,0.5)
(0.5,0.5,0.5)     (-0.5,0.5,0.5)
(0.5,-0.5,0.5)    (-0.5,0.5,-0.5)
```

## API Reference

### Constructor
```cpp
UCubePrimitive::UCubePrimitive(const FColor& color = FColor::White);
```
Creates a cube primitive with the specified color.

**Parameters:**
- `color`: The color to apply to all vertices of the cube

**Implementation Details:**
- Calls base `UPrimitive` constructor with `EPrimitiveType::Quads`
- Generates 24 vertices defining the 6 faces of the cube
- All vertices are positioned relative to origin (0,0,0)
- Cube extends from -0.5 to +0.5 in all three axes

### Destructor
```cpp
virtual ~UCubePrimitive() = default;
```
Default virtual destructor for proper inheritance cleanup.

## Usage Examples

### Basic Cube Creation
```cpp
// Create a white cube at the origin
UCubePrimitive* cube = new UCubePrimitive();

// Create a colored cube
UCubePrimitive* redCube = new UCubePrimitive(FColor::Red);
UCubePrimitive* blueCube = new UCubePrimitive(FColor(0.0f, 0.0f, 1.0f, 1.0f));
```

### Positioned and Scaled Cubes
```cpp
// Create and position a cube
UCubePrimitive* building = new UCubePrimitive(FColor::Gray);
building->SetPosition(FVector3(10.0f, 5.0f, 0.0f));
building->SetScale(FVector3(2.0f, 10.0f, 2.0f));  // Tall building
```

### Cube Grid Generation
```cpp
class CubeGrid {
private:
    std::vector<UCubePrimitive*> m_cubes;

public:
    void GenerateGrid(int width, int height, int depth) {
        for (int x = 0; x < width; ++x) {
            for (int y = 0; y < height; ++y) {
                for (int z = 0; z < depth; ++z) {
                    auto* cube = new UCubePrimitive(FColor::White);
                    cube->SetPosition(FVector3(x * 2.0f, y * 2.0f, z * 2.0f));
                    m_cubes.push_back(cube);
                }
            }
        }
    }

    void Render(IRenderer& renderer) {
        for (auto* cube : m_cubes) {
            cube->Draw(renderer);
        }
    }
};
```

### Dynamic Cube Coloring
```cpp
class ColorChangingCube {
private:
    UCubePrimitive* m_cube;
    float m_timer;

public:
    ColorChangingCube() {
        m_cube = new UCubePrimitive(FColor::White);
        m_timer = 0.0f;
    }

    void Update(float deltaTime) {
        m_timer += deltaTime;

        // Cycle through colors
        float r = (std::sin(m_timer) + 1.0f) * 0.5f;
        float g = (std::sin(m_timer + 2.0f) + 1.0f) * 0.5f;
        float b = (std::sin(m_timer + 4.0f) + 1.0f) * 0.5f;

        m_cube->SetColor(FColor(r, g, b, 1.0f));
    }

    void Draw(IRenderer& renderer) {
        m_cube->Draw(renderer);
    }
};
```

### Physics Debug Visualization
```cpp
class PhysicsDebugger {
private:
    std::vector<UCubePrimitive*> m_debugCubes;

public:
    void AddBoundingBox(const FBoundingBox& bounds, const FColor& color) {
        FVector3 center = bounds.GetCenter();
        FVector3 extents = bounds.GetExtents();

        auto* cube = new UCubePrimitive(color);
        cube->SetPosition(center);
        cube->SetScale(extents * 2.0f);  // Convert extents to scale

        m_debugCubes.push_back(cube);
    }

    void Clear() {
        for (auto* cube : m_debugCubes) {
            delete cube;
        }
        m_debugCubes.clear();
    }

    void Render(IRenderer& renderer) {
        for (auto* cube : m_debugCubes) {
            // Draw wireframe
            cube->Draw(renderer, FRenderStates(/* wireframe mode */));
        }
    }
};
```

### Voxel Terrain
```cpp
class VoxelTerrain {
private:
    std::vector<UCubePrimitive*> m_voxels;
    const float VOXEL_SIZE = 1.0f;

public:
    void SetVoxel(int x, int y, int z, bool active, const FColor& color) {
        FVector3 position(x * VOXEL_SIZE, y * VOXEL_SIZE, z * VOXEL_SIZE);

        if (active) {
            // Add or update voxel
            auto* voxel = new UCubePrimitive(color);
            voxel->SetPosition(position);
            voxel->SetScale(FVector3(VOXEL_SIZE, VOXEL_SIZE, VOXEL_SIZE));
            m_voxels.push_back(voxel);
        } else {
            // Remove voxel at position
            RemoveVoxelAt(position);
        }
    }

    void RemoveVoxelAt(const FVector3& position) {
        auto it = std::find_if(m_voxels.begin(), m_voxels.end(),
                              [&position](UCubePrimitive* voxel) {
                                  return voxel->GetPosition() == position;
                              });
        if (it != m_voxels.end()) {
            delete *it;
            m_voxels.erase(it);
        }
    }

    void Render(IRenderer& renderer) {
        for (auto* voxel : m_voxels) {
            voxel->Draw(renderer);
        }
    }
};
```

## Mathematical Foundation

### Cube Geometry
The cube is centered at the origin with vertices at coordinates (±0.5, ±0.5, ±0.5):

**Volume**: (1 × 1 × 1) = 1 cubic unit
**Surface Area**: 6 × (1 × 1) = 6 square units
**Edge Length**: 1 unit per edge
**Center to Face**: 0.5 units

### Vertex Ordering
Vertices are ordered counter-clockwise when viewed from outside:
```
For Front Face (Z+):
3 ────── 2
│       │
│       │
0 ────── 1
```
This ensures proper backface culling with standard winding order.

### Face Normals
Each face has an outward-pointing normal:
- **Front (Z+)**: (0, 0, 1)
- **Back (Z-)**: (0, 0, -1)
- **Top (Y+)**: (0, 1, 0)
- **Bottom (Y-)**: (0, -1, 0)
- **Right (X+)**: (1, 0, 0)
- **Left (X-)**: (-1, 0, 0)

## Performance Considerations

### Memory Usage
- **Per Cube**: ~32 bytes (inherited from UPrimitive)
- **Vertex Data**: 24 vertices × ~32 bytes = 768 bytes
- **Total**: ~800 bytes per cube instance

### Rendering Performance
- **Draw Calls**: One per cube (can be batched)
- **Vertex Count**: 24 vertices per cube
- **Face Count**: 6 faces (12 triangles after tessellation)
- **State Changes**: Minimal (color changes only)

### Optimization Strategies
1. **Instancing**: Render multiple cubes in single draw call
2. **Frustum Culling**: Only render visible cubes
3. **Level-of-Detail**: Use simpler geometry at distance
4. **Texture Atlasing**: Share textures across cubes

### Best Practices
- Use object pooling for frequently created/destroyed cubes
- Implement frustum culling for large numbers of cubes
- Consider using triangle strips for better vertex cache performance
- Batch cubes with same material properties

## Diagrams

### Cube Coordinate System
```
     Y
     │
     │
     3 ────── 2
    ╱│      ╱│
   ╱ │     ╱ │
  7 ────── 6 │
  │  │    │  │
  │  0 ───│── 1 ── X
  │ ╱     │ ╱
  │╱      │╱
  4 ────── 5
 ╱
Z
```

### Face Identification
```
     ┌─────┐
    ╱     ╱│
   ╱  T  ╱ │
  ┌─────┼──┤
  │     │ R│
  │  F  │  │
  │     │  │
  └─────┼──┘
   ╲  B ╲
    ╲    ╲
     └────┘
F=Front, B=Back, L=Left, R=Right, T=Top, B=Bottom
```

### Vertex Indices per Face
```
Front Face:  [0,1,2,3]  Back Face:  [4,5,6,7]
Top Face:    [8,9,10,11] Bottom Face:[12,13,14,15]
Right Face:  [16,17,18,19] Left Face: [20,21,22,23]
```

## Integration with Rendering Pipeline

### Batch Rendering
```cpp
class FCubeBatch {
private:
    std::vector<FVertex> m_vertexBuffer;
    std::vector<FMatrix4x4> m_transforms;
    FColor m_currentColor;

public:
    void AddCube(const UCubePrimitive* cube) {
        if (cube->GetColor() != m_currentColor) {
            Flush();
            m_currentColor = cube->GetColor();
        }

        // Add cube vertices with transform
        const auto& vertices = cube->GetVertices();
        FMatrix4x4 transform = cube->GetTransform().GetMatrix();

        for (const auto& vertex : vertices) {
            FVertex transformedVertex = vertex;
            transformedVertex.position = transform * vertex.position;
            m_vertexBuffer.push_back(transformedVertex);
        }
    }

    void Flush(IRenderer& renderer) {
        if (!m_vertexBuffer.empty()) {
            renderer.Draw(m_vertexBuffer, EPrimitiveType::Quads);
            m_vertexBuffer.clear();
        }
    }
};
```

### Instanced Rendering
```cpp
class FInstancedCubeRenderer {
private:
    UCubePrimitive* m_cubeTemplate;
    std::vector<FMatrix4x4> m_instanceTransforms;

public:
    FInstancedCubeRenderer() {
        m_cubeTemplate = new UCubePrimitive(FColor::White);
    }

    void AddInstance(const FMatrix4x4& transform) {
        m_instanceTransforms.push_back(transform);
    }

    void Render(IRenderer& renderer) {
        if (!m_instanceTransforms.empty()) {
            renderer.DrawInstanced(
                m_cubeTemplate->GetVertices(),
                m_cubeTemplate->GetPrimitiveType(),
                m_instanceTransforms
            );
            m_instanceTransforms.clear();
        }
    }
};
```

## Troubleshooting

### Common Issues

#### Cube Not Visible
**Problem**: Cube doesn't appear on screen
**Solutions:**
- Check position is within camera view frustum
- Verify scale is not zero or negative
- Ensure proper lighting or shading
- Check renderer viewport settings

#### Incorrect Orientation
**Problem**: Cube appears rotated incorrectly
**Solutions:**
- Verify coordinate system expectations
- Check transform order (scale → rotate → translate)
- Ensure proper parent-child relationships

#### Performance Issues with Many Cubes
**Problem**: Frame rate drops with large numbers of cubes
**Solutions:**
- Implement frustum culling
- Use level-of-detail (LOD) system
- Switch to instanced rendering
- Consider using impostors at distance

#### Z-Fighting Between Faces
**Problem**: Visible seams between cube faces
**Solutions:**
- Enable polygon offset in render states
- Use higher precision depth buffer
- Add small epsilon to face positions

### Debug Visualization
```cpp
void DebugDrawCube(const UCubePrimitive* cube) {
    FVector3 center = cube->GetPosition();
    FVector3 scale = cube->GetScale();

    // Draw wireframe outline
    DebugRenderer::DrawWireframeCube(center, scale, FColor::Cyan);

    // Draw face normals
    FVector3 normals[6] = {
        FVector3(0,0,1), FVector3(0,0,-1),  // Front, Back
        FVector3(0,1,0), FVector3(0,-1,0),  // Top, Bottom
        FVector3(1,0,0), FVector3(-1,0,0)   // Right, Left
    };

    for (int i = 0; i < 6; ++i) {
        FVector3 normalStart = center;
        FVector3 normalEnd = center + normals[i] * (scale.GetMax() * 0.5f);
        DebugRenderer::DrawLine(normalStart, normalEnd, FColor::Yellow);
    }
}
```

## Future Enhancements

### Potential Features
- **Textured Cubes**: Support for different textures per face
- **Rounded Edges**: Beveled cube corners and edges
- **Procedural Generation**: Algorithmic cube modifications
- **Physics Integration**: Built-in collision detection
- **Material System**: PBR materials and lighting

### API Extensions
```cpp
// Potential future methods
void UCubePrimitive::SetFaceColor(EFace face, const FColor& color);
void UCubePrimitive::SetFaceTexture(EFace face, ITexture* texture);
void UCubePrimitive::SetRoundedEdges(float radius);
void UCubePrimitive::Subdivide(int level);
FVector3 UCubePrimitive::GetFaceNormal(EFace face) const;

enum class EFace {
    Front, Back, Left, Right, Top, Bottom
};
```
