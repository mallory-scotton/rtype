///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Runtime/World/UWorld.hpp>
#include <Engine/Static/FNetworkInterface.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
UWorld::UWorld(void)
    : m_actors()
    , m_worldTime(0.0f)
{}

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
ULevel* UWorld::GetCurrentLevel(void) { return &m_currentLevel; }

///////////////////////////////////////////////////////////////////////////////
void UWorld::BeginPlay(void)
{
    m_worldTime = 0.0f;
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

    // Tick all active actors
    for (const auto& actor: m_actors)
    {
        if (actor && actor->IsActive()) { actor->Tick(deltaTime); }
    }
}

///////////////////////////////////////////////////////////////////////////////
void UWorld::EndPlay(void)
{
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
bool UWorld::SpawnLevel(const ULevel& level)
{
    // Simple implementation: just set the current level to the new level
    m_currentLevel = level;

    // Clear existing actors
    for (const auto& actor: m_actors)
    {
        if (actor) { actor->EndPlay(); }
    }
    m_actors.clear();

    // Spawn actors from the level
    for (const auto& entry: level.GetActorEntries())
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
    for (const auto& level: m_loadedLevels)
    {
        if (level.GetLevelName() == levelName) { return SpawnLevel(level); }
    }

    // Level not found
    return false;
}

}   // namespace tkd
