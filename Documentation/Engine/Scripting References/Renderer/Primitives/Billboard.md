# UBillboardPrimitive - Billboard Rendering Primitive

## Overview

The `UBillboardPrimitive` class represents a billboard primitive in the TKD Game Engine. A billboard is a 2D sprite that always faces the camera, commonly used for particles, trees, grass, and other effects that should maintain consistent orientation regardless of camera movement.

Billboards are essential for:
- **Particle Systems**: Individual particles that always face the camera
- **Vegetation Rendering**: Trees, grass, and foliage that maintain visual consistency
- **Sprite-based Effects**: Explosions, smoke, and other atmospheric effects
- **Performance Optimization**: Reducing geometry complexity for distant objects
- **UI Elements**: 2D overlays in 3D space

## Architecture

### Class Hierarchy
```
UPrimitive (base class)
├── IDrawable
├── FTransformable
└── UBillboardPrimitive
    ├── Member Variables
    │   ├── m_texture (ITexture*) - Billboard texture
    │   ├── m_textureRect (FRectanglei) - Texture cropping rectangle
    │   └── m_vertices (inherited) - Vertex array
    ├── Texture Management
    ├── Vertex Generation
    └── Camera Alignment (TODO)
```

### Dependencies
- `UPrimitive` - Base primitive class providing core functionality
- `ITexture` - Texture interface for billboard imagery
- `FRectanglei` - Integer rectangle for texture cropping
- `FVertex` - Vertex structure with position, color, and UV coordinates
- `IRenderer` - Rendering interface for drawing operations
- `FCamera` - Camera information for billboard orientation

### Memory Layout
The `UBillboardPrimitive` extends `UPrimitive` with:
- **Texture Pointer**: 8 bytes (pointer to ITexture)
- **Texture Rectangle**: 16 bytes (FRectanglei: left, top, width, height)
- **Inherited Members**: Vertex array, primitive type, color, transform

## Core Components

### Member Variables

#### Texture (`m_texture`)
```cpp
mutable ITexture* m_texture;  //<! Texture of the billboard
```
- **Type**: `ITexture*` (mutable for const-correctness in Draw method)
- **Purpose**: Stores the texture used for billboard rendering
- **Default**: `nullptr`
- **Usage**: Provides visual content for the billboard quad

#### Texture Rectangle (`m_textureRect`)
```cpp
FRectanglei m_textureRect;  //<! Texture rectangle for cropping
```
- **Type**: `FRectanglei`
- **Purpose**: Defines which portion of the texture to use
- **Default**: `FRectanglei::Zero` (empty rectangle)
- **Usage**: Enables texture atlasing and sprite sheets

### Inherited Members

#### Vertices (`m_vertices`)
```cpp
std::vector<FVertex> m_vertices;  //<! Vertex array (inherited)
```
- **Type**: `std::vector<FVertex>`
- **Purpose**: Stores the four corners of the billboard quad
- **Structure**: Each vertex contains position, color, and UV coordinates

#### Primitive Type (`m_primitiveType`)
```cpp
EPrimitiveType m_primitiveType;  //<! Type of the primitive (inherited)
```
- **Value**: `EPrimitiveType::Quads`
- **Purpose**: Defines how vertices are interpreted during rendering

#### Color (`m_color`)
```cpp
FColor m_color;  //<! Color of the primitive (inherited)
```
- **Purpose**: Base color applied to all vertices
- **Default**: `FColor::White`

## API Reference

### Constructors

#### Primary Constructor
```cpp
UBillboardPrimitive::UBillboardPrimitive(
    ITexture* texture = nullptr,
    const FRectanglei& rect = FRectanglei::Zero,
    const FColor& color = FColor::White
);
```
Initializes a billboard primitive with optional texture, texture rectangle, and color.

**Parameters:**
- `texture`: Pointer to the texture to use (can be nullptr)
- `rect`: Rectangle defining texture region to use
- `color`: Base color for the billboard

### Destructor
```cpp
virtual ~UBillboardPrimitive() = default;
```
Default virtual destructor for proper inheritance cleanup.

