# FLibrary

## Overview

The `FLibrary` class provides a cross-platform abstraction for loading and managing dynamic libraries (shared objects/DLLs). It offers a safe, RAII-compliant interface for runtime library loading with automatic resource management, symbol caching, and comprehensive error handling. The class maintains a global registry of loaded libraries to prevent duplicate loading and ensure proper cleanup.

## Basic Usage

### Loading a Library

```cpp
#include <Engine/Core/Utils/FLibrary.hpp>

int main() {
    // Load a library by path
    auto library = FLibrary::Load("mylib.so");  // or "mylib.dll" on Windows

    if (!library) {
        std::cerr << "Failed to load library" << std::endl;
        return 1;
    }

    // Library is automatically unloaded when it goes out of scope
    return 0;
}
```

### Loading by Name

```cpp
// Load by name - searches standard locations
auto library = FLibrary::LoadFromName("mylib");

// On Unix: tries mylib.so, libmylib.so, ./libmylib.so, etc.
// On Windows: tries mylib.dll
```

### Getting Functions

```cpp
// Define function signature
using CreateObjectFunc = void* (*)();

// Get function pointer
CreateObjectFunc createFunc = nullptr;
if (library->GetFunction("CreateObject", createFunc) == ELibraryStatus::Success) {
    // Use the function
    void* obj = createFunc();
} else {
    std::cerr << "Function not found: " << library->GetLastError() << std::endl;
}

// Or use function wrapper for easier usage
auto createFuncWrapper = library->GetFunctionWrapper<void*()>("CreateObject");
if (createFuncWrapper) {
    void* obj = createFuncWrapper();
}
```

## Library Management

### Loading Libraries

#### `static FLibraryPtr Load(const FilePath& path)`

Loads a dynamic library from the specified path.

**Parameters:**
- `path`: Absolute or relative path to the library file

**Returns:** Shared pointer to the loaded library, or `nullptr` if loading failed

**Behavior:**
- Normalizes the path and checks for existing loaded instances
- Returns cached instance if already loaded
- Creates new instance and loads the library
- Registers the library in the global registry

```cpp
// Load with absolute path
auto lib1 = FLibrary::Load("/usr/lib/libmylib.so");

// Load with relative path
auto lib2 = FLibrary::Load("./plugins/plugin.dll");

// Check if already loaded
if (FLibrary::IsLibraryLoaded("mylib.so")) {
    std::cout << "Library already loaded" << std::endl;
}
```

#### `static FLibraryPtr LoadFromName(const FString& name)`

Loads a library by searching for it in standard locations.

**Search Order:**
1. `name` (exact name)
2. `name + extension` (e.g., "mylib.so")
3. `lib + name + extension` (e.g., "libmylib.so")
4. `./lib + name + extension`
5. `libs/ + name + extension`
6. `plugins/ + name + extension`

**Returns:** Shared pointer to the loaded library, or `nullptr` if not found

```cpp
// Load "mylib" - will find libmylib.so on Linux
auto library = FLibrary::LoadFromName("mylib");
```

### Unloading Libraries

#### `void Unload(void)`

Unloads the currently loaded library.

**Behavior:**
- Calls the platform-specific unload function
- Clears cached symbols
- Resets internal state
- Library remains in registry until all shared pointers are destroyed

```cpp
auto library = FLibrary::Load("mylib.so");
// ... use library ...
library->Unload();  // Explicit unload
```

#### `static void UnloadAll(void)`

Clears the global library registry.

**Behavior:**
- Does not immediately unload libraries
- Prevents future `Load()` calls from finding cached instances
- Libraries are unloaded when their shared pointers are destroyed

```cpp
// Force cleanup of all libraries
FLibrary::UnloadAll();
```

### Library State

#### `bool IsLoaded(void) const`

Checks if the library is currently loaded.

**Returns:** `true` if loaded, `false` otherwise

```cpp
if (library->IsLoaded()) {
    // Safe to get functions
}
```

#### `static bool IsLibraryLoaded(const FilePath& path)`

