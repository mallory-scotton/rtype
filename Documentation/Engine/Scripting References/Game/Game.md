# Game Documentation

## Overview

The `UGame` class serves as the root application class for games built with the TKD Game Engine. It acts as the central coordinator for game initialization, resource management, and lifecycle management. As the main entry point for game applications, `UGame` manages engine settings, default level loading, packed resource handling, and provides the framework for game-specific initialization and update logic.

### Key Features

- **Application Root**: Main entry point for game applications
- **Engine Configuration**: Manages engine settings and initialization parameters
- **Level Management**: Controls default level loading and game world setup
- **Resource Packaging**: Handles packed assets and resource bundles
- **Lifecycle Management**: Complete application lifecycle from startup to shutdown
- **Extensibility**: Base class designed for game-specific customization
- **Settings Management**: Centralized access to engine configuration
- **Resource Discovery**: Automatic resource loading and management

### Architecture

```cpp
class UGame : public UObject, public ITickable
{
private:
    FString m_defaultLevel;                // Default level to load
    FEngineSettings m_settings;            // Engine configuration
    TVector<FilePath> m_packedResources;   // Packed asset paths

public:
    // Level management
    void SetDefaultLevel(const FString& levelName);
    const FString& GetDefaultLevel() const;

    // Resource management
    void AddPackedResource(const FilePath& resource);
    const TVector<FilePath>& GetPackedResources() const;

    // Settings access
    const FEngineSettings& GetEngineSettings();

    // Lifecycle
    virtual void BeginPlay() override;
    virtual void Tick(Float32 deltaTime) override;
    virtual void EndPlay() override;
};
```

The `UGame` operates as the application singleton, managing the high-level game state while delegating specific game logic to specialized components like `AGameMode`.

## Core Components

### Engine Settings Management

The game maintains a reference to engine configuration:

```cpp
struct FEngineSettings {
    // Rendering settings
    bool enableVSync;
    UInt32 targetFPS;
    FResolution resolution;

    // Audio settings
    bool enableAudio;
    Float32 masterVolume;
    Float32 sfxVolume;
    Float32 musicVolume;

    // Network settings
    bool enableNetworking;
    UInt32 maxConnections;
    FString serverAddress;

    // Debug settings
    bool enableDebugOverlay;
    ELogLevel logLevel;
    bool enableProfiling;

    // Platform settings
    FString windowTitle;
    bool fullscreen;
    bool resizable;
};
```

### Level Management System

Controls the default game world loading:

```cpp
struct LevelConfig {
    FString levelName;           // Level identifier
    FString levelPath;           // File system path
    bool autoLoad;              // Load on startup
    TArray<FString> dependencies; // Required assets
};
```

### Resource Packaging System

Manages bundled assets and resources:

```cpp
struct PackedResource {
    FilePath sourcePath;         // Original file location
    FilePath packedPath;         // Location in pack file
    EResourceType type;          // Asset type (texture, sound, etc.)
    UInt64 size;                // File size in bytes
    UInt64 offset;              // Offset in pack file
    FString checksum;           // Integrity verification
};
```

## API Reference

### Constructor & Destructor

#### `UGame(const FEngineSettings& settings = FEngineSettings())`

Creates a new game instance with specified engine settings.

**Parameters:**
- `settings`: Engine configuration (defaults to default settings)

**Behavior:**
- Initializes the game object with "UGame" name
- Sets default level to "L_DefaultLevel"
- Stores engine settings reference
- Initializes empty packed resources list

#### `virtual ~UGame() = default`

Default virtual destructor for proper cleanup.

### Level Management Methods

#### `void SetDefaultLevel(const FString& levelName)`

Sets the default level to load when the game starts.

**Parameters:**
- `levelName`: Name of the level to load (e.g., "L_MainMenu", "L_Level01")

**Behavior:**
- Updates the default level name
- Level will be loaded during game initialization
- Can be called at any time before game start

#### `const FString& GetDefaultLevel(void) const`

Gets the current default level name.

**Returns:** Constant reference to the default level string

**Usage:**
```cpp
const FString& levelName = game->GetDefaultLevel();
// levelName == "L_DefaultLevel" (default)
```

### Resource Management Methods

#### `void AddPackedResource(const FilePath& resource)`

Adds a packed resource to the game's resource list.

**Parameters:**
- `resource`: File path to the packed resource file

**Behavior:**
- Adds the resource path to the internal list
- Resources are loaded during game initialization
- Supports multiple pack files

#### `const TVector<FilePath>& GetPackedResources(void) const`

Gets the list of all packed resources.

**Returns:** Constant reference to vector of packed resource paths

**Usage:**
```cpp
const auto& resources = game->GetPackedResources();
for (const FilePath& resource : resources) {
    FLogger::Log(ELogLevel::Debug, FString::Format("Packed: {}", resource));
}
```

### Settings Access Methods

#### `const FEngineSettings& GetEngineSettings(void)`

Gets the current engine settings.

**Returns:** Constant reference to the engine settings structure

**Usage:**
```cpp
const FEngineSettings& settings = game->GetEngineSettings();
if (settings.enableDebugOverlay) {
    // Enable debug features
}
```

### Lifecycle Methods

#### `virtual void BeginPlay(void) override`

Called when the game begins play (application startup).

**Behavior:**
- Base implementation is empty
- Override in derived classes for game-specific initialization
- Called after engine initialization but before main loop
- Safe to access engine systems and load resources

#### `virtual void Tick(Float32 deltaTime) override`

Updates the game each frame.

**Parameters:**
- `deltaTime`: Time elapsed since last update in seconds

**Behavior:**
- Base implementation is empty
- Override for frame-based game logic
- Called every frame during main game loop
- Use for high-level game state updates

#### `virtual void EndPlay(void) override`

Called when the game ends play (application shutdown).

**Behavior:**
- Base implementation is empty
- Override for cleanup and save operations
- Called before engine shutdown
- Last chance to save game state or statistics

## Usage Examples

### Basic Game Implementation

