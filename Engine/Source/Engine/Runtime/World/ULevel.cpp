///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Runtime/World/ULevel.hpp>
#include <cstring>
#include <Engine/Assets/UAsset.hpp>
#include <Engine/Network/FBinaryReader.hpp>
#include <Engine/Network/FBinaryWriter.hpp>
#include <Engine/Runtime/World/UWorld.hpp>
#include <fstream>
#include <sstream>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{
///////////////////////////////////////////////////////////////////////////////
// const AgameMode& ULevel::GetGameMode(void) const
// {
//     return m_gameMode;
// }

///////////////////////////////////////////////////////////////////////////////
// void ULevel::SetGameMode(const AgameMode& gameMode)
// {
//     m_gameMode = gameMode;
// }

///////////////////////////////////////////////////////////////////////////////
const TVector<ULevel::ActorEntry>& ULevel::GetActorEntries(void) const
{
    return m_actorEntries;
}

///////////////////////////////////////////////////////////////////////////////
void ULevel::SetActorEntries(const TVector<ActorEntry>& actors)
{
    m_actorEntries = actors;
}

///////////////////////////////////////////////////////////////////////////////
FString ULevel::GetLevelName(void) const { return m_levelName; }

///////////////////////////////////////////////////////////////////////////////
void ULevel::SetLevelName(const FString& name) { m_levelName = name; }

///////////////////////////////////////////////////////////////////////////////
ULevel ULevel::LoadLevelFromAsset(UAsset& asset)
{
    ULevel level;

    // Validate asset type
    if (asset.GetType() != EAssetType::Level)
    {
        // Invalid asset type for level loading
        return level;
    }

    // Load asset data if not already loaded
    if (!asset.IsLoaded())
    {
        if (!asset.Load())
        {
            // Failed to load asset data
            return level;
        }
    }

    // Set level name from asset
    level.m_levelName = asset.GetName();

    // Parse the level data from asset
    const auto& data = asset.GetData();
    if (!level.ParseLevelData(data))
    {
        // Failed to parse level data
        level.m_levelName = "";
        level.m_actorEntries.Clear();
        return level;
    }

    return level;
}

///////////////////////////////////////////////////////////////////////////////
ULevel ULevel::LoadLevelFromFile(const FilePath& levelPath)
{
    ULevel level;

    try
    {
        // Create a UAsset from the file path
        UAsset asset(levelPath);

        // Verify it's a level asset
        if (asset.GetType() != EAssetType::Level)
        {
            // File is not a level asset
            return level;
        }

        // Delegate to LoadFromAsset
        level = LoadLevelFromAsset(asset);
    }
    catch (const std::exception& e)
    {
        // Failed to load asset from file - return empty level
        level.m_levelName = "";
        level.m_actorEntries.Clear();
    }

    return level;
}

///////////////////////////////////////////////////////////////////////////////
ULevel ULevel::LoadLevelFromWorld(const UWorld& world)
{
    ULevel level;
    world;
    // TODO: Implement conversion from UWorld to ULevel
    // Convert UWorld data to ULevel format
    // ...
    return level;
}

///////////////////////////////////////////////////////////////////////////////
bool ULevel::UnLoadLevel(void)
{
    // Clear level data
    m_levelName = "";
    m_actorEntries.Clear();
    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool ULevel::SaveToFile(const FilePath& levelPath)
{
    auto data = SerializeLevelData();
    std::ofstream file(levelPath, std::ios::binary);
    if (!file)
    {
        return false;   // Failed to open file
    }
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    if (!file)
    {
        return false;   // Failed to write data
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool ULevel::SaveToAsset(UAsset& asset)
{
    if (asset.GetType() != EAssetType::Level)
    {
        return false;   // Asset type mismatch
    }

    auto data = SerializeLevelData();
    asset.SetData(data);

    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool ULevel::ParseLevelData(const std::vector<Byte>& data)
{
    if (data.empty()) { return false; }

    // Create a binary reader for the data
    FBinaryReader reader(data.data(), data.size());

    // Clear existing actor entries
    m_actorEntries.Clear();

    try
    {
        // Read level format version (for future compatibility)
        UInt32 formatVersion;
        if (!reader.Read(formatVersion)) { return false; }

        // Currently only support version 1
        if (formatVersion != 1) { return false; }

        // Read number of actors
        SizeT actorCount;
        if (!reader.Read(actorCount)) { return false; }

        // Reserve space for actors
        m_actorEntries.Reserve(actorCount);

        // Read each actor entry
        for (SizeT i = 0; i < actorCount; ++i)
        {
            ActorEntry actor;

            // Read actor name
            if (!reader.Read(actor.name)) { return false; }

            // Read actor active state
            if (!reader.Read(actor.isActive)) { return false; }

            // Read actor position (FVector3)
            if (!reader.Read(actor.position)) { return false; }

            // Read actor rotation (FRotator)
            if (!reader.Read(actor.rotation)) { return false; }

            // Read actor scale (FVector3)
            if (!reader.Read(actor.scale)) { return false; }

            // Read number of properties
            SizeT propertyCount;
            if (!reader.Read(propertyCount)) { return false; }

            // Reserve space for properties
            actor.properties.Reserve(propertyCount);

            // Read each property
            for (SizeT j = 0; j < propertyCount; ++j)
            {
                PropertyEntry property;

                // Read property name, size, and value
                if (!reader.Read(property.name) ||
                    !reader.Read(property.size) ||
                    !reader.Read(property.value))
                {
                    return false;
                }

                actor.properties.PushBack(property);
            }

            m_actorEntries.PushBack(actor);
        }

        return true;
    }
    catch (...)
    {
        // Clear partially loaded data on error
        m_actorEntries.Clear();
        return false;
    }
}

///////////////////////////////////////////////////////////////////////////////
std::vector<Byte> ULevel::SerializeLevelData(void) const
{
    std::vector<Byte> data;
    FBinaryWriter writer(data);

    // Write level format version
    // TODO: get version from somewhere
    UInt32 formatVersion = 1;
    writer.Write(formatVersion);

    // Write number of actors
    SizeT actorCount = m_actorEntries.Size();
    writer.Write(actorCount);

    // Write each actor entry
    for (const auto& actor: m_actorEntries)
    {
        // Write actor name
        writer.Write(actor.name);

        // Write actor active state
        writer.Write(actor.isActive);

        // Write actor position (FVector3)
        writer.Write(actor.position);

        // Write actor rotation (FRotator)
        writer.Write(actor.rotation);

        // Write actor scale (FVector3)
        writer.Write(actor.scale);

        // Write number of properties
        SizeT propertyCount = actor.properties.Size();
        writer.Write(propertyCount);

        // Write each property
        for (const auto& property: actor.properties)
        {
            // Write property name, size, and value
            writer.Write(property.name);
            writer.Write(property.size);
            writer.Write(property.value);
        }
    }

    return data;
}

}   // namespace tkd
