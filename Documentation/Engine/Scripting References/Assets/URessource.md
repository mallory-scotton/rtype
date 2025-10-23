# UResource - Resource Management System

## Overview

The `UResource` class is the central resource management system in the TKD Game Engine, implementing a singleton-based architecture for loading, caching, and managing game assets. It provides thread-safe resource handling with automatic reference counting, support for multiple source types (files, assets, pak files), and efficient memory management through weak pointer caching.

Resource management is crucial for:
- **Memory Efficiency**: Preventing duplicate resource loading and enabling automatic cleanup
- **Thread Safety**: Safe concurrent access to resources from multiple threads
- **Asset Organization**: Support for loose files, packaged assets, and compressed archives
- **Performance**: Fast resource lookup and caching mechanisms
- **Development Workflow**: Hot-reloading and resource monitoring capabilities

## Architecture

### Class Hierarchy
```
TSingleton<URessource>
├── UResource (singleton instance)
    ├── Member Variables
    │   ├── m_graphicsFactory (IGraphicsFactory*) - Graphics backend
    │   ├── m_isShuttingDown (bool) - Shutdown state
    │   ├── m_mutex (std::mutex) - Thread synchronization
    │   ├── m_textures (TSharedMap<ITexture>) - Texture cache
    │   ├── m_shaders (TSharedMap<IShader>) - Shader cache
    │   ├── m_buffers (TSharedMap<IAudioBuffer>) - Audio buffer cache
    │   ├── m_sources (TSharedMap<IAudioSource>) - Audio source cache
    │   ├── m_pakFiles (TUniqueMap<UPak>) - Mounted pak files
    │   └── m_assets (TUniqueMap<UAsset>) - Loaded assets
    ├── Resource Loading
    ├── Cache Management
    ├── Pak File Support
    └── Thread Safety
```

### Dependencies
- `TSingleton` - Singleton pattern implementation
- `IGraphicsFactory` - Graphics backend for resource creation
- `TResourceHandle<T>` - Smart resource handles with reference counting
- `UAsset` - Asset file management
- `UPak` - Pak file archive management
- `std::mutex` - Thread synchronization
- `std::weak_ptr` - Weak reference caching
- `TVariant` - Type-safe union for multiple source types

### Memory Layout
The `UResource` singleton contains:
- **Factory Pointer**: 8 bytes (IGraphicsFactory*)
- **Shutdown Flag**: 1 byte (bool)
- **Mutex**: Variable size (platform-dependent)
- **Resource Caches**: Multiple unordered_maps with weak pointers
- **Pak Files**: Unique pointers to UPak instances
- **Assets**: Unique pointers to UAsset instances

## Core Components

### Member Variables

#### Graphics Factory (`m_graphicsFactory`)
```cpp
IGraphicsFactory* m_graphicsFactory;  //<! Graphics factory
```
- **Type**: `IGraphicsFactory*`
- **Purpose**: Backend for creating graphics resources (textures, shaders)
- **Default**: `nullptr`
- **Usage**: Must be set before loading graphics resources

#### Shutdown Flag (`m_isShuttingDown`)
```cpp
bool m_isShuttingDown;  //<! Shutdown flag
```
- **Type**: `bool`
- **Purpose**: Prevents new resource loads during shutdown
- **Default**: `false`
- **Usage**: Set by `BeginShutdown()` to stop accepting new loads

#### Thread Safety Mutex (`m_mutex`)
```cpp
mutable std::mutex m_mutex;  //<! Thread safety mutex
```
- **Type**: `std::mutex`
- **Purpose**: Synchronizes access to all member variables
- **Usage**: All public methods acquire this lock

#### Texture Cache (`m_textures`)
```cpp
TSharedMap<ITexture> m_textures;  //<! Loaded textures (weak refs)
```
- **Type**: `std::unordered_map<FString, std::weak_ptr<ITexture>>`
- **Purpose**: Caches loaded textures with weak references
- **Key**: Resource identifier (file path or UUID)
- **Value**: Weak pointer to prevent ownership cycles