```cpp
class AMyGame : public UGame
{
public:
    AMyGame(const FEngineSettings& settings)
        : UGame(settings)
    {
        // Configure game settings
        SetDefaultLevel("L_MainMenu");

        // Add packed resources
        AddPackedResource("Content/GameAssets.pack");
        AddPackedResource("Content/AudioAssets.pack");
        AddPackedResource("Content/LevelAssets.pack");
    }

    virtual void BeginPlay() override
    {
        UGame::BeginPlay();

        FLogger::Log(ELogLevel::Info, "My Game starting up...");

        // Initialize game systems
        InitializeGameSystems();

        // Load initial resources
        LoadEssentialResources();

        // Set up game state
        InitializeGameState();

        // Start background music
        PlayBackgroundMusic();

        FLogger::Log(ELogLevel::Info, "My Game startup complete");
    }

    virtual void Tick(Float32 deltaTime) override
    {
        UGame::Tick(deltaTime);

        // Update game systems
        UpdateGameSystems(deltaTime);

        // Handle global game logic
        ProcessGlobalGameLogic(deltaTime);

        // Update statistics
        UpdateGameStatistics(deltaTime);
    }

    virtual void EndPlay() override
    {
        FLogger::Log(ELogLevel::Info, "My Game shutting down...");

        // Save game state
        SaveGameState();

        // Clean up resources
        CleanupGameResources();

        // Save statistics
        SaveGameStatistics();

        UGame::EndPlay();

        FLogger::Log(ELogLevel::Info, "My Game shutdown complete");
    }

private:
    void InitializeGameSystems()
    {
        // Initialize audio system
        AudioManager = new FAudioManager();
        AudioManager->Initialize();

        // Initialize input system
        InputManager = new FInputManager();
        InputManager->Initialize();

        // Initialize save system
        SaveManager = new FSaveManager();
        SaveManager->Initialize("Saves/");
    }

    void LoadEssentialResources()
    {
        // Load essential textures
        TextureManager->LoadTexture("UI/Cursor.png");
        TextureManager->LoadTexture("UI/LoadingScreen.png");

        // Load essential sounds
        AudioManager->LoadSound("Audio/UI/Click.wav");
        AudioManager->LoadSound("Audio/UI/Hover.wav");
    }

    void InitializeGameState()
    {
        // Load or create save data
        if (SaveManager->HasSaveFile()) {
            LoadGameSave();
        } else {
            CreateNewGameSave();
        }

        // Initialize global game state
        GameState = new FGlobalGameState();
        GameState->Initialize();
    }

    void PlayBackgroundMusic()
    {
        // Play menu music
        AudioManager->PlayMusic("Audio/Music/MainMenu.ogg", true);
    }

    void UpdateGameSystems(Float32 deltaTime)
    {
        // Update all game systems
        AudioManager->Update(deltaTime);
        InputManager->Update(deltaTime);
        SaveManager->Update(deltaTime);
        GameState->Update(deltaTime);
    }

    void ProcessGlobalGameLogic(Float32 deltaTime)
    {
        // Handle global events
        ProcessAchievements(deltaTime);
        ProcessStatistics(deltaTime);
        ProcessAutoSave(deltaTime);
    }

    void UpdateGameStatistics(Float32 deltaTime)
    {
        // Update play time
        totalPlayTime += deltaTime;

        // Update session time
        sessionPlayTime += deltaTime;

        // Check for achievements
        CheckTimeBasedAchievements();
    }

    void SaveGameState()
    {
        // Save current game state
        GameState->Save();

        // Save statistics
        SaveStatistics();

        // Force save to disk
        SaveManager->Flush();
    }

    void CleanupGameResources()
    {
        // Clean up systems
        delete AudioManager;
        delete InputManager;
        delete SaveManager;
        delete GameState;

        // Clean up managers
        TextureManager->UnloadAll();
        AudioManager->UnloadAll();
    }

    void SaveGameStatistics()
    {
        // Save play statistics
        FJsonObject stats;
        stats.SetNumberField("totalPlayTime", totalPlayTime);
        stats.SetNumberField("sessionsPlayed", sessionsPlayed);
        stats.SetNumberField("lastPlayed", GetCurrentTime());

        SaveManager->SaveJson("Statistics.json", stats);
    }

private:
    FAudioManager* AudioManager;
    FInputManager* InputManager;
    FSaveManager* SaveManager;
    FGlobalGameState* GameState;

    Float32 totalPlayTime;
    Float32 sessionPlayTime;
    UInt32 sessionsPlayed;
};
```

### Game with Multiple Levels

```cpp
class ALevelBasedGame : public UGame
{
public:
    ALevelBasedGame(const FEngineSettings& settings)
        : UGame(settings)
    {
        // Set initial level
        SetDefaultLevel("L_Tutorial");

        // Add level packs
        AddPackedResource("Content/Levels/Tutorial.pack");
        AddPackedResource("Content/Levels/World1.pack");
        AddPackedResource("Content/Levels/World2.pack");
        AddPackedResource("Content/Levels/BossLevels.pack");
    }

    virtual void BeginPlay() override
    {
        UGame::BeginPlay();

        // Initialize level manager
        LevelManager = new FLevelManager();
        LevelManager->Initialize();

        // Load level progression
        LoadLevelProgression();

        // Set up level transitions
        InitializeLevelTransitions();

        // Load current level
        LoadCurrentLevel();
    }

    virtual void Tick(Float32 deltaTime) override
    {
        UGame::Tick(deltaTime);

        // Update level logic
        LevelManager->Update(deltaTime);

        // Check level completion
        CheckLevelCompletion();

        // Handle level transitions
        ProcessLevelTransitions(deltaTime);
    }

    void LoadLevel(const FString& levelName)
    {
        FLogger::Log(ELogLevel::Info,
            FString::Format("Loading level: {}", levelName));

        // Save current level state
        if (CurrentLevel.IsValid()) {
            SaveCurrentLevelState();
        }

        // Unload current level
        UnloadCurrentLevel();

        // Load new level
        CurrentLevel = LevelManager->LoadLevel(levelName);

        if (CurrentLevel.IsValid()) {
            // Initialize level
            CurrentLevel->BeginPlay();

            // Update game state
            currentLevelName = levelName;
            levelStartTime = GetWorld()->GetTime();

            FLogger::Log(ELogLevel::Info,
                FString::Format("Level '{}' loaded successfully", levelName));
        } else {
            FLogger::Log(ELogLevel::Error,
                FString::Format("Failed to load level: {}", levelName));
        }
    }

    void CompleteLevel()
    {
        if (!CurrentLevel.IsValid()) return;

        // Calculate completion time
        Float32 completionTime = GetWorld()->GetTime() - levelStartTime;

        // Update statistics
        UpdateLevelStatistics(currentLevelName, completionTime);

        // Mark level as completed
        levelProgression[currentLevelName] = ELevelStatus::Completed;

        // Award completion rewards
        AwardLevelCompletionRewards(currentLevelName);

        // Save progress
        SaveLevelProgression();

        FLogger::Log(ELogLevel::Info,
            FString::Format("Level '{}' completed in {:.2f}s",
                currentLevelName, completionTime));
    }

private:
    void LoadLevelProgression()
    {
        // Load level completion status
        FJsonObject progress = SaveManager->LoadJson("LevelProgress.json");

        if (progress.IsValid()) {
            // Parse level progression
            TArray<FString> levelNames = progress.GetArrayField("levels");

            for (const FString& levelName : levelNames) {
                FString statusStr = progress.GetStringField(levelName);
                ELevelStatus status = ParseLevelStatus(statusStr);
                levelProgression[levelName] = status;
            }
        } else {
            // Initialize default progression
            InitializeDefaultProgression();
        }
    }

    void InitializeLevelTransitions()
    {
        // Define level flow
        levelTransitions["L_Tutorial"] = "L_World1_Level1";
        levelTransitions["L_World1_Level1"] = "L_World1_Level2";
        levelTransitions["L_World1_Level2"] = "L_World1_Boss";
        levelTransitions["L_World1_Boss"] = "L_World2_Level1";
        // ... more transitions
    }

    void LoadCurrentLevel()
    {
        // Determine which level to load
        FString levelToLoad = GetDefaultLevel();

        // Check if we should resume from last played level
        if (levelProgression.contains("LastPlayed")) {
            levelToLoad = levelProgression["LastPlayed"];
        }

        LoadLevel(levelToLoad);
    }

    void CheckLevelCompletion()
    {
        if (!CurrentLevel.IsValid()) return;

        // Check level-specific completion conditions
        if (CurrentLevel->IsCompleted()) {
            CompleteLevel();

            // Transition to next level
            if (levelTransitions.contains(currentLevelName)) {
                FString nextLevel = levelTransitions[currentLevelName];
                LoadLevel(nextLevel);
            } else {
                // Game completed
                OnGameCompleted();
            }
        }
    }

    void ProcessLevelTransitions(Float32 deltaTime)
    {
        // Handle transition effects
        if (isTransitioning) {
            transitionTimer -= deltaTime;

            if (transitionTimer <= 0.0f) {
                CompleteLevelTransition();
            }
        }
    }

    void SaveCurrentLevelState()
    {
        // Save player position, inventory, etc.
        FJsonObject levelState;
        levelState.SetStringField("levelName", currentLevelName);
        levelState.SetNumberField("playTime", GetWorld()->GetTime() - levelStartTime);

        // Save player state
        if (APlayerController* player = GetPlayerController()) {
            levelState.SetVectorField("playerPosition", player->GetPawn()->GetTransform().GetPosition());
            // Save inventory, health, etc.
        }

        SaveManager->SaveJson(FString::Format("LevelState_{}.json", currentLevelName), levelState);
    }

    void UpdateLevelStatistics(const FString& levelName, Float32 completionTime)
    {
        // Update level statistics
        levelStats[levelName].completions++;
        levelStats[levelName].bestTime = Math<Float32>::Min(
            levelStats[levelName].bestTime, completionTime);
        levelStats[levelName].totalTime += completionTime;
        levelStats[levelName].averageTime = levelStats[levelName].totalTime /
            levelStats[levelName].completions;
    }

private:
    FLevelManager* LevelManager;
    TSharedPtr<ULevel> CurrentLevel;

    FString currentLevelName;
    Float32 levelStartTime;

    TMap<FString, ELevelStatus> levelProgression;
    TMap<FString, FString> levelTransitions;

    struct LevelStats {
        UInt32 completions;
        Float32 bestTime;
        Float32 totalTime;
        Float32 averageTime;
    };
    TMap<FString, LevelStats> levelStats;

    bool isTransitioning;
    Float32 transitionTimer;
};
```