### Rendering Methods

#### Draw Method
```cpp
virtual void Draw(
    IRenderer& renderer, FRenderStates states = FRenderStates()
) const override;
```
Renders the billboard primitive. Currently implements basic quad rendering with texture support.

**Parameters:**
- `renderer`: Reference to the renderer to use for drawing
- `states`: Additional render states (currently unused)

**Implementation Details:**
- Retrieves camera information from renderer
- TODO: Implements camera-facing alignment
- Handles texture binding and blending
- Calls renderer.Draw with vertex data and transform

### Texture Management

#### SetTexture
```cpp
void SetTexture(ITexture* texture);
```
Sets the texture used by the billboard.

**Parameters:**
- `texture`: New texture pointer (can be nullptr)

#### SetTextureRect
```cpp
void SetTextureRect(const FRectanglei& rect);
```
Sets the texture rectangle for cropping and triggers vertex regeneration.

**Parameters:**
- `rect`: New texture rectangle defining the region to use

### Private Methods

#### UpdateVertices
```cpp
void UpdateVertices(void);
```
Regenerates the vertex array based on current texture and rectangle settings.

**Algorithm:**
1. Determines billboard dimensions from texture or defaults to 1x1
2. Calculates UV coordinates based on texture rectangle
3. Creates four vertices forming a centered quad
4. Applies proper UV mapping for texture coordinates

## Usage Examples

### Basic Billboard Creation
```cpp
// Create a simple billboard with a texture
ITexture* treeTexture = renderer.LoadTexture("tree.png");
UBillboardPrimitive* treeBillboard = new UBillboardPrimitive(treeTexture);

// Position the billboard in the world
treeBillboard->SetPosition(FVector3(10.0f, 0.0f, 5.0f));
treeBillboard->SetScale(FVector3(2.0f, 2.0f, 1.0f));  // Make it larger
```

### Texture Atlas Usage
```cpp
// Use a specific region from a texture atlas
FRectanglei spriteRect(64, 0, 32, 32);  // 32x32 sprite at position (64,0)
UBillboardPrimitive* particle = new UBillboardPrimitive(
    particleAtlasTexture, spriteRect, FColor::Yellow
);
```

### Particle System Integration
```cpp
class ParticleSystem {
private:
    std::vector<UBillboardPrimitive*> m_particles;
    ITexture* m_particleTexture;

public:
    void Initialize() {
        m_particleTexture = renderer.LoadTexture("particle.png");
    }

    void SpawnParticle(const FVector3& position, const FColor& color) {
        auto* particle = new UBillboardPrimitive(
            m_particleTexture, FRectanglei::Zero, color
        );
        particle->SetPosition(position);
        particle->SetScale(FVector3(0.5f, 0.5f, 1.0f));
        m_particles.push_back(particle);
    }

    void Render(IRenderer& renderer) {
        for (auto* particle : m_particles) {
            particle->Draw(renderer);
        }
    }
};
```

### Animated Billboards
```cpp
class AnimatedBillboard {
private:
    UBillboardPrimitive* m_billboard;
    ITexture* m_textureAtlas;
    std::vector<FRectanglei> m_frames;
    float m_frameTime;
    float m_timer;
    size_t m_currentFrame;

public:
    AnimatedBillboard(ITexture* atlas, const std::vector<FRectanglei>& frames)
        : m_textureAtlas(atlas), m_frames(frames), m_frameTime(0.1f)
        , m_timer(0.0f), m_currentFrame(0) {
        m_billboard = new UBillboardPrimitive(atlas, frames[0]);
    }

    void Update(float deltaTime) {
        m_timer += deltaTime;
        if (m_timer >= m_frameTime) {
            m_timer = 0.0f;
            m_currentFrame = (m_currentFrame + 1) % m_frames.size();
            m_billboard->SetTextureRect(m_frames[m_currentFrame]);
        }
    }

    void Draw(IRenderer& renderer) {
        m_billboard->Draw(renderer);
    }
};
```

