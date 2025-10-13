///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <chrono>
#include <deque>
#include <Engine/Config.hpp>
#include <Engine/Core/Containers/FString.hpp>
#include <Engine/Debug/IDebugInterface.hpp>
#include <Engine/Network/FNetworkStatistics.hpp>
#include <Engine/Network/FPacketHeader.hpp>
#if TKD_ENGINE_CLIENT
    #include <imgui.h>
#endif

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::debug
///////////////////////////////////////////////////////////////////////////////
namespace tkd::debug
{

#if TKD_ENGINE_CLIENT

///////////////////////////////////////////////////////////////////////////////
/// \brief Packet log entry structure
///
///////////////////////////////////////////////////////////////////////////////
struct FPacketLogEntry
{
    FPacketHeader header;     //<! Packet header
    FString endpoint;         //<! Endpoint address
    bool isOutgoing;          //<! true if outgoing, false if incoming
    SizeT size;               //<! Packet size in bytes
    UInt64 timestamp;         //<! Timestamp in milliseconds
    FString packetTypeName;   //<! Human readable packet type name
    std::vector<Byte> payloadPreview;   //<! First bytes of payload for preview
};

///////////////////////////////////////////////////////////////////////////////
/// \brief Network Debug Monitor Interface
///
/// Provides a comprehensive network debugging interface that displays:
/// - Real-time network statistics
/// - Packet history (last 128 packets)
/// - Detailed packet previews
/// - Bandwidth graphs
/// - Connection information
///
///////////////////////////////////////////////////////////////////////////////
class FNetworkDebug : public IDebugInterface
{
private:
    ///////////////////////////////////////////////////////////////////////////
    // Class Constants
    ///////////////////////////////////////////////////////////////////////////
    static constexpr SizeT MAX_PACKET_HISTORY = 128;
    static constexpr SizeT MAX_HISTORY_SIZE = 300;
    static constexpr SizeT PREVIEW_BYTES = 64;

    ///////////////////////////////////////////////////////////////////////////
    // Class Member
    ///////////////////////////////////////////////////////////////////////////
    // Packet history
    std::deque<FPacketLogEntry> m_packetHistory;

    // Bandwidth history (bytes per second)
    std::vector<float> m_incomingBandwidthHistory;
    std::vector<float> m_outgoingBandwidthHistory;

    // Packet rate history (packets per second)
    std::vector<float> m_incomingPacketRateHistory;
    std::vector<float> m_outgoingPacketRateHistory;

    // Tracking variables
    UInt64 m_lastUpdateTime = 0;
    UInt64 m_lastBytesIncoming = 0;
    UInt64 m_lastBytesOutgoing = 0;
    UInt64 m_lastPacketsSent = 0;
    UInt64 m_lastPacketsReceived = 0;

    // Current statistics
    float m_currentIncomingBandwidth = 0.0f;
    float m_currentOutgoingBandwidth = 0.0f;
    float m_currentIncomingPacketRate = 0.0f;
    float m_currentOutgoingPacketRate = 0.0f;

