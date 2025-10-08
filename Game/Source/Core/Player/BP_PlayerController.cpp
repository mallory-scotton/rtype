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
        BindActionPressed("Fire", std::bind(&BP_Player::Fire, player));

        BindAxis(
            "HorizontalMoves",
            std::bind(
                &BP_Player::MoveHorizontal, player, std::placeholders::_1
            )
        );

        BindAxis(
            "VerticalMoves",
            std::bind(&BP_Player::MoveVertical, player, std::placeholders::_1)
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_PlayerController, APlayerController)

}   // namespace tkd
