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

    BSLevel level(levels[0]);

    if (level.isValid)
    {
        std::cout << level << std::endl;
        Audio::PlaySound(level.levelPath / level.songFilename);
    }
}

///////////////////////////////////////////////////////////////////////////////
void BeatSaberGameMode::Tick(Float32 deltaTime) { Super::Tick(deltaTime); }

///////////////////////////////////////////////////////////////////////////////
void BeatSaberGameMode::EndPlay(void) { Super::EndPlay(); }

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BeatSaberGameMode, AGameMode)

}   // namespace tkd
