///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Core/Menu/BP_LobbyPlayerController.hpp>
#include <Core/Menu/BP_LobbyGameMode.hpp>
#include <Core/Menu/BP_LobbyHUD.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
BP_LobbyPlayerController::BP_LobbyPlayerController(void)
    : APlayerController("BP_LobbyPlayerController")
    , isReady(*this, "IsReady", false, EPropertyFlags::Replicated)
    , ServerSetReady(
          *this,
          "ServerSetReady",
          ERPCType::Server,
          std::bind(
              &BP_LobbyPlayerController::RPC_ServerSetReady,
              this,
              std::placeholders::_1
          ),
          true
      )
    , ClientUpdateUI(
          *this,
          "ClientUpdateUI",
          ERPCType::Client,
          std::bind(
              &BP_LobbyPlayerController::RPC_ClientUpdateUI,
              this,
              std::placeholders::_1,
              std::placeholders::_2
          ),
          true
      )
    , m_spaceWasPressed(false)
{}

///////////////////////////////////////////////////////////////////////////////
void BP_LobbyPlayerController::BeginPlay(void)
{
    Super::BeginPlay();

    isReady = false;
    FLogger::Info(
        "[Lobby] Player controller initialized for client {}",
        GetOwningClientID()
    );
}

///////////////////////////////////////////////////////////////////////////////
void BP_LobbyPlayerController::SetupInputBindings(void)
{
    Super::SetupInputBindings();

    // Bind spacebar to toggle ready
    BindActionPressed(
        "Fire", [this](EInput input) { OnToggleReadyPressed(); }
    );
}

///////////////////////////////////////////////////////////////////////////////
void BP_LobbyPlayerController::Tick(Float32 deltaTime)
{
    Super::Tick(deltaTime);
}

///////////////////////////////////////////////////////////////////////////////
void BP_LobbyPlayerController::OnToggleReadyPressed(void)
{
    // Toggle local ready state
    bool newReadyState = !isReady();
    isReady = newReadyState;

    FLogger::Info("[Lobby] Player toggled ready: {}", newReadyState);

    // Send to server
    ServerSetReady(newReadyState);
}

///////////////////////////////////////////////////////////////////////////////
void BP_LobbyPlayerController::RPC_ServerSetReady(bool ready)
{
    if (!IsAuthority()) { return; }

    // Update this controller's ready state on server
    isReady = ready;

    FLogger::Info(
        "[Lobby Server] Client {} set ready: {}", GetOwningClientID(), ready
    );

    // Get the lobby game mode to update ready counts
    World::WithWorld(
        [this, ready](UWorld& world)
        {
            AGameMode gameMode = world.GetGameMode();
            if (BP_LobbyGameMode* lobbyMode =
                    dynamic_cast<BP_LobbyGameMode*>(&gameMode))
            {
                lobbyMode->OnPlayerReadyChanged(this, ready);
            }
        }
    );
}

///////////////////////////////////////////////////////////////////////////////
void BP_LobbyPlayerController::RPC_ClientUpdateUI(
    UInt32 readyCount, UInt32 totalCount
)
{
    // This is called on clients to update their UI

    FLogger::Info(
        "[Lobby Client] Ready status: {}/{} players ready",
        readyCount,
        totalCount
    );

    // Update the lobby HUD with these counts
    World::WithWorld(
        [readyCount, totalCount](UWorld& world)
        {
            auto huds = world.GetActorsOfClass<BP_LobbyHUD>();
            for (auto* hud: huds)
            {
                if (hud) { hud->UpdateReadyStatus(readyCount, totalCount); }
            }
        }
    );
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_LobbyPlayerController, APlayerController)

}   // namespace tkd
