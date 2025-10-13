///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Runtime/Core/AGameMode.hpp>
#include <Engine/Core/Math.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
AGameMode::AGameMode(const FString& name)
    : UObject(name)
    , m_actorClassName()
    , m_playerControllerClassName()
    , m_gameStateClassName()
    , m_hudClassName()
    , m_spectatorClassName()
    , m_defaultPlayerName("Player")
    , m_numPlayers(1)
    , m_minRespawnDelay(1.0f)
    , m_maxRespawnDelay(3.0f)
    , m_gravityZ(-980.0f)
    , m_worldToMeters(100.0f)
{
    m_numPlayers = Math<UInt32>::Max(m_numPlayers, 1);
    m_minRespawnDelay = Math<Float32>::Max(m_minRespawnDelay, 0.0f);
    m_maxRespawnDelay = Math<Float32>::Max(m_maxRespawnDelay, 0.0f);

    if (m_minRespawnDelay > m_maxRespawnDelay)
    {
        Math<Float32>::Swap(m_minRespawnDelay, m_maxRespawnDelay);
    }

    if (m_worldToMeters < 0.0f) { m_worldToMeters = 100.0f; }
}

///////////////////////////////////////////////////////////////////////////////
void AGameMode::BeginPlay(void) {}

///////////////////////////////////////////////////////////////////////////////
void AGameMode::Tick(Float32) {}

///////////////////////////////////////////////////////////////////////////////
void AGameMode::EndPlay(void) {}

///////////////////////////////////////////////////////////////////////////////
Bool AGameMode::IsValid(void) const
{
    if (m_actorClassName.IsEmpty() || m_playerControllerClassName.IsEmpty())
    {
        return false;
    }

    if (UClass::FindClass(m_actorClassName) == nullptr) { return false; }

    if (UClass::FindClass(m_playerControllerClassName) == nullptr)
    {
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
const FString& AGameMode::GetActorClassName(void) const
{
    return m_actorClassName;
}

///////////////////////////////////////////////////////////////////////////////
UClass* AGameMode::GetActorClass(void) const
{
    return UClass::FindClass(m_actorClassName);
}

///////////////////////////////////////////////////////////////////////////////
const FString& AGameMode::GetPlayerControllerClassName(void) const
{
    return m_playerControllerClassName;
}

///////////////////////////////////////////////////////////////////////////////
UClass* AGameMode::GetPlayerControllerClass(void) const
{
    return UClass::FindClass(m_playerControllerClassName);
}

///////////////////////////////////////////////////////////////////////////////
const FString& AGameMode::GetGameStateClassName(void) const
{
    return m_gameStateClassName;
}

///////////////////////////////////////////////////////////////////////////////
UClass* AGameMode::GetGameStateClass(void) const
{
    return UClass::FindClass(m_gameStateClassName);
}

///////////////////////////////////////////////////////////////////////////////
const FString& AGameMode::GetHUDClassName(void) const
{
    return m_hudClassName;
}

///////////////////////////////////////////////////////////////////////////////
UClass* AGameMode::GetHUDClass(void) const
{
    return UClass::FindClass(m_hudClassName);
}

///////////////////////////////////////////////////////////////////////////////
const FString& AGameMode::GetSpectatorClassName(void) const
{
    return m_spectatorClassName;
}

///////////////////////////////////////////////////////////////////////////////
UClass* AGameMode::GetSpectatorClass(void) const
{
    return UClass::FindClass(m_spectatorClassName);
}

///////////////////////////////////////////////////////////////////////////////
const FString& AGameMode::GetDefaultPlayerName(void) const
{
    return m_defaultPlayerName;
}

///////////////////////////////////////////////////////////////////////////////
UInt32 AGameMode::GetPlayerCount(void) const { return m_numPlayers; }

///////////////////////////////////////////////////////////////////////////////
Float32 AGameMode::GetMinimumRespawnDelay(void) const
{
    return m_minRespawnDelay;
}

///////////////////////////////////////////////////////////////////////////////
Float32 AGameMode::GetMaximumRespawnDelay(void) const
{
    return m_maxRespawnDelay;
}

///////////////////////////////////////////////////////////////////////////////
Float32 AGameMode::GetGravityZ(void) const { return m_gravityZ; }

///////////////////////////////////////////////////////////////////////////////
Float32 AGameMode::GetWorldToMeters(void) const { return m_worldToMeters; }

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(AGameMode, UObject)

}   // namespace tkd
