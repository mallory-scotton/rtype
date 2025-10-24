# Graphics Factory System

## Overview

The **`Graphics Factory`** system in the TKD Engine implements the Abstract Factory design pattern to provide a unified interface for creating graphics resources across different rendering backends. It abstracts the creation of textures, shaders, windows, renderers, and render targets, enabling the engine to support multiple graphics APIs while maintaining a consistent API for game developers.

The factory pattern allows the engine to decouple graphics resource creation from specific implementations, making it easier to add new graphics backends or switch between them without changing game code.

## Purpose

The Graphics Factory system is designed to:
- Provide a unified interface for graphics resource creation
- Abstract platform-specific graphics implementations
- Enable backend switching without code changes
- Support multiple input formats for resource loading
- Centralize graphics object lifecycle management
- Enable dependency injection for testing and modularity
- Support both client and server configurations

## Core Architecture

### Abstract Factory Pattern

```cpp
namespace tkd
{
    // Abstract interface
    class IGraphicsFactory
    {
    public:
        virtual std::unique_ptr<ITexture> CreateTexture(...) const = 0;
        virtual std::unique_ptr<IShader> CreateShader(...) const = 0;
        virtual std::unique_ptr<IWindow> CreateWindow(...) const = 0;
        // ... other factory methods
    };

    // Concrete implementation (SFML)
    namespace SFML
    {
        class GraphicsFactory : public IGraphicsFactory
        {
            // SFML-specific implementations
        };
    }
}
```

### Key Design Principles

1. **Abstraction**: Platform-specific details hidden behind interfaces
2. **Polymorphism**: Different backends implement the same interface
3. **Resource Management**: RAII-based resource lifecycle
4. **Multiple Input Formats**: Support for files, memory, assets
5. **Type Safety**: Template-free interface with runtime polymorphism
6. **Extensibility**: Easy to add new resource types and backends

## IGraphicsFactory Interface

The `IGraphicsFactory` interface defines the contract that all graphics factory implementations must follow.

### Interface Definition

```cpp
namespace tkd
{
    class IGraphicsFactory
    {
    public:
        virtual ~IGraphicsFactory() = default;

        // Texture creation methods
        virtual std::unique_ptr<ITexture> CreateTexture(void) const = 0;
        virtual std::unique_ptr<ITexture> CreateTexture(
            const FilePath& filePath, const FRectanglei& area = FRectanglei::Zero
        ) const = 0;
        // ... additional overloads

        // Renderer creation
        virtual std::unique_ptr<IRenderer> CreateRenderer(IWindow* window) const = 0;

        // Window creation
        virtual std::unique_ptr<IWindow> CreateWindow(...) const = 0;

        // Shader creation methods (multiple overloads)
        virtual std::unique_ptr<IShader> CreateShader(void) const = 0;
        // ... many shader creation overloads

        // Render target creation
        virtual std::unique_ptr<IRenderTarget> CreateRenderTarget(void) const = 0;
    };
}
```

### Pure Virtual Methods

All methods in `IGraphicsFactory` are pure virtual, requiring concrete implementations to provide functionality.

## SFML Graphics Factory Implementation

### Class Overview

```cpp
namespace tkd::SFML
{
    class GraphicsFactory : public IGraphicsFactory
    {
    public:
        ~GraphicsFactory() = default;

        // Implementation of all IGraphicsFactory methods
    };
}
```

The SFML implementation provides concrete implementations using the SFML library for all graphics operations.

### Compilation Guards

```cpp
#if TKD_ENGINE_CLIENT
// Graphics factory implementation
#endif
```

The factory is only compiled on the client side, as servers don't need graphics capabilities.

## Texture Creation Methods

### CreateTexture Overloads

#### Empty Texture
```cpp
virtual std::unique_ptr<ITexture> CreateTexture(void) const override;
```
- **Purpose**: Create an uninitialized texture
- **Usage**: For procedural texture generation or delayed loading
- **Implementation**: Returns `std::make_unique<SFML::Texture>()`

