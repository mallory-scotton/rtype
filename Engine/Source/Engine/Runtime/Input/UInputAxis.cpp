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
    , m_scale(0.0f)
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
void UInputAxis::Move(EInput input, float factor)
{
    factor = Math<float>::Clamp(factor, -1.0f, 1.0f);

    for (const auto& [existingInput, scale]: m_inputs)
    {
        if (existingInput == input)
        {
            float oldScale = m_scale;
            m_scale = scale * factor;
            m_scale = Math<float>::Clamp(m_scale, -1.0f, 1.0f);
            float delta = m_scale - oldScale;
            this->Emit(Events::Changed{ m_scale, delta, input });
            return;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void UInputAxis::Reset(void)
{
    m_scale = 0.0f;
    this->Emit(Events::Reset{});
}

///////////////////////////////////////////////////////////////////////////////
float UInputAxis::GetScale(void) const { return m_scale; }

}   // namespace tkd
