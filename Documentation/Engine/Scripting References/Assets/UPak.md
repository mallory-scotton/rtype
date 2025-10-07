# UPak Class Documentation

## Overview

The `UPak` class manages a collection of assets stored in a single `.pak` file. It provides efficient access to individual assets through an index table stored at the beginning of the file. This class is part of the TKD Engine's asset management system.

### File Format (.pak)

The `.pak` file format consists of the following structure:

- **Magic Number** (4 bytes): `0x5041434B` ("PACK")
- **Version** (2 bytes): Format version (currently 1)
- **Entry Count** (4 bytes): Number of assets in the package
- **Index Table**: Contains metadata for each asset
  - For each entry:
    - UUID Length (4 bytes) + UUID Data
    - Name Length (4 bytes) + Name Data
    - Asset Type (1 byte)
    - Data Offset (8 bytes)
    - Data Size (8 bytes)
    - Compressed Size (8 bytes)
    - Is Compressed (1 byte)
- **Asset Data Section**: Raw binary data for all assets stored sequentially

## FPakEntry Structure

The `FPakEntry` struct represents an entry in the package file, containing metadata about an asset.

### Members

- `FString uuid`: Unique identifier of the asset
- `FString name`: Name of the asset
- `EAssetType type`: Type of the asset
- `UInt64 offset`: Offset in the pak file where asset data starts
- `UInt64 size`: Size of the asset data in bytes
- `UInt64 compressedSize`: Compressed size (0 if not compressed)
- `bool isCompressed`: Flag indicating if asset is compressed

### Constructor

```cpp
FPakEntry(void)
```

Default constructor initializing all members to default values.

## UPak Class

### Constructors

#### Default Constructor

```cpp
UPak(void)
```

Creates an empty UPak instance that is not associated with any file.

#### Constructor with Path

```cpp
UPak(const FilePath& pakPath)
```

Creates a UPak instance and attempts to open the specified pak file.

**Parameters:**
- `pakPath`: The file path to the pak file

### Destructor

```cpp
~UPak()
```

Closes the pak file and cleans up resources.

### Public Methods

#### Open

```cpp
bool Open(const FilePath& pakPath)
```

Opens and parses a pak file. If another file is already open, it will be closed first.

**Parameters:**
- `pakPath`: The file path to the pak file

**Returns:**
- `true` if the pak was opened successfully, `false` otherwise

**Example:**
```cpp
UPak pak;
if (pak.Open("assets/game_assets.pak")) {
    // Pak file opened successfully
} else {
    // Failed to open pak file
}
```

#### Close

```cpp
void Close(void)
```

Closes the pak file and clears the index. After calling this method, the UPak instance is in the same state as a newly constructed instance.

**Example:**
```cpp
UPak pak("assets/game_assets.pak");
// ... use pak ...
pak.Close(); // Pak is now closed
```

#### Create

```cpp
bool Create(const FilePath& pakPath, const std::vector<UAsset*>& assets)
```

Creates a new pak file from a list of assets. This method writes the pak file format to disk and then opens it.

**Parameters:**
- `pakPath`: The file path where the pak should be saved
- `assets`: Vector of assets to include in the package

**Returns:**
- `true` if the pak was created successfully, `false` otherwise

**Note:** All assets in the vector must be loaded (i.e., `asset->IsLoaded()` returns `true`) for the creation to succeed.

**Example:**
```cpp
std::vector<UAsset*> assets;
// ... populate assets vector ...
UPak pak;
if (pak.Create("output/new_assets.pak", assets)) {
    // Pak file created and opened successfully
}
```

#### AddAsset

```cpp
bool AddAsset(UAsset* asset)
```

Adds an asset to an existing pak file.

**Parameters:**
- `asset`: The asset to add to the package

**Returns:**
- `true` if the asset was added successfully, `false` otherwise

**Note:** This method is currently not implemented and always returns `false`. Use `Create()` with all assets instead.

#### LoadAssetData

```cpp
bool LoadAssetData(const FString& uuid, std::vector<Byte>& outData)
```

Loads an asset's data from the pak file by UUID.

**Parameters:**
- `uuid`: The unique identifier of the asset
- `outData`: Output vector to store the loaded data

**Returns:**
- `true` if the asset was loaded successfully, `false` otherwise

**Example:**
```cpp
UPak pak("assets/game_assets.pak");
std::vector<Byte> textureData;
if (pak.LoadAssetData("texture-uuid-123", textureData)) {
    // Texture data loaded successfully
    // Use textureData...
}
```

#### LoadAssetDataByName

```cpp
bool LoadAssetDataByName(const FString& name, std::vector<Byte>& outData)
```

Loads an asset's data from the pak file by name.

**Parameters:**
- `name`: The name of the asset
- `outData`: Output vector to store the loaded data

**Returns:**
- `true` if the asset was loaded successfully, `false` otherwise

**Example:**
```cpp
UPak pak("assets/game_assets.pak");
std::vector<Byte> soundData;
if (pak.LoadAssetDataByName("background_music.wav", soundData)) {
    // Sound data loaded successfully
    // Use soundData...
}
```

