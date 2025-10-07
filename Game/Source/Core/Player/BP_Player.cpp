///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Core/Player/BP_Player.hpp>
#include <Core/Weapons/BP_Projectile.hpp>

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
    FAnimation2D A_IDLE("IDLE", true);
    A_IDLE.AddFrame(FAnimation2DFrame(FRectanglei(66, 0, 33, 17), 1.f));
    SpriteComponent->AddAnimation(A_IDLE);

    // Define IDLE_TO_FLY_UP animation
    FAnimation2D A_ITFU("IDLE_TO_FLY_UP", false);
    A_ITFU.AddFrame(FAnimation2DFrame(FRectanglei(33, 0, 33, 17), 0.1f));
    A_ITFU.AddFrame(FAnimation2DFrame(FRectanglei(0, 0, 33, 17), 1.0f));
    SpriteComponent->AddAnimation(A_ITFU);

    // Define FLY_UP_TO_IDLE animation
    FAnimation2D A_FUTI("FLY_UP_TO_IDLE", false);
    A_FUTI.AddFrame(FAnimation2DFrame(FRectanglei(33, 0, 33, 17), 0.1f));
    A_FUTI.AddFrame(FAnimation2DFrame(FRectanglei(66, 0, 33, 17), 1.0f));
    SpriteComponent->AddAnimation(A_FUTI);

    // Define IDLE_TO_FLY_DOWN animation
    FAnimation2D A_ITFD("IDLE_TO_FLY_DOWN", false);
    A_ITFD.AddFrame(FAnimation2DFrame(FRectanglei(99, 0, 33, 17), 0.1f));
    A_ITFD.AddFrame(FAnimation2DFrame(FRectanglei(132, 0, 33, 17), 1.0f));
    SpriteComponent->AddAnimation(A_ITFD);

    // Define FLY_DOWN_TO_IDLE animation
    FAnimation2D A_FDTI("FLY_DOWN_TO_IDLE", false);
    A_FDTI.AddFrame(FAnimation2DFrame(FRectanglei(99, 0, 33, 17), 0.1f));
    A_FDTI.AddFrame(FAnimation2DFrame(FRectanglei(66, 0, 33, 17), 1.0f));
    SpriteComponent->AddAnimation(A_FDTI);

    // Set the default animation to IDLE
    SpriteComponent->Play("IDLE", true);

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

    auto vel = velocity.GetValue();
    auto SpriteComponent =
        GetComponent<UAnimatedSpriteComponent>("AnimatedSpriteComponent");

    // Move player based on velocity and speed
    if (vel != 0.0f)
    {
        // Normalize velocity to ensure consistent speed in all directions
        if (vel.Length() != 0.0f) { vel = vel.Normalized(); }

        // Update transform
        FTransform transform = GetTransform();
        transform.Translate(FVector3(
            vel.x * (speed * 1.5f) * deltaTime, vel.y * speed * deltaTime, 0.0f
        ));
        SetTransform(transform);
    }

    // Update animation state
    if (vel.y > 0.0f) { SpriteComponent->Play("IDLE_TO_FLY_UP", false); }
    else if (vel.y < 0.0f)
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
    if (vel != FVector2f::Zero) { m_lastVelocity = velocity; }

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
        World::SpawnActor<BP_Projectile>("BP_Projectile", transform);
    }
}

///////////////////////////////////////////////////////////////////////////////
void BP_Player::MoveHorizontal(Float32 value)
{
    value = Math<float>::Clamp(value, -1.0f, 1.0f);
    velocity.GetValue().x += value;
}

///////////////////////////////////////////////////////////////////////////////
void BP_Player::MoveVertical(Float32 value)
{
    value = Math<float>::Clamp(value, -1.0f, 1.0f);
    velocity.GetValue().y += value;
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_Player, APawn)

}   // namespace tkd