### Game with Mod Support

```cpp
class AModdableGame : public UGame
{
public:
    AModdableGame(const FEngineSettings& settings)
        : UGame(settings)
    {
        // Add base game resources
        AddPackedResource("Content/BaseGame.pack");

        // Mod directory will be scanned at startup
    }

    virtual void BeginPlay() override
    {
        UGame::BeginPlay();

        // Initialize mod system
        ModManager = new FModManager();
        ModManager->Initialize("Mods/");

        // Discover and load mods
        DiscoverMods();
        LoadMods();

        // Apply modded settings
        ApplyModSettings();

        // Initialize modded systems
        InitializeModSystems();
    }

    virtual void Tick(Float32 deltaTime) override
    {
        UGame::Tick(deltaTime);

        // Update mods
        ModManager->Update(deltaTime);

        // Update modded systems
        UpdateModSystems(deltaTime);
    }

    virtual void EndPlay() override
    {
        // Unload mods
        UnloadMods();

        // Cleanup mod systems
        CleanupModSystems();

        UGame::EndPlay();
    }

    void DiscoverMods()
    {
        FLogger::Log(ELogLevel::Info, "Discovering mods...");

        // Scan mod directory
        TArray<FString> modDirectories = FFileSystem::GetDirectories("Mods/");

        for (const FString& modDir : modDirectories) {
            // Check for mod manifest
            FString manifestPath = FString::Format("Mods/{}/ModManifest.json", modDir);

            if (FFileSystem::FileExists(manifestPath)) {
                FJsonObject manifest = LoadJsonFile(manifestPath);

                if (manifest.IsValid()) {
                    FModInfo modInfo;
                    modInfo.name = manifest.GetStringField("name");
                    modInfo.version = manifest.GetStringField("version");
                    modInfo.author = manifest.GetStringField("author");
                    modInfo.description = manifest.GetStringField("description");
                    modInfo.directory = modDir;

                    // Parse dependencies
                    TArray<FString> dependencies = manifest.GetArrayField("dependencies");
                    modInfo.dependencies = dependencies;

                    discoveredMods.push_back(modInfo);

                    FLogger::Log(ELogLevel::Info,
                        FString::Format("Found mod: {} v{} by {}",
                            modInfo.name, modInfo.version, modInfo.author));
                }
            }
        }
    }

    void LoadMods()
    {
        FLogger::Log(ELogLevel::Info, "Loading mods...");

        // Sort mods by dependencies
        SortModsByDependencies();

        // Load mods in order
        for (const FModInfo& modInfo : discoveredMods) {
            if (CanLoadMod(modInfo)) {
                LoadMod(modInfo);
            } else {
                FLogger::Log(ELogLevel::Warning,
                    FString::Format("Skipping mod '{}' - dependencies not met", modInfo.name));
            }
        }
    }

    void LoadMod(const FModInfo& modInfo)
    {
        FLogger::Log(ELogLevel::Info,
            FString::Format("Loading mod: {}", modInfo.name));

        try {
            // Load mod DLL/library
            FString modDllPath = FString::Format("Mods/{}/Mod.dll", modInfo.directory);
            void* modHandle = LoadLibrary(modDllPath);

            if (modHandle) {
                // Get mod entry point
                GetModInstanceFunc getInstance =
                    (GetModInstanceFunc)GetProcAddress(modHandle, "GetModInstance");

                if (getInstance) {
                    IGameMod* mod = getInstance();
                    if (mod) {
                        // Initialize mod
                        mod->Initialize(this);

                        // Add to loaded mods
                        loadedMods.push_back(mod);
                        loadedModHandles.push_back(modHandle);

                        FLogger::Log(ELogLevel::Info,
                            FString::Format("Mod '{}' loaded successfully", modInfo.name));
                    } else {
                        FreeLibrary(modHandle);
                        FLogger::Log(ELogLevel::Error,
                            FString::Format("Failed to get mod instance for '{}'", modInfo.name));
                    }
                } else {
                    FreeLibrary(modHandle);
                    FLogger::Log(ELogLevel::Error,
                        FString::Format("Mod '{}' missing entry point", modInfo.name));
                }
            } else {
                FLogger::Log(ELogLevel::Error,
                    FString::Format("Failed to load mod DLL: {}", modDllPath));
            }
        }
        catch (const std::exception& e) {
            FLogger::Log(ELogLevel::Error,
                FString::Format("Exception loading mod '{}': {}", modInfo.name, e.what()));
        }
    }

    void ApplyModSettings()
    {
        // Apply settings from all loaded mods
        for (IGameMod* mod : loadedMods) {
            mod->ApplySettings(this);
        }
    }

    void InitializeModSystems()
    {
        // Initialize systems added by mods
        for (IGameMod* mod : loadedMods) {
            mod->InitializeSystems();
        }
    }

    void UpdateModSystems(Float32 deltaTime)
    {
        // Update modded systems
        for (IGameMod* mod : loadedMods) {
            mod->Update(deltaTime);
        }
    }

    void UnloadMods()
    {
        FLogger::Log(ELogLevel::Info, "Unloading mods...");

        // Unload mods in reverse order
        for (auto it = loadedMods.rbegin(); it != loadedMods.rend(); ++it) {
            IGameMod* mod = *it;
            mod->Shutdown();
            delete mod;
        }

        loadedMods.clear();

        // Free libraries
        for (void* handle : loadedModHandles) {
            FreeLibrary(handle);
        }

        loadedModHandles.clear();

        FLogger::Log(ELogLevel::Info, "Mods unloaded");
    }

    bool CanLoadMod(const FModInfo& modInfo) const
    {
        // Check if all dependencies are loaded
        for (const FString& dependency : modInfo.dependencies) {
            bool dependencyFound = false;

            for (const FModInfo& loadedMod : discoveredMods) {
                if (loadedMod.name == dependency) {
                    dependencyFound = true;
                    break;
                }
            }

            if (!dependencyFound) {
                return false;
            }
        }

        return true;
    }

    void SortModsByDependencies()
    {
        // Topological sort based on dependencies
        // (Implementation would use dependency graph sorting)
    }

public:
    // Mod interface
    void RegisterModdedResource(const FString& resourcePath)
    {
        AddPackedResource(resourcePath);
    }

    void RegisterModdedSystem(IGameSystem* system)
    {
        moddedSystems.push_back(system);
    }

private:
    FModManager* ModManager;

    struct FModInfo {
        FString name;
        FString version;
        FString author;
        FString description;
        FString directory;
        TArray<FString> dependencies;
    };

    TVector<FModInfo> discoveredMods;
    TVector<IGameMod*> loadedMods;
    TVector<void*> loadedModHandles;
    TVector<IGameSystem*> moddedSystems;
};
```