#### Texture from File
```cpp
virtual std::unique_ptr<ITexture> CreateTexture(
    const FilePath& filePath, const FRectanglei& area = FRectanglei::Zero
) const override;
```
- **Parameters**:
  - `filePath`: Path to image file (PNG, JPEG, etc.)
  - `area`: Rectangular region to load (default: entire image)
- **Usage**: Load textures from disk files
- **Implementation**: `std::make_unique<SFML::Texture>(filePath, area)`

#### Texture from Raw Data
```cpp
virtual std::unique_ptr<ITexture> CreateTexture(
    const void* data, SizeT size, const FRectanglei& area = FRectanglei::Zero
) const override;
```
- **Parameters**:
  - `data`: Pointer to raw image data
  - `size`: Size of data in bytes
  - `area`: Region to load from data
- **Usage**: Load textures from memory buffers
- **Implementation**: `std::make_unique<SFML::Texture>(data, size, area)`

#### Texture from Byte Vector
```cpp
virtual std::unique_ptr<ITexture> CreateTexture(
    const TVector<Byte>& bytes, const FRectanglei& area = FRectanglei::Zero
) const override;
```
- **Parameters**:
  - `bytes`: Vector containing image data
  - `area`: Region to load from vector
- **Usage**: Load textures from byte arrays
- **Implementation**: `std::make_unique<SFML::Texture>(bytes, area)`

#### Texture from Asset
```cpp
virtual std::unique_ptr<ITexture> CreateTexture(
    const UAsset& asset, const FRectanglei& area = FRectanglei::Zero
) const override;
```
- **Parameters**:
  - `asset`: Engine asset containing texture data
  - `area`: Region to load from asset
- **Usage**: Load textures from engine's asset system
- **Implementation**: `std::make_unique<SFML::Texture>(asset, area)`

## Renderer Creation

### CreateRenderer
```cpp
virtual std::unique_ptr<IRenderer> CreateRenderer(IWindow* window) const override;
```
- **Parameter**: `window` - Window to attach renderer to
- **Returns**: Renderer bound to the specified window
- **Usage**: Create rendering context for a window
- **Implementation**: `std::make_unique<SFML::Renderer>(window)`

## Window Creation

### CreateWindow
```cpp
virtual std::unique_ptr<IWindow> CreateWindow(
    const FString& title,
    bool openDefault = true,
    const FVector2i& position = FVector2i::Zero,
    const FVector2u& dimension = FVector2u(1280, 720),
    EWindowState state = EWindowState::Windowed
) const override;
```
- **Parameters**:
  - `title`: Window title string
  - `openDefault`: Whether to show window immediately (default: true)
  - `position`: Screen position (default: (0,0))
  - `dimension`: Window size (default: 1280x720)
  - `state`: Window state (Windowed, Fullscreen, etc.)
- **Usage**: Create application windows
- **Implementation**: `std::make_unique<SFML::Window>(...)`

## Shader Creation Methods

The factory provides extensive shader creation capabilities with multiple overloads for different input formats and shader combinations.

### Shader Types

```cpp
enum class EShaderType {
    Vertex,     // Vertex shader
    Geometry,   // Geometry shader
    Fragment    // Fragment (pixel) shader
};
```

### Empty Shader
```cpp
virtual std::unique_ptr<IShader> CreateShader(void) const override;
```
- **Purpose**: Create uninitialized shader for manual setup
- **Implementation**: `std::make_unique<SFML::Shader>()`

### Single Shader from File
```cpp
virtual std::unique_ptr<IShader> CreateShader(
    const FilePath& filePath, EShaderType type
) const override;
```
- **Parameters**:
  - `filePath`: Path to shader source file
  - `type`: Shader type (Vertex, Geometry, Fragment)
