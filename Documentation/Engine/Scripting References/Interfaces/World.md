# World Interface Documentation

## Overview

The `FWorldInterface` provides thread-safe, high-level access to the game world subsystem. It serves as the primary interface for actor management, world queries, time management, and safe world manipulation in a multi-threaded environment.

### Key Features

- **Thread-Safe Operations**: All methods are protected by mutexes for safe cross-thread access
- **Type-Safe Actor Spawning**: Template-based spawning with compile-time type checking
- **Flexible Actor Queries**: Multiple ways to find and filter actors in the world
- **Time Management**: Access to world time, simulation time, and performance metrics
- **Exclusive World Access**: `WithWorld()` method for complex operations requiring direct world access

### Architecture

```cpp
class FWorldInterface final
{
private:
    static std::mutex s_mutex;
    static __internal::FWorldSubsystem* GetWorldSubsystem(void);

public:
    // Actor spawning methods
    template <typename T = AActor>
    static T* SpawnActor(const FTransform& transform = FTransform());

    // Actor query methods
    static std::vector<std::shared_ptr<AActor>> GetActors(void);
    template <typename T> static std::vector<T*> GetActorsOfClass(void);

    // Time management
    static float GetTime(void);
    static double GetSimulationTime(void);

    // World access
    template <typename Func>
    static auto WithWorld(Func&& func) -> decltype(func(std::declval<UWorld&>()));
};
```

## Actor Spawning

### Template-Based Spawning

#### `template <typename T = AActor> static T* SpawnActor(const FTransform& transform = FTransform())`

Spawns an actor of the specified type with default initialization.

**Template Parameters:**
- `T`: The actor type to spawn (must inherit from `AActor`)

**Parameters:**
- `transform`: Initial transform for the actor (default: identity transform)

**Returns:** Pointer to the spawned actor, or `nullptr` if spawning failed

**Type Requirements:**
- `T` must be derived from `AActor`
- `T` must have a default constructor or be registered with the reflection system

**Example:**
```cpp
// Spawn a player character at origin
APlayerCharacter* player = World::SpawnActor<APlayerCharacter>();

// Spawn an enemy at specific location
AEnemy* enemy = World::SpawnActor<AEnemy>(
    FTransform(FVector3(100.0f, 0.0f, 0.0f))
);

// Spawn with rotation
FTransform transform;
transform.SetPosition(FVector3(0, 0, 50));
transform.SetRotation(FQuat(FVector3::Up, 45.0f * DEG_TO_RAD));
AActor* rotatedActor = World::SpawnActor<AActor>(transform);
```

#### `template <typename T = AActor> static T* SpawnActor(UClass* actorClass, const FTransform& transform = FTransform::Identity)`

Spawns an actor from a `UClass` instance.

**Template Parameters:**
- `T`: The type to cast the result to (must inherit from `AActor`)

**Parameters:**
- `actorClass`: Pointer to the `UClass` to instantiate
- `transform`: Initial transform for the actor

**Returns:** Pointer to the spawned actor cast to type `T`, or `nullptr` if spawning failed

**Requirements:**
- `actorClass` must not be `nullptr`
- `actorClass` must represent a type derived from `AActor`
- `T` must be compatible with the actual actor type

**Example:**
```cpp
// Find enemy class
UClass* enemyClass = UClass::FindClass("AEnemy");
if (enemyClass) {
    // Spawn enemy using class
    AEnemy* enemy = World::SpawnActor<AEnemy>(enemyClass,
        FTransform(FVector3(50, 0, 0)));
}

// Spawn from blueprint class
UClass* blueprintClass = UClass::FindClass("BP_MyCharacter");
if (blueprintClass) {
    AMyCharacter* character = World::SpawnActor<AMyCharacter>(blueprintClass);
}
```

#### `template <typename T = AActor> static T* SpawnActor(const FString& className, const FTransform& transform = FTransform())`

Spawns an actor by class name string.

**Template Parameters:**
- `T`: The type to cast the result to (must inherit from `AActor`)

**Parameters:**
- `className`: Name of the class to spawn (e.g., "APlayerCharacter")
- `transform`: Initial transform for the actor

**Returns:** Pointer to the spawned actor cast to type `T`, or `nullptr` if spawning failed

