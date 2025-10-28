# UPlanePrimitive - Plane Rendering Primitive

## Overview

The `UPlanePrimitive` class represents a plane primitive in the TKD Game Engine. A plane is a flat, 2D surface in 3D space, commonly used for floors, walls, water surfaces, and terrain patches.

Planes are essential for:
- **Terrain Rendering**: Ground surfaces and terrain patches
- **Architectural Elements**: Floors, ceilings, and walls
- **Water Surfaces**: Lakes, rivers, and reflective surfaces
- **Particle Systems**: Ground collision planes
- **Debugging**: Reference surfaces and grid systems

## Architecture

### Class Hierarchy
```
UPrimitive (base class)
├── IDrawable
├── FTransformable
└── UPlanePrimitive
    ├── Member Variables
    │   └── m_bottomFaceCulled (bool) - Face culling control
    ├── Face Management
    └── Dynamic Geometry Generation
```

### Dependencies
- `UPrimitive` - Base primitive class providing core functionality
- `FVertex` - Vertex structure with position, color, and UV coordinates
- `FVector3` - 3D vector mathematics
- `FColor` - Color representation
- `EPrimitiveType` - Primitive rendering type enumeration

### Memory Layout
The `UPlanePrimitive` contains:
- **Member Variable**: 1 byte boolean for culling control
- **Inherited Members**: Vertex array, primitive type, color, transform
- **Dynamic Geometry**: 4 or 8 vertices depending on culling mode

## Core Components

### Member Variables

#### Bottom Face Culling (`m_bottomFaceCulled`)
```cpp
bool m_bottomFaceCulled;  //<! Indicates if bottom face is culled
```
- **Type**: `bool`
- **Purpose**: Controls whether the bottom face of the plane is rendered
- **Default**: `true` (bottom face culled by default)
- **Usage**: Optimizes rendering by hiding invisible faces

### Vertex Structure
The plane consists of vertices arranged in the XZ plane (Y = 0):

**Single-sided (bottom face culled)**: 4 vertices
```
(-0.5, 0.0, 0.5)     (0.5, 0.0, 0.5)
        ┌─────────────┐
        │             │
        │             │
        │    Plane    │
        │             │
        └─────────────┘
(-0.5, 0.0, -0.5)    (0.5, 0.0, -0.5)
```

**Double-sided (bottom face visible)**: 8 vertices (top + bottom faces)
```
Top Face:     Bottom Face:
3 ──── 2      7 ──── 6
│     │       │     │
│     │       │     │
0 ──── 1      4 ──── 5
```

### Face Definitions
- **Top Face**: Y = 0, normal pointing up (0, 1, 0)
- **Bottom Face**: Y = 0, normal pointing down (0, -1, 0) [optional]

## API Reference

### Constructor
```cpp
UPlanePrimitive::UPlanePrimitive(
    bool bottomFaceCulled = true, const FColor& color = FColor::White
);
```
Creates a plane primitive with optional bottom face culling and color.

**Parameters:**
- `bottomFaceCulled`: Whether to cull the bottom face (default: true)
- `color`: The color to apply to all vertices of the plane

**Implementation Details:**
- Calls base `UPrimitive` constructor with `EPrimitiveType::Quads`
- Generates initial vertex geometry based on culling setting

### Destructor
```cpp
virtual ~UPlanePrimitive() = default;
```
Default virtual destructor for proper inheritance cleanup.

### Face Management Methods

#### SetBottomFaceCulled
```cpp
void SetBottomFaceCulled(bool culled);
```
Sets whether the bottom face of the plane should be culled.

**Parameters:**
- `culled`: True to cull the bottom face, false to render both sides

**Implementation Details:**
- Only regenerates vertices if the setting actually changes
- Triggers `GenerateVertices()` when culling mode changes

#### IsBottomFaceCulled
```cpp
TKD_NODISCARD bool IsBottomFaceCulled(void) const;
```
Returns whether the bottom face is currently being culled.

