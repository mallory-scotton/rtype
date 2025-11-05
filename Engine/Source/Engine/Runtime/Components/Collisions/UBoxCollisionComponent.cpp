///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Runtime/Components/Collisions/UBoxCollisionComponent.hpp>
#include <Engine/Runtime/Actor/AActor.hpp>
#include <Engine/Runtime/Physics/FOBB.hpp>
#include <Engine/Runtime/Physics/UCollisionSystem.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
UBoxCollisionComponent::UBoxCollisionComponent(const FString& name)
    : UCollisionComponent(name)
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

    // Mark the component as dirty in the collision system when transform
    // changes
    if (m_collisionSystem) { m_collisionSystem->MarkDirty(this); }
}

///////////////////////////////////////////////////////////////////////////////
void UBoxCollisionComponent::Render(IRenderer& renderer) const
{
    // Call parent render for debug bounding box
    UCollisionComponent::Render(renderer);
}

///////////////////////////////////////////////////////////////////////////////
void UBoxCollisionComponent::Tick(Float32)
{
    if (!IsHiddenInGame()) { SetShowDebug(true); }
    else { SetShowDebug(false); }
}

///////////////////////////////////////////////////////////////////////////////
FOBB UBoxCollisionComponent::GetLocalBoundingBox(void) const
{
    FOBB bound;

    bound.center = m_localTransform.GetPosition();
    bound.extents = m_boxExtent * m_localTransform.GetScale();

    // Local axes (identity - no rotation in local space)
    bound.axes[0] = FVector3(1.0f, 0.0f, 0.0f);   // X-axis
    bound.axes[1] = FVector3(0.0f, 1.0f, 0.0f);   // Y-axis
    bound.axes[2] = FVector3(0.0f, 0.0f, 1.0f);   // Z-axis

    return bound;
}

///////////////////////////////////////////////////////////////////////////////
FOBB UBoxCollisionComponent::GetWorldBoundingBox(void) const
{
    FOBB bound;

    // Get the owner's transform
    FTransform transform = GetOwner()->GetTransform() * m_localTransform;

    // Get local bounding box
    FOBB localBound = GetLocalBoundingBox();

    // Transform the center to world space
    bound.center = transform.TransformPoint(localBound.center);

    // Scale the box extents by the transform's scale
    bound.extents = localBound.extents * GetOwner()->GetTransform().GetScale();

    // Transform the axes to world space (rotate but don't translate)
    bound.axes[0] = transform.TransformDirection(localBound.axes[0]);
    bound.axes[1] = transform.TransformDirection(localBound.axes[1]);
    bound.axes[2] = transform.TransformDirection(localBound.axes[2]);

    // Normalize axes (in case transform has scaling)
    bound.axes[0] = bound.axes[0].Normalized();
    bound.axes[1] = bound.axes[1].Normalized();
    bound.axes[2] = bound.axes[2].Normalized();

    // Extents remain the same (assuming uniform scaling or no scaling)
    // If you need to handle non-uniform scaling, you'd need to scale extents
    // along each axis based on the transform's scale
    bound.extents = localBound.extents;

    return bound;
}

///////////////////////////////////////////////////////////////////////////////
FAABB UBoxCollisionComponent::GetWorldAABB(void) const
{
    // Full transform (world * local)
    FTransform worldTransform = GetOwner()->GetTransform() * m_localTransform;

    // Get the 8 corners of the box in local space
    FVector3 halfExtent(m_boxExtent.x, m_boxExtent.y, m_boxExtent.z);

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

    // Transform all corners to world space and find min/max
    FVector3 min = worldTransform.TransformPoint(corners[0]);
    FVector3 max = min;

    for (int i = 1; i < 8; ++i)
    {
        FVector3 worldCorner = worldTransform.TransformPoint(corners[i]);

        min.x = std::min(min.x, worldCorner.x);
        min.y = std::min(min.y, worldCorner.y);
        min.z = std::min(min.z, worldCorner.z);

        max.x = std::max(max.x, worldCorner.x);
        max.y = std::max(max.y, worldCorner.y);
        max.z = std::max(max.z, worldCorner.z);
    }

    return FAABB(min, max);
}

}   // namespace tkd
