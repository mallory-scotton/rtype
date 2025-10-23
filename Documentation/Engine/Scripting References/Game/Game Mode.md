# Game Mode Documentation

## Overview

The `AGameMode` class is the central authority for defining the rules, settings, and overall structure of a game match in the TKD Game Engine. It serves as the game rules arbiter, managing everything from default actor classes and player spawning to respawn mechanics, gravity settings, and world scaling. As a singleton-like authority that exists on the server, it ensures consistent game state across all clients in multiplayer scenarios.

### Key Features

- **Class Registry**: Defines default classes for actors, controllers, HUD, and spectators
- **Player Management**: Controls player count, spawning, and respawn mechanics
- **Game Rules**: Enforces match rules, win conditions, and game flow
- **World Settings**: Manages gravity, world scaling, and physics parameters
- **Server Authority**: Runs on server with replicated state to clients
- **Lifecycle Management**: Complete game session lifecycle from start to finish
- **Validation System**: Ensures game mode configuration is valid before play
- **Extensibility**: Base class designed for game-specific rule implementation

### Architecture

```cpp
class AGameMode : public UObject, public ITickable
{
protected:
    // Class Definitions
    FString m_actorClassName;              // Default pawn class
    FString m_playerControllerClassName;   // Player controller class
    FString m_gameStateClassName;          // Game state class
    FString m_hudClassName;                // HUD class
    FString m_spectatorClassName;          // Spectator class

    // Player Settings
    FString m_defaultPlayerName;           // Default player name
    UInt32 m_numPlayers;                   // Number of players
    Float32 m_minRespawnDelay;             // Minimum respawn delay
    Float32 m_maxRespawnDelay;             // Maximum respawn delay

    // World Settings
    Float32 m_gravityZ;                    // Gravity along Z-axis
    Float32 m_worldToMeters;               // World to meters scale
};
```

The `AGameMode` operates as the server-side game authority, defining the blueprint for how a game match operates while remaining independent of specific actor implementations.

## Core Components

### Class Registry System

The game mode maintains a registry of default classes used throughout the game:

```cpp
struct GameModeClasses {
    UClass* defaultPawnClass;        // Player character class
    UClass* playerControllerClass;   // Player input handler
    UClass* gameStateClass;          // Global game state
    UClass* hudClass;                // Heads-up display
    UClass* spectatorClass;          // Spectator pawn
};
```

### Player Management System

Controls player spawning, respawning, and lifecycle:

```cpp
struct PlayerSettings {
    UInt32 maxPlayers;               // Maximum player count
    Float32 minRespawnDelay;         // Minimum time before respawn
    Float32 maxRespawnDelay;         // Maximum time before respawn
    FString defaultPlayerName;       // Default player display name
};
```

### World Physics Settings

Defines global physics parameters:

```cpp
struct WorldSettings {
    Float32 gravityZ;                // Gravity acceleration (negative = down)
    Float32 worldToMeters;           // Unit conversion factor
    // Future: Wind, atmospheric density, etc.
};
```

## API Reference

### Constructor & Destructor

#### `AGameMode(const FString& name = "AGameMode")`

Creates a new game mode instance with default settings.

**Parameters:**
- `name`: Optional game mode name (defaults to "AGameMode")

**Behavior:**
- Initializes all member variables with sensible defaults
- Validates and clamps configuration values
- Registers with the game framework

### Lifecycle Methods

#### `virtual void BeginPlay(void) override`

Called when the game mode begins play (match starts).

**Behavior:**
- Sets up initial game state
- Spawns necessary game actors
- Initializes game rules
- Override in derived classes for game-specific setup

#### `virtual void Tick(Float32 deltaTime) override`

Updates the game mode each frame.

**Parameters:**
- `deltaTime`: Time elapsed since last update in seconds

**Behavior:**
- Updates game state
- Checks win/lose conditions
- Manages respawn timers
- Processes game rules

#### `virtual void EndPlay(void) override`

Called when the game mode ends play (match ends).

**Behavior:**
- Cleans up game state
- Saves statistics
- Prepares for next match
- Override for game-specific cleanup

### Validation Methods

#### `Bool IsValid(void) const`

Validates that the game mode configuration is correct.

**Returns:** True if all required classes exist and configuration is valid

**Validation Checks:**
- Actor class name is not empty and class exists
- Player controller class name is not empty and class exists
- All referenced classes are loadable

### Class Registry Getters

#### `const FString& GetActorClassName(void) const` / `UClass* GetActorClass(void) const`

Gets the default actor (pawn) class name or UClass pointer.

**Returns:** Actor class name string or UClass pointer

#### `const FString& GetPlayerControllerClassName(void) const` / `UClass* GetPlayerControllerClass(void) const`

Gets the player controller class name or UClass pointer.

**Returns:** Player controller class name string or UClass pointer

#### `const FString& GetGameStateClassName(void) const` / `UClass* GetGameStateClass(void) const`

Gets the game state class name or UClass pointer.

**Returns:** Game state class name string or UClass pointer

#### `const FString& GetHUDClassName(void) const` / `UClass* GetHUDClass(void) const`

Gets the HUD class name or UClass pointer.

**Returns:** HUD class name string or UClass pointer

#### `const FString& GetSpectatorClassName(void) const` / `UClass* GetSpectatorClass(void) const`

Gets the spectator class name or UClass pointer.

**Returns:** Spectator class name string or UClass pointer

### Player Settings Getters

#### `const FString& GetDefaultPlayerName(void) const`

Gets the default player name for new players.

**Returns:** Default player name string

#### `UInt32 GetPlayerCount(void) const`

Gets the maximum number of players allowed.

**Returns:** Maximum player count

#### `Float32 GetMinimumRespawnDelay(void) const` / `Float32 GetMaximumRespawnDelay(void) const`

Gets the minimum/maximum respawn delay times.

**Returns:** Respawn delay in seconds

### World Settings Getters

#### `Float32 GetGravityZ(void) const`

