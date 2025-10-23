# TResourceHandle - Resource Management System

## Overview

The `TResourceHandle` class is a template-based smart handle system in the TKD Game Engine that provides safe, reference-counted resource management. It serves as a wrapper around `std::shared_ptr` with additional features like unique identification and resource lifecycle management.

Resource handles are essential for:
- **Memory Management**: Automatic resource cleanup through reference counting
- **Resource Sharing**: Safe sharing of resources across multiple systems
- **Cache Management**: Efficient resource loading and unloading
- **Thread Safety**: Reference counting is thread-safe in most implementations
- **Resource Tracking**: Unique identification for debugging and profiling

## Architecture

### Class Hierarchy
```
TResourceHandle<T>
├── Template Parameters
│   └── T - Resource type (ITexture, IShader, etc.)
├── Member Variables
│   ├── m_resource (std::shared_ptr<T>) - Shared resource pointer
│   └── m_id (FString) - Unique resource identifier
├── Access Methods
├── State Queries
├── Resource Management
└── Operator Overloads
```

### Dependencies
- `std::shared_ptr` - Standard library smart pointer for reference counting
- `FString` - Engine's string class for resource identifiers
- `SizeT` - Engine's size type definition
- Template parameter `T` - Any resource type (interfaces like `ITexture`, `IShader`)

### Memory Layout
The `TResourceHandle` contains:
- **Shared Pointer**: 16 bytes (typical `std::shared_ptr` size)
- **String ID**: Variable size (depends on resource identifier length)
- **Total**: ~24+ bytes per handle instance

## Core Components

### Template Parameter

#### Resource Type (T)
```cpp
template <typename T>
class TResourceHandle
```
- **Purpose**: Defines the type of resource being managed
- **Requirements**: Must be a valid C++ type, typically resource interfaces
- **Usage**: Enables type-safe resource handling

### Member Variables

#### Resource Pointer (`m_resource`)
```cpp
std::shared_ptr<T> m_resource;  //<! Shared pointer to the resource
```
- **Type**: `std::shared_ptr<T>`
- **Purpose**: Holds the actual resource with reference counting
- **Default**: `nullptr`
- **Thread Safety**: Reference counting is thread-safe

#### Resource ID (`m_id`)
```cpp
FString m_id;  //<! Unique identifier for the resource
```
- **Type**: `FString`
- **Purpose**: Provides unique identification for the resource
- **Default**: Empty string `""`
- **Usage**: Resource lookup, debugging, and profiling

## API Reference

### Constructors

#### Default Constructor
```cpp
TResourceHandle(void);
```
Creates an invalid resource handle with no resource and empty ID.

**Implementation Details:**
- Initializes `m_resource` to `nullptr`
- Initializes `m_id` to empty string

#### Parameterized Constructor
```cpp
TResourceHandle(std::shared_ptr<T> resource, const FString& id);
```
Creates a resource handle with the specified resource and identifier.

**Parameters:**
- `resource`: Shared pointer to the resource to manage
- `id`: Unique identifier for the resource

**Implementation Details:**
- Takes ownership of the shared pointer
- Copies the identifier string

### Access Methods

#### Get
```cpp
T* Get(void) const;
```
Returns a raw pointer to the managed resource.

**Returns:** Raw pointer to the resource, or `nullptr` if invalid

**Usage Notes:**
- Returns `nullptr` for invalid handles
- Does not affect reference counting
- Use for direct resource access

#### GetId
```cpp
const FString& GetId(void) const;
```
Returns the unique identifier of the resource.

**Returns:** Reference to the resource ID string

### State Query Methods

#### IsValid
```cpp
bool IsValid(void) const;
```
Checks if the handle points to a valid resource.

**Returns:** `true` if `m_resource` is not `nullptr`, `false` otherwise

**Implementation Details:**
- Simply checks `m_resource != nullptr`

#### GetRefCount
```cpp
SizeT GetRefCount(void) const;
```
Returns the current reference count of the resource.

**Returns:** Number of shared pointers referencing the resource, or 0 if invalid

