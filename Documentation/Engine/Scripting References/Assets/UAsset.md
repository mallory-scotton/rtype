# UAsset Class Documentation

## Overview

The `UAsset` class represents an asset in the TKD Engine, providing comprehensive management of asset metadata and data. The class is designed for efficiency by loading only the asset header (metadata) by default, with the actual asset data loaded on demand. This approach allows for efficient asset tracking and memory management.

Assets can be sourced from two locations:
- **Standalone files**: Individual `.uasset` files stored on disk
- **Package files**: Assets embedded within `.pak` package files for optimized distribution

The class handles the binary `.uasset` file format, which includes metadata such as UUID, name, type, and the raw asset data.

## File Format (.uasset)

The `.uasset` file uses a binary format with the following structure:

- **Magic Number** (4 bytes): `0x54455355` ("USET" in ASCII)
- **Version** (2 bytes): Format version number (current: 1)
- **Asset Type** (1 byte): Value from `EAssetType` enum
- **UUID Length** (4 bytes) + **UUID Data** (variable length)
- **Name Length** (4 bytes) + **Name Data** (variable length)
- **Path Length** (4 bytes) + **Path Data** (variable length)
- **Data Size** (8 bytes): Size of asset data in bytes
- **Asset Data** (variable length): Raw binary data

## Enumerations

### EAssetType

Defines the type of asset:

```cpp
enum class EAssetType : UInt8
{
    Unknown = 0,   // Unknown asset type
    Level,         // Level/map asset
    Texture,       // Texture/image asset
    Audio          // Audio/sound asset
};
```

### EAssetSource

Defines the source location of the asset:

```cpp
enum class EAssetSource : UInt8
{
    Standalone,   // Asset loaded from standalone .uasset file
    Package       // Asset loaded from .pak package file
};
```

## Class Interface

### Public Constructors

#### UAsset(const FilePath& uassetPath)

Creates a `UAsset` instance by parsing a standalone `.uasset` file. Only the header metadata is loaded during construction; the actual asset data remains unloaded until explicitly requested.

**Parameters:**
- `uassetPath`: File path to the `.uasset` file

**Throws:**
- `std::runtime_error` if the file cannot be opened or parsed

**Example:**
```cpp
#include <Engine/Assets/UAsset.hpp>

try {
    tkd::UAsset asset("/path/to/my_asset.uasset");
    // Asset metadata is now available, but data is not loaded
} catch (const std::runtime_error& e) {
    std::cerr << "Failed to load asset: " << e.what() << std::endl;
}
```

#### UAsset(UPak* pakFile, const FString& uuid, const FString& name, EAssetType type, UInt64 offset, UInt64 size)

Creates a `UAsset` instance referencing an asset within a `.pak` package file. The asset data will be loaded from the pak file at the specified offset when `Load()` is called.

**Parameters:**
- `pakFile`: Pointer to the parent `UPak` instance
- `uuid`: Unique identifier of the asset
- `name`: Display name of the asset
- `type`: Asset type from `EAssetType` enum
- `offset`: Byte offset within the pak file where asset data begins
- `size`: Size of the asset data in bytes

**Example:**
```cpp
#include <Engine/Assets/UAsset.hpp>
#include <Engine/Assets/UPak.hpp>

// Assuming pakFile is a valid UPak instance
tkd::UAsset asset(pakFile, "asset-uuid-123", "My Texture", tkd::EAssetType::Texture, 1024, 4096);
```

#### UAsset()

Default constructor creating an empty asset instance. Use this when manually setting up asset metadata and data.

**Example:**
```cpp
#include <Engine/Assets/UAsset.hpp>

tkd::UAsset asset;  // Empty asset, ready for manual setup
asset.SetMetadata("custom-uuid", "Custom Asset", tkd::EAssetType::Unknown);
```

### Public Methods

#### bool Load(void)

Loads the asset data into memory. If the asset is already loaded, returns `true` immediately without reloading.

**Returns:**
- `true` if the asset data was loaded successfully
- `false` if loading failed (file not found, corrupted data, etc.)

**Example:**
```cpp
tkd::UAsset asset("/path/to/asset.uasset");

if (asset.Load()) {
    std::cout << "Asset loaded successfully. Size: " << asset.GetSize() << " bytes" << std::endl;
    // Asset data is now available via GetData()
} else {
    std::cerr << "Failed to load asset" << std::endl;
}
```