**Returns:** True if bottom face is culled, false if both faces are rendered

### Private Methods

#### GenerateVertices
```cpp
void GenerateVertices(void);
```
Regenerates the vertex array based on the current culling setting.

**Algorithm:**
1. Clear existing vertex array
2. Add top face vertices (always present)
3. Add bottom face vertices if culling is disabled
4. All vertices use the current primitive color

## Usage Examples

### Basic Plane Creation
```cpp
// Create a single-sided ground plane
UPlanePrimitive* ground = new UPlanePrimitive(true, FColor::Green);

// Create a double-sided wall
UPlanePrimitive* wall = new UPlanePrimitive(false, FColor::Gray);
wall->SetRotation(FRotator3D(0.0f, 90.0f, 0.0f));  // Rotate to vertical
```

### Terrain System
```cpp
class TerrainSystem {
private:
    std::vector<UPlanePrimitive*> m_terrainPatches;

public:
    void GenerateTerrain(int patchesX, int patchesZ, float patchSize) {
        for (int x = 0; x < patchesX; ++x) {
            for (int z = 0; z < patchesZ; ++z) {
                auto* patch = new UPlanePrimitive(true, FColor::Green);
                patch->SetPosition(FVector3(
                    x * patchSize - (patchesX * patchSize * 0.5f),
                    0.0f,
                    z * patchSize - (patchesZ * patchSize * 0.5f)
                ));
                patch->SetScale(FVector3(patchSize, 1.0f, patchSize));
                m_terrainPatches.push_back(patch);
            }
        }
    }

    void Render(IRenderer& renderer) {
        for (auto* patch : m_terrainPatches) {
            patch->Draw(renderer);
        }
    }
};
```

### Water Surface
```cpp
class WaterSurface {
private:
    UPlanePrimitive* m_waterPlane;
    float m_waveTimer;

public:
    WaterSurface(float size) {
        m_waterPlane = new UPlanePrimitive(true, FColor(0.0f, 0.3f, 0.8f, 0.7f));
        m_waterPlane->SetScale(FVector3(size, 1.0f, size));
        m_waveTimer = 0.0f;
    }

    void Update(float deltaTime) {
        m_waveTimer += deltaTime;

        // Simple wave animation using scale
        float waveScale = 1.0f + std::sin(m_waveTimer * 2.0f) * 0.05f;
        FVector3 currentScale = m_waterPlane->GetScale();
        m_waterPlane->SetScale(FVector3(
            currentScale.x,
            waveScale,
            currentScale.z
        ));
    }

    void Render(IRenderer& renderer) {
        // Enable blending for transparency
        renderer.SetBlend(true);
        m_waterPlane->Draw(renderer);
        renderer.SetBlend(false);
    }
};
```

### Grid System for Debugging
```cpp
class DebugGrid {
private:
    std::vector<UPlanePrimitive*> m_gridLines;

public:
    void CreateGrid(int size, float spacing, const FColor& color) {
        // Create horizontal lines
        for (int i = -size; i <= size; ++i) {
            auto* line = new UPlanePrimitive(true, color);
            line->SetPosition(FVector3(0.0f, 0.0f, i * spacing));
            line->SetScale(FVector3(size * spacing * 2.0f, 1.0f, 0.01f));
            m_gridLines.push_back(line);
        }

        // Create vertical lines
        for (int i = -size; i <= size; ++i) {
            auto* line = new UPlanePrimitive(true, color);
            line->SetPosition(FVector3(i * spacing, 0.0f, 0.0f));
            line->SetScale(FVector3(0.01f, 1.0f, size * spacing * 2.0f));
            m_gridLines.push_back(line);
        }
    }

    void Render(IRenderer& renderer) {
        for (auto* line : m_gridLines) {
            line->Draw(renderer);
        }
    }
};
```

