///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Config.hpp>
#include <Engine/Core/Containers/FString.hpp>
#include <Engine/Core/Object/IFunction.hpp>
#include <Engine/Core/Utils/FLogger.hpp>
#include <Engine/Network/FBinaryReader.hpp>
#include <Engine/Network/FBinaryWriter.hpp>
#include <Engine/Static/FNetworkInterface.hpp>
#include <functional>
#include <tuple>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief UFunction class template
///
/// \tparam TArgs
///
///////////////////////////////////////////////////////////////////////////////
template <typename... TArgs>
class UFunction : public IFunction
{
public:
    ///////////////////////////////////////////////////////////////////////////
    // Class Alias
    ///////////////////////////////////////////////////////////////////////////
    using Type = std::function<void(TArgs...)>;   //<! Alias for function type

private:
    ///////////////////////////////////////////////////////////////////////////
    // Class Member
    ///////////////////////////////////////////////////////////////////////////
    FString m_name;     //<! Function name
    UObject& m_owner;   //<! Owner object
    Type m_function;    //<! Function object
    ERPCType m_rpc;     //<! RPC type
    bool m_reliable;    //<! Is the RPC reliable

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor
    ///
    /// \param owner Owner object
    /// \param name Function name
    /// \param rpc RPC type (default is None)
    /// \param function Function object (default is nullptr)
    ///
    ///////////////////////////////////////////////////////////////////////////
    UFunction(
        UObject& owner,
        const FString& name,
        ERPCType rpc = ERPCType::None,
        const Type& function = nullptr,
        bool reliable = false
    )
        : m_name(name)
        , m_owner(owner)
        , m_function(function)
        , m_rpc(rpc)
        , m_reliable(reliable)
    {
        owner.RegisterFunction(this);

        auto ownerClass = owner.GetClass();
        if (ownerClass && !ownerClass->IsRegistered())
        {
            ownerClass->AddFunction(this->GetName());
        }
    }

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the function object
    ///
    /// \param function Function object
    ///
    ///////////////////////////////////////////////////////////////////////////
    UFunction& operator=(const Type& function)
    {
        m_function = function;
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the function object
    ///
    /// \param function Function object
    ///
    ///////////////////////////////////////////////////////////////////////////
    UFunction& operator=(Type&& function)
    {
        m_function = std::move(function);
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Function call operator
    ///
    /// \param args Arguments to pass to the function object
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    void operator()(TArgs... args) { Execute(args...); }

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Execute the function object
    ///
    /// \param args Arguments to pass to the function object
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Execute(TArgs... args)
    {
        if (m_rpc == ERPCType::None || !Network::IsInitialized())
        {
            if (m_function) { m_function(args...); }
            return;
        }

        if (m_rpc == ERPCType::Server)
        {
            if (m_owner.GetNetRole() == ENetRole::Authority)
            {
                if (m_function) { m_function(args...); }
            }
            else { SendRPC(args...); }
        }
        else if (m_rpc == ERPCType::Client || m_rpc == ERPCType::Multicast)
        {
            if (m_owner.GetNetRole() == ENetRole::Authority)
            {
                SendRPC(args...);
            }
            else
            {
                if (m_function) { m_function(args...); }
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Execute the function with the given parameters
    ///
    /// \param parameters Parameters to pass to the function
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void Execute(const std::vector<Byte>& parameters) override
    {
        // Deserialize parameters using tuple unpacking
        FBinaryReader reader(parameters);

        // Create a tuple to hold deserialized arguments
        std::tuple<TArgs...> args;

        // Deserialize each argument from the binary reader
        DeserializeArgs(reader, args, std::index_sequence_for<TArgs...>{});

        // Execute the function with deserialized arguments
        std::apply(
            [this](TArgs... unpackedArgs) { Execute(unpackedArgs...); }, args
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Check if the function is bound
    ///
    /// \return True if the function is bound, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual bool IsBound(void) const override
    {
        return static_cast<bool>(m_function);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the function name
    ///
    /// \return Function name
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual const FString& GetName(void) const override { return m_name; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the function object
    ///
    /// \param function Function object
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetFunction(const Type& function) { m_function = function; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the function object
    ///
    /// \param function Function object
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetFunction(Type&& function) { m_function = std::move(function); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the function object
    ///
    /// \param function Function object
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Bind(const Type& function) { m_function = function; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the function object
    ///
    /// \param function Function object
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Bind(Type&& function) { m_function = std::move(function); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets the owner UObject of the function.
    ///
    /// \return The owner UObject of the function.
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual const UObject& GetOwner(void) const override { return m_owner; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets the owner UObject of the function.
    ///
    /// \return The owner UObject of the function.
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual UObject& GetOwner(void) override { return m_owner; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the RPC type
    ///
    /// \return RPC type
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual ERPCType GetRPCType(void) const override { return m_rpc; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the RPC type
    ///
    /// \param type New RPC type
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void SetRPCType(ERPCType type) override { m_rpc = type; }

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Send an RPC if the function is marked as an RPC
    ///
    /// \param args Arguments to pass to the function object
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SendRPC(TArgs... args)
    {
        // No RPC to send or network not initialized
        if (m_rpc == ERPCType::None || !Network::IsInitialized()) { return; }

        // Check for correct RPC type and network role
        if (m_owner.GetNetRole() == ENetRole::None) { return; }

        // Serialize parameters
        std::vector<Byte> parameters;
        FBinaryWriter writer(parameters);
        (writer.Write(args), ...);

        // Create and send the RPC packet
        Packets::RemoteProcedureCall rpc;

        // Fill the RPC packet
        rpc.actorID = m_owner.GetUUID().Data();
        rpc.rpcType = m_rpc;
        rpc.functionName = m_name;
        rpc.parameters = std::move(parameters);

        // Send the RPC packet
        if (m_rpc == ERPCType::Server || m_rpc == ERPCType::Multicast)
        {
            if (m_reliable) { Network::SendReliablePacket(rpc); }
            else { Network::SendPacket(rpc); }
        }
        else
        {
            if (m_owner.GetOwningClientID() != 0 && Network::IsServer())
            {
                FConnectionInformation* info =
                    Network::GetClientInformation(m_owner.GetOwningClientID());

                if (info && info->connected)
                {
                    if (m_reliable)
                    {
                        Network::SendReliablePacket(rpc, info->endpoint);
                    }
                    else { Network::SendPacket(rpc, info->endpoint); }
                }
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Helper function to deserialize arguments into a tuple
    ///
    /// \param reader Binary reader containing serialized data
    /// \param args Tuple to store deserialized arguments
    /// \param indices Index sequence for unpacking
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename... Args, typename Indices>
    static void DeserializeArgs(
        FBinaryReader& reader, std::tuple<Args...>& args, Indices
    )
    {
        DeserializeArgsImpl(reader, args, Indices{});
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Implementation of argument deserialization
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename... Args, std::size_t... I>
    static void
        DeserializeArgsImpl(FBinaryReader& reader, std::tuple<Args...>& args, std::index_sequence<I...>)
    {
        // Deserialize each argument in sequence
        (reader.Read(std::get<I>(args)), ...);
    }
};

}   // namespace tkd
