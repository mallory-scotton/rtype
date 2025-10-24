# USpherePrimitive - Sphere Rendering Primitive

## Overview

The `USpherePrimitive` class represents a sphere primitive in the TKD Game Engine. A sphere is a perfectly round 3D geometric shape, commonly used for planets, balls, particles, and smooth organic shapes.

Spheres are essential for:
- **Celestial Bodies**: Planets, moons, and stars
- **Game Objects**: Balls, marbles, and spherical projectiles
- **Particle Systems**: Spherical particles and effects
- **Collision Detection**: Spherical bounding volumes
- **Debugging**: Distance fields and influence spheres

## Architecture

### Class Hierarchy
```
UPrimitive (base class)
├── IDrawable
├── FTransformable
└── USpherePrimitive
    ├── Member Variables
    │   ├── m_stacks (std::vector<std::vector<FVertex>>) - Vertex stacks
    │   ├── m_sectorCount (SizeT) - Longitude divisions
    │   └── m_stackCount (SizeT) - Latitude divisions
    ├── Spherical Geometry Generation
    ├── Dynamic Tesselation Control
    └── Multi-Stack Rendering
```

### Dependencies
- `UPrimitive` - Base primitive class providing core functionality
- `FVertex` - Vertex structure with position, color, and UV coordinates
- `FVector3` - 3D vector mathematics
- `FColor` - Color representation
- `EPrimitiveType` - Primitive rendering type enumeration
- `<vector>` - Dynamic arrays for vertex storage
- `<cmath>` - Mathematical functions for spherical coordinates

### Memory Layout
The `USpherePrimitive` contains:
- **Stack Storage**: Vector of vectors containing vertex data
- **Tessellation Parameters**: Sector and stack counts
- **Inherited Members**: Primitive type, color, transform
- **Dynamic Geometry**: Variable vertex count based on tessellation

## Core Components

### Member Variables

#### Stacks (`m_stacks`)
```cpp
std::vector<std::vector<FVertex>> m_stacks;  //<! Stacks of vertices
```
- **Type**: `std::vector<std::vector<FVertex>>`
- **Purpose**: Stores vertex data organized by latitude stacks
- **Structure**: Each inner vector represents one stack of vertices
- **Usage**: Enables efficient rendering of sphere geometry

#### Sector Count (`m_sectorCount`)
```cpp
SizeT m_sectorCount;  //<! Number of sectors (longitude)
```
- **Type**: `SizeT`
- **Purpose**: Controls horizontal tessellation (longitude divisions)
- **Range**: Typically 8-64, default 32
- **Effect**: Higher values create smoother horizontal curvature

#### Stack Count (`m_stackCount`)
```cpp
SizeT m_stackCount;  //<! Number of stacks (latitude)
```
- **Type**: `SizeT`
- **Purpose**: Controls vertical tessellation (latitude divisions)
- **Range**: Typically 6-48, default 32
- **Effect**: Higher values create smoother vertical curvature

### Spherical Geometry
The sphere is generated using spherical coordinates:
- **Radius**: 1.0 unit (can be scaled via transform)
- **Latitude**: From -90° to +90° (θ angle)
- **Longitude**: From 0° to 360° (φ angle)

**Spherical to Cartesian conversion:**
```
x = cos(θ) × sin(φ)
y = sin(θ)
z = cos(θ) × cos(φ)
```
Where θ is latitude and φ is longitude.

## API Reference

### Constructor
```cpp
USpherePrimitive::USpherePrimitive(
    SizeT sectorCount = 32,
    SizeT stackCount = 32,
    const FColor& color = FColor::White
);
```
Creates a sphere primitive with specified tessellation and color.

**Parameters:**
- `sectorCount`: Number of sectors (longitude divisions, default: 32)
- `stackCount`: Number of stacks (latitude divisions, default: 32)
- `color`: The color to apply to all vertices of the sphere

**Implementation Details:**
- Calls base `UPrimitive` constructor with `EPrimitiveType::QuadStrip`
- Generates initial vertex geometry based on tessellation parameters

### Destructor
```cpp
virtual ~USpherePrimitive() = default;
```
Default virtual destructor for proper inheritance cleanup.

### Tessellation Control Methods

#### SetSectorCount
```cpp
void SetSectorCount(SizeT sectorCount);
```
Sets the number of sectors (longitude divisions) for the sphere.

**Parameters:**
- `sectorCount`: New sector count (must be > 0)

**Implementation Details:**
- Only regenerates vertices if the value actually changes
- Triggers `GenerateVertices()` with new tessellation

