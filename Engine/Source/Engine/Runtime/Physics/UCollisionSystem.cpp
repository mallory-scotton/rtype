///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Runtime/Physics/UCollisionSystem.hpp>
#include <algorithm>
#include <cfloat>
#include <cmath>
#include <Engine/Runtime/Actor/AActor.hpp>
#include <Engine/Runtime/Components/UCollisionComponent.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
UCollisionSystem::UCollisionSystem(Float32 cellSize)
    : m_cellSize(cellSize)
{
    // Initialize collision matrix - by default all channels collide
    for (SizeT i = 0; i < 9; ++i)
    {
        for (SizeT j = 0; j < 9; ++j) { m_collisionMatrix[i][j] = true; }
    }

    // Triggers don't block by default
    for (SizeT i = 0; i < 9; ++i)
    {
        m_collisionMatrix[static_cast<int>(ECollisionChannel::Trigger)][i] =
            false;
        m_collisionMatrix[i][static_cast<int>(ECollisionChannel::Trigger)] =
            false;
    }

    // None channel doesn't collide with anything
    for (SizeT i = 0; i < 9; ++i)
    {
        m_collisionMatrix[static_cast<int>(ECollisionChannel::None)][i] =
            false;
        m_collisionMatrix[i][static_cast<int>(ECollisionChannel::None)] =
            false;
    }
}

///////////////////////////////////////////////////////////////////////////////
void UCollisionSystem::RegisterComponent(
    UCollisionComponent* component, AActor* owner
)
{
    if (!component || !owner) { return; }

    // Check if already registered
    auto it = m_componentToIndex.find(component);
    if (it != m_componentToIndex.end()) { return; }

    // Add new entry
    CollisionEntry entry;
    entry.component = component;
    entry.owner = owner;
    entry.isDirty = true;

    SizeT index = m_collisionComponents.size();
    m_collisionComponents.push_back(entry);
    m_componentToIndex[component] = index;
}

///////////////////////////////////////////////////////////////////////////////
void UCollisionSystem::UnregisterComponent(UCollisionComponent* component)
{
    // Mark for deferred removal instead of removing immediately
    // This prevents issues when unregistering during collision detection
    m_pendingRemovals.push_back(component);
}

///////////////////////////////////////////////////////////////////////////////
void UCollisionSystem::ProcessPendingRemovals()
{
    for (UCollisionComponent* component: m_pendingRemovals)
    {
        auto it = m_componentToIndex.find(component);
        if (it == m_componentToIndex.end()) { continue; }

        SizeT index = it->second;

        // Remove from vector (swap with last element for O(1) removal)
        if (index < m_collisionComponents.size() - 1)
        {
            m_collisionComponents[index] = m_collisionComponents.back();
            m_componentToIndex[m_collisionComponents[index].component] = index;
        }

        m_collisionComponents.pop_back();
        m_componentToIndex.erase(it);

        // Clean up any overlapping pairs involving this component
        std::vector<ComponentPair> toRemove;
        for (const auto& pair: m_overlappingPairs)
        {
            ComponentPair compPair = pair.first;
            if (compPair.a == component || compPair.b == component)
            {
                toRemove.push_back(compPair);
            }
        }

        for (const ComponentPair& pair: toRemove)
        {
            m_overlappingPairs.erase(pair);
        }

        // Clean up any registered callbacks for this component
        m_onCollisionBegin.erase(component);
        m_onCollisionEnd.erase(component);
        m_onOverlapBegin.erase(component);
        m_onOverlapEnd.erase(component);
    }

    // Clear pending removals
    m_pendingRemovals.clear();
}

///////////////////////////////////////////////////////////////////////////////
void UCollisionSystem::MarkDirty(UCollisionComponent* component)
{
    auto it = m_componentToIndex.find(component);
    if (it != m_componentToIndex.end())
    {
        m_collisionComponents[it->second].isDirty = true;
    }
}

