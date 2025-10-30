///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Runtime/World/UWorld.hpp>
#include <Engine/Runtime/Actor.hpp>
#include <Engine/Runtime/Controllers.hpp>
#include <Engine/Static/FNetworkInterface.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
UWorld::UWorld(const FString& name)
    : UObject(name, UUID() /* UUID 0000-0000-0000 */)
    , m_actors()
    , m_worldTime(0.0f)
    , m_currentLevel()
    , m_loadedLevels()
    , m_lastSnapshotID(0)
    , m_hasBegunPlay(false)
    , SpawnActorRPC(
          *this,
          "SpawnActor",
          ERPCType::Client,
          std::bind(
              &UWorld::RPC_SpawnActor,
              this,
              std::placeholders::_1,
              std::placeholders::_2,
              std::placeholders::_3,
              std::placeholders::_4
          ),
          true
      )
    , DestroyActorRPC(
          *this,
          "DestroyActor",
          ERPCType::Client,
          std::bind(&UWorld::RPC_DestroyActor, this, std::placeholders::_1),
          true
      )
    , SpawnClientRPC(
          *this,
          "SpawnClient",
          ERPCType::Server,
          std::bind(&UWorld::RPC_SpawnClient, this, std::placeholders::_1)
      )
    , SpawnPlayerRPC(
          *this,
          "SpawnPlayer",
          ERPCType::Client,
          std::bind(
              &UWorld::RPC_SpawnPlayer,
              this,
              std::placeholders::_1,
              std::placeholders::_2,
              std::placeholders::_3
          ),
          true
      )
    , SyncSnapshotRPC(
          *this,
          "SyncSnapshot",
          ERPCType::Client,
          std::bind(&UWorld::RPC_SyncSnapshot, this, std::placeholders::_1)
      )
{
#if TKD_ENGINE_SERVER
    SetNetRole(ENetRole::Authority);
#else
    SetNetRole(ENetRole::SimulatedProxy);
#endif

    auto classes = UClass::GetAllClasses();

    for (auto* cls: classes)
    {
        if (cls && cls->IsChildOf(ULevel::StaticClass()) &&
            cls != ULevel::StaticClass())
        {
            UObject* instance = cls->CreateInstance();
            if (instance)
            {
                ULevel* level = dynamic_cast<ULevel*>(instance);
                if (level) { m_loadedLevels.push_back(*level); }
                delete static_cast<ULevel*>(instance);
            }
        }
    }

    if (!m_loadedLevels.empty())
    {
        m_currentLevel = &m_loadedLevels[0];
        SpawnLevel(m_currentLevel);
    }
}

///////////////////////////////////////////////////////////////////////////////
const std::vector<std::shared_ptr<AActor>>& UWorld::GetActors(void) const
{
    return m_actors;
}

