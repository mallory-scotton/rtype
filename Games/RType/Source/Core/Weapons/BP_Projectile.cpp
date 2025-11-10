///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Core/Weapons/BP_Projectile.hpp>
#include <Core/Boss/BP_Boss.hpp>
#include <Core/Monsters/BP_Monster.hpp>
#include <Core/Player/BP_Player.hpp>

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
    AddComponent<UBoxCollisionComponent>("CC_ProjectileCollision");
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

    // Setup collision for the projectile
    auto Collision =
        GetComponent<UBoxCollisionComponent>("CC_ProjectileCollision");
    if (Collision)
    {
        Collision->SetHiddenInGame(false);
        Collision->SetBoxExtent(FVector3(0.40f, 0.40f, 0.40f));
        FTransform transform = Collision->GetLocalTransform();
        transform.SetPosition(FVector3(0.f, 0.f, 0.1f));
        Collision->SetLocalTransform(transform);

        auto* cs = Collision->GetCollisionSystem();
        if (cs)
        {
            cs->BindOnOverlapBegin(
                Collision,
                [this](const FCollisionInfo& info)
                {
                    // Safety checks - validate all pointers before use
                    if (!info.otherActor) { return; }
                    if (!info.otherComponent) { return; }

                    // Check if either actor is already marked for deletion
                    if (info.otherActor->IsMarkedForDeletion()) { return; }
                    if (this->IsMarkedForDeletion()) { return; }

                    // Don't collide with players
                    if (info.otherActor->Is<BP_Player>()) { return; }

                    // Only destroy monsters
                    if (info.otherActor->Is<BP_Monster>())
                    {
                        // Mark both for deletion
                        info.otherActor->MarkForDeletion();
                        this->MarkForDeletion();
                    }
                    if (info.otherActor->Is<BP_Boss>())
                    {
                        auto boss = info.otherActor->As<BP_Boss>();
                        boss->TakeDamage(1);
                        this->MarkForDeletion();
                    }
                }
            );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void BP_Projectile::Tick(Float32 deltaTime)
{
    // Early exit if marked for deletion
    if (IsMarkedForDeletion()) { return; }

    // Call Super Tick
    Super::Tick(deltaTime);

    // Move the projectile forward
    Translate(FVector3(8.0f * deltaTime, 0.0f, 0.0f));

    // If the projectile goes out of bounds, destroy it
    if (GetTransform().GetPosition().x > 200.0f) { MarkForDeletion(); }
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_Projectile, AActor)

}   // namespace tkd