#### GetSectorCount
```cpp
TKD_NODISCARD SizeT GetSectorCount(void) const;
```
Returns the current number of sectors.

**Returns:** Current sector count

#### SetStackCount
```cpp
void SetStackCount(SizeT stackCount);
```
Sets the number of stacks (latitude divisions) for the sphere.

**Parameters:**
- `stackCount`: New stack count (must be > 0)

**Implementation Details:**
- Only regenerates vertices if the value actually changes
- Triggers `GenerateVertices()` with new tessellation

#### GetStackCount
```cpp
TKD_NODISCARD SizeT GetStackCount(void) const;
```
Returns the current number of stacks.

**Returns:** Current stack count

### Rendering Methods

#### Draw
```cpp
virtual void Draw(IRenderer& renderer, FRenderStates states = FRenderStates()) const override;
```
Renders the sphere primitive by drawing each stack as a quad strip.

**Parameters:**
- `renderer`: Reference to the renderer to use for drawing
- `states`: Additional render states (currently unused)

**Implementation Details:**
- Iterates through each stack in `m_stacks`
- Calls `renderer.Draw()` for each stack with `EPrimitiveType::QuadStrip`
- Applies the current transform to all vertices

### Color Management

#### SetColor
```cpp
virtual void SetColor(const FColor& color) override;
```
Sets the color of the sphere and updates all vertices.

**Parameters:**
- `color`: New color for the sphere

**Implementation Details:**
- Updates the base `m_color` member
- Iterates through all stacks and updates each vertex color

### Private Methods

#### GenerateVertices
```cpp
void GenerateVertices(void);
```
Regenerates the entire sphere geometry based on current tessellation parameters.

**Algorithm:**
1. Clear existing stacks
2. For each stack (latitude division):
   - Create a new vertex vector for this stack
   - For each sector (longitude division):
     - Calculate spherical coordinates (θ, φ)
     - Convert to Cartesian coordinates (x, y, z)
     - Create two vertices per sector (for quad strip)
     - Add vertices to current stack
3. Store completed stack in `m_stacks`

## Usage Examples

### Basic Sphere Creation
```cpp
// Create a default sphere (32x32 tessellation)
USpherePrimitive* sphere = new USpherePrimitive();

// Create a low-poly sphere for performance
USpherePrimitive* lowPolySphere = new USpherePrimitive(16, 16, FColor::Red);

// Create a high-detail sphere
USpherePrimitive* highDetailSphere = new USpherePrimitive(64, 64, FColor::Blue);
```

### Dynamic Tessellation
```cpp
class AdaptiveSphere {
private:
    USpherePrimitive* m_sphere;
    float m_distanceToCamera;

public:
    AdaptiveSphere() {
        m_sphere = new USpherePrimitive(32, 32);
        m_distanceToCamera = 0.0f;
    }

    void UpdateTessellation(float distance) {
        m_distanceToCamera = distance;

        // Adjust tessellation based on distance
        if (distance < 10.0f) {
            m_sphere->SetSectorCount(64);
            m_sphere->SetStackCount(64);
        } else if (distance < 50.0f) {
            m_sphere->SetSectorCount(32);
            m_sphere->SetStackCount(32);
        } else {
            m_sphere->SetSectorCount(16);
            m_sphere->SetStackCount(16);
        }
    }

    void Render(IRenderer& renderer) {
        m_sphere->Draw(renderer);
    }
};
```

### Planet System
```cpp
class Planet {
private:
    USpherePrimitive* m_sphere;
    FVector3 m_position;
    float m_radius;
    FColor m_color;
    float m_rotationSpeed;

public:
    Planet(float radius, const FColor& color, float rotationSpeed)
        : m_radius(radius), m_color(color), m_rotationSpeed(rotationSpeed) {
        m_sphere = new USpherePrimitive(48, 24, color);  // Oblate spheroid
        m_sphere->SetScale(FVector3(radius, radius * 0.8f, radius));  // Slightly flattened
    }

    void Update(float deltaTime) {
        // Rotate planet
        FRotator3D currentRotation = m_sphere->GetRotation();
        m_sphere->SetRotation(FRotator3D(
            currentRotation.pitch,
            currentRotation.yaw + m_rotationSpeed * deltaTime,
            currentRotation.roll
        ));
    }

    void SetPosition(const FVector3& position) {
        m_position = position;
        m_sphere->SetPosition(position);
    }

    void Render(IRenderer& renderer) {
        m_sphere->Draw(renderer);
    }
};

class SolarSystem {
private:
    std::vector<Planet*> m_planets;

public:
    void Initialize() {
        // Create planets with different sizes and colors
        m_planets.push_back(new Planet(2.0f, FColor::Blue, 0.5f));    // Earth-like
        m_planets.push_back(new Planet(1.5f, FColor::Red, 0.3f));     // Mars-like
        m_planets.push_back(new Planet(3.0f, FColor::Yellow, 0.1f));  // Gas giant
    }

    void Update(float deltaTime) {
        for (auto* planet : m_planets) {
            planet->Update(deltaTime);
        }
    }

    void Render(IRenderer& renderer) {
        for (auto* planet : m_planets) {
            planet->Draw(renderer);
        }
    }
};
```

