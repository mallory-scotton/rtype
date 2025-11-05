///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Core/Monsters/BP_MonsterSpawner.hpp>
#include <Core/Monsters/BP_Monster.hpp>
#include <Engine.hpp>

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

    FTransform t = GetTransform();
    t.Translate(center);

    // Spawn the monster on the server (authority)
    World::SpawnActorDeferredWithParams<BP_Monster>(t);

    ++m_spawned;
    MulticastSpawnOne(t);
}

///////////////////////////////////////////////////////////////////////////////
void BP_MonsterSpawner::RPC_MulticastSpawnOne(FTransform transform)
{
    // Only clients should run this
    if (IsAuthority()) { return; }

    m_time = 0.0f;
    m_spawned++;

    World::SpawnActorDeferredWithParams<BP_Monster>(transform);
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_MonsterSpawner, AActor)

}   // namespace tkd