Gets the gravity acceleration along the Z-axis.

**Returns:** Gravity value (negative for downward acceleration)

#### `Float32 GetWorldToMeters(void) const`

Gets the world unit to meters conversion factor.

**Returns:** Scale factor for converting world units to meters

## Usage Examples

### Basic Game Mode Setup

```cpp
class AMyGameMode : public AGameMode
{
public:
    AMyGameMode()
        : AGameMode("MyGameMode")
    {
        // Set default classes
        m_actorClassName = "APlayerCharacter";
        m_playerControllerClassName = "APlayerController";
        m_gameStateClassName = "AGameState";
        m_hudClassName = "AGameHUD";
        m_spectatorClassName = "ASpectatorPawn";

        // Configure player settings
        m_defaultPlayerName = "Player";
        m_numPlayers = 8;
        m_minRespawnDelay = 1.0f;
        m_maxRespawnDelay = 3.0f;

        // Configure world settings
        m_gravityZ = -980.0f;        // Earth-like gravity
        m_worldToMeters = 100.0f;    // 1 world unit = 1 meter
    }

    virtual void BeginPlay() override
    {
        AGameMode::BeginPlay();

        // Initialize game-specific setup
        InitializeGameWorld();
        SpawnInitialActors();
        StartGameTimer();
    }

    virtual void Tick(Float32 deltaTime) override
    {
        AGameMode::Tick(deltaTime);

        // Update game logic
        UpdateGameTimer(deltaTime);
        CheckWinConditions();
        ProcessRespawns(deltaTime);
    }

private:
    void InitializeGameWorld()
    {
        // Spawn world actors (lights, triggers, etc.)
        GetWorld()->SpawnActor<ADirectionalLight>("SunLight");
        GetWorld()->SpawnActor<APostProcessVolume>("GlobalPostProcess");
    }

    void SpawnInitialActors()
    {
        // Spawn game-specific actors
        for (int i = 0; i < 5; ++i)
        {
            FVector3 spawnLocation = GetRandomSpawnLocation();
            GetWorld()->SpawnActor<AEnemy>("Enemy", spawnLocation);
        }
    }

    void StartGameTimer()
    {
        gameTimeRemaining = 300.0f; // 5 minutes
        gameStarted = true;
    }

    void UpdateGameTimer(Float32 deltaTime)
    {
        if (gameStarted && gameTimeRemaining > 0.0f)
        {
            gameTimeRemaining -= deltaTime;

            if (gameTimeRemaining <= 0.0f)
            {
                EndGameWithTimeout();
            }
        }
    }

    void CheckWinConditions()
    {
        // Check for win/lose conditions
        if (GetEnemyCount() == 0)
        {
            EndGameWithVictory();
        }
        else if (GetPlayerCount() == 0)
        {
            EndGameWithDefeat();
        }
    }

    void ProcessRespawns(Float32 deltaTime)
    {
        // Handle player respawns
        for (auto& respawnRequest : pendingRespawns)
        {
            respawnRequest.timer -= deltaTime;
            if (respawnRequest.timer <= 0.0f)
            {
                RespawnPlayer(respawnRequest.playerId);
                pendingRespawns.erase(
                    std::remove_if(pendingRespawns.begin(), pendingRespawns.end(),
                        [respawnRequest](const RespawnRequest& req) {
                            return req.playerId == respawnRequest.playerId;
                        }),
                    pendingRespawns.end());
            }
        }
    }

    void EndGameWithVictory()
    {
        gameStarted = false;
        // Award victory to players
        // Show victory screen
        // Save statistics
    }

    void EndGameWithDefeat()
    {
        gameStarted = false;
        // Show defeat screen
        // Save statistics
    }

    void EndGameWithTimeout()
    {
        gameStarted = false;
        // Determine winner based on score
        // Show results screen
    }

private:
    Float32 gameTimeRemaining;
    bool gameStarted;

    struct RespawnRequest {
        UInt32 playerId;
        Float32 timer;
    };
    TVector<RespawnRequest> pendingRespawns;
};
```

### Deathmatch Game Mode

