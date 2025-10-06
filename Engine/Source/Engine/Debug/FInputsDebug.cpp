///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Debug/FInputsDebug.hpp>
#include <cmath>
#include <cstdio>
#include <Engine/Runtime/Input/FInputManager.hpp>
#include <Engine/Static/FEngineInterface.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::debug
///////////////////////////////////////////////////////////////////////////////
namespace tkd::debug
{

#if TKD_ENGINE_CLIENT

///////////////////////////////////////////////////////////////////////////////
void FInputsDebug::Show(const FEngineSettings& settings, UWorld* world)
{
    TKD_UNUSED(settings);
    TKD_UNUSED(world);

    FInputManager* inputManager =
        Engine::GetInstance().GetWindow()->GetInputManager();
    const auto& actions = inputManager->GetActions();
    const auto& axis = inputManager->GetAxes();

    ImGui::Begin("Inputs Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    // Header with styling
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.8f, 1.0f, 1.0f));
    ImGui::Text("Input System Monitor");
    ImGui::PopStyleColor();
    ImGui::Text(
        "Total Actions: %zu | Total Axes: %zu", actions.size(), axis.size()
    );
    ImGui::Separator();
    ImGui::Spacing();

    ShowActions(actions);
    ImGui::Spacing();
    ImGui::Spacing();
    ShowAxes(axis);

    ImGui::End();
}

///////////////////////////////////////////////////////////////////////////////
void FInputsDebug::ShowActions(const std::vector<UInputAction>& actions)
{
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f, 0.6f, 0.2f, 0.8f));
    ImGui::PushStyleColor(
        ImGuiCol_HeaderHovered, ImVec4(0.3f, 0.7f, 0.3f, 0.8f)
    );
    ImGui::PushStyleColor(
        ImGuiCol_HeaderActive, ImVec4(0.4f, 0.8f, 0.4f, 0.8f)
    );

    if (ImGui::CollapsingHeader("Actions", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::PopStyleColor(3);

        if (actions.empty())
        {
            ImGui::PushStyleColor(
                ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f)
            );
            ImGui::Text("   No actions configured");
            ImGui::PopStyleColor();
        }
        else
        {
            if (ImGui::BeginTable(
                    "ActionsTable",
                    4,
                    ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                        ImGuiTableFlags_Resizable
                ))
            {
                ImGui::TableSetupColumn(
                    "Action", ImGuiTableColumnFlags_WidthFixed, 150.0f
                );
                ImGui::TableSetupColumn(
                    "State", ImGuiTableColumnFlags_WidthFixed, 100.0f
                );
                ImGui::TableSetupColumn(
                    "Current Input", ImGuiTableColumnFlags_WidthFixed, 130.0f
                );
                ImGui::TableSetupColumn(
                    "Bound Inputs", ImGuiTableColumnFlags_WidthStretch
                );
                ImGui::TableHeadersRow();

                for (const auto& action: actions)
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);

                    // Action name
                    ImGui::PushStyleColor(
                        ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.2f, 1.0f)
                    );
                    ImGui::Text("%s", action.GetName().CStr());
                    ImGui::PopStyleColor();

                    ImGui::TableSetColumnIndex(1);

                    // State with colored badge
                    DisplayActionState(action);

                    ImGui::TableSetColumnIndex(2);

                    // Current input
                    DisplayCurrentInput(action);

                    ImGui::TableSetColumnIndex(3);

                    // Bound inputs
                    DisplayBoundInputs(action.GetInputs());
                }
                ImGui::EndTable();
            }
        }
    }
    else { ImGui::PopStyleColor(3); }
}