///////////////////////////////////////////////////////////////////////////////
void UCollisionSystem::Update(Float32)
{
    // Process pending removals FIRST to avoid accessing deleted components
    ProcessPendingRemovals();

    // Update bounding boxes for dirty components
    for (auto& entry: m_collisionComponents)
    {
        if (entry.isDirty)
        {
            UpdateBoundingBox(entry);
            entry.isDirty = false;
        }
    }

    // Rebuild spatial grid
    RebuildSpatialGrid();

    // Track current overlaps
    std::unordered_map<ComponentPair, bool, ComponentPairHash> currentOverlaps;

    // Broad phase: Check potential collisions using spatial grid
    for (const auto& cellPair: m_spatialGrid)
    {
        const SpatialCell& cell = cellPair.second;

        // Check all pairs within this cell
        for (SizeT i = 0; i < cell.componentIndices.size(); ++i)
        {
            const CollisionEntry& entryA =
                m_collisionComponents[cell.componentIndices[i]];

            // Skip if component is not active or owner is marked for deletion
            if (!entryA.component || !entryA.component->IsActive())
            {
                continue;
            }
            if (!entryA.owner || entryA.owner->IsMarkedForDeletion())
            {
                continue;
            }

            for (SizeT j = i + 1; j < cell.componentIndices.size(); ++j)
            {
                const CollisionEntry& entryB =
                    m_collisionComponents[cell.componentIndices[j]];

                // Skip if component is not active or owner is marked for
                // deletion
                if (!entryB.component || !entryB.component->IsActive())
                {
                    continue;
                }
                if (!entryB.owner || entryB.owner->IsMarkedForDeletion())
                {
                    continue;
                }

                // Check collision channels
                if (!ShouldCollide(
                        entryA.component->GetCollisionChannel(),
                        entryB.component->GetCollisionChannel()
                    ))
                {
                    continue;
                }

                // Broad phase: AABB test for quick rejection
                if (!entryA.aabb.Intersects(entryB.aabb)) { continue; }

                // Narrow phase: Detailed OBB collision check
                FCollisionInfo info;
                if (CheckCollision(entryA, entryB, info))
                {
                    ComponentPair pair(entryA.component, entryB.component);
                    currentOverlaps[pair] = true;

                    // Check if this is a new collision
                    bool isNew =
                        (m_overlappingPairs.find(pair) ==
                         m_overlappingPairs.end());

                    TriggerCollisionEvents(entryA, entryB, info, isNew);
                }
            }
        }
    }

    // Check for ended overlaps
    for (const auto& pair: m_overlappingPairs)
    {
        if (currentOverlaps.find(pair.first) == currentOverlaps.end())
        {
            // Overlap ended - trigger end events
            ComponentPair compPair = pair.first;
            UCollisionComponent* compA = compPair.a;
            UCollisionComponent* compB = compPair.b;

            auto itA = m_componentToIndex.find(compA);
            auto itB = m_componentToIndex.find(compB);

            if (itA != m_componentToIndex.end() &&
                itB != m_componentToIndex.end())
            {
                const CollisionEntry& entryA =
                    m_collisionComponents[itA->second];
                const CollisionEntry& entryB =
                    m_collisionComponents[itB->second];

                FCollisionInfo infoA, infoB;
                infoA.otherActor = entryB.owner;
                infoA.otherComponent = entryB.component;
                infoB.otherActor = entryA.owner;
                infoB.otherComponent = entryA.component;

                // Trigger end events based on collision response
                if (entryA.component->GetCollisionResponse() ==
                    ECollisionResponse::Block)
                {
                    auto& delegates = m_onCollisionEnd[entryA.component];
                    for (auto& callback: delegates) { callback(infoA); }
                }
                else if (entryA.component->GetCollisionResponse() ==
                         ECollisionResponse::Overlap)
                {
                    auto& delegates = m_onOverlapEnd[entryA.component];
                    for (auto& callback: delegates) { callback(infoA); }
                }

                if (entryB.component->GetCollisionResponse() ==
                    ECollisionResponse::Block)
                {
                    auto& delegates = m_onCollisionEnd[entryB.component];
                    for (auto& callback: delegates) { callback(infoB); }
                }
                else if (entryB.component->GetCollisionResponse() ==
                         ECollisionResponse::Overlap)
                {
                    auto& delegates = m_onOverlapEnd[entryB.component];
                    for (auto& callback: delegates) { callback(infoB); }
                }
            }
        }
    }

    // Update overlap tracking
    m_overlappingPairs = std::move(currentOverlaps);
}

