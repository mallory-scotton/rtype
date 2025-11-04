///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Core/Monsters/BP_MonsterSpawner.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructor
///
///////////////////////////////////////////////////////////////////////////////
BP_MonsterSpawner::BP_MonsterSpawner(void)
    : AActor()
    , SpawnInterval(*this, "SpawnInterval", 2.5f)
    , MaxCount(*this, "MaxCount", 6)
    , SpawnRadius(*this, "SpawnRadius", 200.0f)
    , MulticastSpawnOne(
          *this,
          "MulticastSpawnOne",
          ERPCType::Multicast,
          std::bind(
              &BP_MonsterSpawner::RPC_MulticastSpawnOne,
              this,
              std::placeholders::_1
          ),
          true
      )
    , m_time(0.0f)
    , m_spawned(0)
{
#if TKD_ENGINE_CLIENT
    SetNetRole(ENetRole::SimulatedProxy);
#else
    SetNetRole(ENetRole::Authority);
#endif
    // SetTransformReplicated(true);
    // Spawner itself doesn't need to replicate transform
    SetTransformReplicated(false);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Called when the object is first created / spawned
///
///////////////////////////////////////////////////////////////////////////////
void BP_MonsterSpawner::BeginPlay(void)
{
    Super::BeginPlay();
    m_time = 0.0f;
    m_spawned = 0;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Per-frame tick
/// \param deltaTime Time since last tick
///
///////////////////////////////////////////////////////////////////////////////
void BP_MonsterSpawner::Tick(Float32 deltaTime)
{
    Super::Tick(deltaTime);

    // Only server/authority should spawn actors
    if (!IsAuthority()) { return; }

    if (m_spawned >= MaxCount()) { return; }

    m_time += deltaTime;
    if (m_time >= SpawnInterval())
    {
        SpawnOneMonster();
        m_time = 0.0f;
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Spawn a single monster at a random point inside the radius
///
///////////////////////////////////////////////////////////////////////////////
void BP_MonsterSpawner::SpawnOneMonster(void)
{
    FVector3 center = GetTransform().GetPosition();
    // Float32 r = Math<Float32>::Random() * SpawnRadius();
    // Float32 angle = Math<Float32>::Random() * Math<Float32>::PI * 2.0f;
    // FVector3 offset(
    // r * Math<Float32>::Cos(angle), r * Math<Float32>::Sin(angle), 0.0f
    // );

    FTransform t = GetTransform();
    t.Translate(
        center
    );   // center for now just to see the monster spawning on screen

    // Spawn the monster on the server (authority)
    auto* spawned = World::SpawnActor("BP_Monster", t);
    if (!spawned) { return; }

    // Notify all clients about the spawn using the world's RPC (multicast)
    World::WithWorld(
        [&](UWorld& world)
        {
            // Temporarily set RPC to multicast so all clients receive the
            // spawn
            world.SpawnActorRPC.SetRPCType(ERPCType::Multicast);
            world.SpawnActorRPC(
                "BP_Monster",
                t,
                spawned->GetUUID(),
                0u   // owningClientID = 0 (server)
            );
            // Restore to client-targeted (default) to avoid changing global
            // behavior
            world.SpawnActorRPC.SetRPCType(ERPCType::Client);
        }
    );

    ++m_spawned;
    // MulticastSpawnOne(t);
}

///////////////////////////////////////////////////////////////////////////////
void BP_MonsterSpawner::RPC_MulticastSpawnOne(FTransform transform)
{
    // Only clients should run this
    if (IsAuthority()) { return; }

    m_time = 0.0f;
    m_spawned++;

    // Spawn the monster on the client
    World::SpawnActor("BP_Monster", transform);
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_MonsterSpawner, AActor)

}   // namespace tkd
