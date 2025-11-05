///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Config.hpp>
#include <Engine/Runtime.hpp>
#include <Engine/Static/FEngineInterface.hpp>
#include <mutex>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief World Interface for static access to the game world
///
///////////////////////////////////////////////////////////////////////////////
class FWorldInterface final
{
private:
    ///////////////////////////////////////////////////////////////////////////
    // Class Static Member
    ///////////////////////////////////////////////////////////////////////////
    static std::mutex s_mutex;   //<! Mutex for thread safety

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the world subsystem (internal helper)
    ///
    /// \return Pointer to the world subsystem
    ///
    ///////////////////////////////////////////////////////////////////////////
    static __internal::FWorldSubsystem* GetWorldSubsystem(void);

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
    /// \warning This may cause a deadlock if called from within Tick()
    /// \note Use SpawnActorDeferred() if calling from Tick() or other
    ///       callbacks that may already hold the world mutex
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T = AActor>
    static T* SpawnActor(const FTransform& transform = FTransform())
    {
        static_assert(
            std::is_base_of<AActor, T>::value, "T must be derived from AActor"
        );

        auto* worldSubsystem = GetWorldSubsystem();
        if (!worldSubsystem) { return nullptr; }

        T* result = nullptr;
        worldSubsystem->WithWorld([&](UWorld& world)
                                  { result = world.SpawnActor<T>(transform); }
        );

        return result;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Spawn an actor with constructor parameters
    ///
    /// \tparam T The type of actor to spawn
    /// \tparam Args The types of constructor arguments
    ///
    /// \param transform The transform of the actor
    /// \param args Constructor arguments to forward to T's constructor
    ///
    /// \return A pointer to the spawned actor
    ///
    /// \warning This may cause a deadlock if called from within Tick()
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T = AActor, typename... Args>
    static T* SpawnActorWithParams(const FTransform& transform, Args&&... args)
    {
        static_assert(
            std::is_base_of<AActor, T>::value, "T must be derived from AActor"
        );

        auto* worldSubsystem = GetWorldSubsystem();
        if (!worldSubsystem) { return nullptr; }

        T* result = nullptr;
        worldSubsystem->WithWorld(
            [&](UWorld& world)
            {
                result = world.SpawnActorWithParams<T>(
                    transform, std::forward<Args>(args)...
                );
            }
        );

        return result;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Spawn an actor deferred (safe to call from Tick)
    ///
    /// \tparam T The type of actor to spawn
    ///
    /// \param transform The transform of the actor
    ///
    /// \note The actor will be spawned after the current tick completes
    /// \note This is safe to call from within Tick() or other callbacks
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T = AActor>
    static void SpawnActorDeferred(const FTransform& transform = FTransform())
    {
        static_assert(
            std::is_base_of<AActor, T>::value, "T must be derived from AActor"
        );

        auto* worldSubsystem = GetWorldSubsystem();
        if (!worldSubsystem) { return; }

        // Access world directly without locking (deferred spawns are queued)
        UWorld* world = worldSubsystem->GetWorldUnsafe();
        if (world) { world->SpawnActorDeferred<T>(transform); }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Spawn an actor deferred with constructor parameters
    ///
    /// \tparam T The type of actor to spawn
    /// \tparam Args The types of constructor arguments
    ///
    /// \param transform The transform of the actor
    /// \param args Constructor arguments to forward to T's constructor
    ///
    /// \note The actor will be spawned after the current tick completes
    /// \note This is safe to call from within Tick() or other callbacks
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T = AActor, typename... Args>
    static void SpawnActorDeferredWithParams(
        const FTransform& transform, Args&&... args
    )
    {
        static_assert(
            std::is_base_of<AActor, T>::value, "T must be derived from AActor"
        );

        auto* worldSubsystem = GetWorldSubsystem();
        if (!worldSubsystem) { return; }

        // Access world directly without locking (deferred spawns are queued)
        UWorld* world = worldSubsystem->GetWorldUnsafe();
        if (world)
        {
            world->SpawnActorDeferredWithParams<T>(
                transform, std::forward<Args>(args)...
            );
        }
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
    /// \warning This may cause a deadlock if called from within Tick()
    /// \note Use SpawnActorDeferred() if calling from Tick() or other
    ///       callbacks that may already hold the world mutex
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T = AActor>
    static T* SpawnActor(
        UClass* actorClass, const FTransform& transform = FTransform::Identity
    )
    {
        static_assert(
            std::is_base_of<AActor, T>::value, "T must be derived from AActor"
        );

        auto* worldSubsystem = GetWorldSubsystem();
        if (!worldSubsystem) { return nullptr; }

        T* result = nullptr;
        worldSubsystem->WithWorld(
            [&](UWorld& world)
            { result = world.SpawnActor<T>(actorClass, transform); }
        );

        return result;
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
    /// \warning This may cause a deadlock if called from within Tick()
    /// \note Use SpawnActorDeferred() if calling from Tick() or other
    ///       callbacks that may already hold the world mutex
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T = AActor>
    static T* SpawnActor(
        const FString& className, const FTransform& transform = FTransform()
    )
    {
        static_assert(
            std::is_base_of<AActor, T>::value, "T must be derived from AActor"
        );

        auto* worldSubsystem = GetWorldSubsystem();
        if (!worldSubsystem) { return nullptr; }

        T* result = nullptr;
        worldSubsystem->WithWorld(
            [&](UWorld& world)
            { result = world.SpawnActor<T>(className, transform); }
        );

        return result;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Spawn an actor deferred from class name (safe to call from Tick)
    ///
    /// \param className The name of the class to spawn
    /// \param transform The transform of the actor
    ///
    /// \note The actor will be spawned after the current tick completes
    /// \note This is safe to call from within Tick() or other callbacks
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void SpawnActorDeferred(
        const FString& className, const FTransform& transform = FTransform()
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get all actors in the world
    ///
    /// \return A vector of shared pointers to actors
    ///
    ///////////////////////////////////////////////////////////////////////////
    static std::vector<std::shared_ptr<AActor>> GetActors(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get all actors of type T in the world
    ///
    /// \tparam T The type of actor to get
    ///
    /// \return A vector of pointers to the actors of type T
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    static std::vector<T*> GetActorsOfClass(void)
    {
        auto* worldSubsystem = GetWorldSubsystem();
        if (!worldSubsystem) { return {}; }

        std::vector<T*> result;
        worldSubsystem->WithWorld([&](UWorld& world)
                                  { result = world.GetActorsOfClass<T>(); });

        return result;
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
    static std::vector<AActor*>
        GetActorsByClass(UClass* actorClass, bool includeChildren = true);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Destroy an actor in the world
    ///
    /// \param actor A pointer to the actor to destroy
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void DestroyActor(AActor* actor);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the current world time
    ///
    /// \return The current world time in seconds
    ///
    ///////////////////////////////////////////////////////////////////////////
    TKD_NODISCARD static float GetTime(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the current simulation time
    ///
    /// \return The simulation time in seconds
    ///
    ///////////////////////////////////////////////////////////////////////////
    TKD_NODISCARD static double GetSimulationTime(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the average tick time
    ///
    /// \return The average tick time in milliseconds
    ///
    ///////////////////////////////////////////////////////////////////////////
    TKD_NODISCARD static float GetAverageTickTime(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the target tick rate
    ///
    /// \param tickRate The new target tick rate (0 for variable)
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void SetTargetTickRate(float tickRate);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Execute a function with exclusive access to the world
    ///
    /// \tparam Func The type of the function
    ///
    /// \param func The function to execute
    ///
    /// \return The result of the function
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename Func>
    static auto WithWorld(Func&& func)
        -> decltype(func(std::declval<UWorld&>()))
    {
        auto* worldSubsystem = GetWorldSubsystem();
        if (!worldSubsystem)
        {
            throw std::runtime_error("World subsystem is not initialized.");
        }

        return worldSubsystem->WithWorld(std::forward<Func>(func));
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the game mode of the current level
    ///
    /// \return The game mode of the current level
    ///
    ///////////////////////////////////////////////////////////////////////////
    static const AGameMode& GetGameMode(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the game mode of the current level
    ///
    /// \return The game mode of the current level
    ///
    ///////////////////////////////////////////////////////////////////////////
    static AGameMode& GetGameModeUnsafe(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the loaded levels
    ///
    /// \return A constant reference to the vector of loaded levels
    ///
    ///////////////////////////////////////////////////////////////////////////
    static const std::vector<ULevel>& GetLoadedLevels(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the current level
    ///
    /// \return A pointer to the current level
    ///
    ///////////////////////////////////////////////////////////////////////////
    static ULevel* GetCurrentLevel(void);
};

}   // namespace tkd

///////////////////////////////////////////////////////////////////////////////
// Alias for easier access
///////////////////////////////////////////////////////////////////////////////
using World = tkd::FWorldInterface;