#### bool Unload(void)

Unloads the asset data from memory while preserving all metadata. The asset can be reloaded later using `Load()`.

**Returns:**
- `true` if the asset was unloaded successfully
- `false` if unloading failed

**Example:**
```cpp
tkd::UAsset asset("/path/to/large_asset.uasset");
asset.Load();  // Load data
// ... use asset ...
asset.Unload();  // Free memory, keep metadata
// Asset can be reloaded later if needed
```

#### bool Save(const FilePath& savePath)

Saves the current asset (metadata and data) to a `.uasset` file at the specified path.

**Parameters:**
- `savePath`: File path where the asset should be saved

**Returns:**
- `true` if the asset was saved successfully
- `false` if saving failed

**Example:**
```cpp
tkd::UAsset asset;
asset.SetMetadata("texture-001", "Background Texture", tkd::EAssetType::Texture);

// Load or set asset data
std::vector<tkd::Byte> textureData = loadTextureFromFile("/path/to/texture.png");
asset.SetData(textureData);

// Save to .uasset format
if (asset.Save("/path/to/background.uasset")) {
    std::cout << "Asset saved successfully" << std::endl;
}
```

#### Getter Methods

##### FString GetUUID(void) const

Returns the unique identifier of the asset.

**Returns:** Asset UUID as a string

**Example:**
```cpp
tkd::UAsset asset("/path/to/asset.uasset");
tkd::FString uuid = asset.GetUUID();
std::cout << "Asset UUID: " << uuid.CStr() << std::endl;
```

##### EAssetType GetType(void) const

Returns the type of the asset.

**Returns:** Asset type from `EAssetType` enum

**Example:**
```cpp
tkd::UAsset asset("/path/to/texture.uasset");
tkd::EAssetType type = asset.GetType();

if (type == tkd::EAssetType::Texture) {
    std::cout << "This is a texture asset" << std::endl;
}
```

##### SizeT GetSize(void) const

Returns the size of the asset data in bytes.

**Returns:** Size of asset data in bytes

**Note:** This returns the size of the loaded data. If the asset is not loaded, it returns the size from the file header.

**Example:**
```cpp
tkd::UAsset asset("/path/to/asset.uasset");
std::cout << "Asset size: " << asset.GetSize() << " bytes" << std::endl;
```

##### FString GetName(void) const

Returns the display name of the asset.

**Returns:** Asset name as a string

**Example:**
```cpp
tkd::UAsset asset("/path/to/asset.uasset");
tkd::FString name = asset.GetName();
std::cout << "Asset name: " << name.CStr() << std::endl;
```

##### FilePath GetPath(void) const

Returns the file path of the asset.

**Returns:** File path (for standalone assets) or pak file path (for packaged assets)

**Example:**
```cpp
tkd::UAsset asset("/path/to/asset.uasset");
std::filesystem::path path = asset.GetPath();
std::cout << "Asset path: " << path.string() << std::endl;
```

##### bool IsLoaded(void) const

Checks if the asset data is currently loaded in memory.

**Returns:**
- `true` if asset data is loaded
- `false` if only metadata is available

**Example:**
```cpp
tkd::UAsset asset("/path/to/asset.uasset");

if (!asset.IsLoaded()) {
    asset.Load();  // Load data if not already loaded
}

// Now safe to access GetData()
```

##### const std::vector<Byte>& GetData(void) const

Returns a reference to the raw asset data.

**Returns:** Constant reference to the asset data vector

**Note:** Ensure the asset is loaded before calling this method.

**Example:**
```cpp
tkd::UAsset asset("/path/to/texture.uasset");
asset.Load();

const std::vector<tkd::Byte>& data = asset.GetData();
// Process texture data...
processTexture(data.data(), data.size());
```

##### EAssetSource GetSource(void) const

Returns the source type of the asset.

**Returns:** Source type from `EAssetSource` enum

**Example:**
```cpp
tkd::UAsset asset("/path/to/asset.uasset");

if (asset.GetSource() == tkd::EAssetSource::Package) {
    std::cout << "Asset is loaded from a pak file" << std::endl;
} else {
    std::cout << "Asset is a standalone file" << std::endl;
}
```

