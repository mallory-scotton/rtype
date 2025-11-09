///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Core/Menu/BP_LobbyGameMode.hpp>
#include <algorithm>
#include <Core/Menu/BP_LobbyPlayerController.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
BP_LobbyGameMode::BP_LobbyGameMode(void)
    : AGameMode("BP_LobbyGameMode")
    , m_readyCount(0)
{
    // Set the classes for lobby
    m_playerControllerClassName = "BP_LobbyPlayerController";
    m_actorClassName = "APawn";   // No pawn needed in lobby
    m_gameStateClassName = "AGameState";
    m_hudClassName = "BP_LobbyHUD";
    m_spectatorClassName = "ASpectator";
    m_defaultPlayerName = "Player";
    m_numPlayers = 4;   // Max 4 players
}

///////////////////////////////////////////////////////////////////////////////
void BP_LobbyGameMode::BeginPlay(void)
{
    Super::BeginPlay();

    m_readyCount = 0;
    m_playerControllers.Clear();

    FLogger::Info("[Lobby GameMode] Initialized");

    // On server, find all player controllers
    if (IsAuthority())
    {
        World::WithWorld(
            [this](UWorld& world)
            {
                // Find all lobby player controllers
                auto controllers =
                    world.GetActorsOfClass<BP_LobbyPlayerController>();
                for (auto* controller: controllers)
                {
                    RegisterPlayerController(controller);
                }

                FLogger::Info(
                    "[Lobby GameMode] Found {} player controllers",
                    m_playerControllers.Size()
                );
            }
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
void BP_LobbyGameMode::Tick(Float32 deltaTime)
{
    Super::Tick(deltaTime);

    // Periodically refresh player controller list on server
    // (in case new players join)
    static Float32 refreshTimer = 0.0f;
    refreshTimer += deltaTime;

    if (IsAuthority() && refreshTimer > 1.0f)
    {
        refreshTimer = 0.0f;

        World::WithWorld(
            [this](UWorld& world)
            {
                auto controllers =
                    world.GetActorsOfClass<BP_LobbyPlayerController>();

                // Add any new controllers
                for (auto* controller: controllers)
                {
                    auto it = std::find(
                        m_playerControllers.Begin(),
                        m_playerControllers.End(),
                        controller
                    );
                    if (it == m_playerControllers.End())
                    {
                        RegisterPlayerController(controller);
                        FLogger::Info(
                            "[Lobby GameMode] New player joined: Client {}",
                            controller->GetOwningClientID()
                        );
                    }
                }
            }
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
void BP_LobbyGameMode::OnPlayerReadyChanged(
    BP_LobbyPlayerController* controller, bool isReady
)
{
    if (!IsAuthority()) { return; }

    // Recalculate ready count
    m_readyCount = 0;
    for (auto* ctrl: m_playerControllers)
    {
        if (ctrl && ctrl->isReady()) { m_readyCount++; }
    }

    FLogger::Info(
        "[Lobby GameMode] Ready count: {}/{}",
        m_readyCount,
        m_playerControllers.Size()
    );

    // Broadcast to all clients
    BroadcastReadyStatus();

    // Check if we should launch
    CheckAndLaunchGame();
}

///////////////////////////////////////////////////////////////////////////////
void BP_LobbyGameMode::CheckAndLaunchGame(void)
{
    if (!IsAuthority()) { return; }

    UInt32 totalPlayers = m_playerControllers.Size();

    // Need at least 1 player, and all must be ready
    if (totalPlayers > 0 && m_readyCount == totalPlayers)
    {
        FLogger::Info(
            "[Lobby GameMode] All {} players ready! Launching game...",
            totalPlayers
        );

        // Launch the first level
        World::WithWorld([](UWorld& world)
                         { world.ChangeLevel("L_RType_Level1"); });
    }
}

///////////////////////////////////////////////////////////////////////////////
void BP_LobbyGameMode::BroadcastReadyStatus(void)
{
    if (!IsAuthority()) { return; }

    UInt32 totalPlayers = m_playerControllers.Size();

    // Send update to all clients
    for (auto* controller: m_playerControllers)
    {
        if (controller)
        {
            controller->ClientUpdateUI(m_readyCount, totalPlayers);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void BP_LobbyGameMode::RegisterPlayerController(
    BP_LobbyPlayerController* controller
)
{
    if (controller)
    {
        auto it = std::find(
            m_playerControllers.Begin(), m_playerControllers.End(), controller
        );
        if (it == m_playerControllers.End())
        {
            m_playerControllers.PushBack(controller);
            BroadcastReadyStatus();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_LobbyGameMode, AGameMode)

}   // namespace tkd
