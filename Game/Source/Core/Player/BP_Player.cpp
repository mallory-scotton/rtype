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
    , speed(*this, "Speed", 200.0f, EPropertyFlags::Replicated)
    , velocity(*this, "Velocity", FVector2f::Zero)
    , playerColor(*this, "PlayerColor", playerColor % 5)
    , ServerFire(
          *this,
          "ServerFire",
          ERPCType::Server,
          std::bind(&BP_Player::RPC_ServerFire, this),
          true
      )
    , MulticastFire(
          *this,
          "MulticastFire",
          ERPCType::Multicast,
          std::bind(
              &BP_Player::RPC_MulticastFire, this, std::placeholders::_1
          ),
          true
      )
    , m_lastVelocity(FVector2f::Zero)
    , m_lastFiredTime(0.0f)
    , m_lastPosition(FVector3::Zero)
{
    // Enable transform replication for networked movement
    SetTransformReplicated(true);

    auto Abp = AddComponent<UAnimatedSpriteComponent>("ABP_PlayerSprite");
    Abp->SetTexturePath("Assets/Images/T_PlayerShips.png");

    auto Box = AddComponent<UBoxCollisionComponent>("BoxCollision");
    Box->SetHiddenInGame(false);
    Box->SetBoxExtent(FVector3f(16.0f, 8.0f, 16.0f));

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
    // For locally controlled players, use input velocity
    if (IsLocallyControlled())
    {
        // Build input vector from velocity
        FVector3 inputVector(velocity->x, velocity->y, 0.0f);

        // Normalize and scale the input if there is movement
        FVector3 scaledInput = FVector3::Zero;
        if (inputVector.Length() > 0.0f)
        {
            // Normalize the input vector
            FVector3 normalizedInput = inputVector.Normalized();

            // Scale by speed
            scaledInput = normalizedInput * speed();
        }

        // ALWAYS call ApplyMovement, even with zero input
        // This ensures stop commands are sent to the server
        ApplyMovement(scaledInput, deltaTime);
    }
    else if (!IsAuthority())
    {
        // For simulated proxies (other players we see),
        // calculate velocity from movement for animations
        // The movement is already being interpolated by AActor::Tick

        FVector3 currentPosition = GetTransform().GetPosition();

        // Calculate movement delta from last frame
        FVector3 movementDelta = currentPosition - m_lastPosition;

        // Update velocity property for animation (normalized direction)
        if (movementDelta.Length() > 0.01f && deltaTime > 0.0f)
        {
            FVector3 velocityDir = movementDelta / deltaTime;
            // Normalize to -1 to 1 range for animation
            Float32 maxSpeed = speed();
            if (maxSpeed > 0.0f)
            {
                velocity->x = Math<Float32>::Clamp(
                    velocityDir.x / maxSpeed, -1.0f, 1.0f
                );
                velocity->y = Math<Float32>::Clamp(
                    velocityDir.y / maxSpeed, -1.0f, 1.0f
                );
            }
        }
        else { velocity = FVector2f::Zero; }

        // Update last position for next frame
        m_lastPosition = currentPosition;
    }

    // Call parent tick for components (handles interpolation for simulated
    // proxies)
    Super::Tick(deltaTime);

    // Add time to last fired time
    m_lastFiredTime += deltaTime;

    // Update animation state based on movement
    UpdateAnimationState();

    // Update last velocity if there is movement
    if (velocity() != FVector2f::Zero) { m_lastVelocity = velocity; }

    // Reset velocity for next frame (only for locally controlled)
    if (IsLocallyControlled()) { velocity = FVector2f::Zero; }
}

///////////////////////////////////////////////////////////////////////////////
void BP_Player::UpdateAnimationState(void)
{
    // Get the animated sprite component
    auto Abp = GetComponent<UAnimatedSpriteComponent>("ABP_PlayerSprite");

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
}

///////////////////////////////////////////////////////////////////////////////
void BP_Player::Fire(void)
{
    if (IsLocallyControlled() && m_lastFiredTime >= 0.25f)
    {
        // Call the server RPC to handle firing
        this->ServerFire();
        // Reset last fired time
        m_lastFiredTime = 0.0f;
    }
}

///////////////////////////////////////////////////////////////////////////////
void BP_Player::TestSpeedReplication(void)
{
    if (IsLocallyControlled())
    {
        // Increase speed by 50 each time Z is pressed
        Float32 newSpeed = speed.Get() + 50.0f;
        speed = newSpeed;

        FLogger::SetNamespace("BP_Player");
        FLogger::Info(
            "Speed modified to: {} (will replicate to server)", newSpeed
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
void BP_Player::RPC_ServerFire(void)
{
    if (IsAuthority() && m_lastFiredTime >= 0.25f)
    {
        // Reset last fired time
        m_lastFiredTime = 0.0f;

        // Get player transform
        FTransform transform = GetTransform();
        transform.SetRotation(FRotator(0.f, 0.f, 0.f));
        transform.SetScale(FVector3f::One);

        // Call multicast RPC to notify all clients
        this->MulticastFire(transform);

        // Spawn a projectile
        World::SpawnActor("BP_Projectile", transform);

        std::cout << "current speed: " << speed << std::endl;
    }
}

///////////////////////////////////////////////////////////////////////////////
void BP_Player::RPC_MulticastFire(FTransform transform)
{
    // Play firing effects on all clients,
    if (IsAuthority()) { return; }

    World::SpawnActor("BP_Projectile", transform);
    // Reset last fired time
    m_lastFiredTime = 0.0f;
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
FTransform BP_Player::SimulateMovement(
    const FVector3& inputVector,
    Float32 deltaTime,
    const FTransform& startTransform
)
{
    // Create result transform from start
    FTransform result = startTransform;

    // Apply movement with player's speed
    if (inputVector.Length() > 0.0f)
    {
        // Input is already scaled by speed in Tick, so just apply directly
        FVector3 movement = inputVector * deltaTime;
        result.Translate(movement);
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_Player, APawn)

}   // namespace tkd
