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
    // Remove transform replication for projectiles
    SetTransformReplicated(false);

    // Add flipbook component
    AddComponent<UBillboardComponent>("BC_ProjectileSprite");
}

///////////////////////////////////////////////////////////////////////////////
void BP_Projectile::BeginPlay(void)
{
    // Call parent BeginPlay first
    Super::BeginPlay();

    // Set the flipbook for the projectile's billboard component
    auto Billboard = GetComponent<UBillboardComponent>("BC_ProjectileSprite");
    if (Billboard)
    {
        Billboard->SetDisplayMode(UBillboardComponent::EDisplayMode::FlipBook);
        Billboard->SetFlipBook(&m_flipBook);
    }
}

///////////////////////////////////////////////////////////////////////////////
void BP_Projectile::Tick(Float32 deltaTime)
{
    // Call Super Tick
    Super::Tick(deltaTime);

    // Move the projectile forward
    Translate(FVector3(8.0f * deltaTime, 0.0f, 0.0f));

    // If the projectile goes out of bounds, destroy it
    if (GetTransform().GetPosition().x > 2000.0f) { MarkForDeletion(); }
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_Projectile, AActor)

}   // namespace tkd