### Game with Network Support

```cpp
class ANetworkedGame : public UGame
{
public:
    ANetworkedGame(const FEngineSettings& settings)
        : UGame(settings)
    {
        // Configure for networking
        FEngineSettings& gameSettings = const_cast<FEngineSettings&>(GetEngineSettings());
        gameSettings.enableNetworking = true;
        gameSettings.maxConnections = 16;

        // Set default level
        SetDefaultLevel("L_Lobby");
    }

    virtual void BeginPlay() override
    {
        UGame::BeginPlay();

        // Initialize network system
        NetworkManager = new FNetworkManager();
        NetworkManager->Initialize(GetEngineSettings());

        // Set up network callbacks
        NetworkManager->OnClientConnected.Bind(this, &ANetworkedGame::OnClientConnected);
        NetworkManager->OnClientDisconnected.Bind(this, &ANetworkedGame::OnClientDisconnected);
        NetworkManager->OnMessageReceived.Bind(this, &ANetworkedGame::OnNetworkMessage);

        // Start server or connect as client
        if (IsServer()) {
            StartServer();
        } else {
            ConnectToServer();
        }

        // Initialize lobby
        InitializeLobby();
    }

    virtual void Tick(Float32 deltaTime) override
    {
        UGame::Tick(deltaTime);

        // Update network
        NetworkManager->Update(deltaTime);

        // Process network messages
        ProcessNetworkMessages(deltaTime);

        // Update lobby state
        UpdateLobby(deltaTime);
    }

    virtual void EndPlay() override
    {
        // Disconnect from network
        if (NetworkManager) {
            NetworkManager->Shutdown();
            delete NetworkManager;
        }

        UGame::EndPlay();
    }

    void StartServer()
    {
        FLogger::Log(ELogLevel::Info, "Starting game server...");

        if (NetworkManager->StartServer(7777, GetEngineSettings().maxConnections)) {
            FLogger::Log(ELogLevel::Info, "Server started successfully");
            isServer = true;

            // Load server-specific level
            SetDefaultLevel("L_ServerLobby");
            LoadCurrentLevel();
        } else {
            FLogger::Log(ELogLevel::Error, "Failed to start server");
        }
    }

    void ConnectToServer()
    {
        FLogger::Log(ELogLevel::Info, "Connecting to server...");

        if (NetworkManager->Connect("127.0.0.1", 7777)) {
            FLogger::Log(ELogLevel::Info, "Connected to server");
            isServer = false;

            // Load client-specific level
            SetDefaultLevel("L_ClientLobby");
            LoadCurrentLevel();
        } else {
            FLogger::Log(ELogLevel::Error, "Failed to connect to server");
        }
    }

    void OnClientConnected(FNetworkConnection* connection)
    {
        FLogger::Log(ELogLevel::Info,
            FString::Format("Client connected: {}", connection->GetAddress()));

        // Add to connected clients
        connectedClients.push_back(connection);

        // Send welcome message
        FNetworkMessage welcome;
        welcome.type = EMessageType::Welcome;
        welcome.data = FString::Format("Welcome to {}!", GetGameName());
        NetworkManager->SendMessage(connection, welcome);

        // Update lobby
        UpdateLobbyPlayerCount();
    }

    void OnClientDisconnected(FNetworkConnection* connection)
    {
        FLogger::Log(ELogLevel::Info,
            FString::Format("Client disconnected: {}", connection->GetAddress()));

        // Remove from connected clients
        connectedClients.erase(
            std::remove(connectedClients.begin(), connectedClients.end(), connection),
            connectedClients.end());

        // Update lobby
        UpdateLobbyPlayerCount();
    }

    void OnNetworkMessage(FNetworkMessage& message)
    {
        // Handle network messages
        switch (message.type) {
            case EMessageType::Chat:
                HandleChatMessage(message);
                break;

            case EMessageType::PlayerReady:
                HandlePlayerReady(message);
                break;

            case EMessageType::StartGame:
                HandleStartGame(message);
                break;

            default:
                FLogger::Log(ELogLevel::Warning,
                    FString::Format("Unknown message type: {}", (int)message.type));
                break;
        }
    }

    void HandleChatMessage(const FNetworkMessage& message)
    {
        // Broadcast chat message to all clients
        FString chatMessage = FString::Format("[{}] {}", message.senderName, message.data);
        FLogger::Log(ELogLevel::Info, chatMessage);

        // Add to chat history
        chatHistory.push_back(chatMessage);

        // Broadcast to all clients
        NetworkManager->BroadcastMessage(message);
    }

    void HandlePlayerReady(const FNetworkMessage& message)
    {
        // Mark player as ready
        readyPlayers.insert(message.senderId);

        // Check if all players are ready
        if (readyPlayers.size() == connectedClients.size()) {
            // Start game countdown
            StartGameCountdown();
        }
    }

    void HandleStartGame(const FNetworkMessage& message)
    {
        if (isServer) {
            // Server initiates game start
            FLogger::Log(ELogLevel::Info, "Starting networked game...");

            // Load game level
            LoadLevel("L_NetworkGame");

            // Notify all clients
            FNetworkMessage startMessage;
            startMessage.type = EMessageType::GameStarted;
            NetworkManager->BroadcastMessage(startMessage);
        }
    }

    void StartGameCountdown()
    {
        countdownTime = 5.0f; // 5 second countdown
        isCountingDown = true;

        FLogger::Log(ELogLevel::Info, "Game starting in 5 seconds...");
    }

    void UpdateLobby(Float32 deltaTime)
    {
        if (isCountingDown) {
            countdownTime -= deltaTime;

            if (countdownTime <= 0.0f) {
                // Start the game
                HandleStartGame(FNetworkMessage());
                isCountingDown = false;
            } else if (Math<Float32>::Floor(countdownTime) !=
                       Math<Float32>::Floor(countdownTime + deltaTime)) {
                // Announce countdown
                FString countdownMsg = FString::Format("Game starting in {}...",
                    Math<UInt32>::Floor(countdownTime));
                FLogger::Log(ELogLevel::Info, countdownMsg);

                // Send to clients
                FNetworkMessage msg;
                msg.type = EMessageType::Countdown;
                msg.data = countdownMsg;
                NetworkManager->BroadcastMessage(msg);
            }
        }
    }

private:
    FNetworkManager* NetworkManager;

    bool isServer;
    Float32 countdownTime;
    bool isCountingDown;

    TVector<FNetworkConnection*> connectedClients;
    TSet<UInt32> readyPlayers;
    TVector<FString> chatHistory;
};
```