    // Filter and display settings
    int m_selectedPacketIndex = -1;
    bool m_autoScroll = true;
    bool m_showIncoming = true;
    bool m_showOutgoing = true;
    char m_filterBuffer[256] = "";

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Show the debug interface
    ///
    /// \param settings Engine settings
    /// \param world Current world instance
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void Show(const FEngineSettings& settings, UWorld* world) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Log a packet for debugging
    ///
    /// \param header Packet header
    /// \param endpoint Endpoint address
    /// \param isOutgoing true if packet is outgoing
    /// \param payload Packet payload data
    /// \param payloadSize Size of payload
    ///
    ///////////////////////////////////////////////////////////////////////////
    void LogPacket(
        const FPacketHeader& header,
        const FString& endpoint,
        bool isOutgoing,
        const Byte* payload = nullptr,
        SizeT payloadSize = 0
    );

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Update network statistics and history
    ///
    /// \param statistics Current network statistics
    ///
    ///////////////////////////////////////////////////////////////////////////
    void UpdateStatistics(const FNetworkStatistics& statistics);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Show network statistics overview
    ///
    /// \param statistics Current network statistics
    ///
    ///////////////////////////////////////////////////////////////////////////
    void ShowStatisticsOverview(const FNetworkStatistics& statistics);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Show bandwidth graphs
    ///
    ///////////////////////////////////////////////////////////////////////////
    void ShowBandwidthGraphs(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Show packet rate graphs
    ///
    ///////////////////////////////////////////////////////////////////////////
    void ShowPacketRateGraphs(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Show packet history list
    ///
    ///////////////////////////////////////////////////////////////////////////
    void ShowPacketHistory(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Show detailed packet information
    ///
    ///////////////////////////////////////////////////////////////////////////
    void ShowPacketDetails(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get packet type name from packet type ID
    ///
    /// \param packetType Packet type ID
    ///
    /// \return Human readable packet type name
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString GetPacketTypeName(UInt16 packetType) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get current timestamp in milliseconds
    ///
    /// \return Current timestamp
    ///
    ///////////////////////////////////////////////////////////////////////////
    UInt64 GetCurrentTimestamp(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Format bytes to human readable string
    ///
    /// \param bytes Number of bytes
    ///
    /// \return Formatted string
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString FormatBytes(UInt64 bytes) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Render a graph with the given data
    ///
    /// \param label Graph label
    /// \param data Data points
    /// \param unit Unit string (e.g., "KB/s")
    /// \param color Graph color
    /// \param height Graph height
    ///
    ///////////////////////////////////////////////////////////////////////////
    void RenderGraph(
        const char* label,
        const std::vector<float>& data,
        const char* unit,
        ImVec4 color,
        float height = 80.0f
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Render packet flags as colored badges
    ///
    /// \param flags Packet flags
    ///
    ///////////////////////////////////////////////////////////////////////////
    void RenderPacketFlags(UInt16 flags);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Render hex dump of packet data
    ///
    /// \param data Data to display
    /// \param size Size of data
    ///
    ///////////////////////////////////////////////////////////////////////////
    void RenderHexDump(const Byte* data, SizeT size);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Preview packet content (generic/fallback)
    ///
    /// \param payload Packet payload data
    /// \param payloadSize Size of payload
    /// \param packetType Type of packet
    ///
    ///////////////////////////////////////////////////////////////////////////
    void PreviewPacket(
        const Byte* payload, SizeT payloadSize, UInt16 packetType
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Preview Connect packet content
    ///
    /// \param payload Packet payload data
    /// \param payloadSize Size of payload
    ///
    ///////////////////////////////////////////////////////////////////////////
    void PreviewConnectPacket(const Byte* payload, SizeT payloadSize);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Preview ConnectResponse packet content
    ///
    /// \param payload Packet payload data
    /// \param payloadSize Size of payload
    ///
    ///////////////////////////////////////////////////////////////////////////
    void PreviewConnectResponsePacket(const Byte* payload, SizeT payloadSize);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Preview Disconnect packet content
    ///
    /// \param payload Packet payload data
    /// \param payloadSize Size of payload
    ///
    ///////////////////////////////////////////////////////////////////////////
    void PreviewDisconnectPacket(const Byte* payload, SizeT payloadSize);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Preview HeartBeat packet content
    ///
    /// \param payload Packet payload data
    /// \param payloadSize Size of payload
    ///
    ///////////////////////////////////////////////////////////////////////////
    void PreviewHeartBeatPacket(const Byte* payload, SizeT payloadSize);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Preview RemoteProcedureCall packet content
    ///
    /// \param payload Packet payload data
    /// \param payloadSize Size of payload
    ///
    ///////////////////////////////////////////////////////////////////////////
    void PreviewRemoteProcedureCallPacket(
        const Byte* payload, SizeT payloadSize
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Preview Acknowledgment packet content
    ///
    /// \param payload Packet payload data
    /// \param payloadSize Size of payload
    ///
    ///////////////////////////////////////////////////////////////////////////
    void PreviewAcknowledgmentPacket(const Byte* payload, SizeT payloadSize);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Preview Replication packet content
    ///
    /// \param payload Packet payload data
    /// \param payloadSize Size of payload
    ///
    ///////////////////////////////////////////////////////////////////////////
    void PreviewReplicationPacket(const Byte* payload, SizeT payloadSize);
};

#endif

}   // namespace tkd::debug
