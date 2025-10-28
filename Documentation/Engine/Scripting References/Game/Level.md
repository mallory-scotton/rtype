# Level Documentation

## Overview

The `ULevel` class represents a game level in the TKD Game Engine. A level is a self-contained game environment that contains actors, their properties, and game mode configuration. Levels can be loaded from files, assets, or created dynamically from the current world state. The level system provides serialization, deserialization, and management of game world data.

### Key Features

- **Actor Management**: Stores actor entries with class names, names, transforms, and properties
- **Game Mode Integration**: Associates levels with specific game modes
- **Serialization Support**: Binary serialization for saving/loading levels
- **Multiple Loading Methods**: Load from files, assets, or world state
- **Property Preservation**: Maintains actor properties during serialization
- **Dynamic Construction**: Create levels programmatically or from existing worlds
- **Asset Integration**: Works with the asset system for level storage

### Architecture

```cpp
class ULevel : public UObject
{
public:
    struct PropertyEntry {
        FString name;              // Property name
        SizeT size;                // Property size in bytes
        std::vector<Byte> value;   // Serialized property value
    };

    struct ActorEntry {
        FString class_name;                  // Actor class name
        FString name;                        // Actor instance name
        bool isActive;                       // Active state
        FVector3 position;                   // Position
        FRotator rotation;                   // Rotation
        FVector3 scale;                      // Scale
        TVector<PropertyEntry> properties;   // Custom properties
    };

private:
    AGameMode m_gameMode;                 // Level's game mode
    TVector<ActorEntry> m_actorEntries;   // All actors in level
    FString m_levelName;                  // Level identifier
};
```

The `ULevel` acts as a blueprint for world construction, containing all the information needed to recreate a game environment.

## Core Components

### Actor Entry Structure

Each actor in a level is represented by an `ActorEntry` containing:

```cpp
struct ActorEntry {
    FString class_name;        // "APlayer", "AEnemy", etc.
    FString name;              // "Player1", "EnemySpawner", etc.
    bool isActive;             // true = active, false = inactive
    FVector3 position;         // World position
    FRotator rotation;         // World rotation
    FVector3 scale;            // World scale
    TVector<PropertyEntry> properties; // Custom properties
};
```

### Property Entry Structure

Custom actor properties are stored as binary data:

```cpp
struct PropertyEntry {
    FString name;              // Property name ("Health", "Speed")
    SizeT size;                // Data size in bytes
    std::vector<Byte> value;   // Raw binary data
};
```

### Serialization Format

Levels are serialized to binary format with version control:

```
[Format Version: UInt32]
[Actor Count: SizeT]
[Actor 1 Data]
  [Name: FString]
  [IsActive: bool]
  [Position: FVector3]
  [Rotation: FRotator]
  [Scale: FVector3]
  [Property Count: SizeT]
  [Property 1: name, size, value]
  [Property 2: name, size, value]
  ...
[Actor 2 Data]
...
```

## API Reference

### Constructor & Initialization

#### `ULevel(const FString& name = "ULevel")`

Creates a new level with the specified name.

**Parameters:**
- `name`: Level identifier (default: "ULevel")

**Behavior:**
- Initializes empty actor list
- Sets level name
- Creates default game mode

**Example:**
```cpp
// Create a new level
ULevel myLevel("MyCustomLevel");

// Create with default name
ULevel defaultLevel;
```

### Game Mode Management

#### `const AGameMode& GetGameMode(void) const`

Gets the level's game mode (read-only).

**Returns:** Constant reference to the game mode

#### `AGameMode& GetGameMode(void)`

Gets the level's game mode (read-write).

**Returns:** Reference to the game mode

#### `void SetGameMode(const AGameMode& gameMode)`

Sets the level's game mode.

**Parameters:**
- `gameMode`: New game mode for the level

**Example:**
```cpp
// Create and set a custom game mode
AMyGameMode customMode;
myLevel.SetGameMode(customMode);

// Get current game mode
const AGameMode& currentMode = myLevel.GetGameMode();
```

