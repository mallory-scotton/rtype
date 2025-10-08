///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Config.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief Physics body types
///
/// Static: Does not move
/// Kinematic: Moves but not affected by forces
/// Dynamic: Fully simulated
///
///////////////////////////////////////////////////////////////////////////////
enum class EPhysicsBodyType : UInt8
{
    Static = 0,   //<! Static body, does not move
    Kinematic,    //<! Kinematic body, moves but not affected by forces
    Dynamic       //<! Dynamic body, fully simulated
};

///////////////////////////////////////////////////////////////////////////////
/// \brief Collision detection modes
///
/// Discrete: Check for collisions at discrete time steps (may miss fast-moving
/// objects) Continuous: More accurate, checks for collisions along the
/// movement path
///
///////////////////////////////////////////////////////////////////////////////
enum class ECollisionDetection : UInt8
{
    Discrete = 0,   //<! Discrete collision detection
    Continuous      //<! Continuous collision detection
};

///////////////////////////////////////////////////////////////////////////////
/// \brief Collision response types
///
/// Ignore: No collision response
/// Overlap: Trigger overlap events but do not block movement
/// Block: Block movement and trigger collision events
///
///////////////////////////////////////////////////////////////////////////////
enum class ECollisionResponse : UInt8
{
    Ignore = 0,   //<! Ignore collisions
    Overlap,      //<! Trigger overlap events
    Block         //<! Block movement
};

///////////////////////////////////////////////////////////////////////////////
/// \brief Collision channels
///
/// Used for defining what types of objects can collide with each other
/// For example, a player might collide with world geometry but ignore other
/// players
///
///////////////////////////////////////////////////////////////////////////////
enum class ECollisionChannel : UInt8
{
    None = 0,       //<! No collision
    WorldStatic,    //<! Static world geometry
    WorldDynamic,   //<! Dynamic world geometry
    Pawn,           //<! Player and NPC characters
    Vehicle,        //<! Vehicles
    Destructible,   //<! Destructible objects
    PhysicsBody,    //<! Physics simulated objects
    Trigger,        //<! Trigger volumes
    All             //<! All channels
};

}   // namespace tkd