Checks if a library is already loaded globally.

**Parameters:**
- `path`: Path to check

**Returns:** `true` if loaded, `false` otherwise

## Function Access

### Getting Raw Function Pointers

#### `void* GetRawFunction(const FString& name) const`

Retrieves a raw function pointer from the library.

**Parameters:**
- `name`: Name of the function to retrieve

**Returns:** Function pointer, or `nullptr` if not found

**Behavior:**
- Caches results to improve performance
- Returns cached pointer on subsequent calls
- Sets error message if function not found

```cpp
void* funcPtr = library->GetRawFunction("MyFunction");
if (funcPtr) {
    // Cast and use
    using FuncType = int (*)(int);
    FuncType func = reinterpret_cast<FuncType>(funcPtr);
    int result = func(42);
}
```

### Type-Safe Function Retrieval

#### `ELibraryStatus GetFunction(const FString& name, FuncType*& out) const`

Retrieves a function and casts it to the specified type.

**Template Parameters:**
- `FuncType`: Function pointer type (e.g., `void (*)()`)

**Parameters:**
- `name`: Function name
- `out`: Reference to store the function pointer

**Returns:** `ELibraryStatus` indicating success or failure

```cpp
// Define function signature
using MathFunc = int (*)(int, int);

// Get typed function pointer
MathFunc addFunc = nullptr;
auto status = library->GetFunction("AddNumbers", addFunc);

if (status == ELibraryStatus::Success) {
    int result = addFunc(5, 3);  // result = 8
}
```

### Function Wrappers

#### `std::function<FuncType> GetFunctionWrapper(const FString& name) const`

Retrieves a function wrapped in `std::function` for easier usage.

**Template Parameters:**
- `FuncType`: Function signature (e.g., `int(int, int)`)

**Parameters:**
- `name`: Function name

**Returns:** `std::function` object, or empty function if not found

```cpp
// Get function wrapper
auto multiply = library->GetFunctionWrapper<int(int, int)>("Multiply");

if (multiply) {
    int result = multiply(4, 7);  // result = 28
} else {
    std::cerr << "Function not found" << std::endl;
}
```

### Function Existence Check

#### `bool HasFunction(const FString& name) const`

Checks if a function exists in the library.

**Parameters:**
- `name`: Function name to check

**Returns:** `true` if function exists, `false` otherwise

```cpp
if (library->HasFunction("Initialize")) {
    // Function exists, safe to call
}
```

## Error Handling

### Error Messages

#### `const FString& GetLastError(void) const`

Retrieves the last error message.

**Returns:** Last error message, or empty string if no error

```cpp
auto library = FLibrary::Load("nonexistent.so");
if (!library) {
    std::cout << "Load failed: " << library->GetLastError() << std::endl;
}
```

### Status Codes

The `ELibraryStatus` enum provides detailed status information:

```cpp
enum class ELibraryStatus : UInt8 {
    Success,          // Operation succeeded
    FileNotFound,     // Library file not found
    LoadFailed,       // Library load failed
    SymbolNotFound,   // Function symbol not found
    AlreadyLoaded,    // Library already loaded
    NotLoaded,        // Library not loaded
    InvalidPath,      // Invalid library path
    UnknownError      // Unknown error occurred
};
```

## Library Information

### Path and Name Access

#### `const FilePath& GetPath(void) const`

Gets the normalized path of the library.

**Returns:** Library file path

#### `const FString& GetName(void) const`

Gets the extracted name of the library (without path or extension).

**Returns:** Library name

```cpp
auto library = FLibrary::Load("/usr/lib/libmylib.so");
std::cout << "Path: " << library->GetPath() << std::endl;  // "/usr/lib/libmylib.so"
std::cout << "Name: " << library->GetName() << std::endl;  // "mylib"
```

### Platform Information

#### `static FString GetPlatformExtension(void)`

Gets the platform-specific library extension.

**Returns:** `".so"` on Unix-like systems, `".dll"` on Windows