### Actor Entry Management

#### `const TVector<ActorEntry>& GetActorEntries(void) const`

Gets all actor entries in the level.

**Returns:** Constant reference to actor entries vector

#### `void SetActorEntries(const TVector<ActorEntry>& actors)`

Replaces all actor entries in the level.

**Parameters:**
- `actors`: New actor entries

#### `void AddActorEntry(const ActorEntry& actor)`

Adds a single actor entry to the level.

**Parameters:**
- `actor`: Actor entry to add

#### `void AddActorEntry(const FString& className, const FString& actorName, const FTransform& transform)`

Adds an actor entry with the specified parameters.

**Parameters:**
- `className`: Actor class name (e.g., "APlayer")
- `actorName`: Actor instance name (e.g., "Player1")
- `transform`: Actor transform (position, rotation, scale)

**Behavior:**
- Creates actor entry with default active state (true)
- Extracts position, rotation, scale from transform

**Example:**
```cpp
// Add a player actor
FTransform playerTransform(FVector3(0, 0, 0), FRotator(0, 0, 0), FVector3(1, 1, 1));
myLevel.AddActorEntry("APlayer", "Player1", playerTransform);

// Add an enemy spawner
FTransform spawnerTransform(FVector3(100, 50, 0), FRotator(0, 90, 0), FVector3(1, 1, 1));
myLevel.AddActorEntry("AEnemySpawner", "EnemySpawner1", spawnerTransform);

// Add multiple actors
TVector<ULevel::ActorEntry> actors;
ULevel::ActorEntry enemy;
enemy.class_name = "AEnemy";
enemy.name = "Enemy1";
enemy.isActive = true;
enemy.position = FVector3(50, 25, 0);
enemy.rotation = FRotator(0, 0, 0);
enemy.scale = FVector3(1, 1, 1);
actors.PushBack(enemy);

myLevel.SetActorEntries(actors);
```

### Level Name Management

#### `FString GetLevelName(void) const`

Gets the level's name.

**Returns:** Level name string

#### `void SetLevelName(const FString& name)`

Sets the level's name.

**Parameters:**
- `name`: New level name

**Example:**
```cpp
myLevel.SetLevelName("Level1_Forest");
FString name = myLevel.GetLevelName(); // "Level1_Forest"
```

### Loading Methods

#### `static ULevel LoadLevelFromAsset(UAsset& asset)`

Loads a level from a UAsset.

**Parameters:**
- `asset`: Asset containing level data (must be EAssetType::Level)

**Returns:** Loaded level, or empty level on failure

**Behavior:**
- Validates asset type
- Loads asset data if needed
- Parses binary level data
- Returns empty level if parsing fails

**Example:**
```cpp
UAsset levelAsset("Levels/ForestLevel.asset");
ULevel forestLevel = ULevel::LoadLevelFromAsset(levelAsset);

if (forestLevel.GetActorEntries().Size() > 0) {
    FLogger::Log(ELogLevel::Info, "Level loaded successfully");
} else {
    FLogger::Log(ELogLevel::Error, "Failed to load level");
}
```

#### `static ULevel LoadLevelFromFile(const FilePath& levelPath)`

Loads a level from a file path.

**Parameters:**
- `levelPath`: Path to level file

**Returns:** Loaded level, or empty level on failure

**Behavior:**
- Creates UAsset from file path
- Validates asset type
- Delegates to LoadLevelFromAsset

**Example:**
```cpp
FilePath levelPath = "Assets/Levels/Tutorial.level";
ULevel tutorialLevel = ULevel::LoadLevelFromFile(levelPath);
```

#### `static ULevel LoadLevelFromWorld(UWorld& world)`

Creates a level from the current world state.

**Parameters:**
- `world`: World to extract level data from

**Returns:** Level containing all actors from the world

**Behavior:**
- Extracts level name from world's current level
- Iterates all actors in world
- Serializes actor properties (excluding Transform and IsActive)
- Creates actor entries for each actor