### Forest Rendering
```cpp
class ForestRenderer {
private:
    std::vector<UBillboardPrimitive*> m_trees;
    ITexture* m_treeTexture;

public:
    void GenerateForest(int treeCount) {
        m_treeTexture = renderer.LoadTexture("tree_billboard.png");

        for (int i = 0; i < treeCount; ++i) {
            // Random position in forest area
            float x = RandomFloat(-100.0f, 100.0f);
            float z = RandomFloat(-100.0f, 100.0f);
            float scale = RandomFloat(0.8f, 1.5f);

            auto* tree = new UBillboardPrimitive(m_treeTexture);
            tree->SetPosition(FVector3(x, 0.0f, z));
            tree->SetScale(FVector3(scale, scale, 1.0f));

            m_trees.push_back(tree);
        }
    }

    void Render(IRenderer& renderer) {
        // Enable alpha blending for proper tree rendering
        for (auto* tree : m_trees) {
            tree->Draw(renderer);
        }
    }
};
```

## Mathematical Foundation

### Vertex Generation Algorithm
```
Size Calculation:
if (textureRect not empty):
    width = textureRect.width / 32.0
    height = textureRect.height / 32.0
else if (texture exists):
    width = texture.width / 32.0
    height = texture.height / 32.0
else:
    width = 1.0, height = 1.0

UV Calculation:
u0 = rect.left / texture.width
v0 = rect.top / texture.height
u1 = (rect.left + rect.width) / texture.width
v1 = (rect.top + rect.height) / texture.height

Vertex Positions (centered quad):
(-width/2, -height/2, 0) → UV(u0, v1)  // Bottom-left
( width/2, -height/2, 0) → UV(u1, v1)  // Bottom-right
( width/2,  height/2, 0) → UV(u1, v0)  // Top-right
(-width/2,  height/2, 0) → UV(u0, v0)  // Top-left
```

### Camera Alignment (Planned)
The billboard is designed to always face the camera using the following approach:
```
Camera Direction = normalize(camera.position - billboard.position)
Right Vector = normalize(cross(Up, CameraDirection))
Up Vector = normalize(cross(CameraDirection, Right))

Billboard Transform = rotation matrix from [Right, Up, CameraDirection]
```

## Performance Considerations

### Memory Usage
- **Per Billboard**: ~64 bytes (base UPrimitive + texture pointer + rectangle)
- **Vertex Data**: 4 vertices × ~32 bytes = 128 bytes
- **Texture**: Shared across multiple billboards

### Rendering Performance
- **Draw Calls**: One per billboard (can be batched)
- **Vertex Count**: 4 vertices per billboard
- **Texture Binds**: One per unique texture
- **State Changes**: Minimal (blend mode for transparency)

### Optimization Strategies
1. **Texture Atlasing**: Combine multiple sprites into single texture
2. **Billboard Culling**: Frustum and distance culling
3. **LOD System**: Switch to simpler geometry at distance
4. **Instancing**: Render multiple identical billboards in single draw call

### Best Practices
- Use power-of-two texture dimensions
- Enable alpha blending for transparent billboards
- Implement proper depth sorting for transparency
- Consider using impostor textures for complex objects

## Diagrams

### Billboard Coordinate System
```
World Space:
     Camera
       ↓
       │
  ┌────┼────┐
  │    │    │
  │    │    │ ← Billboard always faces camera
  │    │    │
  └────┼────┘
       │
       ↑
   Billboard Position
```

### Texture Mapping
```
Texture Atlas:
┌─────────────────┐
│                 │
│  [64,0,32,32]   │ ← m_textureRect
│                 │
└─────────────────┘
      ↓ Maps to ↓
Billboard Quad:
(-w/2,h/2)   (w/2,h/2)
     ┌─────────┐
     │         │
     │         │
     └─────────┘
(-w/2,-h/2)  (w/2,-h/2)
```

