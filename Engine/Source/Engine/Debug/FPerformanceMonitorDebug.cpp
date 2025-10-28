///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Debug/FPerformanceMonitorDebug.hpp>
#if TKD_ENGINE_CLIENT
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
void FPerformanceMonitorDebug::Show(
    const FEngineSettings& settings, UWorld* world
)
{
    TKD_UNUSED(settings);
    TKD_UNUSED(world);

    ImGui::Begin("Performance Monitor");

    // Update performance data
    UpdatePerformanceData();

    // Show main performance overview
    ShowPerformanceOverview();

    // FPS and timing graph
    if (ImGui::CollapsingHeader(
            "Frame Rate & Timing", ImGuiTreeNodeFlags_DefaultOpen
        ))
    {
        ShowFPSGraph();
        ShowFrameTimeGraph();
        ShowTimingBreakdown();
    }

    // Memory usage
    if (ImGui::CollapsingHeader("Memory Usage")) { ShowMemoryGraph(); }

    // CPU/GPU metrics
    if (ImGui::CollapsingHeader("Hardware Metrics"))
    {
        ShowCPUGraph();
        ShowGPUGraph();
    }

    // Configuration
    if (ImGui::CollapsingHeader("Settings")) { ShowMonitorSettings(); }

    ImGui::End();
}

///////////////////////////////////////////////////////////////////////////////
void FPerformanceMonitorDebug::UpdatePerformanceData()
{
    static double lastTime = ImGui::GetTime();
    double currentTime = ImGui::GetTime();
    double deltaTime = currentTime - lastTime;

    // Update FPS data
    float currentFPS =
        (deltaTime > 0.0) ? static_cast<float>(1.0 / deltaTime) : 0.0f;
    m_fpsHistory.push_back(currentFPS);

    // Update frame time data (in milliseconds)
    float currentFrameTime = static_cast<float>(deltaTime * 1000.0);
    m_frameTimeHistory.push_back(currentFrameTime);

    // Keep history within limits
    if (m_fpsHistory.size() > m_historySize)
    {
        m_fpsHistory.erase(m_fpsHistory.begin());
    }
    if (m_frameTimeHistory.size() > m_historySize)
    {
        m_frameTimeHistory.erase(m_frameTimeHistory.begin());
    }

    // Update min/max/average
    UpdateStatistics();

    // Update memory usage (placeholder - implement with your engine's memory
    // tracking)
    UpdateMemoryUsage();

    // Update CPU/GPU usage (placeholder - implement with your platform's APIs)
    UpdateHardwareMetrics();

    lastTime = currentTime;
}

///////////////////////////////////////////////////////////////////////////////
void FPerformanceMonitorDebug::ShowPerformanceOverview()
{
    ImGui::Text("Performance Overview");
    ImGui::Separator();

    ImGui::Columns(4, "overview_columns", false);

    // FPS display with color coding
    ImVec4 fpsColor = (m_currentFPS >= 60.0f) ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f)
                      : (m_currentFPS >= 30.0f)
                          ? ImVec4(1.0f, 1.0f, 0.0f, 1.0f)
                          : ImVec4(1.0f, 0.0f, 0.0f, 1.0f);

    ImGui::TextColored(fpsColor, "FPS: %.1f", m_currentFPS);
    ImGui::NextColumn();

    // Frame time
    ImGui::Text("Frame: %.2f ms", m_currentFrameTime);
    ImGui::NextColumn();

    // Memory usage
    ImGui::Text("Memory: %.1f MB", m_currentMemoryUsage);
    ImGui::NextColumn();

    // CPU usage
    ImGui::Text("CPU: %.1f%%", m_currentCPUUsage);
    ImGui::NextColumn();

    ImGui::Columns(1);

    ImGui::Spacing();
}

