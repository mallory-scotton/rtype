# Renderer System Documentation

## Overview

The TKD Engine Renderer system provides a comprehensive, cross-platform graphics rendering abstraction layer built on top of SFML and OpenGL. It serves as the core component responsible for all visual output in the engine, supporting both 2D and 3D rendering with advanced state management and performance optimizations.

### Key Features

- **Dual Rendering Pipeline**: Support for both 2D (FVertex2D) and 3D (FVertex) rendering
- **Multiple Primitive Types**: Points, lines, triangles, quads with various connectivity modes
- **Advanced State Management**: Complete OpenGL state control including blending, depth testing, culling
- **Camera System Integration**: Built-in 3D camera support with perspective and orthographic projections
- **Render Target Flexibility**: Support for rendering to windows, framebuffers, and custom targets
- **Matrix Operations**: Full transformation stack management with push/pop semantics
- **Scissor Testing**: Viewport clipping and scissor rectangle support
- **Shader Integration**: Extensible shader system for custom rendering effects

### Architecture

The renderer system follows a classic interface-implementation pattern for maximum flexibility and portability:

```
┌─────────────────┐
│   IRenderer     │ ← Interface (Abstract)
│                 │
│ • Draw methods  │
│ • State control │
│ • Camera/view   │
│ • Matrix ops    │
└─────────────────┘
         ▲
         │
┌─────────────────┐
│ SFML::Renderer  │ ← Concrete Implementation
│                 │
│ • SFML backend  │
│ • OpenGL calls  │
│ • State caching │
└─────────────────┘
```

## Core Classes

### IRenderer (Interface)

The `IRenderer` class defines the abstract interface for all rendering operations in the TKD Engine. It provides a platform-independent API that can be implemented for different graphics backends.

#### Class Declaration

```cpp
namespace tkd
{
class IRenderer
{
public:
    virtual ~IRenderer() = default;

    // Core rendering methods
    virtual void Clear(const FColor& color = FColor::Black) = 0;
    virtual void Draw(const FVertex2D* vertices, UInt32 vertexCount,
                     EPrimitiveType type, const FRenderStates& states = FRenderStates()) = 0;
    virtual void Draw(const FVertex* vertices, SizeT count,
                     EPrimitiveType type, FTransform transform = FTransform::Identity) = 0;

    // View and camera management
    virtual void SetView(const FView& view) = 0;
    virtual const FView& GetView(void) const = 0;
    virtual FView GetDefaultView(void) const = 0;
    virtual void SetCamera(const FCamera& camera) = 0;
    virtual const FCamera& GetCamera(void) const = 0;

    // Render target management
    virtual void SetRenderTarget(IRenderTarget* target = nullptr) = 0;
    virtual FVector2u GetRenderTargetSize(void) const = 0;

    // Frame management
    virtual void BeginFrame(void) = 0;
    virtual void EndFrame(void) = 0;

    // OpenGL state management
    virtual void SetDepthTest(bool enable) = 0;
    virtual void SetBlend(bool enable) = 0;
    virtual void SetCullFace(bool enable) = 0;
    // ... additional state methods
};
}
```

#### Key Methods

##### Rendering Methods

- **`Clear(const FColor& color = FColor::Black)`**
  - Clears the current render target with the specified color
  - Default color is black
  - Typically called at the beginning of each frame

- **`Draw(const FVertex2D* vertices, UInt32 vertexCount, EPrimitiveType type, const FRenderStates& states)`**
  - Renders 2D vertices using the specified primitive type
  - Supports texture mapping and blending through render states
  - Used for UI elements, sprites, and 2D graphics

- **`Draw(const FVertex* vertices, SizeT count, EPrimitiveType type, FTransform transform)`**
  - Renders 3D vertices with optional transformation
  - Supports lighting, depth testing, and 3D transformations
  - Used for 3D models, primitives, and complex geometry

- **`Draw(const IDrawable& drawable, FRenderStates states)`**
  - Renders drawable objects through the IDrawable interface
  - Provides a unified way to render complex objects
  - Automatically calls the drawable's Draw method

##### View and Camera Management

