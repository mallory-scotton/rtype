///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <BSLevel.hpp>
#include <algorithm>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
BSBeatMap::BSBeatMap(void)
    : isValid(false)
{}

///////////////////////////////////////////////////////////////////////////////
BSBeatMap::BSBeatMap(const BSLevel& level, SizeT difficultyIndex)
    : difficulty(
          level.difficulties[difficultyIndex % level.difficulties.size()]
      )
    , isValid(false)
    , m_beatmapPath(level.levelPath / difficulty.beatmapFilename)
{
    if (CheckValidity() && level.version.size() > 0)
    {
        if (level.version.at(0) == '2') { LoadVersion2(); }
        else if (level.version.at(0) == '3') { LoadVersion3(); }
    }
    m_data.clear();
}

///////////////////////////////////////////////////////////////////////////////
bool BSBeatMap::CheckValidity(void)
{
    if (!FileSystem::FileExists(m_beatmapPath)) { return false; }

    try
    {
        std::ifstream dataStream(m_beatmapPath);
        m_data = Json::parse(dataStream);
    }
    catch (...)
    {
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
void BSBeatMap::LoadVersion2(void)
{
    // Parse notes
    if (m_data.contains("_notes"))
    {
        for (const auto& note: m_data["_notes"])
        {
            BSLevelNodeEntry entry;

            entry.time = note.value("_time", -1.f);
            entry.lineIndex = note.value("_lineIndex", -1);
            entry.lineLayer = note.value("_lineLayer", -1);
            entry.type = static_cast<ENoteType>(note.value("_type", 0));
            entry.cutDirection =
                static_cast<ECutDirection>(note.value("_cutDirection", 0));

            notes.push_back(entry);
        }
    }
    std::sort(
        notes.begin(),
        notes.end(),
        [](const BSLevelNodeEntry& a, const BSLevelNodeEntry& b)
        { return a.time < b.time; }
    );

    // Parse events
    if (m_data.contains("_events"))
    {
        for (const auto& event: m_data["_events"])
        {
            BSLevelEventEntry entry;

            entry.time = event.value("_time", -1.f);
            entry.type = event.value("_type", -1);
            entry.value = event.value("_value", -1);

            events.push_back(entry);
        }
    }
    std::sort(
        events.begin(),
        events.end(),
        [](const BSLevelEventEntry& a, const BSLevelEventEntry& b)
        { return a.time < b.time; }
    );

    // Parse obstacles
    if (m_data.contains("_obstacles"))
    {
        for (const auto& obs: m_data["_obstacles"])
        {
            BSLevelObstacleEntry entry;

            entry.time = obs.value("_time", -1.f);
            entry.lineIndex = obs.value("_lineIndex", -1);
            entry.height = 1;
            entry.duration = obs.value("_duration", 0.25);
            entry.type = obs.value("_type", 0);

            obstacles.push_back(entry);
        }
    }
    std::sort(
        obstacles.begin(),
        obstacles.end(),
        [](const BSLevelObstacleEntry& a, const BSLevelObstacleEntry& b)
        { return a.time < b.time; }
    );

    // Parse waypoints
    if (m_data.contains("_waypoints"))
    {
        for (const auto& way: m_data["_waypoints"])
        {
            BSLevelWaypointEntry entry;

            entry.time = way.value("_time", -1.f);

            waypoints.push_back(entry);
        }
    }
    std::sort(
        waypoints.begin(),
        waypoints.end(),
        [](const BSLevelWaypointEntry& a, const BSLevelWaypointEntry& b)
        { return a.time < b.time; }
    );

    // Set the map has valid
    isValid = true;
}

///////////////////////////////////////////////////////////////////////////////
void BSBeatMap::LoadVersion3(void)
{
    // TODO: Add the loader for version
}

///////////////////////////////////////////////////////////////////////////////
BSLevel::BSLevel(void)
    : isValid(false)
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
    m_infoData.clear();
}

///////////////////////////////////////////////////////////////////////////////
BSBeatMap BSLevel::LoadMap(SizeT difficultyIndex)
{
    return BSBeatMap(*this, difficultyIndex);
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
