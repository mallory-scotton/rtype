///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Runtime/World/UWorld.hpp>

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

}   // namespace tkd