### Particle System with Spheres
```cpp
class SphericalParticleSystem {
private:
    std::vector<USpherePrimitive*> m_particles;
    std::vector<FVector3> m_velocities;
    USpherePrimitive* m_particleTemplate;

public:
    SphericalParticleSystem(int maxParticles) {
        m_particleTemplate = new USpherePrimitive(8, 6, FColor::White);  // Low poly for performance

        for (int i = 0; i < maxParticles; ++i) {
            auto* particle = new USpherePrimitive(8, 6, FColor::White);
            particle->SetScale(FVector3(0.1f, 0.1f, 0.1f));  // Small particles
            particle->SetPosition(FVector3(1000, 1000, 1000));  // Inactive position
            m_particles.push_back(particle);
            m_velocities.push_back(FVector3::Zero);
        }
    }

    void SpawnParticle(const FVector3& position, const FVector3& velocity, const FColor& color) {
        // Find inactive particle
        for (size_t i = 0; i < m_particles.size(); ++i) {
            if (m_particles[i]->GetPosition().x > 999) {  // Inactive check
                m_particles[i]->SetPosition(position);
                m_particles[i]->SetColor(color);
                m_velocities[i] = velocity;
                break;
            }
        }
    }

    void Update(float deltaTime) {
        for (size_t i = 0; i < m_particles.size(); ++i) {
            FVector3 pos = m_particles[i]->GetPosition();
            if (pos.x < 999) {  // Active particle
                // Update position
                pos += m_velocities[i] * deltaTime;
                m_particles[i]->SetPosition(pos);

                // Apply gravity
                m_velocities[i].y -= 9.81f * deltaTime;

                // Deactivate if below ground
                if (pos.y < -10.0f) {
                    m_particles[i]->SetPosition(FVector3(1000, 1000, 1000));
                    m_velocities[i] = FVector3::Zero;
                }
            }
        }
    }

    void Render(IRenderer& renderer) {
        for (auto* particle : m_particles) {
            if (particle->GetPosition().x < 999) {  // Only render active particles
                particle->Draw(renderer);
            }
        }
    }
};
```

### Collision Sphere
```cpp
class CollisionSphere {
private:
    USpherePrimitive* m_visualSphere;
    FVector3 m_center;
    float m_radius;

public:
    CollisionSphere(float radius, const FColor& color) : m_radius(radius) {
        m_visualSphere = new USpherePrimitive(16, 12, FColor(color.r, color.g, color.b, 0.3f));
        m_visualSphere->SetScale(FVector3(radius, radius, radius));
    }

    void SetCenter(const FVector3& center) {
        m_center = center;
        m_visualSphere->SetPosition(center);
    }

    bool TestPoint(const FVector3& point) const {
        float distance = (point - m_center).Length();
        return distance <= m_radius;
    }

    bool TestSphere(const CollisionSphere& other) const {
        float distance = (m_center - other.m_center).Length();
        return distance <= (m_radius + other.m_radius);
    }

    void Render(IRenderer& renderer) {
        // Enable blending for transparency
        renderer.SetBlend(true);
        m_visualSphere->Draw(renderer);
        renderer.SetBlend(false);
    }
};
```

### Dynamic Sphere Morphing
```cpp
class MorphingSphere {
private:
    USpherePrimitive* m_sphere;
    float m_time;
    float m_morphSpeed;

public:
    MorphingSphere() : m_time(0.0f), m_morphSpeed(1.0f) {
        m_sphere = new USpherePrimitive(32, 32, FColor::Cyan);
    }

    void Update(float deltaTime) {
        m_time += deltaTime * m_morphSpeed;

        // Morph between sphere and cube-like shape
        float morphFactor = (std::sin(m_time) + 1.0f) * 0.5f;  // 0 to 1

        // Adjust tessellation based on morph factor
        SizeT sectors = static_cast<SizeT>(16 + morphFactor * 32);
        SizeT stacks = static_cast<SizeT>(12 + morphFactor * 32);

        m_sphere->SetSectorCount(sectors);
        m_sphere->SetStackCount(stacks);

        // Change color based on morph
        FColor morphedColor = FColor::Lerp(FColor::Cyan, FColor::Magenta, morphFactor);
        m_sphere->SetColor(morphedColor);
    }

    void Render(IRenderer& renderer) {
        m_sphere->Draw(renderer);
    }
};
```

