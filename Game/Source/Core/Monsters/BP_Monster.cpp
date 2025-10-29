///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Core/Monsters/BP_Monster.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
BP_Monster::BP_Monster(void)
    : AActor("BP_Monster")
    , speed(*this, "Speed", 3.0f)
    , velocity(*this, "Velocity", FVector2f::Zero)
    , roamRadius(*this, "RoamRadius", 120.0f)
    , m_targetPosition(FVector3(100.0f, 100.0f, 0.0f))
    , m_timeSinceTarget(0.0f)
{
    // Monster is not a pawn and does not need transform replication by default
    SetTransformReplicated(true);

    AddComponent<UBillboardComponent>("BC_MonsterSprite");
    AddComponent<UBoxCollisionComponent>("BoxCollision");
}

///////////////////////////////////////////////////////////////////////////////
void BP_Monster::BeginPlay(void)
{
    Super::BeginPlay();

    // Record spawn position as center of roaming
    auto Billboard = GetComponent<UBillboardComponent>("BC_MonsterSprite");
    if (Billboard)
    {
        Billboard->SetDisplayMode(UBillboardComponent::EDisplayMode::FlipBook);
        Billboard->SetFlipBook(&m_idleAnimation);
    }

    auto Box = GetComponent<UBoxCollisionComponent>("BoxCollision");
    if (Box)
    {
        Box->SetHiddenInGame(false);
        Box->SetBoxExtent(FVector3f(0.5f, 0.5f, 0.5f));
    }
}

///////////////////////////////////////////////////////////////////////////////
void BP_Monster::Tick(Float32 deltaTime)
{
    // Parent tick (handles components)
    Super::Tick(deltaTime);

    // Only the server/authority should drive AI movement
    if (!IsAuthority()) { return; }

    m_timeSinceTarget += deltaTime;

    auto Billboard = GetComponent<UBillboardComponent>("BC_MonsterSprite");

    if (!Billboard) { return; }

    FVector3 currentPosition = GetTransform().GetPosition();
    FVector3 toTarget = m_targetPosition - currentPosition;

    const Float32 dist2 = toTarget.x * toTarget.x + toTarget.y * toTarget.y +
                          toTarget.z * toTarget.z;
    const Float32 eps = 1e-4f;

    if (dist2 < eps)
    {
        // Reached target (or extremely close) — stop and pick a new one
        velocity = FVector2f::Zero;
        PickNewTarget();
        m_timeSinceTarget = 0.0f;
    }
    else
    {
        // Desired velocity (units/sec) towards target
        FVector3 dir = toTarget.Normalized();
        FVector3 desiredVel3 = dir * speed();

        // If this tick would overshoot the target, clamp velocity so we
        // exactly reach it
        const Float32 dist = Math<Float32>::Sqrt(dist2);
        const Float32 maxTravel = speed() * deltaTime;
        if (maxTravel >= dist && deltaTime > 0.0f)
        {
            // Set velocity so ApplyMovement moves exactly to the target this
            // frame
            desiredVel3 = toTarget / deltaTime;
        }

        // Apply movement and record 2D velocity for animation updates
        ApplyMovement(desiredVel3, deltaTime);
        velocity = FVector2f(desiredVel3.x, desiredVel3.y);
    }

    UpdateAnimationState();
}

///////////////////////////////////////////////////////////////////////////////
void BP_Monster::PickNewTarget(void)
{
    // TODO: add random or fix this
    //  Choose a random point within roamRadius circle around spawn
    //  Float32 r = Math<Float32>::Random() * roamRadius();
    //  Float32 angle = Math<Float32>::Random() * Math<Float32>::PI * 2.0f;
    /*FVector3 offset(
        r * Math<Float32>::Cos(angle), r *
     * Math<Float32>::Sin(angle), 0.0f
    );*/
    // m_targetPosition = m_spawnPosition + offset;
}

///////////////////////////////////////////////////////////////////////////////
void BP_Monster::UpdateAnimationState(void)
{
    auto Billboard = GetComponent<UBillboardComponent>("BC_MonsterSprite");

    if (!Billboard) { return; }

    if (velocity().y < 0.0f)
    {
        m_moveUpAnimation.SetPlaybackSpeed(1.f);
        Billboard->SetFlipBook(&m_moveUpAnimation);
    }
    else
    {
        if (velocity().x == 0.0f && velocity().y == 0.0f)
        {
            Billboard->SetFlipBook(&m_idleAnimation);
        }
        else { Billboard->SetFlipBook(&m_walkAnimation); }
    }
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_Monster, AActor)

}   // namespace tkd