```cpp
class ADeathmatchGameMode : public AGameMode
{
public:
    ADeathmatchGameMode()
        : AGameMode("DeathmatchGameMode")
    {
        m_actorClassName = "ADeathmatchCharacter";
        m_playerControllerClassName = "APlayerController";
        m_gameStateClassName = "ADeathmatchGameState";
        m_hudClassName = "ADeathmatchHUD";

        m_numPlayers = 16;
        m_minRespawnDelay = 0.5f;
        m_maxRespawnDelay = 2.0f;

        killLimit = 25;
        timeLimit = 600.0f; // 10 minutes
    }

    virtual void BeginPlay() override
    {
        AGameMode::BeginPlay();

        // Initialize scores
        playerScores.clear();

        // Spawn weapon pickups
        SpawnWeaponPickups();

        // Start match timer
        matchStartTime = GetWorld()->GetTime();
    }

    virtual void Tick(Float32 deltaTime) override
    {
        AGameMode::Tick(deltaTime);

        // Check time limit
        Float32 elapsedTime = GetWorld()->GetTime() - matchStartTime;
        if (elapsedTime >= timeLimit)
        {
            EndMatchWithTimeLimit();
            return;
        }

        // Update HUD with remaining time
        UpdateTimeDisplay(timeLimit - elapsedTime);
    }

    void OnPlayerKilled(APlayerController* killer, APlayerController* victim)
    {
        // Award kill
        if (killer)
        {
            playerScores[killer->GetPlayerId()]++;
            killer->AddScore(100); // Kill points

            // Check kill limit
            if (playerScores[killer->GetPlayerId()] >= killLimit)
            {
                EndMatchWithKillLimit(killer);
                return;
            }
        }

        // Handle victim respawn
        ScheduleRespawn(victim->GetPlayerId());
    }

    void OnPlayerRespawned(APlayerController* player)
    {
        // Give default loadout
        GiveDefaultWeapons(player);

        // Teleport to spawn point
        FTransform spawnTransform = FindSpawnLocation(player);
        player->GetPawn()->SetTransform(spawnTransform);
    }

private:
    void SpawnWeaponPickups()
    {
        TArray<FVector3> pickupLocations = GetPickupSpawnLocations();

        for (const FVector3& location : pickupLocations)
        {
            // Random weapon type
            EWeaponType weaponType = GetRandomWeaponType();
            AWeaponPickup* pickup = GetWorld()->SpawnActor<AWeaponPickup>(
                GetWeaponPickupClassName(weaponType), location);
            pickup->SetWeaponType(weaponType);
        }
    }

    void ScheduleRespawn(UInt32 playerId)
    {
        Float32 respawnTime = FMath::RandRange(m_minRespawnDelay, m_maxRespawnDelay);

        RespawnRequest request;
        request.playerId = playerId;
        request.respawnTime = GetWorld()->GetTime() + respawnTime;
        pendingRespawns.push_back(request);
    }

    void ProcessRespawns()
    {
        Float32 currentTime = GetWorld()->GetTime();

        for (auto it = pendingRespawns.begin(); it != pendingRespawns.end(); )
        {
            if (currentTime >= it->respawnTime)
            {
                RespawnPlayer(it->playerId);
                it = pendingRespawns.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    void EndMatchWithKillLimit(APlayerController* winner)
    {
        // Winner by kill limit
        BroadcastMessage(FString::Format("{} wins by reaching kill limit!",
            winner->GetPlayerName()));

        EndMatch();
    }

    void EndMatchWithTimeLimit()
    {
        // Find highest score
        UInt32 winningPlayerId = 0;
        Int32 highestScore = -1;

        for (const auto& score : playerScores)
        {
            if (score.second > highestScore)
            {
                highestScore = score.second;
                winningPlayerId = score.first;
            }
        }

        APlayerController* winner = FindPlayerController(winningPlayerId);
        if (winner)
        {
            BroadcastMessage(FString::Format("{} wins with {} kills!",
                winner->GetPlayerName(), highestScore));
        }

        EndMatch();
    }

    void EndMatch()
    {
        // Stop accepting new players
        // Show final scores
        // Transition to post-match state
        // Save match statistics
    }

private:
    Int32 killLimit;
    Float32 timeLimit;
    Float32 matchStartTime;

    TMap<UInt32, Int32> playerScores; // Player ID -> Kill count

    struct RespawnRequest {
        UInt32 playerId;
        Float32 respawnTime;
    };
    TVector<RespawnRequest> pendingRespawns;
};
```

### Capture the Flag Game Mode

```cpp
class ACTFGameMode : public AGameMode
{
public:
    ACTFGameMode()
        : AGameMode("CTFGameMode")
    {
        m_actorClassName = "ACTFCharacter";
        m_playerControllerClassName = "ACTFPlayerController";
        m_gameStateClassName = "ACTFGameState";
        m_hudClassName = "ACTFHUD";

        m_numPlayers = 12; // 6 vs 6
        m_minRespawnDelay = 2.0f;
        m_maxRespawnDelay = 5.0f;

        flagCaptureLimit = 3;
        matchTimeLimit = 900.0f; // 15 minutes
    }

    virtual void BeginPlay() override
    {
        AGameMode::BeginPlay();

        // Initialize team scores
        teamScores[ETeam::Red] = 0;
        teamScores[ETeam::Blue] = 0;

        // Spawn flags
        redFlag = SpawnFlag(ETeam::Red, redFlagSpawnLocation);
        blueFlag = SpawnFlag(ETeam::Blue, blueFlagSpawnLocation);

        // Assign players to teams
        AssignPlayersToTeams();

        matchStartTime = GetWorld()->GetTime();
    }

    virtual void Tick(Float32 deltaTime) override
    {
        AGameMode::Tick(deltaTime);

        // Check time limit
        Float32 elapsedTime = GetWorld()->GetTime() - matchStartTime;
        if (elapsedTime >= matchTimeLimit)
        {
            EndMatchWithTimeLimit();
            return;
        }

        // Update flag status
        UpdateFlagStatus();
    }

    void OnFlagCaptured(APlayerController* capturer, ETeam capturedTeam)
    {
        ETeam capturerTeam = GetPlayerTeam(capturer);

        // Award capture points
        teamScores[capturerTeam]++;

        // Broadcast capture message
        BroadcastMessage(FString::Format("{} captured the {} flag!",
            capturer->GetPlayerName(),
            capturedTeam == ETeam::Red ? "red" : "blue"));

        // Check win condition
        if (teamScores[capturerTeam] >= flagCaptureLimit)
        {
            EndMatchWithCaptureLimit(capturerTeam);
            return;
        }

        // Respawn flag
        RespawnFlag(capturedTeam);
    }

    void OnFlagPickedUp(APlayerController* carrier, ETeam flagTeam)
    {
        // Update flag status
        if (flagTeam == ETeam::Red)
        {
            redFlagCarrier = carrier;
            redFlag->SetVisible(false);
        }
        else
        {
            blueFlagCarrier = carrier;
            blueFlag->SetVisible(false);
        }

        // Notify team
        BroadcastTeamMessage(carrier,
            FString::Format("Your flag has been taken by {}!",
                carrier->GetPlayerName()));
    }

    void OnFlagDropped(APlayerController* dropper, ETeam flagTeam)
    {
        // Drop flag at current location
        FVector3 dropLocation = dropper->GetPawn()->GetTransform().GetPosition();

        if (flagTeam == ETeam::Red)
        {
            redFlag->SetTransform(FTransform(dropLocation));
            redFlag->SetVisible(true);
            redFlagCarrier = nullptr;
        }
        else
        {
            blueFlag->SetTransform(FTransform(dropLocation));
            blueFlag->SetVisible(true);
            blueFlagCarrier = nullptr;
        }
    }

private:
    AFlag* SpawnFlag(ETeam team, const FVector3& location)
    {
        FString flagName = (team == ETeam::Red) ? "RedFlag" : "BlueFlag";
        AFlag* flag = GetWorld()->SpawnActor<AFlag>(flagName, location);
        flag->SetTeam(team);
        return flag;
    }

    void AssignPlayersToTeams()
    {
        // Simple team assignment (alternate players)
        TArray<APlayerController*> players = GetAllPlayerControllers();

        for (size_t i = 0; i < players.size(); ++i)
        {
            ETeam team = (i % 2 == 0) ? ETeam::Red : ETeam::Blue;
            AssignPlayerToTeam(players[i], team);
        }
    }

    void UpdateFlagStatus()
    {
        // Update flag carrier status
        if (redFlagCarrier)
        {
            // Check if carrier is in enemy base
            if (IsInEnemyBase(redFlagCarrier, ETeam::Blue))
            {
                OnFlagCaptured(redFlagCarrier, ETeam::Red);
            }
        }

        if (blueFlagCarrier)
        {
            if (IsInEnemyBase(blueFlagCarrier, ETeam::Red))
            {
                OnFlagCaptured(blueFlagCarrier, ETeam::Blue);
            }
        }
    }

    void RespawnFlag(ETeam team)
    {
        if (team == ETeam::Red)
        {
            redFlag->SetTransform(FTransform(redFlagSpawnLocation));
            redFlag->SetVisible(true);
            redFlagCarrier = nullptr;
        }
        else
        {
            blueFlag->SetTransform(FTransform(blueFlagSpawnLocation));
            blueFlag->SetVisible(true);
            blueFlagCarrier = nullptr;
        }
    }

    void EndMatchWithCaptureLimit(ETeam winningTeam)
    {
        BroadcastMessage(FString::Format("Team {} wins by capturing {} flags!",
            winningTeam == ETeam::Red ? "Red" : "Blue", flagCaptureLimit));
        EndMatch();
    }

    void EndMatchWithTimeLimit()
    {
        // Determine winner by score
        if (teamScores[ETeam::Red] > teamScores[ETeam::Blue])
        {
            BroadcastMessage("Red team wins!");
        }
        else if (teamScores[ETeam::Blue] > teamScores[ETeam::Red])
        {
            BroadcastMessage("Blue team wins!");
        }
        else
        {
            BroadcastMessage("Match ends in a tie!");
        }
        EndMatch();
    }

private:
    Int32 flagCaptureLimit;
    Float32 matchTimeLimit;
    Float32 matchStartTime;

    TMap<ETeam, Int32> teamScores;

    AFlag* redFlag;
    AFlag* blueFlag;
    FVector3 redFlagSpawnLocation;
    FVector3 blueFlagSpawnLocation;

    APlayerController* redFlagCarrier;
    APlayerController* blueFlagCarrier;
};
```

