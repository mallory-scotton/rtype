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
BP_Monster::BP_Monster(UInt32 row)
    : AActor("BP_Monster")
    , speed(*this, "Speed", 60.0f)
    , roamRadius(*this, "RoamRadius", 120.0f)
    , m_targetPosition(FVector3::Zero)
    , m_timeSinceTarget(0.0f)
{
    // Monster is not a pawn and does not need transform replication by default
    SetTransformReplicated(false);

    // Create animated sprite component and point it to the shared sprite
    auto Abp = AddComponent<UAnimatedSpriteComponent>("ABP_MonsterSprite");
    Abp->SetTexturePath("Assets/Images/r-typesheet7.png");
    Abp->SetLocalTransform(
        FTransform2D(FVector2f::Zero, 0.0f, FVector2f(1.5f, 1.5f))
    );

    // Setup animations using the same frame sizing as player
    SetupAnimations();

    // Pick an initial target (BeginPlay will re-center on spawn)
    PickNewTarget();
}

///////////////////////////////////////////////////////////////////////////////
void BP_Monster::BeginPlay(void)
{
    Super::BeginPlay();

    // Record spawn position as center of roaming
    m_spawnPosition = GetTransform().GetPosition();

    // Ensure a target exists inside roam radius
    PickNewTarget();
}

///////////////////////////////////////////////////////////////////////////////
void BP_Monster::Tick(Float32 deltaTime)
{
    // Parent tick (handles components)
    Super::Tick(deltaTime);

    // Only the server/authority should drive AI movement
    if (!IsAuthority()) { return; }

    m_timeSinceTarget += deltaTime;

    // Occasionally pick a new target (every ~1-3 seconds) or if close to
    // current
    if (/*m_timeSinceTarget > (1.0f + Math<Float32>::Random() * 2.0f) || */
        (GetTransform().GetPosition() - m_targetPosition).Length() < 4.0f)
    {
        PickNewTarget();
        m_timeSinceTarget = 0.0f;
    }

    // Move toward target
    FVector3 current = GetTransform().GetPosition();
    FVector3 toTarget = m_targetPosition - current;
    Float32 dist = toTarget.Length();
    if (dist > 0.01f)
    {
        FVector3 dir = toTarget / dist;
        FVector3 movement = dir * speed() * deltaTime;
        if (movement.Length() > dist) { movement = dir * dist; }

        FTransform t = GetTransform();
        t.Translate(movement);
        SetTransform(t);
    }

    // Play idle/walk animation based on movement distance
    auto Abp = GetComponent<UAnimatedSpriteComponent>("ABP_MonsterSprite");
    if (Abp)
    {
        if (dist > 2.0f) { Abp->Play("Walk", true); }
        else { Abp->Play("Idle", true); }
    }
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
void BP_Monster::SetupAnimations(void)
{
    auto Abp = GetComponent<UAnimatedSpriteComponent>("ABP_MonsterSprite");
    if (!Abp) { return; }

    Abp->ClearAnimations();

    // The provided sprite sheet re-uses the player sizing: 33x33 per frame
    const Int32 frameW = 33;
    const Int32 frameH = 33;

    FAnimation2D idle("Idle", true);
    idle.AddFrame(FRectanglei(frameW * 2, frameH, frameW, frameH), 1.0f);
    Abp->AddAnimation(idle);

    FAnimation2D walk("Walk", true);
    walk.AddFrame(FRectanglei(frameW * 0, frameH, frameW, frameH), 0.15f);
    walk.AddFrame(FRectanglei(frameW * 1, frameH, frameW, frameH), 0.15f);
    Abp->AddAnimation(walk);

    FAnimation2D jump("Jump", false);
    jump.AddFrame(FRectanglei(frameW * 0, frameH * 2, frameW, frameH), 0.2f);
    jump.AddFrame(FRectanglei(frameW * 1, frameH * 2, frameW, frameH), 0.2f);
    jump.AddFrame(FRectanglei(frameW * 2, frameH * 2, frameW, frameH), 0.2f);
    Abp->AddAnimation(jump);

    Abp->Play("Idle");
}

///////////////////////////////////////////////////////////////////////////////
void BP_Monster::UpdateAnimationState(void)
{
    // Update the animation state based on movement
    FVector3 velocity = GetTransform().GetPosition() - m_spawnPosition;
    if (velocity.Length() > 0.01f)
    {
        // If moving, play walk animation
        auto Abp = GetComponent<UAnimatedSpriteComponent>("ABP_MonsterSprite");
        if (Abp) { Abp->Play("Walk", true); }
    }
    else
    {
        // If not moving, play idle animation
        auto Abp = GetComponent<UAnimatedSpriteComponent>("ABP_MonsterSprite");
        if (Abp) { Abp->Play("Idle", true); }
    }
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_Monster, AActor)

}   // namespace tkd