**Implementation Details:**
- Calls `m_resource.use_count()` if valid
- Returns 0 for invalid handles

### Resource Management

#### Reset
```cpp
void Reset(void);
```
Resets the handle to an invalid state, potentially releasing the resource.

**Implementation Details:**
- Calls `m_resource.reset()`
- Sets `m_id` to empty string
- May trigger resource destruction if this was the last reference

### Operator Overloads

#### Arrow Operator (->)
```cpp
T* operator->(void) const;
```
Provides direct member access to the resource.

**Returns:** Raw pointer to the resource

**Usage:**
```cpp
FTextureHandle texture = resourceManager.LoadTexture("player.png");
if (texture) {
    texture->Bind();  // Direct access to ITexture methods
}
```

#### Dereference Operator (*)
```cpp
T& operator*(void) const;
```
Provides direct reference access to the resource.

**Returns:** Reference to the resource

**Usage:**
```cpp
FTextureHandle texture = resourceManager.LoadTexture("background.png");
ITexture& texRef = *texture;  // Get reference
```

#### Bool Conversion Operator
```cpp
explicit operator bool(void) const;
```
Allows implicit conversion to bool for validity checking.

**Returns:** `true` if the handle is valid, `false` otherwise

**Usage:**
```cpp
FTextureHandle texture = resourceManager.LoadTexture("sprite.png");
if (texture) {  // Implicit bool conversion
    // Use texture
}
```

## Type Aliases

The engine provides convenient type aliases for common resource types:

### FTextureHandle
```cpp
using FTextureHandle = TResourceHandle<ITexture>;
```
- **Purpose**: Handle for texture resources
- **Usage**: 2D/3D textures, render targets, cube maps

### FShaderHandle
```cpp
using FShaderHandle = TResourceHandle<IShader>;
```
- **Purpose**: Handle for shader programs
- **Usage**: Vertex, fragment, geometry, and compute shaders

### FAudioSourceHandle
```cpp
using FAudioSourceHandle = TResourceHandle<IAudioSource>;
```
- **Purpose**: Handle for audio playback sources
- **Usage**: Sound effects, music, positional audio

### FAudioBufferHandle
```cpp
using FAudioBufferHandle = TResourceHandle<IAudioBuffer>;
```
- **Purpose**: Handle for audio data buffers
- **Usage**: Loaded audio files, procedural audio data

## Usage Examples

### Basic Resource Loading
```cpp
class ResourceManager {
private:
    std::unordered_map<FString, FTextureHandle> m_textures;

public:
    FTextureHandle LoadTexture(const FString& path) {
        // Check if already loaded
        auto it = m_textures.find(path);
        if (it != m_textures.end()) {
            return it->second;  // Return existing handle
        }

        // Load new texture
        auto texture = std::make_shared<SFMLTexture>();
        if (texture->LoadFromFile(path)) {
            FTextureHandle handle(texture, path);
            m_textures[path] = handle;
            return handle;
        }

        return FTextureHandle();  // Invalid handle
    }
};

// Usage
ResourceManager rm;
FTextureHandle playerTex = rm.LoadTexture("player.png");
if (playerTex.IsValid()) {
    SizeT refCount = playerTex.GetRefCount();  // Should be 2 (manager + local)
    renderer.BindTexture(playerTex.Get());
}
```

