///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Static/FNetworkInterface.hpp>
#include <Engine/Core/Utils/FLogger.hpp>
#include <Engine/Runtime/World/UWorld.hpp>
#include <Engine/Static/Engine.hpp>
#include <Engine/Static/FNetworkSubsystem.hpp>
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
std::mutex FNetworkInterface::s_mutex;
__internal::FNetworkSubsystem* FNetworkInterface::s_networkSubsystem = nullptr;

///////////////////////////////////////////////////////////////////////////////
void FNetworkInterface::Setup(__internal::FNetworkSubsystem* subsystem)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    s_networkSubsystem = subsystem;
}

///////////////////////////////////////////////////////////////////////////////
__internal::FNetworkSubsystem* FNetworkInterface::GetSubsystem(void)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    return s_networkSubsystem;
}

///////////////////////////////////////////////////////////////////////////////
Bool FNetworkInterface::IsInitialized(void)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    return s_networkSubsystem != nullptr;
}

///////////////////////////////////////////////////////////////////////////////
Bool FNetworkInterface::SendData(const std::vector<Byte>& data)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    if (s_networkSubsystem == nullptr)
    {
        FLogger::SetNamespace("Network");
        FLogger::Warn("Network subsystem is not initialized.");
        return false;
    }
    return s_networkSubsystem->SendData(data);
}

///////////////////////////////////////////////////////////////////////////////
Bool FNetworkInterface::SendData(
    const std::vector<Byte>& data, const FEndpoint& endpoint
)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    if (s_networkSubsystem == nullptr)
    {
        FLogger::SetNamespace("Network");
        FLogger::Warn("Network subsystem is not initialized.");
        return false;
    }
    return s_networkSubsystem->SendData(data, endpoint);
}

///////////////////////////////////////////////////////////////////////////////
Bool FNetworkInterface::SendData(
    const std::vector<Byte>& data, const std::vector<FEndpoint>& endpoints
)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    if (s_networkSubsystem == nullptr)
    {
        FLogger::SetNamespace("Network");
        FLogger::Warn("Network subsystem is not initialized.");
        return false;
    }
    return s_networkSubsystem->SendData(data, endpoints);
}

///////////////////////////////////////////////////////////////////////////////
Bool FNetworkInterface::SendPacket(const IPacket& packet)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    if (s_networkSubsystem == nullptr)
    {
        FLogger::SetNamespace("Network");
        FLogger::Warn("Network subsystem is not initialized.");
        return false;
    }
    return s_networkSubsystem->SendPacket(packet);
}

///////////////////////////////////////////////////////////////////////////////
Bool FNetworkInterface::SendPacket(const IPacket& packet, UInt32 clientID)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    if (s_networkSubsystem == nullptr)
    {
        FLogger::SetNamespace("Network");
        FLogger::Warn("Network subsystem is not initialized.");
        return false;
    }

    auto server = s_networkSubsystem->GetServer();
    if (server == nullptr)
    {
        FLogger::SetNamespace("Network");
        FLogger::Warn("Network subsystem is not in server mode.");
        return false;
    }

    auto info = server->GetClientInformation(clientID);
    if (info == nullptr)
    {
        FLogger::SetNamespace("Network");
        FLogger::Warn("Client not found.");
        return false;
    }

    return s_networkSubsystem->SendPacket(packet, info->endpoint);
}

///////////////////////////////////////////////////////////////////////////////
Bool FNetworkInterface::SendPacket(
    const IPacket& packet, const FEndpoint& endpoint
)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    if (s_networkSubsystem == nullptr)
    {
        FLogger::SetNamespace("Network");
        FLogger::Warn("Network subsystem is not initialized.");
        return false;
    }
    return s_networkSubsystem->SendPacket(packet, endpoint);
}

///////////////////////////////////////////////////////////////////////////////
Bool FNetworkInterface::SendPacket(
    const IPacket& packet, const std::vector<FEndpoint>& endpoints
)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    if (s_networkSubsystem == nullptr)
    {
        FLogger::SetNamespace("Network");
        FLogger::Warn("Network subsystem is not initialized.");
        return false;
    }
    return s_networkSubsystem->SendPacket(packet, endpoints);
}