///////////////////////////////////////////////////////////////////////////////
void FInputsDebug::ShowAxes(const std::vector<UInputAxis>& axes)
{
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.6f, 0.2f, 0.6f, 0.8f));
    ImGui::PushStyleColor(
        ImGuiCol_HeaderHovered, ImVec4(0.7f, 0.3f, 0.7f, 0.8f)
    );
    ImGui::PushStyleColor(
        ImGuiCol_HeaderActive, ImVec4(0.8f, 0.4f, 0.8f, 0.8f)
    );

    if (ImGui::CollapsingHeader("Axes", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::PopStyleColor(3);

        if (axes.empty())
        {
            ImGui::PushStyleColor(
                ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f)
            );
            ImGui::Text("   No axes configured");
            ImGui::PopStyleColor();
        }
        else
        {
            if (ImGui::BeginTable(
                    "AxesTable",
                    4,
                    ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                        ImGuiTableFlags_Resizable
                ))
            {
                ImGui::TableSetupColumn(
                    "Axis", ImGuiTableColumnFlags_WidthFixed, 150.0f
                );
                ImGui::TableSetupColumn(
                    "Value", ImGuiTableColumnFlags_WidthFixed, 80.0f
                );
                ImGui::TableSetupColumn(
                    "Visual", ImGuiTableColumnFlags_WidthFixed, 150.0f
                );
                ImGui::TableSetupColumn(
                    "Bound Inputs", ImGuiTableColumnFlags_WidthStretch
                );
                ImGui::TableHeadersRow();

                for (const auto& ax: axes)
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);

                    // Axis name
                    ImGui::PushStyleColor(
                        ImGuiCol_Text, ImVec4(0.8f, 0.4f, 1.0f, 1.0f)
                    );
                    ImGui::Text("%s", ax.GetName().CStr());
                    ImGui::PopStyleColor();

                    ImGui::TableSetColumnIndex(1);

                    // Numeric value
                    DisplayAxisValue(ax.GetScale());

                    ImGui::TableSetColumnIndex(2);

                    // Visual progress bar
                    DisplayAxisBar(ax.GetScale());

                    ImGui::TableSetColumnIndex(3);

                    // Bound inputs with scales
                    DisplayBoundAxisInputs(ax.GetInputs());
                }
                ImGui::EndTable();
            }
        }
    }
    else { ImGui::PopStyleColor(3); }
}

///////////////////////////////////////////////////////////////////////////////
void FInputsDebug::DisplayActionState(const UInputAction& action)
{
    ImVec4 color;
    const char* text;

    if (action.IsPressed())
    {
        color = ImVec4(0.2f, 1.0f, 0.2f, 1.0f);
        text = "PRESSED";
    }
    else if (action.IsHeld())
    {
        color = ImVec4(1.0f, 1.0f, 0.2f, 1.0f);
        text = "HELD";
    }
    else if (action.IsReleased())
    {
        color = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);
        text = "RELEASED";
    }
    else
    {
        color = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
        text = "IDLE";
    }

    ImGui::PushStyleColor(ImGuiCol_Text, color);
    ImGui::Text("%s", text);
    ImGui::PopStyleColor();
}

///////////////////////////////////////////////////////////////////////////////
void FInputsDebug::DisplayCurrentInput(const UInputAction& action)
{
    EInput currentInput = action.GetCurrentInput();

    // Check if action is in idle state (no active input)
    if (!action.IsPressed() && !action.IsHeld() && !action.IsReleased())
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        ImGui::Text("None");
        ImGui::PopStyleColor();
    }
    else
    {
        FString inputStr = EInputToString(currentInput);
        ImVec4 color = GetInputColor(currentInput);
        FString icon = GetInputIcon(currentInput);

        ImGui::PushStyleColor(
            ImGuiCol_Button,
            ImVec4(color.x * 0.3f, color.y * 0.3f, color.z * 0.3f, 0.8f)
        );
        ImGui::PushStyleColor(
            ImGuiCol_ButtonHovered,
            ImVec4(color.x * 0.5f, color.y * 0.5f, color.z * 0.5f, 0.8f)
        );
        ImGui::PushStyleColor(
            ImGuiCol_ButtonActive,
            ImVec4(color.x * 0.7f, color.y * 0.7f, color.z * 0.7f, 0.8f)
        );
        ImGui::PushStyleColor(ImGuiCol_Text, color);

        ImGui::SmallButton((icon + " " + inputStr).CStr());

        ImGui::PopStyleColor(4);
    }
}

