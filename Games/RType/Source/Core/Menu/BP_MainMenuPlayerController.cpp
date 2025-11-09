///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Core/Menu/BP_MainMenuPlayerController.hpp>
#include <Core/UI/BP_MainMenuHUD.hpp>
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

    // No input bindings needed - buttons handle everything
}

///////////////////////////////////////////////////////////////////////////////
void BP_MainMenuPlayerController::Tick(Float32 deltaTime)
{
    Super::Tick(deltaTime);
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
