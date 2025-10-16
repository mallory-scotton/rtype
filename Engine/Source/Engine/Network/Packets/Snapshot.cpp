///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Network/Packets/Snapshot.hpp>
#include <Engine/Runtime/Actor.hpp>
#include <Engine/Runtime/Controllers.hpp>
#include <Engine/Runtime/World/UWorld.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::Packets
///////////////////////////////////////////////////////////////////////////////
namespace tkd::Packets
{

///////////////////////////////////////////////////////////////////////////////
Snapshot::Snapshot(const UWorld& world) { SetupFromWorld(world); }

///////////////////////////////////////////////////////////////////////////////
void Snapshot::SetupFromWorld(const UWorld& world)
{
    // Clear existing actors
    actors.clear();

    // Retrieve all actors from the world
    const auto& worldActors = world.GetActors();
    for (const auto& actorPtr: worldActors)
    {
        // Skip null or inactive actors
        if (actorPtr == nullptr || !actorPtr->IsActive()) { continue; }

        // Create an ActorState for each actor
        ActorState state;
        state.className = actorPtr->GetClass()->GetName();
        state.id = actorPtr->GetUUID();
        state.transform = actorPtr->GetTransform();
        state.owningClientID = actorPtr->GetOwningClientID();

        // Serialize properties
        const auto& properties = actorPtr->GetProperties();
        for (const auto& [propName, propPtr]: properties)
        {
            // Skip null properties
            if (propPtr == nullptr) { continue; }

            // Create the vector to hold the serialized property data
            state.properties[propName] = propPtr->Serialize();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
bool Snapshot::Serialize(FBinaryWriter& writer) const
{
    // Serialize the snapshot ID
    writer.Write(snapshotID);

    // Serialize the number of actors
    writer.Write(static_cast<UInt32>(actors.size()));

    // Serialize each actor's state
    for (const auto& actor: actors)
    {
        writer.Write(actor.className);
        writer.Write(actor.id);
        writer.Write(actor.transform);
        writer.Write(actor.owningClientID);

        // Serialize properties
        writer.Write(static_cast<UInt32>(actor.properties.size()));
        for (const auto& [name, data]: actor.properties)
        {
            writer.Write(name);
            writer.Write(data);
        }
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool Snapshot::Deserialize(FBinaryReader& reader)
{
    // Deserialize the snapshot ID
    if (!reader.Read(snapshotID)) { return false; }

    // Deserialize the number of actors
    UInt32 actorCount = 0;

    if (!reader.Read(actorCount)) { return false; }
    actors.resize(actorCount);

    // Deserialize each actor's state
    for (auto& actor: actors)
    {
        if (!reader.Read(actor.className)) { return false; }
        if (!reader.Read(actor.id)) { return false; }
        if (!reader.Read(actor.transform)) { return false; }
        if (!reader.Read(actor.owningClientID)) { return false; }

        // Deserialize properties
        UInt32 propertyCount = 0;
        if (!reader.Read(propertyCount)) { return false; }
        actor.properties.clear();
        for (UInt32 i = 0; i < propertyCount; ++i)
        {
            FString propName;
            std::vector<Byte> propData;
            if (!reader.Read(propName)) { return false; }
            if (!reader.Read(propData)) { return false; }
            actor.properties[propName] = propData;
        }
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
SizeT Snapshot::GetSize(void) const
{
    SizeT size = sizeof(UInt32) * 2;   // snapshot ID + Actor count

    for (const auto& actor: actors)
    {
        size += sizeof(SizeT) + actor.className.Size();   // Class name
        size += sizeof(UUID);                             // UUID
        size += sizeof(FTransform);                       // Transform
        size += sizeof(UInt32);                           // Owning client ID

        size += sizeof(UInt32);                           // Property count
        for (const auto& [name, data]: actor.properties)
        {
            size += sizeof(SizeT) + name.Size();   // Property name
            size += sizeof(Byte) + data.size();    // Property data
        }
    }

    return size;
}

}   // namespace tkd::Packets