### Vertex Layout
```
Vertex Array (4 vertices):
[0]: Position(-w/2,-h/2,0), UV(u0,v1)  // Bottom-left
[1]: Position( w/2,-h/2,0), UV(u1,v1)  // Bottom-right
[2]: Position( w/2, h/2,0), UV(u1,v0)  // Top-right
[3]: Position(-w/2, h/2,0), UV(u0,v0)  // Top-left
```

## Integration with Rendering Pipeline

### Renderer Integration
```cpp
class FBillboardRenderer {
private:
    std::vector<UBillboardPrimitive*> m_billboards;

public:
    void AddBillboard(UBillboardPrimitive* billboard) {
        m_billboards.push_back(billboard);
    }

    void Render(IRenderer& renderer) {
        // Sort by distance for transparency
        std::sort(m_billboards.begin(), m_billboards.end(),
                 [](const auto* a, const auto* b) {
                     return DistanceToCamera(a) > DistanceToCamera(b);
                 });

        for (auto* billboard : m_billboards) {
            billboard->Draw(renderer);
        }
    }
};
```

### Batch Rendering (Future Enhancement)
```cpp
class FBillboardBatch {
private:
    std::vector<FVertex> m_vertexBuffer;
    ITexture* m_currentTexture;

public:
    void AddBillboard(const UBillboardPrimitive* billboard) {
        if (billboard->GetTexture() != m_currentTexture) {
            Flush();
            m_currentTexture = billboard->GetTexture();
        }

        // Transform and add vertices to batch
        const auto& vertices = billboard->GetVertices();
        FMatrix4x4 transform = billboard->GetTransform().GetMatrix();

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

## Troubleshooting

### Common Issues

#### Billboard Not Visible
**Problem**: Billboard doesn't appear on screen
**Solutions:**
- Check texture pointer is valid
- Verify texture coordinates are within [0,1] range
- Ensure proper alpha blending is enabled
- Check viewport and camera settings

#### Incorrect Texture Mapping
**Problem**: Wrong part of texture appears on billboard
**Solutions:**
- Verify `m_textureRect` coordinates are correct
- Check texture dimensions match expectations
- Ensure UV coordinates are calculated properly

#### Performance Issues
**Problem**: Too many billboards causing frame rate drops
**Solutions:**
- Implement frustum culling
- Use level-of-detail (LOD) system
- Consider using instanced rendering
- Batch billboards with same texture

#### Z-Fighting with Other Geometry
**Problem**: Billboards fight for depth with 3D geometry
**Solutions:**
- Use polygon offset in render states
- Implement proper depth sorting
- Use separate render passes for billboards

### Debug Visualization
```cpp
void DebugDrawBillboard(const UBillboardPrimitive* billboard) {
    // Draw billboard bounds
    FVector3 center = billboard->GetPosition();
    FVector3 size = billboard->GetScale();

    DebugRenderer::DrawWireframeQuad(
        center, size.x, size.y, FColor::Cyan
    );

    // Draw texture info
    if (billboard->GetTexture()) {
        DebugRenderer::DrawText(
            center + FVector3(0, size.y/2 + 0.1f, 0),
            Format("Tex: {}x{}", billboard->GetTexture()->GetWidth(),
                   billboard->GetTexture()->GetHeight()),
            FColor::White
        );
    }
}
```

## Future Enhancements

### Planned Features
- **Camera Alignment**: Proper billboard rotation to face camera
- **Spherical Billboards**: Cylindrical vs spherical orientation modes
- **LOD System**: Automatic detail reduction at distance
- **Animation Support**: Built-in sprite animation
- **Particle Integration**: Native particle system support

### API Extensions
```cpp
// Potential future methods
enum class EBillboardMode {
    Spherical,     // Always face camera (full rotation)
    Cylindrical,   // Rotate only around Y axis
    Fixed          // No rotation, fixed orientation
};

void UBillboardPrimitive::SetBillboardMode(EBillboardMode mode);
void UBillboardPrimitive::SetAnimation(const std::vector<FRectanglei>& frames,
                                      float frameRate);
void UBillboardPrimitive::SetSize(float width, float height);
```
