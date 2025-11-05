///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Config.hpp>
#include <Engine/Runtime/Physics/FAABB.hpp>
#include <Engine/Runtime/Physics/FCollisionInfo.hpp>
#include <Engine/Runtime/Physics/FOBB.hpp>
#include <Engine/Runtime/Physics/UPhysicsObject.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief Type definition for collision delegate functions
///
///////////////////////////////////////////////////////////////////////////////
using FCollisionDelegate = TFunction<void(const FCollisionInfo&)>;

///////////////////////////////////////////////////////////////////////////////
/// \brief Collision System class for managing physics objects and collisions
///
///////////////////////////////////////////////////////////////////////////////
class UCollisionSystem
{
private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Structure to track collision components
    ///
    ///////////////////////////////////////////////////////////////////////////
    class CollisionEntry
    {
    public:
        ///////////////////////////////////////////////////////////////////////
        // Class Member
        ///////////////////////////////////////////////////////////////////////
        UCollisionComponent* component;   //<! The collision component
        AActor* owner;                    //<! The owning actor
        FOBB boundingBox;                 //<! The oriented bounding box
        FAABB
        aabb;   //<! The axis-aligned bounding box for spatial partitioning
        bool isDirty;   //<! Needs bounding box recalculation

    public:
        ///////////////////////////////////////////////////////////////////////
        /// \brief Default constructor
        ///
        ///////////////////////////////////////////////////////////////////////
        CollisionEntry(void)
            : component(nullptr)
            , owner(nullptr)
            , boundingBox()
            , aabb()
            , isDirty(true)
        {}
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Struct to represent a pair of collision components
    ///
    ///////////////////////////////////////////////////////////////////////////
    struct ComponentPair
    {
        UCollisionComponent* a;
        UCollisionComponent* b;

        ComponentPair(UCollisionComponent* compA, UCollisionComponent* compB)
            : a(compA < compB ? compA : compB)
            , b(compA < compB ? compB : compA)
        {}