## Resource Management

### Packed Resource System

The game manages packed assets for efficient loading:

```cpp
class FResourceManager {
public:
    void LoadPackedResources(const TVector<FilePath>& packFiles) {
        for (const FilePath& packFile : packFiles) {
            LoadPackFile(packFile);
        }
    }

    void LoadPackFile(const FilePath& packPath) {
        // Open pack file
        FFile packFile(packPath, EFileMode::Read);

        // Read pack header
        FPackHeader header;
        packFile.Read(&header, sizeof(FPackHeader));

        // Validate header
        if (header.magic != PACK_MAGIC) {
            FLogger::Log(ELogLevel::Error,
                FString::Format("Invalid pack file: {}", packPath));
            return;
        }

        // Read file entries
        for (UInt32 i = 0; i < header.fileCount; ++i) {
            FPackFileEntry entry;
            packFile.Read(&entry, sizeof(FPackFileEntry));

            // Store file info
            packedFiles[entry.fileName] = {
                packPath,
                entry.offset,
                entry.size,
                entry.checksum
            };
        }
    }

    TArray<UInt8> LoadPackedFile(const FString& fileName) {
        auto it = packedFiles.find(fileName);
        if (it == packedFiles.end()) {
            FLogger::Log(ELogLevel::Error,
                FString::Format("Packed file not found: {}", fileName));
            return {};
        }

        const FPackedFileInfo& info = it->second;

        // Open pack file
        FFile packFile(info.packPath, EFileMode::Read);

        // Seek to file offset
        packFile.Seek(info.offset);

        // Read file data
        TArray<UInt8> data;
        data.Resize(info.size);
        packFile.Read(data.GetData(), info.size);

        // Verify checksum
        if (CalculateChecksum(data) != info.checksum) {
            FLogger::Log(ELogLevel::Error,
                FString::Format("Checksum mismatch for: {}", fileName));
            return {};
        }

        return data;
    }

private:
    struct FPackedFileInfo {
        FilePath packPath;
        UInt64 offset;
        UInt64 size;
        UInt32 checksum;
    };

    TMap<FString, FPackedFileInfo> packedFiles;
};
```

### Resource Loading Strategy

Games can implement different loading strategies:

```cpp
enum class EResourceLoadStrategy {
    LoadOnDemand,      // Load when first requested
    PreloadAll,        // Load all at startup
    BackgroundLoad,    // Load in background threads
    StreamingLoad      // Load based on proximity/distance
};

class FResourceLoader {
public:
    void SetLoadStrategy(EResourceLoadStrategy strategy) {
        loadStrategy = strategy;
    }

    void LoadResource(const FString& resourceName) {
        switch (loadStrategy) {
            case EResourceLoadStrategy::LoadOnDemand:
                LoadOnDemand(resourceName);
                break;

            case EResourceLoadStrategy::PreloadAll:
                // Already loaded
                break;

            case EResourceLoadStrategy::BackgroundLoad:
                QueueBackgroundLoad(resourceName);
                break;

            case EResourceLoadStrategy::StreamingLoad:
                QueueStreamingLoad(resourceName);
                break;
        }
    }

private:
    EResourceLoadStrategy loadStrategy;
    TQueue<FString> backgroundLoadQueue;
    TMap<FString, Float32> streamingLoadPriorities;
};
```

## Performance Considerations

### Startup Optimization

- **Resource Loading**: Load essential resources first, defer non-essential
- **Initialization Order**: Initialize critical systems before secondary ones
- **Threading**: Use background threads for non-blocking initialization
- **Caching**: Cache frequently accessed data structures

### Runtime Performance

- **Tick Frequency**: Balance update frequency with performance needs
- **Memory Management**: Monitor memory usage and implement cleanup
- **Resource Streaming**: Load/unload resources based on usage patterns
- **Network Efficiency**: Minimize network traffic in multiplayer scenarios

### Memory Usage

- **Resource Packing**: Reduces memory fragmentation and load times
- **Reference Counting**: Automatic cleanup of unused resources
- **Pool Allocation**: Reuse objects to reduce allocation overhead
- **Streaming**: Load resources on-demand to manage memory footprint

### Best Practices

1. **Minimal Startup**: Load only essential resources at startup
2. **Progressive Loading**: Load additional resources as needed
3. **Resource Cleanup**: Unload unused resources to free memory
4. **Background Processing**: Use threads for non-critical operations
5. **Performance Monitoring**: Track and optimize bottlenecks

## Architecture Diagrams

### Game Class Hierarchy

```
UObject (Base Object)
├── UUID: Unique identifier
├── Name: Object name
├── Properties: Reflection system
├── Events: Object lifecycle
└── Serialization: Data persistence

ITickable (Update Interface)
└── Tick(deltaTime): Frame update method

UGame (Application Root)
├── Default Level: Starting level name
├── Engine Settings: Configuration reference
├── Packed Resources: Asset bundle paths
├── Lifecycle Management: BeginPlay/Tick/EndPlay
├── Resource Loading: Pack file management
├── Mod Support: Runtime extensibility
└── Network Integration: Multiplayer foundation
```

### Game Startup Sequence

```
Application Entry ──► Create UGame Instance ──► Initialize Engine ──► Load Settings
      │                        │                        │              │
      ▼                        ▼                        ▼              ▼
Parse Command Line      Set Default Level        Validate Settings   Load Resources
(Override Settings)     (L_DefaultLevel)         (Check Validity)    (Pack Files)
      │                        │                        │              │
      └────────────────────────┼────────────────────────┼──────────────┘
                               ▼                        ▼              ▼
                    Call BeginPlay() ◄─────── Initialize Systems ──────┘
                    ├── Setup Game State      ├── Audio Manager
                    ├── Load UI Assets        ├── Input Manager
                    ├── Initialize Network    ├── Physics World
                    └── Start Background Tasks └── Load Game Data
                                           │              │
                                           ▼              ▼
                                Enter Main Loop ◄─────── Start Rendering
                                ├── Process Input      ├── Update World
                                ├── Tick Game Logic    ├── Render Frame
                                ├── Update Network     ├── Present Frame
                                └── Handle Events      └── Check Exit Condition
                                             │              │
                                             ▼              ▼
                                  Game Shutdown ◄─────── Call EndPlay()
                                  ├── Save Game State
                                  ├── Cleanup Resources
                                  ├── Disconnect Network
                                  └── Exit Application
```

### Resource Management Flow

```
Resource Request ──► Check Cache ──► Cache Hit? ──► Return Cached Resource
      │                    │              │
      ▼                    ▼              ▼
  Cache Miss        Load from Pack     Use Cached
(Continue Loading)  (Pack File)        (Skip Loading)
      │                    │
      └────────────────────┘
               ▼
      Open Pack File ──► Find File Entry ──► Seek to Offset ──► Read Data
             │                  │                    │              │
             ▼                  ▼                    ▼              ▼
      Validate Header    Check File Exists     Calculate Size    Load Bytes
      (Magic Number)     (Entry Table)         (Compressed?)     (Decompress)
             │                  │                    │              │
             └──────────────────┼────────────────────┼──────────────┘
                                ▼                    ▼              ▼
                     Verify Checksum ◄─────── Validate Data ──────┘
                     (Integrity Check)       (Format Check)
                                │                    │
                                ▼                    ▼
                     Store in Cache ◄─────── Create Resource ──────┘
                     (Memory Pool)          (Texture/Sound/etc)
                                │                    │
                                └────────────────────┘
                                             ▼
                                   Return Resource
                                   (Ready for Use)
```