///////////////////////////////////////////////////////////////////////////////
Bool FNetworkInterface::SendReliablePacket(const IPacket& packet)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    if (s_networkSubsystem == nullptr)
    {
        FLogger::SetNamespace("Network");
        FLogger::Warn("Network subsystem is not initialized.");
        return false;
    }
    return s_networkSubsystem->SendReliablePacket(packet);
}

///////////////////////////////////////////////////////////////////////////////
Bool FNetworkInterface::SendReliablePacket(
    const IPacket& packet, UInt32 clientID
)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    if (s_networkSubsystem == nullptr)
    {
        FLogger::SetNamespace("Network");
        FLogger::Warn("Network subsystem is not initialized.");
        return false;
    }

    auto server = s_networkSubsystem->GetServer();
    if (server == nullptr)
    {
        FLogger::SetNamespace("Network");
        FLogger::Warn("Network subsystem is not in server mode.");
        return false;
    }

    auto info = server->GetClientInformation(clientID);
    if (info == nullptr)
    {
        FLogger::SetNamespace("Network");
        FLogger::Warn("Client not found.");
        return false;
    }

    return s_networkSubsystem->SendReliablePacket(packet, info->endpoint);
}

///////////////////////////////////////////////////////////////////////////////
Bool FNetworkInterface::SendReliablePacket(
    const IPacket& packet, const FEndpoint& endpoint
)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    if (s_networkSubsystem == nullptr)
    {
        FLogger::SetNamespace("Network");
        FLogger::Warn("Network subsystem is not initialized.");
        return false;
    }
    return s_networkSubsystem->SendReliablePacket(packet, endpoint);
}

///////////////////////////////////////////////////////////////////////////////
Bool FNetworkInterface::SendReliablePacket(
    const IPacket& packet, const std::vector<FEndpoint>& endpoints
)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    if (s_networkSubsystem == nullptr)
    {
        FLogger::SetNamespace("Network");
        FLogger::Warn("Network subsystem is not initialized.");
        return false;
    }
    return s_networkSubsystem->SendReliablePacket(packet, endpoints);
}

///////////////////////////////////////////////////////////////////////////////
Bool FNetworkInterface::BroadcastData(const std::vector<Byte>& data)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    if (s_networkSubsystem == nullptr)
    {
        FLogger::SetNamespace("Network");
        FLogger::Warn("Network subsystem is not initialized.");
        return false;
    }
    return s_networkSubsystem->BroadcastData(data);
}

///////////////////////////////////////////////////////////////////////////////
Bool FNetworkInterface::BroadcastPacket(const IPacket& packet)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    if (s_networkSubsystem == nullptr)
    {
        FLogger::SetNamespace("Network");
        FLogger::Warn("Network subsystem is not initialized.");
        return false;
    }
    return s_networkSubsystem->BroadcastPacket(packet);
}

///////////////////////////////////////////////////////////////////////////////
FNetworkStatistics FNetworkInterface::GetStatistics(void)
{
    static FNetworkStatistics emptyStats;
    std::lock_guard<std::mutex> lock(s_mutex);
    if (s_networkSubsystem == nullptr)
    {
        FLogger::SetNamespace("Network");
        FLogger::Warn("Network subsystem is not initialized.");
        return emptyStats;
    }
    return s_networkSubsystem->GetStatistics();
}

///////////////////////////////////////////////////////////////////////////////
Bool FNetworkInterface::Connect(const FString& address, UInt16 port)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    if (s_networkSubsystem == nullptr)
    {
        FLogger::SetNamespace("Network");
        FLogger::Warn("Network subsystem is not initialized.");
        return false;
    }

    FNetworkClient* client = s_networkSubsystem->GetClient();
    if (client == nullptr)
    {
        FLogger::SetNamespace("Network");
        FLogger::Warn("Network subsystem is not in client mode.");
        return false;
    }

    return client->Connect(address, port);
}

