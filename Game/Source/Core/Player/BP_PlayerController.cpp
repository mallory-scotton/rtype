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
        BindActionPressed("Fire", [player](EInput) {});

        BindAxis(
            "HorizontalMoves",
            [player](float scale)
            {
                auto velocity = player->velocity.GetValue();
                player->velocity = FVector2f(scale, velocity.y);
            }
        );

        BindAxis(
            "VerticalMoves",
            [player](float scale)
            {
                auto velocity = player->velocity.GetValue();
                player->velocity = FVector2f(velocity.x, scale);
            }
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_PlayerController, APlayerController)

}   // namespace tkd
