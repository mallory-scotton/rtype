///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Runtime/Input/FInputManager.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
void FInputManager::Initialize(const FEngineSettings& settings)
{
    m_invertY = settings.inputs.invertYAxis;
    m_enableGamepad = settings.inputs.enableGamepad;

    for (const auto& [actionName, inputs]: settings.inputs.inputActions)
    {
        m_actions.emplace_back(actionName, inputs);
    }

    for (const auto& [axisName, inputAndScale]: settings.inputs.inputAxes)
    {
        m_axes.emplace_back(axisName, inputAndScale);
    }

    m_initialized = true;
}

///////////////////////////////////////////////////////////////////////////////
bool FInputManager::IsInitialized(void) const { return m_initialized; }

///////////////////////////////////////////////////////////////////////////////
bool FInputManager::IsInvertY(void) const { return m_invertY; }

///////////////////////////////////////////////////////////////////////////////
bool FInputManager::IsGamepadEnabled(void) const { return m_enableGamepad; }

///////////////////////////////////////////////////////////////////////////////
const std::vector<UInputAction>& FInputManager::GetActions(void) const
{
    return m_actions;
}

///////////////////////////////////////////////////////////////////////////////
const UInputAction* FInputManager::GetAction(const FString& actionName) const
{
    for (const auto& action: m_actions)
    {
        if (action.GetName() == actionName) { return &action; }
    }
    return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
const std::vector<UInputAxis>& FInputManager::GetAxes(void) const
{
    return m_axes;
}

///////////////////////////////////////////////////////////////////////////////
const UInputAxis* FInputManager::GetAxis(const FString& axisName) const
{
    for (const auto& axis: m_axes)
    {
        if (axis.GetName() == axisName) { return &axis; }
    }
    return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
void FInputManager::SetInvertY(bool invert) { m_invertY = invert; }

///////////////////////////////////////////////////////////////////////////////
void FInputManager::SetGamepadEnabled(bool enable)
{
    m_enableGamepad = enable;
}

///////////////////////////////////////////////////////////////////////////////
void FInputManager::Update(IWindow* window)
{
    // Process input states and emit events as necessary
    for (auto& action: m_actions)
    {
        bool isPressed = false;
        EInput pressedInput = EInput::Unknown;

        const auto& inputs = action.GetInputs();
        for (const auto& input: inputs)
        {
            if (input >= EInput::Keyboard_A && input <= EInput::Keyboard_Pause)
            {
                if (window->IsKeyPressed(static_cast<EKeyboardKeys>(input)))
                {
                    isPressed = true;
                    pressedInput = input;
                    break;
                }
            }
            else if (input >= EInput::Mouse_Left &&
                     input <= EInput::Mouse_XButton2)
            {
                if (window->IsMouseButtonPressed(
                        static_cast<EMouseButtons>(input)
                    ))
                {
                    isPressed = true;
                    pressedInput = input;
                    break;
                }
            }
            else if (input >= EInput::GamePad_A &&
                     input <= EInput::GamePad_DPadRight)
            {
                if (m_enableGamepad && window->IsGamepadButtonPressed(
                                           static_cast<EGamepadButtons>(input)
                                       ))
                {
                    isPressed = true;
                    pressedInput = input;
                    break;
                }
            }
            else if (input >= EInput::GamePadAxis_LeftX &&
                     input <= EInput::GamePadAxis_RightTrigger)
            {
                if (m_enableGamepad &&
                    window->GetGamepadAxis(static_cast<EGamepadAxes>(input)) >=
                        0.10f)
                {
                    isPressed = true;
                    pressedInput = input;
                    break;
                }
            }
        }

        if (isPressed)
        {
            action.Press(pressedInput);
            if (action.IsPressed()) { action.Hold(pressedInput); }
        }
        else
        {
            if (action.IsPressed() || action.IsHeld())
            {
                action.Release(action.GetCurrentInput());
            }
            else if (action.IsReleased()) { action.Idle(); }
        }
    }

    // Process axes and emit events as necessary
    for (auto& axis: m_axes)
    {
        // Sum the contributions of all inputs to get the final axis value
        float combinedValue = 0.0f;
        float factor = 1.0f;
        EInput activeInput = EInput::Unknown;

        const auto& inputs = axis.GetInputs();
        for (const auto& [input, scale]: inputs)
        {
            if (input >= EInput::Keyboard_A && input <= EInput::Keyboard_Pause)
            {
                if (window->IsKeyPressed(static_cast<EKeyboardKeys>(input)))
                {
                    combinedValue += scale;
                    if ((scale < 0.0f && combinedValue < 0.0f) ||
                        (scale > 0.0f && combinedValue > 0.0f))
                    {
                        activeInput = input;
                    }
                }
            }
            else if (input >= EInput::Mouse_Left &&
                     input <= EInput::Mouse_XButton2)
            {
                if (window->IsMouseButtonPressed(
                        static_cast<EMouseButtons>(input)
                    ))
                {
                    combinedValue += scale;
                    if ((scale < 0.0f && combinedValue < 0.0f) ||
                        (scale > 0.0f && combinedValue > 0.0f))
                    {
                        activeInput = input;
                    }
                }
            }
            else if (input >= EInput::GamePad_A &&
                     input <= EInput::GamePad_DPadRight)
            {
                if (m_enableGamepad && window->IsGamepadButtonPressed(
                                           static_cast<EGamepadButtons>(input)
                                       ))
                {
                    combinedValue += scale;
                    if ((scale < 0.0f && combinedValue < 0.0f) ||
                        (scale > 0.0f && combinedValue > 0.0f))
                    {
                        activeInput = input;
                    }
                }
            }
            else if (input >= EInput::GamePadAxis_LeftX &&
                     input <= EInput::GamePadAxis_RightTrigger)
            {
                if (m_enableGamepad)
                {
                    float axisValue =
                        window->GetGamepadAxis(static_cast<EGamepadAxes>(input)
                        );
                    if (std::abs(axisValue) >= 0.10f)
                    {
                        combinedValue += scale;
                        if ((scale < 0.0f && combinedValue < 0.0f) ||
                            (scale > 0.0f && combinedValue > 0.0f))
                        {
                            activeInput = input;
                        }
                        factor = std::abs(axisValue);
                    }
                }
            }
        }

        // Clamp combined value to [-1.0, 1.0]
        combinedValue = std::clamp(combinedValue, -1.0f, 1.0f);

        // Apply dead zone
        static const float deadZone = 0.05f;
        if (std::abs(combinedValue) < deadZone) { combinedValue = 0.0f; }

        // Update axis state
        if (combinedValue != 0.0f) { axis.Move(activeInput, factor); }
        else if (axis.GetScale() != 0.0f) { axis.Reset(); }
    }
}

}   // namespace tkd
