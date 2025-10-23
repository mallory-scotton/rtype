# World Documentation

## Overview

The `UWorld` class represents the game world in the TKD Game Engine. It serves as the central hub for managing all actors, handling the game loop, coordinating networking, and maintaining the current level state. The world is responsible for actor lifecycle management, physics simulation, rendering coordination, and network synchronization.

### Key Features

- **Actor Management**: Creation, destruction, and lifecycle management of all actors
- **Game Loop**: Tick-based update system with delta time
- **Level Management**: Loading and switching between game levels
- **Networking**: RPC system for client-server communication
- **Rendering**: Coordination of rendering for all actors
- **Time Management**: World time tracking and time-based operations
- **Authority System**: Client-server authority management
- **Snapshot System**: State synchronization for multiplayer

### Architecture

```cpp
class UWorld : public UObject, public ITickable
{
private:
    std::vector<std::shared_ptr<AActor>> m_actors;      // All actors in world
    float m_worldTime;                                  // Current world time
    ULevel m_currentLevel;                              // Active level
    std::vector<ULevel> m_loadedLevels;                 // Available levels
    UInt32 m_lastSnapshotID;                            // Network sync tracking

public:
    // RPC Functions for networking
    UFunction<FString, FTransform, UUID, UInt32> SpawnActorRPC;
    UFunction<UUID> DestroyActorRPC;
    UFunction<UInt32> SpawnClientRPC;
    UFunction<UInt32, UUID, FTransform> SpawnPlayerRPC;
    UFunction<std::vector<Byte>> SyncSnapshotRPC;
};
```

The `UWorld` acts as the root container and coordinator for all game systems, managing the complex interactions between actors, levels, and network systems.

## Core Components

### Actor Lifecycle Management

The world manages actor creation, updates, and destruction:

```cpp
enum class ActorState {
    Spawning,    // Being created
    Active,      // Normal operation
    Inactive,    // Temporarily disabled
    Destroying,  // Being destroyed
    Destroyed    // Fully removed
};
```

### Game Loop Structure

```
BeginPlay() ──► Tick Loop ──► EndPlay()
       │             │             │
       ▼             ▼             ▼
Initialize     Update World    Cleanup World
Actors         Time + Delta    Destroy Actors
Setup Game     Tick Actors     Save State
Mode           Tick Game Mode
```

### Network Authority System

```
Server (Authority) ──► Client (Simulated Proxy)
       │                        │
       ▼                        ▼
Full Control              Receive Updates
Send Commands             Predict Movement
Validate Actions          Reconcile State
Broadcast State           Handle Latency
```

## API Reference

### Constructor & Initialization

#### `UWorld(const FString& name = "UWorld")`

Creates a new game world.

**Parameters:**
- `name`: World identifier (default: "UWorld")

**Behavior:**
- Initializes actor container
- Sets world time to 0.0
- Creates default level
- Sets up RPC functions
- Configures network role (Authority on server, SimulatedProxy on client)
- Auto-loads available levels from registered classes

**Network Role Configuration:**
```cpp
#if TKD_ENGINE_SERVER
    SetNetRole(ENetRole::Authority);      // Server has full control
#else
    SetNetRole(ENetRole::SimulatedProxy); // Client receives updates
#endif
```

**Example:**
```cpp
// Create main game world
UWorld gameWorld("MainWorld");

// Create separate worlds for different contexts
UWorld menuWorld("MenuWorld");
UWorld editorWorld("EditorWorld");
```

### Actor Management

#### `template <typename T = AActor> T* SpawnActor(const FTransform& transform = FTransform())`

Spawns a new actor of the specified type.

**Template Parameters:**
- `T`: Actor type to spawn (must inherit from AActor)

**Parameters:**
- `transform`: Initial transform for the actor

**Returns:** Pointer to spawned actor, or nullptr on failure

**Behavior:**
- Creates new instance of T
- Sets actor transform
- Calls BeginPlay() on actor
- Adds to world's actor list

**Example:**
```cpp
// Spawn a player character
APlayer* player = gameWorld.SpawnActor<APlayer>(FTransform(FVector3(0, 0, 0)));

// Spawn an enemy
AEnemy* enemy = gameWorld.SpawnActor<AEnemy>(FTransform(FVector3(100, 50, 0)));

// Spawn with specific rotation
FTransform enemyTransform(FVector3(50, 25, 0), FRotator(0, 90, 0), FVector3(1, 1, 1));
AAIController* ai = gameWorld.SpawnActor<AAIController>(enemyTransform);
```

#### `template <typename T = AActor> T* SpawnActor(UClass* actorClass, const FTransform& transform = FTransform::Identity)`

Spawns an actor from a UClass.

**Template Parameters:**
- `T`: Type to cast result to

