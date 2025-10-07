///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Core/Weapons/BP_Projectile.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
BP_Projectile::BP_Projectile(void)
    : AActor("BP_Projectile")
{
    auto SpriteComponent =
        AddComponent<UAnimatedSpriteComponent>("AnimatedSpriteComponent");
    SpriteComponent->SetTexturePath("Game/Assets/Images/T_Projectiles.png");

    // Define PROJECTILE animation
    FAnimation2D A_PROJECTILE("PROJECTILE", false);
    A_PROJECTILE.AddFrame(
        FAnimation2DFrame(FRectanglei(233, 17, 34, 34), 0.15f)
    );
    A_PROJECTILE.AddFrame(
        FAnimation2DFrame(FRectanglei(199, 17, 34, 34), 0.15f)
    );
    A_PROJECTILE.AddFrame(
        FAnimation2DFrame(FRectanglei(165, 17, 34, 34), 0.15f)
    );
    A_PROJECTILE.AddFrame(
        FAnimation2DFrame(FRectanglei(131, 17, 34, 34), 0.15f)
    );
    SpriteComponent->AddAnimation(A_PROJECTILE);

    // Set the default animation to PROJECTILE
    SpriteComponent->Play("PROJECTILE", true);
}

///////////////////////////////////////////////////////////////////////////////
void BP_Projectile::Tick(Float32 deltaTime)
{
    // Call Super Tick
    Super::Tick(deltaTime);

    // Move the projectile forward
    FTransform transform = GetTransform();
    transform.Translate(FVector3(800.0f * deltaTime, 0.0f, 0.0f));
    SetTransform(transform);

    // If the projectile goes out of bounds, destroy it
    if (transform.GetPosition().x > 2000.0f) { MarkForDeletion(); }
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_Projectile, AActor)

}   // namespace tkd