**Behavior:**
- Searches the class registry for a class with the given name
- Instantiates the class if found
- Returns `nullptr` if class not found or instantiation fails

**Example:**
```cpp
// Spawn by string name
APlayerController* controller = World::SpawnActor<APlayerController>(
    "APlayerController"
);

// Spawn with configuration
FTransform spawnTransform;
spawnTransform.SetPosition(FVector3(0, 0, 100));
spawnTransform.SetScale(FVector3(2.0f, 2.0f, 2.0f));

ABossEnemy* boss = World::SpawnActor<ABossEnemy>(
    "ABossEnemy", spawnTransform
);
```

## Actor Queries

### Getting All Actors

#### `static std::vector<std::shared_ptr<AActor>> GetActors(void)`

Retrieves all actors currently in the world.

**Returns:** Vector of shared pointers to all actors

**Performance:** O(n) where n is the number of actors in the world

**Example:**
```cpp
// Get all actors
auto allActors = World::GetActors();

// Count actors
std::cout << "Total actors: " << allActors.size() << std::endl;

// Find specific actor types
for (const auto& actor : allActors) {
    if (actor->IsA<APlayerCharacter>()) {
        // Found a player
        APlayerCharacter* player = static_cast<APlayerCharacter*>(actor.get());
        player->ApplyDamage(10.0f);
    }
}
```

### Type-Specific Queries

#### `template <typename T> static std::vector<T*> GetActorsOfClass(void)`

Retrieves all actors of a specific type.

**Template Parameters:**
- `T`: The actor type to find (must inherit from `AActor`)

**Returns:** Vector of pointers to actors of type `T`

**Performance:** O(n) where n is the number of actors in the world

**Example:**
```cpp
// Get all enemies
auto enemies = World::GetActorsOfClass<AEnemy>();
std::cout << "Enemy count: " << enemies.size() << std::endl;

// Process all enemies
for (AEnemy* enemy : enemies) {
    enemy->SetAggressive(true);
    enemy->MoveTowardsPlayer();
}

// Get all projectiles
auto projectiles = World::GetActorsOfClass<AProjectile>();
for (AProjectile* proj : projectiles) {
    if (proj->GetLifetime() > 5.0f) {
        World::DestroyActor(proj);
    }
}
```

#### `static std::vector<AActor*> GetActorsByClass(UClass* actorClass, bool includeChildren = true)`

Retrieves actors by class type with inheritance control.

**Parameters:**
- `actorClass`: The class to filter by
- `includeChildren`: Whether to include derived classes (default: `true`)

**Returns:** Vector of pointers to matching actors

**Example:**
```cpp
// Get all vehicles (base class)
UClass* vehicleClass = UClass::FindClass("AVehicle");
auto vehicles = World::GetActorsByClass(vehicleClass, true);

// Get only cars, not other vehicle types
UClass* carClass = UClass::FindClass("ACar");
auto cars = World::GetActorsByClass(carClass, false);
```

## Actor Destruction

#### `static void DestroyActor(AActor* actor)`

Destroys an actor and removes it from the world.

**Parameters:**
- `actor`: Pointer to the actor to destroy

**Behavior:**
- Calls `actor->EndPlay()` to allow cleanup
- Removes the actor from the world's actor list
- Deallocates the actor's memory
- Invalidates any existing pointers to the actor

**Thread Safety:** Safe to call from any thread

**Example:**
```cpp
// Destroy specific actor
AEnemy* enemy = GetNearestEnemy();
if (enemy) {
    World::DestroyActor(enemy);
}

// Destroy all actors of a type
auto projectiles = World::GetActorsOfClass<AProjectile>();
for (AProjectile* proj : projectiles) {
    if (proj->ShouldDestroy()) {
        World::DestroyActor(proj);
    }
}

// Safe destruction with null check
AActor* actor = FindActorByID(actorID);
if (actor && !actor->IsPendingDestroy()) {
    World::DestroyActor(actor);
}
```

## Time Management

### World Time

#### `static float GetTime(void)`

Gets the current world time in seconds since engine startup.

**Returns:** World time as a float

**Precision:** Millisecond precision

**Example:**
```cpp
// Get current time
float currentTime = World::GetTime();

// Calculate delta time
static float lastTime = 0.0f;
float deltaTime = currentTime - lastTime;
lastTime = currentTime;

// Use for timing
if (currentTime > nextSpawnTime) {
    SpawnEnemy();
    nextSpawnTime = currentTime + 2.0f;
}
```