**Parameters:**
- `actorClass`: Class to instantiate
- `transform`: Initial transform

**Returns:** Pointer to spawned actor, or nullptr on failure

**Behavior:**
- Validates class inheritance from AActor
- Creates instance using UClass system
- Casts to requested type
- Sets transform and calls BeginPlay()

#### `template <typename T = AActor> T* SpawnActor(const FString& className, const FTransform& transform = FTransform())`

Spawns an actor by class name.

**Template Parameters:**
- `T`: Type to cast result to

**Parameters:**
- `className`: Name of class to spawn
- `transform`: Initial transform

**Returns:** Pointer to spawned actor, or nullptr on failure

**Behavior:**
- Looks up class by name
- Delegates to class-based spawning

**Example:**
```cpp
// Spawn actors by name (useful for data-driven systems)
AActor* player = gameWorld.SpawnActor("APlayer", FTransform(FVector3(0, 0, 0)));
AActor* enemy = gameWorld.SpawnActor("AEnemy", FTransform(FVector3(100, 0, 0)));
AActor* powerup = gameWorld.SpawnActor("APowerUp", FTransform(FVector3(50, 50, 0)));
```

#### `const std::vector<std::shared_ptr<AActor>>& GetActors(void) const`

Gets all actors in the world.

**Returns:** Constant reference to actor vector

#### `template <typename T> std::vector<T*> GetActorsOfClass(void) const`

Gets all actors of a specific type.

**Template Parameters:**
- `T`: Actor type to find

**Returns:** Vector of pointers to actors of type T

**Example:**
```cpp
// Get all enemies
auto enemies = gameWorld.GetActorsOfClass<AEnemy>();
FLogger::Log(ELogLevel::Info, FString::Format("Found {} enemies", enemies.size()));

// Get all players
auto players = gameWorld.GetActorsOfClass<APlayer>();
for (APlayer* player : players) {
    player->ApplyDamage(10.0f); // Damage all players
}
```

#### `std::vector<AActor*> GetActorsByClass(UClass* actorClass, bool includeChildren = true) const`

Gets actors by UClass with inheritance support.

**Parameters:**
- `actorClass`: Class to search for
- `includeChildren`: Whether to include derived classes

**Returns:** Vector of matching actors

**Example:**
```cpp
// Get all controllers (base class)
UClass* controllerClass = AController::StaticClass();
auto controllers = gameWorld.GetActorsByClass(controllerClass, true);

// Get only player controllers (exact class)
UClass* playerControllerClass = APlayerController::StaticClass();
auto playerControllers = gameWorld.GetActorsByClass(playerControllerClass, false);
```

#### `void DestroyActor(AActor* actor)`

Marks an actor for destruction.

**Parameters:**
- `actor`: Actor to destroy

**Behavior:**
- Sets actor's deletion flag
- Actor is removed during next tick
- EndPlay() is called before removal

**Example:**
```cpp
// Destroy a specific enemy
AEnemy* enemy = FindEnemyByID(enemyID);
if (enemy) {
    gameWorld.DestroyActor(enemy);
}

// Destroy all enemies in an area
FVector2 center(100, 100);
float radius = 50.0f;
auto enemies = gameWorld.GetActorsOfClass<AEnemy>();
for (AEnemy* enemy : enemies) {
    if ((enemy->GetPosition() - center).Length() <= radius) {
        gameWorld.DestroyActor(enemy);
    }
}
```

### Game Loop Methods

#### `virtual void BeginPlay(void) override`

Initializes the world and all actors.

**Behavior:**
- Resets world time to 0.0
- Calls BeginPlay() on game mode
- Calls BeginPlay() on all actors

**Called:** Once when world starts

#### `virtual void Tick(Float32 deltaTime) override`

Updates the world and all active systems.

**Parameters:**
- `deltaTime`: Time elapsed since last tick (in seconds)

**Behavior:**
- Updates world time
- Removes destroyed actors
- Ticks game mode
- Ticks all active actors

**Called:** Every frame

**Example:**
```cpp
// Custom world subclass with additional tick logic
class AMyWorld : public UWorld {
public:
    virtual void Tick(Float32 deltaTime) override {
        // Call parent tick first
        UWorld::Tick(deltaTime);

        // Custom logic
        UpdatePhysics(deltaTime);
        UpdateAI(deltaTime);
        CheckWinConditions();
    }

private:
    void UpdatePhysics(Float32 deltaTime) {
        // Custom physics updates
    }

    void UpdateAI(Float32 deltaTime) {
        // AI system updates
    }

    void CheckWinConditions() {
        // Game state checks
    }
};
```

#### `virtual void EndPlay(void) override`

Cleans up the world and all actors.

