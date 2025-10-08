///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Core/Player/BP_Player.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
BP_Player::BP_Player(UInt32 playerColor)
    : APawn()
    , speed(*this, "Speed", 200.0f)
    , velocity(*this, "Velocity", FVector2f::Zero)
    , playerColor(*this, "PlayerColor", playerColor % 5)
    , m_lastVelocity(FVector2f::Zero)
    , m_lastFiredTime(0.0f)
{
    auto Abp = AddComponent<UAnimatedSpriteComponent>("ABP_PlayerSprite");
    Abp->SetTexturePath("Assets/Images/T_PlayerShips.png");

    // Set up animations
    SetupAnimations();

    // Local transform to scale up the sprite
    Abp->SetLocalTransform(
        FTransform2D(FVector2f::Zero, 0.0f, FVector2f(2.0f, 2.0f))
    );
}

///////////////////////////////////////////////////////////////////////////////
void BP_Player::SetupAnimations(void)
{
    // Get the animated sprite component
    auto Abp = GetComponent<UAnimatedSpriteComponent>("ABP_PlayerSprite");

    // Early out if no sprite component
    if (!Abp) { return; }

    // Clear the existing animations
    Abp->ClearAnimations();

    // Define Idle animation
    FAnimation2D A_Idle("Idle", true);
    A_Idle.AddFrame(FRectanglei(66, playerColor * 17, 33, 17), 1.f);
    Abp->AddAnimation(A_Idle);

    // Define IdleToFlyUp animation
    FAnimation2D A_IdleToFlyUp("IdleToFlyUp", false);
    A_IdleToFlyUp.AddFrame(FRectanglei(33, playerColor * 17, 33, 17), 0.1f);
    A_IdleToFlyUp.AddFrame(FRectanglei(0, playerColor * 17, 33, 17), 1.0f);
    Abp->AddAnimation(A_IdleToFlyUp);

    // Define FlyUpToIdle animation
    FAnimation2D A_FlyUpToIdle("FlyUpToIdle", false);
    A_FlyUpToIdle.AddFrame(FRectanglei(33, playerColor * 17, 33, 17), 0.1f);
    A_FlyUpToIdle.AddFrame(FRectanglei(66, playerColor * 17, 33, 17), 1.0f);
    Abp->AddAnimation(A_FlyUpToIdle);

    // Define IdleToFlyDown animation
    FAnimation2D A_IdleToFlyDown("IdleToFlyDown", false);
    A_IdleToFlyDown.AddFrame(FRectanglei(99, playerColor * 17, 33, 17), 0.1f);
    A_IdleToFlyDown.AddFrame(FRectanglei(132, playerColor * 17, 33, 17), 1.0f);
    Abp->AddAnimation(A_IdleToFlyDown);

    // Define FlyDownToIdle animation
    FAnimation2D A_FlyDownToIdle("FlyDownToIdle", false);
    A_FlyDownToIdle.AddFrame(FRectanglei(99, playerColor * 17, 33, 17), 0.1f);
    A_FlyDownToIdle.AddFrame(FRectanglei(66, playerColor * 17, 33, 17), 1.0f);
    Abp->AddAnimation(A_FlyDownToIdle);

    // Set the default animation to IDLE
    Abp->Play("Idle");
}

///////////////////////////////////////////////////////////////////////////////
void BP_Player::Tick(Float32 deltaTime)
{
    // Call Super Tick
    Super::Tick(deltaTime);

    // Add time to last fired time
    m_lastFiredTime += deltaTime;

    auto Abp = GetComponent<UAnimatedSpriteComponent>("ABP_PlayerSprite");

    // Move player based on velocity and speed
    if (velocity() != 0.0f)
    {
        // Normalize velocity to ensure consistent speed in all directions
        if (velocity().Length() != 0.0f)
        {
            velocity = velocity().Normalized();
        }

        // Update transform
        Translate(FVector3(
            velocity->x * (speed * 1.5f) * deltaTime,
            velocity->y * speed * deltaTime,
            0.0f
        ));
    }

    // Update animation state
    if (velocity().y > 0.0f) { Abp->Play("IdleToFlyUp", false); }
    else if (velocity().y < 0.0f) { Abp->Play("IdleToFlyDown", false); }
    else
    {
        if (m_lastVelocity.y > 0.0f) { Abp->Play("FlyUpToIdle", false); }
        else if (m_lastVelocity.y < 0.0f)
        {
            Abp->Play("FlyDownToIdle", false);
        }
        else { Abp->Play("Idle", true); }
    }

    // Update last velocity if there is movement
    if (velocity() != FVector2f::Zero) { m_lastVelocity = velocity; }

    // Reset velocity for next frame
    velocity = FVector2f::Zero;
}

///////////////////////////////////////////////////////////////////////////////
void BP_Player::Fire(void)
{
    if (m_lastFiredTime >= 0.25f)
    {
        // Reset last fired time
        m_lastFiredTime = 0.0f;

        // Get player transform
        FTransform transform = GetTransform();
        transform.SetRotation(FRotator(0.f, 0.f, 0.f));
        transform.SetScale(FVector3f::One);

        // Spawn a projectile
        World::SpawnActor("BP_Projectile", transform);
    }
}

///////////////////////////////////////////////////////////////////////////////
void BP_Player::MoveHorizontal(Float32 value)
{
    value = Math<float>::Clamp(value, -1.0f, 1.0f);
    velocity->x += value;
}

///////////////////////////////////////////////////////////////////////////////
void BP_Player::MoveVertical(Float32 value)
{
    value = Math<float>::Clamp(value, -1.0f, 1.0f);
    velocity->y += value;
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_Player, APawn)

}   // namespace tkd
