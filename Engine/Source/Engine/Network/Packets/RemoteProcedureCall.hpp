///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Config.hpp>
#include <Engine/Core/Object/IFunction.hpp>
#include <Engine/Network/Enumerations.hpp>
#include <Engine/Network/FBinaryWriter.hpp>
#include <Engine/Network/TPacket.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::Packets
///////////////////////////////////////////////////////////////////////////////
namespace tkd::Packets
{

///////////////////////////////////////////////////////////////////////////////
/// \brief Packet for Remote Procedure Call (RPC)
///
/// This packet is used to invoke functions on remote actors across the
/// network. It contains the actor ID, the type of RPC, the function name, and
/// the parameters.
///
///////////////////////////////////////////////////////////////////////////////
class RemoteProcedureCall : public TPacket<RemoteProcedureCall>
{
public:
    ///////////////////////////////////////////////////////////////////////////
    // Class Member
    ///////////////////////////////////////////////////////////////////////////
    std::array<Byte, 16> actorID;          //<! The ID of the actor
    ERPCType rpcType = ERPCType::Server;   //<! The type of RPC (ERPCType)
    FString functionName;                  //<! The name of the function
    std::vector<Byte> parameters;          //<! Serialized parameters

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor
    ///
    ///////////////////////////////////////////////////////////////////////////
    RemoteProcedureCall(void) = default;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor with parameters
    ///
    /// \param name Name of the function to call
    /// \param type Type of RPC (Client or Server)
    /// \param uuid UUID of the target actor (default is nil UUID)
    ///
    ///////////////////////////////////////////////////////////////////////////
    RemoteProcedureCall(
        const FString& name, ERPCType type, const UUID& uuid = UUID::Nil
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param name
    /// \param type
    /// \param uuid
    /// \param params
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename... Args>
    RemoteProcedureCall(
        const FString& name, ERPCType type, const UUID& uuid, Args&&... params
    )
        : actorID(uuid.Data())
        , rpcType(type)
        , functionName(name)
        , parameters()
    {
        // Serialize parameters
        std::vector<Byte> parameters;
        FBinaryWriter writer(parameters);
        (writer.Write(params), ...);
        this->parameters = std::move(parameters);
    }

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Serialize the packet data into the writer
    ///
    /// \param writer Binary writer to serialize data into
    ///
    /// \return true if serialization was successful, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool Serialize(FBinaryWriter& writer) const override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Deserialize the packet data from the reader
    ///
    /// \param reader Binary reader to deserialize data from
    ///
    /// \return true if deserialization was successful, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool Deserialize(FBinaryReader& reader) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the size of the packet in bytes
    ///
    /// \return size of the packet in bytes
    ///
    ///////////////////////////////////////////////////////////////////////////
    SizeT GetSize(void) const override;
};

}   // namespace tkd::Packets
