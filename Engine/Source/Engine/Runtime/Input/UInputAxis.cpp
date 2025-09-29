///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Runtime/Input/UInputAxis.hpp>
#include <Engine/Core/Math/Utils.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
UInputAxis::UInputAxis(
    const FString& name, const std::vector<UInputAxis::Input>& inputs
)
    : m_name(name)
    , m_inputs(inputs)
{
    // Clamp the scales to -1.0f to 1.0f
    for (auto& [input, scale]: m_inputs)
    {
        scale = Math<float>::Clamp(scale, -1.0f, 1.0f);
    }
}

///////////////////////////////////////////////////////////////////////////////
const FString& UInputAxis::GetName(void) const { return m_name; }

///////////////////////////////////////////////////////////////////////////////
const std::vector<UInputAxis::Input>& UInputAxis::GetInputs(void) const
{
    return m_inputs;
}

///////////////////////////////////////////////////////////////////////////////
bool UInputAxis::Bind(EInput input, float scale)
{
    for (const auto& [existingInput, existingScale]: m_inputs)
    {
        if (existingInput == input) { return false; }
    }
    scale = Math<float>::Clamp(scale, -1.0f, 1.0f);
    m_inputs.emplace_back(input, scale);
    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool UInputAxis::SetScale(EInput input, float scale)
{
    for (auto& [existingInput, existingScale]: m_inputs)
    {
        if (existingInput == input)
        {
            existingScale = Math<float>::Clamp(scale, -1.0f, 1.0f);
            return true;
        }
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
bool UInputAxis::Unbind(EInput input)
{
    auto it = std::remove_if(
        m_inputs.begin(),
        m_inputs.end(),
        [input](const Input& pair) { return pair.first == input; }
    );
    if (it != m_inputs.end())
    {
        m_inputs.erase(it, m_inputs.end());
        return true;
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
void UInputAxis::ClearBindings(void) { m_inputs.clear(); }

///////////////////////////////////////////////////////////////////////////////
void UInputAxis::Move(float value, float delta)
{
    value = Math<float>::Clamp(value, -1.0f, 1.0f);
    delta = Math<float>::Clamp(delta, -1.0f, 1.0f);
    EmitEvent(Events::Changed{ value, delta });
}

}   // namespace tkd