#### CreateAsset

```cpp
std::unique_ptr<UAsset> CreateAsset(const FString& uuid)
```

Creates a UAsset instance from pak data by UUID. The created asset contains metadata and provides methods to load the actual data on demand.

**Parameters:**
- `uuid`: The unique identifier of the asset

**Returns:**
- Unique pointer to the created asset, or `nullptr` on failure

**Example:**
```cpp
UPak pak("assets/game_assets.pak");
auto asset = pak.CreateAsset("model-uuid-456");
if (asset) {
    // Asset created successfully
    // asset->GetName(), asset->GetType(), etc.
}
```

#### CreateAssetByName

```cpp
std::unique_ptr<UAsset> CreateAssetByName(const FString& name)
```

Creates a UAsset instance from pak data by name.

**Parameters:**
- `name`: The name of the asset

**Returns:**
- Unique pointer to the created asset, or `nullptr` on failure

**Example:**
```cpp
UPak pak("assets/game_assets.pak");
auto textureAsset = pak.CreateAssetByName("hero_texture.png");
if (textureAsset) {
    // Texture asset created successfully
}
```

#### HasAsset

```cpp
bool HasAsset(const FString& uuid) const
```

Checks if an asset exists in the pak by UUID.

**Parameters:**
- `uuid`: The unique identifier of the asset

**Returns:**
- `true` if the asset exists, `false` otherwise

**Example:**
```cpp
UPak pak("assets/game_assets.pak");
if (pak.HasAsset("level-1-uuid")) {
    // Asset exists, can load it
}
```

#### HasAssetByName

```cpp
bool HasAssetByName(const FString& name) const
```

Checks if an asset exists in the pak by name.

**Parameters:**
- `name`: The name of the asset

**Returns:**
- `true` if the asset exists, `false` otherwise

**Example:**
```cpp
UPak pak("assets/game_assets.pak");
if (pak.HasAssetByName("main_menu_background.jpg")) {
    // Asset exists by name
}
```

#### GetEntry

```cpp
const FPakEntry* GetEntry(const FString& uuid) const
```

Gets the entry information for an asset by UUID.

**Parameters:**
- `uuid`: The unique identifier of the asset

**Returns:**
- Pointer to the entry, or `nullptr` if not found

**Example:**
```cpp
UPak pak("assets/game_assets.pak");
const FPakEntry* entry = pak.GetEntry("asset-uuid");
if (entry) {
    UInt64 size = entry->size;
    EAssetType type = entry->type;
    // Use entry information...
}
```

#### GetAllEntries

```cpp
std::vector<FPakEntry> GetAllEntries(void) const
```

Gets all entries in the pak file.

**Returns:**
- Vector containing all pak entries

**Example:**
```cpp
UPak pak("assets/game_assets.pak");
std::vector<FPakEntry> allEntries = pak.GetAllEntries();
for (const auto& entry : allEntries) {
    // Process each entry
    // entry.name, entry.uuid, etc.
}
```

#### GetEntryCount

```cpp
UInt32 GetEntryCount(void) const
```

Gets the number of assets in the pak.

**Returns:**
- Number of assets in the pak

**Example:**
```cpp
UPak pak("assets/game_assets.pak");
UInt32 count = pak.GetEntryCount();
// count now contains the number of assets
```

#### IsOpen

```cpp
bool IsOpen(void) const
```

Checks if the pak file is currently open.

**Returns:**
- `true` if the pak is open, `false` otherwise

**Example:**
```cpp
UPak pak;
if (!pak.IsOpen()) {
    pak.Open("assets/game_assets.pak");
}
```

#### GetPath

```cpp
FilePath GetPath(void) const
```

Gets the path to the pak file.

**Returns:**
- The file path to the pak file

**Example:**
```cpp
UPak pak("assets/game_assets.pak");
// ... later ...
FilePath currentPath = pak.GetPath();
// currentPath is "assets/game_assets.pak"
```

## Usage Example

```cpp
#include <Engine/Assets/UPak.hpp>

// Open an existing pak file
UPak pak("assets/game_assets.pak");
if (!pak.IsOpen()) {
    // Handle error
    return;
}

// Check if a specific asset exists
if (pak.HasAssetByName("hero_sprite.png")) {
    // Load the asset data
    std::vector<Byte> spriteData;
    if (pak.LoadAssetDataByName("hero_sprite.png", spriteData)) {
        // Use spriteData...
    }
}

// Create an asset object for lazy loading
auto heroAsset = pak.CreateAssetByName("hero_sprite.png");
if (heroAsset) {
    // Asset metadata is available immediately
    FString assetName = heroAsset->GetName();
    EAssetType assetType = heroAsset->GetType();
    
    // Data can be loaded later when needed
    // heroAsset->LoadData();
}

// Iterate through all assets
std::vector<FPakEntry> entries = pak.GetAllEntries();
for (const auto& entry : entries) {
    // Process each asset entry
}

// Clean up
pak.Close();
```
