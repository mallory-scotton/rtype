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

}   // namespace tkd