### Mirror Surface
```cpp
class MirrorSurface {
private:
    UPlanePrimitive* m_mirrorPlane;
    ITexture* m_reflectionTexture;

public:
    MirrorSurface(float width, float height) {
        // Create double-sided mirror plane
        m_mirrorPlane = new UPlanePrimitive(false, FColor::White);
        m_mirrorPlane->SetScale(FVector3(width, 1.0f, height));

        // TODO: Set up reflection texture
        m_reflectionTexture = nullptr;
    }

    void Render(IRenderer& renderer) {
        // Render reflection first (if implemented)
        if (m_reflectionTexture) {
            // Bind reflection texture
            m_reflectionTexture->Bind();
        }

        // Render mirror surface
        m_mirrorPlane->Draw(renderer);

        if (m_reflectionTexture) {
            m_reflectionTexture->Unbind();
        }
    }
};
```

### Collision Plane
```cpp
class CollisionPlane {
private:
    UPlanePrimitive* m_visualPlane;
    FVector3 m_normal;
    float m_distance;

public:
    CollisionPlane(const FVector3& normal, float distance) {
        m_normal = normal.Normalized();
        m_distance = distance;

        // Create visual representation
        m_visualPlane = new UPlanePrimitive(true, FColor(1.0f, 1.0f, 0.0f, 0.3f));

        // Position plane at correct location
        FVector3 position = m_normal * m_distance;
        m_visualPlane->SetPosition(position);

        // Orient plane to match normal
        FRotator3D rotation = CalculateRotationFromNormal(m_normal);
        m_visualPlane->SetRotation(rotation);
    }

    bool TestPoint(const FVector3& point) const {
        float distance = FVector3::Dot(point, m_normal) - m_distance;
        return distance >= 0.0f;  // Point is on positive side of plane
    }

    void Render(IRenderer& renderer) {
        renderer.SetBlend(true);
        m_visualPlane->Draw(renderer);
        renderer.SetBlend(false);
    }

private:
    FRotator3D CalculateRotationFromNormal(const FVector3& normal) {
        // Calculate rotation to align plane with normal
        FVector3 up = FVector3::Up;
        FVector3 right = FVector3::Cross(up, normal).Normalized();
        up = FVector3::Cross(normal, right).Normalized();

        // Convert to rotation matrix, then to rotator
        FMatrix3x3 rotationMatrix(right, up, normal);
        return FRotator3D(rotationMatrix);
    }
};
```

## Mathematical Foundation

### Plane Equation
The plane is defined by the equation: `ax + by + cz + d = 0`

For a plane at Y = 0:
- **Normal**: (0, 1, 0)
- **Equation**: `y = 0` or `0x + 1y + 0z + 0 = 0`

### Vertex Coordinates
```
Single-sided plane (Y = 0):
(-0.5, 0, 0.5)     (0.5, 0, 0.5)
       ┌─────────────┐
       │             │
       │             │
       └─────────────┘
(-0.5, 0, -0.5)    (0.5, 0, -0.5)
```

### Face Normals
- **Top Face**: (0, 1, 0) - pointing upward
- **Bottom Face**: (0, -1, 0) - pointing downward

### UV Coordinates (Future Enhancement)
When texture support is added:
```
(0,0)     (1,0)
   ┌───────┐
   │       │
   │       │
   └───────┘
(0,1)     (1,1)
```

## Performance Considerations

### Memory Usage
- **Per Plane**: ~33 bytes (inherited + 1 bool)
- **Vertex Data**: 4-8 vertices × ~32 bytes = 128-256 bytes
- **Total**: ~160-290 bytes per plane instance

### Rendering Performance
- **Draw Calls**: One per plane
- **Vertex Count**: 4 (single-sided) or 8 (double-sided)
- **Face Count**: 1 or 2 faces
- **State Changes**: Minimal

### Optimization Strategies
1. **Texture Atlasing**: Share textures across planes
2. **Batch Rendering**: Group planes with same properties
3. **Frustum Culling**: Only render visible planes
4. **LOD System**: Simplify distant planes