```cpp
std::string extension = FLibrary::GetPlatformExtension();
// Use when constructing library paths
std::string libPath = "mylib" + extension;
```

## Advanced Usage

### Plugin System Implementation

```cpp
class PluginManager {
private:
    std::vector<FLibrary::FLibraryPtr> m_plugins;

public:
    bool LoadPlugin(const std::string& name) {
        auto plugin = FLibrary::LoadFromName(name);
        if (!plugin) {
            std::cerr << "Failed to load plugin '" << name << "': "
                      << plugin->GetLastError() << std::endl;
            return false;
        }

        // Check for required interface
        using InitFunc = bool (*)();
        InitFunc initFunc = nullptr;

        if (plugin->GetFunction("Initialize", initFunc) != ELibraryStatus::Success) {
            std::cerr << "Plugin missing Initialize function" << std::endl;
            return false;
        }

        // Initialize plugin
        if (!initFunc()) {
            std::cerr << "Plugin initialization failed" << std::endl;
            return false;
        }

        m_plugins.push_back(plugin);
        return true;
    }

    void UnloadAllPlugins() {
        m_plugins.clear();  // Libraries unloaded automatically
    }
};
```

### Factory Pattern with Dynamic Loading

```cpp
class ComponentFactory {
public:
    using CreateFunc = IComponent* (*)();
    using DestroyFunc = void (*)(IComponent*);

    std::unique_ptr<IComponent> CreateComponent(const std::string& type) {
        std::string libName = "component_" + type;
        auto library = FLibrary::LoadFromName(libName);

        if (!library) return nullptr;

        CreateFunc create = nullptr;
        DestroyFunc destroy = nullptr;

        if (library->GetFunction("CreateComponent", create) != ELibraryStatus::Success ||
            library->GetFunction("DestroyComponent", destroy) != ELibraryStatus::Success) {
            return nullptr;
        }

        IComponent* component = create();
        // Store destroy function for later cleanup
        // ...

        return std::unique_ptr<IComponent>(component);
    }
};
```

### Cross-Platform Library Loading

```cpp
FLibrary::FLibraryPtr LoadLibraryCrossPlatform(const std::string& baseName) {
    // Try different naming conventions
    std::vector<std::string> candidates = {
        baseName,
        baseName + FLibrary::GetPlatformExtension(),
        "lib" + baseName + FLibrary::GetPlatformExtension()
    };

    for (const auto& candidate : candidates) {
        auto library = FLibrary::Load(candidate);
        if (library) return library;
    }

    return nullptr;
}
```

## Memory Management

### RAII Compliance

The `FLibrary` class follows RAII principles:

```cpp
void ProcessData() {
    auto lib = FLibrary::Load("data_processor.so");
    if (!lib) return;

    // Library loaded here
    auto processFunc = lib->GetFunctionWrapper<void(const char*)>("ProcessData");
    if (processFunc) {
        processFunc("input data");
    }
    // Library automatically unloaded here
}
```

### Shared Ownership

Libraries use shared pointers for automatic reference counting:

```cpp
auto lib1 = FLibrary::Load("mylib.so");
// Reference count = 1

auto lib2 = lib1;  // Shared ownership
// Reference count = 2

lib1.reset();  // Reference count = 1
// Library still loaded

lib2.reset();  // Reference count = 0
// Library unloaded
```

### Global Registry

The global registry prevents duplicate loading:

```cpp
auto lib1 = FLibrary::Load("mylib.so");  // Loads library
auto lib2 = FLibrary::Load("mylib.so");  // Returns cached instance

// lib1 and lib2 point to the same library instance
assert(lib1.get() == lib2.get());
```

## Error Handling Best Practices

### Comprehensive Error Checking

```cpp
FLibrary::FLibraryPtr LoadWithDetailedError(const std::string& path) {
    auto library = FLibrary::Load(path);

    if (!library) {
        // Check what went wrong
        if (!FileSystem::FileExists(path)) {
            std::cerr << "Library file does not exist: " << path << std::endl;
        } else {
            // Create temporary instance to get error
            FLibrary tempLib(path);
            tempLib.Load();
            std::cerr << "Failed to load library: " << tempLib.GetLastError() << std::endl;
        }
        return nullptr;
    }

    return library;
}
```