#### `static double GetSimulationTime(void)`

Gets the high-precision simulation time.

**Returns:** Simulation time as a double

**Precision:** Microsecond or higher precision

**Use Cases:**
- Physics calculations requiring high precision
- Network synchronization
- Profiling and performance measurement

**Example:**
```cpp
// High-precision timing for physics
double simTime = World::GetSimulationTime();

// Calculate precise delta time
static double lastSimTime = 0.0;
double simDeltaTime = simTime - lastSimTime;
lastSimTime = simTime;

// Physics integration
velocity += acceleration * simDeltaTime;
position += velocity * simDeltaTime;
```

### Performance Metrics

#### `static float GetAverageTickTime(void)`

Gets the average time spent per world tick in milliseconds.

**Returns:** Average tick time in milliseconds

**Use Cases:**
- Performance monitoring
- Debugging frame rate issues
- Optimization analysis

**Example:**
```cpp
// Monitor performance
float avgTickTime = World::GetAverageTickTime();
if (avgTickTime > 16.67f) { // Slower than 60 FPS
    std::cout << "Warning: Tick time too high: " << avgTickTime << "ms" << std::endl;
    // Enable performance optimizations
    EnableLowQualityMode();
}
```

#### `static void SetTargetTickRate(float tickRate)`

Sets the target tick rate for the world simulation.

**Parameters:**
- `tickRate`: Target ticks per second (0 for variable rate)

**Behavior:**
- `tickRate > 0`: Fixed time step simulation
- `tickRate = 0`: Variable time step (frame-based)

**Example:**
```cpp
// Set to 60 FPS fixed timestep
World::SetTargetTickRate(60.0f);

// Variable timestep for certain game modes
World::SetTargetTickRate(0.0f);

// Very high precision for physics-heavy games
World::SetTargetTickRate(120.0f);
```

## Exclusive World Access

#### `template <typename Func> static auto WithWorld(Func&& func)`

Executes a function with exclusive access to the world object.

**Template Parameters:**
- `Func`: Function type that takes `UWorld&` as parameter

**Parameters:**
- `func`: Function to execute with world access

**Returns:** The return value of the function

**Thread Safety:** Provides exclusive access to the world, blocking other threads

**Use Cases:**
- Complex world operations requiring multiple steps
- Direct manipulation of world state
- Operations that need to be atomic

**Example:**
```cpp
// Complex world operation
World::WithWorld([](UWorld& world) {
    // Multiple operations in one atomic block
    auto* player = world.SpawnActor<APlayerCharacter>();
    player->SetPosition(FVector3(0, 0, 100));

    // Modify world settings
    world.SetGravity(FVector3(0, 0, -9.81f));
    world.SetTimeDilation(0.5f); // Slow motion

    // Batch spawn enemies
    for (int i = 0; i < 10; ++i) {
        auto* enemy = world.SpawnActor<AEnemy>();
        enemy->SetPosition(FVector3(i * 10, 0, 0));
    }

    return player; // Can return values
});

// Advanced world queries
auto result = World::WithWorld([](UWorld& world) -> std::vector<AActor*> {
    std::vector<AActor*> foundActors;

    // Direct world traversal
    for (auto& actor : world.GetActors()) {
        if (actor->GetPosition().z > 100.0f) {
            foundActors.push_back(actor.get());
        }
    }

    return foundActors;
});
```

## Usage Patterns

### Game Initialization

```cpp
class AGameMode : public AActor {
public:
    virtual void BeginPlay() override {
        // Spawn initial actors
        SpawnInitialActors();
    }

private:
    void SpawnInitialActors() {
        // Spawn player
        APlayerCharacter* player = World::SpawnActor<APlayerCharacter>(
            FTransform(FVector3(0, 0, 100))
        );

        // Spawn camera
        ACameraActor* camera = World::SpawnActor<ACameraActor>(
            FTransform(FVector3(0, -500, 200))
        );

        // Spawn initial enemies
        for (int i = 0; i < 5; ++i) {
            AEnemy* enemy = World::SpawnActor<AEnemy>(
                FTransform(FVector3(i * 100 - 200, 0, 0))
            );
        }
    }
};
```

### Actor Management System