### Custom Game Mode with Mods

```cpp
class AModdableGameMode : public AGameMode
{
public:
    AModdableGameMode()
        : AGameMode("ModdableGameMode")
    {
        // Load mod configuration
        LoadModConfiguration();
    }

    virtual void BeginPlay() override
    {
        AGameMode::BeginPlay();

        // Initialize mods
        InitializeMods();

        // Apply modded settings
        ApplyModSettings();
    }

    virtual void Tick(Float32 deltaTime) override
    {
        AGameMode::Tick(deltaTime);

        // Update mods
        UpdateMods(deltaTime);
    }

private:
    void LoadModConfiguration()
    {
        // Load mod list from configuration file
        FString modConfigPath = "Mods/GameModeMods.json";
        FJsonObject modConfig = LoadJsonFile(modConfigPath);

        if (modConfig.IsValid())
        {
            // Parse mod list
            TArray<FString> modList = modConfig.GetArrayField("mods");

            for (const FString& modName : modList)
            {
                LoadMod(modName);
            }
        }
    }

    void LoadMod(const FString& modName)
    {
        // Load mod DLL/library
        FString modPath = FString::Format("Mods/{}.dll", modName);
        void* modHandle = LoadLibrary(modPath);

        if (modHandle)
        {
            // Get mod interface
            GetModInterfaceFunc getInterface =
                (GetModInterfaceFunc)GetProcAddress(modHandle, "GetModInterface");

            if (getInterface)
            {
                IGameModeMod* mod = getInterface();
                if (mod)
                {
                    loadedMods.push_back(mod);
                    mod->OnLoad(this);
                }
            }
        }
    }

    void InitializeMods()
    {
        for (IGameModeMod* mod : loadedMods)
        {
            mod->Initialize();
        }
    }

    void ApplyModSettings()
    {
        for (IGameModeMod* mod : loadedMods)
        {
            mod->ApplySettings(this);
        }
    }

    void UpdateMods(Float32 deltaTime)
    {
        for (IGameModeMod* mod : loadedMods)
        {
            mod->Update(deltaTime);
        }
    }

public:
    // Mod interface
    void RegisterModdedClass(const FString& classType, const FString& className)
    {
        moddedClasses[classType] = className;
    }

    FString GetModdedClass(const FString& classType) const
    {
        auto it = moddedClasses.find(classType);
        return it != moddedClasses.end() ? it->second : "";
    }

    void AddModdedRule(IGameRule* rule)
    {
        moddedRules.push_back(rule);
    }

private:
    TVector<IGameModeMod*> loadedMods;
    TMap<FString, FString> moddedClasses;
    TVector<IGameRule*> moddedRules;
};
```

## Game Rules System

### Rule Engine Architecture

The game mode implements a rule engine for enforcing game logic:

```cpp
class IGameRule {
public:
    virtual void Initialize(AGameMode* gameMode) = 0;
    virtual void Evaluate(Float32 deltaTime) = 0;
    virtual bool IsConditionMet() const = 0;
    virtual void Execute() = 0;
};

class WinConditionRule : public IGameRule {
public:
    virtual void Evaluate(Float32 deltaTime) override {
        // Check win conditions
        if (CheckVictoryConditions()) {
            Execute();
        }
    }

    virtual void Execute() override {
        // Execute win logic
        gameMode->EndGameWithVictory();
    }
};
```