- **Usage**: Load individual shader stages from files

### Vertex + Fragment from Files
```cpp
virtual std::unique_ptr<IShader> CreateShader(
    const FilePath& vertexPath, const FilePath& fragmentPath
) const override;
```
- **Parameters**:
  - `vertexPath`: Vertex shader file path
  - `fragmentPath`: Fragment shader file path
- **Usage**: Load complete shader program from separate files

### Vertex + Geometry + Fragment from Files
```cpp
virtual std::unique_ptr<IShader> CreateShader(
    const FilePath& vertexPath,
    const FilePath& geometryPath,
    const FilePath& fragmentPath
) const override;
```
- **Parameters**:
  - `vertexPath`: Vertex shader file path
  - `geometryPath`: Geometry shader file path
  - `fragmentPath`: Fragment shader file path
- **Usage**: Load complete shader pipeline with geometry shader

### Shader from Source String
```cpp
virtual std::unique_ptr<IShader> CreateShader(
    const FString& shader, EShaderType type
) const override;
```
- **Parameters**:
  - `shader`: Shader source code as string
  - `type`: Shader type
- **Usage**: Create shaders from string literals or generated code

### Vertex + Fragment from Strings
```cpp
virtual std::unique_ptr<IShader> CreateShader(
    const FString& vertex, const FString& fragment
) const override;
```
- **Parameters**:
  - `vertex`: Vertex shader source string
  - `fragment`: Fragment shader source string
- **Usage**: Create complete programs from string sources

### Vertex + Geometry + Fragment from Strings
```cpp
virtual std::unique_ptr<IShader> CreateShader(
    const FString& vertex, const FString& geometry, const FString& fragment
) const override;
```
- **Parameters**:
  - `vertex`: Vertex shader source string
  - `geometry`: Geometry shader source string
  - `fragment`: Fragment shader source string
- **Usage**: Create full pipeline from string sources

### Shader from Byte Vector
```cpp
virtual std::unique_ptr<IShader> CreateShader(
    const TVector<Byte>& shader, EShaderType type
) const override;
```
- **Parameters**:
  - `shader`: Shader source as byte vector
  - `type`: Shader type
- **Usage**: Load shaders from binary data or compressed sources

### Vertex + Fragment from Byte Vectors
```cpp
virtual std::unique_ptr<IShader> CreateShader(
    const TVector<Byte>& vertex, const TVector<Byte>& fragment
) const override;
```
- **Parameters**:
  - `vertex`: Vertex shader as byte vector
  - `fragment`: Fragment shader as byte vector
- **Usage**: Create programs from binary shader data

### Vertex + Geometry + Fragment from Byte Vectors
```cpp
virtual std::unique_ptr<IShader> CreateShader(
    const TVector<Byte>& vertex,
    const TVector<Byte>& geometry,
    const TVector<Byte>& fragment
) const override;
```
- **Parameters**:
  - `vertex`: Vertex shader as byte vector
  - `geometry`: Geometry shader as byte vector
  - `fragment`: Fragment shader as byte vector
- **Usage**: Create full pipeline from binary data

### Shader from Asset
```cpp
virtual std::unique_ptr<IShader> CreateShader(
    const UAsset& asset, EShaderType type
) const override;
```
- **Parameters**:
  - `asset`: Asset containing shader source
  - `type`: Shader type
- **Usage**: Load shaders from engine asset system

### Vertex + Fragment from Assets
```cpp
virtual std::unique_ptr<IShader> CreateShader(
    const UAsset& vertex, const UAsset& fragment
) const override;
```
- **Parameters**:
  - `vertex`: Asset containing vertex shader
  - `fragment`: Asset containing fragment shader
- **Usage**: Create programs from asset system