```cpp
class AActorManager : public AActor {
private:
    std::unordered_map<FString, std::vector<AActor*>> m_actorGroups;

public:
    void RegisterActor(AActor* actor, const FString& groupName) {
        m_actorGroups[groupName].push_back(actor);
    }

    void UpdateGroup(const FString& groupName) {
        auto it = m_actorGroups.find(groupName);
        if (it != m_actorGroups.end()) {
            // Remove destroyed actors
            it->second.erase(
                std::remove_if(it->second.begin(), it->second.end(),
                    [](AActor* actor) { return actor->IsPendingDestroy(); }),
                it->second.end()
            );

            // Update remaining actors
            for (AActor* actor : it->second) {
                actor->Update(0.016f); // 60 FPS
            }
        }
    }

    void DestroyGroup(const FString& groupName) {
        auto it = m_actorGroups.find(groupName);
        if (it != m_actorGroups.end()) {
            for (AActor* actor : it->second) {
                if (!actor->IsPendingDestroy()) {
                    World::DestroyActor(actor);
                }
            }
            it->second.clear();
        }
    }
};
```

### Time-Based Systems

```cpp
class ATimeManager : public AActor {
private:
    float m_gameTimeScale = 1.0f;
    float m_lastTime = 0.0f;

public:
    void SetTimeScale(float scale) {
        m_gameTimeScale = scale;
        // Apply to world
        World::WithWorld([scale](UWorld& world) {
            world.SetTimeDilation(scale);
        });
    }

    void Update() {
        float currentTime = World::GetTime();
        float deltaTime = (currentTime - m_lastTime) * m_gameTimeScale;
        m_lastTime = currentTime;

        // Update time-based systems
        UpdateTimers(deltaTime);
        UpdateAnimations(deltaTime);
        UpdatePhysics(deltaTime);
    }

    void Pause() {
        SetTimeScale(0.0f);
    }

    void Resume() {
        SetTimeScale(1.0f);
    }

    void SlowMotion(float duration) {
        SetTimeScale(0.3f);
        // Schedule resume after duration
        ScheduleResume(duration);
    }
};
```

### Spatial Partitioning

```cpp
class ASpatialManager : public AActor {
private:
    struct Cell {
        std::vector<AActor*> actors;
        FVector3 center;
        float size;
    };

    std::vector<Cell> m_grid;
    float m_cellSize = 100.0f;
    int m_gridSize = 10;

public:
    void InitializeGrid() {
        m_grid.clear();
        m_grid.reserve(m_gridSize * m_gridSize);

        for (int x = 0; x < m_gridSize; ++x) {
            for (int y = 0; y < m_gridSize; ++y) {
                Cell cell;
                cell.center = FVector3(
                    (x - m_gridSize/2) * m_cellSize,
                    (y - m_gridSize/2) * m_cellSize,
                    0
                );
                cell.size = m_cellSize;
                m_grid.push_back(cell);
            }
        }
    }

    void UpdateSpatialIndex() {
        // Clear all cells
        for (auto& cell : m_grid) {
            cell.actors.clear();
        }

        // Rebuild from all actors
        auto allActors = World::GetActors();
        for (const auto& actor : allActors) {
            AddToGrid(actor.get());
        }
    }

    std::vector<AActor*> GetNearbyActors(const FVector3& position, float radius) {
        std::vector<AActor*> nearby;

        // Find cells within radius
        int minX = std::floor((position.x - radius) / m_cellSize) + m_gridSize/2;
        int maxX = std::ceil((position.x + radius) / m_cellSize) + m_gridSize/2;
        int minY = std::floor((position.y - radius) / m_cellSize) + m_gridSize/2;
        int maxY = std::ceil((position.y + radius) / m_cellSize) + m_gridSize/2;

        // Clamp to grid bounds
        minX = std::max(0, minX);
        maxX = std::min(m_gridSize - 1, maxX);
        minY = std::max(0, minY);
        maxY = std::min(m_gridSize - 1, maxY);

        // Collect actors from relevant cells
        for (int x = minX; x <= maxX; ++x) {
            for (int y = minY; y <= maxY; ++y) {
                int index = y * m_gridSize + x;
                const auto& cell = m_grid[index];

                for (AActor* actor : cell.actors) {
                    float distance = (actor->GetPosition() - position).Length();
                    if (distance <= radius) {
                        nearby.push_back(actor);
                    }
                }
            }
        }

        return nearby;
    }

private:
    void AddToGrid(AActor* actor) {
        FVector3 pos = actor->GetPosition();

        int x = std::floor(pos.x / m_cellSize) + m_gridSize/2;
        int y = std::floor(pos.y / m_cellSize) + m_gridSize/2;

        // Clamp to bounds
        x = std::clamp(x, 0, m_gridSize - 1);
        y = std::clamp(y, 0, m_gridSize - 1);

        int index = y * m_gridSize + x;
        m_grid[index].actors.push_back(actor);
    }
};
```

