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
    if (IsHiddenInGame()) { return; }

    const FTransform& worldTransform =
        GetOwner()->GetTransform() * m_localTransform;
    renderer.Draw(m_vertices, EPrimitiveType::LineStrip, worldTransform);
}

///////////////////////////////////////////////////////////////////////////////
FOBB UBoxCollisionComponent::GetLocalBoundingBox(void) const
{
    FOBB obb;
    obb.center = m_localTransform.GetPosition();
    obb.extents = FVector3(m_boxExtent.x, m_boxExtent.y, m_boxExtent.z);

    // Extract rotation axes from local transform
    FRotator rotation = m_localTransform.GetRotation();
    Float32 pitch = rotation.GetPitch() * M_PI / 180.0f;
    Float32 yaw = rotation.GetYaw() * M_PI / 180.0f;
    Float32 roll = rotation.GetRoll() * M_PI / 180.0f;

    Float32 cp = std::cos(pitch);
    Float32 sp = std::sin(pitch);
    Float32 cy = std::cos(yaw);
    Float32 sy = std::sin(yaw);
    Float32 cr = std::cos(roll);
    Float32 sr = std::sin(roll);

    // Right axis (X)
    obb.axes[0] = FVector3(cy * cr, cy * sr, -sy);
    // Up axis (Y)
    obb.axes[1] =
        FVector3(sp * sy * cr - cp * sr, sp * sy * sr + cp * cr, sp * cy);
    // Forward axis (Z)
    obb.axes[2] =
        FVector3(cp * sy * cr + sp * sr, cp * sy * sr - sp * cr, cp * cy);

    return obb;
}

///////////////////////////////////////////////////////////////////////////////
FOBB UBoxCollisionComponent::GetWorldBoundingBox(void) const
{
    // Full transform (world * local)
    FTransform worldTransform = GetOwner()->GetTransform() * m_localTransform;

    FOBB obb;
    obb.UpdateFromTransform(
        worldTransform, FVector3(m_boxExtent.x, m_boxExtent.y, m_boxExtent.z)
    );
    return obb;
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
