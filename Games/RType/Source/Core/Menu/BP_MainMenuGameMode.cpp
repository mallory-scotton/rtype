///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Core/Menu/BP_MainMenuGameMode.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
BP_MainMenuGameMode::BP_MainMenuGameMode(void)
    : AGameMode("BP_MainMenuGameMode")
{
    // Set the classes for main menu
    m_playerControllerClassName = "BP_MainMenuPlayerController";
    m_actorClassName = "APawn";   // No pawn needed in main menu
    m_gameStateClassName = "AGameState";
    m_hudClassName = "BP_MainMenuHUD";
    m_spectatorClassName = "ASpectator";
    m_defaultPlayerName = "Player";
    m_numPlayers = 1;

    std::cout << "BP_MainMenuGameMode constructor called!" << std::endl;
    FLogger::Info("[MainMenu GameMode] Constructor called");
}

///////////////////////////////////////////////////////////////////////////////
void BP_MainMenuGameMode::BeginPlay(void)
{
    Super::BeginPlay();

    FLogger::Info("[MainMenu GameMode] BeginPlay called!");

    // Spawn the HUD using deferred spawning to ensure BeginPlay is called
    if (!m_hudClassName.IsEmpty())
    {
        World::SpawnActorDeferred(m_hudClassName, FTransform::Identity);
        FLogger::Info(
            "[MainMenu GameMode] Spawned HUD (deferred): {}", m_hudClassName
        );
    }
    else { FLogger::Warn("[MainMenu GameMode] HUD class name is empty!"); }
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_MainMenuGameMode, AGameMode)

}   // namespace tkd