///////////////////////////////////////////////////////////////////////////////
Bool FNetworkInterface::Connect(const FEndpoint& endpoint)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    if (s_networkSubsystem == nullptr)
    {
        FLogger::SetNamespace("Network");
        FLogger::Warn("Network subsystem is not initialized.");
        return false;
    }

    FNetworkClient* client = s_networkSubsystem->GetClient();
    if (client == nullptr)
    {
        FLogger::SetNamespace("Network");
        FLogger::Warn("Network subsystem is not in client mode.");
        return false;
    }

    std::string address = endpoint.address().to_string();
    UInt16 port = endpoint.port();

    return client->Connect(address, port);
}

///////////////////////////////////////////////////////////////////////////////
Bool FNetworkInterface::Disconnect(
    EDisconnectionReason reason /*= EDisconnectionReason::Unknown*/
)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    if (s_networkSubsystem == nullptr)
    {
        FLogger::SetNamespace("Network");
        FLogger::Warn("Network subsystem is not initialized.");
        return false;
    }

    FNetworkClient* client = s_networkSubsystem->GetClient();
    if (client == nullptr)
    {
        FLogger::SetNamespace("Network");
        FLogger::Warn("Network subsystem is not in client mode.");
        return false;
    }

    client->Disconnect(reason);
    return true;
}

///////////////////////////////////////////////////////////////////////////////
Bool FNetworkInterface::IsConnected(void)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    if (s_networkSubsystem == nullptr)
    {
        FLogger::SetNamespace("Network");
        FLogger::Warn("Network subsystem is not initialized.");
        return false;
    }

    FNetworkClient* client = s_networkSubsystem->GetClient();
    if (client == nullptr)
    {
        FLogger::SetNamespace("Network");
        FLogger::Warn("Network subsystem is not in client mode.");
        return false;
    }

    return client->IsConnected();
}

///////////////////////////////////////////////////////////////////////////////
Bool FNetworkInterface::IsClient(void)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    if (s_networkSubsystem == nullptr)
    {
        FLogger::SetNamespace("Network");
        FLogger::Warn("Network subsystem is not initialized.");
        return false;
    }
    return s_networkSubsystem->GetClient() != nullptr;
}

///////////////////////////////////////////////////////////////////////////////
Bool FNetworkInterface::IsServer(void)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    if (s_networkSubsystem == nullptr)
    {
        FLogger::SetNamespace("Network");
        FLogger::Warn("Network subsystem is not initialized.");
        return false;
    }
    return s_networkSubsystem->GetServer() != nullptr;
}

///////////////////////////////////////////////////////////////////////////////
FConnectionInformation* FNetworkInterface::GetClientInformation(UInt32 clientID
)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    if (s_networkSubsystem == nullptr)
    {
        FLogger::SetNamespace("Network");
        FLogger::Warn("Network subsystem is not initialized.");
        return nullptr;
    }

    FNetworkServer* server = s_networkSubsystem->GetServer();
    if (server == nullptr)
    {
        FLogger::SetNamespace("Network");
        FLogger::Warn("Network subsystem is not in server mode.");
        return nullptr;
    }

    return server->GetClientInformation(clientID);
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkInterface::ProcessDeferredRPCs(UWorld& world)
{
    // DO NOT LOCK s_mutex HERE!
    // If we lock it, and an RPC calls any Network:: function during execution,
    // we'll deadlock because those functions also need s_mutex
    // The subsystem pointer is stable after initialization, so no lock needed
    if (s_networkSubsystem == nullptr) { return; }
    s_networkSubsystem->ProcessDeferredRPCs(world);
}

///////////////////////////////////////////////////////////////////////////////
UInt32 FNetworkInterface::GetClientID(void)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    if (s_networkSubsystem == nullptr) { return 0; }

    FNetworkClient* client = s_networkSubsystem->GetClient();
    if (client == nullptr) { return 0; }

    return client->GetClientID().value_or(0);
}

}   // namespace tkd
