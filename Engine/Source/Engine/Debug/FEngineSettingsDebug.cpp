///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Debug/FEngineSettingsDebug.hpp>
#include <Engine/Static/FEngineInterface.hpp>
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
void FEngineSettingsDebug::Show(const FEngineSettings& settings, UWorld* world)
{
    TKD_UNUSED(world);

    ImGui::Begin(
        "Engine Settings Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize
    );

    // Header with styling
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.8f, 1.0f, 1.0f));
    ImGui::Text("Engine Settings Configuration");
    ImGui::PopStyleColor();
    ImGui::Text("Engine Version: %s", settings.version.c_str());
    ImGui::Separator();
    ImGui::Spacing();

    // Create tab bar for different setting categories
    if (ImGui::BeginTabBar("EngineSettingsTabs", ImGuiTabBarFlags_None))
    {
        if (ImGui::BeginTabItem("Window"))
        {
            ShowWindowSettings(settings);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Network"))
        {
            ShowNetworkSettings(settings);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Logging"))
        {
            ShowLoggingSettings(settings);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Game"))
        {
            ShowGameSettings(settings);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Audio"))
        {
            ShowAudioSettings(settings);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Input"))
        {
            ShowInputSettings(settings);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Accessibility"))
        {
            ShowAccessibilitySettings(settings);
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}

///////////////////////////////////////////////////////////////////////////////
void FEngineSettingsDebug::ShowWindowSettings(const FEngineSettings& settings)
{
    ImGui::Spacing();

    const auto& window = settings.window;

    if (ImGui::BeginTable(
            "WindowSettingsTable",
            2,
            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                ImGuiTableFlags_Resizable
        ))
    {
        ImGui::TableSetupColumn(
            "Property", ImGuiTableColumnFlags_WidthFixed, 200.0f
        );
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        DisplayBoolSetting("V-Sync Enabled", window.enableVSync);
        DisplayIntSetting("Target FPS", window.targetFPS);
        DisplayIntSetting("Window Width", window.windowWidth);
        DisplayIntSetting("Window Height", window.windowHeight);
        DisplayBoolSetting("Fullscreen", window.isFullscreen);
        DisplayFloatSetting("Brightness", window.brightness);
        DisplayBoolSetting("Show Cursor", window.showCursor);
        DisplayBoolSetting("Resizable", window.resizable);
        DisplayBoolSetting("Borderless", window.borderless);

        ImGui::EndTable();
    }
}

///////////////////////////////////////////////////////////////////////////////
void FEngineSettingsDebug::ShowNetworkSettings(const FEngineSettings& settings)
{
    ImGui::Spacing();

    const auto& network = settings.network;

    if (ImGui::BeginTable(
            "NetworkSettingsTable",
            2,
            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                ImGuiTableFlags_Resizable
        ))
    {
        ImGui::TableSetupColumn(
            "Property", ImGuiTableColumnFlags_WidthFixed, 200.0f
        );
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        DisplayBoolSetting("Network Enabled", network.enabled);
        DisplayIntSetting("Max Clients", network.maxClients);
        DisplayIntSetting("Port", network.port);
        DisplayIntSetting("Timeout (ms)", network.timeout);
        DisplayIntSetting("Max Packet Size", network.maxPacketSize);
        DisplayIntSetting("Protocol Version", network.protocolVersion);

        ImGui::EndTable();
    }
}

///////////////////////////////////////////////////////////////////////////////
void FEngineSettingsDebug::ShowLoggingSettings(const FEngineSettings& settings)
{
    ImGui::Spacing();

    const auto& logging = settings.logging;

    if (ImGui::BeginTable(
            "LoggingSettingsTable",
            2,
            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                ImGuiTableFlags_Resizable
        ))
    {
        ImGui::TableSetupColumn(
            "Property", ImGuiTableColumnFlags_WidthFixed, 200.0f
        );
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        DisplayBoolSetting("Logging Enabled", logging.enabled);
        DisplayBoolSetting("Console Logging", logging.enableConsoleLogging);
        DisplayBoolSetting("File Logging", logging.enableFileLogging);
        DisplayIntSetting("Max Log File Size (MB)", logging.maxLogFileSizeMB);
        DisplayIntSetting("Log Retention (Days)", logging.logRetentionDays);

        ImGui::EndTable();
    }
}

///////////////////////////////////////////////////////////////////////////////
void FEngineSettingsDebug::ShowGameSettings(const FEngineSettings& settings)
{
    ImGui::Spacing();

    const auto& game = settings.game;

    if (ImGui::BeginTable(
            "GameSettingsTable",
            2,
            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                ImGuiTableFlags_Resizable
        ))
    {
        ImGui::TableSetupColumn(
            "Property", ImGuiTableColumnFlags_WidthFixed, 200.0f
        );
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        DisplayStringSetting("Title", game.title);
        DisplayStringSetting("Description", game.description);
        DisplayStringSetting("Version", game.version);
        DisplayStringSetting("Author", game.author);
        DisplayStringSetting("License", game.license);

        ImGui::EndTable();
    }
}

///////////////////////////////////////////////////////////////////////////////
void FEngineSettingsDebug::ShowAudioSettings(const FEngineSettings& settings)
{
    ImGui::Spacing();

    const auto& audio = settings.audio;

    if (ImGui::BeginTable(
            "AudioSettingsTable",
            2,
            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                ImGuiTableFlags_Resizable
        ))
    {
        ImGui::TableSetupColumn(
            "Property", ImGuiTableColumnFlags_WidthFixed, 200.0f
        );
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        DisplayBoolSetting("Audio Enabled", audio.enabled);
        DisplayIntSetting("Master Volume", audio.masterVolume);
        DisplayIntSetting("Music Volume", audio.musicVolume);
        DisplayIntSetting("SFX Volume", audio.sfxVolume);
        DisplayBoolSetting("3D Audio Enabled", audio.enable3DAudio);

        ImGui::EndTable();
    }
}

///////////////////////////////////////////////////////////////////////////////
void FEngineSettingsDebug::ShowInputSettings(const FEngineSettings& settings)
{
    ImGui::Spacing();

    const auto& inputs = settings.inputs;

    // Basic input settings
    if (ImGui::BeginTable(
            "InputSettingsTable",
            2,
            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                ImGuiTableFlags_Resizable
        ))
    {
        ImGui::TableSetupColumn(
            "Property", ImGuiTableColumnFlags_WidthFixed, 200.0f
        );
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        DisplayBoolSetting("Gamepad Enabled", inputs.enableGamepad);
        DisplayFloatSetting("Mouse Sensitivity", inputs.mouseSensitivity);
        DisplayBoolSetting("Invert Y-Axis", inputs.invertYAxis);

        ImGui::EndTable();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Show input mappings using the existing FInputDebug functionality
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.2f, 1.0f));
    ImGui::Text("Input Mappings:");
    ImGui::PopStyleColor();
    ImGui::Spacing();

    ShowActionMappings(settings);
    ImGui::Spacing();
    ShowAxisMappings(settings);
}

///////////////////////////////////////////////////////////////////////////////
void FEngineSettingsDebug::ShowAccessibilitySettings(
    const FEngineSettings& settings
)
{
    ImGui::Spacing();

    const auto& accessibility = settings.accessibility;

    if (ImGui::BeginTable(
            "AccessibilitySettingsTable",
            2,
            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                ImGuiTableFlags_Resizable
        ))
    {
        ImGui::TableSetupColumn(
            "Property", ImGuiTableColumnFlags_WidthFixed, 200.0f
        );
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        DisplayBoolSetting(
            "High Contrast Mode", accessibility.highContrastMode
        );
        DisplayIntSetting("Colorblind Mode", accessibility.colorblindMode);
        DisplayFloatSetting("UI Scale", accessibility.uiScale);
        DisplayBoolSetting("Subtitles", accessibility.subtitles);
        DisplayIntSetting("Subtitle Size", accessibility.subtitleSize);

        ImGui::EndTable();
    }
}

///////////////////////////////////////////////////////////////////////////////
void FEngineSettingsDebug::ShowActionMappings(const FEngineSettings& settings)
{
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f, 0.6f, 0.2f, 0.8f));
    ImGui::PushStyleColor(
        ImGuiCol_HeaderHovered, ImVec4(0.3f, 0.7f, 0.3f, 0.8f)
    );
    ImGui::PushStyleColor(
        ImGuiCol_HeaderActive, ImVec4(0.4f, 0.8f, 0.4f, 0.8f)
    );

    if (ImGui::CollapsingHeader(
            "Action Mappings", ImGuiTreeNodeFlags_DefaultOpen
        ))
    {
        ImGui::PopStyleColor(3);

        const auto& actions = settings.inputs.inputActions;
        if (actions.empty())
        {
            ImGui::PushStyleColor(
                ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f)
            );
            ImGui::Text("   No action mappings configured");
            ImGui::PopStyleColor();
        }
        else
        {
            if (ImGui::BeginTable(
                    "ActionMappingsTable",
                    2,
                    ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                        ImGuiTableFlags_Resizable
                ))
            {
                ImGui::TableSetupColumn(
                    "Action", ImGuiTableColumnFlags_WidthFixed, 120.0f
                );
                ImGui::TableSetupColumn(
                    "Inputs", ImGuiTableColumnFlags_WidthStretch
                );
                ImGui::TableHeadersRow();

                for (const auto& action: actions)
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);

                    // Action name with icon
                    ImGui::PushStyleColor(
                        ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.2f, 1.0f)
                    );
                    ImGui::Text("%s", action.first.c_str());
                    ImGui::PopStyleColor();

                    ImGui::TableSetColumnIndex(1);

                    // List inputs with proper formatting
                    bool first = true;
                    for (const auto& input: action.second)
                    {
                        if (!first) { ImGui::SameLine(); }

                        FString inputStr = EInputToString(input);
                        ImVec4 color = GetInputColor(input);
                        FString icon = GetInputIcon(input);

                        ImGui::PushStyleColor(
                            ImGuiCol_Button,
                            ImVec4(
                                color.x * 0.3f,
                                color.y * 0.3f,
                                color.z * 0.3f,
                                0.8f
                            )
                        );
                        ImGui::PushStyleColor(
                            ImGuiCol_ButtonHovered,
                            ImVec4(
                                color.x * 0.5f,
                                color.y * 0.5f,
                                color.z * 0.5f,
                                0.8f
                            )
                        );
                        ImGui::PushStyleColor(
                            ImGuiCol_ButtonActive,
                            ImVec4(
                                color.x * 0.7f,
                                color.y * 0.7f,
                                color.z * 0.7f,
                                0.8f
                            )
                        );
                        ImGui::PushStyleColor(ImGuiCol_Text, color);

                        ImGui::SmallButton((icon + " " + inputStr).CStr());

                        ImGui::PopStyleColor(4);
                        first = false;
                    }
                }
                ImGui::EndTable();
            }
        }
    }
    else { ImGui::PopStyleColor(3); }
}