**Supported Property Types:**
- Bool, Int32, UInt32, Float32, Float64
- FVector3, FRotator, FTransform

**Example:**
```cpp
// Create level from current world state
ULevel snapshot = ULevel::LoadLevelFromWorld(*myWorld);

// Save the snapshot
snapshot.SaveToFile("WorldSnapshot.level");
```

### Saving Methods

#### `bool SaveToFile(const FilePath& levelPath)`

Saves the level to a file.

**Parameters:**
- `levelPath`: Path where to save the level

**Returns:** True on success, false on failure

**Behavior:**
- Serializes level data to binary format
- Writes to file
- Handles file I/O errors

**Example:**
```cpp
if (myLevel.SaveToFile("MyLevel.level")) {
    FLogger::Log(ELogLevel::Info, "Level saved successfully");
} else {
    FLogger::Log(ELogLevel::Error, "Failed to save level");
}
```

#### `bool SaveToAsset(UAsset& asset)`

Saves the level to a UAsset.

**Parameters:**
- `asset`: Asset to save level data to (must be EAssetType::Level)

**Returns:** True on success, false on failure

**Behavior:**
- Validates asset type
- Serializes level data
- Sets asset data

#### `static bool SaveWorldToFile(UWorld& world, const FilePath& levelPath)`

Saves the current world state as a level file.

**Parameters:**
- `world`: World to save
- `levelPath`: Path where to save the level

**Returns:** True on success, false on failure

**Behavior:**
- Creates level from world state
- Saves level to file

**Example:**
```cpp
// Save current world state
if (ULevel::SaveWorldToFile(*myWorld, "CurrentWorld.level")) {
    FLogger::Log(ELogLevel::Info, "World saved successfully");
}
```

### Utility Methods

#### `bool UnLoadLevel(void)`

Clears all level data.

**Returns:** Always true

**Behavior:**
- Clears level name
- Clears all actor entries

**Example:**
```cpp
myLevel.UnLoadLevel(); // Reset level to empty state
```

## Usage Examples

### Creating a Level Programmatically

```cpp
class LevelBuilder
{
public:
    static ULevel CreateTutorialLevel()
    {
        ULevel level("TutorialLevel");

        // Set game mode
        ATutorialGameMode gameMode;
        level.SetGameMode(gameMode);

        // Add player spawn point
        FTransform playerSpawn(FVector3(0, 0, 0), FRotator(0, 0, 0), FVector3(1, 1, 1));
        level.AddActorEntry("APlayer", "Player", playerSpawn);

        // Add enemies
        for (int i = 0; i < 5; ++i) {
            FTransform enemyPos(FVector3(i * 20.0f, 50.0f, 0), FRotator(0, 180, 0), FVector3(1, 1, 1));
            level.AddActorEntry("AEnemy", FString::Format("Enemy{}", i), enemyPos);
        }

        // Add collectibles
        level.AddActorEntry("ACoin", "Coin1", FTransform(FVector3(25, 25, 0)));
        level.AddActorEntry("ACoin", "Coin2", FTransform(FVector3(75, 25, 0)));

        // Add exit
        level.AddActorEntry("ALevelExit", "Exit", FTransform(FVector3(100, 0, 0)));

        return level;
    }

    static ULevel CreateBossLevel()
    {
        ULevel level("BossLevel");

        // Set boss game mode
        ABossGameMode bossMode;
        level.SetGameMode(bossMode);

        // Add player
        level.AddActorEntry("APlayer", "Player", FTransform(FVector3(0, 0, 0)));

        // Add boss enemy
        level.AddActorEntry("ABossEnemy", "Boss", FTransform(FVector3(50, 0, 0), FRotator(0, 180, 0), FVector3(2, 2, 2)));

        // Add platforms
        for (int i = 0; i < 10; ++i) {
            level.AddActorEntry("APlatform", FString::Format("Platform{}", i),
                FTransform(FVector3(i * 10.0f - 45.0f, -20.0f, 0)));
        }

        return level;
    }
};
```

