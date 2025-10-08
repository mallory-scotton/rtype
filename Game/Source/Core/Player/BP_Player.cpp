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
BP_Player::BP_Player(void)
    : APawn()
    , speed(*this, "Speed", 200.0f)
    , velocity(*this, "Velocity", FVector2f::Zero)
    , m_lastVelocity(FVector2f::Zero)
    , m_lastFiredTime(0.0f)
{
    auto SpriteComponent =
        AddComponent<UAnimatedSpriteComponent>("AnimatedSpriteComponent");
    SpriteComponent->SetTexturePath("Game/Assets/Images/T_PlayerShips.png");

    // Define IDLE animation
    FAnimation2D A_Idle("IDLE", true);
    A_Idle.AddFrame(FRectanglei(66, 0, 33, 17), 1.f);
    SpriteComponent->AddAnimation(A_Idle);

    // Define IDLE_TO_FLY_UP animation
    FAnimation2D A_IdleToFlyUp("IDLE_TO_FLY_UP", false);
    A_IdleToFlyUp.AddFrame(FRectanglei(33, 0, 33, 17), 0.1f);
    A_IdleToFlyUp.AddFrame(FRectanglei(0, 0, 33, 17), 1.0f);
    SpriteComponent->AddAnimation(A_IdleToFlyUp);

    // Define FLY_UP_TO_IDLE animation
    FAnimation2D A_FlyUpToIdle("FLY_UP_TO_IDLE", false);
    A_FlyUpToIdle.AddFrame(FRectanglei(33, 0, 33, 17), 0.1f);
    A_FlyUpToIdle.AddFrame(FRectanglei(66, 0, 33, 17), 1.0f);
    SpriteComponent->AddAnimation(A_FlyUpToIdle);

    // Define IDLE_TO_FLY_DOWN animation
    FAnimation2D A_IdleToFlyDown("IDLE_TO_FLY_DOWN", false);
    A_IdleToFlyDown.AddFrame(FRectanglei(99, 0, 33, 17), 0.1f);
    A_IdleToFlyDown.AddFrame(FRectanglei(132, 0, 33, 17), 1.0f);
    SpriteComponent->AddAnimation(A_IdleToFlyDown);

    // Define FLY_DOWN_TO_IDLE animation
    FAnimation2D A_FlyDownToIdle("FLY_DOWN_TO_IDLE", false);
    A_FlyDownToIdle.AddFrame(FRectanglei(99, 0, 33, 17), 0.1f);
    A_FlyDownToIdle.AddFrame(FRectanglei(66, 0, 33, 17), 1.0f);
    SpriteComponent->AddAnimation(A_FlyDownToIdle);

    // Set the default animation to IDLE
    SpriteComponent->Play("Idle");

    // Local transform to scale up the sprite
    SpriteComponent->SetLocalTransform(
        FTransform2D(FVector2f::Zero, 0.0f, FVector2f(2.0f, 2.0f))
    );
}

///////////////////////////////////////////////////////////////////////////////
void BP_Player::Tick(Float32 deltaTime)
{
    // Call Super Tick
    Super::Tick(deltaTime);

    // Add time to last fired time
    m_lastFiredTime += deltaTime;

    auto SpriteComponent =
        GetComponent<UAnimatedSpriteComponent>("AnimatedSpriteComponent");

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
    if (velocity().y > 0.0f)
    {
        SpriteComponent->Play("IDLE_TO_FLY_UP", false);
    }
    else if (velocity().y < 0.0f)
    {
        SpriteComponent->Play("IDLE_TO_FLY_DOWN", false);
    }
    else
    {
        if (m_lastVelocity.y > 0.0f)
        {
            SpriteComponent->Play("FLY_UP_TO_IDLE", false);
        }
        else if (m_lastVelocity.y < 0.0f)
        {
            SpriteComponent->Play("FLY_DOWN_TO_IDLE", false);
        }
        else { SpriteComponent->Play("IDLE", true); }
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