///////////////////////////////////////////////////////////////////////////////
void FEngineSettingsDebug::ShowAxisMappings(const FEngineSettings& settings)
{
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.6f, 0.2f, 0.6f, 0.8f));
    ImGui::PushStyleColor(
        ImGuiCol_HeaderHovered, ImVec4(0.7f, 0.3f, 0.7f, 0.8f)
    );
    ImGui::PushStyleColor(
        ImGuiCol_HeaderActive, ImVec4(0.8f, 0.4f, 0.8f, 0.8f)
    );

    if (ImGui::CollapsingHeader(
            "Axis Mappings", ImGuiTreeNodeFlags_DefaultOpen
        ))
    {
        ImGui::PopStyleColor(3);

        const auto& axes = settings.inputs.inputAxes;
        if (axes.empty())
        {
            ImGui::PushStyleColor(
                ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f)
            );
            ImGui::Text("   No axis mappings configured");
            ImGui::PopStyleColor();
        }
        else
        {
            if (ImGui::BeginTable(
                    "AxisMappingsTable",
                    3,
                    ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                        ImGuiTableFlags_Resizable
                ))
            {
                ImGui::TableSetupColumn(
                    "Axis", ImGuiTableColumnFlags_WidthFixed, 120.0f
                );
                ImGui::TableSetupColumn(
                    "Input", ImGuiTableColumnFlags_WidthStretch
                );
                ImGui::TableSetupColumn(
                    "Scale", ImGuiTableColumnFlags_WidthFixed, 60.0f
                );
                ImGui::TableHeadersRow();

                for (const auto& axis: axes)
                {
                    bool firstInput = true;
                    for (const auto& [input, scale]: axis.second)
                    {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);

                        if (firstInput)
                        {
                            // Axis name with icon
                            ImGui::PushStyleColor(
                                ImGuiCol_Text, ImVec4(0.8f, 0.4f, 1.0f, 1.0f)
                            );
                            ImGui::Text("%s", axis.first.c_str());
                            ImGui::PopStyleColor();
                            firstInput = false;
                        }

                        ImGui::TableSetColumnIndex(1);

                        FString inputStr = EInputToString(input);
                        ImVec4 color = GetInputColor(input);
                        FString icon = GetInputIcon(input);

                        ImGui::PushStyleColor(
                            ImGuiCol_Button,
                            ImVec4(
                                color.x * 0.3f,
                                color.y * 0.3f,
                                color.z * 0.3f,
                                0.8f
                            )
                        );
                        ImGui::PushStyleColor(
                            ImGuiCol_ButtonHovered,
                            ImVec4(
                                color.x * 0.5f,
                                color.y * 0.5f,
                                color.z * 0.5f,
                                0.8f
                            )
                        );
                        ImGui::PushStyleColor(
                            ImGuiCol_ButtonActive,
                            ImVec4(
                                color.x * 0.7f,
                                color.y * 0.7f,
                                color.z * 0.7f,
                                0.8f
                            )
                        );
                        ImGui::PushStyleColor(ImGuiCol_Text, color);

                        ImGui::SmallButton((icon + " " + inputStr).CStr());

                        ImGui::PopStyleColor(4);

                        ImGui::TableSetColumnIndex(2);

                        // Scale value with color coding
                        ImVec4 scaleColor =
                            scale > 0 ? ImVec4(0.4f, 0.8f, 0.4f, 1.0f)
                                      : ImVec4(0.8f, 0.4f, 0.4f, 1.0f);
                        ImGui::PushStyleColor(ImGuiCol_Text, scaleColor);
                        ImGui::Text("%.2f", scale);
                        ImGui::PopStyleColor();
                    }
                }
                ImGui::EndTable();
            }
        }
    }
    else { ImGui::PopStyleColor(3); }
}