### Level Editor Integration

```cpp
class LevelEditor
{
private:
    ULevel* m_currentLevel;
    UWorld* m_previewWorld;

public:
    void CreateNewLevel(const FString& levelName)
    {
        m_currentLevel = new ULevel(levelName);
        UpdatePreview();
    }

    void AddActorToLevel(const FString& className, const FTransform& transform)
    {
        if (!m_currentLevel) return;

        FString actorName = GenerateUniqueName(className);
        m_currentLevel->AddActorEntry(className, actorName, transform);
        UpdatePreview();
    }

    void RemoveActorFromLevel(const FString& actorName)
    {
        if (!m_currentLevel) return;

        // Find and remove actor entry
        auto& entries = m_currentLevel->GetActorEntries();
        for (size_t i = 0; i < entries.Size(); ++i) {
            if (entries[i].name == actorName) {
                entries.RemoveAt(i);
                break;
            }
        }
        UpdatePreview();
    }

    void SaveLevel(const FilePath& path)
    {
        if (m_currentLevel && m_currentLevel->SaveToFile(path)) {
            FLogger::Log(ELogLevel::Info, FString::Format("Level saved to {}", path));
        }
    }

    void LoadLevel(const FilePath& path)
    {
        ULevel loadedLevel = ULevel::LoadLevelFromFile(path);
        if (loadedLevel.GetActorEntries().Size() > 0) {
            *m_currentLevel = loadedLevel;
            UpdatePreview();
            FLogger::Log(ELogLevel::Info, FString::Format("Level loaded from {}", path));
        }
    }

    void UpdatePreview()
    {
        if (!m_currentLevel || !m_previewWorld) return;

        // Clear preview world
        for (auto& actor : m_previewWorld->GetActors()) {
            m_previewWorld->DestroyActor(actor.get());
        }

        // Spawn preview actors
        for (const auto& entry : m_currentLevel->GetActorEntries()) {
            FTransform transform(entry.position, entry.rotation, entry.scale);
            m_previewWorld->SpawnActor(entry.class_name, transform);
        }
    }

private:
    FString GenerateUniqueName(const FString& baseName)
    {
        static int counter = 0;
        return FString::Format("{}{}", baseName, counter++);
    }
};
```

### Level Streaming System

```cpp
class LevelStreamer
{
public:
    void LoadLevelAsync(const FString& levelName, std::function<void(ULevel&)> callback)
    {
        // Start async loading
        std::thread([levelName, callback]() {
            FilePath levelPath = FString::Format("Levels/{}.level", levelName);
            ULevel level = ULevel::LoadLevelFromFile(levelPath);

            // Call callback on main thread
            MainThread::Execute([level, callback]() mutable {
                callback(level);
            });
        }).detach();
    }

    void StreamLevelIn(UWorld& world, const ULevel& level)
    {
        // Spawn level actors
        for (const auto& entry : level.GetActorEntries()) {
            FTransform transform(entry.position, entry.rotation, entry.scale);
            AActor* actor = world.SpawnActor(entry.class_name, transform);

            if (actor) {
                // Restore custom properties
                RestoreActorProperties(actor, entry.properties);
                actor->SetActive(entry.isActive);
            }
        }
    }

    void StreamLevelOut(UWorld& world, const FString& levelName)
    {
        // Find and destroy actors from the level
        auto actors = world.GetActors();
        for (auto& actor : actors) {
            if (actor && GetActorLevelName(actor.get()) == levelName) {
                world.DestroyActor(actor.get());
            }
        }
    }

private:
    void RestoreActorProperties(AActor* actor, const TVector<ULevel::PropertyEntry>& properties)
    {
        for (const auto& prop : properties) {
            // Find property by name and deserialize
            IProperty* property = actor->GetProperty(prop.name);
            if (property) {
                // Deserialize based on property type
                DeserializeProperty(property, prop.value);
            }
        }
    }

    FString GetActorLevelName(AActor* actor)
    {
        // Custom logic to determine which level an actor belongs to
        // This could be stored as a property or determined by location
        return "Unknown";
    }

    void DeserializeProperty(IProperty* property, const std::vector<Byte>& data)
    {
        // Deserialize based on concrete property type
        // Similar to the serialization logic in LoadLevelFromWorld
    }
};
```