///////////////////////////////////////////////////////////////////////////////
void FPerformanceMonitorDebug::ShowFPSGraph()
{
    ImGui::Text("Frame Rate (FPS)");

    // Stats display
    ImGui::Columns(3, "fps_stats", false);
    ImGui::Text("Current: %.1f", m_currentFPS);
    ImGui::NextColumn();
    ImGui::Text("Average: %.1f", m_avgFPS);
    ImGui::NextColumn();
    ImGui::Text("Min: %.1f", m_minFPS);
    ImGui::NextColumn();
    ImGui::Columns(1);

    // Graph dimensions
    float graphHeight = 80.0f;
    ImVec2 graphSize(ImGui::GetContentRegionAvail().x, graphHeight);

    // Create FPS graph
    if (m_fpsHistory.size() > 1)
    {
        // Calculate graph scale
        float maxFPS =
            *std::max_element(m_fpsHistory.begin(), m_fpsHistory.end());
        float minFPS =
            *std::min_element(m_fpsHistory.begin(), m_fpsHistory.end());
        float range =
            std::max(maxFPS - minFPS, 10.0f);   // Ensure minimum range

        ImU32 graphColor = ImGui::GetColorU32(ImVec4(0.0f, 0.8f, 0.0f, 1.0f));
        ImU32 avgLineColor =
            ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
        ImU32 refLineColor =
            ImGui::GetColorU32(ImVec4(1.0f, 0.0f, 0.0f, 0.5f));

        if (ImGui::BeginChild("FPSGraph", graphSize, true))
        {
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
            ImVec2 canvas_size = ImGui::GetContentRegionAvail();

            // Draw reference lines (30, 60 FPS)
            float y30 = canvas_pos.y + canvas_size.y -
                        ((30.0f - minFPS) / range) * canvas_size.y;
            float y60 = canvas_pos.y + canvas_size.y -
                        ((60.0f - minFPS) / range) * canvas_size.y;

            draw_list->AddLine(
                ImVec2(canvas_pos.x, y30),
                ImVec2(canvas_pos.x + canvas_size.x, y30),
                refLineColor
            );
            draw_list->AddLine(
                ImVec2(canvas_pos.x, y60),
                ImVec2(canvas_pos.x + canvas_size.x, y60),
                refLineColor
            );

            // Draw FPS graph
            for (size_t i = 1; i < m_fpsHistory.size(); i++)
            {
                float x1 =
                    canvas_pos.x +
                    (float(i - 1) / (m_fpsHistory.size() - 1)) * canvas_size.x;
                float x2 = canvas_pos.x + (float(i) / (m_fpsHistory.size() - 1)
                                          ) * canvas_size.x;

                float y1 =
                    canvas_pos.y + canvas_size.y -
                    ((m_fpsHistory[i - 1] - minFPS) / range) * canvas_size.y;
                float y2 =
                    canvas_pos.y + canvas_size.y -
                    ((m_fpsHistory[i] - minFPS) / range) * canvas_size.y;

                draw_list->AddLine(
                    ImVec2(x1, y1), ImVec2(x2, y2), graphColor, 2.0f
                );
            }

            // Draw average line
            float avgY = canvas_pos.y + canvas_size.y -
                         ((m_avgFPS - minFPS) / range) * canvas_size.y;
            draw_list->AddLine(
                ImVec2(canvas_pos.x, avgY),
                ImVec2(canvas_pos.x + canvas_size.x, avgY),
                avgLineColor
            );
        }
        ImGui::EndChild();
    }

    ImGui::Spacing();
}