### Vertex + Geometry + Fragment from Assets
```cpp
virtual std::unique_ptr<IShader> CreateShader(
    const UAsset& vertex, const UAsset& geometry, const UAsset& fragment
) const override;
```
- **Parameters**:
  - `vertex`: Asset containing vertex shader
  - `geometry`: Asset containing geometry shader
  - `fragment`: Asset containing fragment shader
- **Usage**: Create full pipeline from assets

## Render Target Creation

### CreateRenderTarget
```cpp
virtual std::unique_ptr<IRenderTarget> CreateRenderTarget(void) const override;
```
- **Purpose**: Create off-screen rendering target
- **Usage**: Render to texture, post-processing effects
- **Implementation**: `std::make_unique<SFML::RenderTarget>()`

## Usage Examples

### Basic Graphics Setup
```cpp
// Create graphics factory (SFML implementation)
auto graphicsFactory = std::make_unique<SFML::GraphicsFactory>();

// Create window
auto window = graphicsFactory->CreateWindow(
    "My Game",
    true,  // open immediately
    FVector2i(100, 100),  // position
    FVector2u(1280, 720), // size
    EWindowState::Windowed
);

// Create renderer for the window
auto renderer = graphicsFactory->CreateRenderer(window.get());

// Create render target for off-screen rendering
auto renderTarget = graphicsFactory->CreateRenderTarget();
```

### Texture Loading Examples
```cpp
// Load texture from file
auto texture1 = graphicsFactory->CreateTexture("assets/textures/player.png");

// Load texture from file with specific region
FRectanglei region(0, 0, 64, 64);  // top-left 64x64 pixels
auto texture2 = graphicsFactory->CreateTexture("assets/tileset.png", region);

// Load texture from memory (e.g., from network or procedural generation)
std::vector<unsigned char> imageData = GenerateProceduralTexture();
auto texture3 = graphicsFactory->CreateTexture(
    imageData.data(),
    imageData.size()
);

// Load texture from asset system
UAsset textureAsset = assetManager.LoadAsset("player_texture");
auto texture4 = graphicsFactory->CreateTexture(textureAsset);
```

### Shader Creation Examples
```cpp
// Create shader from separate files
auto shader1 = graphicsFactory->CreateShader(
    "shaders/vertex.glsl",
    "shaders/fragment.glsl"
);

// Create shader with geometry stage
auto shader2 = graphicsFactory->CreateShader(
    "shaders/vertex.glsl",
    "shaders/geometry.glsl",
    "shaders/fragment.glsl"
);

// Create shader from source strings (for runtime generation)
const char* vertexSource = R"(
    void main() {
        gl_Position = projection * view * model * vec4(position, 1.0);
    }
)";

const char* fragmentSource = R"(
    void main() {
        fragColor = vec4(1.0, 0.0, 0.0, 1.0);  // Red color
    }
)";

auto shader3 = graphicsFactory->CreateShader(vertexSource, fragmentSource);

// Create shader from assets
UAsset vertexAsset = assetManager.LoadAsset("vertex_shader");
UAsset fragmentAsset = assetManager.LoadAsset("fragment_shader");
auto shader4 = graphicsFactory->CreateShader(vertexAsset, fragmentAsset);
```

### Advanced Usage Patterns
```cpp
class GraphicsManager
{
private:
    std::unique_ptr<IGraphicsFactory> factory;
    std::unordered_map<std::string, std::unique_ptr<ITexture>> textures;
    std::unordered_map<std::string, std::unique_ptr<IShader>> shaders;

public:
    GraphicsManager(std::unique_ptr<IGraphicsFactory> gfxFactory)
        : factory(std::move(gfxFactory))
    {}

    ITexture* LoadTexture(const std::string& name, const FilePath& path) {
        auto texture = factory->CreateTexture(path);
        textures[name] = std::move(texture);
        return textures[name].get();
    }

    IShader* LoadShader(const std::string& name,
                       const FilePath& vertexPath,
                       const FilePath& fragmentPath) {
        auto shader = factory->CreateShader(vertexPath, fragmentPath);
        shaders[name] = std::move(shader);
        return shaders[name].get();
    }

    ITexture* GetTexture(const std::string& name) const {
        auto it = textures.find(name);
        return it != textures.end() ? it->second.get() : nullptr;
    }

    IShader* GetShader(const std::string& name) const {
        auto it = shaders.find(name);
        return it != shaders.end() ? it->second.get() : nullptr;
    }
};

// Usage
auto graphics = std::make_unique<GraphicsManager>(
    std::make_unique<SFML::GraphicsFactory>()
);

ITexture* playerTex = graphics->LoadTexture("player", "player.png");
IShader* basicShader = graphics->LoadShader("basic",
                                           "basic.vert",
                                           "basic.frag");
```