#### Shader Cache (`m_shaders`)
```cpp
TSharedMap<IShader> m_shaders;  //<! Loaded shaders (weak refs)
```
- **Type**: `std::unordered_map<FString, std::weak_ptr<IShader>>`
- **Purpose**: Caches loaded shaders with weak references
- **Key**: Resource identifier (combined file paths or UUID)
- **Value**: Weak pointer to prevent ownership cycles

#### Audio Buffer Cache (`m_buffers`)
```cpp
TSharedMap<IAudioBuffer> m_buffers;  //<! Loaded audio buffers
```
- **Type**: `std::unordered_map<FString, std::weak_ptr<IAudioBuffer>>`
- **Purpose**: Caches loaded audio buffers
- **Status**: Declared but not implemented in current version

#### Audio Source Cache (`m_sources`)
```cpp
TSharedMap<IAudioSource> m_sources;  //<! Loaded audio sources
```
- **Type**: `std::unordered_map<FString, std::weak_ptr<IAudioSource>>`
- **Purpose**: Caches loaded audio sources
- **Status**: Declared but not implemented in current version

#### Pak Files (`m_pakFiles`)
```cpp
TUniqueMap<UPak> m_pakFiles;  //<! Loaded pak files
```
- **Type**: `std::unordered_map<FString, std::unique_ptr<UPak>>`
- **Purpose**: Stores mounted pak file archives
- **Key**: Pak file path as string
- **Value**: Unique pointer to UPak instance

#### Assets (`m_assets`)
```cpp
TUniqueMap<UAsset> m_assets;  //<! Loaded assets
```
- **Type**: `std::unordered_map<FString, std::unique_ptr<UAsset>>`
- **Purpose**: Stores loaded asset files
- **Key**: Asset UUID
- **Value**: Unique pointer to UAsset instance

## API Reference

### Singleton Access

#### GetInstance
```cpp
static UResource& GetInstance(void);
```
Returns the singleton instance of the resource manager.

**Returns:** Reference to the singleton UResource instance

**Implementation Details:**
- Inherited from `TSingleton<UResource>`
- Thread-safe singleton creation

### Lifecycle Management

#### Constructor
```cpp
UResource::UResource(void);
```
Initializes the resource manager with default values.

**Implementation Details:**
- Sets `m_graphicsFactory` to `nullptr`
- Sets `m_isShuttingDown` to `false`
- Initializes empty caches

#### Destructor
```cpp
virtual ~UResource();
```
Cleans up all resources in proper shutdown order.

**Implementation Details:**
- Calls `BeginShutdown()` to stop new loads
- Clears all resource caches in dependency order
- Sets factory pointer to `nullptr`

#### BeginShutdown
```cpp
void BeginShutdown(void);
```
Signals the start of shutdown process, preventing new resource loads.

**Implementation Details:**
- Thread-safe operation with mutex lock
- Sets `m_isShuttingDown` flag to `true`

### Resource Cleanup

#### CleanupUnusedResources
```cpp
SizeT CleanupUnusedResources(void);
```
Removes expired weak references from all resource caches.

**Returns:** Number of resources that were cleaned up

**Implementation Details:**
- Iterates through texture and shader caches
- Removes entries where `weak_ptr.expired()` returns true
- Thread-safe operation with mutex lock

### Graphics Factory Setup

#### SetGraphicsFactory
```cpp
void SetGraphicsFactory(IGraphicsFactory* factory);
```
Sets the graphics factory for creating graphics resources.

**Parameters:**
- `factory`: Pointer to the graphics factory implementation

**Implementation Details:**
- Thread-safe operation with mutex lock
- Must be called before loading textures or shaders

### Asset Management

#### LoadAsset
```cpp
UAsset* LoadAsset(const FilePath& path);
```
Loads an asset from file or pak archive.

**Parameters:**
- `path`: Path to the asset file

**Returns:** Pointer to loaded asset, or `nullptr` on failure

**Implementation Details:**
- Creates UAsset instance and attempts to load
- Caches asset by UUID to prevent duplicate loading
- Thread-safe operation

#### GetAsset
```cpp
UAsset* GetAsset(const FString& uuid) const;
```
Retrieves a previously loaded asset by UUID.

**Parameters:**
- `uuid`: Unique identifier of the asset