- **`SetView(const FView& view)` / `GetView()`**
  - Controls the 2D viewing area and transformation
  - Affects how 2D coordinates are mapped to screen space
  - Used for camera movement, zooming, and UI layout

- **`SetCamera(const FCamera& camera)` / `GetCamera()`**
  - Manages the 3D camera position, orientation, and projection
  - Controls the 3D viewing frustum and perspective
  - Essential for 3D scene rendering

##### Render Target Management

- **`SetRenderTarget(IRenderTarget* target = nullptr)`**
  - Sets the active render target (window, framebuffer, etc.)
  - `nullptr` targets the default window
  - Enables off-screen rendering and multi-pass effects

##### Frame Management

- **`BeginFrame()` / `EndFrame()`**
  - Frame boundary markers for proper rendering setup/cleanup
  - `BeginFrame()` initializes OpenGL states and clears buffers
  - `EndFrame()` presents the rendered frame and handles vsync

##### OpenGL State Management

The renderer provides comprehensive control over OpenGL rendering states:

- **Depth Management**: `SetDepthTest()`, `SetDepthFunc()`, `SetDepthMask()`
- **Blending**: `SetBlend()`, `SetBlendFunc()`
- **Face Culling**: `SetCullFace()`, `SetCullFaceMode()`, `SetFrontFace()`
- **Polygon Mode**: `SetPolygonMode()` (point, line, fill)
- **Matrix Operations**: `PushMatrix()`, `PopMatrix()`, `Translate()`, `Rotate()`, `Scale()`
- **Viewport**: `SetViewport()`, `SetMatrixMode()`

### SFML::Renderer (Implementation)

The `SFML::Renderer` class provides the concrete SFML-based implementation of the `IRenderer` interface. It translates high-level rendering commands into SFML and OpenGL calls.

#### Class Declaration

```cpp
namespace tkd::SFML
{
class Renderer : public IRenderer
{
private:
    sf::RenderWindow* m_window;
    sf::RenderTarget* m_currentTarget;
    FView m_currentView;
    FCamera m_camera;
    std::stack<FRectangle> m_scissorStack;

public:
    Renderer(IWindow* window);
    virtual ~Renderer() = default;

    // Implementation of all IRenderer methods
    virtual void Clear(const FColor& color = FColor::Black) override;
    virtual void Draw(const FVertex2D* vertices, UInt32 vertexCount,
                     EPrimitiveType type, const FRenderStates& states = FRenderStates()) override;
    // ... all other interface methods
};
}
```

#### Key Implementation Details

- **SFML Integration**: Uses `sf::RenderWindow` and `sf::RenderTarget` for window management
- **OpenGL State Caching**: Maintains current OpenGL states to minimize redundant calls
- **Matrix Stack**: Implements full OpenGL matrix stack semantics
- **Scissor Management**: Stack-based scissor rectangle management
- **Camera Integration**: Direct OpenGL camera setup using `gluLookAt()`

## Enumerations

### EPrimitiveType

Defines the geometric primitives that can be rendered:

```cpp
enum class EPrimitiveType : UInt8
{
    Points,           // Individual points
    Lines,            // Line segments (2 vertices each)
    LineStrip,        // Connected line strip
    Triangles,        // Individual triangles (3 vertices each)
    TriangleStrip,    // Triangle strip
    TriangleFan,      // Triangle fan
    Quads,            // Quadrilaterals (4 vertices each)
    QuadStrip         // Quad strip
};
```

### EBlendMode

Controls how colors are blended during rendering:

```cpp
enum class EBlendMode : UInt8
{
    None,       // No blending (opaque)
    Alpha,      // Standard alpha blending: src_alpha * src + (1-src_alpha) * dst
    Add,        // Additive blending: src + dst
    Multiply,   // Multiplicative blending: src * dst
    Screen,     // Screen blending: 1 - (1-src) * (1-dst)
    Subtract,   // Subtractive blending: dst - src
    Replace     // Replace blending: src (no blending)
};
```

### EBlendFactor

OpenGL blend factor constants for advanced blending control:

```cpp
enum class EBlendFactor : UInt16
{
    Zero = GL_ZERO,
    One = GL_ONE,
    SrcColor = GL_SRC_COLOR,
    OneMinusSrcColor = GL_ONE_MINUS_SRC_COLOR,
    SrcAlpha = GL_SRC_ALPHA,
    OneMinusSrcAlpha = GL_ONE_MINUS_SRC_ALPHA,
    // ... additional factors
};
```

### EDepthFunc

Depth comparison functions for depth testing:

```cpp
enum class EDepthFunc : UInt16
{
    Never = GL_NEVER,        // Never passes
    Less = GL_LESS,          // Passes if fragment depth < buffer depth
    Equal = GL_EQUAL,        // Passes if fragment depth == buffer depth
    LessEqual = GL_LEQUAL,   // Passes if fragment depth <= buffer depth
    Greater = GL_GREATER,    // Passes if fragment depth > buffer depth
    NotEqual = GL_NOTEQUAL,  // Passes if fragment depth != buffer depth
    GreaterEqual = GL_GEQUAL,// Passes if fragment depth >= buffer depth
    Always = GL_ALWAYS       // Always passes
};
```

### ECullFaceMode

Face culling modes for performance optimization:

```cpp
enum class ECullFaceMode : UInt16
{
    Front = GL_FRONT,           // Cull front faces
    Back = GL_BACK,             // Cull back faces (default)
    FrontAndBack = GL_FRONT_AND_BACK  // Cull both faces
};
```

### EPolygonMode

Polygon rendering modes for debugging and wireframe rendering:

```cpp
enum class EPolygonMode : UInt16
{
    Point = GL_POINT,  // Render vertices as points
    Line = GL_LINE,    // Render as wireframe
    Fill = GL_FILL     // Render filled polygons (default)
};
```

## Supporting Structures

### FRenderStates

Encapsulates all rendering state parameters for a draw operation:

```cpp
class FRenderStates
{
public:
    EBlendMode blendMode;      // Blending mode
    FTransform2D transform;    // 2D transformation
    const ITexture* texture;   // Texture to apply
    const IShader* shader;     // Shader program

    FRenderStates(EBlendMode blend = EBlendMode::Alpha);
};
```

### FView

Represents a 2D camera/viewport for 2D rendering:

```cpp
class FView
{
private:
    FVector2 m_center;
    FVector2 m_size;
    float m_rotation;
    FRectangle m_viewport;

public:
    FView(void);
    FView(const FRectangle& rectangle);
    FView(const FVector2& center, const FVector2& size);

    void SetCenter(float x, float y);
    void SetCenter(const FVector2& center);
    void SetSize(float width, float height);
    void SetSize(const FVector2& size);
    void SetRotation(float angle);
    void SetViewport(const FRectangle& viewport);

    const FVector2& GetCenter(void) const;
    const FVector2& GetSize(void) const;
    float GetRotation(void) const;
    const FRectangle& GetViewport(void) const;
    FMatrix4 GetTransform(void) const;
    FMatrix4 GetInverseTransform(void) const;
};
```

### FCamera

Represents a 3D camera with position, orientation, and projection parameters:

```cpp
class FCamera
{
public:
    FVector3 position;     // Camera position in world space
    FVector3 front;        // Forward direction vector
    FVector3 up;           // Up direction vector
    FVector3 right;        // Right direction vector
    float yaw;             // Yaw angle in degrees
    float pitch;           // Pitch angle in degrees
    float fov;             // Field of view in degrees
    float aspectRatio;     // Aspect ratio (width/height)
    float nearPlane;       // Near clipping plane
    float farPlane;        // Far clipping plane

    FCamera(void);
    void UpdateCameraVectors(void);
    FMatrix4 GetViewMatrix(void) const;
    FMatrix4 GetProjectionMatrix(void) const;
};
```

## Usage Examples

### Basic Rendering Setup

