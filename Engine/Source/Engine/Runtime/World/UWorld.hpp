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
class UWorld : public ITickable
{
private:
    ///////////////////////////////////////////////////////////////////////////
    // Class Member
    ///////////////////////////////////////////////////////////////////////////
    std::vector<std::shared_ptr<AActor>>
        m_actors;                     //<! The list of actors in the world
    float m_worldTime;                //<! The current world time
    ULevel m_currentLevel;            //<! The current level
    TVector<ULevel> m_loadedLevels;   //<! The loaded levels

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor
    ///
    ///////////////////////////////////////////////////////////////////////////
    UWorld(void);

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
        actor->BeginPlay();
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
            delete instance;
            return nullptr;
        }

        // Add to world
        m_actors.push_back(std::shared_ptr<AActor>(actor));
        actor->SetTransform(transform);
        actor->BeginPlay();

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
    bool SpawnLevel(const ULevel& level);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Change the current level to a loaded level by name
    ///
    /// \param levelName The name of the level to switch to
    ///
    /// \return True if the level was changed successfully, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool ChangeLevel(const FString& levelName);
};

}   // namespace tkd