**Behavior:**
- Calls EndPlay() on game mode
- Calls EndPlay() on all actors

**Called:** When world ends

### Level Management

#### `ULevel* GetCurrentLevel(void)`

Gets the current active level.

**Returns:** Pointer to current level

#### `bool SpawnLevel(const ULevel& level)`

Loads a level into the world.

**Parameters:**
- `level`: Level to load

**Returns:** True on success

**Behavior:**
- Sets current level
- Clears existing actors
- Spawns actors from level entries

**Example:**
```cpp
// Load a level
ULevel tutorialLevel = ULevel::LoadLevelFromFile("Levels/Tutorial.level");
if (gameWorld.SpawnLevel(tutorialLevel)) {
    FLogger::Log(ELogLevel::Info, "Tutorial level loaded");
} else {
    FLogger::Log(ELogLevel::Error, "Failed to load tutorial level");
}
```

#### `bool ChangeLevel(const FString& levelName)`

Switches to a different loaded level.

**Parameters:**
- `levelName`: Name of level to switch to

**Returns:** True if level was found and loaded

**Behavior:**
- Finds level by name in loaded levels
- Calls SpawnLevel() if found

#### `const AGameMode& GetGameMode(void) const`

Gets the current game mode.

**Returns:** Current game mode

#### `const std::vector<ULevel>& GetLoadedLevels(void) const`

Gets all loaded levels.

**Returns:** Vector of loaded levels

### Time Management

#### `float GetWorldTime(void) const`

Gets the current world time.

**Returns:** World time in seconds since BeginPlay()

**Example:**
```cpp
float currentTime = gameWorld.GetWorldTime();

// Check for timed events
if (currentTime > 60.0f && !bossSpawned) {
    SpawnBoss();
    bossSpawned = true;
}

// Calculate delta time manually (normally handled by Tick)
static float lastTime = 0.0f;
float deltaTime = currentTime - lastTime;
lastTime = currentTime;
```

### Rendering

#### `void Render(IRenderer& renderer)`

Renders all actors in the world.

**Parameters:**
- `renderer`: Renderer to use for drawing

**Behavior:**
- Iterates all actors
- Calls Render() on each actor

**Example:**
```cpp
// In main game loop
void GameLoop(UWorld& world, IRenderer& renderer) {
    while (gameRunning) {
        // Update
        float deltaTime = CalculateDeltaTime();
        world.Tick(deltaTime);

        // Render
        renderer.Clear();
        world.Render(renderer);
        renderer.Present();
    }
}
```

## Networking (RPC System)

### RPC Functions

The world provides several RPC functions for network communication:

#### `UFunction<FString, FTransform, UUID, UInt32> SpawnActorRPC`

Spawns an actor across the network.

**Parameters:**
- `className`: Actor class name
- `transform`: Actor transform
- `actorID`: Unique actor ID
- `owningClientID`: Client that owns the actor

**Direction:** Client → Server

#### `UFunction<UUID> DestroyActorRPC`

Destroys an actor across the network.

**Parameters:**
- `actorID`: ID of actor to destroy

**Direction:** Client → Server

#### `UFunction<UInt32> SpawnClientRPC`

Notifies server of client connection.

**Parameters:**
- `owningClientID`: Connecting client ID

**Direction:** Server → Client

#### `UFunction<UInt32, UUID, FTransform> SpawnPlayerRPC`

Spawns a player across the network.

**Parameters:**
- `owningClientID`: Client ID
- `playerID`: Player actor ID
- `transform`: Player spawn transform

**Direction:** Client → Server

#### `UFunction<std::vector<Byte>> SyncSnapshotRPC`

Synchronizes world state.

**Parameters:**
- `snapshotData`: Serialized snapshot data

**Direction:** Client → Server

### RPC Handler Methods

#### `void RPC_SpawnActor(const FString& className, const FTransform& transform, const UUID& actorID, UInt32 owningClientID)`

Handles actor spawning RPC.

**Behavior:**
- Validates class name
- Checks if local client owns actor
- Spawns actor if needed
- Sets ownership

#### `void RPC_DestroyActor(const UUID& actorID)`

Handles actor destruction RPC.

**Behavior:**
- Finds actor by ID
- Destroys actor if found

#### `void RPC_SyncSnapshot(const std::vector<Byte>& snapshotData)`

Handles snapshot synchronization.

**Behavior:**
- Deserializes snapshot
- Validates snapshot ID
- Updates actor states
- Reconciles client state

## Usage Examples

### Basic World Setup