### Backend Switching Example
```cpp
// Abstract factory allows easy backend switching
std::unique_ptr<IGraphicsFactory> CreateGraphicsFactory(GraphicsBackend backend) {
    switch (backend) {
        case GraphicsBackend::SFML:
            return std::make_unique<SFML::GraphicsFactory>();
        case GraphicsBackend::OpenGL:
            return std::make_unique<OpenGL::GraphicsFactory>();
        case GraphicsBackend::Vulkan:
            return std::make_unique<Vulkan::GraphicsFactory>();
        case GraphicsBackend::DirectX:
            return std::make_unique<DirectX::GraphicsFactory>();
        default:
            throw std::runtime_error("Unsupported graphics backend");
    }
}

// Game code remains unchanged
class Game {
private:
    std::unique_ptr<IGraphicsFactory> graphicsFactory;
    std::unique_ptr<IWindow> window;
    std::unique_ptr<IRenderer> renderer;

public:
    Game(GraphicsBackend backend) {
        graphicsFactory = CreateGraphicsFactory(backend);
        window = graphicsFactory->CreateWindow("Game", true);
        renderer = graphicsFactory->CreateRenderer(window.get());
    }

    // Game logic uses abstract interfaces
    void Render() {
        renderer->BeginFrame();
        // Render game objects
        renderer->EndFrame();
    }
};
```

### Resource Management Integration
```cpp
class ResourceManager
{
private:
    std::unique_ptr<IGraphicsFactory> graphicsFactory;
    std::unordered_map<ResourceId, std::unique_ptr<ITexture>> textures;
    std::unordered_map<ResourceId, std::unique_ptr<IShader>> shaders;

public:
    template<typename... Args>
    ITexture* CreateTexture(Args&&... args) {
        auto texture = graphicsFactory->CreateTexture(std::forward<Args>(args)...);
        ResourceId id = GenerateResourceId();
        textures[id] = std::move(texture);
        return textures[id].get();
    }

    template<typename... Args>
    IShader* CreateShader(Args&&... args) {
        auto shader = graphicsFactory->CreateShader(std::forward<Args>(args)...);
        ResourceId id = GenerateResourceId();
        shaders[id] = std::move(shader);
        return shaders[id].get();
    }

    void Cleanup() {
        textures.clear();
        shaders.clear();
    }
};
```

## Performance Considerations

### Memory Management
- **RAII Pattern**: Resources automatically managed by unique_ptr
- **Factory Overhead**: Minimal cost for object creation
- **Backend Selection**: Choose appropriate backend for target platform

### Resource Loading
- **File I/O**: Load from disk when possible for better caching
- **Memory Loading**: Use for procedural content or network resources
- **Asset System**: Leverage engine's asset management for optimization

### Shader Compilation
- **Compile Time**: Shaders compiled during creation
- **Caching**: Consider caching compiled shaders
- **Optimization**: Use appropriate shader precision and features

## Integration with Engine Systems

