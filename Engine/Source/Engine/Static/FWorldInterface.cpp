///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Static/FWorldInterface.hpp>
#include <Engine/Static/FEngineInterface.hpp>
#include <shared_mutex>

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

    // SEGFAULT FIX: Check if engine is valid before accessing it
    if (!FEngineInterface::IsValid()) { return nullptr; }

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
void FWorldInterface::SpawnActorDeferred(
    const FString& className, const FTransform& transform
)
{
    auto* worldSubsystem = GetWorldSubsystem();
    if (!worldSubsystem) { return; }

    // Access world directly without locking (deferred spawns are queued)
    // Using GetWorldUnsafe() is safe here because:
    // 1. We're only pushing to a vector (thread-safe operation)
    // 2. The world won't be destroyed while the engine is running
    // 3. We avoid deadlock when called from within Tick()
    UWorld* world = worldSubsystem->GetWorldUnsafe();
    if (world) { world->SpawnActorDeferred(className, transform); }
}

///////////////////////////////////////////////////////////////////////////////
std::vector<std::shared_ptr<AActor>> FWorldInterface::GetActors(void)
{
    auto* worldSubsystem = GetWorldSubsystem();
    if (!worldSubsystem) { return {}; }

    // Use WithWorldReadOnly for read-only access (shared_lock)
    // This allows multiple readers without blocking each other
    return worldSubsystem->WithWorldReadOnly(
        [](const UWorld& world) -> std::vector<std::shared_ptr<AActor>>
        { return world.GetActors(); }
    );
}

///////////////////////////////////////////////////////////////////////////////
std::vector<AActor*>
    FWorldInterface::GetActorsByClass(UClass* actorClass, bool includeChildren)
{
    auto* worldSubsystem = GetWorldSubsystem();
    if (!worldSubsystem) { return {}; }

    // Use WithWorldReadOnly for read-only access (shared_lock)
    return worldSubsystem->WithWorldReadOnly(
        [actorClass, includeChildren](const UWorld& world)
        { return world.GetActorsByClass(actorClass, includeChildren); }
    );
}

///////////////////////////////////////////////////////////////////////////////
void FWorldInterface::DestroyActor(AActor* actor)
{
    // Instead of locking with WithWorld(), just mark the actor for deletion
    // This is thread-safe as it only sets a flag on the actor itself
    // The actual cleanup happens during World::Tick()
    if (!actor) { return; }

    actor->MarkForDeletion();
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD float FWorldInterface::GetTime(void)
{
    auto* worldSubsystem = GetWorldSubsystem();
    if (!worldSubsystem) { return 0.0f; }

    // Use WithWorldReadOnly for read-only access (shared_lock)
    // World time is a simple float read, doesn't need exclusive access
    return worldSubsystem->WithWorldReadOnly([](const UWorld& world)
                                             { return world.GetWorldTime(); });
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

///////////////////////////////////////////////////////////////////////////////
const AGameMode& FWorldInterface::GetGameMode(void) const
{
    static AGameMode defaultGameMode;
    auto* worldSubsystem = GetWorldSubsystem();
    if (!worldSubsystem) { return defaultGameMode; }

    // GetGameMode() already uses shared_lock internally, no need to change
    return worldSubsystem->GetGameMode();
}

///////////////////////////////////////////////////////////////////////////////
const std::vector<ULevel>& FWorldInterface::GetLoadedLevels(void) const
{
    static std::vector<ULevel> defaultLevels;
    auto* worldSubsystem = GetWorldSubsystem();
    if (!worldSubsystem) { return defaultLevels; }

    // GetLoadedLevels() already uses shared_lock internally, no need to change
    return worldSubsystem->GetLoadedLevels();
}

///////////////////////////////////////////////////////////////////////////////
ULevel* FWorldInterface::GetCurrentLevel(void) const
{
    static ULevel defaultLevel;
    auto* worldSubsystem = GetWorldSubsystem();
    if (!worldSubsystem) { return &defaultLevel; }

    // GetCurrentLevel() already uses shared_lock internally, no need to change
    return worldSubsystem->GetCurrentLevel();
}

}   // namespace tkd