## Performance Considerations

### Spawning Optimization

- **Batch Spawning**: Spawn multiple actors in a single `WithWorld()` call
- **Pool Actors**: Reuse destroyed actors instead of spawning new ones
- **Deferred Spawning**: Queue spawns for execution during safe times

```cpp
// Efficient batch spawning
World::WithWorld([](UWorld& world) {
    for (int i = 0; i < 100; ++i) {
        // Spawn enemies in batch
        auto* enemy = world.SpawnActor<AEnemy>();
        enemy->SetPosition(FVector3(i * 10, 0, 0));
    }
    // Single mutex unlock at end
});
```

### Query Optimization

- **Cache Results**: Store query results when possible
- **Spatial Queries**: Use spatial partitioning for proximity queries
- **Type Filtering**: Use specific type queries instead of filtering all actors

```cpp
// Cache expensive queries
class ActorCache {
private:
    std::vector<AEnemy*> m_enemies;
    float m_lastUpdate = 0.0f;

public:
    const std::vector<AEnemy*>& GetEnemies() {
        float currentTime = World::GetTime();
        if (currentTime - m_lastUpdate > 0.1f) { // Update every 100ms
            m_enemies = World::GetActorsOfClass<AEnemy>();
            m_lastUpdate = currentTime;
        }
        return m_enemies;
    }
};
```

### Memory Management

- **Smart Pointers**: Actors are managed by shared pointers internally
- **Reference Counting**: Automatic cleanup when no references remain
- **Pool Allocation**: Consider object pooling for frequently spawned types

## Threading Considerations

### Thread Safety

All `FWorldInterface` methods are thread-safe and can be called from any thread:

```cpp
// Safe cross-thread operations
std::thread worker([]() {
    // Spawn actor from worker thread
    ABackgroundActor* actor = World::SpawnActor<ABackgroundActor>();

    // Query from worker thread
    auto enemies = World::GetActorsOfClass<AEnemy>();
});

// Main thread continues...
```

### Exclusive Access Patterns

Use `WithWorld()` for operations requiring consistency:

```cpp
// Atomic multi-step operation
World::WithWorld([](UWorld& world) {
    // These operations happen atomically
    auto* player = world.SpawnActor<APlayerCharacter>();
    auto* camera = world.SpawnActor<ACameraActor>();

    // Set up relationships
    camera->AttachTo(player);
    camera->SetRelativeTransform(FTransform(FVector3(0, -100, 50)));
});
```

### Performance Impact

- **Lock Contention**: Minimize time spent in `WithWorld()` blocks
- **Read Operations**: Prefer query methods over `WithWorld()` for reads
- **Batch Operations**: Group related operations to reduce lock/unlock cycles

## Error Handling

### Spawn Failures

```cpp
AActor* SpawnWithErrorHandling(const FString& className, const FTransform& transform) {
    try {
        AActor* actor = World::SpawnActor<AActor>(className, transform);
        if (!actor) {
            FLogger::Log(ELogLevel::Error, "Failed to spawn actor: " + className);
            return nullptr;
        }

        // Verify actor is valid
        if (!actor->IsValid()) {
            FLogger::Log(ELogLevel::Error, "Spawned actor is invalid: " + className);
            World::DestroyActor(actor);
            return nullptr;
        }

        return actor;
    } catch (const std::exception& e) {
        FLogger::Log(ELogLevel::Error, "Exception during spawn: " + std::string(e.what()));
        return nullptr;
    }
}
```

### Null Pointer Safety