### Asset System Integration
```cpp
// Graphics factory works with UAsset system
class AssetLoader
{
public:
    ITexture* LoadTextureAsset(const std::string& assetName) {
        UAsset asset = assetManager.LoadAsset(assetName);
        if (asset.IsValid()) {
            return graphicsFactory->CreateTexture(asset).release();
        }
        return nullptr;
    }

    IShader* LoadShaderAsset(const std::string& vertexAsset,
                           const std::string& fragmentAsset) {
        UAsset vertex = assetManager.LoadAsset(vertexAsset);
        UAsset fragment = assetManager.LoadAsset(fragmentAsset);

        if (vertex.IsValid() && fragment.IsValid()) {
            return graphicsFactory->CreateShader(vertex, fragment).release();
        }
        return nullptr;
    }
};
```

### Renderer Integration
```cpp
class GameRenderer
{
private:
    std::unique_ptr<IGraphicsFactory> factory;
    std::unique_ptr<IRenderer> renderer;
    std::unique_ptr<IRenderTarget> renderTarget;

public:
    GameRenderer(std::unique_ptr<IGraphicsFactory> gfxFactory)
        : factory(std::move(gfxFactory))
    {
        // Create main rendering resources
        auto window = factory->CreateWindow("Game");
        renderer = factory->CreateRenderer(window.get());
        renderTarget = factory->CreateRenderTarget();
    }

    void RenderScene() {
        renderer->BeginFrame();

        // Render to main window
        renderer->Clear(FColor::Black);
        RenderGameObjects();

        // Render to texture for post-processing
        renderTarget->Clear(FColor::Transparent);
        RenderUI();

        // Apply post-processing
        ApplyPostProcessing(renderTarget.get());

        renderer->EndFrame();
    }
};
```

## Error Handling

### Resource Creation Failures
```cpp
ITexture* SafeLoadTexture(const FilePath& path) {
    try {
        auto texture = graphicsFactory->CreateTexture(path);
        if (texture && texture->IsValid()) {
            return texture.release();
        }
    } catch (const std::exception& e) {
        FLogger::Error("Failed to load texture {}: {}", path, e.what());
    }
    return nullptr;
}

IShader* SafeLoadShader(const FilePath& vertexPath, const FilePath& fragmentPath) {
    try {
        auto shader = graphicsFactory->CreateShader(vertexPath, fragmentPath);
        if (shader && shader->IsValid()) {
            return shader.release();
        }
    } catch (const std::exception& e) {
        FLogger::Error("Failed to load shader: {}", e.what());
    }
    return nullptr;
}
```

## Best Practices

### Factory Usage
1. **Singleton Pattern**: Use single factory instance per application
2. **Resource Caching**: Cache created resources to avoid duplication
3. **Error Handling**: Always check resource validity after creation
4. **Backend Selection**: Choose appropriate backend for target platform

### Resource Management
1. **RAII Compliance**: Use unique_ptr for automatic cleanup
2. **Reference Counting**: Consider shared_ptr for shared resources
3. **Pool Pattern**: Use object pools for frequently created/destroyed resources
4. **Lazy Loading**: Load resources on-demand to reduce startup time

### Shader Management
1. **Separate Files**: Keep vertex/fragment shaders in separate files
2. **Include System**: Use shader includes for common code
3. **Validation**: Validate shader compilation before use
4. **Version Compatibility**: Ensure shader versions match target GLSL version

### Texture Management
1. **Format Selection**: Choose appropriate texture formats
2. **Mipmapping**: Generate mipmaps for distance rendering
3. **Compression**: Use texture compression for memory efficiency
4. **Atlas Usage**: Combine small textures into atlases

## Diagram: Graphics Factory Architecture