        bool operator==(const ComponentPair& other) const
        {
            return a == other.a && b == other.b;
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Hash function for ComponentPair
    ///
    ///////////////////////////////////////////////////////////////////////////
    struct ComponentPairHash
    {
        std::size_t operator()(const ComponentPair& pair) const
        {
            // Combine the two pointer hashes
            std::size_t h1 = std::hash<UCollisionComponent*>{}(pair.a);
            std::size_t h2 = std::hash<UCollisionComponent*>{}(pair.b);
            return h1 ^ (h2 << 1);
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Spatial grid cell for broad-phase collision detection
    ///
    ///////////////////////////////////////////////////////////////////////////
    struct SpatialCell
    {
        std::vector<SizeT> componentIndices;
    };

private:
    ///////////////////////////////////////////////////////////////////////////
    // Class Members
    ///////////////////////////////////////////////////////////////////////////
    std::vector<CollisionEntry> m_collisionComponents;
    std::unordered_map<UCollisionComponent*, SizeT> m_componentToIndex;

    // Spatial grid for broad-phase optimization
    std::unordered_map<Int64, SpatialCell> m_spatialGrid;
    Float32 m_cellSize;

    // Collision delegates (per component, not per actor)
    std::unordered_map<UCollisionComponent*, std::vector<FCollisionDelegate>>
        m_onCollisionBegin;
    std::unordered_map<UCollisionComponent*, std::vector<FCollisionDelegate>>
        m_onCollisionEnd;
    std::unordered_map<UCollisionComponent*, std::vector<FCollisionDelegate>>
        m_onOverlapBegin;
    std::unordered_map<UCollisionComponent*, std::vector<FCollisionDelegate>>
        m_onOverlapEnd;

    // Track overlapping pairs using proper hash-based pair tracking
    std::unordered_map<ComponentPair, bool, ComponentPairHash>
        m_overlappingPairs;

    // Collision matrix for channel filtering
    bool m_collisionMatrix[9][9];   //<! 9 = ECollisionChannel enum size

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor
    ///
    /// \param cellSize The size of spatial grid cells (default: 200.0f)
    ///
    ///////////////////////////////////////////////////////////////////////////
    UCollisionSystem(Float32 cellSize = 200.0f);

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Register a collision component
    ///
    /// \param component The component to register
    /// \param owner The owner actor
    ///
    ///////////////////////////////////////////////////////////////////////////
    void RegisterComponent(UCollisionComponent* component, AActor* owner);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Unregister a collision component
    ///
    /// \param component The component to unregister
    ///
    ///////////////////////////////////////////////////////////////////////////
    void UnregisterComponent(UCollisionComponent* component);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Mark a component as dirty (needs bounding box update)
    ///
    /// \param component The component to mark dirty
    ///
    ///////////////////////////////////////////////////////////////////////////
    void MarkDirty(UCollisionComponent* component);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Update the collision system
    ///
    /// \param deltaTime Time elapsed since last update
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Update(Float32 deltaTime);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Bind collision begin delegate
    ///
    /// \param component The collision component to bind to
    /// \param callback The callback function
    ///
    ///////////////////////////////////////////////////////////////////////////
    void BindOnCollisionBegin(
        UCollisionComponent* component, const FCollisionDelegate& callback
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Bind collision end delegate
    ///
    /// \param component The collision component to bind to
    /// \param callback The callback function
    ///
    ///////////////////////////////////////////////////////////////////////////
    void BindOnCollisionEnd(
        UCollisionComponent* component, const FCollisionDelegate& callback
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Bind overlap begin delegate
    ///
    /// \param component The collision component to bind to
    /// \param callback The callback function
    ///
    ///////////////////////////////////////////////////////////////////////////
    void BindOnOverlapBegin(
        UCollisionComponent* component, const FCollisionDelegate& callback
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Bind overlap end delegate
    ///
    /// \param component The collision component to bind to
    /// \param callback The callback function
    ///
    ///////////////////////////////////////////////////////////////////////////
    void BindOnOverlapEnd(
        UCollisionComponent* component, const FCollisionDelegate& callback
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set collision channel interaction
    ///
    /// \param channel1 First collision channel
    /// \param channel2 Second collision channel
    /// \param collides Whether these channels should collide
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetChannelCollision(
        ECollisionChannel channel1, ECollisionChannel channel2, bool collides
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Line trace against collision components
    ///
    /// \param start Start point of the trace
    /// \param end End point of the trace
    /// \param outInfo Collision information if hit
    /// \param channel Collision channel to trace against
    ///
    /// \return True if hit something, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool LineTrace(
        const FVector3& start,
        const FVector3& end,
        FCollisionInfo& outInfo,
        ECollisionChannel channel = ECollisionChannel::All
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Sphere overlap check
    ///
    /// \param center Center of the sphere
    /// \param radius Radius of the sphere
    /// \param outActors Vector to fill with overlapping actors
    /// \param channel Collision channel to check
    ///
    /// \return Number of overlapping actors
    ///
    ///////////////////////////////////////////////////////////////////////////
    SizeT SphereOverlap(
        const FVector3& center,
        Float32 radius,
        std::vector<AActor*>& outActors,
        ECollisionChannel channel = ECollisionChannel::All
    );

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Update bounding box for a component
    ///
    ///////////////////////////////////////////////////////////////////////////
    void UpdateBoundingBox(CollisionEntry& entry);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Rebuild spatial grid
    ///
    ///////////////////////////////////////////////////////////////////////////
    void RebuildSpatialGrid();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get spatial grid cell key
    ///
    ///////////////////////////////////////////////////////////////////////////
    Int64 GetCellKey(Int32 x, Int32 y, Int32 z) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get cell coordinates from world position
    ///
    ///////////////////////////////////////////////////////////////////////////
    void WorldToCell(const FVector3& pos, Int32& x, Int32& y, Int32& z) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Narrow-phase collision detection (detailed check)
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool CheckCollision(
        const CollisionEntry& a,
        const CollisionEntry& b,
        FCollisionInfo& outInfo
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Check if two collision channels should interact
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool ShouldCollide(ECollisionChannel ch1, ECollisionChannel ch2) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Trigger collision events
    ///
    ///////////////////////////////////////////////////////////////////////////
    void TriggerCollisionEvents(
        const CollisionEntry& entryA,
        const CollisionEntry& entryB,
        const FCollisionInfo& info,
        bool isNewCollision
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Ray-AABB intersection test
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool RayAABBIntersection(
        const FVector3& rayOrigin,
        const FVector3& rayDir,
        const FAABB& aabb,
        Float32& tMin,
        FVector3& outNormal
    );
};

}   // namespace tkd
