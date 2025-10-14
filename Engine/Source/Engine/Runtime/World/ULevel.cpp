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
ULevel::ULevel(const FString& name)
    : UObject(name)
    , m_gameMode()
    , m_levelName("")
    , m_actorEntries()
{}

///////////////////////////////////////////////////////////////////////////////
const AGameMode& ULevel::GetGameMode(void) const { return m_gameMode; }

///////////////////////////////////////////////////////////////////////////////
AGameMode& ULevel::GetGameMode(void) { return m_gameMode; }

///////////////////////////////////////////////////////////////////////////////
void ULevel::SetGameMode(const AGameMode& gameMode) { m_gameMode = gameMode; }

///////////////////////////////////////////////////////////////////////////////
const TVector<ULevel::ActorEntry>& ULevel::GetActorEntries(void) const
{
    return m_actorEntries;
}

///////////////////////////////////////////////////////////////////////////////
void ULevel::AddActorEntry(const ActorEntry& actor)
{
    m_actorEntries.PushBack(actor);
}

///////////////////////////////////////////////////////////////////////////////
void ULevel::AddActorEntry(
    const FString& className,
    const FString& actorName,
    const FTransform& transform
)
{
    ActorEntry actor;
    actor.class_name = className;
    actor.name = actorName;
    actor.isActive = true;   // Default to active
    actor.position = transform.GetPosition();
    actor.rotation = transform.GetRotation();
    actor.scale = transform.GetScale();
    m_actorEntries.PushBack(actor);
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
ULevel ULevel::LoadLevelFromWorld(UWorld& world)
{
    ULevel level;

    // Use the world's name if available (current level name as fallback)
    if (world.GetCurrentLevel())
    {
        level.m_levelName = world.GetCurrentLevel()->GetLevelName();
    }

    // Iterate all actors in the world and convert them to ActorEntry
    const auto& actors = world.GetActors();
    for (const auto& actorPtr: actors)
    {
        if (!actorPtr) { continue; }

        AActor* actor = actorPtr.get();

        ActorEntry entry;

        // Class name (if available)
        UClass* cls = actor->GetClass();
        entry.class_name = cls ? cls->GetName() : FString("<Unknown>");

        // Actor name and active state
        entry.name = actor->GetName();
        entry.isActive = actor->IsActive();

        // Transform: extract position, rotation and scale
        const auto& transform = actor->GetTransform();
        entry.position = transform.GetPosition();
        entry.rotation = transform.GetRotation();
        entry.scale = transform.GetScale();

        // Serialize properties. We skip the built-in Transform and IsActive
        // properties since they are stored separately above.
        const auto& props = actor->GetProperties();
        entry.properties.Reserve(props.size());

        for (const auto& kv: props)
        {
            const FString& propName = kv.first;
            IProperty* prop = kv.second;

            if (!prop) { continue; }

            // Skip duplicated stored properties
            if (propName == "Transform" || propName == "IsActive")
            {
                continue;
            }

            PropertyEntry pentry;
            pentry.name = propName;

            // Try common property concrete types and serialize their raw bytes
            bool serialized = false;

            // Bool
            if (!serialized)
            {
                if (auto concrete = dynamic_cast<UProperty<Bool>*>(prop))
                {
                    Bool v = concrete->Get();
                    pentry.size = sizeof(v);
                    pentry.value.resize(pentry.size);
                    std::memcpy(pentry.value.data(), &v, pentry.size);
                    serialized = true;
                }
            }

            // Int32
            if (!serialized)
            {
                if (auto concrete = dynamic_cast<UProperty<Int32>*>(prop))
                {
                    Int32 v = concrete->Get();
                    pentry.size = sizeof(v);
                    pentry.value.resize(pentry.size);
                    std::memcpy(pentry.value.data(), &v, pentry.size);
                    serialized = true;
                }
            }

            // UInt32
            if (!serialized)
            {
                if (auto concrete = dynamic_cast<UProperty<UInt32>*>(prop))
                {
                    UInt32 v = concrete->Get();
                    pentry.size = sizeof(v);
                    pentry.value.resize(pentry.size);
                    std::memcpy(pentry.value.data(), &v, pentry.size);
                    serialized = true;
                }
            }

            // Float32
            if (!serialized)
            {
                if (auto concrete = dynamic_cast<UProperty<Float32>*>(prop))
                {
                    Float32 v = concrete->Get();
                    pentry.size = sizeof(v);
                    pentry.value.resize(pentry.size);
                    std::memcpy(pentry.value.data(), &v, pentry.size);
                    serialized = true;
                }
            }

            // Float64
            if (!serialized)
            {
                if (auto concrete = dynamic_cast<UProperty<Float64>*>(prop))
                {
                    Float64 v = concrete->Get();
                    pentry.size = sizeof(v);
                    pentry.value.resize(pentry.size);
                    std::memcpy(pentry.value.data(), &v, pentry.size);
                    serialized = true;
                }
            }

            // FVector3
            if (!serialized)
            {
                if (auto concrete = dynamic_cast<UProperty<FVector3>*>(prop))
                {
                    auto v = concrete->Get();
                    pentry.size = sizeof(v);
                    pentry.value.resize(pentry.size);
                    std::memcpy(pentry.value.data(), &v, pentry.size);
                    serialized = true;
                }
            }

            // FRotator
            if (!serialized)
            {
                if (auto concrete = dynamic_cast<UProperty<FRotator>*>(prop))
                {
                    auto v = concrete->Get();
                    pentry.size = sizeof(v);
                    pentry.value.resize(pentry.size);
                    std::memcpy(pentry.value.data(), &v, pentry.size);
                    serialized = true;
                }
            }

            // FTransform
            if (!serialized)
            {
                if (auto concrete = dynamic_cast<UProperty<FTransform>*>(prop))
                {
                    auto v = concrete->Get();
                    pentry.size = sizeof(v);
                    pentry.value.resize(pentry.size);
                    std::memcpy(pentry.value.data(), &v, pentry.size);
                    serialized = true;
                }
            }

            // FString (serialize as raw FString object bytes) - only if
            // necessary
            if (!serialized)
            {
                if (auto concrete = dynamic_cast<UProperty<FString>*>(prop))
                {
                    auto v = concrete->Get();
                    pentry.size = sizeof(v);
                    pentry.value.resize(pentry.size);
                    std::memcpy(pentry.value.data(), &v, pentry.size);
                    serialized = true;
                }
            }

            // If we managed to serialize the property, push it
            if (serialized)
            {
                entry.properties.EmplaceBack(std::move(pentry));
            }
        }

        // Add actor entry to level
        level.m_actorEntries.EmplaceBack(std::move(entry));
    }

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

///////////////////////////////////////////////////////////////////////////////
bool ULevel::SaveWorldToFile(UWorld& world, const FilePath& levelPath)
{
    ULevel level = LoadLevelFromWorld(world);
    return level.SaveToFile(levelPath);
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(ULevel, UObject)

}   // namespace tkd