```cpp
class GameApplication {
private:
    UWorld m_gameWorld;
    IRenderer* m_renderer;

public:
    bool Initialize() {
        // Create world
        m_gameWorld = UWorld("GameWorld");

        // Load initial level
        ULevel startLevel = ULevel::LoadLevelFromFile("Levels/Start.level");
        m_gameWorld.SpawnLevel(startLevel);

        // Spawn initial actors
        m_gameWorld.SpawnActor<APlayer>(FTransform(FVector3(0, 0, 0)));
        m_gameWorld.SpawnActor<ACamera>(FTransform(FVector3(0, -100, 50)));

        return true;
    }

    void Run() {
        m_gameWorld.BeginPlay();

        while (!ShouldQuit()) {
            float deltaTime = GetDeltaTime();

            // Update
            m_gameWorld.Tick(deltaTime);

            // Render
            m_renderer->Clear();
            m_gameWorld.Render(*m_renderer);
            m_renderer->Present();
        }

        m_gameWorld.EndPlay();
    }
};
```

### Actor Querying and Management

```cpp
class ActorManager {
public:
    static void UpdateEnemies(UWorld& world) {
        auto enemies = world.GetActorsOfClass<AEnemy>();

        for (AEnemy* enemy : enemies) {
            // Update enemy AI
            enemy->UpdateAI();

            // Check if enemy should be removed
            if (enemy->GetHealth() <= 0) {
                world.DestroyActor(enemy);
            }
        }
    }

    static APlayer* FindClosestPlayer(UWorld& world, const FVector3& position) {
        auto players = world.GetActorsOfClass<APlayer>();

        APlayer* closest = nullptr;
        float minDistance = FLT_MAX;

        for (APlayer* player : players) {
            float distance = (player->GetPosition() - position).Length();
            if (distance < minDistance) {
                minDistance = distance;
                closest = player;
            }
        }

        return closest;
    }

    static void ApplyAreaEffect(UWorld& world, const FVector3& center, float radius, float damage) {
        // Get all damageable actors in area
        auto actors = world.GetActors();
        for (const auto& actor : actors) {
            if (actor) {
                float distance = (actor->GetPosition() - center).Length();
                if (distance <= radius) {
                    // Apply damage if actor is damageable
                    if (auto damageable = dynamic_cast<IDamageable*>(actor.get())) {
                        damageable->TakeDamage(damage);
                    }
                }
            }
        }
    }

    static void DebugActorCounts(UWorld& world) {
        auto allActors = world.GetActors();
        auto players = world.GetActorsOfClass<APlayer>();
        auto enemies = world.GetActorsOfClass<AEnemy>();
        auto projectiles = world.GetActorsOfClass<AProjectile>();

        FLogger::Log(ELogLevel::Debug, FString::Format("Total actors: {}", allActors.size()));
        FLogger::Log(ELogLevel::Debug, FString::Format("Players: {}", players.size()));
        FLogger::Log(ELogLevel::Debug, FString::Format("Enemies: {}", enemies.size()));
        FLogger::Log(ELogLevel::Debug, FString::Format("Projectiles: {}", projectiles.size()));
    }
};
```

### Level Management System

```cpp
class LevelManager {
public:
    static bool LoadLevel(UWorld& world, const FString& levelName) {
        // Check if level is already loaded
        const auto& loadedLevels = world.GetLoadedLevels();
        for (const auto& level : loadedLevels) {
            if (level.GetLevelName() == levelName) {
                return world.SpawnLevel(level);
            }
        }

        // Load level from file
        FilePath levelPath = FString::Format("Levels/{}.level", levelName);
        ULevel level = ULevel::LoadLevelFromFile(levelPath);

        if (level.GetActorEntries().Size() > 0) {
            return world.SpawnLevel(level);
        }

        FLogger::Log(ELogLevel::Error, FString::Format("Failed to load level: {}", levelName));
        return false;
    }

    static bool SaveCurrentLevel(UWorld& world, const FString& levelName) {
        ULevel currentLevel = ULevel::LoadLevelFromWorld(world);
        currentLevel.SetLevelName(levelName);

        FilePath savePath = FString::Format("Levels/{}.level", levelName);
        return currentLevel.SaveToFile(savePath);
    }

    static void TransitionToLevel(UWorld& world, const FString& levelName, float transitionTime = 1.0f) {
        // Start transition
        StartTransition(transitionTime);

        // Load level asynchronously
        std::thread([levelName, transitionTime]() {
            // Simulate loading time
            std::this_thread::sleep_for(std::chrono::milliseconds((int)(transitionTime * 1000)));

            // Load level on main thread
            MainThread::Execute([levelName]() {
                UWorld* world = GetGameWorld();
                if (world) {
                    LoadLevel(*world, levelName);
                }
                EndTransition();
            });
        }).detach();
    }

    static void PreloadLevels(UWorld& world, const TVector<FString>& levelNames) {
        for (const FString& levelName : levelNames) {
            FilePath levelPath = FString::Format("Levels/{}.level", levelName);
            ULevel level = ULevel::LoadLevelFromFile(levelPath);

            if (level.GetActorEntries().Size() > 0) {
                // Add to loaded levels (would need to modify UWorld to expose this)
                // world.AddLoadedLevel(level);
                FLogger::Log(ELogLevel::Info, FString::Format("Preloaded level: {}", levelName));
            }
        }
    }

private:
    static void StartTransition(float duration) {
        // Show loading screen, fade out, etc.
        FLogger::Log(ELogLevel::Info, FString::Format("Starting level transition ({}s)", duration));
    }

    static void EndTransition() {
        // Hide loading screen, fade in, etc.
        FLogger::Log(ELogLevel::Info, "Level transition complete");
    }
};
```