### Resource Sharing Across Systems
```cpp
class TextureCache {
private:
    std::unordered_map<FString, std::weak_ptr<ITexture>> m_cache;

public:
    FTextureHandle GetOrLoadTexture(const FString& path) {
        // Check weak pointer cache
        auto it = m_cache.find(path);
        if (it != m_cache.end()) {
            if (auto texture = it->second.lock()) {
                // Resource still exists, create new handle
                return FTextureHandle(texture, path);
            }
        }

        // Load new texture
        auto texture = LoadTextureFromFile(path);
        if (texture) {
            m_cache[path] = texture;  // Store weak pointer
            return FTextureHandle(texture, path);
        }

        return FTextureHandle();
    }
};

class SpriteRenderer {
private:
    FTextureHandle m_texture;

public:
    void SetTexture(const FTextureHandle& texture) {
        m_texture = texture;  // Shared ownership
    }

    void Render() {
        if (m_texture) {
            m_texture->Bind();
            // Render sprite
        }
    }
};

// Usage
TextureCache cache;
SpriteRenderer renderer1, renderer2;

// Both renderers share the same texture
FTextureHandle tex = cache.GetOrLoadTexture("sprite.png");
renderer1.SetTexture(tex);  // ref count = 2
renderer2.SetTexture(tex);  // ref count = 3

// When tex goes out of scope, ref count = 2 (still held by renderers)
// When renderers are destroyed, texture is automatically cleaned up
```

### Resource Pool Management
```cpp
template <typename T>
class ResourcePool {
private:
    std::unordered_map<FString, std::weak_ptr<T>> m_pool;
    std::function<std::shared_ptr<T>(const FString&)> m_factory;

public:
    ResourcePool(std::function<std::shared_ptr<T>(const FString&)> factory)
        : m_factory(factory) {}

    TResourceHandle<T> GetResource(const FString& id) {
        // Check if resource exists in pool
        auto it = m_pool.find(id);
        if (it != m_pool.end()) {
            if (auto resource = it->second.lock()) {
                return TResourceHandle<T>(resource, id);
            }
        }

        // Create new resource
        auto resource = m_factory(id);
        if (resource) {
            m_pool[id] = resource;  // Store weak pointer
            return TResourceHandle<T>(resource, id);
        }

        return TResourceHandle<T>();
    }

    void Cleanup() {
        // Remove expired weak pointers
        for (auto it = m_pool.begin(); it != m_pool.end(); ) {
            if (it->second.expired()) {
                it = m_pool.erase(it);
            } else {
                ++it;
            }
        }
    }
};

// Usage
auto textureFactory = [](const FString& path) -> std::shared_ptr<ITexture> {
    auto tex = std::make_shared<SFMLTexture>();
    return tex->LoadFromFile(path) ? tex : nullptr;
};

ResourcePool<ITexture> texturePool(textureFactory);
FTextureHandle tex1 = texturePool.GetResource("background.png");
FTextureHandle tex2 = texturePool.GetResource("background.png");  // Same resource

// Both handles share the same texture instance
assert(tex1.Get() == tex2.Get());
assert(tex1.GetRefCount() == 3);  // pool + tex1 + tex2
```

### Asynchronous Resource Loading
```cpp
class AsyncResourceLoader {
private:
    std::queue<std::pair<FString, std::function<void(FTextureHandle)>>> m_loadQueue;
    std::thread m_loaderThread;
    std::mutex m_queueMutex;
    std::atomic<bool> m_running;

public:
    AsyncResourceLoader() : m_running(true) {
        m_loaderThread = std::thread(&AsyncResourceLoader::LoaderThread, this);
    }

    ~AsyncResourceLoader() {
        m_running = false;
        m_loaderThread.join();
    }

    void LoadTextureAsync(const FString& path, std::function<void(FTextureHandle)> callback) {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        m_loadQueue.push({path, callback});
    }

private:
    void LoaderThread() {
        while (m_running) {
            std::pair<FString, std::function<void(FTextureHandle)>> task;

            {
                std::lock_guard<std::mutex> lock(m_queueMutex);
                if (!m_loadQueue.empty()) {
                    task = m_loadQueue.front();
                    m_loadQueue.pop();
                } else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    continue;
                }
            }

            // Load texture (blocking operation)
            auto texture = std::make_shared<SFMLTexture>();
            FTextureHandle handle;

            if (texture->LoadFromFile(task.first)) {
                handle = FTextureHandle(texture, task.first);
            }

            // Call callback with loaded handle
            task.second(handle);
        }
    }
};

// Usage
AsyncResourceLoader loader;

loader.LoadTextureAsync("large_texture.png",
    [](FTextureHandle texture) {
        if (texture) {
            // Texture loaded successfully
            std::cout << "Loaded texture: " << texture.GetId() << std::endl;
            // Use texture in main thread
        } else {
            std::cout << "Failed to load texture" << std::endl;
        }
    });

// Continue with other work while texture loads asynchronously
```