```
Graphics Factory System
├── IGraphicsFactory (Interface)
│   ├── CreateTexture() - Multiple overloads
│   ├── CreateShader() - Multiple overloads
│   ├── CreateWindow()
│   ├── CreateRenderer()
│   └── CreateRenderTarget()
│
├── SFML::GraphicsFactory (Concrete Implementation)
│   ├── Implements all interface methods
│   ├── Uses SFML library internally
│   └── Client-side only (#if TKD_ENGINE_CLIENT)
│
└── Resource Creation Flow
    ├── Input Sources
    │   ├── File paths
    │   ├── Memory buffers
    │   ├── String literals
    │   ├── Byte vectors
    │   └── UAsset objects
    │
    ├── Output Resources
    │   ├── ITexture implementations
    │   ├── IShader implementations
    │   ├── IWindow implementations
    │   ├── IRenderer implementations
    │   └── IRenderTarget implementations
```

## Diagram: Shader Creation Overloads

```mermaid
graph TD
    A[CreateShader] --> B{Input Type}
    B -->|File Path| C[Single Shader File]
    B -->|File Paths| D[Vertex + Fragment Files]
    B -->|File Paths| E[Vertex + Geometry + Fragment Files]
    B -->|String| F[Single Shader String]
    B -->|Strings| G[Vertex + Fragment Strings]
    B -->|Strings| H[Vertex + Geometry + Fragment Strings]
    B -->|Byte Vector| I[Single Shader Bytes]
    B -->|Byte Vectors| J[Vertex + Fragment Bytes]
    B -->|Byte Vectors| K[Vertex + Geometry + Fragment Bytes]
    B -->|Asset| L[Single Shader Asset]
    B -->|Assets| M[Vertex + Fragment Assets]
    B -->|Assets| N[Vertex + Geometry + Fragment Assets]

    C --> O[SFML::Shader(file, type)]
    D --> P[SFML::Shader(vFile, fFile)]
    E --> Q[SFML::Shader(vFile, gFile, fFile)]
    F --> R[SFML::Shader(string, type)]
    G --> S[SFML::Shader(vString, fString)]
    H --> T[SFML::Shader(vString, gString, fString)]
    I --> U[SFML::Shader(bytes, type)]
    J --> V[SFML::Shader(vBytes, fBytes)]
    K --> W[SFML::Shader(vBytes, gBytes, fBytes)]
    L --> X[SFML::Shader(asset, type)]
    M --> Y[SFML::Shader(vAsset, fAsset)]
    N --> Z[SFML::Shader(vAsset, gAsset, fAsset)]
```

## Troubleshooting

### Common Issues

1. **Factory Not Available**: Ensure TKD_ENGINE_CLIENT is defined for client builds
2. **Resource Loading Failures**: Check file paths, formats, and permissions
3. **Shader Compilation Errors**: Validate shader syntax and version compatibility
4. **Memory Issues**: Monitor resource lifetime and prevent leaks
5. **Backend Compatibility**: Ensure chosen backend supports target platform

### Debug Techniques
```cpp
// Factory validation
bool ValidateGraphicsFactory(IGraphicsFactory* factory) {
    if (!factory) {
        FLogger::Error("Graphics factory is null");
        return false;
    }

    // Test basic creation
    try {
        auto texture = factory->CreateTexture();
        auto shader = factory->CreateShader();
        auto renderTarget = factory->CreateRenderTarget();

        FLogger::Info("Graphics factory validation passed");
        return true;
    } catch (const std::exception& e) {
        FLogger::Error("Graphics factory validation failed: {}", e.what());
        return false;
    }
}

// Resource validation
bool ValidateResourceCreation(IGraphicsFactory* factory, const FilePath& testFile) {
    try {
        auto texture = factory->CreateTexture(testFile);
        if (texture && texture->IsValid()) {
            FLogger::Info("Texture creation successful");
            return true;
        } else {
            FLogger::Error("Texture creation failed or invalid");
            return false;
        }
    } catch (const std::exception& e) {
        FLogger::Error("Texture creation exception: {}", e.what());
        return false;
    }
}
```

This documentation provides a comprehensive overview of the Graphics Factory system, covering its implementation, usage patterns, and integration within the TKD Engine's rendering architecture.