### Match State Management

Game modes manage match state transitions:

```cpp
enum class EMatchState {
    WaitingForPlayers,
    Starting,
    InProgress,
    Ending,
    Finished
};

class AGameMode {
private:
    EMatchState currentMatchState;

public:
    void SetMatchState(EMatchState newState) {
        if (currentMatchState != newState) {
            OnMatchStateChanged(currentMatchState, newState);
            currentMatchState = newState;
        }
    }

    UFunction<EMatchState, EMatchState> OnMatchStateChanged;
};
```

## Player Management

### Player Lifecycle

The game mode manages the complete player lifecycle:

```cpp
class AGameMode {
public:
    virtual APlayerController* Login(const FString& playerName, const FString& uniqueId) {
        // Create player controller
        APlayerController* pc = SpawnPlayerController();

        // Assign player info
        pc->SetPlayerName(playerName);
        pc->SetUniqueId(uniqueId);

        // Add to player list
        players.push_back(pc);

        // Notify of login
        OnPlayerLogin(pc);

        return pc;
    }

    virtual void Logout(APlayerController* player) {
        // Remove from player list
        players.erase(std::remove(players.begin(), players.end(), player), players.end());

        // Clean up player state
        if (player->GetPawn()) {
            player->GetPawn()->MarkForDeletion();
        }

        // Notify of logout
        OnPlayerLogout(player);

        // Destroy player controller
        player->MarkForDeletion();
    }

    virtual void RestartPlayer(APlayerController* player) {
        // Spawn new pawn
        APawn* pawn = SpawnDefaultPawnFor(player);

        // Possess the pawn
        player->Possess(pawn);

        // Notify of restart
        OnPlayerRestart(player);
    }
};
```

### Team Management

For team-based game modes:

```cpp
class ATeamBasedGameMode : public AGameMode {
protected:
    TMap<ETeam, TVector<APlayerController*>> teamPlayers;
    TMap<ETeam, Int32> teamScores;

public:
    void AssignPlayerToTeam(APlayerController* player, ETeam team) {
        // Remove from current team
        for (auto& teamPair : teamPlayers) {
            teamPair.second.erase(
                std::remove(teamPair.second.begin(), teamPair.second.end(), player),
                teamPair.second.end());
        }

        // Add to new team
        teamPlayers[team].push_back(player);
        player->SetTeam(team);

        OnPlayerTeamChanged(player, team);
    }

    ETeam GetPlayerTeam(APlayerController* player) const {
        for (const auto& teamPair : teamPlayers) {
            if (std::find(teamPair.second.begin(), teamPair.second.end(), player)
                != teamPair.second.end()) {
                return teamPair.first;
            }
        }
        return ETeam::None;
    }

    void AddTeamScore(ETeam team, Int32 points) {
        teamScores[team] += points;
        OnTeamScoreChanged(team, teamScores[team]);
    }
};
```

## Performance Considerations

### Memory Usage

- **Class Registry**: Minimal storage for class name strings
- **Player Management**: Scales with player count
- **Rule Engine**: Depends on number of active rules
- **Mod Support**: Additional memory for loaded mods

### CPU Performance

- **Validation**: Performed once at startup
- **Rule Evaluation**: Every frame for active rules
- **Player Management**: Scales with player count
- **Mod Updates**: Additional processing for loaded mods

### Optimization Strategies

1. **Lazy Evaluation**: Only evaluate rules when necessary
2. **Rule Prioritization**: Process critical rules first
3. **Player Batching**: Group player operations
4. **Mod Caching**: Cache mod-loaded resources

### Best Practices

- **Rule Simplicity**: Keep individual rules focused and efficient
- **State Caching**: Cache frequently accessed game state
- **Event-Driven**: Use events rather than polling for state changes
- **Mod Compatibility**: Design with mod extensibility in mind

## Architecture Diagrams

### Game Mode Class Hierarchy

```
UObject (Base Object)
├── UUID: Unique identifier
├── Name: Object name
├── Properties: Reflection system
├── Events: Object lifecycle
└── Serialization: Data persistence

ITickable (Update Interface)
└── Tick(deltaTime): Frame update method

AGameMode (Game Authority)
├── Class Registry: Actor/Controller/HUD classes
├── Player Settings: Count, respawn, naming
├── World Settings: Gravity, scaling
├── Rule Engine: Game logic evaluation
├── Match State: Current game phase
├── Team Management: Player team assignment
└── Mod Support: Runtime extensibility
```

### Game Mode Lifecycle

```
Game Startup ──► Validate Configuration ──► Initialize World ──► Spawn Actors
      │                      │                        │              │
      ▼                      ▼                        ▼              ▼
Load Classes        Check Dependencies        Create Game State   Begin Match
(Actors, HUD)       (Validate UClasses)       (Rules, Settings)   (Start Timer)
      │                      │                        │              │
      └──────────────────────┼────────────────────────┼──────────────┘
                             ▼                        ▼              ▼
                    Match In Progress ◄─────── Player Actions ──────┘
                    ├── Rule Evaluation      ├── Input Processing
                    ├── State Updates        ├── Movement
                    ├── Win Condition Checks ├── Combat
                    └── Time Management      └── Respawning
                                           │              │
                                           ▼              ▼
                                    Match End ◄─────────── Win/Lose
                                    ├── Score Calculation
                                    ├── Statistics Saving
                                    ├── Cleanup
                                    └── Next Match Setup
```

### Player Management Flow