### Resource Lifetime Management
```cpp
class Scene {
private:
    std::vector<FTextureHandle> m_textures;
    std::vector<FShaderHandle> m_shaders;

public:
    void LoadResources() {
        // Load scene resources
        m_textures.push_back(resourceManager.LoadTexture("skybox.png"));
        m_textures.push_back(resourceManager.LoadTexture("terrain.png"));
        m_shaders.push_back(resourceManager.LoadShader("terrain.vert", "terrain.frag"));

        std::cout << "Scene loaded. Texture refs: "
                  << m_textures[0].GetRefCount() << std::endl;
    }

    void Render() {
        for (const auto& texture : m_textures) {
            if (texture) {
                texture->Bind();
                // Render with texture
            }
        }
    }
};

// Usage
{
    Scene scene;
    scene.LoadResources();
    // Resources are kept alive by scene's handles

    scene.Render();
} // Scene destroyed, handles released, resources may be cleaned up
```

### Resource Debugging and Profiling
```cpp
class ResourceProfiler {
private:
    std::unordered_map<FString, SizeT> m_peakRefCounts;
    std::unordered_map<FString, std::chrono::steady_clock::time_point> m_loadTimes;

public:
    void TrackHandle(const FTextureHandle& handle) {
        if (handle.IsValid()) {
            const FString& id = handle.GetId();
            SizeT refCount = handle.GetRefCount();

            // Track peak reference count
            auto it = m_peakRefCounts.find(id);
            if (it == m_peakRefCounts.end() || refCount > it->second) {
                m_peakRefCounts[id] = refCount;
            }

            // Track load time (if not already tracked)
            if (m_loadTimes.find(id) == m_loadTimes.end()) {
                m_loadTimes[id] = std::chrono::steady_clock::now();
            }
        }
    }

    void PrintReport() {
        std::cout << "Resource Profiling Report:" << std::endl;
        for (const auto& pair : m_peakRefCounts) {
            auto loadTimeIt = m_loadTimes.find(pair.first);
            if (loadTimeIt != m_loadTimes.end()) {
                auto now = std::chrono::steady_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>
                               (now - loadTimeIt->second);

                std::cout << "  " << pair.first
                         << ": Peak refs=" << pair.second
                         << ", Load time=" << duration.count() << "ms"
                         << std::endl;
            }
        }
    }
};

// Usage
ResourceProfiler profiler;

FTextureHandle tex = resourceManager.LoadTexture("character.png");
profiler.TrackHandle(tex);

// Later...
profiler.PrintReport();
```

## Mathematical Foundation

### Reference Counting
The `TResourceHandle` uses `std::shared_ptr` which implements reference counting:

**Reference Count**: Number of shared pointers pointing to the resource
**Construction**: Count starts at 1
**Copy**: Count increments
**Destruction**: Count decrements
**Cleanup**: Resource destroyed when count reaches 0

### Memory Overhead
```
TResourceHandle<T> memory layout:
- std::shared_ptr control block: 16-32 bytes (implementation dependent)
- T* pointer: 8 bytes
- FString m_id: 16+ bytes (string data)
- Total: ~40+ bytes per handle
```

### Thread Safety
- **Reference Counting**: Thread-safe in C++11 and later
- **Resource Access**: Not thread-safe (external synchronization required)
- **ID Access**: Thread-safe (const access)

## Performance Considerations

### Memory Usage
- **Per Handle**: ~40-64 bytes (depending on string length)
- **Shared Resources**: Control block shared across handles
- **String Storage**: ID strings stored per handle instance

### Performance Characteristics
- **Construction**: Fast (shared_ptr copy)
- **Destruction**: Fast (reference count decrement)
- **Access**: Very fast (pointer dereference)
- **Copy**: Fast (reference count increment)