### Networked World Management

```cpp
class NetworkedWorldManager {
public:
    static void HandlePlayerJoin(UWorld& world, UInt32 clientID) {
        // Spawn player for new client
        FTransform spawnTransform = GetPlayerSpawnPoint(clientID);
        UUID playerID = GeneratePlayerID(clientID);

        // Call RPC to spawn player on server
        world.SpawnPlayerRPC(clientID, playerID, spawnTransform);

        // Notify other clients
        BroadcastPlayerJoined(clientID, playerID, spawnTransform);
    }

    static void HandlePlayerLeave(UWorld& world, UInt32 clientID) {
        // Find and destroy player's actors
        auto actors = world.GetActors();
        for (const auto& actor : actors) {
            if (actor && GetActorOwnerID(actor.get()) == clientID) {
                world.DestroyActorRPC(actor->GetUUID());
            }
        }

        // Notify other clients
        BroadcastPlayerLeft(clientID);
    }

    static void SyncWorldState(UWorld& world) {
        // Create snapshot of current world state
        ULevel snapshot = ULevel::LoadLevelFromWorld(world);

        // Serialize snapshot
        std::vector<Byte> snapshotData = snapshot.SerializeLevelData();

        // Send to all clients
        world.SyncSnapshotRPC(snapshotData);
    }

    static void ReconcileClientState(UWorld& world, const std::vector<Byte>& serverSnapshot) {
        // Deserialize server snapshot
        Packets::Snapshot snapshot;
        FBinaryReader reader(serverSnapshot.data(), serverSnapshot.size());

        if (snapshot.Deserialize(reader)) {
            // Apply snapshot to local world
            world.RPC_SyncSnapshot(serverSnapshot);
        }
    }

private:
    static FTransform GetPlayerSpawnPoint(UInt32 clientID) {
        // Calculate spawn point based on client ID or game rules
        int spawnIndex = clientID % 4; // 4 spawn points
        FVector3 spawnPositions[4] = {
            FVector3(-100, -100, 0),
            FVector3(100, -100, 0),
            FVector3(-100, 100, 0),
            FVector3(100, 100, 0)
        };
        return FTransform(spawnPositions[spawnIndex]);
    }

    static UUID GeneratePlayerID(UInt32 clientID) {
        // Generate unique player ID
        return UUID::Generate();
    }

    static UInt32 GetActorOwnerID(AActor* actor) {
        // Get the client ID that owns this actor
        // This would be stored as a property or network variable
        return 0; // Placeholder
    }

    static void BroadcastPlayerJoined(UInt32 clientID, const UUID& playerID, const FTransform& transform) {
        // Send notification to all connected clients
        FLogger::Log(ELogLevel::Info, FString::Format("Player {} joined", clientID));
    }

    static void BroadcastPlayerLeft(UInt32 clientID) {
        // Send notification to all connected clients
        FLogger::Log(ELogLevel::Info, FString::Format("Player {} left", clientID));
    }
};
```

### World Partitioning System

