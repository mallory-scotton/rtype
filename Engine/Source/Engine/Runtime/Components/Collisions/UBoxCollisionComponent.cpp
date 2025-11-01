///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Runtime/Components/Collisions/UBoxCollisionComponent.hpp>
#include <Engine/Runtime/Actor/AActor.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
UBoxCollisionComponent::UBoxCollisionComponent(const FString& name)
    : UActorComponent(name)
    , UPhysicsObject(
          EPhysicsBodyType::Kinematic,
          ECollisionDetection::Continuous,
          ECollisionResponse::Overlap,
          ECollisionChannel::PhysicsBody
      )
    , m_boxExtent(50.0f, 50.0f, 50.0f)
    , m_localTransform(FTransform::Identity)
{
    SetHiddenInGame(true);
    SetBoxExtent(m_boxExtent);
}

///////////////////////////////////////////////////////////////////////////////
const FVector3f& UBoxCollisionComponent::GetBoxExtent(void) const
{
    return m_boxExtent;
}

///////////////////////////////////////////////////////////////////////////////
void UBoxCollisionComponent::SetBoxExtent(const FVector3f& extent)
{
    m_boxExtent = extent;

    // Define a color for the box lines
    FColor color(0.82f, 0.61f, 0.12f, 1.0f);

    m_vertices.clear();

    // Calculate half extents for easier vertex calculation
    FVector3 halfExtent(m_boxExtent.x, m_boxExtent.y, m_boxExtent.z);

    // Define the 8 corners of the box
    FVector3 corners[8] = {
        FVector3(-halfExtent.x, -halfExtent.y, -halfExtent.z),
        FVector3(halfExtent.x, -halfExtent.y, -halfExtent.z),
        FVector3(halfExtent.x, halfExtent.y, -halfExtent.z),
        FVector3(-halfExtent.x, halfExtent.y, -halfExtent.z),
        FVector3(-halfExtent.x, -halfExtent.y, halfExtent.z),
        FVector3(halfExtent.x, -halfExtent.y, halfExtent.z),
        FVector3(halfExtent.x, halfExtent.y, halfExtent.z),
        FVector3(-halfExtent.x, halfExtent.y, halfExtent.z)
    };

    // Create line strip vertices to draw the wireframe box
    // Bottom face (back)
    m_vertices.push_back(FVertex(corners[0], color));
    m_vertices.push_back(FVertex(corners[1], color));
    m_vertices.push_back(FVertex(corners[2], color));
    m_vertices.push_back(FVertex(corners[3], color));
    m_vertices.push_back(FVertex(corners[0], color));

    // Move to front bottom
    m_vertices.push_back(FVertex(corners[4], color));
    m_vertices.push_back(FVertex(corners[5], color));
    m_vertices.push_back(FVertex(corners[6], color));
    m_vertices.push_back(FVertex(corners[7], color));
    m_vertices.push_back(FVertex(corners[4], color));

    // Connect back to front (vertical edges)
    m_vertices.push_back(FVertex(corners[5], color));
    m_vertices.push_back(FVertex(corners[1], color));
    m_vertices.push_back(FVertex(corners[2], color));
    m_vertices.push_back(FVertex(corners[6], color));
    m_vertices.push_back(FVertex(corners[7], color));
    m_vertices.push_back(FVertex(corners[3], color));
}

///////////////////////////////////////////////////////////////////////////////
const FTransform& UBoxCollisionComponent::GetLocalTransform(void) const
{
    return m_localTransform;
}

///////////////////////////////////////////////////////////////////////////////
void UBoxCollisionComponent::SetLocalTransform(const FTransform& transform)
{
    m_localTransform = transform;
}

///////////////////////////////////////////////////////////////////////////////
void UBoxCollisionComponent::Render(IRenderer& renderer) const
{
    if (IsHiddenInGame()) { return; }

    const FTransform& worldTransform =
        GetOwner()->GetTransform() * m_localTransform;
    renderer.Draw(m_vertices, EPrimitiveType::LineStrip, worldTransform);
}

}   // namespace tkd