### Mod Loading Architecture

```
Mod Discovery ──► Scan Directory ──► Find Manifest ──► Parse Metadata
      │               │                   │              │
      ▼               ▼                   ▼              ▼
Check Mods/ Dir   Read JSON Files     Validate Format  Extract Info
(Directory List)  (ModManifest.json)  (Required Fields) (Name/Version)
      │               │                   │              │
      └───────────────┼───────────────────┼──────────────┘
                      ▼                   ▼              ▼
           Sort by Dependencies ◄─────── Build Graph ──────┘
           (Topological Sort)           (Dependency Tree)
                      │                   │
                      ▼                   ▼
           Load Mod DLL ◄─────── Check Dependencies ──────┘
           (LoadLibrary)         (Verify Loaded)
                      │                   │
                      ▼                   ▼
           Get Entry Point ◄─────── Create Instance ──────┘
           (GetProcAddress)      (IGameMod*)
                      │                   │
                      ▼                   ▼
           Initialize Mod ◄─────── Register Systems ──────┘
           (Call Initialize)     (Add to Game)
                      │                   │
                      └───────────────────┘
                                   ▼
                        Mod Active
                        (Participates in Game Loop)
```

## Troubleshooting

### Common Issues

#### Game Won't Start

**Symptoms:** Application fails to launch or crashes immediately

**Possible Causes:**
- Invalid engine settings
- Missing or corrupted pack files
- Missing dependencies
- Invalid default level

**Solutions:**
```cpp
// Validate game configuration before startup
void ValidateGameConfiguration(UGame* game) {
    FLogger::Log(ELogLevel::Debug, "Validating game configuration...");

    // Check engine settings
    const FEngineSettings& settings = game->GetEngineSettings();

    if (settings.resolution.width == 0 || settings.resolution.height == 0) {
        FLogger::Log(ELogLevel::Error, "Invalid resolution in engine settings");
    }

    if (settings.worldToMeters <= 0.0f) {
        FLogger::Log(ELogLevel::Error, "Invalid world scale in engine settings");
    }

    // Check default level
    const FString& defaultLevel = game->GetDefaultLevel();
    if (defaultLevel.IsEmpty()) {
        FLogger::Log(ELogLevel::Error, "Default level is not set");
    } else {
        // Check if level exists
        if (!LevelExists(defaultLevel)) {
            FLogger::Log(ELogLevel::Error,
                FString::Format("Default level '{}' does not exist", defaultLevel));
        }
    }

    // Check packed resources
    const auto& packedResources = game->GetPackedResources();
    for (const FilePath& resource : packedResources) {
        if (!FFileSystem::FileExists(resource)) {
            FLogger::Log(ELogLevel::Error,
                FString::Format("Packed resource '{}' does not exist", resource));
        }
    }

    FLogger::Log(ELogLevel::Debug, "Game configuration validation complete");
}

// Safe game initialization
UGame* CreateGameSafely(const FEngineSettings& settings) {
    try {
        // Create game instance
        UGame* game = new AMyGame(settings);

        // Validate configuration
        ValidateGameConfiguration(game);

        // Test basic functionality
        if (!TestGameInitialization(game)) {
            FLogger::Log(ELogLevel::Error, "Game initialization test failed");
            delete game;
            return nullptr;
        }

        FLogger::Log(ELogLevel::Info, "Game created successfully");
        return game;

    } catch (const std::exception& e) {
        FLogger::Log(ELogLevel::Error,
            FString::Format("Exception creating game: {}", e.what()));
        return nullptr;
    }
}

bool TestGameInitialization(UGame* game) {
    // Test basic getters
    if (game->GetDefaultLevel().IsEmpty()) {
        return false;
    }

    // Test settings access
    const FEngineSettings& settings = game->GetEngineSettings();
    if (settings.resolution.width <= 0) {
        return false;
    }

    return true;
}
```

#### Resources Not Loading

**Symptoms:** Missing textures, sounds, or crashes when accessing assets

**Possible Causes:**
- Incorrect pack file paths
- Corrupted pack files
- Missing resource entries
- Path case sensitivity issues

**Solutions:**
```cpp
// Debug resource loading
void DebugResourceLoading(UGame* game) {
    FLogger::Log(ELogLevel::Debug, "Debugging resource loading...");

    const auto& packedResources = game->GetPackedResources();

    FLogger::Log(ELogLevel::Debug,
        FString::Format("Found {} packed resource files", packedResources.size()));

    for (size_t i = 0; i < packedResources.size(); ++i) {
        const FilePath& packPath = packedResources[i];

        FLogger::Log(ELogLevel::Debug,
            FString::Format("Pack {}: {}", i, packPath));

        // Check if file exists
        if (!FFileSystem::FileExists(packPath)) {
            FLogger::Log(ELogLevel::Error,
                FString::Format("Pack file does not exist: {}", packPath));
            continue;
        }

        // Check file size
        UInt64 fileSize = FFileSystem::GetFileSize(packPath);
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Pack size: {} bytes", fileSize));

        // Try to open pack file
        FFile packFile(packPath, EFileMode::Read);
        if (!packFile.IsOpen()) {
            FLogger::Log(ELogLevel::Error,
                FString::Format("Cannot open pack file: {}", packPath));
            continue;
        }

        // Read and validate header
        FPackHeader header;
        if (packFile.Read(&header, sizeof(FPackHeader)) != sizeof(FPackHeader)) {
            FLogger::Log(ELogLevel::Error,
                FString::Format("Cannot read pack header: {}", packPath));
            continue;
        }

        if (header.magic != PACK_MAGIC) {
            FLogger::Log(ELogLevel::Error,
                FString::Format("Invalid pack magic number: {} (expected {})",
                    header.magic, PACK_MAGIC));
            continue;
        }

        FLogger::Log(ELogLevel::Debug,
            FString::Format("Pack contains {} files", header.fileCount));

        // List first few files
        for (UInt32 j = 0; j < Math<UInt32>::Min(header.fileCount, 5); ++j) {
            FPackFileEntry entry;
            if (packFile.Read(&entry, sizeof(FPackFileEntry)) == sizeof(FPackFileEntry)) {
                FLogger::Log(ELogLevel::Debug,
                    FString::Format("  File: {} ({} bytes)",
                        entry.fileName, entry.size));
            }
        }
    }
}

// Test resource loading
void TestResourceLoading(UGame* game, const FString& testResource) {
    FLogger::Log(ELogLevel::Debug,
        FString::Format("Testing resource loading: {}", testResource));

    // Try to load the resource
    TArray<UInt8> resourceData = LoadPackedResource(testResource);

    if (resourceData.IsEmpty()) {
        FLogger::Log(ELogLevel::Error,
            FString::Format("Failed to load resource: {}", testResource));
        return;
    }

    FLogger::Log(ELogLevel::Debug,
        FString::Format("Successfully loaded {} bytes", resourceData.size()));

    // Validate resource data (basic check)
    if (resourceData.size() < 4) {
        FLogger::Log(ELogLevel::Warning, "Resource data is very small");
    }

    // Check for common file headers
    if (resourceData.size() >= 4) {
        UInt32 magic = *reinterpret_cast<const UInt32*>(resourceData.GetData());

        if (magic == PNG_MAGIC) {
            FLogger::Log(ELogLevel::Debug, "Resource appears to be a PNG file");
        } else if (magic == WAV_MAGIC) {
            FLogger::Log(ELogLevel::Debug, "Resource appears to be a WAV file");
        } else {
            FLogger::Log(ELogLevel::Debug,
                FString::Format("Unknown resource type (magic: 0x{:08X})", magic));
        }
    }
}
```