///////////////////////////////////////////////////////////////////////////////
std::vector<AActor*>
    UWorld::GetActorsByClass(UClass* actorClass, bool includeChildren) const
{
    std::vector<AActor*> result;

    if (actorClass == nullptr) { return result; }

    for (const auto& actor: m_actors)
    {
        if (actor == nullptr) { continue; }

        UClass* instanceClass = actor->StaticClass();
        if (instanceClass == nullptr) { continue; }

        bool matches = false;
        if (includeChildren)
        {
            matches = (instanceClass == actorClass) ||
                      instanceClass->IsChildOf(actorClass);
        }
        else { matches = (instanceClass == actorClass); }

        if (matches) { result.push_back(actor.get()); }
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
void UWorld::DestroyActor(AActor* actor)
{
    if (actor == nullptr) { return; }
    actor->MarkForDeletion();
}

///////////////////////////////////////////////////////////////////////////////
ULevel* UWorld::GetCurrentLevel(void) { return m_currentLevel; }

///////////////////////////////////////////////////////////////////////////////
void UWorld::BeginPlay(void)
{
    // Initialize world time
    m_worldTime = 0.0f;

    // Set begun play flag
    m_hasBegunPlay = true;

    // Begin play for game mode
    if (m_gameMode) { m_gameMode->BeginPlay(); }

    // Begin play
    for (const auto& actor: m_actors) { actor->BeginPlay(); }
}

///////////////////////////////////////////////////////////////////////////////
void UWorld::Tick(Float32 deltaTime)
{
    // Update world time
    m_worldTime += deltaTime;

    // Destroy actors that are marked for deletion
    m_actors.erase(
        std::remove_if(
            m_actors.begin(),
            m_actors.end(),
            [](const std::shared_ptr<AActor>& actor)
            {
                bool toDelete = actor && actor->IsMarkedForDeletion();
                if (toDelete && actor) { actor->EndPlay(); }
                return toDelete;
            }
        ),
        m_actors.end()
    );

    // Tick the game mode
    if (m_gameMode) { m_gameMode->Tick(deltaTime); }

    // Tick all active actors
    for (const auto& actor: m_actors)
    {
        if (actor && actor->IsActive()) { actor->Tick(deltaTime); }
    }
}

///////////////////////////////////////////////////////////////////////////////
void UWorld::EndPlay(void)
{
    // End play for game mode
    if (m_gameMode) { m_gameMode->EndPlay(); }

    // Reset begun play flag
    m_hasBegunPlay = false;

    // End play for all actors
    for (const auto& actor: m_actors)
    {
        if (actor) { actor->EndPlay(); }
    }
}

///////////////////////////////////////////////////////////////////////////////
float UWorld::GetWorldTime(void) const { return m_worldTime; }

///////////////////////////////////////////////////////////////////////////////
void UWorld::Render(IRenderer& renderer)
{
    for (const auto& actor: m_actors)
    {
        if (actor && actor->IsActive())
        {
            auto components = actor->GetComponents();
            for (const auto& component: components)
            {
                if (component && component->IsActive())
                {
                    component->Render(renderer);
                }
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
bool UWorld::SpawnLevel(ULevel* level)
{
    if (level == nullptr) { return false; }

    // Simple implementation: just set the current level to the new level
    m_currentLevel = level;

    // Clear existing actors
    for (const auto& actor: m_actors)
    {
        if (actor) { actor->EndPlay(); }
    }
    m_actors.clear();

    // Get the game mode class from the level
    UClass* gameModeClass = UClass::FindClass(level->GetGameMode());
    if (gameModeClass)
    {
        UObject* instance = gameModeClass->CreateInstance();
        if (instance)
        {
            AGameMode* gameMode = instance->As<AGameMode>();
            if (gameMode) { m_gameMode.reset(gameMode); }
            else { delete static_cast<AGameMode*>(instance); }
        }
    }

    if (!m_gameMode)
    {
        // Fallback to default game mode if none specified
        UClass* defaultGameModeClass = UClass::FindClass("AGameMode");
        if (defaultGameModeClass)
        {
            UObject* instance = defaultGameModeClass->CreateInstance();
            if (instance)
            {
                AGameMode* gameMode = instance->As<AGameMode>();
                if (gameMode) { m_gameMode.reset(gameMode); }
                else { delete static_cast<AGameMode*>(instance); }
            }
        }
    }

    // Spawn actors from the level
    for (const auto& entry: level->GetActorEntries())
    {
        auto actor = SpawnActor(
            entry.class_name,
            FTransform(entry.position, entry.rotation, entry.scale)
        );

        // If the actor failed to spawn, skip it
        if (!actor) { continue; }

        // Set the actor's properties
        for (const auto& prop: entry.properties)
        {
            auto propPtr = actor->GetProperty(prop.name);
            if (!propPtr) { continue; }
            propPtr->SetValue(
                static_cast<const void*>(prop.value.data()), prop.size
            );
        }
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool UWorld::ChangeLevel(const FString& levelName)
{
    // Find the level by name in the loaded levels
    for (SizeT i = 0; i < m_loadedLevels.size(); i++)
    {
        ULevel* level = &m_loadedLevels[i];
        if (level->GetLevelName() == levelName) { return SpawnLevel(level); }
    }

    // Level not found
    return false;
}

///////////////////////////////////////////////////////////////////////////////
const AGameMode& UWorld::GetGameMode(void) const { return *m_gameMode; }

///////////////////////////////////////////////////////////////////////////////
const std::vector<ULevel>& UWorld::GetLoadedLevels(void) const
{
    return m_loadedLevels;
}

///////////////////////////////////////////////////////////////////////////////
void UWorld::RPC_SyncSnapshot(const std::vector<Byte>& snapshotData)
{
    // Create a snapshot object to hold the deserialized data
    Packets::Snapshot snapshot;
    FBinaryReader reader(
        snapshotData.data(), static_cast<SizeT>(snapshotData.size())
    );

    // Try to deserialize the snapshot
    if (!snapshot.Deserialize(reader)) { return; }

    // Verify snapshot ID
    if (snapshot.snapshotID < /*=*/m_lastSnapshotID) { return; }

    // Update last processed snapshot ID
    m_lastSnapshotID = snapshot.snapshotID;

    // RACE CONDITION FIX: Copy actors vector to avoid iterator invalidation
    // when spawning new actors during snapshot processing
    auto actorsCopy = m_actors;

    // Loop through each actor state in the snapshot
    for (const auto& actorState: snapshot.actors)
    {
        // Find the actor in the copied vector by its UUID
        auto it = std::find_if(
            actorsCopy.begin(),
            actorsCopy.end(),
            [&actorState](const std::shared_ptr<AActor>& actor)
            { return actor && actor->GetUUID() == actorState.id; }
        );

        if (it != actorsCopy.end())
        {
            // Actor exists, update its state
            auto& actor = *it;
            actor->SetTransform(actorState.transform);

            // Update properties
            for (const auto& [propName, propData]: actorState.properties)
            {
                auto propPtr = actor->GetProperty(propName);
                if (propPtr)
                {
                    propPtr->SetValue(
                        static_cast<const void*>(propData.data()),
                        propData.size()
                    );
                }
            }
        }
        else
        {
            // Actor does not exist, spawn it
            UClass* actorClass = UClass::FindClass(actorState.className);
            if (actorClass)
            {
                auto newActor = SpawnActor(actorClass, actorState.transform);
                if (newActor)
                {
                    // Set network-related properties
                    newActor->SetUUID(actorState.id);
                    newActor->SetOwningClientID(actorState.owningClientID);
                    newActor->SetNetRole(ENetRole::SimulatedProxy);

                    // Set properties
                    for (const auto& [propName, propData]:
                         actorState.properties)
                    {
                        auto propPtr = newActor->GetProperty(propName);
                        if (propPtr)
                        {
                            propPtr->SetValue(
                                static_cast<const void*>(propData.data()),
                                propData.size()
                            );
                        }
                    }
                }
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void UWorld::RPC_SpawnActor(
    const FString& className,
    const FTransform& transform,
    const UUID& actorID,
    UInt32 owningClientID
)
{
    // Find the class by name
    UClass* actorClass = UClass::FindClass(className);

    // Check if the class is valid
    if (actorClass == nullptr) { return; }

    // Check if the RPC is for the local client (to avoid duplicate spawns)
    if (Network::GetClientID() == owningClientID) { return; }

    // Spawn the actor
    auto actor = SpawnActor(actorClass, transform);
    if (actor)
    {
        actor->SetUUID(actorID);
        actor->SetOwningClientID(owningClientID);
        actor->SetNetRole(ENetRole::SimulatedProxy);
    }
}

///////////////////////////////////////////////////////////////////////////////
void UWorld::RPC_DestroyActor(const UUID& actorID)
{
    auto it = std::find_if(
        m_actors.begin(),
        m_actors.end(),
        [&actorID](const std::shared_ptr<AActor>& actor)
        { return actor && actor->GetNetworkID() == actorID; }
    );

    if (it != m_actors.end()) { (*it)->MarkForDeletion(); }
}

///////////////////////////////////////////////////////////////////////////////
void UWorld::RPC_SpawnPlayer(
    UInt32 owningClientID, const UUID& playerID, const FTransform& transform
)
{
    // Get the player and controller classes from the game mode
    UClass* plyrClass = m_gameMode->GetActorClass();
    UClass* ctlrClass = m_gameMode->GetPlayerControllerClass();

    // Check if both classes are valid
    if (plyrClass == nullptr || ctlrClass == nullptr) { return; }

    // Spawn the player actor
    auto playerObj = SpawnActor(plyrClass, transform);
    auto controllerObj = SpawnActor(ctlrClass, FTransform::Identity);
    auto player = playerObj->As<APawn>();
    auto controller = controllerObj->As<APlayerController>();

    // Check if both actors were spawned successfully
    if (player == nullptr || controller == nullptr)
    {
        if (playerObj) { playerObj->MarkForDeletion(); }
        if (controllerObj) { controllerObj->MarkForDeletion(); }
        return;
    }

    // Set up the player actor
    player->SetUUID(playerID);
    player->SetOwningClientID(owningClientID);
    player->SetNetRole(ENetRole::AutonomousProxy);

    // Set up the controller actor
    controller->SetOwningClientID(owningClientID);
    controller->SetNetRole(ENetRole::AutonomousProxy);
    controller->Possess(player);
}

///////////////////////////////////////////////////////////////////////////////
void UWorld::RPC_SpawnClient(UInt32 owningClientID)
{
    // Get the player class from the game mode
    UClass* plyrClass = m_gameMode->GetActorClass();

    // Check if the class is valid
    if (plyrClass == nullptr) { return; }

    FTransform transform = FTransform::Identity;

    // Create a snapshot of the current world state to send to the new client
    Packets::Snapshot snapshot(*this);

    // Spawn the player actor
    auto playerObj = SpawnActor(plyrClass, transform);
    auto player = playerObj->As<APawn>();

    // Check if the actor was spawned successfully
    if (player == nullptr)
    {
        if (playerObj) { playerObj->MarkForDeletion(); }
        return;
    }

    // Set up the player actor
    player->SetOwningClientID(owningClientID);
    player->SetNetRole(ENetRole::Authority);

    // Notify the game mode of the new player
    // GetGameMode().OnPlayerJoined(player);

    // Set the owning client ID for the RPC
    SetOwningClientID(owningClientID);

    // Send Snapshot to the new client
    Network::SendReliablePacket(snapshot, owningClientID);

    // Prepare the RPC to spawn the player on the client
    this->SpawnPlayerRPC(
        owningClientID, player->GetUUID(), player->GetTransform()
    );

    // Also spawn the actor for all other clients
    this->SpawnActorRPC.SetRPCType(ERPCType::Multicast);
    this->SpawnActorRPC(
        plyrClass->GetName(),
        player->GetTransform(),
        player->GetUUID(),
        owningClientID
    );
    this->SpawnActorRPC.SetRPCType(ERPCType::Client);

    // Reset the owning client ID
    SetOwningClientID(0);
}

///////////////////////////////////////////////////////////////////////////////
void UWorld::SpawnActorDeferred(
    const FString& className, const FTransform& transform
)
{
    std::lock_guard<std::mutex> lock(m_deferredSpawnsMutex);
    DeferredSpawnRequest request;
    request.className = className;
    request.transform = transform;
    request.resultPtr = nullptr;
    m_deferredSpawns.push_back(request);
}

///////////////////////////////////////////////////////////////////////////////
void UWorld::ProcessDeferredSpawns(void)
{
    std::vector<DeferredSpawnRequest> spawnsToProcess;

    {
        std::lock_guard<std::mutex> lock(m_deferredSpawnsMutex);
        if (m_deferredSpawns.empty()) { return; }

        // Move spawns to local vector to minimize lock time
        spawnsToProcess = std::move(m_deferredSpawns);
        m_deferredSpawns.clear();
    }

    // Process all deferred spawns without holding the mutex
    for (const auto& request: spawnsToProcess)
    {
        AActor* actor = nullptr;

        // Use factory function if available (for parameterized spawns)
        if (request.factory)
        {
            auto actorPtr = request.factory();
            if (actorPtr)
            {
                m_actors.push_back(actorPtr);
                actor = actorPtr.get();
                actor->SetTransform(request.transform);
                if (m_hasBegunPlay) { actor->BeginPlay(); }
            }
        }
        else
        {
            // Fallback to UClass-based spawning for string class names
            UClass* actorClass = UClass::FindClass(request.className);
            if (actorClass)
            {
                actor = SpawnActor<AActor>(actorClass, request.transform);
            }
        }

        // If a result pointer was provided, store the result
        if (actor && request.resultPtr != nullptr)
        {
            *static_cast<AActor**>(request.resultPtr) = actor;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(UWorld, UObject)

}   // namespace tkd