///////////////////////////////////////////////////////////////////////////////
void FEngineSettingsDebug::DisplayBoolSetting(const char* label, bool value)
{
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text("%s", label);

    ImGui::TableSetColumnIndex(1);
    ImVec4 color = value ? ImVec4(0.4f, 0.8f, 0.4f, 1.0f)
                         : ImVec4(0.8f, 0.4f, 0.4f, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_Text, color);
    ImGui::Text("%s", value ? "True" : "False");
    ImGui::PopStyleColor();
}

///////////////////////////////////////////////////////////////////////////////
void FEngineSettingsDebug::DisplayIntSetting(const char* label, int value)
{
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text("%s", label);

    ImGui::TableSetColumnIndex(1);
    ImVec4 color = (value == -1) ? ImVec4(0.8f, 0.8f, 0.4f, 1.0f)
                                 : ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_Text, color);
    if (value == -1) { ImGui::Text("(Unlimited)"); }
    else { ImGui::Text("%d", value); }
    ImGui::PopStyleColor();
}

///////////////////////////////////////////////////////////////////////////////
void FEngineSettingsDebug::DisplayFloatSetting(const char* label, float value)
{
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text("%s", label);

    ImGui::TableSetColumnIndex(1);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.8f, 1.0f));
    ImGui::Text("%.3f", value);
    ImGui::PopStyleColor();
}