**Returns:** Pointer to asset, or `nullptr` if not found

**Implementation Details:**
- Thread-safe lookup in asset cache
- Returns `nullptr` for invalid UUIDs

### Texture Management

#### LoadTexture
```cpp
FTextureHandle LoadTexture(
    const TVariant<FilePath, FString, UAsset*>& source,
    const FRectanglei& area = FRectanglei::Zero
);
```
Loads a texture from various source types with caching.

**Parameters:**
- `source`: Texture source (file path, UUID string, or UAsset pointer)
- `area`: Optional texture region to load (default: entire texture)

**Returns:** Texture handle, or invalid handle on failure

**Implementation Details:**
- Supports three source types via TVariant
- Checks cache first to avoid duplicate loading
- Searches pak files if file not found on disk
- Uses graphics factory to create texture
- Thread-safe operation

**Source Type Handling:**
- **FilePath**: Loads from disk file or pak archive
- **FString**: Treats as UUID, searches assets and paks
- **UAsset***: Loads directly from asset data

#### GetTexture
```cpp
FTextureHandle GetTexture(const FString& id) const;
```
Retrieves a cached texture by identifier.

**Parameters:**
- `id`: Texture identifier (file path or UUID)

**Returns:** Texture handle, or invalid handle if not found

**Implementation Details:**
- Thread-safe cache lookup
- Returns valid handle if texture still exists

### Shader Management

#### LoadShader
```cpp
FShaderHandle LoadShader(const TVariant<
                         std::tuple<FilePath, EShaderType>,
                         std::tuple<FilePath, FilePath>,
                         std::tuple<FilePath, FilePath, FilePath>,
                         FString,
                         UAsset*>& source);
```
Loads a shader from various source configurations with caching.

**Parameters:**
- `source`: Shader source configuration

**Returns:** Shader handle, or invalid handle on failure

**Implementation Details:**
- Supports five source types via TVariant
- Checks cache first to avoid duplicate loading
- Uses graphics factory to create shader
- Thread-safe operation

**Source Type Handling:**
- **tuple<FilePath, EShaderType>**: Single shader file with type
- **tuple<FilePath, FilePath>**: Vertex + fragment shader files
- **tuple<FilePath, FilePath, FilePath>**: Vertex + geometry + fragment files
- **FString**: UUID for asset-based loading
- **UAsset***: Direct asset data loading

#### GetShader
```cpp
FShaderHandle GetShader(const FString& id) const;
```
Retrieves a cached shader by identifier.

**Parameters:**
- `id`: Shader identifier (combined file paths or UUID)

**Returns:** Shader handle, or invalid handle if not found

**Implementation Details:**
- Thread-safe cache lookup
- Returns valid handle if shader still exists

### Pak File Management

#### LoadPak
```cpp
bool LoadPak(const FilePath& pakPath);
```
Loads and mounts a pak file archive.

**Parameters:**
- `pakPath`: Path to the .pak file

**Returns:** `true` if loaded successfully, `false` otherwise

**Implementation Details:**
- Creates UPak instance and opens archive
- Prevents duplicate loading of same pak
- Thread-safe operation

#### UnloadPak
```cpp
void UnloadPak(const FilePath& pakPath);
```
Unmounts and unloads a pak file.

**Parameters:**
- `pakPath`: Path to the .pak file to unload

**Implementation Details:**
- Removes pak from cache by path
- Thread-safe operation

#### GetLoadedPaks
```cpp
std::vector<FilePath> GetLoadedPaks(void) const;
```
Returns list of all currently loaded pak files.

**Returns:** Vector of pak file paths

**Implementation Details:**
- Thread-safe enumeration of pak files
- Returns paths in arbitrary order

### Static Helper Methods

#### GetTextureHandle (Static)
```cpp
static FTextureHandle GetTextureHandle(
    const FilePath& path, const FRectanglei& area = FRectanglei::Zero
);
```
Static convenience method for loading textures.

**Parameters:**
- `path`: Path to texture file
- `area`: Optional texture region

**Returns:** Texture handle, or invalid handle on failure

**Implementation Details:**
- Gets singleton instance and calls `GetTexture()` first
- Falls back to `LoadTexture()` if not cached