### Optimization Strategies
1. **Handle Pooling**: Reuse handle objects to reduce allocations
2. **Weak Pointers**: Use `std::weak_ptr` for non-owning references
3. **Resource Caching**: Cache loaded resources to avoid reloads
4. **Batch Loading**: Load multiple resources together

### Best Practices
- Use handles consistently across the codebase
- Implement resource caching to avoid duplicate loads
- Monitor reference counts for memory leak detection
- Use weak pointers for observer patterns
- Profile resource usage in performance-critical code

## Diagrams

### Reference Counting Lifecycle
```
Creation:
┌─────────────┐    ┌─────────────────┐
│ Application │───▶│ TResourceHandle │
└─────────────┘    └─────────────────┘
                       │
                       ▼
                  ┌─────────────┐
                  │std::shared_ │
                  │    ptr      │
                  │ ref_count=1 │
                  └─────────────┘
                       │
                       ▼
                  ┌─────────────┐
                  │   Resource  │
                  │  (ITexture) │
                  └─────────────┘

Copy Handle:
┌─────────────┐    ┌─────────────────┐
│ Component  │───▶│ TResourceHandle │
│   A        │    │  (copy)         │
└─────────────┘    └─────────────────┘
                       │
                       ▼
                  ┌─────────────┐
                  │std::shared_ │
                  │    ptr      │
                  │ ref_count=2 │ ← Incremented
                  └─────────────┘
                       │
                       ▼
                  ┌─────────────┐
                  │   Resource  │
                  │  (ITexture) │
                  └─────────────┘

Destruction:
Component A destroyed ──▶ ref_count=1 (still valid)

All handles destroyed ──▶ ref_count=0 ──▶ Resource destroyed
```

### Resource Management Architecture
```
┌─────────────────┐    ┌──────────────────┐
│  Resource       │    │  Resource Pool   │
│  Manager        │    │                  │
└─────────────────┘    └──────────────────┘
         │                        │
         ▼                        ▼
┌─────────────────┐    ┌──────────────────┐
│ TResourceHandle │◄───┤ std::weak_ptr    │
│                 │    │  Cache           │
└─────────────────┘    └──────────────────┘
         │                        │
         ▼                        ▼
┌─────────────────┐    ┌──────────────────┐
│ std::shared_ptr │    │  Resource        │
│                 │    │  Instances       │
└─────────────────┘    └──────────────────┘
         │
         ▼
┌─────────────────┐
│   Actual        │
│   Resource      │
│   (ITexture,    │
│    IShader)     │
└─────────────────┘
```

### Handle Usage Patterns
```
Direct Usage:
┌─────────────┐    ┌─────────────────┐    ┌─────────────┐
│ User Code   │───▶│ TResourceHandle │───▶│ Resource    │
└─────────────┘    └─────────────────┘    └─────────────┘

Resource Manager:
┌─────────────┐    ┌─────────────────┐    ┌─────────────┐
│ Manager     │───▶│ TResourceHandle │───▶│ Cached      │
│             │    └─────────────────┘    │ Resources   │
└─────────────┘                           └─────────────┘

Component System:
┌─────────────┐    ┌─────────────────┐
│ Component   │───▶│ TResourceHandle │◄──┐
│   A         │    └─────────────────┘    │
└─────────────┘                           │
                                          │
┌─────────────┐    ┌─────────────────┐    │
│ Component   │───▶│ TResourceHandle │◄──┘
│   B         │    └─────────────────┘
└─────────────┘
    Shared resource, independent lifetimes
```

## Integration with Rendering Pipeline

### Texture Management in Renderer
```cpp
class Renderer {
private:
    std::unordered_map<FString, FTextureHandle> m_textureCache;

public:
    void BindTexture(const FTextureHandle& texture) {
        if (texture.IsValid()) {
            texture->Bind();
            m_boundTexture = texture;
        }
    }

    FTextureHandle LoadTexture(const FString& path) {
        auto it = m_textureCache.find(path);
        if (it != m_textureCache.end()) {
            return it->second;
        }

        auto texImpl = std::make_shared<OpenGLTexture>();
        if (texImpl->LoadFromFile(path)) {
            FTextureHandle handle(texImpl, path);
            m_textureCache[path] = handle;
            return handle;
        }

        return FTextureHandle();
    }
};
```