///////////////////////////////////////////////////////////////////////////////
void FEngineSettingsDebug::DisplayStringSetting(
    const char* label, const std::string& value
)
{
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text("%s", label);

    ImGui::TableSetColumnIndex(1);
    if (value.empty())
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
        ImGui::Text("(empty)");
        ImGui::PopStyleColor();
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.8f, 1.0f));
        ImGui::Text("%s", value.c_str());
        ImGui::PopStyleColor();
    }
}

///////////////////////////////////////////////////////////////////////////////
ImVec4 FEngineSettingsDebug::GetInputColor(EInput input)
{
    // Color code different input types
    if (input >= EInput::Keyboard_A && input <= EInput::Keyboard_Pause)
    {
        return ImVec4(0.4f, 0.8f, 1.0f, 1.0f);   // Light blue for keyboard
    }
    else if (input >= EInput::Mouse_Left && input <= EInput::Mouse_XButton2)
    {
        return ImVec4(1.0f, 0.6f, 0.2f, 1.0f);   // Orange for mouse
    }
    else if (input >= EInput::GamePad_A &&
             input <= EInput::GamePadAxis_RightTrigger)
    {
        return ImVec4(0.6f, 1.0f, 0.4f, 1.0f);   // Green for gamepad
    }
    return ImVec4(0.8f, 0.8f, 0.8f, 1.0f);       // Default gray
}

