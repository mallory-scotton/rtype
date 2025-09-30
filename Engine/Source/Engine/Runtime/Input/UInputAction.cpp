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
{}

///////////////////////////////////////////////////////////////////////////////
void UInputAction::Press(EInput input)
{
    if (std::find(m_inputs.begin(), m_inputs.end(), input) != m_inputs.end())
    {
        this->Emit(Events::Pressed{ input });
    }
}

///////////////////////////////////////////////////////////////////////////////
void UInputAction::Release(EInput input)
{
    if (std::find(m_inputs.begin(), m_inputs.end(), input) != m_inputs.end())
    {
        this->Emit(Events::Released{ input });
    }
}

///////////////////////////////////////////////////////////////////////////////
void UInputAction::Hold(EInput input)
{
    if (std::find(m_inputs.begin(), m_inputs.end(), input) != m_inputs.end())
    {
        this->Emit(Events::Held{ input });
    }
}

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

}   // namespace tkd