### Shader Program Management
```cpp
class ShaderManager {
private:
    std::unordered_map<FString, FShaderHandle> m_shaders;

public:
    FShaderHandle GetShader(const FString& vertexPath, const FString& fragmentPath) {
        FString key = vertexPath + ":" + fragmentPath;

        auto it = m_shaders.find(key);
        if (it != m_shaders.end()) {
            return it->second;
        }

        auto shader = std::make_shared<GLSLShader>();
        if (shader->LoadFromFiles(vertexPath, fragmentPath)) {
            FShaderHandle handle(shader, key);
            m_shaders[key] = handle;
            return handle;
        }

        return FShaderHandle();
    }

    void UseShader(const FShaderHandle& shader) {
        if (shader) {
            shader->Use();
        }
    }
};
```

## Troubleshooting

### Common Issues

#### Handle Always Invalid
**Problem**: `IsValid()` always returns false
**Solutions:**
- Check if resource was loaded successfully
- Verify resource creation didn't fail
- Ensure handle wasn't reset accidentally

#### Resource Not Released
**Problem**: Memory usage increases over time
**Solutions:**
- Check for circular references
- Use weak pointers where appropriate
- Implement proper cleanup in destructors
- Monitor reference counts

#### Thread Safety Issues
**Problem**: Crashes when accessing resources from multiple threads
**Solutions:**
- Use mutexes for resource access
- Avoid sharing handles across threads
- Use thread-local resource managers

#### Performance Degradation
**Problem**: Slow performance with many handles
**Solutions:**
- Implement handle pooling
- Use object pooling for resources
- Cache frequently used resources
- Profile and optimize hot paths

### Debug Visualization
```cpp
void DebugResourceHandle(const FTextureHandle& handle) {
    if (handle.IsValid()) {
        DebugRenderer::DrawText(
            FVector2(10, 10),
            Format("Texture: {} (refs: {}, size: {}x{})",
                   handle.GetId(),
                   handle.GetRefCount(),
                   handle->GetWidth(),
                   handle->GetHeight()),
            FColor::White
        );
    } else {
        DebugRenderer::DrawText(
            FVector2(10, 10),
            "No texture loaded",
            FColor::Red
        );
    }
}

class ResourceLeakDetector {
private:
    std::unordered_map<FString, SizeT> m_initialRefCounts;

public:
    void Snapshot() {
        // Store initial reference counts for all tracked resources
        // Implementation would iterate through all known handles
    }

    void CheckForLeaks() {
        // Compare current ref counts with initial snapshot
        // Report any resources that haven't been released
    }
};
```

## Future Enhancements

### Potential Features
- **Resource Streaming**: Asynchronous loading with progress callbacks
- **Resource Dependencies**: Automatic loading of dependent resources
- **Resource Compression**: On-demand decompression for memory efficiency
- **Resource Hot-Reloading**: Runtime resource replacement for development
- **Resource Profiling**: Detailed memory and performance tracking

### API Extensions
```cpp
// Potential future methods
template <typename T>
class TResourceHandle {
    // Additional future methods
    bool IsLoaded() const;  // Check if resource is fully loaded
    void AddRef() const;    // Manual reference increment
    void Release();         // Manual reference decrement
    void SetUserData(void* data);  // Associate custom data
    void* GetUserData() const;     // Retrieve custom data

    // Async loading support
    using LoadCallback = std::function<void(TResourceHandle<T>)>;
    static TResourceHandle<T> LoadAsync(const FString& id, LoadCallback callback);

    // Resource metadata
    FString GetTypeName() const;
    SizeT GetMemoryUsage() const;
    std::chrono::system_clock::time_point GetLoadTime() const;
};
```