```
Player Join ──► Validate Player ──► Assign Team ──► Spawn Pawn ──► Possess Pawn
      │              │                   │              │              │
      ▼              ▼                   ▼              ▼              ▼
Check Limits    Check Ban List      Balance Teams   Find Spawn Loc  Setup Input
(Max Players)   (Unique ID)         (Auto-assign)   (Safe Location) (Bind Keys)
      │              │                   │              │              │
      └──────────────┼───────────────────┼──────────────┼──────────────┘
                     ▼                   ▼              ▼              ▼
            Player Active ◄─────── Game Participation ──────┘
            ├── Input Processing      ├── Movement
            ├── Action Execution      ├── Combat
            ├── Score Tracking        ├── Achievement Progress
            └── Statistics Collection └── Voice Chat
                                   │              │
                                   ▼              ▼
                            Player Leave ◄─────── Disconnect/Kick
                            ├── Save Statistics
                            ├── Clean Up Pawn
                            ├── Update Team Balance
                            └── Free Player Slot
```

### Rule Engine Architecture

```
AGameMode
├── Rule Registry: TArray<IGameRule*>
├── Rule Evaluation: Tick-based processing
└── Rule Dependencies: Inter-rule relationships
    │
    ▼
IGameRule (Interface)
├── Initialize(): Setup rule state
├── Evaluate(): Check conditions
├── IsConditionMet(): State query
└── Execute(): Perform rule action
    │
    ▼
Concrete Rules
├── WinConditionRule: Check victory conditions
├── RespawnRule: Handle player respawning
├── ScoreRule: Manage scoring system
├── TimeLimitRule: Enforce match time limits
└── Custom Rules: Game-specific logic
```

## Troubleshooting

### Common Issues

#### Game Mode Not Loading

**Symptoms:** Game fails to start with invalid game mode errors

**Possible Causes:**
- Required classes not found
- Invalid class names
- Missing dependencies
- Configuration errors

**Solutions:**
```cpp
// Validate game mode before use
void ValidateGameMode(AGameMode* gameMode) {
    if (!gameMode) {
        FLogger::Log(ELogLevel::Error, "Game mode is null");
        return;
    }

    if (!gameMode->IsValid()) {
        FLogger::Log(ELogLevel::Error, "Game mode validation failed");

        // Check individual components
        if (gameMode->GetActorClass() == nullptr) {
            FLogger::Log(ELogLevel::Error,
                FString::Format("Actor class '{}' not found", gameMode->GetActorClassName()));
        }

        if (gameMode->GetPlayerControllerClass() == nullptr) {
            FLogger::Log(ELogLevel::Error,
                FString::Format("Player controller class '{}' not found",
                    gameMode->GetPlayerControllerClassName()));
        }

        if (gameMode->GetHUDClass() == nullptr) {
            FLogger::Log(ELogLevel::Error,
                FString::Format("HUD class '{}' not found", gameMode->GetHUDClassName()));
        }

        return;
    }

    FLogger::Log(ELogLevel::Debug, "Game mode validation passed");
}

// Load game mode with error handling
AGameMode* LoadGameMode(const FString& gameModeName) {
    try {
        UClass* gameModeClass = UClass::FindClass(gameModeName);
        if (!gameModeClass) {
            FLogger::Log(ELogLevel::Error,
                FString::Format("Game mode class '{}' not found", gameModeName));
            return nullptr;
        }

        AGameMode* gameMode = static_cast<AGameMode*>(
            gameModeClass->CreateInstance(gameModeName));

        if (!gameMode) {
            FLogger::Log(ELogLevel::Error,
                FString::Format("Failed to create game mode '{}'", gameModeName));
            return nullptr;
        }

        if (!gameMode->IsValid()) {
            FLogger::Log(ELogLevel::Error,
                FString::Format("Game mode '{}' is not valid", gameModeName));
            delete gameMode;
            return nullptr;
        }

        return gameMode;
    }
    catch (const std::exception& e) {
        FLogger::Log(ELogLevel::Error,
            FString::Format("Exception loading game mode '{}': {}", gameModeName, e.what()));
        return nullptr;
    }
}
```

#### Players Not Spawning

**Symptoms:** Players join but no pawn is created

**Possible Causes:**
- Invalid spawn locations
- Actor class instantiation failure
- Possession failures
- Network synchronization issues

**Solutions:**
```cpp
// Debug player spawning
void DebugPlayerSpawning(AGameMode* gameMode, APlayerController* player) {
    FLogger::Log(ELogLevel::Debug,
        FString::Format("Attempting to spawn pawn for player '{}'", player->GetPlayerName()));

    // Check actor class
    UClass* actorClass = gameMode->GetActorClass();
    if (!actorClass) {
        FLogger::Log(ELogLevel::Error,
            FString::Format("Actor class '{}' not found", gameMode->GetActorClassName()));
        return;
    }

    // Find spawn location
    FTransform spawnTransform = gameMode->FindPlayerStart(player);
    FLogger::Log(ELogLevel::Debug,
        FString::Format("Spawn location: ({}, {}, {})",
            spawnTransform.GetPosition().x,
            spawnTransform.GetPosition().y,
            spawnTransform.GetPosition().z));

    // Attempt spawn
    APawn* pawn = GetWorld()->SpawnActor<APawn>(
        gameMode->GetActorClassName(), spawnTransform);

    if (!pawn) {
        FLogger::Log(ELogLevel::Error, "Failed to spawn pawn");
        return;
    }

    FLogger::Log(ELogLevel::Debug,
        FString::Format("Pawn spawned: {}", pawn->GetName()));

    // Attempt possession
    player->Possess(pawn);

    if (player->GetPawn() == pawn) {
        FLogger::Log(ELogLevel::Debug, "Possession successful");
    } else {
        FLogger::Log(ELogLevel::Error, "Possession failed");
    }
}

// Validate spawn locations
void ValidateSpawnLocations(AGameMode* gameMode) {
    TArray<APlayerStart*> spawnPoints = GetWorld()->FindActorsOfType<APlayerStart>();

    FLogger::Log(ELogLevel::Debug,
        FString::Format("Found {} player start locations", spawnPoints.size()));

    for (size_t i = 0; i < spawnPoints.size(); ++i) {
        APlayerStart* spawn = spawnPoints[i];
        FVector3 location = spawn->GetTransform().GetPosition();

        FLogger::Log(ELogLevel::Debug,
            FString::Format("Spawn {}: ({}, {}, {}) - Enabled: {}",
                i, location.x, location.y, location.z, spawn->IsEnabled()));

        // Check for obstacles
        if (GetWorld()->LineTrace(location + FVector3(0, 0, 100),
                                location - FVector3(0, 0, 10))) {
            FLogger::Log(ELogLevel::Warning,
                FString::Format("Spawn {} has geometry above/below", i));
        }
    }
}
```