```cpp
// Initialize renderer with window
IWindow* window = graphicsFactory->CreateWindow("Game Window", 800, 600);
IRenderer* renderer = graphicsFactory->CreateRenderer(window);

// Main render loop
while (window->IsOpen())
{
    // Begin frame
    renderer->BeginFrame();

    // Clear screen
    renderer->Clear(FColor::CornflowerBlue);

    // Set up camera for 3D rendering
    FCamera camera;
    camera.position = FVector3(0.0f, 0.0f, 5.0f);
    camera.UpdateCameraVectors();
    renderer->SetCamera(camera);

    // Draw 3D geometry
    std::vector<FVertex> triangle = {
        {FVector3(-1.0f, -1.0f, 0.0f), FColor::Red, FVector2(0.0f, 0.0f)},
        {FVector3( 1.0f, -1.0f, 0.0f), FColor::Green, FVector2(1.0f, 0.0f)},
        {FVector3( 0.0f,  1.0f, 0.0f), FColor::Blue, FVector2(0.5f, 1.0f)}
    };
    renderer->Draw(triangle.data(), triangle.size(), EPrimitiveType::Triangles);

    // End frame
    renderer->EndFrame();
}
```

### 2D Sprite Rendering

```cpp
// Create 2D vertices for a quad
FVertex2D quadVertices[4] = {
    {FVector2f(0.0f, 0.0f), FColor::White, FVector2f(0.0f, 0.0f)},  // Bottom-left
    {FVector2f(100.0f, 0.0f), FColor::White, FVector2f(1.0f, 0.0f)}, // Bottom-right
    {FVector2f(100.0f, 100.0f), FColor::White, FVector2f(1.0f, 1.0f)}, // Top-right
    {FVector2f(0.0f, 100.0f), FColor::White, FVector2f(0.0f, 1.0f)}   // Top-left
};

// Set up render states with texture
FRenderStates states;
states.texture = texture;

// Draw the quad
renderer->Draw(quadVertices, 4, EPrimitiveType::Quads, states);
```

### Advanced State Management

```cpp
// Enable depth testing for 3D rendering
renderer->SetDepthTest(true);
renderer->SetDepthFunc(EDepthFunc::Less);

// Set up alpha blending
renderer->SetBlend(true);
renderer->SetBlendFunc(EBlendFactor::SrcAlpha, EBlendFactor::OneMinusSrcAlpha);

// Enable backface culling
renderer->SetCullFace(true);
renderer->SetCullFaceMode(ECullFaceMode::Back);

// Set up matrix transformations
renderer->PushMatrix();
renderer->Translate(10.0f, 20.0f, 30.0f);
renderer->Rotate(45.0f, 0.0f, 1.0f, 0.0f);
renderer->Scale(2.0f, 2.0f, 2.0f);

// Draw transformed geometry
renderer->Draw(vertices, vertexCount, EPrimitiveType::Triangles);

renderer->PopMatrix(); // Restore previous transformation
```

### Render Target Switching

```cpp
// Create a framebuffer render target
IRenderTarget* framebuffer = graphicsFactory->CreateRenderTarget(512, 512);

// Render to framebuffer
renderer->SetRenderTarget(framebuffer);
renderer->Clear(FColor::Black);
// ... draw scene to framebuffer ...

// Switch back to window
renderer->SetRenderTarget(nullptr);
// Use framebuffer texture in main scene
FRenderStates states;
states.texture = framebuffer->GetTexture();
renderer->Draw(quadVertices, 4, EPrimitiveType::Quads, states);
```

### Scissor Testing

```cpp
// Set up a scissor rectangle for UI clipping
FRectangle scissorRect(100.0f, 100.0f, 200.0f, 150.0f);
renderer->PushScissorTest(scissorRect);

// Draw UI elements (only visible within scissor rectangle)
renderer->Draw(uiVertices, uiVertexCount, EPrimitiveType::Triangles);

renderer->PopScissorTest(); // Restore previous scissor state
```

## Architecture Diagrams

### Rendering Pipeline

```
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│   Application   │───▶│    IRenderer     │───▶│ SFML::Renderer  │
│                 │    │                  │    │                 │
│ • Game Logic    │    │ • Draw()         │    │ • OpenGL Calls  │
│ • Scene Graph   │    │ • State Mgmt     │    │ • State Caching │
│ • Update Loop   │    │ • Camera/View    │    │ • SFML Backend  │
└─────────────────┘    └──────────────────┘    └─────────────────┘
                                                        │
                                                        ▼
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│   OpenGL        │    │     SFML         │    │   Window        │
│   Commands      │    │   Abstraction    │    │   System        │
└─────────────────┘    └──────────────────┘    └─────────────────┘
```