### Level Validation System

```cpp
class LevelValidator
{
public:
    struct ValidationResult {
        bool isValid;
        FString errorMessage;
        TVector<FString> warnings;
    };

    ValidationResult ValidateLevel(const ULevel& level)
    {
        ValidationResult result;
        result.isValid = true;

        // Check for required actors
        if (!HasRequiredActors(level)) {
            result.isValid = false;
            result.errorMessage = "Level missing required actors (Player, Exit)";
        }

        // Check for actor placement
        auto placementWarnings = CheckActorPlacement(level);
        result.warnings.Insert(result.warnings.End(), placementWarnings.Begin(), placementWarnings.End());

        // Check for property validity
        auto propertyWarnings = CheckPropertyValidity(level);
        result.warnings.Insert(result.warnings.End(), propertyWarnings.Begin(), propertyWarnings.End());

        return result;
    }

private:
    bool HasRequiredActors(const ULevel& level)
    {
        bool hasPlayer = false;
        bool hasExit = false;

        for (const auto& entry : level.GetActorEntries()) {
            if (entry.class_name == "APlayer") hasPlayer = true;
            if (entry.class_name == "ALevelExit") hasExit = true;
        }

        return hasPlayer && hasExit;
    }

    TVector<FString> CheckActorPlacement(const ULevel& level)
    {
        TVector<FString> warnings;

        for (const auto& entry : level.GetActorEntries()) {
            // Check if actor is placed outside level bounds
            if (entry.position.x < -1000.0f || entry.position.x > 1000.0f ||
                entry.position.y < -1000.0f || entry.position.y > 1000.0f) {
                warnings.PushBack(FString::Format("Actor {} is placed far from origin", entry.name));
            }

            // Check for overlapping actors
            for (const auto& other : level.GetActorEntries()) {
                if (&entry != &other) {
                    float distance = (entry.position - other.position).Length();
                    if (distance < 10.0f) {
                        warnings.PushBack(FString::Format("Actors {} and {} are too close", entry.name, other.name));
                    }
                }
            }
        }

        return warnings;
    }

    TVector<FString> CheckPropertyValidity(const ULevel& level)
    {
        TVector<FString> warnings;

        for (const auto& entry : level.GetActorEntries()) {
            for (const auto& prop : entry.properties) {
                // Check for invalid property values
                if (prop.name == "Health" && prop.size == sizeof(Int32)) {
                    Int32 health = *reinterpret_cast<const Int32*>(prop.value.data());
                    if (health <= 0) {
                        warnings.PushBack(FString::Format("Actor {} has invalid health: {}", entry.name, health));
                    }
                }
            }
        }

        return warnings;
    }
};
```

## Architecture Diagrams

### Level Loading Flow

```
File/Asset ──► UAsset ──► Load Data ──► Parse Binary ──► ULevel
     │              │          │             │            │
     ▼              ▼          ▼             ▼            ▼
Validate     Create Asset   Check Type   Read Format   Create Entries
Type              Load       (Level)      Version      Set Properties
Mismatch         Data        Fail →       Mismatch →   Set Game Mode
→ Empty Level              Empty Level   Empty Level
```

### Level Saving Flow

```
ULevel ──► Serialize ──► Write Binary ──► UAsset ──► Save File
   │           │             │            │           │
   ▼           ▼             ▼            ▼           ▼
Get Entries  Format     Write Actors   Set Data    Write to Disk
Get Props   Version     Write Props    Validate    Handle Errors
Get Mode    + Count     Write Size     Type
```

### World to Level Conversion