### Function Loading with Fallbacks

```cpp
template<typename FuncType>
FuncType* GetFunctionWithFallback(FLibrary& lib,
                                  const std::vector<std::string>& names) {
    for (const auto& name : names) {
        FuncType* func = nullptr;
        if (lib.GetFunction(name, func) == ELibraryStatus::Success) {
            return func;
        }
    }
    return nullptr;
}

// Usage
auto createFunc = GetFunctionWithFallback<CreateObjectFunc>(
    *library, {"CreateObject", "create_object", "Create", "create"});
```

## Performance Considerations

### Symbol Caching

Functions are cached after first lookup:

```cpp
// First call: resolves symbol from library
void* func1 = library->GetRawFunction("MyFunction");

// Second call: returns cached pointer
void* func2 = library->GetRawFunction("MyFunction");

// func1 == func2 (same pointer)
```

### Library Registry

Global registry prevents redundant loading:

```cpp
// Only loads once, even with multiple calls
auto lib1 = FLibrary::Load("heavy_lib.so");  // Loads library
auto lib2 = FLibrary::Load("heavy_lib.so");  // Returns cached instance
```

## Platform-Specific Behavior

### Windows (DLLs)

- Uses `LoadLibraryA`, `GetProcAddress`, `FreeLibrary`
- Extension: `.dll`
- Error messages from `FormatMessageA`

### Unix-like Systems (Shared Objects)

- Uses `dlopen`, `dlsym`, `dlclose`
- Extension: `.so`
- Error messages from `dlerror`

### Path Handling

- Automatic path normalization
- Backslash to forward slash conversion
- Absolute path resolution when possible

## Limitations

### Function Pointer Safety

- No type safety guarantees for `GetRawFunction`
- Incorrect casting can cause crashes
- Use `GetFunction<T>` for type safety

### Library Dependencies

- Does not handle library dependencies automatically
- Dependent libraries must be loaded separately
- Load order may be important

### Thread Safety

- Library loading is not thread-safe
- Concurrent access to the same library instance is safe
- Global registry access should be synchronized in multi-threaded applications

### Error Message Limitations

- System error messages may not always be descriptive
- Error state is per-instance, not thread-local

## Integration with Engine

### Plugin Architecture

```cpp
class EnginePlugin {
public:
    virtual ~EnginePlugin() = default;
    virtual bool Initialize() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Shutdown() = 0;
};

class PluginLoader {
private:
    std::vector<FLibrary::FLibraryPtr> m_libraries;
    std::vector<std::unique_ptr<EnginePlugin>> m_plugins;

public:
    bool LoadPlugin(const std::string& pluginName) {
        auto library = FLibrary::LoadFromName(pluginName);
        if (!library) return false;

        // Get plugin factory function
        using CreatePluginFunc = EnginePlugin* (*)();
        CreatePluginFunc createFunc = nullptr;

        if (library->GetFunction("CreatePlugin", createFunc) != ELibraryStatus::Success) {
            return false;
        }

        // Create plugin instance
        EnginePlugin* plugin = createFunc();
        if (!plugin) return false;

        // Initialize plugin
        if (!plugin->Initialize()) {
            delete plugin;
            return false;
        }

        // Store both library and plugin
        m_libraries.push_back(library);
        m_plugins.emplace_back(plugin);

        return true;
    }

    void UpdateAll(float deltaTime) {
        for (auto& plugin : m_plugins) {
            plugin->Update(deltaTime);
        }
    }

    void ShutdownAll() {
        for (auto& plugin : m_plugins) {
            plugin->Shutdown();
        }
        m_plugins.clear();
        m_libraries.clear();  // Unloads libraries
    }
};
```

This implementation provides a robust foundation for building extensible applications with dynamic plugin loading capabilities.
