///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Core/AI/AI_Player.hpp>
#include <Core/Monsters/BP_Monster.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
AI_Player::AI_Player(UInt32 playerColor)
    : BP_Player(playerColor)
    , m_isMovementInitialized(false)
    , m_totalTime(0.0f)
    , m_waveFrequency(2.0f)
    , m_waveAmplitude(300.0f)
    , m_forwardSpeed(-1.0f)
    , m_startY(0.0f)
    , m_fireRate(0.8f)
{
    SetTransformReplicated(true);
    AddComponent<UBillboardComponent>("BC_PlayerSprite");
    AddComponent<UBoxCollisionComponent>("BoxCollision");
}

void AI_Player::BeginPlay(void)
{
    m_startY = GetTransform().GetPosition().y;

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
        Box->SetBoxExtent(FVector3f(0.6f, 0.3f, 0.3f));
        FTransform transform = Box->GetLocalTransform();
        transform.SetPosition(FVector3(0.f, 0.f, 0.2f));
        Box->SetLocalTransform(transform);

        auto* cs = Box->GetCollisionSystem();
        if (cs)
        {
            cs->BindOnOverlapBegin(
                Box,
                [this](const FCollisionInfo& info)
                {
                    if (info.otherActor && info.otherActor->Is<BP_Monster>())
                    {
                        // Handle player collision with monster
                        // this->MarkForDeletion();
                    }
                }
            );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AI_Player::Tick(Float32 deltaTime)
{
    if (IsAuthority())
    {
        ApplyMovement(FVector3::Zero, deltaTime);
        m_lastFiredTime += deltaTime;
        if (m_lastFiredTime >= m_fireRate)
        {
            m_lastFiredTime = 0.0f;
            this->Fire();
        }
    }
    else
    {
        FVector3 currentPosition = GetTransform().GetPosition();
        FVector3 movementDelta = currentPosition - m_lastPosition;
        if (movementDelta.Length() > 0.01f && deltaTime > 0.0f)
        {
            FVector3 velocityDir = movementDelta / deltaTime;
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
        else
        {
            velocity = FVector2f::Zero;
        }
        m_lastPosition = currentPosition;
    }

    Super::Tick(deltaTime);

    UpdateAnimationState();

    if (velocity() != FVector2f::Zero) { m_lastVelocity = velocity; }
}

///////////////////////////////////////////////////////////////////////////////
FTransform AI_Player::SimulateMovement(
    const FVector3& inputVector,
    Float32 deltaTime,
    const FTransform& startTransform
)
{
    Float32 m_totalTime = 0.0f;
    Float32 m_waveFrequency = 2.0f;
    Float32 m_waveAmplitude = 300.0f;

    // One-time initialization on the first frame
    if (!m_isMovementInitialized)
    {
        // Store the initial Y position to use as the center of the wave
        m_startY = startTransform.GetPosition().y;
        m_isMovementInitialized = true;
    }

    // Update the total time elapsed to progress the sine function
    m_totalTime += deltaTime;

    // Calculate the target Y position for this frame
    Float32 targetY =
        m_startY +
        (Math<Float32>::Sin(m_totalTime * m_waveFrequency) * m_waveAmplitude);

    // Determine the Y direction needed to reach the targetY
    FVector3 currentPosition = startTransform.GetPosition();
    Float32 directionY = 0.0f;
    _Float32 m_forwardSpeed = -1.0f;

    if (targetY > currentPosition.y + 1.0f)
    {
        directionY = 1.0f;   // Move up
    }
    else if (targetY < currentPosition.y - 1.0f)
    {
        directionY = -1.0f;   // Move down
    }

    // Create the AI's desired direction vector
    FVector3 aiDirectionVector(m_forwardSpeed, directionY, 0.0f);

    // Normalize the vector to ensure consistent speed
    aiDirectionVector = FVector3::Normalize(aiDirectionVector);

    // Calculate the final velocity vector with the 'speed' property inherited
    // from BP_Player
    FVector3 aiInputVector = aiDirectionVector * this->speed.Get();

    // Start with the current transform
    FTransform result = startTransform;

    // Apply the calculated AI movement
    if (aiInputVector.Length() > 0.0f)
    {
        // Calculate movement based on velocity and delta time
        FVector3 movement = aiInputVector * deltaTime;
        result.Translate(movement);
    }

    // Return the new transform
    return result;
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(AI_Player, BP_Player)

}   // namespace tkd
