///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Runtime/Components/UCollisionComponent.hpp>
#include <Engine/Runtime/Actor/AActor.hpp>
#include <Engine/Runtime/Physics/UCollisionSystem.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
UCollisionComponent::UCollisionComponent(const FString& name)
    : UActorComponent(name)
    , UPhysicsObject(
          EPhysicsBodyType::Kinematic,
          ECollisionDetection::Continuous,
          ECollisionResponse::Overlap,
          ECollisionChannel::PhysicsBody
      )
    , m_collisionSystem(nullptr)
    , m_showDebug(false)
{}

///////////////////////////////////////////////////////////////////////////////
void UCollisionComponent::SetCollisionSystem(UCollisionSystem* system)
{
    m_collisionSystem = system;
}

///////////////////////////////////////////////////////////////////////////////
UCollisionSystem* UCollisionComponent::GetCollisionSystem(void) const
{
    return m_collisionSystem;
}

///////////////////////////////////////////////////////////////////////////////
void UCollisionComponent::BeginPlay(void)
{
    // Call parent BeginPlay
    UActorComponent::BeginPlay();

    // Register with the collision system if available
    if (m_collisionSystem)
    {
        m_collisionSystem->RegisterComponent(this, GetOwner());
    }
}

///////////////////////////////////////////////////////////////////////////////
void UCollisionComponent::EndPlay(void)
{
    // Unregister from the collision system if available
    if (m_collisionSystem) { m_collisionSystem->UnregisterComponent(this); }
}

///////////////////////////////////////////////////////////////////////////////
void UCollisionComponent::SetShowDebug(bool debug) { m_showDebug = debug; }

///////////////////////////////////////////////////////////////////////////////
void UCollisionComponent::Render(IRenderer& renderer) const
{
    if (IsHiddenInGame() || !m_showDebug) { return; }

    // Draw the bounding box for debugging
    FOBB worldBox = GetWorldBoundingBox();

    // Draw the oriented bounding box edges
    FColor debugColor = FColor::Green;

    FVector3 corners[8];
    worldBox.GetCorners(corners);

    // Draw edges between the corners
    std::vector<FVertex> lineVertices;
    auto addLine = [&](const FVector3& start, const FVector3& end)
    {
        lineVertices.emplace_back(start, debugColor);
        lineVertices.emplace_back(end, debugColor);
    };

    // Top (Z+)
    addLine(corners[0], corners[1]);
    addLine(corners[1], corners[2]);
    addLine(corners[2], corners[3]);
    addLine(corners[3], corners[0]);

    // Bottom (Z-)
    addLine(corners[4], corners[5]);
    addLine(corners[5], corners[6]);
    addLine(corners[6], corners[7]);
    addLine(corners[7], corners[4]);

    // Sides
    addLine(corners[0], corners[4]);
    addLine(corners[1], corners[5]);
    addLine(corners[2], corners[6]);
    addLine(corners[3], corners[7]);

    renderer.Draw(lineVertices, EPrimitiveType::Lines);
}

}   // namespace tkd