#### Performance Issues

**Symptoms:** Slow startup, low frame rate, high memory usage

**Possible Causes:**
- Too many resources loaded at startup
- Inefficient update loops
- Memory leaks
- Resource loading bottlenecks

**Solutions:**
```cpp
// Profile game performance
class GameProfiler {
public:
    void StartProfiling(UGame* game) {
        FLogger::Log(ELogLevel::Info, "Starting game performance profiling...");

        // Profile startup time
        startupStartTime = GetHighResolutionTime();

        // Hook into game lifecycle
        originalBeginPlay = game->BeginPlay;
        game->BeginPlay = [this, game]() {
            Float64 beginPlayStart = GetHighResolutionTime();

            // Call original
            originalBeginPlay.call(game);

            Float64 beginPlayEnd = GetHighResolutionTime();
            FLogger::Log(ELogLevel::Debug,
                FString::Format("BeginPlay took {:.3f}ms",
                    (beginPlayEnd - beginPlayStart) * 1000.0));

            // Record startup completion
            startupEndTime = GetHighResolutionTime();
            FLogger::Log(ELogLevel::Info,
                FString::Format("Game startup took {:.3f}ms",
                    (startupEndTime - startupStartTime) * 1000.0));
        };

        // Profile tick performance
        originalTick = game->Tick;
        game->Tick = [this, game](Float32 deltaTime) {
            Float64 tickStart = GetHighResolutionTime();

            // Call original
            originalTick.call(game, deltaTime);

            Float64 tickEnd = GetHighResolutionTime();
            Float64 tickTimeMs = (tickEnd - tickStart) * 1000.0;

            // Track frame time
            frameTimes.push_back(tickTimeMs);
            if (frameTimes.size() > 60) { // Keep last 60 frames
                frameTimes.erase(frameTimes.begin());
            }

            // Calculate average
            Float64 avgFrameTime = 0.0;
            for (Float64 time : frameTimes) {
                avgFrameTime += time;
            }
            avgFrameTime /= frameTimes.size();

            // Log if performance is poor
            if (tickTimeMs > 16.67) { // Slower than 60fps
                FLogger::Log(ELogLevel::Warning,
                    FString::Format("Slow frame: {:.3f}ms (avg: {:.3f}ms)",
                        tickTimeMs, avgFrameTime));
            }
        };
    }

    void LogMemoryUsage(UGame* game) {
        // Get memory statistics
        FMemoryStats memoryStats = FMemoryManager::GetStats();

        FLogger::Log(ELogLevel::Debug,
            FString::Format("Memory Usage - Used: {}MB, Peak: {}MB, Allocations: {}",
                memoryStats.usedBytes / (1024 * 1024),
                memoryStats.peakBytes / (1024 * 1024),
                memoryStats.allocationCount));

        // Check for memory leaks
        static UInt64 lastAllocationCount = 0;
        if (memoryStats.allocationCount > lastAllocationCount + 1000) {
            FLogger::Log(ELogLevel::Warning, "Potential memory leak detected");
        }
        lastAllocationCount = memoryStats.allocationCount;
    }

private:
    Float64 startupStartTime;
    Float64 startupEndTime;

    std::function<void()> originalBeginPlay;
    std::function<void(Float32)> originalTick;

    TVector<Float64> frameTimes;
};

// Optimize resource loading
void OptimizeResourceLoading(UGame* game) {
    FLogger::Log(ELogLevel::Info, "Optimizing resource loading...");

    const auto& packedResources = game->GetPackedResources();

    // Analyze pack file sizes
    UInt64 totalSize = 0;
    for (const FilePath& packPath : packedResources) {
        UInt64 size = FFileSystem::GetFileSize(packPath);
        totalSize += size;

        FLogger::Log(ELogLevel::Debug,
            FString::Format("Pack '{}' size: {}MB",
                FFileSystem::GetFileName(packPath), size / (1024 * 1024)));
    }

    FLogger::Log(ELogLevel::Info,
        FString::Format("Total packed resources: {}MB", totalSize / (1024 * 1024)));

    // Suggest optimizations
    if (totalSize > 500 * 1024 * 1024) { // > 500MB
        FLogger::Log(ELogLevel::Warning,
            "Large resource footprint - consider streaming or compression");
    }

    // Check for unused resources
    // (This would require tracking resource usage)
}
```

### Debug Tools

Enable comprehensive game debugging:

```cpp
class GameDebugger {
public:
    static void LogGameState(UGame* game) {
        FLogger::Log(ELogLevel::Debug, "=== Game Debug Info ===");
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Name: {}", game->GetName()));
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Default Level: {}", game->GetDefaultLevel()));

        // Engine settings summary
        const FEngineSettings& settings = game->GetEngineSettings();
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Resolution: {}x{}", settings.resolution.width, settings.resolution.height));
        FLogger::Log(ELogLevel::Debug,
            FString::Format("VSync: {}", settings.enableVSync));
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Target FPS: {}", settings.targetFPS));

        // Packed resources
        const auto& resources = game->GetPackedResources();
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Packed Resources: {}", resources.size()));

        for (size_t i = 0; i < resources.size(); ++i) {
            FLogger::Log(ELogLevel::Debug,
                FString::Format("  {}: {}", i, resources[i]));
        }
    }

    static void ValidateGameIntegrity(UGame* game) {
        FLogger::Log(ELogLevel::Debug, "Validating game integrity...");

        // Check critical paths
        const FString& defaultLevel = game->GetDefaultLevel();
        if (defaultLevel.IsEmpty()) {
            FLogger::Log(ELogLevel::Error, "Default level is empty");
        }

        // Check resource accessibility
        const auto& resources = game->GetPackedResources();
        for (const FilePath& resource : resources) {
            if (!FFileSystem::FileExists(resource)) {
                FLogger::Log(ELogLevel::Error,
                    FString::Format("Resource not found: {}", resource));
            }
        }

        // Validate engine settings
        const FEngineSettings& settings = game->GetEngineSettings();
        if (settings.resolution.width <= 0 || settings.resolution.height <= 0) {
            FLogger::Log(ELogLevel::Error, "Invalid resolution");
        }

        FLogger::Log(ELogLevel::Debug, "Game integrity validation complete");
    }

    static void MonitorGamePerformance(UGame* game) {
        static Float32 lastUpdateTime = 0.0f;
        static UInt32 frameCount = 0;

        frameCount++;

        Float32 currentTime = GetWorld()->GetTime();
        Float32 deltaTime = currentTime - lastUpdateTime;

        if (deltaTime >= 1.0f) { // Update every second
            Float32 fps = frameCount / deltaTime;
            FLogger::Log(ELogLevel::Debug,
                FString::Format("Game FPS: {:.1f}", fps));

            // Reset counters
            frameCount = 0;
            lastUpdateTime = currentTime;

            // Check for performance issues
            if (fps < 30.0f) {
                FLogger::Log(ELogLevel::Warning, "Low frame rate detected");
            }
        }
    }

    static void TestGameFunctionality(UGame* game) {
        FLogger::Log(ELogLevel::Debug, "Testing game functionality...");

        // Test level management
        FString originalLevel = game->GetDefaultLevel();
        game->SetDefaultLevel("L_TestLevel");

        if (game->GetDefaultLevel() != "L_TestLevel") {
            FLogger::Log(ELogLevel::Error, "Level management test failed");
        } else {
            FLogger::Log(ELogLevel::Debug, "Level management test passed");
        }

        // Restore original level
        game->SetDefaultLevel(originalLevel);

        // Test resource management
        size_t originalCount = game->GetPackedResources().size();
        game->AddPackedResource("TestResource.pack");

        if (game->GetPackedResources().size() != originalCount + 1) {
            FLogger::Log(ELogLevel::Error, "Resource management test failed");
        } else {
            FLogger::Log(ELogLevel::Debug, "Resource management test passed");
        }
    }
};
```

