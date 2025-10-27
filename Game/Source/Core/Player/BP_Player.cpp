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
    , speed(*this, "Speed", 2.0f)
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
    , m_idleAnimation("FB_IdleAnimation")
    , m_moveUpAnimation("FB_MoveUpAnimation")
    , m_moveDownAnimation("FB_MoveDownAnimation")
{
    // Enable transform replication for networked movement
    SetTransformReplicated(true);

    // Add components
    AddComponent<UBillboardComponent>("BC_PlayerSprite");
    AddComponent<UBoxCollisionComponent>("BoxCollision");
}

///////////////////////////////////////////////////////////////////////////////
void BP_Player::SetupAnimations(void)
{
    // Define frames for Idle Animation
    m_idleAnimation.ClearFrames();
    m_idleAnimation.SetSourceMode(UFlipBook::ESourceMode::FromSprite);
    m_idleAnimation.SetSpriteSheetPath("Assets/Images/T_PlayerShips.png");
    m_idleAnimation.SetPlayMode(UFlipBook::EPlayMode::Loop);
    m_idleAnimation.AddFrame(FRectangle(66, playerColor * 17, 33, 17));
    m_idleAnimation.SetDuration(1.0f);
    m_idleAnimation.SetPlaybackSpeed(1.f);
    m_idleAnimation.LoadFrames();

    // Define frames for Move Up Animation
    m_moveUpAnimation.ClearFrames();
    m_moveUpAnimation.SetSourceMode(UFlipBook::ESourceMode::FromSprite);
    m_moveUpAnimation.SetSpriteSheetPath("Assets/Images/T_PlayerShips.png");
    m_moveUpAnimation.SetPlayMode(UFlipBook::EPlayMode::Once);
    m_moveUpAnimation.AddFrame(FRectangle(33, playerColor * 17, 33, 17));
    m_moveUpAnimation.AddFrame(FRectangle(0, playerColor * 17, 33, 17));
    m_moveUpAnimation.SetDuration(0.2f);
    m_moveUpAnimation.SetPlaybackSpeed(1.f);
    m_moveUpAnimation.LoadFrames();

    // Define frames for Move Down Animation
    m_moveDownAnimation.ClearFrames();
    m_moveDownAnimation.SetSourceMode(UFlipBook::ESourceMode::FromSprite);
    m_moveDownAnimation.SetSpriteSheetPath("Assets/Images/T_PlayerShips.png");
    m_moveDownAnimation.SetPlayMode(UFlipBook::EPlayMode::Once);
    m_moveDownAnimation.AddFrame(FRectangle(99, playerColor * 17, 33, 17));
    m_moveDownAnimation.AddFrame(FRectangle(132, playerColor * 17, 33, 17));
    m_moveDownAnimation.SetDuration(0.2f);
    m_moveDownAnimation.SetPlaybackSpeed(1.f);
    m_moveDownAnimation.LoadFrames();
}

///////////////////////////////////////////////////////////////////////////////
void BP_Player::BeginPlay(void)
{
    // Call parent BeginPlay first
    Super::BeginPlay();

    // Setup animations
    SetupAnimations();

    // Ensure textures are properly loaded after initialization
    auto Billboard = GetComponent<UBillboardComponent>("BC_PlayerSprite");
    if (Billboard)
    {
        Billboard->SetDisplayMode(UBillboardComponent::EDisplayMode::FlipBook);
        Billboard->SetFlipBook(&m_idleAnimation);
        // Frames will be loaded automatically when setting the FlipBook
    }

    auto Box = GetComponent<UBoxCollisionComponent>("BoxCollision");
    if (Box)
    {
        Box->SetHiddenInGame(false);
        Box->SetBoxExtent(FVector3f(0.8f, 0.3f, 0.3f));
    }
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
    auto Billboard = GetComponent<UBillboardComponent>("BC_PlayerSprite");

    // Update animation state
    if (velocity().y < 0.0f)
    {
        m_moveUpAnimation.SetPlaybackSpeed(1.f);
        Billboard->SetFlipBook(&m_moveUpAnimation);
    }
    else if (velocity().y > 0.0f)
    {
        m_moveDownAnimation.SetPlaybackSpeed(1.f);
        Billboard->SetFlipBook(&m_moveDownAnimation);
    }
    else
    {
        if (m_lastVelocity.y < 0.0f)
        {
            m_moveUpAnimation.SetPlaybackSpeed(-1.f);
            Billboard->SetFlipBook(&m_moveUpAnimation);
            if (Billboard->GetFlipBook() == &m_moveUpAnimation &&
                m_moveUpAnimation.HasFinished())
            {
                m_lastVelocity.y = 0.0f;
            }
        }
        else if (m_lastVelocity.y > 0.0f)
        {
            m_moveDownAnimation.SetPlaybackSpeed(-1.f);
            Billboard->SetFlipBook(&m_moveDownAnimation);
            if (Billboard->GetFlipBook() == &m_moveDownAnimation &&
                m_moveDownAnimation.HasFinished())
            {
                m_lastVelocity.y = 0.0f;
            }
        }
        else { Billboard->SetFlipBook(&m_idleAnimation); }
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
