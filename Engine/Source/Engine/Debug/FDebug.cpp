///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Debug/FDebug.hpp>
#include <Engine/Config.hpp>
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
void FDebug::ApplyDebugMenuStyling(void)
{
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, 6.0f));
    ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.1f, 0.1f, 0.1f, 0.95f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
}

///////////////////////////////////////////////////////////////////////////////
void FDebug::PopDebugMenuStyling(void)
{
    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar();
}

///////////////////////////////////////////////////////////////////////////////
void FDebug::Show(const FEngineSettings& settings, UWorld* world)
{
    ApplyDebugMenuStyling();

    if (ImGui::BeginMainMenuBar())
    {
        // Engine Debug Menu
        if (ImGui::BeginMenu("Debug"))
        {
            ImGui::PushStyleColor(
                ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f)
            );

            // Engine Settings
            if (ImGui::MenuItem(
                    "Engine Settings", "Ctrl+Shift+E", m_showEngineSettings
                ))
            {
                m_showEngineSettings = !m_showEngineSettings;
            }

            ImGui::Separator();

            // Performance section
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Performance");
            ImGui::Indent(10.0f);

            if (ImGui::MenuItem(
                    "Performance Monitor",
                    "Ctrl+Shift+P",
                    m_showPerformanceMonitor
                ))
            {
                m_showPerformanceMonitor = !m_showPerformanceMonitor;
            }

            if (ImGui::MenuItem(
                    "Memory Profiler",
                    "Ctrl+Shift+M",
                    m_showMemoryProfiler,
                    false
                ))
            {
                m_showMemoryProfiler = !m_showMemoryProfiler;
            }

            ImGui::Unindent(10.0f);
            ImGui::Separator();

            // Rendering section
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Rendering");
            ImGui::Indent(10.0f);

            if (ImGui::MenuItem(
                    "Render Debug", "Ctrl+Shift+R", m_showRenderDebug, false
                ))
            {
                m_showRenderDebug = !m_showRenderDebug;
            }

            if (ImGui::MenuItem(
                    "Camera Debug", "Ctrl+Shift+C", m_showCameraDebug, false
                ))
            {
                m_showCameraDebug = !m_showCameraDebug;
            }

            ImGui::Unindent(10.0f);
            ImGui::Separator();

            // Network section
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Network");
            ImGui::Indent(10.0f);

            if (ImGui::MenuItem(
                    "Network Monitor",
                    "Ctrl+Shift+N",
                    m_showNetworkMonitor,
                    false
                ))
            {
                m_showNetworkMonitor = !m_showNetworkMonitor;
            }

            if (ImGui::MenuItem(
                    "Packet Inspector",
                    "Ctrl+Shift+I",
                    m_showPacketInspector,
                    false
                ))
            {
                m_showPacketInspector = !m_showPacketInspector;
            }

            ImGui::Unindent(10.0f);
            ImGui::Separator();

            // Debug section
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Debug");
            ImGui::Indent(10.0f);

            if (ImGui::MenuItem(
                    "Input Debugger", "Ctrl+Shift+B", m_showInputDebug
                ))
            {
                m_showInputDebug = !m_showInputDebug;
            }

            if (ImGui::MenuItem(
                    "Actor Debugger", "Ctrl+Shift+A", m_showActorDebug
                ))
            {
                m_showActorDebug = !m_showActorDebug;
            }

            ImGui::Unindent(10.0f);
            ImGui::PopStyleColor();
            ImGui::EndMenu();
        }

        // Quick access toolbar
        ImGui::SameLine(0.0f, 20.0f);

        // Engine Settings quick button
        bool settingsActive = m_showEngineSettings;
        if (settingsActive)
        {
            ImGui::PushStyleColor(
                ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.8f, 0.8f)
            );
            ImGui::PushStyleColor(
                ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.5f, 0.9f, 0.9f)
            );
            ImGui::PushStyleColor(
                ImGuiCol_ButtonActive, ImVec4(0.1f, 0.3f, 0.7f, 0.9f)
            );
        }
        else
        {
            ImGui::PushStyleColor(
                ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 0.6f)
            );
            ImGui::PushStyleColor(
                ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.8f)
            );
            ImGui::PushStyleColor(
                ImGuiCol_ButtonActive, ImVec4(0.4f, 0.4f, 0.4f, 0.9f)
            );
        }

        if (ImGui::Button("SETTINGS"))
        {
            m_showEngineSettings = !m_showEngineSettings;
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Engine Settings (Ctrl+Shift+E)");
        }
        ImGui::PopStyleColor(3);

        // Add a visual separator
        ImGui::SameLine();
        ImGui::Text("|");
        ImGui::SameLine();

        // FPS counter in the toolbar
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.2f, 1.0f));
        ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
        ImGui::PopStyleColor();

        // Spacer to push info to the right
        float windowWidth = ImGui::GetWindowWidth();
        float infoWidth =
            std::string(TKD_VERSION_STRING).length() * 8.0f + 180.0f;
        ImGui::SetCursorPosX(windowWidth - infoWidth);

        // Engine info on the right side
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.8f, 0.6f, 1.0f));
        ImGui::Text("TKD Engine (%s) | Debug Mode", TKD_VERSION_STRING);
        ImGui::PopStyleColor();

        ImGui::EndMainMenuBar();
    }

    PopDebugMenuStyling();

    // Handle keyboard input for toggling debug windows
    bool KeyCtrl = ImGui::GetIO().KeyCtrl;
    bool KeyShift = ImGui::GetIO().KeyShift;

    // Handle keyboard shortcuts
    if (ImGui::IsKeyPressed(ImGuiKey_E) && KeyCtrl && KeyShift)
    {
        m_showEngineSettings = !m_showEngineSettings;
    }
    else if (ImGui::IsKeyPressed(ImGuiKey_B) && KeyCtrl && KeyShift)
    {
        m_showInputDebug = !m_showInputDebug;
    }
    else if (ImGui::IsKeyPressed(ImGuiKey_A) && KeyCtrl && KeyShift)
    {
        m_showActorDebug = !m_showActorDebug;
    }
    else if (ImGui::IsKeyPressed(ImGuiKey_P) && KeyCtrl && KeyShift)
    {
        m_showPerformanceMonitor = !m_showPerformanceMonitor;
    }
    else if (ImGui::IsKeyPressed(ImGuiKey_M) && KeyCtrl && KeyShift)
    {
        m_showMemoryProfiler = !m_showMemoryProfiler;
    }
    else if (ImGui::IsKeyPressed(ImGuiKey_R) && KeyCtrl && KeyShift)
    {
        m_showRenderDebug = !m_showRenderDebug;
    }
    else if (ImGui::IsKeyPressed(ImGuiKey_C) && KeyCtrl && KeyShift)
    {
        m_showCameraDebug = !m_showCameraDebug;
    }
    else if (ImGui::IsKeyPressed(ImGuiKey_N) && KeyCtrl && KeyShift)
    {
        m_showNetworkMonitor = !m_showNetworkMonitor;
    }
    else if (ImGui::IsKeyPressed(ImGuiKey_I) && KeyCtrl && KeyShift)
    {
        m_showPacketInspector = !m_showPacketInspector;
    }

    // Show debug windows
    if (m_showEngineSettings) { m_engineSettings.Show(settings, world); }
    if (m_showInputDebug) { m_inputsDebug.Show(settings, world); }
    if (m_showActorDebug) { m_actorDebug.Show(settings, world); }
    if (m_showPerformanceMonitor)
    {
        m_performanceMonitor.Show(settings, world);
    }
}

#endif

}   // namespace tkd::debug