## Future Enhancements

### Planned Features

1. **Advanced Resource Management**: Dynamic loading, streaming, and compression
2. **Mod Ecosystem**: Comprehensive mod support with dependency management
3. **Cloud Integration**: Cross-platform save synchronization and achievements
4. **Performance Analytics**: Built-in profiling and optimization tools
5. **Accessibility**: Comprehensive accessibility options and customizations
6. **Multiplayer Framework**: Built-in matchmaking and session management
7. **Cross-Platform Support**: Platform-specific optimizations and features
8. **Content Creation Tools**: Integrated level editor and asset pipeline

### Extension Points

The game class allows for easy extension:

```cpp
class AdvancedGame : public UGame {
public:
    // Add custom resource management
    virtual void LoadCustomResources() {
        // Load platform-specific resources
        #ifdef PLATFORM_WINDOWS
        AddPackedResource("Content/WindowsAssets.pack");
        #elif defined(PLATFORM_MACOS)
        AddPackedResource("Content/MacAssets.pack");
        #endif
    }

    // Add performance monitoring
    virtual void Tick(Float32 deltaTime) override {
        UGame::Tick(deltaTime);

        // Monitor performance
        MonitorPerformance(deltaTime);

        // Update custom systems
        UpdateCustomSystems(deltaTime);
    }

    // Add save/load functionality
    virtual void BeginPlay() override {
        UGame::BeginPlay();

        // Load saved game state
        LoadGameState();
    }

    virtual void EndPlay() override {
        // Save game state
        SaveGameState();

        UGame::EndPlay();
    }

private:
    void MonitorPerformance(Float32 deltaTime) {
        // Track frame time
        frameTimeAccumulator += deltaTime;
        frameCount++;

        if (frameTimeAccumulator >= 1.0f) { // Every second
            Float32 avgFrameTime = frameTimeAccumulator / frameCount;
            Float32 fps = 1.0f / avgFrameTime;

            // Log performance metrics
            FLogger::Log(ELogLevel::Debug,
                FString::Format("Performance - FPS: {:.1f}, Avg Frame: {:.3f}ms",
                    fps, avgFrameTime * 1000.0f));

            // Reset counters
            frameTimeAccumulator = 0.0f;
            frameCount = 0;
        }
    }

    void UpdateCustomSystems(Float32 deltaTime) {
        // Update custom game systems
        // (Analytics, achievements, etc.)
    }

    void LoadGameState() {
        // Load player progress, settings, etc.
        FJsonObject saveData = LoadJsonFile("SaveData.json");

        if (saveData.IsValid()) {
            // Restore game state
            currentLevel = saveData.GetStringField("currentLevel");
            playerScore = saveData.GetNumberField("playerScore");
            // ... more state restoration
        }
    }

    void SaveGameState() {
        // Save current game state
        FJsonObject saveData;
        saveData.SetStringField("currentLevel", currentLevel);
        saveData.SetNumberField("playerScore", playerScore);
        saveData.SetNumberField("playTime", totalPlayTime);
        // ... more state saving

        SaveJsonFile("SaveData.json", saveData);
    }

private:
    Float32 frameTimeAccumulator;
    UInt32 frameCount;

    FString currentLevel;
    Int32 playerScore;
    Float32 totalPlayTime;
};

// Custom game with analytics
class AnalyticsGame : public UGame {
public:
    virtual void BeginPlay() override {
        UGame::BeginPlay();

        // Initialize analytics
        AnalyticsManager = new FAnalyticsManager();
        AnalyticsManager->Initialize("MyGameAnalytics");

        // Track session start
        AnalyticsManager->TrackEvent("session_start", {
            {"platform", GetPlatformString()},
            {"version", GetGameVersion()},
            {"timestamp", GetCurrentTimestamp()}
        });
    }

    virtual void Tick(Float32 deltaTime) override {
        UGame::Tick(deltaTime);

        // Update analytics
        AnalyticsManager->Update(deltaTime);

        // Track gameplay metrics
        TrackGameplayMetrics(deltaTime);
    }

    virtual void EndPlay() override {
        // Track session end
        AnalyticsManager->TrackEvent("session_end", {
            {"duration", sessionDuration},
            {"score", finalScore},
            {"level", finalLevel}
        });

        // Shutdown analytics
        AnalyticsManager->Shutdown();
        delete AnalyticsManager;

        UGame::EndPlay();
    }

private:
    void TrackGameplayMetrics(Float32 deltaTime) {
        sessionDuration += deltaTime;

        // Track events based on game state
        if (playerDied) {
            AnalyticsManager->TrackEvent("player_death", {
                {"level", currentLevel},
                {"time_alive", timeAlive},
                {"cause", deathCause}
            });
            playerDied = false;
        }

        if (levelCompleted) {
            AnalyticsManager->TrackEvent("level_complete", {
                {"level", currentLevel},
                {"time", levelTime},
                {"attempts", levelAttempts}
            });
            levelCompleted = false;
        }
    }

public:
    // Analytics interface
    void TrackCustomEvent(const FString& eventName, const TMap<FString, FString>& parameters) {
        AnalyticsManager->TrackEvent(eventName, parameters);
    }

private:
    FAnalyticsManager* AnalyticsManager;

    Float32 sessionDuration;
    Int32 finalScore;
    FString finalLevel;

    // Gameplay tracking
    bool playerDied;
    Float32 timeAlive;
    FString deathCause;

    bool levelCompleted;
    FString currentLevel;
    Float32 levelTime;
    UInt32 levelAttempts;
};
```

### Advanced Features

Future versions may include:
- **Machine Learning**: AI-driven dynamic difficulty and content adaptation
- **Procedural Generation**: Algorithmically generated levels and content
- **Cloud Gaming**: Streaming game execution with local input rendering
- **Blockchain Integration**: Decentralized achievements and item ownership
- **VR/AR Support**: Virtual and augmented reality game modes
- **Mobile Optimization**: Touch controls and mobile-specific features
- **Esports Integration**: Tournament support and competitive features
- **Content Creation**: Built-in tools for user-generated content

---

*This documentation covers the complete UGame implementation as of TKD Engine v1.0.0. For the latest updates and additional examples, refer to the engine's source code and test suites.*