```cpp
void SafeActorOperations() {
    // Safe destruction
    AActor* actor = FindActor();
    if (actor && !actor->IsPendingDestroy()) {
        World::DestroyActor(actor);
    }

    // Safe queries
    auto actors = World::GetActorsOfClass<AActor>();
    actors.erase(
        std::remove_if(actors.begin(), actors.end(),
            [](AActor* a) { return !a || a->IsPendingDestroy(); }),
        actors.end()
    );
}
```

## Integration Examples

### With Game Framework

```cpp
class AGameModeBase : public AActor {
public:
    virtual void InitGame() {
        // Spawn essential actors
        m_playerController = World::SpawnActor<APlayerController>();
        m_gameState = World::SpawnActor<AGameState>();

        // Initialize subsystems
        InitializeAI();
        InitializePhysics();
    }

    virtual void StartMatch() {
        // Spawn players
        for (int i = 0; i < GetNumPlayers(); ++i) {
            APlayerCharacter* player = World::SpawnActor<APlayerCharacter>();
            m_playerController->Possess(player);
        }

        // Start game timer
        m_matchStartTime = World::GetTime();
    }

private:
    APlayerController* m_playerController = nullptr;
    AGameState* m_gameState = nullptr;
    float m_matchStartTime = 0.0f;
};
```

### With Networking

```cpp
class ANetworkManager : public AActor {
public:
    void Server_SpawnNetworkedActor(const FString& className, const FTransform& transform) {
        // Spawn on server
        AActor* actor = World::SpawnActor<AActor>(className, transform);
        if (actor) {
            // Assign network ID
            actor->SetNetworkID(GenerateNetworkID());

            // Replicate to clients
            FSpawnPacket packet;
            packet.className = className;
            packet.transform = transform;
            packet.networkID = actor->GetNetworkID();

            Network::BroadcastReliablePacket(packet);
        }
    }

    void Client_SpawnReplicatedActor(const FSpawnPacket& packet) {
        // Spawn on client
        AActor* actor = World::SpawnActor<AActor>(packet.className, packet.transform);
        if (actor) {
            actor->SetNetworkID(packet.networkID);
            m_networkedActors[packet.networkID] = actor;
        }
    }
};
```

## Architecture Diagrams

### World Interface Flow

```
Game Code
    │
    ▼
FWorldInterface (Static Methods)
    │
    ▼
std::mutex (Thread Safety)
    │
    ▼
FWorldSubsystem (Internal)
    │
    ▼
UWorld (Core World Object)
    │
    ├── Actor Management
    ├── Time Management
    ├── Physics Simulation
    └── Level Management
```

### Actor Lifecycle

```
SpawnActor<T>(transform)
    │
    ▼
FWorldSubsystem::WithWorld()
    │
    ▼
UWorld::SpawnActor<T>()
    │
    ├── Create Actor Instance
    ├── Initialize Components
    ├── Set Transform
    ├── Call BeginPlay()
    └── Add to Actor List

    │
    ▼
Return Actor Pointer
```

### Query System

```
GetActorsOfClass<T>()
    │
    ▼
FWorldSubsystem Access
    │
    ▼
UWorld::GetActors()
    │
    ├── Iterate All Actors
    ├── Type Check (dynamic_cast)
    └── Filter Matching Types
    │
    ▼
Return Filtered Vector
```

## Troubleshooting

### Common Issues

#### Spawn Returns Null

**Symptoms:** `SpawnActor()` returns `nullptr`

**Possible Causes:**
- Invalid class name or type
- World subsystem not initialized
- Memory allocation failure
- Class not registered with reflection system

**Debug:**
```cpp
AActor* actor = World::SpawnActor<AActor>("AMyActor");
if (!actor) {
    // Check world initialization
    if (World::GetTime() < 0.0f) {
        FLogger::Log(ELogLevel::Error, "World not initialized");
    }

    // Check class registration
    UClass* cls = UClass::FindClass("AMyActor");
    if (!cls) {
        FLogger::Log(ELogLevel::Error, "Class not found: AMyActor");
    }
}
```

#### Actors Not Found in Queries

**Symptoms:** `GetActorsOfClass()` returns empty or incomplete results

**Possible Causes:**
- Actors destroyed but not removed from cache
- Type checking issues
- Timing issues (actors not yet spawned)

