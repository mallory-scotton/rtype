///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <asio.hpp>
#include <Engine/Config.hpp>
#include <Engine/Network/Asio.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief Connection information structure
///
/// This structure holds information about a network connection, including
/// the remote endpoint, client ID, sequence numbers for sent and received
/// packets, last activity timestamp, round trip time (RTT), and connection
/// status.
///
///////////////////////////////////////////////////////////////////////////////
struct FConnectionInformation
{
    FEndpoint endpoint;                //<! Remote endpoint
    UInt32 clientID = 0;               //<! Unique client identifier
    UInt32 lastSequenceReceived = 0;   //<! Last sequence number received
    UInt32 lastSequenceSent = 0;       //<! Last sequence number sent
    TimePoint lastActivity;            //<! Last activity timestamp
    Float32 rtt = 0.0f;                //<! Round trip time in milliseconds
    bool connected = false;            //<! Connection status
};

}   // namespace tkd
