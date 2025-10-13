///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Network/Packets/Replication.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::Packets
///////////////////////////////////////////////////////////////////////////////
namespace tkd::Packets
{

///////////////////////////////////////////////////////////////////////////////
bool Replication::Serialize(FBinaryWriter& writer) const
{
    writer.WriteBytes(
        reinterpret_cast<const Byte*>(actorID.data()), actorID.size()
    );
    writer.Write(timestamp);
    writer.Write(propertyName);
    writer.Write(data);
    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool Replication::Deserialize(FBinaryReader& reader)
{
    return reader.ReadBytes(
               reinterpret_cast<Byte*>(actorID.data()), actorID.size()
           ) &&
           reader.Read(timestamp) && reader.Read(propertyName) &&
           reader.Read(data);
}

///////////////////////////////////////////////////////////////////////////////
SizeT Replication::GetSize(void) const
{
    return sizeof(SizeT) * 2 + actorID.size() + propertyName.Size() +
           data.size() + sizeof(timestamp);
}

}   // namespace tkd::Packets
