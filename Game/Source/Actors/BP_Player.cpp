///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Actors/BP_Player.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
BP_Player::BP_Player(void)
    : APawn()
    , speed(*this, "Speed", 600.0f)
{
    auto SpriteComponent = AddComponent<USpriteComponent>("SpriteComponent");
    SpriteComponent->SetTexturePath("Game/Assets/Images/T_PlayerShips.png");
}

///////////////////////////////////////////////////////////////////////////////
void BP_Player::Tick(Float32 deltaTime)
{
    TKD_UNUSED(deltaTime);
    // auto hAxis = Engine::Inputs.GetAxis("HorizontalMoves");
    // auto vAxis = Engine::Inputs.GetAxis("VerticalMoves");
    // auto fire = Engine::Inputs.GetAction("Fire");

    // if (hAxis)
    // {
    //     float scale = hAxis->GetScale();
    //     if (scale != 0.0f)
    //     {
    //         FTransform transform = GetTransform();
    //         FVector3 position = transform.GetPosition();
    //         position.x += scale * speed * deltaTime;
    //         transform.SetPosition(position);
    //         SetTransform(transform);
    //     }
    // }

    // if (vAxis)
    // {
    //     float scale = vAxis->GetScale();
    //     if (scale != 0.0f)
    //     {
    //         FTransform transform = GetTransform();
    //         FVector3 position = transform.GetPosition();
    //         position.y += scale * speed * deltaTime;
    //         transform.SetPosition(position);
    //         SetTransform(transform);
    //     }
    // }

    // if (fire)
    // {
    //     // Process fire action input
    // }
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_Player, APawn)

}   // namespace tkd
