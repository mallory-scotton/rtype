///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Core/Menu/BP_MainMenuPlayerController.hpp>
#include <Engine/Static/FNetworkInterface.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
BP_MainMenuPlayerController::BP_MainMenuPlayerController(void)
    : APlayerController("BP_MainMenuPlayerController")
    , ServerRequestConnect(
          *this,
          "ServerRequestConnect",
          ERPCType::Server,
          std::bind(
              &BP_MainMenuPlayerController::RPC_ServerRequestConnect,
              this,
              std::placeholders::_1,
              std::placeholders::_2
          ),
          true
      )
    , m_spaceWasPressed(false)
{}

///////////////////////////////////////////////////////////////////////////////
void BP_MainMenuPlayerController::BeginPlay(void)
{
    Super::BeginPlay();

    FLogger::Info("[MainMenu] Player controller initialized");
}

///////////////////////////////////////////////////////////////////////////////
void BP_MainMenuPlayerController::SetupInputBindings(void)
{
    Super::SetupInputBindings();

    // Bind spacebar to start game
    BindActionPressed("Fire", [this](EInput input) { OnStartGamePressed(); });
}

///////////////////////////////////////////////////////////////////////////////
void BP_MainMenuPlayerController::Tick(Float32 deltaTime)
{
    Super::Tick(deltaTime);

    // Additional menu logic can go here
}

///////////////////////////////////////////////////////////////////////////////
void BP_MainMenuPlayerController::OnStartGamePressed(void)
{
    FLogger::Info("[MainMenu] Start game pressed");

    // For single-player or local host, just transition to lobby
    // In a real implementation, you might show a UI to choose:
    // - Host Game (become server)
    // - Join Game (connect to server)
    // - Single Player

    // For now, just go to lobby as if we're hosting
    World::WithWorld([](UWorld& world) { world.ChangeLevel("L_RType_Lobby"); }
    );
}

///////////////////////////////////////////////////////////////////////////////
void BP_MainMenuPlayerController::OnConnectPressed(void)
{
    FLogger::Info("[MainMenu] Connect to server requested");

    // In a real implementation, you would:
    // 1. Show UI to enter server IP/port
    // 2. Call Network::Connect(address, port)
    // 3. Wait for connection result
    // 4. On success, change to lobby level

    // Example (hardcoded for demonstration):
    /*
    if (Network::Connect("127.0.0.1", 8080))
    {

     *
     * World::WithWorld([](UWorld& world)
                         {
     *
     * world.ChangeLevel("L_RType_Lobby"); });
    }
    else
    {

     *
     * FLogger::Error("[MainMenu] Failed to connect to server");
    }
    */
}

///////////////////////////////////////////////////////////////////////////////
void BP_MainMenuPlayerController::RPC_ServerRequestConnect(
    const FString& address, UInt16 port
)
{
    // Server-side validation of connection request
    // This is where you'd implement server authentication, etc.

    if (!IsAuthority()) { return; }

    FLogger::Info(
        "[MainMenu Server] Client requested connection to {}:{}", address, port
    );

    // Validate and process connection request
    // ...
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_MainMenuPlayerController, APlayerController)

}   // namespace tkd