///////////////////////////////////////////////////////////////////////////////
void FPerformanceMonitorDebug::ShowFrameTimeGraph()
{
    ImGui::Text("Frame Time (ms)");

    // Stats display
    ImGui::Columns(3, "frame_time_stats", false);
    ImGui::Text("Current: %.2f ms", m_currentFrameTime);
    ImGui::NextColumn();
    ImGui::Text("Average: %.2f ms", m_avgFrameTime);
    ImGui::NextColumn();
    ImGui::Text("Max: %.2f ms", m_maxFrameTime);
    ImGui::NextColumn();
    ImGui::Columns(1);

    float graphHeight = 80.0f;
    ImVec2 graphSize(ImGui::GetContentRegionAvail().x, graphHeight);

    if (m_frameTimeHistory.size() > 1)
    {
        float maxTime = *std::max_element(
            m_frameTimeHistory.begin(), m_frameTimeHistory.end()
        );
        float minTime = *std::min_element(
            m_frameTimeHistory.begin(), m_frameTimeHistory.end()
        );
        float range =
            std::max(maxTime - minTime, 5.0f);   // Ensure minimum range

        ImU32 graphColor = ImGui::GetColorU32(ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
        ImU32 avgLineColor =
            ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
        ImU32 refLineColor =
            ImGui::GetColorU32(ImVec4(0.0f, 1.0f, 0.0f, 0.5f));

        if (ImGui::BeginChild("FrameTimeGraph", graphSize, true))
        {
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
            ImVec2 canvas_size = ImGui::GetContentRegionAvail();

            // Draw reference line (16.67ms for 60FPS)
            float y16 = canvas_pos.y + canvas_size.y -
                        ((16.67f - minTime) / range) * canvas_size.y;
            draw_list->AddLine(
                ImVec2(canvas_pos.x, y16),
                ImVec2(canvas_pos.x + canvas_size.x, y16),
                refLineColor
            );

            // Draw frame time graph
            for (size_t i = 1; i < m_frameTimeHistory.size(); i++)
            {
                float x1 = canvas_pos.x +
                           (float(i - 1) / (m_frameTimeHistory.size() - 1)) *
                               canvas_size.x;
                float x2 =
                    canvas_pos.x + (float(i) / (m_frameTimeHistory.size() - 1)
                                   ) * canvas_size.x;

                float y1 = canvas_pos.y + canvas_size.y -
                           ((m_frameTimeHistory[i - 1] - minTime) / range) *
                               canvas_size.y;
                float y2 = canvas_pos.y + canvas_size.y -
                           ((m_frameTimeHistory[i] - minTime) / range) *
                               canvas_size.y;

                draw_list->AddLine(
                    ImVec2(x1, y1), ImVec2(x2, y2), graphColor, 2.0f
                );
            }

            // Draw average line
            float avgY = canvas_pos.y + canvas_size.y -
                         ((m_avgFrameTime - minTime) / range) * canvas_size.y;
            draw_list->AddLine(
                ImVec2(canvas_pos.x, avgY),
                ImVec2(canvas_pos.x + canvas_size.x, avgY),
                avgLineColor
            );
        }
        ImGui::EndChild();
    }

    ImGui::Spacing();
}

///////////////////////////////////////////////////////////////////////////////
void FPerformanceMonitorDebug::ShowTimingBreakdown()
{
    ImGui::Text("Timing Breakdown");

    // Placeholder for engine-specific timing categories
    // Replace with your engine's actual timing data
    struct TimingCategory
    {
        const char* name;
        float time;
        ImVec4 color;
    };

    std::vector<TimingCategory> categories = {
        { "Rendering", 8.2f, ImVec4(0.2f, 0.6f, 1.0f, 1.0f) },
        {   "Physics", 3.1f, ImVec4(0.9f, 0.3f, 0.3f, 1.0f) },
        {        "AI", 1.5f, ImVec4(0.4f, 0.8f, 0.2f, 1.0f) },
        {     "Audio", 0.8f, ImVec4(0.8f, 0.4f, 1.0f, 1.0f) },
        {     "Other", 2.1f, ImVec4(0.7f, 0.7f, 0.7f, 1.0f) }
    };

    float totalTime = 0.0f;
    for (const auto& category: categories) { totalTime += category.time; }

    ImVec2 barSize(ImGui::GetContentRegionAvail().x, 20.0f);
    float cursorX = ImGui::GetCursorPosX();

    for (const auto& category: categories)
    {
        float width = (category.time / totalTime) * barSize.x;

        ImGui::GetWindowDrawList()->AddRectFilled(
            ImVec2(
                ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y
            ),
            ImVec2(
                ImGui::GetCursorScreenPos().x + width,
                ImGui::GetCursorScreenPos().y + barSize.y
            ),
            ImGui::GetColorU32(category.color)
        );

        // Add text label if there's enough space
        if (width > 50.0f)
        {
            ImGui::GetWindowDrawList()->AddText(
                ImVec2(
                    ImGui::GetCursorScreenPos().x + 4.0f,
                    ImGui::GetCursorScreenPos().y + 4.0f
                ),
                IM_COL32_WHITE,
                category.name
            );
        }

        ImGui::SetCursorPosX(cursorX + width);
    }

    ImGui::Dummy(barSize);
    ImGui::Spacing();
}

///////////////////////////////////////////////////////////////////////////////
void FPerformanceMonitorDebug::ShowMemoryGraph()
{
    ImGui::Text("Memory Usage (MB)");

    float graphHeight = 60.0f;
    ImVec2 graphSize(ImGui::GetContentRegionAvail().x, graphHeight);

    // Placeholder - implement with your engine's memory tracking
    static std::vector<float> memoryHistory;
    static float currentMemory = 512.0f;

    // Simulate memory usage changes
    currentMemory += (rand() % 10 - 5) * 0.1f;
    memoryHistory.push_back(currentMemory);
    if (memoryHistory.size() > m_historySize)
    {
        memoryHistory.erase(memoryHistory.begin());
    }

    if (memoryHistory.size() > 1)
    {
        float maxMem =
            *std::max_element(memoryHistory.begin(), memoryHistory.end());
        float minMem =
            *std::min_element(memoryHistory.begin(), memoryHistory.end());
        float range = std::max(maxMem - minMem, 10.0f);

        ImU32 graphColor = ImGui::GetColorU32(ImVec4(0.2f, 0.6f, 1.0f, 1.0f));

        if (ImGui::BeginChild("MemoryGraph", graphSize, true))
        {
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
            ImVec2 canvas_size = ImGui::GetContentRegionAvail();

            for (size_t i = 1; i < memoryHistory.size(); i++)
            {
                float x1 =
                    canvas_pos.x + (float(i - 1) / (memoryHistory.size() - 1)
                                   ) * canvas_size.x;
                float x2 =
                    canvas_pos.x +
                    (float(i) / (memoryHistory.size() - 1)) * canvas_size.x;

                float y1 =
                    canvas_pos.y + canvas_size.y -
                    ((memoryHistory[i - 1] - minMem) / range) * canvas_size.y;
                float y2 =
                    canvas_pos.y + canvas_size.y -
                    ((memoryHistory[i] - minMem) / range) * canvas_size.y;

                draw_list->AddLine(
                    ImVec2(x1, y1), ImVec2(x2, y2), graphColor, 2.0f
                );
            }
        }
        ImGui::EndChild();
    }

    ImGui::Text("Current: %.1f MB", currentMemory);
    ImGui::Spacing();
}

///////////////////////////////////////////////////////////////////////////////
void FPerformanceMonitorDebug::ShowCPUGraph()
{
    ImGui::Text("CPU Usage (%%)");

    // Placeholder - implement with platform-specific CPU monitoring
    static std::vector<float> cpuHistory;
    static float currentCPU = 45.0f;

    currentCPU += (rand() % 10 - 5) * 0.5f;
    currentCPU = std::clamp(currentCPU, 0.0f, 100.0f);
    cpuHistory.push_back(currentCPU);
    if (cpuHistory.size() > m_historySize)
    {
        cpuHistory.erase(cpuHistory.begin());
    }

    float graphHeight = 60.0f;
    ImVec2 graphSize(ImGui::GetContentRegionAvail().x, graphHeight);

    if (cpuHistory.size() > 1)
    {
        ImU32 graphColor = ImGui::GetColorU32(ImVec4(1.0f, 0.5f, 0.0f, 1.0f));

        if (ImGui::BeginChild("CPUGraph", graphSize, true))
        {
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
            ImVec2 canvas_size = ImGui::GetContentRegionAvail();

            for (size_t i = 1; i < cpuHistory.size(); i++)
            {
                float x1 =
                    canvas_pos.x +
                    (float(i - 1) / (cpuHistory.size() - 1)) * canvas_size.x;
                float x2 = canvas_pos.x + (float(i) / (cpuHistory.size() - 1)
                                          ) * canvas_size.x;

                float y1 = canvas_pos.y + canvas_size.y -
                           (cpuHistory[i - 1] / 100.0f) * canvas_size.y;
                float y2 = canvas_pos.y + canvas_size.y -
                           (cpuHistory[i] / 100.0f) * canvas_size.y;

                draw_list->AddLine(
                    ImVec2(x1, y1), ImVec2(x2, y2), graphColor, 2.0f
                );
            }
        }
        ImGui::EndChild();
    }

    ImGui::Text("Current: %.1f%%", currentCPU);
    ImGui::Spacing();
}

///////////////////////////////////////////////////////////////////////////////
void FPerformanceMonitorDebug::ShowGPUGraph()
{
    ImGui::Text("GPU Usage (%%)");

    // Placeholder - implement with platform-specific GPU monitoring
    static std::vector<float> gpuHistory;
    static float currentGPU = 65.0f;

    currentGPU += (rand() % 10 - 5) * 0.5f;
    currentGPU = std::clamp(currentGPU, 0.0f, 100.0f);
    gpuHistory.push_back(currentGPU);
    if (gpuHistory.size() > m_historySize)
    {
        gpuHistory.erase(gpuHistory.begin());
    }

    float graphHeight = 60.0f;
    ImVec2 graphSize(ImGui::GetContentRegionAvail().x, graphHeight);

    if (gpuHistory.size() > 1)
    {
        ImU32 graphColor = ImGui::GetColorU32(ImVec4(0.8f, 0.2f, 0.8f, 1.0f));

        if (ImGui::BeginChild("GPUGraph", graphSize, true))
        {
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
            ImVec2 canvas_size = ImGui::GetContentRegionAvail();

            for (size_t i = 1; i < gpuHistory.size(); i++)
            {
                float x1 =
                    canvas_pos.x +
                    (float(i - 1) / (gpuHistory.size() - 1)) * canvas_size.x;
                float x2 = canvas_pos.x + (float(i) / (gpuHistory.size() - 1)
                                          ) * canvas_size.x;

                float y1 = canvas_pos.y + canvas_size.y -
                           (gpuHistory[i - 1] / 100.0f) * canvas_size.y;
                float y2 = canvas_pos.y + canvas_size.y -
                           (gpuHistory[i] / 100.0f) * canvas_size.y;

                draw_list->AddLine(
                    ImVec2(x1, y1), ImVec2(x2, y2), graphColor, 2.0f
                );
            }
        }
        ImGui::EndChild();
    }

    ImGui::Text("Current: %.1f%%", currentGPU);
    ImGui::Spacing();
}

///////////////////////////////////////////////////////////////////////////////
void FPerformanceMonitorDebug::ShowMonitorSettings()
{
    int newHistorySize = static_cast<int>(m_historySize);
    ImGui::SliderInt("History Size", &newHistorySize, 60, 1000);
    ImGui::Checkbox("Show Average Lines", &m_showAverageLines);
    ImGui::Checkbox("Show Reference Lines", &m_showReferenceLines);

    if (newHistorySize != static_cast<int>(m_historySize))
    {
        m_historySize = static_cast<size_t>(newHistorySize);
        if (m_fpsHistory.size() > m_historySize)
        {
            m_fpsHistory.erase(
                m_fpsHistory.begin(),
                m_fpsHistory.begin() + (m_fpsHistory.size() - m_historySize)
            );
        }
        if (m_frameTimeHistory.size() > m_historySize)
        {
            m_frameTimeHistory.erase(
                m_frameTimeHistory.begin(),
                m_frameTimeHistory.begin() +
                    (m_frameTimeHistory.size() - m_historySize)
            );
        }
    }

    if (ImGui::Button("Reset Statistics")) { ResetStatistics(); }
}

///////////////////////////////////////////////////////////////////////////////
void FPerformanceMonitorDebug::UpdateStatistics()
{
    if (m_fpsHistory.empty()) { return; }

    m_currentFPS = m_fpsHistory.back();
    m_currentFrameTime = m_frameTimeHistory.back();

    // Calculate averages
    m_avgFPS = 0.0f;
    m_avgFrameTime = 0.0f;

    for (float fps: m_fpsHistory) { m_avgFPS += fps; }
    for (float time: m_frameTimeHistory) { m_avgFrameTime += time; }

    m_avgFPS /= m_fpsHistory.size();
    m_avgFrameTime /= m_frameTimeHistory.size();

    // Calculate min/max
    m_minFPS = *std::min_element(m_fpsHistory.begin(), m_fpsHistory.end());
    m_maxFrameTime = *std::max_element(
        m_frameTimeHistory.begin(), m_frameTimeHistory.end()
    );
}

///////////////////////////////////////////////////////////////////////////////
void FPerformanceMonitorDebug::UpdateMemoryUsage()
{
    // Placeholder - implement with your engine's memory tracking system
    static float baseMemory = 500.0f;
    m_currentMemoryUsage = baseMemory + (rand() % 100) * 0.1f;
}

///////////////////////////////////////////////////////////////////////////////
void FPerformanceMonitorDebug::UpdateHardwareMetrics()
{
    // Placeholder - implement with platform-specific APIs
    m_currentCPUUsage = 40.0f + (rand() % 30);
    m_currentGPUUsage = 50.0f + (rand() % 40);
}

///////////////////////////////////////////////////////////////////////////////
void FPerformanceMonitorDebug::ResetStatistics()
{
    m_fpsHistory.clear();
    m_frameTimeHistory.clear();
    m_minFPS = 0.0f;
    m_avgFPS = 0.0f;
    m_maxFrameTime = 0.0f;
    m_avgFrameTime = 0.0f;
}

#endif

}   // namespace tkd::debug