### State Management Flow

```
┌─────────────────┐
│ BeginFrame()    │
└─────────────────┘
         │
         ▼
┌─────────────────┐    ┌─────────────────┐
│ Clear Screen    │───▶│ Set Viewport    │
└─────────────────┘    └─────────────────┘
         │
         ▼
┌─────────────────┐    ┌─────────────────┐
│ Set Camera      │───▶│ Configure      │
│ & View           │    │ OpenGL State   │
└─────────────────┘    └─────────────────┘
         │
         ▼
┌─────────────────┐    ┌─────────────────┐
│ Draw Objects    │───▶│ Update Matrix   │
│                 │    │ Stack           │
└─────────────────┘    └─────────────────┘
         │
         ▼
┌─────────────────┐
│ EndFrame()      │
│ Present         │
└─────────────────┘
```

### Vertex Processing Pipeline

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Vertex Data   │───▶│  Primitive      │───▶│  OpenGL         │
│   (FVertex/     │    │  Assembly       │    │  Processing     │
│    FVertex2D)   │    │                 │    │                 │
└─────────────────┘    └─────────────────┘    └─────────────────┘
         │                       │                       │
         │                       │                       │
         ▼                       ▼                       ▼
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│ Position        │    │ Points/Lines/   │    │ Rasterization   │
│ Color           │    │ Triangles       │    │                 │
│ UV Coordinates  │    │ Quads           │    │                 │
└─────────────────┘    └─────────────────┘    └─────────────────┘
```

## Performance Considerations

### State Change Minimization

- **Batch Rendering**: Group draw calls with similar states together
- **State Sorting**: Sort draw calls by render state to minimize changes
- **Texture Atlasing**: Combine multiple textures to reduce texture switches

### Memory Management

- **Vertex Buffer Objects**: Use VBOs for static geometry
- **Index Buffers**: Use indexed rendering for shared vertices
- **Texture Compression**: Use compressed texture formats

### OpenGL Best Practices

- **Minimize glGet* Calls**: Cache OpenGL state internally
- **Use Vertex Arrays**: Prefer vertex arrays over immediate mode
- **Proper Matrix Management**: Use matrix stack efficiently
- **Scissor Optimization**: Use scissor tests to limit overdraw

## Error Handling

The renderer system includes comprehensive error checking:

- **Parameter Validation**: Checks for null pointers and invalid parameters
- **OpenGL Error Checking**: Validates OpenGL state changes
- **Resource Validation**: Ensures textures and shaders are valid
- **Context Validation**: Verifies rendering context is active

## Threading Considerations

- **Single Threaded**: Renderer operations are not thread-safe
- **Context Binding**: OpenGL context must be current for the calling thread
- **Resource Sharing**: Textures and shaders can be shared across contexts

## Platform-Specific Notes

### Windows
- Uses WGL for OpenGL context management
- Supports DirectX interoperability through SFML

### Linux
- Uses GLX for OpenGL context management
- Native X11 integration

### macOS
- Uses CGL for OpenGL context management
- Native Cocoa integration

## Future Extensions

The renderer architecture supports future enhancements:

- **Vulkan Backend**: Planned Vulkan implementation
- **Metal Backend**: Planned Metal implementation for Apple platforms
- **Compute Shaders**: GPU compute shader support
- **Ray Tracing**: Hardware-accelerated ray tracing
- **Multi-GPU**: Support for multiple GPU configurations

## Troubleshooting

### Common Issues

1. **Black Screen**: Check if BeginFrame()/EndFrame() are properly called
2. **No 3D Rendering**: Verify camera is set and depth testing is enabled
3. **Performance Issues**: Check for excessive state changes or large vertex buffers
4. **Memory Leaks**: Ensure render targets and textures are properly released

### Debug Features

- **Wireframe Mode**: Set polygon mode to line for debugging geometry
- **State Logging**: Enable verbose logging to track state changes
- **Performance Profiling**: Built-in frame time and draw call counting

---

*This documentation covers the complete TKD Engine Renderer system as of the current version. For the latest updates and additional examples, refer to the engine's source code and test suites.*
