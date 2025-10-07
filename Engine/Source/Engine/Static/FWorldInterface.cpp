///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Static/FWorldInterface.hpp>
#include <Engine/Static/FEngineInterface.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
std::mutex FWorldInterface::s_mutex;

///////////////////////////////////////////////////////////////////////////////
__internal::FWorldSubsystem* FWorldInterface::GetWorldSubsystem(void)
{
    std::lock_guard lock(s_mutex);

    try
    {
        auto& engine = FEngineInterface::GetInstance();
        return engine.GetWorld();
    }
    catch (const std::exception&)
    {
        return nullptr;
    }
}

///////////////////////////////////////////////////////////////////////////////
std::vector<std::shared_ptr<AActor>> FWorldInterface::GetActors(void)
{
    auto* worldSubsystem = GetWorldSubsystem();
    if (!worldSubsystem) { return {}; }

    std::vector<std::shared_ptr<AActor>> result;
    worldSubsystem->WithWorld(
        [&](UWorld& world)
        {
            const auto& actors = world.GetActors();
            result.assign(actors.begin(), actors.end());
        }
    );

    return result;
}

///////////////////////////////////////////////////////////////////////////////
std::vector<AActor*>
    FWorldInterface::GetActorsByClass(UClass* actorClass, bool includeChildren)
{
    auto* worldSubsystem = GetWorldSubsystem();
    if (!worldSubsystem) { return {}; }

    std::vector<AActor*> result;
    worldSubsystem->WithWorld(
        [&](UWorld& world)
        { result = world.GetActorsByClass(actorClass, includeChildren); }
    );

    return result;
}

///////////////////////////////////////////////////////////////////////////////
void FWorldInterface::DestroyActor(AActor* actor)
{
    auto* worldSubsystem = GetWorldSubsystem();
    if (!worldSubsystem) { return; }

    worldSubsystem->WithWorld([&](UWorld& world) { world.DestroyActor(actor); }
    );
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD float FWorldInterface::GetTime(void)
{
    auto* worldSubsystem = GetWorldSubsystem();
    if (!worldSubsystem) { return 0.0f; }

    float result = 0.0f;
    worldSubsystem->WithWorld([&](UWorld& world)
                              { result = world.GetWorldTime(); });

    return result;
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD double FWorldInterface::GetSimulationTime(void)
{
    auto* worldSubsystem = GetWorldSubsystem();
    if (!worldSubsystem) { return 0.0; }

    return worldSubsystem->GetSimulationTime();
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD float FWorldInterface::GetAverageTickTime(void)
{
    auto* worldSubsystem = GetWorldSubsystem();
    if (!worldSubsystem) { return 0.0f; }

    return worldSubsystem->GetAverageTickTime();
}

///////////////////////////////////////////////////////////////////////////////
void FWorldInterface::SetTargetTickRate(float tickRate)
{
    auto* worldSubsystem = GetWorldSubsystem();
    if (!worldSubsystem) { return; }

    worldSubsystem->SetTargetTickRate(tickRate);
}

}   // namespace tkd
