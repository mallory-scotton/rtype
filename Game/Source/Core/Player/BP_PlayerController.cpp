///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Core/Player/BP_PlayerController.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
BP_PlayerController::BP_PlayerController()
    : APlayerController()
{}

///////////////////////////////////////////////////////////////////////////////
void BP_PlayerController::SetupInputBindings(void)
{
    APawn* pawn = GetPawn();

    if (pawn == nullptr) { return; }

    if (BP_Player* player = dynamic_cast<BP_Player*>(pawn))
    {
        // Fire button
        BindActionPressed(
            "Fire",
            [player](EInput input)
            {
                if (player->IsLocallyControlled())
                {
                    // Send RPC to server
                    player->Fire();
                }
            }
        );

        // Test speed replication button (Z key)
        BindActionPressed(
            "TestSpeed",
            [player](EInput input)
            {
                if (player->IsLocallyControlled())
                {
                    player->TestSpeedReplication();
                }
            }
        );

        // Movement axes - handle in Tick for smooth prediction
        // (See below)
    }
}

///////////////////////////////////////////////////////////////////////////////
void BP_PlayerController::Tick(Float32 deltaTime)
{
    Super::Tick(deltaTime);

    BP_Player* player = dynamic_cast<BP_Player*>(GetPawn());
    if (!player || !player->IsLocallyControlled()) { return; }

    // Get input values
    FVector2f inputVelocity = FVector2f::Zero;

    // Retrieve input from the input manager
    if (auto* inputManager = GetInputManager())
    {
        inputVelocity.x = inputManager->GetAxisValue("HorizontalMoves");
        inputVelocity.y = inputManager->GetAxisValue("VerticalMoves");
    }

    // Use client-side prediction
    // player->ClientPredictMove(deltaTime, inputVelocity);
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_PlayerController, APlayerController)

}   // namespace tkd
