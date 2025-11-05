///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Core/Menu/BP_LobbyController.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
BP_LobbyController::BP_LobbyController(void)
    : AActor()
    , totalPlayers(*this, "TotalPlayers", 0, EPropertyFlags::Replicated)
    , readyPlayers(*this, "ReadyPlayers", 0, EPropertyFlags::Replicated)
    , ServerToggleReady(
          *this,
          "ServerToggleReady",
          ERPCType::Server,
          std::bind(
              &BP_LobbyController::RPC_ServerToggleReady,
              this,
              std::placeholders::_1
          ),
          true
      )
    , MulticastUpdateReadyCount(
          *this,
          "MulticastUpdateReadyCount",
          ERPCType::Multicast,
          std::bind(
              &BP_LobbyController::RPC_MulticastUpdateReadyCount,
              this,
              std::placeholders::_1,
              std::placeholders::_2
          ),
          true
      )
    , m_inputManager(nullptr)
    , m_isLocalPlayerReady(false)
{}

///////////////////////////////////////////////////////////////////////////////
void BP_LobbyController::BeginPlay(void)
{
    Super::BeginPlay();

    // Get input manager reference
    m_inputManager = FWindowInterface::GetInputManager();

    // If this is the server, initialize player tracking
    if (IsAuthority())
    {
        // Get the network subsystem to find connected clients
        auto* network = FNetworkInterface::GetNetworkBase();
        if (network)
        {
            totalPlayers =
                network->GetConnectedClientCount() + 1;   // +1 for host
            readyPlayers = 0;

            FLogger::Info(
                "[Lobby] Initialized with {} players", totalPlayers()
            );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void BP_LobbyController::Tick(Float32 deltaTime)
{
    Super::Tick(deltaTime);

    if (!m_inputManager) { return; }

    // Check if spacebar is pressed to toggle ready
    auto* fireAction = m_inputManager->GetAction("Fire");
    if (fireAction && fireAction->IsPressed())
    {
        // Toggle local ready state
        m_isLocalPlayerReady = !m_isLocalPlayerReady;

        FLogger::Info(
            "[Lobby] Local player toggled ready: {}", m_isLocalPlayerReady
        );

        // Send RPC to server
        auto* network = FNetworkInterface::GetNetworkBase();
        if (network)
        {
            UInt32 clientID = network->GetClientID();
            ServerToggleReady(clientID);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void BP_LobbyController::RPC_ServerToggleReady(UInt32 clientID)
{
    if (!IsAuthority()) { return; }

    // Toggle the ready state for this client
    bool& isReady = m_playerReadyStates[clientID];
    isReady = !isReady;

    // Count how many players are ready
    UInt32 ready = 0;
    for (const auto& pair: m_playerReadyStates)
    {
        if (pair.second) { ready++; }
    }

    readyPlayers = ready;

    FLogger::Info(
        "[Lobby Server] Client {} toggled ready. {}/{} ready",
        clientID,
        ready,
        totalPlayers()
    );

    // Broadcast updated count to all clients
    MulticastUpdateReadyCount(ready, totalPlayers());

    // Check if we should launch the game
    CheckAndLaunchGame();
}

///////////////////////////////////////////////////////////////////////////////
void BP_LobbyController::RPC_MulticastUpdateReadyCount(
    UInt32 ready, UInt32 total
)
{
    // Skip on server since it already updated in RPC_ServerToggleReady
    if (IsAuthority()) { return; }

    // Update clients with the ready count for UI display
    FLogger::Info("[Lobby] Ready status: {}/{} players ready", ready, total);

    // Here you would update UI elements to show ready status
    // For example: UpdateLobbyUI(ready, total);
}

///////////////////////////////////////////////////////////////////////////////
void BP_LobbyController::CheckAndLaunchGame(void)
{
    if (!IsAuthority()) { return; }

    // All players must be ready AND we need at least 1 player
    if (readyPlayers() == totalPlayers() && totalPlayers() > 0)
    {
        FLogger::Info("[Lobby Server] All players ready! Launching game...");

        // Change to the first level
        World::WithWorld([](UWorld& world)
                         { world.ChangeLevel("L_RType_Level1"); });
    }
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_LobbyController, AActor)

}   // namespace tkd