#### Game Rules Not Working

**Symptoms:** Win conditions not triggering, respawns not happening

**Possible Causes:**
- Rule evaluation timing issues
- Incorrect rule conditions
- State synchronization problems
- Event handler failures

**Solutions:**
```cpp
// Debug rule evaluation
void DebugRuleEvaluation(AGameMode* gameMode) {
    FLogger::Log(ELogLevel::Debug, "Evaluating game rules...");

    // Check common conditions manually
    Int32 playerCount = gameMode->GetPlayerCount();
    Int32 enemyCount = GetEnemyCount();

    FLogger::Log(ELogLevel::Debug,
        FString::Format("Players: {}, Enemies: {}", playerCount, enemyCount));

    // Check win conditions
    if (enemyCount == 0) {
        FLogger::Log(ELogLevel::Debug, "Win condition met: All enemies defeated");
    } else if (playerCount == 0) {
        FLogger::Log(ELogLevel::Debug, "Lose condition met: All players defeated");
    }

    // Check respawn timers
    FLogger::Log(ELogLevel::Debug,
        FString::Format("Min respawn delay: {}, Max: {}",
            gameMode->GetMinimumRespawnDelay(), gameMode->GetMaximumRespawnDelay()));
}

// Test rule execution
void TestRuleExecution(AGameMode* gameMode) {
    // Manually trigger win condition
    FLogger::Log(ELogLevel::Debug, "Manually triggering win condition...");
    gameMode->EndGameWithVictory();

    // Check if game state changed
    // (Implementation depends on game state tracking)
}

// Validate event handlers
void ValidateEventHandlers(AGameMode* gameMode) {
    // Check if critical events are bound
    if (!gameMode->OnPlayerKilled.IsBound()) {
        FLogger::Log(ELogLevel::Warning, "OnPlayerKilled event not bound");
    }

    if (!gameMode->OnPlayerRespawned.IsBound()) {
        FLogger::Log(ELogLevel::Warning, "OnPlayerRespawned event not bound");
    }

    if (!gameMode->OnGameEnded.IsBound()) {
        FLogger::Log(ELogLevel::Warning, "OnGameEnded event not bound");
    }
}
```

#### Performance Issues with Many Players

**Symptoms:** Frame rate drops with many players

**Possible Causes:**
- Inefficient player iteration
- Too many rule evaluations
- Network overhead
- Memory leaks in player management

**Solutions:**
```cpp
// Profile game mode performance
class GameModeProfiler {
public:
    void ProfileGameMode(AGameMode* gameMode) {
        auto start = std::chrono::high_resolution_clock::now();

        gameMode->Tick(1.0f / 60.0f);

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        FLogger::Log(ELogLevel::Debug,
            FString::Format("Game mode tick: {}us", duration.count()));

        if (duration.count() > 5000) { // > 5ms at 60fps
            FLogger::Log(ELogLevel::Warning, "Game mode performance is poor");
            OptimizeGameMode(gameMode);
        }
    }

private:
    void OptimizeGameMode(AGameMode* gameMode) {
        // Implement optimizations:
        // 1. Cache frequently accessed data
        // 2. Use spatial partitioning for player queries
        // 3. Batch network updates
        // 4. Implement rule evaluation throttling
        // 5. Use object pooling for temporary objects
    }
};

// Implement player management optimizations
class OptimizedPlayerManager {
public:
    void UpdatePlayers(const TVector<APlayerController*>& players, Float32 deltaTime) {
        // Use parallel processing for independent operations
        #pragma omp parallel for
        for (int i = 0; i < players.size(); ++i) {
            UpdatePlayer(players[i], deltaTime);
        }
    }

private:
    void UpdatePlayer(APlayerController* player, Float32 deltaTime) {
        // Update player-specific logic
        // (Movement, input processing, etc.)
    }
};
```

### Debug Tools

Enable comprehensive game mode debugging:

```cpp
class GameModeDebugger {
public:
    static void LogGameModeState(AGameMode* gameMode) {
        FLogger::Log(ELogLevel::Debug, "=== Game Mode Debug Info ===");
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Name: {}", gameMode->GetName()));
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Valid: {}", gameMode->IsValid()));

        // Class information
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Actor Class: {} ({})",
                gameMode->GetActorClassName(),
                gameMode->GetActorClass() ? "Found" : "Not Found"));

        FLogger::Log(ELogLevel::Debug,
            FString::Format("Player Controller: {} ({})",
                gameMode->GetPlayerControllerClassName(),
                gameMode->GetPlayerControllerClass() ? "Found" : "Not Found"));

        FLogger::Log(ELogLevel::Debug,
            FString::Format("HUD Class: {} ({})",
                gameMode->GetHUDClassName(),
                gameMode->GetHUDClass() ? "Found" : "Not Found"));

        // Player settings
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Max Players: {}", gameMode->GetPlayerCount()));
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Default Name: {}", gameMode->GetDefaultPlayerName()));
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Respawn Delay: {} - {}",
                gameMode->GetMinimumRespawnDelay(), gameMode->GetMaximumRespawnDelay()));

        // World settings
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Gravity Z: {}", gameMode->GetGravityZ()));
        FLogger::Log(ELogLevel::Debug,
            FString::Format("World to Meters: {}", gameMode->GetWorldToMeters()));
    }

    static void ValidateGameModeConfiguration(AGameMode* gameMode) {
        FLogger::Log(ELogLevel::Debug, "Validating game mode configuration...");

        // Check class existence
        TArray<std::pair<FString, UClass*>> classesToCheck = {
            {gameMode->GetActorClassName(), gameMode->GetActorClass()},
            {gameMode->GetPlayerControllerClassName(), gameMode->GetPlayerControllerClass()},
            {gameMode->GetGameStateClassName(), gameMode->GetGameStateClass()},
            {gameMode->GetHUDClassName(), gameMode->GetHUDClass()},
            {gameMode->GetSpectatorClassName(), gameMode->GetSpectatorClass()}
        };

        for (const auto& classPair : classesToCheck) {
            if (classPair.first.IsEmpty()) {
                FLogger::Log(ELogLevel::Warning,
                    FString::Format("Class name is empty for: {}", classPair.first));
            } else if (!classPair.second) {
                FLogger::Log(ELogLevel::Error,
                    FString::Format("Class '{}' not found", classPair.first));
            } else {
                FLogger::Log(ELogLevel::Debug,
                    FString::Format("Class '{}' found", classPair.first));
            }
        }

        // Validate numeric settings
        if (gameMode->GetPlayerCount() <= 0) {
            FLogger::Log(ELogLevel::Warning, "Player count is invalid");
        }

        if (gameMode->GetMinimumRespawnDelay() < 0.0f ||
            gameMode->GetMaximumRespawnDelay() < gameMode->GetMinimumRespawnDelay()) {
            FLogger::Log(ELogLevel::Warning, "Respawn delays are invalid");
        }

        if (gameMode->GetWorldToMeters() <= 0.0f) {
            FLogger::Log(ELogLevel::Warning, "World to meters scale is invalid");
        }
    }

    static void MonitorGameModePerformance(AGameMode* gameMode) {
        static Float32 lastUpdateTime = 0.0f;
        static Int32 frameCount = 0;

        frameCount++;
        Float32 currentTime = GetWorld()->GetTime();

        if (currentTime - lastUpdateTime >= 1.0f) { // Update every second
            Float32 fps = frameCount / (currentTime - lastUpdateTime);
            FLogger::Log(ELogLevel::Debug,
                FString::Format("Game mode FPS: {:.1f}", fps));

            lastUpdateTime = currentTime;
            frameCount = 0;
        }
    }

    static void TestGameModeRules(AGameMode* gameMode) {
        FLogger::Log(ELogLevel::Debug, "Testing game mode rules...");

        // Test win conditions
        // (This would depend on specific game mode implementation)

        // Test respawn logic
        Float32 minDelay = gameMode->GetMinimumRespawnDelay();
        Float32 maxDelay = gameMode->GetMaximumRespawnDelay();

        FLogger::Log(ELogLevel::Debug,
            FString::Format("Respawn delay range: {} - {}", minDelay, maxDelay));

        // Test player limits
        UInt32 maxPlayers = gameMode->GetPlayerCount();
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Max players: {}", maxPlayers));
    }
};
```

## Future Enhancements

### Planned Features

1. **Advanced Rule Engine**: Visual rule editing and complex condition builders
2. **Dynamic Game Modes**: Runtime game mode switching and hybrid modes
3. **Mod Ecosystem**: Comprehensive mod support with dependency management
4. **Tournament System**: Automated tournament bracket management
5. **Statistics Engine**: Detailed match statistics and analytics
6. **Anti-Cheat Integration**: Server-side cheat detection and prevention
7. **Cross-Platform Support**: Platform-specific optimizations and features
8. **AI Game Modes**: AI vs AI matches and training scenarios

### Extension Points

The game mode system allows for easy extension:

```cpp
class AdvancedGameMode : public AGameMode {
public:
    // Add custom rule system
    virtual void RegisterRule(IGameRule* rule) {
        rules.push_back(rule);
        rule->Initialize(this);
    }

    virtual void EvaluateRules(Float32 deltaTime) {
        for (IGameRule* rule : rules) {
            rule->Evaluate(deltaTime);
        }
    }

    // Add mod support
    virtual void LoadMod(IGameModeMod* mod) {
        mods.push_back(mod);
        mod->OnLoad(this);
    }

    // Add custom player management
    virtual void OnPlayerLogin(APlayerController* player) override {
        AGameMode::OnPlayerLogin(player);
        // Custom login logic
    }

    // Add tournament support
    virtual void StartTournament(const FTournamentConfig& config) {
        // Initialize tournament mode
    }

private:
    TVector<IGameRule*> rules;
    TVector<IGameModeMod*> mods;
};

// Custom rule implementation
class TimeLimitRule : public IGameRule {
public:
    TimeLimitRule(Float32 timeLimit) : maxTime(timeLimit), elapsedTime(0.0f) {}

    virtual void Initialize(AGameMode* gameMode) override {
        this->gameMode = gameMode;
    }

    virtual void Evaluate(Float32 deltaTime) override {
        elapsedTime += deltaTime;
        if (elapsedTime >= maxTime) {
            Execute();
        }
    }

    virtual bool IsConditionMet() const override {
        return elapsedTime >= maxTime;
    }

    virtual void Execute() override {
        gameMode->EndGameWithTimeLimit();
    }

private:
    AGameMode* gameMode;
    Float32 maxTime;
    Float32 elapsedTime;
};
```

### Advanced Features

Future versions may include:
- **Machine Learning**: AI-driven dynamic difficulty and rule adaptation
- **Procedural Generation**: Algorithmically generated game modes and maps
- **Cloud Integration**: Cross-server matchmaking and statistics
- **Esports Features**: Professional tournament support and spectator systems
- **Accessibility**: Comprehensive accessibility options and customizations
- **Mobile Support**: Touch controls and mobile-specific optimizations
- **VR Integration**: Virtual reality game mode adaptations
- **Blockchain Integration**: Decentralized tournament and achievement systems

---

*This documentation covers the complete AGameMode implementation as of TKD Engine v1.0.0. For the latest updates and additional examples, refer to the engine's source code and test suites.*