```
UWorld ──► Iterate Actors ──► Extract Data ──► Serialize Props ──► Create Entries
   │             │                │                │                │
   ▼             ▼                ▼                ▼                ▼
Get Actors   For Each       Get Class       Dynamic Cast    Add to Vector
Get Level    Actor          Get Name        to Concrete     Set Transform
Name         Get Props      Get Values      Types           Set Active
             Get Transform  Skip Built-in   (Bool, Int,     Set Properties
             Get Active     (Transform,     Float, Vector)
                            IsActive)
```

## Performance Considerations

### Memory Usage

- **Actor Entries**: Each entry stores strings and vectors
- **Property Data**: Binary blobs for each property
- **Serialization Buffer**: Temporary buffer during save/load
- **Asset Loading**: Cached asset data in memory

### CPU Performance

- **Serialization**: Binary format is fast to read/write
- **Property Handling**: Dynamic casting during serialization
- **Actor Iteration**: Linear search during world conversion
- **File I/O**: Blocking operations for save/load

### Optimization Strategies

1. **Property Caching**: Cache serialized properties
2. **Incremental Saving**: Save only changed actors
3. **Compression**: Compress level data for storage
4. **Streaming**: Load levels in chunks
5. **Validation Caching**: Cache validation results

## Troubleshooting

### Loading Failures

**Symptoms:** Level fails to load, returns empty level

**Possible Causes:**
- Invalid file path
- Corrupted file data
- Wrong asset type
- Unsupported format version
- Missing dependencies

**Solutions:**
```cpp
ULevel LoadLevelWithErrorHandling(const FilePath& path)
{
    ULevel level = ULevel::LoadLevelFromFile(path);

    if (level.GetActorEntries().Size() == 0) {
        FLogger::Log(ELogLevel::Error, FString::Format("Failed to load level: {}", path));

        // Try to provide more specific error information
        if (!FileExists(path)) {
            FLogger::Log(ELogLevel::Error, "File does not exist");
        } else {
            // Check file size, permissions, etc.
            FLogger::Log(ELogLevel::Error, "File exists but loading failed");
        }

        return ULevel(); // Return empty level
    }

    FLogger::Log(ELogLevel::Info,
        FString::Format("Successfully loaded level with {} actors", level.GetActorEntries().Size()));
    return level;
}
```

### Serialization Errors

**Symptoms:** Save operations fail or produce corrupted data

**Possible Causes:**
- File write permissions
- Disk space issues
- Unsupported property types
- Buffer overflow during serialization

**Solutions:**
```cpp
bool SaveLevelSafely(ULevel& level, const FilePath& path)
{
    // Validate level before saving
    if (level.GetActorEntries().Size() == 0) {
        FLogger::Log(ELogLevel::Warning, "Attempting to save empty level");
    }

    // Check disk space (simplified)
    if (!HasEnoughDiskSpace(path)) {
        FLogger::Log(ELogLevel::Error, "Insufficient disk space for level save");
        return false;
    }

    // Attempt save with error handling
    try {
        if (level.SaveToFile(path)) {
            FLogger::Log(ELogLevel::Info, FString::Format("Level saved to: {}", path));
            return true;
        } else {
            FLogger::Log(ELogLevel::Error, "Level save failed");
            return false;
        }
    } catch (const std::exception& e) {
        FLogger::Log(ELogLevel::Error, FString::Format("Exception during save: {}", e.what()));
        return false;
    }
}
```

### Property Deserialization Issues

**Symptoms:** Actors lose properties after loading

**Possible Causes:**
- Unsupported property types
- Buffer size mismatches
- Endianness issues
- Type casting failures

