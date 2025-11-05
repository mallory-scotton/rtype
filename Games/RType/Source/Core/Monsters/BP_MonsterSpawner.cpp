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
    , SpawnRadius(*this, "SpawnRadius", 5.0f)
    , MulticastSpawnOne(
          *this,
          "MulticastSpawnOne",
          ERPCType::Multicast,
          std::bind(
              &BP_MonsterSpawner::RPC_MulticastSpawnOne,
              this,
              std::placeholders::_1,
              std::placeholders::_2,
              std::placeholders::_3
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
    // Center of the spawner
    FVector3 center = GetTransform().GetPosition();

    // Create a pseudo-random offset using the spawner's accumulated time
    // and spawn count. This mirrors the simple deterministic approach
    // used in BP_Monster::PickNewTarget so spawned positions vary over time.
    Float32 seed = (m_time + static_cast<Float32>(m_spawned)) * 1000.0f;
    int idx = static_cast<int>(seed) & 7;   // 8 possible directions (0..7)
    Float32 frac = seed - static_cast<int>(seed);   // fractional part in [0,1)
    if (frac < 0.0f) { frac = -frac; }

    // radius in [0.25*SpawnRadius, 1.0*SpawnRadius]
    Float32 r = SpawnRadius() * (0.25f + 0.75f * frac);
    const Float32 KD = 0.70710678f;

    FVector3 offset;
    switch (idx)
    {
    case 0 : offset = FVector3(r, 0.0f, 0.2f); break;
    case 1 : offset = FVector3(r * KD, r * KD, 0.2f); break;
    case 2 : offset = FVector3(0.0f, r, 0.2f); break;
    case 3 : offset = FVector3(-r * KD, r * KD, 0.2f); break;
    case 4 : offset = FVector3(-r, 0.0f, 0.2f); break;
    case 5 : offset = FVector3(-r * KD, -r * KD, 0.2f); break;
    case 6 : offset = FVector3(0.0f, -r, 0.2f); break;
    default: offset = FVector3(r * KD, -r * KD, 0.2f); break;
    }

    // Spawn transform: start from the spawner transform and translate by
    // the computed offset so the monster appears within SpawnRadius.
    FTransform t = GetTransform();
    t.Translate(offset);

    // Spawn the monster on the server (authority) with a deterministic UUID
    UUID id = UUID::V4();

    // Derive a small initial target offset (jitter) so monsters spawned
    // around the spawner don't all pick the exact same path on first move.
    // Use a second pseudo-random fraction derived from the seed so the
    // jitter is stable/deterministic per spawn count.
    Float32 seed2 =
        seed * 1.6180339f + static_cast<Float32>(m_spawned) * 13.7f;
    Float32 frac2 = seed2 - static_cast<int>(seed2);
    if (frac2 < 0.0f) { frac2 = -frac2; }

    Float32 jitterScale =
        SpawnRadius() * 0.35f;   // up to ~35% of spawn radius
    Float32 jitterX = (frac2 - 0.5f) * 2.0f * jitterScale;
    Float32 jitterY = ((1.0f - frac2) - 0.5f) * 2.0f * jitterScale;
    FVector3 initOffset = FVector3(jitterX, jitterY, 0.2f);

    World::SpawnActorDeferredWithParams<BP_Monster>(t, id, initOffset);

    ++m_spawned;
    // Tell clients to spawn the same monster with the same UUID and
    // initial offset so both sides can correlate the actor and the
    // first movement target.
    MulticastSpawnOne(t, id, initOffset);
}

///////////////////////////////////////////////////////////////////////////////
void BP_MonsterSpawner::RPC_MulticastSpawnOne(
    FTransform transform, UUID uuid, FVector3 initOffset
)
{
    // Only clients should run this
    if (IsAuthority()) { return; }

    m_time = 0.0f;
    m_spawned++;

    // Spawn the monster on clients using the server-provided UUID and
    // initial offset so client-side instance maps to the server's and
    // picks a different first target.
    World::SpawnActorDeferredWithParams<BP_Monster>(
        transform, uuid, initOffset
    );
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_MonsterSpawner, AActor)

}   // namespace tkd
