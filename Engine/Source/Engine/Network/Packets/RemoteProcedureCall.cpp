///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Network/Packets/RemoteProcedureCall.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::Packets
///////////////////////////////////////////////////////////////////////////////
namespace tkd::Packets
{

///////////////////////////////////////////////////////////////////////////////
bool RemoteProcedureCall::Serialize(FBinaryWriter& writer) const
{
    writer.WriteBytes(
        reinterpret_cast<const UInt8*>(actorID.data()), actorID.size()
    );
    writer.Write(static_cast<UInt8>(rpcType));
    writer.Write(functionName);
    writer.Write(parameters);
    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool RemoteProcedureCall::Deserialize(FBinaryReader& reader)
{
    UInt8 rpcTypeValue;
    if (!reader.ReadBytes(
            reinterpret_cast<UInt8*>(actorID.data()), actorID.size()
        ) ||
        !reader.Read(rpcTypeValue) || !reader.Read(functionName) ||
        !reader.Read(parameters))
    {
        return false;
    }
    rpcType = static_cast<ERPCType>(rpcTypeValue);
    return true;
}

///////////////////////////////////////////////////////////////////////////////
SizeT RemoteProcedureCall::GetSize(void) const
{
    return sizeof(SizeT) * 2 + actorID.size() + functionName.Size() +
           parameters.size() * sizeof(Byte) + sizeof(UInt8);
}

}   // namespace tkd::Packets
