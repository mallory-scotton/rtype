///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Runtime/Input/UInputAction.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
UInputAction::UInputAction(
    const FString& name, const std::vector<EInput>& inputs
)
    : m_name(name)
    , m_inputs(inputs)
    , m_pressed(false)
    , m_held(false)
    , m_released(false)
    , m_currentInput(EInput::Unknown)
{}

///////////////////////////////////////////////////////////////////////////////
void UInputAction::Press(EInput input)
{
    if (std::find(m_inputs.begin(), m_inputs.end(), input) != m_inputs.end())
    {
        m_currentInput = input;
        m_pressed = true;
        this->Emit(Events::Pressed{ input });
    }
}

///////////////////////////////////////////////////////////////////////////////
void UInputAction::Release(EInput input)
{
    if (std::find(m_inputs.begin(), m_inputs.end(), input) != m_inputs.end())
    {
        m_currentInput = input;
        m_pressed = false;
        m_held = false;
        m_released = true;
        this->Emit(Events::Released{ input });
    }
}

///////////////////////////////////////////////////////////////////////////////
void UInputAction::Hold(EInput input)
{
    if (std::find(m_inputs.begin(), m_inputs.end(), input) != m_inputs.end())
    {
        m_currentInput = input;
        m_held = true;
        this->Emit(Events::Held{ input });
    }
}

///////////////////////////////////////////////////////////////////////////////
void UInputAction::Idle(void)
{
    m_pressed = false;
    m_held = false;
    m_released = false;
}

///////////////////////////////////////////////////////////////////////////////
EInput UInputAction::GetCurrentInput(void) const { return m_currentInput; }

///////////////////////////////////////////////////////////////////////////////
const FString& UInputAction::GetName(void) const { return m_name; }

///////////////////////////////////////////////////////////////////////////////
const std::vector<EInput>& UInputAction::GetInputs(void) const
{
    return m_inputs;
}

///////////////////////////////////////////////////////////////////////////////
bool UInputAction::Bind(EInput input)
{
    if (std::find(m_inputs.begin(), m_inputs.end(), input) != m_inputs.end())
    {
        return false;   // Input already bound
    }
    m_inputs.push_back(input);
    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool UInputAction::Unbind(EInput input)
{
    auto it = std::find(m_inputs.begin(), m_inputs.end(), input);
    if (it == m_inputs.end())
    {
        return false;   // Input not found
    }
    m_inputs.erase(it);
    return true;
}

///////////////////////////////////////////////////////////////////////////////
void UInputAction::Reset(void)
{
    m_pressed = false;
    m_held = false;
    m_released = false;
}

///////////////////////////////////////////////////////////////////////////////
bool UInputAction::IsPressed(void) const { return m_pressed; }

///////////////////////////////////////////////////////////////////////////////
bool UInputAction::IsHeld(void) const { return m_held; }

///////////////////////////////////////////////////////////////////////////////
bool UInputAction::IsReleased(void) const { return m_released; }

}   // namespace tkd
