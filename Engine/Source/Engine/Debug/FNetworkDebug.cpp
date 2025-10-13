///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Debug/FNetworkDebug.hpp>
#include <Engine/Static/FEngineInterface.hpp>
#include <Engine/Static/FNetworkInterface.hpp>
#include <iomanip>
#include <sstream>
#if TKD_ENGINE_CLIENT
    #include <Engine/Network/FBinaryReader.hpp>
    #include <Engine/Network/Packets.hpp>
    #include <imgui.h>
    #include <imgui_internal.h>
#endif

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::debug
///////////////////////////////////////////////////////////////////////////////
namespace tkd::debug
{

#if TKD_ENGINE_CLIENT

///////////////////////////////////////////////////////////////////////////////
void FNetworkDebug::Show(const FEngineSettings& settings, UWorld* world)
{
    TKD_UNUSED(settings);
    TKD_UNUSED(world);

    ImGui::SetNextWindowSize(ImVec2(1000, 700), ImGuiCond_FirstUseEver);
    ImGui::Begin("Network Monitor", nullptr, ImGuiWindowFlags_MenuBar);

    // Get network statistics from the network subsystem
    auto networkSubsystem = Engine::GetInstance().GetNetwork();
    if (!networkSubsystem)
    {
        ImGui::TextColored(
            ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Network subsystem not available"
        );
        ImGui::End();
        return;
    }

    const auto& statistics = networkSubsystem->GetStatistics();
    UpdateStatistics(statistics);

    // Menu bar
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Options"))
        {
            ImGui::MenuItem("Auto-scroll", nullptr, &m_autoScroll);
            ImGui::Separator();
            if (ImGui::MenuItem("Clear History"))
            {
                m_packetHistory.clear();
                m_selectedPacketIndex = -1;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    // Statistics Overview
    ShowStatisticsOverview(statistics);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Graphs section
    if (ImGui::CollapsingHeader(
            "Bandwidth & Packet Rate", ImGuiTreeNodeFlags_DefaultOpen
        ))
    {
        ShowBandwidthGraphs();
        ImGui::Spacing();
        ShowPacketRateGraphs();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Packet History section
    if (ImGui::CollapsingHeader(
            "Packet History", ImGuiTreeNodeFlags_DefaultOpen
        ))
    {
        ImGui::BeginChild("PacketHistorySection", ImVec2(0, 0), false);

        ImGui::Columns(2, "packet_columns", true);
        static bool firstTime = true;
        if (firstTime)
        {
            ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() * 0.5f);
            firstTime = false;
        }

        // Left column: Packet list
        ShowPacketHistory();

        ImGui::NextColumn();

        // Right column: Packet details
        ShowPacketDetails();

        ImGui::Columns(1);
        ImGui::EndChild();
    }

    ImGui::End();
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkDebug::LogPacket(
    const FPacketHeader& header,
    const FString& endpoint,
    bool isOutgoing,
    const Byte* payload,
    SizeT payloadSize
)
{
    FPacketLogEntry entry;
    entry.header = header;
    entry.endpoint = endpoint;
    entry.isOutgoing = isOutgoing;
    entry.size = FPacketHeader::SIZE + payloadSize;
    entry.timestamp = GetCurrentTimestamp();
    entry.packetTypeName = GetPacketTypeName(header.packetType);

    // Store preview of payload
    if (payload && payloadSize > 0)
    {
        SizeT previewSize = std::min(payloadSize, PREVIEW_BYTES);
        entry.payloadPreview.assign(payload, payload + previewSize);
    }

    // Add to history
    m_packetHistory.push_back(entry);

    // Maintain history size limit
    while (m_packetHistory.size() > MAX_PACKET_HISTORY)
    {
        m_packetHistory.pop_front();
        if (m_selectedPacketIndex > 0) { m_selectedPacketIndex--; }
    }
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkDebug::UpdateStatistics(const FNetworkStatistics& statistics)
{
    UInt64 currentTime = GetCurrentTimestamp();

    if (m_lastUpdateTime == 0)
    {
        m_lastUpdateTime = currentTime;
        m_lastBytesIncoming = statistics.bytesIncoming;
        m_lastBytesOutgoing = statistics.bytesOutgoing;
        m_lastPacketsSent = statistics.packetsSent;
        m_lastPacketsReceived = statistics.packetsReceived;
        return;
    }

    // Calculate time delta in seconds
    float deltaTime = (currentTime - m_lastUpdateTime) / 1000.0f;

    if (deltaTime >= 0.1f)   // Update every 100ms
    {
        // Calculate bandwidth (bytes per second)
        UInt64 bytesInDelta = statistics.bytesIncoming - m_lastBytesIncoming;
        UInt64 bytesOutDelta = statistics.bytesOutgoing - m_lastBytesOutgoing;

        m_currentIncomingBandwidth = (bytesInDelta / deltaTime);
        m_currentOutgoingBandwidth = (bytesOutDelta / deltaTime);

        // Calculate packet rate (packets per second)
        UInt64 packetsInDelta =
            statistics.packetsReceived - m_lastPacketsReceived;
        UInt64 packetsOutDelta = statistics.packetsSent - m_lastPacketsSent;

        m_currentIncomingPacketRate = (packetsInDelta / deltaTime);
        m_currentOutgoingPacketRate = (packetsOutDelta / deltaTime);

        // Update history
        m_incomingBandwidthHistory.push_back(
            m_currentIncomingBandwidth / 1024.0f
        );   // Convert to KB/s
        m_outgoingBandwidthHistory.push_back(
            m_currentOutgoingBandwidth / 1024.0f
        );
        m_incomingPacketRateHistory.push_back(m_currentIncomingPacketRate);
        m_outgoingPacketRateHistory.push_back(m_currentOutgoingPacketRate);

        // Maintain history size
        while (m_incomingBandwidthHistory.size() > MAX_HISTORY_SIZE)
        {
            m_incomingBandwidthHistory.erase(m_incomingBandwidthHistory.begin()
            );
        }
        while (m_outgoingBandwidthHistory.size() > MAX_HISTORY_SIZE)
        {
            m_outgoingBandwidthHistory.erase(m_outgoingBandwidthHistory.begin()
            );
        }
        while (m_incomingPacketRateHistory.size() > MAX_HISTORY_SIZE)
        {
            m_incomingPacketRateHistory.erase(
                m_incomingPacketRateHistory.begin()
            );
        }
        while (m_outgoingPacketRateHistory.size() > MAX_HISTORY_SIZE)
        {
            m_outgoingPacketRateHistory.erase(
                m_outgoingPacketRateHistory.begin()
            );
        }

        // Update tracking variables
        m_lastUpdateTime = currentTime;
        m_lastBytesIncoming = statistics.bytesIncoming;
        m_lastBytesOutgoing = statistics.bytesOutgoing;
        m_lastPacketsSent = statistics.packetsSent;
        m_lastPacketsReceived = statistics.packetsReceived;
    }
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkDebug::ShowStatisticsOverview(const FNetworkStatistics& statistics
)
{
    ImGui::Text("Network Statistics");
    ImGui::Separator();

    ImGui::Columns(5, "stats_columns", false);

    // Total packets
    ImGui::Text("Packets Sent");
    ImGui::TextColored(
        ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "%llu", statistics.packetsSent
    );
    ImGui::NextColumn();

    ImGui::Text("Packets Received");
    ImGui::TextColored(
        ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "%llu", statistics.packetsReceived
    );
    ImGui::NextColumn();

    ImGui::Text("Packets Dropped");
    ImVec4 dropColor = (statistics.packetsDropped > 0)
                           ? ImVec4(1.0f, 0.0f, 0.0f, 1.0f)
                           : ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
    ImGui::TextColored(dropColor, "%llu", statistics.packetsDropped);
    ImGui::NextColumn();

    ImGui::Text("Avg RTT");
    ImGui::TextColored(
        ImVec4(1.0f, 1.0f, 0.4f, 1.0f), "%.2f ms", statistics.averageRTT
    );
    ImGui::NextColumn();

    ImGui::Text("Packet Loss");
    UInt64 totalPackets = statistics.packetsSent + statistics.packetsReceived;
    float lossRate = (totalPackets > 0)
                         ? (static_cast<float>(statistics.packetsDropped) /
                            totalPackets * 100.0f)
                         : 0.0f;
    ImVec4 lossColor = (lossRate > 5.0f)   ? ImVec4(1.0f, 0.0f, 0.0f, 1.0f)
                       : (lossRate > 1.0f) ? ImVec4(1.0f, 1.0f, 0.0f, 1.0f)
                                           : ImVec4(0.4f, 1.0f, 0.4f, 1.0f);
    ImGui::TextColored(lossColor, "%.2f%%", lossRate);
    ImGui::NextColumn();

    ImGui::Columns(1);
    ImGui::Spacing();

    // Bandwidth information
    ImGui::Columns(4, "bandwidth_columns", false);

    ImGui::Text("Total Sent");
    ImGui::TextColored(
        ImVec4(0.4f, 0.8f, 1.0f, 1.0f),
        "%s",
        FormatBytes(statistics.bytesOutgoing).CStr()
    );
    ImGui::NextColumn();

    ImGui::Text("Total Received");
    ImGui::TextColored(
        ImVec4(0.4f, 1.0f, 0.4f, 1.0f),
        "%s",
        FormatBytes(statistics.bytesIncoming).CStr()
    );
    ImGui::NextColumn();

    ImGui::Text("Upload Rate");
    ImGui::TextColored(
        ImVec4(0.4f, 0.8f, 1.0f, 1.0f),
        "%.2f KB/s",
        m_currentOutgoingBandwidth / 1024.0f
    );
    ImGui::NextColumn();

    ImGui::Text("Download Rate");
    ImGui::TextColored(
        ImVec4(0.4f, 1.0f, 0.4f, 1.0f),
        "%.2f KB/s",
        m_currentIncomingBandwidth / 1024.0f
    );
    ImGui::NextColumn();

    ImGui::Columns(1);
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkDebug::ShowBandwidthGraphs(void)
{
    ImGui::Columns(2, "bandwidth_graphs", false);

    // Incoming bandwidth
    RenderGraph(
        "Incoming Bandwidth",
        m_incomingBandwidthHistory,
        "KB/s",
        ImVec4(0.4f, 1.0f, 0.4f, 1.0f)
    );

    ImGui::NextColumn();

    // Outgoing bandwidth
    RenderGraph(
        "Outgoing Bandwidth",
        m_outgoingBandwidthHistory,
        "KB/s",
        ImVec4(0.4f, 0.8f, 1.0f, 1.0f)
    );

    ImGui::Columns(1);
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkDebug::ShowPacketRateGraphs(void)
{
    ImGui::Columns(2, "packet_rate_graphs", false);

    // Incoming packet rate
    RenderGraph(
        "Incoming Packet Rate",
        m_incomingPacketRateHistory,
        "pkt/s",
        ImVec4(0.4f, 1.0f, 0.4f, 1.0f)
    );

    ImGui::NextColumn();

    // Outgoing packet rate
    RenderGraph(
        "Outgoing Packet Rate",
        m_outgoingPacketRateHistory,
        "pkt/s",
        ImVec4(0.4f, 0.8f, 1.0f, 1.0f)
    );

    ImGui::Columns(1);
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkDebug::ShowPacketHistory(void)
{
    ImGui::BeginChild("PacketList", ImVec2(0, 0), true);

    // Filter controls
    ImGui::Text("Filters:");
    ImGui::SameLine();
    ImGui::Checkbox("Incoming", &m_showIncoming);
    ImGui::SameLine();
    ImGui::Checkbox("Outgoing", &m_showOutgoing);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(200.0f);
    ImGui::InputText("##filter", m_filterBuffer, sizeof(m_filterBuffer));

    ImGui::Separator();

    // Table header
    ImGui::Text("Time");
    ImGui::SameLine(80);
    ImGui::Text("Dir");
    ImGui::SameLine(120);
    ImGui::Text("Type");
    ImGui::SameLine(250);
    ImGui::Text("Size");
    ImGui::SameLine(320);
    ImGui::Text("Endpoint");
    ImGui::Separator();

    // Packet list
    FString filterStr(m_filterBuffer);
    int displayIndex = 0;

    for (int i = static_cast<int>(m_packetHistory.size()) - 1; i >= 0; i--)
    {
        const auto& entry = m_packetHistory[i];

        // Apply filters
        if (!m_showIncoming && !entry.isOutgoing) { continue; }
        if (!m_showOutgoing && entry.isOutgoing) { continue; }

        if (!filterStr.IsEmpty())
        {
            bool matches =
                entry.packetTypeName.Find(filterStr) != FString::NPOS ||
                entry.endpoint.Find(filterStr) != FString::NPOS;
            if (!matches) { continue; }
        }

        // Format time
        UInt64 currentTime = GetCurrentTimestamp();
        float ageSeconds = (currentTime - entry.timestamp) / 1000.0f;
        char timeStr[32];
        snprintf(timeStr, sizeof(timeStr), "%.0fs", ageSeconds);

        // Direction icon and color
        const char* dirIcon = entry.isOutgoing ? "^" : "v";
        ImVec4 dirColor = entry.isOutgoing ? ImVec4(0.4f, 0.8f, 1.0f, 1.0f)
                                           : ImVec4(0.4f, 1.0f, 0.4f, 1.0f);

        // Selection
        bool isSelected = (m_selectedPacketIndex == i);
        if (ImGui::Selectable(
                ("##packet" + std::to_string(i)).c_str(),
                isSelected,
                ImGuiSelectableFlags_SpanAllColumns
            ))
        {
            m_selectedPacketIndex = i;
        }

        ImGui::SameLine(0);
        ImGui::Text("%s", timeStr);
        ImGui::SameLine(80);
        ImGui::TextColored(dirColor, "%s", dirIcon);
        ImGui::SameLine(120);
        ImGui::Text("%s", entry.packetTypeName.CStr());
        ImGui::SameLine(250);
        ImGui::Text("%zu B", entry.size);
        ImGui::SameLine(320);
        ImGui::TextColored(
            ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "%s", entry.endpoint.CStr()
        );

        displayIndex++;
    }

    // Auto-scroll to bottom
    if (m_autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
    {
        ImGui::SetScrollHereY(1.0f);
    }

    ImGui::EndChild();
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkDebug::ShowPacketDetails(void)
{
    ImGui::BeginChild("PacketDetails", ImVec2(0, 0), true);

    if (m_selectedPacketIndex < 0 ||
        m_selectedPacketIndex >= static_cast<int>(m_packetHistory.size()))
    {
        ImGui::TextColored(
            ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Select a packet to view details"
        );
        ImGui::EndChild();
        return;
    }

    const auto& entry = m_packetHistory[m_selectedPacketIndex];

    ImGui::Text("Packet Details");
    ImGui::Separator();
    ImGui::Spacing();

    // Basic information
    ImGui::Text("Type:");
    ImGui::SameLine(150);
    ImGui::TextColored(
        ImVec4(1.0f, 1.0f, 0.4f, 1.0f), "%s", entry.packetTypeName.CStr()
    );

    ImGui::Text("Direction:");
    ImGui::SameLine(150);
    ImVec4 dirColor = entry.isOutgoing ? ImVec4(0.4f, 0.8f, 1.0f, 1.0f)
                                       : ImVec4(0.4f, 1.0f, 0.4f, 1.0f);
    ImGui::TextColored(
        dirColor, "%s", entry.isOutgoing ? "Outgoing" : "Incoming"
    );

    ImGui::Text("Endpoint:");
    ImGui::SameLine(150);
    ImGui::Text("%s", entry.endpoint.CStr());

    ImGui::Text("Size:");
    ImGui::SameLine(150);
    ImGui::Text("%zu bytes", entry.size);

    ImGui::Text("Timestamp:");
    ImGui::SameLine(150);
    ImGui::Text("%llu ms", entry.timestamp);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Header information
    ImGui::Text("Header Information");
    ImGui::Separator();

    ImGui::Text("Magic:");
    ImGui::SameLine(150);
    ImGui::Text("0x%08X", entry.header.magic);

    ImGui::Text("Protocol Version:");
    ImGui::SameLine(150);
    ImGui::Text("%u", entry.header.protocolVersion);

    ImGui::Text("Flags:");
    ImGui::SameLine(150);
    RenderPacketFlags(entry.header.flags);

    ImGui::Text("Packet Type:");
    ImGui::SameLine(150);
    ImGui::Text("%u", entry.header.packetType);

    ImGui::Text("Packet Size:");
    ImGui::SameLine(150);
    ImGui::Text("%u bytes", entry.header.packetSize);

    ImGui::Text("Sequence Number:");
    ImGui::SameLine(150);
    ImGui::Text("%u", entry.header.sequenceNumber);

    ImGui::Text("Header Timestamp:");
    ImGui::SameLine(150);
    ImGui::Text("%u", entry.header.timestamp);

    ImGui::Text("Checksum:");
    ImGui::SameLine(150);
    ImGui::Text("0x%08X", entry.header.checksum);

    // Payload preview with tabs
    if (!entry.payloadPreview.empty())
    {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Text(
            "Payload Preview (%zu bytes)", entry.payloadPreview.size()
        );
        ImGui::Separator();

        if (ImGui::BeginTabBar("PayloadPreviewTabs"))
        {
            // Hex Dump Tab
            if (ImGui::BeginTabItem("Hex Dump"))
            {
                ImGui::BeginChild("HexDumpContent", ImVec2(0, 0), false);
                RenderHexDump(
                    entry.payloadPreview.data(), entry.payloadPreview.size()
                );
                ImGui::EndChild();
                ImGui::EndTabItem();
            }

            // Content Preview Tab
            if (ImGui::BeginTabItem("Content"))
            {
                ImGui::BeginChild("ContentPreview", ImVec2(0, 0), false);
                PreviewPacket(
                    entry.payloadPreview.data(),
                    entry.payloadPreview.size(),
                    entry.header.packetType
                );
                ImGui::EndChild();
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
    }

    ImGui::EndChild();
}

///////////////////////////////////////////////////////////////////////////////
FString FNetworkDebug::GetPacketTypeName(UInt16 packetType) const
{
    if (packetType == Packets::Connect::GetStaticType()) { return "Connect"; }
    else if (packetType == Packets::ConnectResponse::GetStaticType())
    {
        return "ConnectResponse";
    }
    else if (packetType == Packets::Disconnect::GetStaticType())
    {
        return "Disconnect";
    }
    else if (packetType == Packets::HeartBeat::GetStaticType())
    {
        return "HeartBeat";
    }
    else if (packetType == Packets::RemoteProcedureCall::GetStaticType())
    {
        return "RemoteProcedureCall";
    }
    else if (packetType == Packets::Acknowledgment::GetStaticType())
    {
        return "Acknowledgment";
    }
    else if (packetType == Packets::Replication::GetStaticType())
    {
        return "Replication";
    }
    return FString::Format("Unknown ({0})", packetType);
}

///////////////////////////////////////////////////////////////////////////////
UInt64 FNetworkDebug::GetCurrentTimestamp(void) const
{
    using namespace std::chrono;
    auto now = steady_clock::now();
    auto epoch = now.time_since_epoch();
    return duration_cast<milliseconds>(epoch).count();
}

///////////////////////////////////////////////////////////////////////////////
FString FNetworkDebug::FormatBytes(UInt64 bytes) const
{
    const char* units[] = { "B", "KB", "MB", "GB", "TB" };
    int unit = 0;
    double size = static_cast<double>(bytes);

    while (size >= 1024.0 && unit < 4)
    {
        size /= 1024.0;
        unit++;
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << size << " " << units[unit];
    return oss.str();
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkDebug::RenderGraph(
    const char* label,
    const std::vector<float>& data,
    const char* unit,
    ImVec4 color,
    float height
)
{
    ImGui::Text("%s", label);

    if (data.empty())
    {
        ImGui::TextColored(
            ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "No data available"
        );
        return;
    }

    // Calculate statistics
    float maxValue = *std::max_element(data.begin(), data.end());
    float avgValue =
        std::accumulate(data.begin(), data.end(), 0.0f) / data.size();
    float currentValue = data.back();

    // Display current value
    ImGui::Text(
        "Current: %.2f %s | Avg: %.2f %s | Max: %.2f %s",
        currentValue,
        unit,
        avgValue,
        unit,
        maxValue,
        unit
    );

    // Render graph
    ImVec2 graphSize(ImGui::GetContentRegionAvail().x, height);
    ImU32 graphColor = ImGui::GetColorU32(color);
    ImU32 avgLineColor = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 0.0f, 0.5f));

    if (ImGui::BeginChild(
            (std::string(label) + "_graph").c_str(), graphSize, true
        ))
    {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 canvasPos = ImGui::GetCursorScreenPos();
        ImVec2 canvasSize = ImGui::GetContentRegionAvail();

        if (canvasSize.x > 0 && canvasSize.y > 0)
        {
            // Draw average line
            if (maxValue > 0)
            {
                float avgY = canvasPos.y + canvasSize.y -
                             (avgValue / maxValue) * canvasSize.y;
                drawList->AddLine(
                    ImVec2(canvasPos.x, avgY),
                    ImVec2(canvasPos.x + canvasSize.x, avgY),
                    avgLineColor,
                    1.0f
                );
            }

            // Draw graph
            if (data.size() > 1)
            {
                for (size_t i = 1; i < data.size(); i++)
                {
                    float x1 = canvasPos.x +
                               (static_cast<float>(i - 1) / (data.size() - 1)
                               ) * canvasSize.x;
                    float x2 = canvasPos.x +
                               (static_cast<float>(i) / (data.size() - 1)) *
                                   canvasSize.x;

                    float y1 = canvasPos.y + canvasSize.y;
                    float y2 = canvasPos.y + canvasSize.y;

                    if (maxValue > 0)
                    {
                        y1 -= (data[i - 1] / maxValue) * canvasSize.y;
                        y2 -= (data[i] / maxValue) * canvasSize.y;
                    }

                    drawList->AddLine(
                        ImVec2(x1, y1), ImVec2(x2, y2), graphColor, 2.0f
                    );
                }
            }
        }
    }
    ImGui::EndChild();
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkDebug::RenderPacketFlags(UInt16 flags)
{
    if (flags == 0)
    {
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "None");
        return;
    }

    bool first = true;

    // Reliable flag
    if (flags & 0x0001)
    {
        if (!first) { ImGui::SameLine(); }
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Reliable");
        first = false;
    }

    // Fragmented flag
    if (flags & 0x0002)
    {
        if (!first) { ImGui::SameLine(); }
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Fragmented");
        first = false;
    }

    // Unknown flags
    UInt16 unknownFlags = flags & ~0x0003;
    if (unknownFlags)
    {
        if (!first) { ImGui::SameLine(); }
        ImGui::TextColored(
            ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Unknown(0x%04X)", unknownFlags
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkDebug::RenderHexDump(const Byte* data, SizeT size)
{
    constexpr SizeT bytesPerLine = 16;
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);   // Use monospace if
                                                       // available

    for (SizeT i = 0; i < size; i += bytesPerLine)
    {
        // Offset
        ImGui::Text("%04zX:  ", i);
        ImGui::SameLine();

        // Hex values
        for (SizeT j = 0; j < bytesPerLine; j++)
        {
            if (i + j < size)
            {
                ImGui::Text("%02X ", data[i + j]);
                ImGui::SameLine();
            }
            else
            {
                ImGui::Text("   ");
                ImGui::SameLine();
            }

            // Add extra space in the middle
            if (j == 7)
            {
                ImGui::Text(" ");
                ImGui::SameLine();
            }
        }

        ImGui::Text(" ");
        ImGui::SameLine();

        // ASCII representation
        for (SizeT j = 0; j < bytesPerLine && i + j < size; j++)
        {
            Byte c = data[i + j];
            ImGui::Text(
                "%c", (c >= 32 && c <= 126) ? static_cast<char>(c) : '.'
            );
            ImGui::SameLine();
        }

        ImGui::NewLine();
    }

    ImGui::PopFont();
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkDebug::PreviewPacket(
    const Byte* payload, SizeT payloadSize, UInt16 packetType
)
{
    if (!payload || payloadSize == 0)
    {
        ImGui::TextColored(
            ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "No payload data available"
        );
        return;
    }

    // Route to specific packet preview based on packet type
    if (packetType == Packets::Connect::GetStaticType())
    {
        PreviewConnectPacket(payload, payloadSize);
    }
    else if (packetType == Packets::ConnectResponse::GetStaticType())
    {
        PreviewConnectResponsePacket(payload, payloadSize);
    }
    else if (packetType == Packets::Disconnect::GetStaticType())
    {
        PreviewDisconnectPacket(payload, payloadSize);
    }
    else if (packetType == Packets::HeartBeat::GetStaticType())
    {
        PreviewHeartBeatPacket(payload, payloadSize);
    }
    else if (packetType == Packets::RemoteProcedureCall::GetStaticType())
    {
        PreviewRemoteProcedureCallPacket(payload, payloadSize);
    }
    else if (packetType == Packets::Acknowledgment::GetStaticType())
    {
        PreviewAcknowledgmentPacket(payload, payloadSize);
    }
    else if (packetType == Packets::Replication::GetStaticType())
    {
        PreviewReplicationPacket(payload, payloadSize);
    }
    else
    {
        ImGui::TextColored(
            ImVec4(1.0f, 0.5f, 0.0f, 1.0f),
            "Unable to preview packet type %u",
            packetType
        );
        ImGui::TextColored(
            ImVec4(0.7f, 0.7f, 0.7f, 1.0f),
            "(Preview not implemented for this packet type)"
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkDebug::PreviewConnectPacket(
    const Byte* payload, SizeT payloadSize
)
{
    if (payloadSize < sizeof(UInt32) + 32)
    {
        ImGui::TextColored(
            ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Invalid packet size"
        );
        return;
    }

    // Deserialize Connect packet
    FBinaryReader reader(payload, payloadSize);
    UInt32 clientID;
    std::array<char, 32> name;

    if (!reader.Read(clientID) ||
        !reader.ReadBytes(reinterpret_cast<UInt8*>(name.data()), name.size()))
    {
        ImGui::TextColored(
            ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Failed to deserialize packet"
        );
        return;
    }

    ImGui::Text("Connect Packet");
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Client ID:");
    ImGui::SameLine(150);
    if (clientID == 0)
    {
        ImGui::TextColored(
            ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "0 (Requesting new ID)"
        );
    }
    else
    {
        ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "%u", clientID);
    }

    ImGui::Text("Client Name:");
    ImGui::SameLine(150);
    // Ensure null-termination
    name[31] = '\0';
    ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "%s", name.data());

    ImGui::Spacing();
    ImGui::TextColored(
        ImVec4(0.7f, 0.7f, 0.7f, 1.0f),
        "Client is requesting connection to server"
    );
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkDebug::PreviewConnectResponsePacket(
    const Byte* payload, SizeT payloadSize
)
{
    if (payloadSize < sizeof(bool) + sizeof(UInt32))
    {
        ImGui::TextColored(
            ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Invalid packet size"
        );
        return;
    }

    // Deserialize ConnectResponse packet
    FBinaryReader reader(payload, payloadSize);
    bool accepted;
    UInt32 clientID;

    if (!reader.Read(accepted) || !reader.Read(clientID))
    {
        ImGui::TextColored(
            ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Failed to deserialize packet"
        );
        return;
    }

    ImGui::Text("Connect Response Packet");
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Status:");
    ImGui::SameLine(150);
    if (accepted)
    {
        ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "ACCEPTED");
    }
    else { ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "REJECTED"); }

    ImGui::Text("Assigned Client ID:");
    ImGui::SameLine(150);
    if (accepted)
    {
        ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "%u", clientID);
    }
    else { ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "N/A"); }

    ImGui::Spacing();
    if (accepted)
    {
        ImGui::TextColored(
            ImVec4(0.7f, 0.7f, 0.7f, 1.0f),
            "Server accepted the connection request"
        );
    }
    else
    {
        ImGui::TextColored(
            ImVec4(0.7f, 0.7f, 0.7f, 1.0f),
            "Server rejected the connection request"
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkDebug::PreviewDisconnectPacket(
    const Byte* payload, SizeT payloadSize
)
{
    if (payloadSize < sizeof(UInt32) * 2)
    {
        ImGui::TextColored(
            ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Invalid packet size"
        );
        return;
    }

    // Deserialize Disconnect packet
    FBinaryReader reader(payload, payloadSize);
    UInt32 clientID;
    UInt32 reason;

    if (!reader.Read(clientID) || !reader.Read(reason))
    {
        ImGui::TextColored(
            ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Failed to deserialize packet"
        );
        return;
    }

    ImGui::Text("Disconnect Packet");
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Client ID:");
    ImGui::SameLine(150);
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "%u", clientID);

    ImGui::Text("Reason Code:");
    ImGui::SameLine(150);
    ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "%u", reason);

    ImGui::Text("Reason:");
    ImGui::SameLine(150);
    // Map reason code to human-readable string
    const char* reasonStr = "Unknown";
    switch (reason)
    {
    case 0: reasonStr = "Unknown"; break;
    case 1: reasonStr = "Client Requested"; break;
    case 2: reasonStr = "Timeout"; break;
    case 3: reasonStr = "Kicked"; break;
    case 4: reasonStr = "Server Shutdown"; break;
    case 5: reasonStr = "Connection Lost"; break;
    }
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.4f, 1.0f), "%s", reasonStr);

    ImGui::Spacing();
    ImGui::TextColored(
        ImVec4(0.7f, 0.7f, 0.7f, 1.0f),
        "Client is being disconnected from server"
    );
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkDebug::PreviewHeartBeatPacket(
    const Byte* payload, SizeT payloadSize
)
{
    if (payloadSize < sizeof(UInt32) * 2)
    {
        ImGui::TextColored(
            ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Invalid packet size"
        );
        return;
    }

    // Deserialize HeartBeat packet
    FBinaryReader reader(payload, payloadSize);
    UInt32 id;
    UInt32 timestamp;

    if (!reader.Read(id) || !reader.Read(timestamp))
    {
        ImGui::TextColored(
            ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Failed to deserialize packet"
        );
        return;
    }

    ImGui::Text("HeartBeat Packet");
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Heartbeat ID:");
    ImGui::SameLine(150);
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "%u", id);

    ImGui::Text("Timestamp:");
    ImGui::SameLine(150);
    ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "%u ms", timestamp);

    // Calculate age
    UInt64 currentTime = GetCurrentTimestamp();
    if (currentTime >= timestamp)
    {
        UInt64 age = currentTime - timestamp;
        ImGui::Text("Age:");
        ImGui::SameLine(150);
        ImGui::TextColored(
            ImVec4(1.0f, 1.0f, 0.4f, 1.0f), "%.3f seconds", age / 1000.0f
        );
    }

    ImGui::Spacing();
    ImGui::TextColored(
        ImVec4(0.7f, 0.7f, 0.7f, 1.0f),
        "Keep-alive packet to maintain connection"
    );
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkDebug::PreviewRemoteProcedureCallPacket(
    const Byte* payload, SizeT payloadSize
)
{
    if (payloadSize < 16 + sizeof(UInt8))
    {
        ImGui::TextColored(
            ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Invalid packet size"
        );
        return;
    }

    // Deserialize RemoteProcedureCall packet
    FBinaryReader reader(payload, payloadSize);
    std::array<Byte, 16> actorID;
    UInt8 rpcType;
    FString functionName;
    std::vector<Byte> parameters;

    if (!reader.ReadBytes(actorID.data(), actorID.size()) ||
        !reader.Read(rpcType) || !reader.Read(functionName) ||
        !reader.Read(parameters))
    {
        ImGui::TextColored(
            ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Failed to deserialize packet"
        );
        return;
    }

    ImGui::Text("Remote Procedure Call Packet");
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Actor ID:");
    ImGui::SameLine(150);
    // Display actor ID as hex
    char actorIDStr[64];
    snprintf(
        actorIDStr,
        sizeof(actorIDStr),
        "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
        actorID[0],
        actorID[1],
        actorID[2],
        actorID[3],
        actorID[4],
        actorID[5],
        actorID[6],
        actorID[7],
        actorID[8],
        actorID[9],
        actorID[10],
        actorID[11],
        actorID[12],
        actorID[13],
        actorID[14],
        actorID[15]
    );
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "%s", actorIDStr);

    ImGui::Text("RPC Type:");
    ImGui::SameLine(150);
    const char* rpcTypeStr = "Unknown";
    ImVec4 rpcTypeColor = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
    switch (rpcType)
    {
    case 0:
        rpcTypeStr = "Server";
        rpcTypeColor = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
        break;
    case 1:
        rpcTypeStr = "Client";
        rpcTypeColor = ImVec4(0.4f, 0.4f, 1.0f, 1.0f);
        break;
    case 2:
        rpcTypeStr = "Multicast";
        rpcTypeColor = ImVec4(0.4f, 1.0f, 0.4f, 1.0f);
        break;
    }
    ImGui::TextColored(rpcTypeColor, "%s (%u)", rpcTypeStr, rpcType);

    ImGui::Text("Function Name:");
    ImGui::SameLine(150);
    ImGui::TextColored(
        ImVec4(1.0f, 1.0f, 0.4f, 1.0f), "%s", functionName.CStr()
    );

    ImGui::Text("Parameters Size:");
    ImGui::SameLine(150);
    ImGui::TextColored(
        ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "%zu bytes", parameters.size()
    );

    ImGui::Spacing();
    ImGui::TextColored(
        ImVec4(0.7f, 0.7f, 0.7f, 1.0f),
        "Invokes a remote function on the target actor"
    );
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkDebug::PreviewAcknowledgmentPacket(
    const Byte* payload, SizeT payloadSize
)
{
    if (payloadSize < sizeof(UInt32))
    {
        ImGui::TextColored(
            ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Invalid packet size"
        );
        return;
    }

    // Deserialize Acknowledgment packet
    FBinaryReader reader(payload, payloadSize);
    UInt32 ackedSequenceNumber;

    if (!reader.Read(ackedSequenceNumber))
    {
        ImGui::TextColored(
            ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Failed to deserialize packet"
        );
        return;
    }

    ImGui::Text("Acknowledgment Packet");
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Acknowledged Sequence:");
    ImGui::SameLine(200);
    ImGui::TextColored(
        ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "%u", ackedSequenceNumber
    );

    ImGui::Spacing();
    ImGui::TextColored(
        ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Confirms receipt of a reliable packet"
    );
}

///////////////////////////////////////////////////////////////////////////////
void FNetworkDebug::PreviewReplicationPacket(
    const Byte* payload, SizeT payloadSize
)
{
    if (payloadSize < 16 + sizeof(UInt32))
    {
        ImGui::TextColored(
            ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Invalid packet size"
        );
        return;
    }

    // Deserialize Replication packet
    FBinaryReader reader(payload, payloadSize);
    std::array<Byte, 16> actorID;
    UInt32 timestamp;
    FString propertyName;
    std::vector<Byte> data;

    if (!reader.ReadBytes(actorID.data(), actorID.size()) ||
        !reader.Read(timestamp) || !reader.Read(propertyName) ||
        !reader.Read(data))
    {
        ImGui::TextColored(
            ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Failed to deserialize packet"
        );
        return;
    }

    ImGui::Text("Replication Packet");
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Actor ID:");
    ImGui::SameLine(150);
    // Display actor ID as hex
    char actorIDStr[64];
    snprintf(
        actorIDStr,
        sizeof(actorIDStr),
        "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
        actorID[0],
        actorID[1],
        actorID[2],
        actorID[3],
        actorID[4],
        actorID[5],
        actorID[6],
        actorID[7],
        actorID[8],
        actorID[9],
        actorID[10],
        actorID[11],
        actorID[12],
        actorID[13],
        actorID[14],
        actorID[15]
    );
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "%s", actorIDStr);

    ImGui::Text("Timestamp:");
    ImGui::SameLine(150);
    ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "%u ms", timestamp);

    ImGui::Text("Property Name:");
    ImGui::SameLine(150);
    ImGui::TextColored(
        ImVec4(1.0f, 1.0f, 0.4f, 1.0f), "%s", propertyName.CStr()
    );

    ImGui::Text("Data Size:");
    ImGui::SameLine(150);
    ImGui::TextColored(
        ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "%zu bytes", data.size()
    );

    ImGui::Spacing();
    ImGui::TextColored(
        ImVec4(0.7f, 0.7f, 0.7f, 1.0f),
        "Replicates property changes across the network"
    );
}

#endif

}   // namespace tkd::debug
