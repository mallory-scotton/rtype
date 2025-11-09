// UWidgetTextboxComponent.cpp - COMPLETE IMPLEMENTATION:

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Runtime/Components/Widgets/UWidgetTextboxComponent.hpp>
#include <Engine/Runtime/Actor/AActor.hpp>
#include <Engine/Runtime/Components/Widgets/QWERTY_AZERTY_map.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
UWidgetTextboxComponent::UWidgetTextboxComponent(const FString& name)
    : UWidgetComponent(name)
    , m_backgroundColor(FColor(0.3, 0.3, 0.3, 1.0))          // Dark gray
    , m_focusedBackgroundColor(FColor(0.5, 0.5, 0.5, 1.0))   // Lighter gray
    , m_textColor(FColor::White)
    , m_rectangleShape()
    , m_textShape()
    , m_text("")
    , m_fontPath()
    , m_charSize(16.0f, 16.0f)
    , m_spacing(-8.0f)
    , m_maxCharacters(0)
    , m_minBoxChars(5)
    , m_maxBoxChars(20)
    , m_adaptSizeToText(true)
{}

///////////////////////////////////////////////////////////////////////////////
void UWidgetTextboxComponent::SetBackgroundColor(const FColor& color)
{
    m_backgroundColor = color;
    if (!Super::IsFocused())
    {
        m_rectangleShape.SetFillColor(m_backgroundColor);
    }
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetTextboxComponent::SetFocusedBackgroundColor(const FColor& color)
{
    m_focusedBackgroundColor = color;
    if (Super::IsFocused())
    {
        m_rectangleShape.SetFillColor(m_focusedBackgroundColor);
    }
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetTextboxComponent::SetTextColor(const FColor& color)
{
    m_textColor = color;
    m_textShape.SetColor(m_textColor);
}

///////////////////////////////////////////////////////////////////////////////
const FString& UWidgetTextboxComponent::GetText(void) const { return m_text; }

///////////////////////////////////////////////////////////////////////////////
void UWidgetTextboxComponent::SetText(const FString& text)
{
    m_text = text;
    m_textShape.SetString(m_text);
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetTextboxComponent::SetFontPath(const FilePath& path)
{
    m_fontPath = path;
    m_textShape.SetFont(m_fontPath);
}

///////////////////////////////////////////////////////////////////////////////
const FilePath& UWidgetTextboxComponent::GetFontPath(void) const
{
    return m_fontPath;
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetTextboxComponent::SetCharacterSize(const FVector2& size)
{
    m_charSize = size;
    m_textShape.SetCharacterSize(m_charSize);
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetTextboxComponent::SetSpacing(Float32 spacing)
{
    m_spacing = spacing;
    m_textShape.SetSpacing(m_spacing);
}

///////////////////////////////////////////////////////////////////////////////
bool UWidgetTextboxComponent::ContainsPoint(const FVector2& point) const
{
    FVector2 origin = GetOrigin();
    FVector2 topLeft = GetPosition() - origin;
    FVector2 bottomRight = topLeft + GetSize();

    return point.x >= topLeft.x && point.x <= bottomRight.x &&
           point.y >= topLeft.y && point.y <= bottomRight.y;
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetTextboxComponent::EraseCtrlBackspace(void)
{
    if (m_text.IsEmpty()) { return; }
    while (!m_text.IsEmpty() && !std::isalnum(m_text.Back()))
    {
        m_text.PopBack();
    }
    while (!m_text.IsEmpty() && std::isalnum(m_text.Back()))
    {
        m_text.PopBack();
    }
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetTextboxComponent::SetMaxCharacters(UInt32 maxChars)
{
    m_maxCharacters = maxChars;

    if (m_maxCharacters > 0 && m_text.Length() > m_maxCharacters)
    {
        m_text = m_text.SubStr(0, m_maxCharacters);
    }
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetTextboxComponent::SetMinBoxChars(UInt32 minChars)
{
    m_minBoxChars = minChars;
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetTextboxComponent::SetMaxBoxChars(UInt32 maxChars)
{
    m_maxBoxChars = maxChars;
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetTextboxComponent::SetAdaptSizeToText(bool adapt)
{
    m_adaptSizeToText = adapt;
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetTextboxComponent::CaptureTypedText(void)
{
    if (!m_inputManager) { return; }

    bool atMaxCapacity =
        (m_maxCharacters > 0 && m_text.Length() >= m_maxCharacters);

    bool shiftHeld = m_inputManager->IsPressed(EInput::Keyboard_LShift) ||
                     m_inputManager->IsJustReleased(EInput::Keyboard_LShift) ||
                     m_inputManager->IsPressed(EInput::Keyboard_RShift) ||
                     m_inputManager->IsJustReleased(EInput::Keyboard_RShift);

    bool ctrlHeld =
        m_inputManager->IsPressed(EInput::Keyboard_LControl) ||
        m_inputManager->IsJustReleased(EInput::Keyboard_LControl) ||
        m_inputManager->IsPressed(EInput::Keyboard_RControl) ||
        m_inputManager->IsJustReleased(EInput::Keyboard_RControl);

    const auto& map = shiftHeld ? tkd::KeyboardMapping::QWERTY_TO_AZERTY_SHIFT
                                : tkd::KeyboardMapping::QWERTY_TO_AZERTY;

    // Backspace
    bool backspaceheld =
        m_inputManager->IsPressed(EInput::Keyboard_Backspace) ||
        m_inputManager->IsJustReleased(EInput::Keyboard_Backspace);

    if (backspaceheld && !m_text.IsEmpty() &&
        m_keyCooldowns[EInput::Keyboard_Backspace] <= 0.0f)
    {
        m_keyCooldowns[EInput::Keyboard_Backspace] = 0.15f;

        if (ctrlHeld && !m_text.IsEmpty()) { EraseCtrlBackspace(); }
        else { m_text.PopBack(); }
    }

    if (atMaxCapacity) { return; }

    // Space
    if (m_inputManager->IsJustPressed(EInput::Keyboard_Space) &&
        m_keyCooldowns[EInput::Keyboard_Space] <= 0.0f)
    {
        m_keyCooldowns[EInput::Keyboard_Space] = 0.15f;
        if (ctrlHeld && !m_text.IsEmpty()) { m_text += "."; }
        else if (shiftHeld && !m_text.IsEmpty()) { m_text += "."; }
        else { m_text += " "; }
    }

    // Other characters
    for (const auto& [key, character]: map)
    {
        if (m_inputManager->IsJustPressed(key) && m_keyCooldowns[key] <= 0.f)
        {
            m_keyCooldowns[key] = 0.15f;
            m_text += FString(1, character);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetTextboxComponent::BeginPlay(void)
{
    Super::BeginPlay();

    m_rectangleShape.SetFillColor(m_backgroundColor);
    m_rectangleShape.SetPosition(GetPosition());
    m_rectangleShape.SetSize(GetSize());
    m_rectangleShape.SetOrigin(GetOrigin());

    m_textShape.SetFont(m_fontPath);
    m_textShape.SetCharacterSize(m_charSize);
    m_textShape.SetSpacing(m_spacing);
    m_textShape.SetString(m_text);
    m_textShape.SetPosition(GetPosition());
    m_textShape.SetColor(m_textColor);
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetTextboxComponent::EndPlay(void) { Super::EndPlay(); }

///////////////////////////////////////////////////////////////////////////////
void UWidgetTextboxComponent::Tick(Float32 deltaTime)
{
    Super::Tick(deltaTime);

    m_rectangleShape.SetPosition(GetPosition());
    m_rectangleShape.SetOrigin(GetOrigin());

    if (Super::IsFocused())
    {
        m_rectangleShape.SetFillColor(m_focusedBackgroundColor);
    }
    else { m_rectangleShape.SetFillColor(m_backgroundColor); }

    m_textShape.SetString(m_text);
    m_textShape.SetPosition(GetPosition());
    m_textShape.SetCharacterSize(m_charSize);
    m_textShape.SetSpacing(m_spacing);
    m_textShape.SetColor(m_textColor);

    float charWidth = m_charSize.x + m_spacing;
    float textHeight = m_charSize.y;
    UInt32 textLength = m_text.Length();

    if (m_adaptSizeToText)
    {
        UInt32 visibleChars =
            std::clamp(textLength, m_minBoxChars, m_maxBoxChars);

        m_size.x = visibleChars * charWidth + 20.0f;   // +20 for padding
        m_size.y = textHeight + 10.0f;                 // +10 for padding
    }
    m_rectangleShape.SetSize(GetSize());

    if (GetAlignment() == EAlignment::None)
    {
        m_textShape.SetOrigin(FVector2::Zero);
    }
    else
    {
        int alignIndex = static_cast<int>(GetAlignment()) - 1;
        int alignX = alignIndex % 3;
        int alignY = alignIndex / 3;

        float textWidth = m_textShape.GetTextWidth();

        FVector2 textOrigin(
            alignX * textWidth * 0.5f, alignY * textHeight * 0.5f
        );

        m_textShape.SetOrigin(textOrigin);
    }

    UpdateInputStates();

    for (auto& [key, timer]: m_keyCooldowns)
    {
        timer = std::max(0.0f, timer - deltaTime);
    }

    if (Super::IsFocused()) { CaptureTypedText(); }
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetTextboxComponent::Render(IRenderer& renderer) const
{
    if (!IsVisible()) { return; }

    m_rectangleShape.Draw(renderer);
    m_textShape.Draw(renderer);
}

}   // namespace tkd