///////////////////////////////////////////////////////////////////////////////
void UCollisionSystem::UpdateBoundingBox(CollisionEntry& entry)
{
    if (!entry.component || !entry.owner) { return; }
    entry.boundingBox = entry.component->GetWorldBoundingBox();
    entry.aabb = entry.component->GetWorldAABB();
}

///////////////////////////////////////////////////////////////////////////////
void UCollisionSystem::RebuildSpatialGrid()
{
    m_spatialGrid.clear();

    for (SizeT i = 0; i < m_collisionComponents.size(); ++i)
    {
        const CollisionEntry& entry = m_collisionComponents[i];

        // Get cell range covered by this AABB (for spatial partitioning)
        Int32 minX, minY, minZ, maxX, maxY, maxZ;
        WorldToCell(entry.aabb.min, minX, minY, minZ);
        WorldToCell(entry.aabb.max, maxX, maxY, maxZ);

        // Add to all cells it overlaps
        for (Int32 x = minX; x <= maxX; ++x)
        {
            for (Int32 y = minY; y <= maxY; ++y)
            {
                for (Int32 z = minZ; z <= maxZ; ++z)
                {
                    Int64 key = GetCellKey(x, y, z);
                    m_spatialGrid[key].componentIndices.push_back(i);
                }
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
Int64 UCollisionSystem::GetCellKey(Int32 x, Int32 y, Int32 z) const
{
    // Pack 3D coordinates into 64-bit key
    return (static_cast<Int64>(x) << 42) | (static_cast<Int64>(y) << 21) |
           static_cast<Int64>(z);
}

///////////////////////////////////////////////////////////////////////////////
void UCollisionSystem::WorldToCell(
    const FVector3& pos, Int32& x, Int32& y, Int32& z
) const
{
    x = static_cast<Int32>(std::floor(pos.x / m_cellSize));
    y = static_cast<Int32>(std::floor(pos.y / m_cellSize));
    z = static_cast<Int32>(std::floor(pos.z / m_cellSize));
}

///////////////////////////////////////////////////////////////////////////////
bool UCollisionSystem::CheckCollision(
    const CollisionEntry& a, const CollisionEntry& b, FCollisionInfo& outInfo
)
{
    // Separating Axis Theorem (SAT) for OBB vs OBB collision
    const FOBB& obbA = a.boundingBox;
    const FOBB& obbB = b.boundingBox;

    Float32 minPenetration = FLT_MAX;
    FVector3 minAxis;

    // Test axes: 3 from A, 3 from B, and 9 cross products
    FVector3 testAxes[15];
    int numAxes = 0;

    // Face normals from A
    for (int i = 0; i < 3; ++i) { testAxes[numAxes++] = obbA.axes[i]; }

    // Face normals from B
    for (int i = 0; i < 3; ++i) { testAxes[numAxes++] = obbB.axes[i]; }

    // Cross products of edge pairs
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            FVector3 axis = obbA.axes[i].Cross(obbB.axes[j]);
            Float32 length = axis.Length();
            if (length > 0.0001f) { testAxes[numAxes++] = axis / length; }
        }
    }

    // Test each axis
    for (int i = 0; i < numAxes; ++i)
    {
        FVector3 axis = testAxes[i];

        // Project A onto axis
        Float32 projA = 0.0f;
        for (int j = 0; j < 3; ++j)
        {
            projA += std::abs(obbA.extents[j] * axis.Dot(obbA.axes[j]));
        }

        // Project B onto axis
        Float32 projB = 0.0f;
        for (int j = 0; j < 3; ++j)
        {
            projB += std::abs(obbB.extents[j] * axis.Dot(obbB.axes[j]));
        }

        // Project distance between centers onto axis
        FVector3 delta = obbB.center - obbA.center;
        Float32 distance = std::abs(delta.Dot(axis));

        // Check for separation
        Float32 penetration = projA + projB - distance;
        if (penetration < 0.0f)
        {
            return false;   // Separating axis found
        }

        // Track minimum penetration
        if (penetration < minPenetration)
        {
            minPenetration = penetration;
            minAxis = axis;

            // Ensure normal points from A to B
            if (delta.Dot(axis) < 0.0f) { minAxis = minAxis * -1.0f; }
        }
    }

    // Fill collision info
    outInfo.otherActor = b.owner;
    outInfo.otherComponent = b.component;
    outInfo.impactNormal = minAxis;
    outInfo.penetrationDepth = minPenetration;
    outInfo.impactPoint = obbA.center + (obbB.center - obbA.center) * 0.5f;

    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool UCollisionSystem::ShouldCollide(
    ECollisionChannel ch1, ECollisionChannel ch2
) const
{
    return m_collisionMatrix[static_cast<int>(ch1)][static_cast<int>(ch2)];
}

///////////////////////////////////////////////////////////////////////////////
void UCollisionSystem::TriggerCollisionEvents(
    const CollisionEntry& entryA,
    const CollisionEntry& entryB,
    const FCollisionInfo& info,
    bool isNewCollision
)
{
    // Prepare collision info for both actors
    FCollisionInfo infoA = info;
    infoA.otherActor = entryB.owner;
    infoA.otherComponent = entryB.component;

    FCollisionInfo infoB = info;
    infoB.otherActor = entryA.owner;
    infoB.otherComponent = entryA.component;
    infoB.impactNormal = FVector3(
        -info.impactNormal.x, -info.impactNormal.y, -info.impactNormal.z
    );

    // Only trigger begin events for new collisions
    if (isNewCollision)
    {
        // Trigger events based on collision response type
        if (entryA.component->GetCollisionResponse() ==
            ECollisionResponse::Block)
        {
            auto& delegates = m_onCollisionBegin[entryA.component];
            for (auto& callback: delegates) { callback(infoA); }
        }
        else if (entryA.component->GetCollisionResponse() ==
                 ECollisionResponse::Overlap)
        {
            auto& delegates = m_onOverlapBegin[entryA.component];
            for (auto& callback: delegates) { callback(infoA); }
        }

        if (entryB.component->GetCollisionResponse() ==
            ECollisionResponse::Block)
        {
            auto& delegates = m_onCollisionBegin[entryB.component];
            for (auto& callback: delegates) { callback(infoB); }
        }
        else if (entryB.component->GetCollisionResponse() ==
                 ECollisionResponse::Overlap)
        {
            auto& delegates = m_onOverlapBegin[entryB.component];
            for (auto& callback: delegates) { callback(infoB); }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void UCollisionSystem::BindOnCollisionBegin(
    UCollisionComponent* component, const FCollisionDelegate& callback
)
{
    if (component) { m_onCollisionBegin[component].push_back(callback); }
}

///////////////////////////////////////////////////////////////////////////////
void UCollisionSystem::BindOnCollisionEnd(
    UCollisionComponent* component, const FCollisionDelegate& callback
)
{
    if (component) { m_onCollisionEnd[component].push_back(callback); }
}

///////////////////////////////////////////////////////////////////////////////
void UCollisionSystem::BindOnOverlapBegin(
    UCollisionComponent* component, const FCollisionDelegate& callback
)
{
    if (component) { m_onOverlapBegin[component].push_back(callback); }
}

///////////////////////////////////////////////////////////////////////////////
void UCollisionSystem::BindOnOverlapEnd(
    UCollisionComponent* component, const FCollisionDelegate& callback
)
{
    if (component) { m_onOverlapEnd[component].push_back(callback); }
}

///////////////////////////////////////////////////////////////////////////////
void UCollisionSystem::SetChannelCollision(
    ECollisionChannel channel1, ECollisionChannel channel2, bool collides
)
{
    m_collisionMatrix[static_cast<int>(channel1)][static_cast<int>(channel2)] =
        collides;
    m_collisionMatrix[static_cast<int>(channel2)][static_cast<int>(channel1)] =
        collides;
}

///////////////////////////////////////////////////////////////////////////////
bool UCollisionSystem::LineTrace(
    const FVector3& start,
    const FVector3& end,
    FCollisionInfo& outInfo,
    ECollisionChannel channel
)
{
    FVector3 direction = end - start;
    Float32 length = direction.Length();

    if (length < 0.001f) { return false; }

    direction = direction / length;

    Float32 closestHit = length;
    bool hitSomething = false;

    for (const auto& entry: m_collisionComponents)
    {
        if (!entry.component || !entry.component->IsActive()) { continue; }

        // Check channel compatibility
        if (channel != ECollisionChannel::All &&
            !ShouldCollide(channel, entry.component->GetCollisionChannel()))
        {
            continue;
        }

        // Ray-AABB intersection (use AABB for simplicity)
        Float32 tMin;
        FVector3 normal;

        if (RayAABBIntersection(start, direction, entry.aabb, tMin, normal))
        {
            if (tMin < closestHit && tMin >= 0.0f)
            {
                closestHit = tMin;
                hitSomething = true;

                outInfo.otherActor = entry.owner;
                outInfo.otherComponent = entry.component;
                outInfo.impactPoint = start + direction * tMin;
                outInfo.impactNormal = normal;
                outInfo.penetrationDepth = 0.0f;
            }
        }
    }

    return hitSomething;
}

///////////////////////////////////////////////////////////////////////////////
bool UCollisionSystem::RayAABBIntersection(
    const FVector3& rayOrigin,
    const FVector3& rayDir,
    const FAABB& aabb,
    Float32& tMin,
    FVector3& outNormal
)
{
    Float32 tNear = -FLT_MAX;
    Float32 tFar = FLT_MAX;
    FVector3 hitNormal(0, 0, 0);

    for (int i = 0; i < 3; ++i)
    {
        Float32 origin = (&rayOrigin.x)[i];
        Float32 dir = (&rayDir.x)[i];
        Float32 min = (&aabb.min.x)[i];
        Float32 max = (&aabb.max.x)[i];

        if (std::abs(dir) < 0.0001f)
        {
            // Ray parallel to slab
            if (origin < min || origin > max) { return false; }
        }
        else
        {
            Float32 t1 = (min - origin) / dir;
            Float32 t2 = (max - origin) / dir;

            if (t1 > t2) { std::swap(t1, t2); }

            if (t1 > tNear)
            {
                tNear = t1;
                hitNormal = FVector3(0, 0, 0);
                (&hitNormal.x)[i] = dir > 0 ? -1.0f : 1.0f;
            }

            tFar = std::min(tFar, t2);

            if (tNear > tFar || tFar < 0) { return false; }
        }
    }

    tMin = tNear;
    outNormal = hitNormal;
    return true;
}

///////////////////////////////////////////////////////////////////////////////
SizeT UCollisionSystem::SphereOverlap(
    const FVector3& center,
    Float32 radius,
    std::vector<AActor*>& outActors,
    ECollisionChannel channel
)
{
    outActors.clear();
    Float32 radiusSq = radius * radius;

    for (const auto& entry: m_collisionComponents)
    {
        if (!entry.component || !entry.component->IsActive()) { continue; }

        // Check channel compatibility
        if (channel != ECollisionChannel::All &&
            !ShouldCollide(channel, entry.component->GetCollisionChannel()))
        {
            continue;
        }

        // Check if AABB intersects sphere (use AABB for simplicity)
        FVector3 closest;

        // Find closest point on AABB to sphere center
        closest.x =
            std::max(entry.aabb.min.x, std::min(center.x, entry.aabb.max.x));
        closest.y =
            std::max(entry.aabb.min.y, std::min(center.y, entry.aabb.max.y));
        closest.z =
            std::max(entry.aabb.min.z, std::min(center.z, entry.aabb.max.z));

        FVector3 delta = closest - center;
        Float32 distSq =
            delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;

        if (distSq <= radiusSq) { outActors.push_back(entry.owner); }
    }

    return outActors.size();
}

}   // namespace tkd