```cpp
class WorldPartitioner {
public:
    struct Partition {
        FVector2 center;
        float size;
        TVector<AActor*> actors;
        bool isActive;
    };

    static void PartitionWorld(UWorld& world, float partitionSize) {
        // Clear existing partitions
        partitions.clear();

        // Calculate world bounds
        FVector2 minBounds = FVector2(FLT_MAX, FLT_MAX);
        FVector2 maxBounds = FVector2(-FLT_MAX, -FLT_MAX);

        auto actors = world.GetActors();
        for (const auto& actor : actors) {
            if (actor) {
                FVector3 pos = actor->GetPosition();
                minBounds.x = std::min(minBounds.x, pos.x);
                minBounds.y = std::min(minBounds.y, pos.y);
                maxBounds.x = std::max(maxBounds.x, pos.x);
                maxBounds.y = std::max(maxBounds.y, pos.y);
            }
        }

        // Create partitions
        int partitionsX = std::ceil((maxBounds.x - minBounds.x) / partitionSize);
        int partitionsY = std::ceil((maxBounds.y - minBounds.y) / partitionSize);

        for (int x = 0; x < partitionsX; ++x) {
            for (int y = 0; y < partitionsY; ++y) {
                Partition partition;
                partition.center = FVector2(
                    minBounds.x + (x + 0.5f) * partitionSize,
                    minBounds.y + (y + 0.5f) * partitionSize
                );
                partition.size = partitionSize;
                partition.isActive = false;
                partitions.push_back(partition);
            }
        }

        // Assign actors to partitions
        for (const auto& actor : actors) {
            if (actor) {
                Partition* partition = GetPartitionForPosition(actor->GetPosition());
                if (partition) {
                    partition->actors.PushBack(actor.get());
                }
            }
        }
    }

    static TVector<AActor*> GetActorsInRadius(const FVector3& center, float radius) {
        TVector<AActor*> result;

        // Find partitions that intersect with the radius
        for (Partition& partition : partitions) {
            float distanceToCenter = (FVector2(center.x, center.y) - partition.center).Length();
            if (distanceToCenter <= radius + partition.size * 0.707f) { // sqrt(2)/2 approximation
                // Check individual actors
                for (AActor* actor : partition.actors) {
                    if (actor) {
                        float actorDistance = (actor->GetPosition() - center).Length();
                        if (actorDistance <= radius) {
                            result.PushBack(actor);
                        }
                    }
                }
            }
        }

        return result;
    }

    static void UpdateActivePartitions(const FVector3& cameraPosition, float viewDistance) {
        for (Partition& partition : partitions) {
            float distance = (FVector2(cameraPosition.x, cameraPosition.y) - partition.center).Length();
            partition.isActive = (distance <= viewDistance);

            // Activate/deactivate actors based on partition
            for (AActor* actor : partition.actors) {
                if (actor) {
                    actor->SetActive(partition.isActive);
                }
            }
        }
    }

private:
    static TVector<Partition> partitions;

    static Partition* GetPartitionForPosition(const FVector3& position) {
        for (Partition& partition : partitions) {
            FVector2 pos2D(position.x, position.y);
            FVector2 diff = pos2D - partition.center;
            if (std::abs(diff.x) <= partition.size * 0.5f &&
                std::abs(diff.y) <= partition.size * 0.5f) {
                return &partition;
            }
        }
        return nullptr;
    }
};
```

## Architecture Diagrams

### World Initialization Flow

```
Constructor ──► Setup RPCs ──► Set Network Role ──► Load Levels ──► Spawn Initial Level
      │              │              │                    │                │
      ▼              ▼              ▼                    ▼                ▼
Create Actor   Bind Handlers   Authority/Proxy     Find Level      Create Actors
Container      (Spawn/Destroy)  Configuration      Classes         From Entries
Set Time=0.0   Setup Events     Auto-Detect       Create Instances  Call BeginPlay
```

### Actor Lifecycle Flow

```
SpawnActor() ──► Create Instance ──► Set Transform ──► Add to World ──► BeginPlay()
      │                │                   │               │              │
      ▼                ▼                   ▼               ▼              ▼
Validate Type   Allocate Memory     Apply Position    Store Shared     Initialize
Check Template  Call Constructor    Rotation/Scale    Pointer          Components
```

### Network Synchronization Flow

```
Server ──► Create Snapshot ──► Serialize Actors ──► Send RPC ──► Client Receives
   │              │                   │               │              │
   ▼              ▼                   ▼               ▼              ▼
Gather State  Extract Positions   Binary Format    SyncSnapshot   Deserialize
Actor Props   Transform Data      Compress Data    RPC Call       Apply Changes
Game Mode     Property Values     Include Timestamps             Reconcile State
```

### Game Loop Flow

```
BeginPlay() ──► Main Loop ──► Tick() ──► Update ──► Render ──► Check Exit
      │             │           │          │           │           │
      ▼             ▼           │          ▼           ▼           ▼
Initialize      Get Delta       │      World Time   Clear Screen  User Input
Systems         Time            │      +Delta       Draw Actors   Window Close
Spawn Actors                    │      Remove       Present       Save State
Setup Game                      │      Marked       Frame
Mode                            │      Actors
                                │      Tick Game
                                │      Mode
                                │      Tick Active
                                │      Actors
```

## Performance Considerations

### Memory Usage

- **Actor Storage**: Shared pointers for all actors
- **Level Data**: Current level and loaded levels
- **RPC Overhead**: Function binding and event systems
- **Snapshot Buffers**: Network synchronization data

### CPU Performance

- **Tick Frequency**: Called every frame (60-120 FPS)
- **Actor Iteration**: Linear search for queries
- **Network Updates**: Serialization/deserialization
- **Memory Management**: Shared pointer reference counting

