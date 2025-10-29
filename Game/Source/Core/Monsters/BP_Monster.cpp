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
    , speed(*this, "Speed", 1.5f)
    , velocity(*this, "Velocity", FVector2f::Zero)
    , roamRadius(*this, "RoamRadius", 4.0f)
    , waitTime(*this, "WaitTime", 0.5f)
    , m_targetPosition(FVector3(2.f, 2.f, 0.0f))
    , m_timeSinceTarget(0.0f)
    , m_waitRemaining(0.0f)
{
    // Monster is not a pawn and does not need transform replication by default
    SetTransformReplicated(false);

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
        Box->SetBoxExtent(FVector3f(0.25f, 0.25f, 0.25f));
    }
}

///////////////////////////////////////////////////////////////////////////////
void BP_Monster::Tick(Float32 deltaTime)
{
    // Parent tick (handles components)
    Super::Tick(deltaTime);

    // Only the server/authority should drive AI movement. If this actor
    // runs on a client and authority-checking is enabled, you'd early-return
    // here. For now we keep movement local for testing.
    // TODO: re-enable authority check
    if (!IsAuthority()) { return; }

    m_timeSinceTarget += deltaTime;

    auto Billboard = GetComponent<UBillboardComponent>("BC_MonsterSprite");
    if (!Billboard) { return; }

    // Move towards the target position
    FVector3 currentPosition = GetTransform().GetPosition();
    FVector3 toTarget = m_targetPosition - currentPosition;

    const Float32 dist2 = toTarget.x * toTarget.x + toTarget.y * toTarget.y +
                          toTarget.z * toTarget.z;
    const Float32 eps = 1e-4f;

    if (dist2 < eps)
    {
        // Reached target (or extremely close) — stop and wait a bit before
        // picking a new target. This avoids jitter and makes movement feel
        // more natural.
        velocity = FVector2f::Zero;

        // If we haven't started waiting yet, initialize the wait timer
        if (m_waitRemaining <= 0.0f) { m_waitRemaining = waitTime(); }

        // Count down the wait timer. When it elapses, pick a new target and
        // reset the accumulated time used for seeding the next pick.
        m_waitRemaining -= deltaTime;
        if (m_waitRemaining <= 0.0f)
        {
            PickNewTarget(deltaTime);
            m_timeSinceTarget = 0.0f;
            m_waitRemaining = 0.0f;
        }
    }
    else
    {
        // Desired velocity (units/sec) towards target
        FVector3 dir = toTarget.Normalized();
        FVector3 desiredVel3 = dir * speed();

        // If this tick would overshoot the target, clamp velocity so we
        // exactly reach it this frame
        const Float32 dist = Math<Float32>::Sqrt(dist2);
        const Float32 maxTravel = speed() * deltaTime;
        if (maxTravel >= dist && deltaTime > 0.0f)
        {
            desiredVel3 = toTarget / deltaTime;
        }
        velocity = FVector2f(desiredVel3.x, desiredVel3.y);

        // Apply movement by updating the actor transform directly
        FVector3 displacement = desiredVel3 * deltaTime;
        FVector3 newPosition = currentPosition + displacement;
        FTransform newTransform = GetTransform();
        newTransform.SetPosition(newPosition);
        SetTransform(newTransform);
    }

    UpdateAnimationState();
}

///////////////////////////////////////////////////////////////////////////////
void BP_Monster::PickNewTarget(Float32 deltaTime)
{
    // Choose a deterministic pseudo-random point within roamRadius around
    // the actor's spawn position so the monster never strays too far.

    // Ensure we have a recorded spawn position. If m_spawnPosition hasn't
    // been set yet (default zero), fall back to current transform and
    // record it for future picks.
    if (m_spawnPosition == FVector3::Zero)
    {
        m_spawnPosition = GetTransform().GetPosition();
    }
    FVector3 spawnPos = m_spawnPosition;

    // Build a simple seed from accumulated time so the choice changes over
    // time
    Float32 seed = (m_timeSinceTarget + deltaTime) * 1000.0f;
    int idx = static_cast<int>(seed) & 7;   // 8 possible directions (0..7)
    Float32 frac = seed - static_cast<int>(seed);   // fractional part in [0,1)

    if (frac < 0.0f) { frac = -frac; }   // ensure positive fractional part

    // radius in [0.25*roamRadius, 1.0*roamRadius] to avoid zero-length moves
    Float32 r = roamRadius() * (0.25f + 0.75f * frac);

    // Precomputed diagonal component (1/sqrt(2)) to avoid trig
    const Float32 KD = 0.70710678f;

    FVector3 offset;
    switch (idx)
    {
    case 0 : offset = FVector3(r, 0.0f, 0.0f); break;
    case 1 : offset = FVector3(r * KD, r * KD, 0.0f); break;
    case 2 : offset = FVector3(0.0f, r, 0.0f); break;
    case 3 : offset = FVector3(-r * KD, r * KD, 0.0f); break;
    case 4 : offset = FVector3(-r, 0.0f, 0.0f); break;
    case 5 : offset = FVector3(-r * KD, -r * KD, 0.0f); break;
    case 6 : offset = FVector3(0.0f, -r, 0.0f); break;
    default: offset = FVector3(r * KD, -r * KD, 0.0f); break;
    }

    // Final target is relative to the spawn position, guaranteeing the
    // monster stays within roamRadius of where it spawned.
    m_targetPosition = spawnPos + offset;
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