## Mathematical Foundation

### Spherical Coordinates
The sphere is generated using parametric equations:

**Latitude (θ)**: `θ = (stack / stackCount) × π - π/2`
**Longitude (φ)**: `φ = (sector / sectorCount) × 2π`

**Cartesian Coordinates**:
```
x = cos(θ) × sin(φ)
y = sin(θ)
z = cos(θ) × cos(φ)
```

### Tessellation Parameters
- **Sector Count**: Controls horizontal resolution (longitude)
- **Stack Count**: Controls vertical resolution (latitude)
- **Total Vertices**: `stackCount × (sectorCount + 1) × 2`
- **Total Faces**: `stackCount × sectorCount × 2` (quads)

### Surface Properties
- **Radius**: 1.0 unit (pre-transform)
- **Surface Area**: `4πr² = 12.566` square units
- **Volume**: `(4/3)πr³ = 4.189` cubic units

## Performance Considerations

### Memory Usage
- **Base Overhead**: ~40 bytes (inherited + 2 SizeT + vector overhead)
- **Vertex Data**: Variable, ~4-16 KB for typical tessellation (32×32)
- **Total**: ~4-20 KB per sphere instance

### Rendering Performance
- **Draw Calls**: One per stack (typically 32 calls for 32 stacks)
- **Vertex Count**: ~2K-8K vertices for typical tessellation
- **Face Count**: ~1K-4K faces for typical tessellation
- **State Changes**: Minimal per sphere

### Optimization Strategies
1. **Adaptive Tessellation**: Reduce detail with distance
2. **LOD System**: Multiple detail levels
3. **Frustum Culling**: Only render visible spheres
4. **Impostor Textures**: Replace with billboards at distance

### Best Practices
- Use lower tessellation for distant objects
- Implement LOD system for large numbers of spheres
- Consider using cube maps for environment mapping
- Enable backface culling for solid spheres

## Diagrams

### Sphere Coordinate System
```
Spherical Coordinates:
     Y (θ = 90°)
     │
     │
     ┌─────┐ ← Equator (θ = 0°)
    ╱     ╱│
   ╱     ╱ │
  ┌─────┼──┤
  │     │  │
  │     │  │
  │     │  │
  └─────┼──┘
   ╲    ╲
    ╲    ╲
     └────┘
       X
      ╱
     Z
φ = 0° to 360°
```

### Tessellation Grid
```
Stack 0 (North Pole): ●
Stack 1: ● ● ● ● ● ● ● ● (8 sectors)
Stack 2: ● ● ● ● ● ● ● ●
...
Stack n: ● ● ● ● ● ● ● ●
Stack N (South Pole): ●

Each ● represents 2 vertices (for quad strip)
Horizontal lines = stacks (latitude)
Vertical connections = sectors (longitude)
```

### Vertex Generation Pattern
```
For each stack i (0 to stackCount):
  For each sector j (0 to sectorCount):
    // Two vertices per sector for quad strip
    θ = (i / stackCount) * π - π/2
    φ = (j / sectorCount) * 2π

    vertex1 = (cos(θ) * sin(φ), sin(θ), cos(θ) * cos(φ))
    vertex2 = (cos(θ + δθ) * sin(φ), sin(θ + δθ), cos(θ + δθ) * cos(φ))
```

## Integration with Rendering Pipeline