### Optimization Strategies

1. **Spatial Partitioning**: Divide world into regions for efficient queries
2. **Actor Pooling**: Reuse destroyed actors instead of allocating new ones
3. **Lazy Updates**: Only update actors in active regions
4. **Network Batching**: Group multiple RPC calls
5. **Component Systems**: Cache frequently accessed components

## Troubleshooting

### Actor Spawning Failures

**Symptoms:** SpawnActor returns nullptr

**Possible Causes:**
- Invalid actor class
- Template type not derived from AActor
- Memory allocation failure
- Class registration issues

**Solutions:**
```cpp
template <typename T>
T* SafeSpawnActor(UWorld& world, const FTransform& transform, const FString& context) {
    FLogger::Log(ELogLevel::Debug,
        FString::Format("Attempting to spawn {} in {}", typeid(T).name(), context));

    T* actor = world.SpawnActor<T>(transform);

    if (!actor) {
        FLogger::Log(ELogLevel::Error,
            FString::Format("Failed to spawn {} in {}", typeid(T).name(), context));

        // Check if class is registered
        UClass* actorClass = T::StaticClass();
        if (!actorClass) {
            FLogger::Log(ELogLevel::Error, "Actor class not registered");
        }

        // Check inheritance
        UClass* baseClass = AActor::StaticClass();
        if (baseClass && !actorClass->IsChildOf(baseClass)) {
            FLogger::Log(ELogLevel::Error, "Actor class doesn't inherit from AActor");
        }

        return nullptr;
    }

    FLogger::Log(ELogLevel::Info,
        FString::Format("Successfully spawned {} in {}", typeid(T).name(), context));
    return actor;
}
```

### Level Loading Issues

**Symptoms:** SpawnLevel returns false or crashes

**Possible Causes:**
- Invalid level data
- Missing actor classes
- Corrupted level file
- Circular dependencies

**Solutions:**
```cpp
bool SafeLoadLevel(UWorld& world, const FString& levelName) {
    FLogger::Log(ELogLevel::Info, FString::Format("Loading level: {}", levelName));

    try {
        // Load level
        FilePath levelPath = FString::Format("Levels/{}.level", levelName);
        ULevel level = ULevel::LoadLevelFromFile(levelPath);

        if (level.GetActorEntries().Size() == 0) {
            FLogger::Log(ELogLevel::Error, "Level contains no actors");
            return false;
        }

        // Validate actor classes exist
        bool hasMissingClasses = false;
        for (const auto& entry : level.GetActorEntries()) {
            UClass* actorClass = UClass::FindClass(entry.class_name);
            if (!actorClass) {
                FLogger::Log(ELogLevel::Warning,
                    FString::Format("Missing actor class: {}", entry.class_name));
                hasMissingClasses = true;
            }
        }

        if (hasMissingClasses) {
            FLogger::Log(ELogLevel::Warning, "Level contains missing actor classes");
        }

        // Attempt to spawn level
        if (world.SpawnLevel(level)) {
            FLogger::Log(ELogLevel::Info,
                FString::Format("Successfully loaded level: {} with {} actors",
                    levelName, level.GetActorEntries().Size()));
            return true;
        } else {
            FLogger::Log(ELogLevel::Error, "Failed to spawn level");
            return false;
        }

    } catch (const std::exception& e) {
        FLogger::Log(ELogLevel::Error,
            FString::Format("Exception loading level {}: {}", levelName, e.what()));
        return false;
    }
}
```

### Network Synchronization Problems

**Symptoms:** Client and server states desync

**Possible Causes:**
- Packet loss
- High latency
- Incorrect snapshot application
- Race conditions