///////////////////////////////////////////////////////////////////////////////
void FInputsDebug::DisplayBoundInputs(const std::vector<EInput>& inputs)
{
    if (inputs.empty())
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
        ImGui::Text("No bindings");
        ImGui::PopStyleColor();
        return;
    }

    bool first = true;
    for (const auto& input: inputs)
    {
        if (!first) { ImGui::SameLine(); }

        FString inputStr = EInputToString(input);
        ImVec4 color = GetInputColor(input);
        FString icon = GetInputIcon(input);

        ImGui::PushStyleColor(
            ImGuiCol_Button,
            ImVec4(color.x * 0.3f, color.y * 0.3f, color.z * 0.3f, 0.8f)
        );
        ImGui::PushStyleColor(
            ImGuiCol_ButtonHovered,
            ImVec4(color.x * 0.5f, color.y * 0.5f, color.z * 0.5f, 0.8f)
        );
        ImGui::PushStyleColor(
            ImGuiCol_ButtonActive,
            ImVec4(color.x * 0.7f, color.y * 0.7f, color.z * 0.7f, 0.8f)
        );
        ImGui::PushStyleColor(ImGuiCol_Text, color);

        ImGui::SmallButton((icon + " " + inputStr).CStr());

        ImGui::PopStyleColor(4);
        first = false;
    }
}

///////////////////////////////////////////////////////////////////////////////
void FInputsDebug::DisplayBoundAxisInputs(
    const std::vector<std::pair<EInput, float>>& inputs
)
{
    if (inputs.empty())
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
        ImGui::Text("No bindings");
        ImGui::PopStyleColor();
        return;
    }

    bool first = true;
    for (const auto& [input, scale]: inputs)
    {
        if (!first) { ImGui::SameLine(); }

        FString inputStr = EInputToString(input);
        ImVec4 color = GetInputColor(input);
        FString icon = GetInputIcon(input);

        // Format scale value using standard C++ approach
        char scaleBuffer[32];
        snprintf(scaleBuffer, sizeof(scaleBuffer), "%.2f", scale);
        FString scaleStr = scaleBuffer;

        // Add scale indicator to button text
        FString buttonText = icon + " " + inputStr + " (" +
                             (scale >= 0 ? "+" : "") + scaleStr + ")";

        ImGui::PushStyleColor(
            ImGuiCol_Button,
            ImVec4(color.x * 0.3f, color.y * 0.3f, color.z * 0.3f, 0.8f)
        );
        ImGui::PushStyleColor(
            ImGuiCol_ButtonHovered,
            ImVec4(color.x * 0.5f, color.y * 0.5f, color.z * 0.5f, 0.8f)
        );
        ImGui::PushStyleColor(
            ImGuiCol_ButtonActive,
            ImVec4(color.x * 0.7f, color.y * 0.7f, color.z * 0.7f, 0.8f)
        );
        ImGui::PushStyleColor(ImGuiCol_Text, color);

        ImGui::SmallButton(buttonText.CStr());

        ImGui::PopStyleColor(4);
        first = false;
    }
}

///////////////////////////////////////////////////////////////////////////////
void FInputsDebug::DisplayAxisValue(float value)
{
    ImVec4 color;
    if (std::abs(value) < 0.01f) { color = ImVec4(0.6f, 0.6f, 0.6f, 1.0f); }
    else if (value > 0) { color = ImVec4(0.4f, 0.8f, 0.4f, 1.0f); }
    else { color = ImVec4(0.8f, 0.4f, 0.4f, 1.0f); }

    ImGui::PushStyleColor(ImGuiCol_Text, color);
    ImGui::Text("%.3f", value);
    ImGui::PopStyleColor();
}

///////////////////////////////////////////////////////////////////////////////
void FInputsDebug::DisplayAxisBar(float value)
{
    // Normalize value to [0, 1] range for display
    // Assuming typical axis values are in [-1, 1]
    float normalizedValue = (value + 1.0f) * 0.5f;

    // Clamp to ensure it's within valid range
    normalizedValue = std::max(0.0f, std::min(1.0f, normalizedValue));

    // Choose color based on value
    ImVec4 barColor;
    if (std::abs(value) < 0.01f) { barColor = ImVec4(0.3f, 0.3f, 0.3f, 1.0f); }
    else if (value > 0) { barColor = ImVec4(0.2f, 0.7f, 0.2f, 1.0f); }
    else { barColor = ImVec4(0.7f, 0.2f, 0.2f, 1.0f); }

    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, barColor);
    ImGui::ProgressBar(normalizedValue, ImVec2(-1, 0), "");
    ImGui::PopStyleColor();
}

///////////////////////////////////////////////////////////////////////////////
ImVec4 FInputsDebug::GetInputColor(EInput input)
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
FString FInputsDebug::GetInputIcon(EInput input)
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
FString FInputsDebug::EInputToString(EInput input)
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