**Solutions:**
```cpp
void DebugPropertySerialization(const ULevel& level)
{
    for (const auto& entry : level.GetActorEntries()) {
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Actor: {} ({})", entry.name, entry.class_name));

        for (const auto& prop : entry.properties) {
            FLogger::Log(ELogLevel::Debug,
                FString::Format("  Property: {} ({} bytes)", prop.name, prop.size));

            // Log first few bytes for debugging
            FString dataStr = "Data: ";
            for (size_t i = 0; i < std::min(prop.value.size(), size_t(4)); ++i) {
                dataStr += FString::Format("{:02X} ", prop.value[i]);
            }
            FLogger::Log(ELogLevel::Debug, dataStr);
        }
    }
}

// Enhanced property deserialization with error handling
void DeserializeActorProperties(AActor* actor, const TVector<ULevel::PropertyEntry>& properties)
{
    for (const auto& prop : properties) {
        IProperty* property = actor->GetProperty(prop.name);
        if (!property) {
            FLogger::Log(ELogLevel::Warning,
                FString::Format("Property {} not found on actor {}", prop.name, actor->GetName()));
            continue;
        }

        try {
            // Attempt deserialization based on property type
            if (DeserializeProperty(property, prop.value)) {
                FLogger::Log(ELogLevel::Debug,
                    FString::Format("Restored property {} on actor {}", prop.name, actor->GetName()));
            } else {
                FLogger::Log(ELogLevel::Warning,
                    FString::Format("Failed to deserialize property {} on actor {}", prop.name, actor->GetName()));
            }
        } catch (const std::exception& e) {
            FLogger::Log(ELogLevel::Error,
                FString::Format("Exception deserializing property {}: {}", prop.name, e.what()));
        }
    }
}
```

## Future Enhancements

### Planned Features

1. **Level Streaming**: Load/unload level sections dynamically
2. **Level Instancing**: Create multiple instances of the same level
3. **Procedural Generation**: Generate levels algorithmically
4. **Level Validation**: Built-in validation and error checking
5. **Version Control**: Handle level format version upgrades
6. **Compression**: Automatic level data compression
7. **Encryption**: Secure level data storage
8. **Collaboration**: Multi-user level editing support

### Extension Points

The level system allows for easy extension:

```cpp
class AdvancedLevel : public ULevel {
public:
    // Add level metadata
    void SetDescription(const FString& desc) { m_description = desc; }
    void SetAuthor(const FString& author) { m_author = author; }
    void SetVersion(const FString& version) { m_version = version; }

    // Add level validation
    bool Validate() const {
        // Custom validation logic
        return ULevel::Validate() && ValidateCustomRules();
    }

    // Add level dependencies
    void AddDependency(const FString& assetPath) {
        m_dependencies.push_back(assetPath);
    }

    const TVector<FString>& GetDependencies() const {
        return m_dependencies;
    }

private:
    FString m_description;
    FString m_author;
    FString m_version;
    TVector<FString> m_dependencies;

    bool ValidateCustomRules() const {
        // Implement custom validation
        return true;
    }
};

// Level optimizer
class LevelOptimizer {
public:
    static ULevel OptimizeForRuntime(const ULevel& level) {
        ULevel optimized = level;

        // Remove editor-only actors
        RemoveEditorOnlyActors(optimized);

        // Optimize actor placement
        OptimizeActorPositions(optimized);

        // Compress property data
        CompressProperties(optimized);

        return optimized;
    }

    static ULevel OptimizeForEditor(const ULevel& level) {
        ULevel optimized = level;

        // Add editor helpers
        AddEditorHelpers(optimized);

        // Decompress for editing
        DecompressProperties(optimized);

        return optimized;
    }

private:
    static void RemoveEditorOnlyActors(ULevel& level) {
        auto& entries = level.GetActorEntries();
        entries.RemoveIf([](const ULevel::ActorEntry& entry) {
            return entry.class_name.Contains("Editor");
        });
    }

    static void OptimizeActorPositions(ULevel& level) {
        // Implement position optimization
    }

    static void CompressProperties(ULevel& level) {
        // Implement property compression
    }
};
```

---

*This documentation covers the complete ULevel implementation as of TKD Engine v1.0.0. For the latest updates and additional examples, refer to the engine's source code and test suites.*