### Best Practices
- Use single-sided planes for ground/terrain (bottom face culled)
- Use double-sided planes for walls/windows
- Enable backface culling for performance
- Consider using triangle strips for better cache performance

## Diagrams

### Plane Orientation
```
World Space:
     Y
     │
     │
     ┌─────┐ ← Top Face (Y = 0.5)
    ╱     ╱│
   ╱     ╱ │
  ┌─────┼──┤
  │     │  │
  │     │  │ ← Plane Surface (Y = 0)
  │     │  │
  └─────┼──┘
   ╲    ╲
    ╲    ╲ ← Bottom Face (Y = -0.5)
     └────┘
       X
      ╱
     Z
```

### Single vs Double Sided
```
Single-Sided (Bottom Culled):
┌─────────────────┐
│ Top Face Only   │
│                 │
│ Normal: (0,1,0) │
└─────────────────┘

Double-Sided (Both Faces):
┌─────────────────┐
│ Top Face        │
│ Normal: (0,1,0) │
├─────────────────┤
│ Bottom Face     │
│ Normal: (0,-1,0)│
└─────────────────┘
```

### Vertex Layout
```
Single-Sided (4 vertices):
[0]: Position(-0.5, 0, 0.5)   // Bottom-left
[1]: Position(0.5, 0, 0.5)    // Bottom-right
[2]: Position(0.5, 0, -0.5)   // Top-right
[3]: Position(-0.5, 0, -0.5)  // Top-left

Double-Sided (8 vertices):
[0-3]: Top face vertices
[4-7]: Bottom face vertices (wound clockwise for proper normals)
```

## Integration with Rendering Pipeline

### Terrain LOD System
```cpp
class TerrainLOD {
private:
    std::vector<UPlanePrimitive*> m_lodLevels[4];

public:
    void GenerateLODLevels(int baseSize) {
        for (int lod = 0; lod < 4; ++lod) {
            int patchSize = baseSize / (1 << lod);
            int patchCount = (1 << lod);

            for (int x = 0; x < patchCount; ++x) {
                for (int z = 0; z < patchCount; ++z) {
                    auto* patch = new UPlanePrimitive(true, FColor::Green);
                    patch->SetScale(FVector3(patchSize, 1.0f, patchSize));
                    patch->SetPosition(FVector3(
                        x * patchSize - baseSize * 0.5f,
                        0.0f,
                        z * patchSize - baseSize * 0.5f
                    ));
                    m_lodLevels[lod].push_back(patch);
                }
            }
        }
    }

    void Render(IRenderer& renderer, const FCamera& camera) {
        FVector3 cameraPos = camera.GetPosition();

        for (int lod = 0; lod < 4; ++lod) {
            for (auto* patch : m_lodLevels[lod]) {
                FVector3 patchPos = patch->GetPosition();
                float distance = (cameraPos - patchPos).Length();

                // Render appropriate LOD level based on distance
                if (ShouldRenderLOD(distance, lod)) {
                    patch->Draw(renderer);
                }
            }
        }
    }

private:
    bool ShouldRenderLOD(float distance, int lod) {
        const float lodDistances[4] = {50.0f, 150.0f, 400.0f, 1000.0f};
        return distance < lodDistances[lod];
    }
};
```

