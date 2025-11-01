///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <GameMode.hpp>
#include <BP_Note.hpp>
#include <BP_Sword.hpp>
#include <BSLevel.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
BeatSaberGameMode::BeatSaberGameMode(void)
    : AGameMode("BeatSaberGameMode")
    , m_gameSaves(FileSystem::GetLocalAppDataDirectory() / "Beat Saber")
    , m_playTime(0.0f)
    , m_beatTime(0.0f)
{
    m_playerControllerClassName = "APlayerController";
    m_actorClassName = "APawn";
    m_gameStateClassName = "AGameState";
    m_hudClassName = "AHUD";
    m_spectatorClassName = "ASpectator";
    m_defaultPlayerName = "Player";
    m_numPlayers = 1;
}

///////////////////////////////////////////////////////////////////////////////
void BeatSaberGameMode::BeginPlay(void)
{
    Super::BeginPlay();

#if TKD_ENGINE_CLIENT
    Window::GetCamera().position = FVector3::Zero;
#endif

    // Spawn both swords for the player
    World::SpawnActorDeferredWithParams<BP_Sword>(
        FTransform::Identity, "SwordLeft", VR::EHand::Left
    );
    World::SpawnActorDeferredWithParams<BP_Sword>(
        FTransform::Identity, "SwordRight", VR::EHand::Right
    );

    // Load a random level from the saves directory
    auto levelsPath = m_gameSaves / "Levels";

    if (!FileSystem::FileExists(levelsPath))
    {
        FileSystem::Mkdirs(levelsPath);
    }

    auto levels = FileSystem::ListDirectories(levelsPath);

    if (!levels.empty())
    {
        m_level = BSLevel(levels[std::rand() % levels.size()]);

        if (m_level.isValid)
        {
            std::cout << m_level << std::endl;
            Audio::PlaySound(m_level.levelPath / m_level.songFilename, 0.2f);
        }

        if (!m_level.difficulties.empty())
        {
            m_map = m_level.LoadMap(std::rand() % m_level.difficulties.size());

            std::cout << "\nLoaded level: " << m_map.difficulty.beatmapFilename
                      << std::endl;
            std::cout << "Version: " << m_map.version << std::endl;
            std::cout << "Notes Count: " << m_map.notes.size() << std::endl;
            std::cout << "Obstacles Count: " << m_map.obstacles.size()
                      << std::endl;
            std::cout << "Events Count: " << m_map.events.size() << std::endl;
            std::cout << "Waypoints Count: " << m_map.waypoints.size()
                      << std::endl;
        }
    }
    else { std::cout << "No levels found in: " << levelsPath << std::endl; }
}

///////////////////////////////////////////////////////////////////////////////
void BeatSaberGameMode::Tick(Float32 deltaTime)
{
    // Call the tick of the Super
    Super::Tick(deltaTime);

    // Update the play time
    m_playTime += deltaTime;
    m_beatTime = m_playTime * m_level.beatsPerMinute / 60.f;

    /*
        🚀 Note Jump Movement Speed (NJS)

        What it does

        Controls the speed at which notes travel from spawn to hit point.
        A higher NJS = notes fly toward you faster and spawn closer.
        A lower NJS = notes move slower and spawn farther away.

        How it works technically

        Beat Saber calculates spawn distance based on NJS and beat timing,
        so that notes always reach the player exactly on beat, but the travel
        time changes with NJS.

        🚀 Note Jump Start Beat Offset (NJO)

        What it does

        Adjusts how many beats before the hit time a note spawns.

        Positive offset → notes appear earlier (further away).

        Negative offset → notes appear later (closer to player).

        Example

        If your NJS = 20 and NJO = 0, notes spawn at the default distance
       (based on a 0.5s travel time at your BPM). If NJO = +1, they spawn one
       beat earlier → appear further away. If NJO = –1, they spawn one beat
       later → appear closer.

        spawnDistance = (jumpDistanceBase + noteJumpOffset) * (NJS / 10)

        🚀 Grid Spacing - Horizontal & Vertical

        y=2  [ ] [ ] [ ] [ ]
        y=1  [ ] [ ] [ ] [ ]
        y=0  [ ] [ ] [ ] [ ]
             x=0 x=1 x=2 x=3

        Each column/row is separated by 1 unit.

        So the centers of the cubes are 1 unit apart in both x and y
        directions.

        Coordinates for note centers:
        Grid (x,y)      Position (units)
        (0,0)           far left, bottom
        (1,0)           slightly left, bottom
        (2,0)           slightly right, bottom
        (3,0)           far right, bottom

        So the entire play area width = 4 units.
        The player’s saber reach roughly spans ±2 units horizontally and ±1.5
        units vertically.

        🚀 Player & Note Hit Plane

        The player’s saber plane (where cubes are hit) is at Z = 0.

        Notes move along the +Z axis toward 0, starting from a spawn point
        further away (positive Z).

        🚀 Base Spawn Distance (Default)

        This depends on Note Jump Speed (NJS) and Note Jump Offset (NJO), but
        the baseline constant is roughly 17–18 units (meters) from the player.

        At default NJS = 10 and NJO = 0, the first visible spawn point ≈ 17 m
       away.

        The formula used internally (simplified from Beat Saber’s C# code):

        jumpDistance = noteJumpMovementSpeed * jumpDuration
        spawnOffset = (jumpDistance * 0.5)
        spawnZ = playerZ + spawnOffset
    */

    // Calculate spawn distance based on noteJumpMovementSpeed and BPM
    // In Beat Saber, notes spawn at a distance that gives time to
    // react
    float halfJumpDuration = 4.0f;   // Half jump duration in beats (standard)
    float jumpDistance = m_map.difficulty.noteJumpMovementSpeed *
                         (halfJumpDuration / (m_level.beatsPerMinute / 60.0f));

    // Spawn the notes
    for (const auto& note: m_map.notes)
    {
        if (note.time <= m_beatTime)
        {
            // Beat Saber uses 0.6 units spacing between lanes
            // LineIndex: horizontal position (0-3, left to right)
            // Lanes are at: -0.9, -0.3, 0.3, 0.9 (centered at 0)
            // LineLayer: vertical position (0-2, bottom to top)
            // Layers are at: 0, 0.6, 1.2
            float horizontalPos =
                (static_cast<float>(note.lineIndex) - 1.5f) * 0.6f;
            float verticalPos = static_cast<float>(note.lineLayer + 1) * 0.6f;

            FVector3 position(horizontalPos, verticalPos, -jumpDistance);
            FTransform noteTransform(
                position, Rotator::Identity, FVector3::One
            );

            // Spawn the note with specific type, cut direction, and speed
            World::SpawnActorDeferredWithParams<BP_Note>(
                noteTransform,
                note.type,
                note.cutDirection,
                m_map.difficulty.noteJumpMovementSpeed
            );
        }
        else { break; }
    }

    // Remove the already processed notes
    m_map.notes.erase(
        std::remove_if(
            m_map.notes.begin(),
            m_map.notes.end(),
            [this](const BSLevelNodeEntry& note)
            { return note.time <= m_beatTime; }
        ),
        m_map.notes.end()
    );
}

///////////////////////////////////////////////////////////////////////////////
void BeatSaberGameMode::EndPlay(void) { Super::EndPlay(); }

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BeatSaberGameMode, AGameMode)

}   // namespace tkd