**Solutions:**
```cpp
class NetworkDebugger {
public:
    static void LogSnapshotDifferences(const UWorld& localWorld, const UWorld& remoteWorld) {
        auto localActors = localWorld.GetActors();
        auto remoteActors = remoteWorld.GetActors();

        FLogger::Log(ELogLevel::Debug,
            FString::Format("Comparing worlds: Local={}, Remote={}",
                localActors.size(), remoteActors.size()));

        // Compare actor counts
        if (localActors.size() != remoteActors.size()) {
            FLogger::Log(ELogLevel::Warning, "Actor count mismatch");
        }

        // Compare actor states (simplified)
        for (size_t i = 0; i < std::min(localActors.size(), remoteActors.size()); ++i) {
            const auto& localActor = localActors[i];
            const auto& remoteActor = remoteActors[i];

            if (localActor && remoteActor) {
                FVector3 localPos = localActor->GetPosition();
                FVector3 remotePos = remoteActor->GetPosition();

                float distance = (localPos - remotePos).Length();
                if (distance > 1.0f) { // 1 unit tolerance
                    FLogger::Log(ELogLevel::Warning,
                        FString::Format("Actor {} position desync: Distance={}", i, distance));
                }
            }
        }
    }

    static void ValidateSnapshot(const std::vector<Byte>& snapshotData) {
        if (snapshotData.empty()) {
            FLogger::Log(ELogLevel::Error, "Empty snapshot data");
            return;
        }

        // Attempt to deserialize
        Packets::Snapshot snapshot;
        FBinaryReader reader(snapshotData.data(), snapshotData.size());

        if (!snapshot.Deserialize(reader)) {
            FLogger::Log(ELogLevel::Error, "Failed to deserialize snapshot");
            return;
        }

        FLogger::Log(ELogLevel::Debug,
            FString::Format("Valid snapshot: ID={}, Actors={}",
                snapshot.snapshotID, snapshot.actors.size()));
    }

    static void MonitorNetworkStats(UWorld& world) {
        static int frameCount = 0;
        static float lastTime = world.GetWorldTime();

        frameCount++;
        float currentTime = world.GetWorldTime();

        if (currentTime - lastTime >= 1.0f) { // Log every second
            FLogger::Log(ELogLevel::Debug,
                FString::Format("Network Stats: {} FPS, {} actors",
                    frameCount, world.GetActors().size()));
            frameCount = 0;
            lastTime = currentTime;
        }
    }
};
```

## Future Enhancements

### Planned Features

1. **World Partitioning**: Automatic spatial partitioning for performance
2. **Level Streaming**: Load/unload level sections dynamically
3. **Save System**: Automatic world state saving/loading
4. **Replay System**: Record and playback world state
5. **Multi-World**: Support for multiple simultaneous worlds
6. **World Queries**: Advanced spatial and semantic queries
7. **Physics Integration**: Built-in physics world management
8. **Event System**: World-level event broadcasting

### Extension Points

The world system allows for easy extension:

```cpp
class AdvancedWorld : public UWorld {
public:
    // Add world bounds
    void SetWorldBounds(const FBox& bounds) { m_worldBounds = bounds; }
    const FBox& GetWorldBounds() const { return m_worldBounds; }

    // Add weather system
    void SetWeather(EWeatherType weather) { m_currentWeather = weather; }
    EWeatherType GetWeather() const { return m_currentWeather; }

    // Add time of day
    void SetTimeOfDay(float time) { m_timeOfDay = time; }
    float GetTimeOfDay() const { return m_timeOfDay; }

    virtual void Tick(Float32 deltaTime) override {
        UWorld::Tick(deltaTime);

        // Update weather
        UpdateWeather(deltaTime);

        // Update time of day
        m_timeOfDay += deltaTime * m_timeScale;
        if (m_timeOfDay >= 24.0f) m_timeOfDay -= 24.0f;
    }

private:
    FBox m_worldBounds;
    EWeatherType m_currentWeather;
    float m_timeOfDay;
    float m_timeScale;

    void UpdateWeather(Float32 deltaTime) {
        // Weather simulation logic
    }
};

// World profiler
class WorldProfiler {
public:
    struct ProfileData {
        int actorCount;
        int activeActors;
        float averageTickTime;
        size_t memoryUsage;
        int networkMessages;
    };

    static ProfileData ProfileWorld(const UWorld& world) {
        ProfileData data;

        auto actors = world.GetActors();
        data.actorCount = actors.size();
        data.activeActors = std::count_if(actors.begin(), actors.end(),
            [](const auto& actor) { return actor && actor->IsActive(); });

        // Measure tick performance
        auto start = std::chrono::high_resolution_clock::now();
        // Simulate tick measurement
        auto end = std::chrono::high_resolution_clock::now();
        data.averageTickTime = std::chrono::duration<float>(end - start).count();

        // Estimate memory usage
        data.memoryUsage = actors.size() * sizeof(std::shared_ptr<AActor>) +
                          data.actorCount * 1000; // Rough estimate

        return data;
    }

    static void LogProfileData(const ProfileData& data) {
        FLogger::Log(ELogLevel::Info, "=== World Profile ===");
        FLogger::Log(ELogLevel::Info, FString::Format("Actors: {} total, {} active", data.actorCount, data.activeActors));
        FLogger::Log(ELogLevel::Info, FString::Format("Performance: {:.3f}ms average tick", data.averageTickTime * 1000));
        FLogger::Log(ELogLevel::Info, FString::Format("Memory: {} KB estimated", data.memoryUsage / 1024));
    }
};
```

---

*This documentation covers the complete UWorld implementation as of TKD Engine v1.0.0. For the latest updates and additional examples, refer to the engine's source code and test suites.*
