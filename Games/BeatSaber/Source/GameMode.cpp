///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <GameMode.hpp>
#include <BP_Note.hpp>
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

    auto levelsPath = m_gameSaves / "Levels";

    if (!FileSystem::FileExists(levelsPath))
    {
        FileSystem::Mkdirs(levelsPath);
    }

    auto levels = FileSystem::ListDirectories(levelsPath);

    if (!levels.empty())
    {
        m_level = BSLevel(levels[0]);

        if (m_level.isValid)
        {
            std::cout << m_level << std::endl;
            Audio::PlaySound(m_level.levelPath / m_level.songFilename);
        }

        if (!m_level.difficulties.empty())
        {
            m_map = m_level.LoadMap(0);

            std::cout << "\nLoaded level: " << m_map.difficulty.beatmapFilename
                      << std::endl;
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

    // Spawn the notes
    for (const auto& note: m_map.notes)
    {
        if (note.time <= m_beatTime)
        {
            std::cout << "Spawning Note at: " << note.time << std::endl;
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
