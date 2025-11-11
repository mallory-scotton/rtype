///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Core/Boss/BP_Boss.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

BP_Boss::BP_Boss(const UUID& uuid)
    : AActor("BP_Boss")
    , attackRange(*this, "AttackRange", 2.0f)
    , health(*this, "Health", 100)
    , attackCooldown(*this, "AttackCooldown", 2.0f)
    , m_attackTimer(0.0f)
    , m_isAttacking(false)
    , m_isDead(false)
{
    SetUUID(uuid);
#if TKD_ENGINE_CLIENT
    SetNetRole(ENetRole::SimulatedProxy);
#else
    SetNetRole(ENetRole::Authority);
#endif
    SetTransformReplicated(true);

    AddComponent<UBillboardComponent>("BC_BossSprite");
    AddComponent<UBoxCollisionComponent>("BoxCollision");
}

void BP_Boss::BeginPlay(void)
{
    Super::BeginPlay();

    auto Billboard = GetComponent<UBillboardComponent>("BC_BossSprite");
    if (Billboard)
    {
        Billboard->SetDisplayMode(UBillboardComponent::EDisplayMode::FlipBook);
        Billboard->SetFlipBook(&m_idleAnimation);
        FTransform t = Billboard->GetLocalTransform();
        t.SetPosition(FVector3(6.0f, -1.0f, -0.1f));
        t.SetScale(FVector3(1.5f, 1.5f, 1.0f));
        SetTransform(t);
    }

    auto Box = GetComponent<UBoxCollisionComponent>("BoxCollision");
    if (Box)
    {
        Box->SetHiddenInGame(true);
        Box->SetBoxExtent(FVector3f(1.5f, 2.5f, 1.5f));
    }
}

void BP_Boss::Tick(Float32 deltaTime)
{
    Super::Tick(deltaTime);

    if (IsMarkedForDeletion() || m_isDead) { return; }

    // Simple attack cooldown timer
    if (m_isAttacking)
    {
        m_attackTimer += deltaTime;
        if (m_attackTimer >= attackCooldown())
        {
            m_isAttacking = false;
            m_attackTimer = 0.0f;
        }
    }
}

void BP_Boss::StartAttack(void)
{
    if (m_isDead) { return; }
    m_isAttacking = true;
    m_attackTimer = 0.0f;
}

void BP_Boss::TakeDamage(Int32 amount)
{
    if (m_isDead) { return; }
    health = health() - amount;
    if (health() <= 0)
    {
        m_isDead = true;
        // optionally play death flipbook here (reuse idle/move as placeholder)
        auto Billboard = GetComponent<UBillboardComponent>("BC_BossSprite");
        if (Billboard)
        {
            // Billboard->SetFlipBook(
            //     &m_moveUpAnimation
            // );   // placeholder for death
        }
        this->MarkForDeletion();
    }
}

IMPLEMENT_CLASS_WITH_SUPER(BP_Boss, AActor)

}   // namespace tkd