**Debug:**
```cpp
auto actors = World::GetActorsOfClass<AActor>();
FLogger::Log(ELogLevel::Info, "Found " + std::to_string(actors.size()) + " actors");

// Check individual actors
for (AActor* actor : actors) {
    if (!actor) {
        FLogger::Log(ELogLevel::Warning, "Null actor in results");
    } else if (actor->IsPendingDestroy()) {
        FLogger::Log(ELogLevel::Warning, "Pending destroy actor in results");
    }
}
```

#### Threading Deadlocks

**Symptoms:** Application hangs or deadlocks

**Possible Causes:**
- Calling `WithWorld()` from within `WithWorld()`
- Long-running operations inside `WithWorld()`
- Circular dependencies between threads

**Solutions:**
```cpp
// Avoid nested WithWorld calls
World::WithWorld([](UWorld& world) {
    // Don't call World::* methods here that use WithWorld internally
    // World::SpawnActor() - OK (direct world method)
    // World::GetActorsOfClass() - OK (direct world method)
    // World::WithWorld() - DEADLOCK!
});

// Keep WithWorld blocks short
World::WithWorld([](UWorld& world) {
    // Prepare data outside the lock
    std::vector<FTransform> transforms = PrepareTransforms();

    // Quick operations inside lock
    for (const auto& transform : transforms) {
        world.SpawnActor<AActor>(transform);
    }
    // Lock released here
});
```

#### Memory Leaks

**Symptoms:** Memory usage grows over time

**Possible Causes:**
- Actors not properly destroyed
- Circular references
- Cached pointers to destroyed actors

**Debug:**
```cpp
// Check for leaked actors
static size_t lastActorCount = 0;
auto actors = World::GetActors();
if (actors.size() > lastActorCount + 100) { // Arbitrary threshold
    FLogger::Log(ELogLevel::Warning, "Potential actor leak detected");
    lastActorCount = actors.size();
}

// Clean up destroyed actors from caches
void CleanupCache() {
    m_cachedActors.erase(
        std::remove_if(m_cachedActors.begin(), m_cachedActors.end(),
            [](AActor* actor) { return !actor || actor->IsPendingDestroy(); }),
        m_cachedActors.end()
    );
}
```

### Performance Monitoring

```cpp
class APerformanceMonitor : public AActor {
private:
    float m_lastTime = 0.0f;
    size_t m_lastActorCount = 0;

public:
    void Update() {
        float currentTime = World::GetTime();
        float deltaTime = currentTime - m_lastTime;
        m_lastTime = currentTime;

        // Monitor actor count
        auto actors = World::GetActors();
        if (actors.size() != m_lastActorCount) {
            FLogger::Log(ELogLevel::Info,
                "Actor count changed: " + std::to_string(m_lastActorCount) +
                " -> " + std::to_string(actors.size()));
            m_lastActorCount = actors.size();
        }

        // Monitor tick performance
        float avgTickTime = World::GetAverageTickTime();
        if (avgTickTime > 16.67f) { // 60 FPS threshold
            FLogger::Log(ELogLevel::Warning,
                "High tick time: " + std::to_string(avgTickTime) + "ms");
        }
    }
};
```

## Future Enhancements

### Planned Features

1. **Actor Pooling**: Built-in object pooling for performance
2. **Spatial Queries**: Native spatial partitioning and queries
3. **Actor Streaming**: Level-of-detail and distance-based culling
4. **Save/Load System**: World state serialization
5. **Multi-World Support**: Multiple simultaneous worlds
6. **Advanced Queries**: LINQ-style actor querying

### Extension Points

The world interface is designed for easy extension:

```cpp
// Custom world operations
class FExtendedWorldInterface {
public:
    static AActor* SpawnActorWithTag(const FString& className,
                                     const FTransform& transform,
                                     const FString& tag) {
        AActor* actor = World::SpawnActor<AActor>(className, transform);
        if (actor) {
            actor->AddTag(tag);
        }
        return actor;
    }

    static std::vector<AActor*> GetActorsWithTag(const FString& tag) {
        return World::WithWorld([tag](UWorld& world) {
            std::vector<AActor*> result;
            for (const auto& actor : world.GetActors()) {
                if (actor->HasTag(tag)) {
                    result.push_back(actor.get());
                }
            }
            return result;
        });
    }
};
```

---

*This documentation covers the complete FWorldInterface as of TKD Engine v1.0.0. The interface provides thread-safe access to world operations while maintaining high performance and ease of use.*
