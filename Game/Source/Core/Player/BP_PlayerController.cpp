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
        BindActionPressed("Fire", [&player](EInput) {});

        BindAxis(
            "HorizontalMoves",
            [&player](float scale)
            {
                FTransform transform = player->GetTransform();
                transform.Translate(TVector3<float>(scale, 0.0f, 0.0f));
                player->SetTransform(transform);
                std::cout << "Player (" << player->GetObjectID()
                          << ") position: "
                          << player->GetTransform().GetPosition() << std::endl;
            }
        );

        BindAxis(
            "VerticalMoves",
            [&player](float scale)
            {
                FTransform transform = player->GetTransform();
                transform.Translate(TVector3<float>(0.0f, scale, 0.0f));
                player->SetTransform(transform);
                std::cout << "Player (" << player->GetObjectID()
                          << ") position: "
                          << player->GetTransform().GetPosition() << std::endl;
            }
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_PlayerController, APlayerController)

}   // namespace tkd
