///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <BSLevel.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
BSBeatMap::BSBeatMap(
    const FilePath& beatmapPath, const BSLevelDifficulty& difficulty
)
    : difficulty(difficulty)
{}

///////////////////////////////////////////////////////////////////////////////
BSLevel::BSLevel(const FilePath& levelPath)
    : levelPath(levelPath)
    , version("")
    , songName("")
    , songSubName("")
    , songAuthorName("")
    , levelAuthorName("")
    , songFilename("")
    , coverImageFilename("")
    , environmentName("")
    , beatsPerMinute(0.0f)
    , shuffle(0.0f)
    , shufflePeriod(0.0f)
    , previewStartTime(0.0f)
    , previewDuration(0.0f)
    , songTimeOffset(0.0f)
    , difficulties()
    , isValid(false)
{
    if (CheckValidity() && version.size() > 0)
    {
        if (version.at(0) == '2') { LoadVersion2(); }
        else if (version.at(0) == '3') { LoadVersion3(); }
    }
}

///////////////////////////////////////////////////////////////////////////////
bool BSLevel::CheckValidity(void)
{
    if (!FileSystem::DirectoryExists(levelPath)) { return false; }

    FilePath infoPath = levelPath / "Info.dat";
    if (!FileSystem::FileExists(infoPath)) { return false; }

    try
    {
        std::ifstream infoStream(infoPath);
        m_infoData = Json::parse(infoStream);
        version = m_infoData["_version"];
        if (version.empty()) { version = m_infoData["version"]; }
        if (version.empty()) { return false; }
    }
    catch (...)
    {
        return false;
    }

    isValid = true;
    return true;
}

///////////////////////////////////////////////////////////////////////////////
void BSLevel::LoadVersion2(void)
{
    songName = m_infoData.value("_songName", "");
    songSubName = m_infoData.value("_songSubName", "");
    songAuthorName = m_infoData.value("_songAuthorName", "");
    levelAuthorName = m_infoData.value("_levelAuthorName", "");
    songFilename = m_infoData.value("_songFilename", "");
    coverImageFilename = m_infoData.value("_coverImageFilename", "");
    environmentName = m_infoData.value("_environmentName", "");
    beatsPerMinute = m_infoData.value("_beatsPerMinute", 0.0f);
    shuffle = m_infoData.value("_shuffle", 0.0f);
    shufflePeriod = m_infoData.value("_shufflePeriod", 0.0f);
    previewStartTime = m_infoData.value("_previewStartTime", 0.0f);
    previewDuration = m_infoData.value("_previewDuration", 0.0f);
    songTimeOffset = m_infoData.value("_songTimeOffset", 0.0f);

    if (m_infoData.contains("_difficultyBeatmapSets"))
    {
        for (const auto& beatmapSet: m_infoData["_difficultyBeatmapSets"])
        {
            std::string characteristic =
                beatmapSet.value("_beatmapCharacteristicName", "Unknown");

            for (const auto& diff: beatmapSet["_difficultyBeatmaps"])
            {
                BSLevelDifficulty d;
                d.difficulty = diff.value("_difficulty", "");
                d.difficultyRank = diff.value("_difficultyRank", 0);
                d.beatmapFilename = diff.value("_beatmapFilename", "");
                d.noteJumpMovementSpeed =
                    diff.value("_noteJumpMovementSpeed", 0.0f);
                d.noteJumpStartBeatOffset =
                    diff.value("_noteJumpStartBeatOffset", 0.0f);

                difficulties.push_back(d);
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void BSLevel::LoadVersion3(void)
{
    // TODO: Implement Version 3 loading
}

///////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& os, const BSLevel& level)
{
    os << "=== Beat Saber Level Information ===" << std::endl;
    os << "Valid: " << (level.isValid ? "Yes" : "No") << std::endl;
    os << "Level Path: " << level.levelPath << std::endl;
    os << "Version: " << level.version << std::endl;
    os << std::endl;

    os << "--- Song Information ---" << std::endl;
    os << "Song Name: " << level.songName << std::endl;
    os << "Song Sub Name: " << level.songSubName << std::endl;
    os << "Song Author: " << level.songAuthorName << std::endl;
    os << "Level Author: " << level.levelAuthorName << std::endl;
    os << "Song Filename: " << level.songFilename << std::endl;
    os << "Cover Image: " << level.coverImageFilename << std::endl;
    os << "Environment: " << level.environmentName << std::endl;
    os << std::endl;

    os << "--- Audio Settings ---" << std::endl;
    os << "BPM: " << level.beatsPerMinute << std::endl;
    os << "Shuffle: " << level.shuffle << std::endl;
    os << "Shuffle Period: " << level.shufflePeriod << std::endl;
    os << "Preview Start Time: " << level.previewStartTime << "s" << std::endl;
    os << "Preview Duration: " << level.previewDuration << "s" << std::endl;
    os << "Song Time Offset: " << level.songTimeOffset << "s" << std::endl;
    os << std::endl;

    os << "--- Difficulties (" << level.difficulties.size() << ") ---"
       << std::endl;
    for (size_t i = 0; i < level.difficulties.size(); ++i)
    {
        const auto& diff = level.difficulties[i];
        os << "  [" << (i + 1) << "] " << diff.difficulty
           << " (Rank: " << diff.difficultyRank << ")" << std::endl;
        os << "      File: " << diff.beatmapFilename << std::endl;
        os << "      Note Jump Speed: " << diff.noteJumpMovementSpeed
           << std::endl;
        os << "      Note Jump Offset: " << diff.noteJumpStartBeatOffset
           << std::endl;
    }

    if (level.difficulties.empty())
    {
        os << "  No difficulties found." << std::endl;
    }

    return os;
}

}   // namespace tkd