### Water Reflection System
```cpp
class WaterReflectionSystem {
private:
    UPlanePrimitive* m_waterSurface;
    FCamera m_reflectionCamera;

public:
    WaterReflectionSystem(float size) {
        m_waterSurface = new UPlanePrimitive(true,
            FColor(0.0f, 0.2f, 0.8f, 0.8f));
        m_waterSurface->SetScale(FVector3(size, 1.0f, size));
    }

    void UpdateReflectionCamera(const FCamera& mainCamera) {
        // Mirror camera across water plane
        FVector3 cameraPos = mainCamera.GetPosition();
        m_reflectionCamera.SetPosition(FVector3(
            cameraPos.x,
            -cameraPos.y,  // Flip Y coordinate
            cameraPos.z
        ));

        FRotator3D cameraRot = mainCamera.GetRotation();
        m_reflectionCamera.SetRotation(FRotator3D(
            -cameraRot.pitch,  // Flip pitch
            cameraRot.yaw,
            cameraRot.roll
        ));
    }

    void Render(IRenderer& renderer, ITexture* reflectionTexture) {
        // Render reflection to texture first
        renderer.SetRenderTarget(reflectionTexture);
        // Render scene with reflection camera
        renderer.SetRenderTarget(nullptr);

        // Render water surface with reflection
        if (reflectionTexture) {
            reflectionTexture->Bind();
        }
        m_waterSurface->Draw(renderer);
        if (reflectionTexture) {
            reflectionTexture->Unbind();
        }
    }
};
```

## Troubleshooting

### Common Issues

#### Plane Not Visible
**Problem**: Plane doesn't appear on screen
**Solutions:**
- Check if positioned correctly in camera frustum
- Verify culling settings (single vs double-sided)
- Ensure proper orientation (might be edge-on to camera)
- Check renderer viewport and projection settings

#### Incorrect Orientation
**Problem**: Plane appears as a line or is invisible from some angles
**Solutions:**
- Verify plane normal is correct for intended orientation
- Check transform order (rotation might be applied before positioning)
- Ensure proper winding order for backface culling

#### Performance Issues
**Problem**: Many planes causing frame rate drops
**Solutions:**
- Implement frustum culling
- Use level-of-detail (LOD) system
- Batch planes with same material
- Consider using impostor textures at distance

#### Texture Mapping Issues
**Problem**: Textures appear stretched or incorrect on planes
**Solutions:**
- Verify UV coordinate generation
- Check aspect ratio of texture vs plane
- Ensure proper texture coordinate interpolation

### Debug Visualization
```cpp
void DebugDrawPlane(const UPlanePrimitive* plane) {
    FVector3 center = plane->GetPosition();
    FVector3 scale = plane->GetScale();

    // Draw plane outline
    FVector3 corners[4] = {
        center + FVector3(-scale.x * 0.5f, 0.0f, scale.z * 0.5f),
        center + FVector3(scale.x * 0.5f, 0.0f, scale.z * 0.5f),
        center + FVector3(scale.x * 0.5f, 0.0f, -scale.z * 0.5f),
        center + FVector3(-scale.x * 0.5f, 0.0f, -scale.z * 0.5f)
    };

    for (int i = 0; i < 4; ++i) {
        DebugRenderer::DrawLine(corners[i], corners[(i + 1) % 4], FColor::Cyan);
    }

    // Draw normal vector
    FVector3 normal = plane->GetTransform().GetUp();  // Assuming Y-up
    DebugRenderer::DrawLine(center, center + normal * 2.0f, FColor::Yellow);

    // Draw culling info
    DebugRenderer::DrawText(center + FVector3(0, 0.1f, 0),
        plane->IsBottomFaceCulled() ? "Single-sided" : "Double-sided",
        FColor::White);
}
```

## Future Enhancements

### Potential Features
- **Texture Support**: UV coordinates and texture mapping
- **Normal Mapping**: Detailed surface normals
- **Tessellation**: Dynamic subdivision for detail
- **Animation**: Wave deformation and movement
- **Physics Integration**: Collision detection and response

### API Extensions
```cpp
// Potential future methods
void UPlanePrimitive::SetTexture(ITexture* texture);
void UPlanePrimitive::SetUVScale(float uScale, float vScale);
void UPlanePrimitive::SetTesselationLevel(int level);
void UPlanePrimitive::ApplyWaveDeformation(float amplitude, float frequency);
FVector3 UPlanePrimitive::GetNormal() const;
bool UPlanePrimitive::TestIntersection(const FRay& ray, FVector3& hitPoint) const;
```