#### LoadResource (Template)
```cpp
template <typename T, typename = std::enable_if_t<std::is_same_v<T, ITexture>>>
static TResourceHandle<T> LoadResource(const FilePath& path);
```
Template helper for loading resources (currently texture-only).

**Template Parameters:**
- `T`: Resource type (currently only ITexture supported)

**Parameters:**
- `path`: Path to resource file

**Returns:** Resource handle, or invalid handle on failure

**Implementation Details:**
- Currently only supports ITexture
- Gets singleton instance and attempts cache lookup then loading

## Usage Examples

### Basic Resource Manager Setup
```cpp
// Initialize resource manager with graphics factory
URessource& resourceManager = URessource::GetInstance();
resourceManager.SetGraphicsFactory(graphicsFactory);

// Load resources
FTextureHandle backgroundTex = resourceManager.LoadTexture("background.png");
FShaderHandle spriteShader = resourceManager.LoadShader(
    std::make_tuple("sprite.vert", "sprite.frag")
);

// Use resources
if (backgroundTex) {
    renderer.BindTexture(backgroundTex.Get());
    renderer.DrawQuad();
}
```

### Asset-Based Loading
```cpp
URessource& rm = URessource::GetInstance();

// Load asset first
UAsset* asset = rm.LoadAsset("character.uasset");
if (asset) {
    // Load texture from asset
    FTextureHandle tex = rm.LoadTexture(asset);

    // Load shader from asset UUID
    FShaderHandle shader = rm.LoadShader(asset->GetUUID());
}
```

### Pak File Integration
```cpp
URessource& rm = URessource::GetInstance();

// Mount game assets pak
bool loaded = rm.LoadPak("game_assets.pak");
if (loaded) {
    // Load texture from pak (by filename)
    FTextureHandle tex = rm.LoadTexture("textures/player.png");

    // Load shader from pak (by UUID)
    FShaderHandle shader = rm.LoadShader("shader_main_uuid");
}

// List loaded paks
auto paks = rm.GetLoadedPaks();
for (const auto& pakPath : paks) {
    std::cout << "Loaded pak: " << pakPath << std::endl;
}

// Unmount when done
rm.UnloadPak("game_assets.pak");
```

### Thread-Safe Resource Loading
```cpp
class ResourceLoader {
private:
    URessource& m_resourceManager;

public:
    ResourceLoader() : m_resourceManager(URessource::GetInstance()) {}

    // This method can be called from any thread
    void LoadLevelResources(const std::vector<std::string>& resourcePaths) {
        for (const auto& path : resourcePaths) {
            // Thread-safe loading
            FTextureHandle tex = m_resourceManager.LoadTexture(path);

            if (tex) {
                // Resource loaded successfully
                std::cout << "Loaded: " << path << " (refs: "
                         << tex.GetRefCount() << ")" << std::endl;
            }
        }
    }
};

// Usage from multiple threads
std::thread loaderThread([]() {
    ResourceLoader loader;
    loader.LoadLevelResources({"level1_bg.png", "level1_tiles.png"});
});
```

### Resource Cleanup and Monitoring
```cpp
class ResourceMonitor {
private:
    URessource& m_resourceManager;

public:
    ResourceMonitor() : m_resourceManager(URessource::GetInstance()) {}

    void Update() {
        // Periodic cleanup of unused resources
        SizeT cleaned = m_resourceManager.CleanupUnusedResources();
        if (cleaned > 0) {
            std::cout << "Cleaned up " << cleaned << " unused resources" << std::endl;
        }
    }

    void ReportStats() {
        // Note: This would require additional methods to get cache sizes
        std::cout << "Resource Manager Stats:" << std::endl;
        std::cout << "- Loaded paks: "
                 << m_resourceManager.GetLoadedPaks().size() << std::endl;
        // Would need methods like GetTextureCount(), GetShaderCount()
    }
};
```

### Static Helper Usage
```cpp
// Direct texture loading without getting instance
FTextureHandle tex1 = URessource::GetTextureHandle("sprite.png");

// Template-based loading (currently texture-only)
auto tex2 = URessource::LoadResource<ITexture>("background.png");

// Both methods are equivalent and thread-safe
assert(tex1.Get() == tex2.Get());
```

