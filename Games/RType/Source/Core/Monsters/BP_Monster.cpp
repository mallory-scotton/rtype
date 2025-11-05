///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Core/Monsters/BP_Monster.hpp>
#include <Core/Weapons/BP_Projectile.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
BP_Monster::BP_Monster(const UUID& uuid)
    : AActor("BP_Monster")
    , speed(*this, "Speed", 1.25f)
    , velocity(*this, "Velocity", FVector2f::Zero)
    , roamRadius(*this, "RoamRadius", 4.0f)
    , waitTime(*this, "WaitTime", 1.0f)
    , m_targetPosition(FVector3(2.f, 2.f, 0.2f))
    , m_timeSinceTarget(0.0f)
    , m_waitRemaining(0.0f)
    , MulticastPos(
          *this,
          "RPC_MulticastPos",
          ERPCType::Multicast,
          std::bind(
              &BP_Monster::RPC_MulticastPos,
              this,
              std::placeholders::_1,
              std::placeholders::_2
          ),
          true
      )
{
    // Ensure the monster has the provided UUID instead of the default one.
    SetUUID(uuid);

    // TODO: Implement network role assignment
#if TKD_ENGINE_CLIENT
    SetNetRole(ENetRole::SimulatedProxy);
#else
    SetNetRole(ENetRole::Authority);
#endif
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
        FTransform t = Billboard->GetLocalTransform();
        t.SetScale(FVector3(0.5f, 0.5f, 1.0f));
        Billboard->SetLocalTransform(t);
    }

    auto Box = GetComponent<UBoxCollisionComponent>("BoxCollision");
    if (Box)
    {
        Box->SetHiddenInGame(false);
        Box->SetBoxExtent(FVector3f(0.25f, 0.25f, 0.25f));
        FTransform transform = Box->GetLocalTransform();
        Box->SetLocalTransform(transform);
    }
}

///////////////////////////////////////////////////////////////////////////////
void BP_Monster::Tick(Float32 deltaTime)
{
    Super::Tick(deltaTime);

    if (IsAuthority())
    {
        m_timeSinceTarget += deltaTime;

        // Move towards the target position
        FVector3 currentPosition = GetTransform().GetPosition();
        FTransform serverTransform =
            SimulateMovement(FVector3::Zero, deltaTime, GetTransform());
        FVector3 toTarget = m_targetPosition - currentPosition;

        const Float32 dist2 = toTarget.x * toTarget.x +
                              toTarget.y * toTarget.y +
                              toTarget.z * toTarget.z;
        const Float32 eps = 1e-4f;

        if (dist2 < eps)
        {
            // We're at the target: stop and handle wait timer
            velocity = FVector2f::Zero;

            if (m_waitRemaining <= 0.0f) { m_waitRemaining = waitTime(); }

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

            // Update replicated velocity property (clients can use this for
            // animation/extrapolation)
            velocity = FVector2f(desiredVel3.x, desiredVel3.y);

            // Simulate movement on the server and replicate to clients by
            // calling the multicast RPC. This ensures the engine's
            // interpolation/extrapolation paths are used on simulated
            // proxies.
            FTransform startTransform = GetTransform();
            serverTransform =
                SimulateMovement(desiredVel3, deltaTime, startTransform);

            // Apply authoritative transform on server
            SetTransform(serverTransform);
        }
        MulticastPos(serverTransform, velocity);
    }

    UpdateAnimationState();
}

///////////////////////////////////////////////////////////////////////////////
void BP_Monster::RPC_MulticastPos(FTransform pos, FVector2f vel)
{
    // Play target update on all clients,
    if (IsAuthority()) { return; }

    SetTransform(pos);
    velocity = vel;
    if (vel.x != 0.0f) { m_lastXVelocity = vel.x; }
}

///////////////////////////////////////////////////////////////////////////////
void BP_Monster::PickNewTarget(Float32 deltaTime)
{
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
    case 0 : offset = FVector3(r, 0.0f, 0.2f); break;
    case 1 : offset = FVector3(r * KD, r * KD, 0.2f); break;
    case 2 : offset = FVector3(0.0f, r, 0.2f); break;
    case 3 : offset = FVector3(-r * KD, r * KD, 0.2f); break;
    case 4 : offset = FVector3(-r, 0.0f, 0.2f); break;
    case 5 : offset = FVector3(-r * KD, -r * KD, 0.2f); break;
    case 6 : offset = FVector3(0.0f, -r, 0.2f); break;
    default: offset = FVector3(r * KD, -r * KD, 0.2f); break;
    }

    // Final target is relative to the spawn position, guaranteeing the
    // monster stays within roamRadius of where it spawned.
    m_targetPosition = spawnPos + offset;
}

///////////////////////////////////////////////////////////////////////////////
FTransform BP_Monster::SimulateMovement(
    const FVector3& inputVector,
    Float32 deltaTime,
    const FTransform& startTransform
)
{
    FTransform result = startTransform;

    auto box = GetComponent<UBoxCollisionComponent>("BoxCollision");

    // Update velocity property for client-side animation/extrapolation
    if (inputVector.Length() > 0.0f)
    {
        velocity = FVector2f(inputVector.x, inputVector.y);

        FVector3 movement = inputVector * deltaTime;
        result.Translate(movement);
        box->SetLocalTransform(result);
    }
    else { velocity = FVector2f::Zero; }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
void BP_Monster::UpdateAnimationState(void)
{
    auto Billboard = GetComponent<UBillboardComponent>("BC_MonsterSprite");

    if (!Billboard) { return; }

    if (m_lastXVelocity > 0.0f)
    {
        if (velocity().y < 0.0f)
        {
            m_moveUpAnimationInverse.SetPlaybackSpeed(1.f);
            Billboard->SetFlipBook(&m_moveUpAnimationInverse);
        }
        else
        {
            if (velocity().x == 0.0f && velocity().y == 0.0f)
            {
                Billboard->SetFlipBook(&m_idleAnimationInverse);
            }
            else { Billboard->SetFlipBook(&m_walkAnimationInverse); }
        }
    }
    else
    {
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
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_Monster, AActor)

}   // namespace tkd
