///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Config.hpp>
#include <Engine/Core.hpp>
#include <Engine/Renderer.hpp>
#include <Engine/Runtime/Actor/AActor.hpp>
#include <Engine/Runtime/Time/ITickable.hpp>
#include <Engine/Runtime/World/ULevel.hpp>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief The world class, representing the game world
///
///////////////////////////////////////////////////////////////////////////////
class UWorld
    : public UObject
    , public ITickable
{
private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Struct to hold deferred spawn request
    ///
    ///////////////////////////////////////////////////////////////////////////
    struct DeferredSpawnRequest
    {
        FString className;      //<! Name of the class to spawn
        FTransform transform;   //<! Transform for the spawned actor
        void* resultPtr;        //<! Pointer to store the result (AActor**)
    };

private:
    ///////////////////////////////////////////////////////////////////////////
    // Class Member
    ///////////////////////////////////////////////////////////////////////////
    std::vector<std::shared_ptr<AActor>>
        m_actors;                         //<! The list of actors in the world
    float m_worldTime;                    //<! The current world time
    ULevel* m_currentLevel;               //<! The current level
    std::vector<ULevel> m_loadedLevels;   //<! The loaded levels
    UInt32 m_lastSnapshotID;              //<! The last snapshot ID
    bool m_hasBegunPlay;                  //<! Whether BeginPlay called
    TUniquePtr<AGameMode> m_gameMode;     //<! Pointer to the game mode
    std::vector<DeferredSpawnRequest>
        m_deferredSpawns;                 //<! Queue of deferred spawn requests

public:
    ///////////////////////////////////////////////////////////////////////////
    // RPC Functions
    ///////////////////////////////////////////////////////////////////////////
    UFunction<FString, FTransform, UUID, UInt32> SpawnActorRPC;
    UFunction<UUID> DestroyActorRPC;
    UFunction<UInt32> SpawnClientRPC;
    UFunction<UInt32, UUID, FTransform> SpawnPlayerRPC;
    UFunction<std::vector<Byte>> SyncSnapshotRPC;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor
    ///
    /// \param name The name overwrite for the UObject
    ///
    ///////////////////////////////////////////////////////////////////////////
    UWorld(const FString& name = "UWorld");

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Spawn actor RPC handler
    ///
    /// \param className The class name of the actor to spawn
    /// \param transform The transform of the actor to spawn
    /// \param actorID The UUID of the actor to spawn
    /// \param owningClientID The ID of the client that owns the actor
    ///
    ///////////////////////////////////////////////////////////////////////////
    void RPC_SpawnActor(
        const FString& className,
        const FTransform& transform,
        const UUID& actorID,
        UInt32 owningClientID
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Destroy actor RPC handler
    ///
    /// \param actorID The UUID of the actor to destroy
    ///
    ///////////////////////////////////////////////////////////////////////////
    void RPC_DestroyActor(const UUID& actorID);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Spawn client RPC handler
    ///
    /// \param owningClientID The ID of the client to spawn
    ///
    ///////////////////////////////////////////////////////////////////////////
    void RPC_SpawnClient(UInt32 owningClientID);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Spawn player RPC handler
    ///
    /// \param owningClientID The ID of the player to spawn
    /// \param playerID The UUID of the player to spawn
    /// \param transform The transform of the player to spawn
    ///
    ///////////////////////////////////////////////////////////////////////////
    void RPC_SpawnPlayer(
        UInt32 owningClientID,
        const UUID& playerID,
        const FTransform& transform
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Sync snapshot RPC handler
    ///
    /// \param snapshot The snapshot data
    ///
    ///////////////////////////////////////////////////////////////////////////
    void RPC_SyncSnapshot(const std::vector<Byte>& snapshot);

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Spawn an actor of type T in the world
    ///
    /// \tparam T The type of actor to spawn
    ///
    /// \param transform The transform of the actor
    ///
    /// \return A pointer to the spawned actor
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T = AActor>
    T* SpawnActor(const FTransform& transform = FTransform())
    {
        static_assert(
            std::is_base_of<AActor, T>::value, "T must be derived from AActor"
        );
        m_actors.push_back(std::make_shared<T>());
        T* actor = static_cast<T*>(m_actors.back().get());
        actor->SetTransform(transform);
        if (m_hasBegunPlay) { actor->BeginPlay(); }
        return actor;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Spawn an actor from a UClass
    ///
    /// \tparam T The type to cast the result to (default: AActor)
    ///
    /// \param actorClass Pointer to the UClass to spawn
    /// \param transform The transform of the actor
    ///
    /// \return A pointer to the spawned actor, or nullptr if spawn failed
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T = AActor>
    T* SpawnActor(
        UClass* actorClass, const FTransform& transform = FTransform::Identity
    )
    {
        static_assert(
            std::is_base_of<AActor, T>::value, "T must be derived from AActor"
        );

        if (actorClass == nullptr) { return nullptr; }

        // Verify the class can create AActor or derived types
        UClass* actorBaseClass = AActor::StaticClass();
        if (actorBaseClass && !actorClass->IsChildOf(actorBaseClass))
        {
            return nullptr;
        }

        // Create the instance
        UObject* instance = actorClass->CreateInstance();
        if (instance == nullptr) { return nullptr; }

        // Cast to requested type
        T* actor = dynamic_cast<T*>(instance);
        if (actor == nullptr)
        {
            // Use shared_ptr with custom deleter to properly destroy the
            // object
            std::shared_ptr<UObject>(
                instance,
                [](UObject* obj)
                {
                    // If UObject has a proper cleanup method, call it here
                    // Otherwise, this will call the correct destructor through
                    // the actual type
                    delete static_cast<AActor*>(obj);
                }
            );
            return nullptr;
        }

        // Add to world
        m_actors.push_back(std::shared_ptr<AActor>(actor));
        actor->SetTransform(transform);
        if (m_hasBegunPlay) { actor->BeginPlay(); }

        return actor;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Spawn an actor from a class name
    ///
    /// \tparam T The type to cast the result to (default: AActor)
    ///
    /// \param className The name of the class to spawn
    /// \param transform The transform of the actor
    ///
    /// \return A pointer to the spawned actor, or nullptr if spawn failed
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T = AActor>
    T* SpawnActor(
        const FString& className, const FTransform& transform = FTransform()
    )
    {
        static_assert(
            std::is_base_of<AActor, T>::value, "T must be derived from AActor"
        );

        UClass* actorClass = UClass::FindClass(className);

        if (actorClass == nullptr) { return nullptr; }

        return SpawnActor<T>(actorClass, transform);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get all actors in the world
    ///
    /// \return A constant reference to the vector of shared pointers to actors
    ///
    ///////////////////////////////////////////////////////////////////////////
    const std::vector<std::shared_ptr<AActor>>& GetActors(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get all actors of type T in the world
    ///
    /// \tparam T The type of actor to get
    ///
    /// \return A vector of pointers to the actors of type T
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    std::vector<T*> GetActorsOfClass(void) const
    {
        std::vector<T*> actors;
        for (const auto& actor: m_actors)
        {
            if (auto castedActor = dynamic_cast<T*>(actor.get()))
            {
                actors.push_back(castedActor);
            }
        }
        return actors;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get all actors of a specific UClass
    ///
    /// \param actorClass Pointer to the UClass to filter by
    /// \param includeChildren If true, also include derived classes
    ///
    /// \return A vector of pointers to matching actors
    ///
    ///////////////////////////////////////////////////////////////////////////
    std::vector<AActor*> GetActorsByClass(
        UClass* actorClass, bool includeChildren = true
    ) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Destroy an actor in the world
    ///
    /// \param actor A pointer to the actor to destroy
    ///
    ///////////////////////////////////////////////////////////////////////////
    void DestroyActor(AActor* actor);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the current level
    ///
    /// \return A pointer to the current level
    ///////////////////////////////////////////////////////////////////////////
    ULevel* GetCurrentLevel(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Called when the object is first created
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void BeginPlay(void) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Ticks the object
    ///
    /// \param deltaTime The time elapsed since the last tick
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void Tick(Float32 deltaTime) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Called when the object is destroyed
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void EndPlay(void) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the current world time
    ///
    /// \return The current world time
    ///
    ///////////////////////////////////////////////////////////////////////////
    float GetWorldTime(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Render the world
    ///
    /// \param renderer The renderer to use
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Render(IRenderer& renderer);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Load a level into the world
    ///
    /// \param levelPath The file path of the level to load
    ///
    /// \return True if the level was loaded successfully, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool SpawnLevel(ULevel* level);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Change the current level to a loaded level by name
    ///
    /// \param levelName The name of the level to switch to
    ///
    /// \return True if the level was changed successfully, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool ChangeLevel(const FString& levelName);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the game mode of the current level
    ///
    /// \return The game mode of the current level
    ///
    ///////////////////////////////////////////////////////////////////////////
    const AGameMode& GetGameMode(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the loaded levels
    ///
    /// \return A constant reference to the vector of loaded levels
    ///
    ///////////////////////////////////////////////////////////////////////////
    const std::vector<ULevel>& GetLoadedLevels(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Spawn an actor deferred (queued until after tick)
    ///
    /// \tparam T The type of actor to spawn
    ///
    /// \param transform The transform of the actor
    ///
    /// \note The actor will be spawned after the current tick completes
    /// \note Returns nullptr immediately, actual spawn happens later
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T = AActor>
    void SpawnActorDeferred(const FTransform& transform = FTransform())
    {
        static_assert(
            std::is_base_of<AActor, T>::value, "T must be derived from AActor"
        );

        UClass* actorClass = T::StaticClass();
        if (actorClass)
        {
            DeferredSpawnRequest request;
            request.className = actorClass->GetName();
            request.transform = transform;
            request.resultPtr = nullptr;
            m_deferredSpawns.push_back(request);
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Spawn an actor deferred from class name
    ///
    /// \param className The name of the class to spawn
    /// \param transform The transform of the actor
    ///
    /// \note The actor will be spawned after the current tick completes
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SpawnActorDeferred(
        const FString& className, const FTransform& transform = FTransform()
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Process all deferred spawn requests
    ///
    /// \note This should be called after Tick() completes
    ///
    ///////////////////////////////////////////////////////////////////////////
    void ProcessDeferredSpawns(void);

public:
    ///////////////////////////////////////////////////////////////////////////
    // Class Definition
    ///////////////////////////////////////////////////////////////////////////
    DECLARE_CLASS_WITH_SUPER(UWorld, UObject)
};

}   // namespace tkd