### Advanced Shader Loading
```cpp
URessource& rm = URessource::GetInstance();

// Single shader file (compute shader)
FShaderHandle computeShader = rm.LoadShader(
    std::make_tuple("blur.compute", EShaderType::Compute)
);

// Vertex + fragment shaders
FShaderHandle basicShader = rm.LoadShader(
    std::make_tuple("basic.vert", "basic.frag")
);

// Full pipeline: vertex + geometry + fragment
FShaderHandle geometryShader = rm.LoadShader(
    std::make_tuple("terrain.vert", "terrain.geom", "terrain.frag")
);

// From asset
UAsset* shaderAsset = rm.LoadAsset("shaders/pbr.uasset");
FShaderHandle pbrShader = rm.LoadShader(shaderAsset);
```

### Resource Lifetime Management
```cpp
class GameLevel {
private:
    std::vector<FTextureHandle> m_levelTextures;
    std::vector<FShaderHandle> m_levelShaders;

public:
    void Load() {
        URessource& rm = URessource::GetInstance();

        // Load level-specific resources
        m_levelTextures.push_back(rm.LoadTexture("level_bg.png"));
        m_levelTextures.push_back(rm.LoadTexture("level_tiles.png"));
        m_levelShaders.push_back(rm.LoadShader(
            std::make_tuple("level.vert", "level.frag")
        ));

        std::cout << "Level loaded with "
                 << m_levelTextures.size() << " textures and "
                 << m_levelShaders.size() << " shaders" << std::endl;
    }

    void Render() {
        // Use resources - handles automatically manage lifetime
        for (const auto& tex : m_levelTextures) {
            if (tex) {
                // Render with texture
            }
        }
    }

    void Unload() {
        // Clear handles - resources may be kept alive if used elsewhere
        m_levelTextures.clear();
        m_levelShaders.clear();

        // Trigger cleanup of unused resources
        URessource::GetInstance().CleanupUnusedResources();
    }
};
```

## Mathematical Foundation

### Reference Counting
The resource manager uses `std::weak_ptr` for caching and `std::shared_ptr` for handles:

**Cache Storage**: `weak_ptr` prevents ownership, allows expiration detection
**Handle Distribution**: `shared_ptr` provides reference counting
**Automatic Cleanup**: Expired weak pointers are removed periodically

### Thread Safety Model
```
Operation → Lock Acquisition → Resource Access → Lock Release
    ↓              ↓                    ↓              ↓
   Load()   →   m_mutex.lock()   →   Cache Check   →   m_mutex.unlock()
   Get()    →   m_mutex.lock()   →   Lookup        →   m_mutex.unlock()
 Cleanup()  →   m_mutex.lock()   →   Iteration     →   m_mutex.unlock()
```

### Memory Management
- **Cache Maps**: O(1) average lookup time
- **Weak Pointers**: Zero overhead when resources are destroyed
- **Unique Pointers**: Exclusive ownership of pak files and assets
- **Cleanup Frequency**: Periodic O(n) cleanup passes

## Performance Considerations

### Memory Usage
- **Cache Overhead**: ~100+ bytes per cached resource entry
- **Weak Pointer Cost**: Minimal additional memory per cache entry
- **Pak File Storage**: Full pak file data in memory when loaded
- **Asset Storage**: Decompressed asset data cached

### Performance Characteristics
- **Lookup Time**: O(1) average for cached resources
- **Load Time**: Variable (file I/O, decompression, GPU upload)
- **Cleanup Time**: O(n) where n is cache size
- **Thread Contention**: Mutex lock/unlock overhead

### Optimization Strategies
1. **Cache Warming**: Pre-load commonly used resources
2. **Pak Files**: Use compressed archives for reduced I/O
3. **Resource Pooling**: Reuse handle objects
4. **Lazy Cleanup**: Don't clean up too frequently
5. **Async Loading**: Load resources in background threads

### Best Practices
- Set graphics factory early in initialization
- Use pak files for packaged game builds
- Call `CleanupUnusedResources()` periodically
- Cache frequently used resources
- Use appropriate shader loading variants