### Level-of-Detail System
```cpp
class SphereLODSystem {
private:
    std::vector<USpherePrimitive*> m_lodLevels;
    float m_lodDistances[4];

public:
    SphereLODSystem() {
        // Create different LOD levels
        m_lodLevels.push_back(new USpherePrimitive(64, 64));  // High detail
        m_lodLevels.push_back(new USpherePrimitive(32, 32));  // Medium detail
        m_lodLevels.push_back(new USpherePrimitive(16, 16));  // Low detail
        m_lodLevels.push_back(new USpherePrimitive(8, 8));    // Very low detail

        m_lodDistances[0] = 10.0f;
        m_lodDistances[1] = 50.0f;
        m_lodDistances[2] = 200.0f;
        m_lodDistances[3] = 1000.0f;
    }

    void Render(IRenderer& renderer, const FVector3& position, const FCamera& camera) {
        FVector3 cameraPos = camera.GetPosition();
        float distance = (position - cameraPos).Length();

        // Select appropriate LOD level
        int lodLevel = 0;
        for (int i = 0; i < 4; ++i) {
            if (distance > m_lodDistances[i]) {
                lodLevel = i;
            }
        }

        // Position and render the sphere
        m_lodLevels[lodLevel]->SetPosition(position);
        m_lodLevels[lodLevel]->Draw(renderer);
    }
};
```

### Environment Mapping
```cpp
class EnvironmentMappedSphere {
private:
    USpherePrimitive* m_sphere;
    ITexture* m_environmentMap;

public:
    EnvironmentMappedSphere(ITexture* envMap) : m_environmentMap(envMap) {
        m_sphere = new USpherePrimitive(32, 32, FColor::White);
    }

    void Render(IRenderer& renderer) {
        // Set environment mapping shader
        renderer.SetShader(m_environmentShader);

        // Bind environment map
        if (m_environmentMap) {
            m_environmentMap->Bind();
        }

        // Render sphere
        m_sphere->Draw(renderer);

        if (m_environmentMap) {
            m_environmentMap->Unbind();
        }

        // Reset shader
        renderer.SetShader(nullptr);
    }
};
```

## Troubleshooting

### Common Issues

#### Sphere Appears as Lines or Points
**Problem**: Sphere renders as wireframe or incorrect primitive type
**Solutions:**
- Verify `EPrimitiveType::QuadStrip` is being used
- Check renderer supports quad strip primitives
- Ensure proper vertex winding order

#### Incorrect Tessellation
**Problem**: Sphere appears faceted or has artifacts
**Solutions:**
- Increase sector and stack counts
- Check for minimum tessellation values (at least 3 sectors, 2 stacks)
- Verify vertex generation algorithm

#### Performance Issues with Many Spheres
**Problem**: Frame rate drops with multiple spheres
**Solutions:**
- Implement LOD system
- Use instanced rendering
- Reduce tessellation for distant spheres
- Consider using impostor billboards

#### Memory Usage Problems
**Problem**: High memory consumption with detailed spheres
**Solutions:**
- Reduce default tessellation values
- Implement dynamic tessellation
- Use object pooling for frequently created spheres

### Debug Visualization
```cpp
void DebugDrawSphere(const USpherePrimitive* sphere) {
    FVector3 center = sphere->GetPosition();
    float radius = sphere->GetScale().x;  // Assume uniform scale

    // Draw wireframe sphere approximation
    const int segments = 16;
    for (int i = 0; i < segments; ++i) {
        float angle1 = (i / (float)segments) * 2.0f * M_PI;
        float angle2 = ((i + 1) / (float)segments) * 2.0f * M_PI;

        FVector3 p1 = center + FVector3(cos(angle1) * radius, 0, sin(angle1) * radius);
        FVector3 p2 = center + FVector3(cos(angle2) * radius, 0, sin(angle2) * radius);

        DebugRenderer::DrawLine(p1, p2, FColor::Cyan);
    }

    // Draw tessellation info
    DebugRenderer::DrawText(center + FVector3(0, radius + 0.5f, 0),
        Format("Sectors: {}, Stacks: {}", sphere->GetSectorCount(), sphere->GetStackCount()),
        FColor::White);

    // Draw bounding sphere
    DebugRenderer::DrawWireframeSphere(center, radius, FColor::Yellow);
}
```

## Future Enhancements

### Potential Features
- **Texture Mapping**: UV coordinates for surface texturing
- **Normal Mapping**: Detailed surface normals and bump mapping
- **Dynamic Deformation**: Real-time shape modification
- **Particle Emission**: Sphere-based particle sources
- **Physics Integration**: Rigid body sphere collision

### API Extensions
```cpp
// Potential future methods
void USpherePrimitive::SetTexture(ITexture* texture);
void USpherePrimitive::SetUVMappingMode(ESphereUVMode mode);
void USpherePrimitive::ApplyDeformation(const std::function<FVector3(float, float)>& deformFunc);
void USpherePrimitive::SetRadius(float radius);
float USpherePrimitive::GetRadius() const;
bool USpherePrimitive::TestRayIntersection(const FRay& ray, FVector3& hitPoint, FVector3& normal) const;
```