///////////////////////////////////////////////////////////////////////////////
FString FEngineSettingsDebug::GetInputIcon(EInput input)
{
    // Add icons for different input types
    if (input >= EInput::Keyboard_A && input <= EInput::Keyboard_Pause)
    {
        return "KEY";
    }
    else if (input >= EInput::Mouse_Left && input <= EInput::Mouse_XButton2)
    {
        return "MOUSE";
    }
    else if (input >= EInput::GamePad_A &&
             input <= EInput::GamePadAxis_RightTrigger)
    {
        return "CTRL";
    }
    return "?";
}

///////////////////////////////////////////////////////////////////////////////
FString FEngineSettingsDebug::EInputToString(EInput input)
{
    switch (input)
    {
    case EInput::Keyboard_A              : return "A";
    case EInput::Keyboard_B              : return "B";
    case EInput::Keyboard_C              : return "C";
    case EInput::Keyboard_D              : return "D";
    case EInput::Keyboard_E              : return "E";
    case EInput::Keyboard_F              : return "F";
    case EInput::Keyboard_G              : return "G";
    case EInput::Keyboard_H              : return "H";
    case EInput::Keyboard_I              : return "I";
    case EInput::Keyboard_J              : return "J";
    case EInput::Keyboard_K              : return "K";
    case EInput::Keyboard_L              : return "L";
    case EInput::Keyboard_M              : return "M";
    case EInput::Keyboard_N              : return "N";
    case EInput::Keyboard_O              : return "O";
    case EInput::Keyboard_P              : return "P";
    case EInput::Keyboard_Q              : return "Q";
    case EInput::Keyboard_R              : return "R";
    case EInput::Keyboard_S              : return "S";
    case EInput::Keyboard_T              : return "T";
    case EInput::Keyboard_U              : return "U";
    case EInput::Keyboard_V              : return "V";
    case EInput::Keyboard_W              : return "W";
    case EInput::Keyboard_X              : return "X";
    case EInput::Keyboard_Y              : return "Y";
    case EInput::Keyboard_Z              : return "Z";
    case EInput::Keyboard_Num0           : return "0";
    case EInput::Keyboard_Num1           : return "1";
    case EInput::Keyboard_Num2           : return "2";
    case EInput::Keyboard_Num3           : return "3";
    case EInput::Keyboard_Num4           : return "4";
    case EInput::Keyboard_Num5           : return "5";
    case EInput::Keyboard_Num6           : return "6";
    case EInput::Keyboard_Num7           : return "7";
    case EInput::Keyboard_Num8           : return "8";
    case EInput::Keyboard_Num9           : return "9";
    case EInput::Keyboard_Escape         : return "Escape";
    case EInput::Keyboard_LControl       : return "Left Ctrl";
    case EInput::Keyboard_LShift         : return "Left Shift";
    case EInput::Keyboard_LAlt           : return "Left Alt";
    case EInput::Keyboard_LSystem        : return "Left Win";
    case EInput::Keyboard_RControl       : return "Right Ctrl";
    case EInput::Keyboard_RShift         : return "Right Shift";
    case EInput::Keyboard_RAlt           : return "Right Alt";
    case EInput::Keyboard_RSystem        : return "Right Win";
    case EInput::Keyboard_Menu           : return "Menu";
    case EInput::Keyboard_LBracket       : return "[";
    case EInput::Keyboard_RBracket       : return "]";
    case EInput::Keyboard_Semicolon      : return ";";
    case EInput::Keyboard_Comma          : return ",";
    case EInput::Keyboard_Period         : return ".";
    case EInput::Keyboard_Quote          : return "'";
    case EInput::Keyboard_Slash          : return "/";
    case EInput::Keyboard_Backslash      : return "\\";
    case EInput::Keyboard_Tilde          : return "~";
    case EInput::Keyboard_Equal          : return "=";
    case EInput::Keyboard_Hyphen         : return "-";
    case EInput::Keyboard_Space          : return "Space";
    case EInput::Keyboard_Enter          : return "Enter";
    case EInput::Keyboard_Backspace      : return "Backspace";
    case EInput::Keyboard_Tab            : return "Tab";
    case EInput::Keyboard_PageUp         : return "PageUp";
    case EInput::Keyboard_PageDown       : return "PageDown";
    case EInput::Keyboard_End            : return "End";
    case EInput::Keyboard_Home           : return "Home";
    case EInput::Keyboard_Insert         : return "Insert";
    case EInput::Keyboard_Delete         : return "Delete";
    case EInput::Keyboard_Add            : return "Add";
    case EInput::Keyboard_Subtract       : return "Subtract";
    case EInput::Keyboard_Multiply       : return "Multiply";
    case EInput::Keyboard_Divide         : return "Divide";
    case EInput::Keyboard_Left           : return "Left";
    case EInput::Keyboard_Right          : return "Right";
    case EInput::Keyboard_Up             : return "Up";
    case EInput::Keyboard_Down           : return "Down";
    case EInput::Keyboard_Numpad0        : return "Numpad0";
    case EInput::Keyboard_Numpad1        : return "Numpad1";
    case EInput::Keyboard_Numpad2        : return "Numpad2";
    case EInput::Keyboard_Numpad3        : return "Numpad3";
    case EInput::Keyboard_Numpad4        : return "Numpad4";
    case EInput::Keyboard_Numpad5        : return "Numpad5";
    case EInput::Keyboard_Numpad6        : return "Numpad6";
    case EInput::Keyboard_Numpad7        : return "Numpad7";
    case EInput::Keyboard_Numpad8        : return "Numpad8";
    case EInput::Keyboard_Numpad9        : return "Numpad9";
    case EInput::Keyboard_F1             : return "F1";
    case EInput::Keyboard_F2             : return "F2";
    case EInput::Keyboard_F3             : return "F3";
    case EInput::Keyboard_F4             : return "F4";
    case EInput::Keyboard_F5             : return "F5";
    case EInput::Keyboard_F6             : return "F6";
    case EInput::Keyboard_F7             : return "F7";
    case EInput::Keyboard_F8             : return "F8";
    case EInput::Keyboard_F9             : return "F9";
    case EInput::Keyboard_F10            : return "F10";
    case EInput::Keyboard_F11            : return "F11";
    case EInput::Keyboard_F12            : return "F12";
    case EInput::Keyboard_F13            : return "F13";
    case EInput::Keyboard_F14            : return "F14";
    case EInput::Keyboard_F15            : return "F15";
    case EInput::Keyboard_Pause          : return "Pause";
    case EInput::Mouse_Left              : return "Left";
    case EInput::Mouse_Right             : return "Right";
    case EInput::Mouse_Middle            : return "Middle";
    case EInput::Mouse_XButton1          : return "X-Button 1";
    case EInput::Mouse_XButton2          : return "X-Button 2";
    case EInput::GamePad_A               : return "A";
    case EInput::GamePad_B               : return "B";
    case EInput::GamePad_X               : return "X";
    case EInput::GamePad_Y               : return "Y";
    case EInput::GamePad_Back            : return "Back";
    case EInput::GamePad_Guide           : return "Guide";
    case EInput::GamePad_Start           : return "Start";
    case EInput::GamePad_LeftStick       : return "Left Stick";
    case EInput::GamePad_RightStick      : return "Right Stick";
    case EInput::GamePad_LeftShoulder    : return "Left Shoulder";
    case EInput::GamePad_RightShoulder   : return "Right Shoulder";
    case EInput::GamePad_DPadUp          : return "D-Pad Up";
    case EInput::GamePad_DPadDown        : return "D-Pad Down";
    case EInput::GamePad_DPadLeft        : return "D-Pad Left";
    case EInput::GamePad_DPadRight       : return "D-Pad Right";
    case EInput::GamePadAxis_LeftX       : return "Axis Left X";
    case EInput::GamePadAxis_LeftY       : return "Axis Left Y";
    case EInput::GamePadAxis_RightX      : return "Axis Right X";
    case EInput::GamePadAxis_RightY      : return "Axis Right Y";
    case EInput::GamePadAxis_LeftTrigger : return "Axis Left Trigger";
    case EInput::GamePadAxis_RightTrigger: return "Axis Right Trigger";
    default                              : return "Unknown";
    }
}

#endif

}   // namespace tkd::debug