## Diagrams

### Resource Loading Flow
```
LoadTexture(path) Request
         │
         ▼
    ┌─────────────┐     No
    │ Path in     │─────────────┐
    │ Cache?      │             │
    └─────────────┘             │
         │                     │
        Yes                    ▼
         │            ┌─────────────┐
         │            │ Search Pak  │
         │            │ Files       │
         │            └─────────────┘
         │                     │
         ▼                     │
    ┌─────────────┐           No
    │ Return      │◄─────────────┘
    │ Cached      │
    │ Handle      │
    └─────────────┘
         ▲
         │
         ▼
    ┌─────────────┐     No
    │ Load from   │─────────────┐
    │ Disk/File   │             │
    └─────────────┘             │
         │                     │
        Yes                    ▼
         │            ┌─────────────┐
         ▼            │ Create      │
    ┌─────────────┐   │ Graphics    │
    │ Cache &     │◄──│ Resource    │
    │ Return      │   └─────────────┘
    │ Handle      │
    └─────────────┘
```

### Resource Management Architecture
```
┌─────────────────┐
│   Application   │
│                 │
│ ┌─────────────┐ │
│ │ UResource   │ │ ← Singleton
│ │ (Instance)  │ │
│ └─────────────┘ │
└─────────────────┘
         │
         ▼
    ┌─────────────┐
    │ Resource    │ ← TResourceHandle<T>
    │ Handles     │
    └─────────────┘
         │
         ▼
    ┌─────────────┐    ┌─────────────┐
    │ Weak Cache  │    │ Pak Files   │
    │ (Textures,  │    │             │
    │  Shaders)   │    │ ┌─────────┐ │
    │             │    │ │ UPak     │ │
    └─────────────┘    │ │ Archive  │ │
                       │ └─────────┘ │
                       └─────────────┘
                              │
                              ▼
                         ┌─────────────┐
                         │ Assets      │
                         │             │
                         │ ┌─────────┐ │
                         │ │ UAsset   │ │
                         │ │ Files    │ │
                         │ └─────────┘ │
                         └─────────────┘
```

### Thread Safety Model
```
Thread A: LoadTexture("tex.png")
    ↓
  Lock Mutex
    ↓
  Check Cache
    ↓
  Load/Create Resource
    ↓
  Update Cache
    ↓
 Unlock Mutex
    ↓
 Return Handle

Thread B: LoadTexture("other.png") ──► Blocked until Thread A unlocks
    ↓
  Lock Mutex (when available)
    ↓
  Continue safely
```

### Resource Lifetime Flow
```
Resource Creation:
1. LoadTexture() called
2. Resource created via GraphicsFactory
3. std::shared_ptr created
4. TResourceHandle created (ref count = 1)
5. Weak pointer stored in cache

Resource Sharing:
6. Another LoadTexture() for same resource
7. Cache hit, existing shared_ptr found
8. New TResourceHandle created (ref count = 2)
9. Both handles share same resource

Resource Cleanup:
10. First handle destroyed (ref count = 1)
11. Second handle destroyed (ref count = 0)
12. Resource destroyed
13. Weak pointer expires
14. CleanupUnusedResources() removes expired entry
```

## Integration with Rendering Pipeline

### Renderer Resource Management
```cpp
class Renderer {
private:
    URessource& m_resourceManager;

public:
    Renderer() : m_resourceManager(URessource::GetInstance()) {}

    void Initialize() {
        // Set graphics factory
        m_resourceManager.SetGraphicsFactory(m_graphicsFactory);
    }

    void LoadShaderProgram(const std::string& name,
                          const std::string& vertexPath,
                          const std::string& fragmentPath) {
        FShaderHandle shader = m_resourceManager.LoadShader(
            std::make_tuple(vertexPath, fragmentPath)
        );

        if (shader) {
            m_shaderPrograms[name] = shader;
        }
    }

    void LoadTextureAtlas(const std::string& name,
                         const std::string& texturePath) {
        FTextureHandle texture = m_resourceManager.LoadTexture(texturePath);

        if (texture) {
            m_textureAtlases[name] = texture;
        }
    }
};
```

