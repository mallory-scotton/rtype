///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Core/Monsters/FlipBooks/FB_MonsterIdle.hpp>
#include <Core/Monsters/FlipBooks/FB_MonsterIdleInverse.hpp>
#include <Core/Monsters/FlipBooks/FB_MonsterMove.hpp>
#include <Core/Monsters/FlipBooks/FB_MonsterMoveInverse.hpp>
#include <Core/Monsters/FlipBooks/FB_MonsterMoveUp.hpp>
#include <Core/Monsters/FlipBooks/FB_MonsterMoveUpInverse.hpp>
#include <Engine.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief Simple monster actor (non-playable) with roaming AI
///
/// This blueprint provides a minimal monster implementation for testing.
/// It uses an animated sprite (re-using the player sprite sheet), picks
/// random targets within a roam radius and moves toward them. Movement is
/// authority-driven (server) while clients simply render the actor.
///////////////////////////////////////////////////////////////////////////////
class BP_Monster : public AActor
{
public:
    ///////////////////////////////////////////////////////////////////////////
    // Configurable properties
    ///////////////////////////////////////////////////////////////////////////
    UProperty<Float32> speed;        //<! movement speed in units/sec
    UProperty<FVector2f> velocity;   //<! The current velocity of the monster
    UProperty<Float32> roamRadius;   //<! roam radius around spawn
    UProperty<Float32> waitTime;     //<! time to wait at each target (seconds)

private:
    ///////////////////////////////////////////////////////////////////////////
    // Runtime state
    ///////////////////////////////////////////////////////////////////////////
    FVector3 m_spawnPosition;    //<! spawn position used as roam center
    FVector3 m_targetPosition;   //<! current movement target
    FVector3 m_initialOffset;    //<! initial offset to apply to first target
    Float32 m_timeSinceTarget;   //<! timer since last target pick
    Float32 m_waitRemaining;   //<! remaining wait time when arrived at target
    Float32 m_lastXVelocity;   //<! last frame x velocity for animation

    FB_MonsterIdle m_idleAnimation;                     //<! Idle animation
    FB_MonsterMove m_walkAnimation;                     //<! Walk animation
    FB_MonsterMoveUp m_moveUpAnimation;                 //<! Move up animation
    FB_MonsterIdleInverse m_idleAnimationInverse;       //<! Idle animation
    FB_MonsterMoveInverse m_walkAnimationInverse;       //<! Walk animation
    FB_MonsterMoveUpInverse m_moveUpAnimationInverse;   //<! Move up animation

    UFunction<FTransform, FVector2f>
        MulticastPos;   //<! Multicast RPC for target updates

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor
    ///
    /// \param row Sprite sheet row to use for this monster
    ///
    ///////////////////////////////////////////////////////////////////////////
    BP_Monster(
        const UUID& uuid = UUID::V4(),
        const FVector3& initialOffset = FVector3::Zero
    );

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Called when the actor is first created / spawned
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void BeginPlay(void) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Per-frame tick
    ///
    /// \param deltaTime Time since last tick
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void Tick(Float32 deltaTime) override;

protected:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Simulate movement for client-side prediction
    ///
    /// \param inputVector The input vector for this move
    /// \param deltaTime The time elapsed since the last tick
    /// \param startTransform The starting transform before the move
    ///
    /// \return The resulting transform after simulating movement
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual FTransform SimulateMovement(
        const FVector3& inputVector,
        Float32 deltaTime,
        const FTransform& startTransform
    ) override;

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Pick a new random target within the roam radius
    ///
    ///////////////////////////////////////////////////////////////////////////
    void PickNewTarget(Float32 deltaTime);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Update the animation state based on movement
    ///
    ///////////////////////////////////////////////////////////////////////////
    void UpdateAnimationState(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Multicast RPC to notify clients of new target position
    ///
    ///////////////////////////////////////////////////////////////////////////
    void RPC_MulticastPos(FTransform pos, FVector2f vel);

public:
    ///////////////////////////////////////////////////////////////////////////
    // Class metadata
    ///////////////////////////////////////////////////////////////////////////
    DECLARE_CLASS_WITH_SUPER(BP_Monster, AActor)
};

}   // namespace tkd