##### bool IsFromPak(void) const

Convenience method to check if the asset originates from a pak file.

**Returns:**
- `true` if asset is from a pak file
- `false` if asset is standalone

**Example:**
```cpp
tkd::UAsset asset("/path/to/asset.uasset");

if (asset.IsFromPak()) {
    // Handle pak-specific logic
    std::cout << "Asset is packaged" << std::endl;
}
```

#### Setter Methods

##### void SetData(const std::vector<Byte>& data)

Sets the asset data by copying the provided vector.

**Parameters:**
- `data`: Vector containing the asset data

**Effects:**
- Sets the asset data
- Updates the internal size
- Marks the asset as loaded

**Example:**
```cpp
tkd::UAsset asset;
asset.SetMetadata("audio-001", "Background Music", tkd::EAssetType::Audio);

std::vector<tkd::Byte> audioData = loadAudioFile("/path/to/music.mp3");
asset.SetData(audioData);  // Copy data
```

##### void SetData(std::vector<Byte>&& data)

Sets the asset data by moving the provided vector (more efficient for large data).

**Parameters:**
- `data`: Vector containing the asset data (moved)

**Effects:**
- Moves the asset data
- Updates the internal size
- Marks the asset as loaded

**Example:**
```cpp
tkd::UAsset asset;
asset.SetMetadata("texture-001", "Background", tkd::EAssetType::Texture);

std::vector<tkd::Byte> textureData = loadTextureData("/path/to/texture.png");
asset.SetData(std::move(textureData));  // Move data for efficiency
```

##### void SetMetadata(const FString& uuid, const FString& name, EAssetType type)

Sets the asset metadata (UUID, name, and type).

**Parameters:**
- `uuid`: Unique identifier for the asset
- `name`: Display name for the asset
- `type`: Asset type from `EAssetType` enum

**Example:**
```cpp
tkd::UAsset asset;
asset.SetMetadata("level-001", "Main Level", tkd::EAssetType::Level);

// Now set data and save
std::vector<tkd::Byte> levelData = createLevelData();
asset.SetData(std::move(levelData));
asset.Save("/path/to/main_level.uasset");
```

## Usage Examples

### Loading and Using a Texture Asset

```cpp
#include <Engine/Assets/UAsset.hpp>

// Load texture asset
tkd::UAsset textureAsset("/assets/textures/player_sprite.uasset");

// Load the data
if (textureAsset.Load()) {
    // Get texture data
    const auto& textureData = textureAsset.GetData();
    
    // Create texture from data
    Texture* texture = createTextureFromData(textureData.data(), textureData.size());
    
    // Use texture in rendering
    renderer->bindTexture(texture);
    
    // Unload when done to free memory
    textureAsset.Unload();
}
```

### Creating and Saving a Custom Asset

```cpp
#include <Engine/Assets/UAsset.hpp>

// Create new asset
tkd::UAsset customAsset;
customAsset.SetMetadata("custom-001", "My Custom Data", tkd::EAssetType::Unknown);

// Generate or load data
std::vector<tkd::Byte> customData = generateCustomData();
customAsset.SetData(std::move(customData));

// Save to file
if (customAsset.Save("/assets/custom/my_asset.uasset")) {
    std::cout << "Custom asset saved successfully" << std::endl;
}
```

### Working with Pak File Assets

```cpp
#include <Engine/Assets/UAsset.hpp>
#include <Engine/Assets/UPak.hpp>

// Open pak file
tkd::UPak pakFile("/assets/game_assets.pak");

// Create asset reference (assuming we know the asset details)
tkd::UAsset asset(&pakFile, "texture-uuid", "Menu Background", 
                  tkd::EAssetType::Texture, 1024, 2048);

// Load from pak
if (asset.Load()) {
    // Asset data is now loaded from pak file
    const auto& data = asset.GetData();
    // Process data...
}
```

## Notes

- Asset data is only loaded into memory when explicitly requested via `Load()`
- Use `Unload()` to free memory when asset data is no longer needed
- The class handles both standalone `.uasset` files and packaged assets from `.pak` files
- All string operations use the engine's `FString` class for consistency
- File I/O operations are performed in binary mode for data integrity