### Game Asset Loading
```cpp
class AssetLoader {
private:
    URessource& m_resources;

public:
    AssetLoader() : m_resources(URessource::GetInstance()) {}

    bool LoadLevelAssets(const std::string& levelName) {
        std::string pakPath = levelName + ".pak";

        // Try to load from pak first
        if (m_resources.LoadPak(pakPath)) {
            // Load assets from pak
            LoadTexturesFromPak(levelName);
            LoadShadersFromPak(levelName);
            return true;
        }

        // Fall back to loose files
        LoadTexturesFromFiles(levelName);
        LoadShadersFromFiles(levelName);
        return true;
    }

private:
    void LoadTexturesFromPak(const std::string& levelName) {
        // Implementation would enumerate pak contents
        // and load appropriate textures
    }

    void LoadTexturesFromFiles(const std::string& levelName) {
        std::vector<std::string> textureFiles = {
            levelName + "_bg.png",
            levelName + "_tiles.png",
            levelName + "_sprites.png"
        };

        for (const auto& texFile : textureFiles) {
            m_resources.LoadTexture(texFile);
        }
    }
};
```

## Troubleshooting

### Common Issues

#### Graphics Factory Not Set
**Problem**: Texture/shader loading fails with invalid handles
**Solutions:**
- Ensure `SetGraphicsFactory()` is called before loading resources
- Verify graphics factory is properly initialized
- Check factory pointer is not null

#### Thread Safety Violations
**Problem**: Crashes or data corruption during concurrent access
**Solutions:**
- Always use UResource methods from main thread for graphics resources
- Use proper synchronization for non-graphics resources
- Avoid sharing resource handles across threads without care

#### Resource Leaks
**Problem**: Memory usage grows over time
**Solutions:**
- Call `CleanupUnusedResources()` periodically
- Ensure resource handles are properly scoped
- Check for circular references in complex objects

#### Pak File Loading Failures
**Problem**: Pak files fail to load or assets not found
**Solutions:**
- Verify pak file exists and is not corrupted
- Check file paths are correct
- Ensure pak file format is compatible

### Debug Visualization
```cpp
class ResourceDebugger {
private:
    URessource& m_resources;

public:
    ResourceDebugger() : m_resources(URessource::GetInstance()) {}

    void PrintStats() {
        std::cout << "=== Resource Manager Stats ===" << std::endl;

        // This would require additional methods to get actual counts
        auto paks = m_resources.GetLoadedPaks();
        std::cout << "Loaded Pak Files: " << paks.size() << std::endl;
        for (const auto& pak : paks) {
            std::cout << "  - " << pak.string() << std::endl;
        }

        std::cout << "Cleanup triggered: "
                 << m_resources.CleanupUnusedResources()
                 << " resources removed" << std::endl;
    }

    void ValidateResources() {
        // This would require methods to iterate through caches
        // and validate each resource handle
        std::cout << "Resource validation not yet implemented" << std::endl;
    }
};
```

## Future Enhancements

### Potential Features
- **Async Resource Loading**: Background loading with progress callbacks
- **Resource Streaming**: Load resources on-demand with LOD
- **Resource Dependencies**: Automatic loading of dependent assets
- **Hot Reloading**: Runtime resource replacement for development
- **Resource Profiling**: Detailed memory and performance tracking
- **Compression**: Runtime decompression for memory efficiency

### API Extensions
```cpp
// Potential future methods
class UResource {
    // Async loading
    using LoadCallback = std::function<void(FTextureHandle)>;
    void LoadTextureAsync(const FilePath& path, LoadCallback callback);

    // Resource enumeration
    std::vector<FString> GetLoadedTextures() const;
    std::vector<FString> GetLoadedShaders() const;

    // Memory management
    SizeT GetMemoryUsage() const;
    void SetMemoryBudget(SizeT bytes);

    // Development features
    void EnableHotReloading(bool enable);
    void ReloadResource(const FString& id);

    // Statistics
    struct ResourceStats {
        SizeT textureCount;
        SizeT shaderCount;
        SizeT totalMemoryUsage;
        SizeT cacheHitRate;
    };
    ResourceStats GetStats() const;
};
```
