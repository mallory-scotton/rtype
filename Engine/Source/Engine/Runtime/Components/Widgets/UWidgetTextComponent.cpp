///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Runtime/Components/Widgets/UWidgetTextComponent.hpp>
#include <Engine/Runtime/Actor/AActor.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
UWidgetTextComponent::UWidgetTextComponent(const FString& name)
    : UWidgetComponent(name)
    , m_color(FColor::White)
    , m_text("")
    , m_charSize(FVector2(16.0f, 16.0f))
    , m_spacing(0.20f)
    , m_textShape()
    , m_fontPath()
    , m_isFocused(false)
{}

///////////////////////////////////////////////////////////////////////////////
const FColor& UWidgetTextComponent::GetColor(void) const { return m_color; }

///////////////////////////////////////////////////////////////////////////////
void UWidgetTextComponent::SetColor(const FColor& color)
{
    m_color = color;
    m_textShape.SetColor(m_color);
}

///////////////////////////////////////////////////////////////////////////////
const FString& UWidgetTextComponent::GetText(void) const
{
    return m_textShape.GetString();
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetTextComponent::SetText(const FString& text)
{
    m_text = text;
    m_textShape.SetString(m_text);
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetTextComponent::SetFontPath(const FilePath& path)
{
    m_fontPath = path;
    m_textShape.SetFont(m_fontPath);
}

///////////////////////////////////////////////////////////////////////////////
const FilePath& UWidgetTextComponent::GetFontPath(void) const
{
    return m_fontPath;
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetTextComponent::SetCharacterSize(const FVector2 size)
{
    m_charSize = size;
    m_textShape.SetCharacterSize(m_charSize);
}

///////////////////////////////////////////////////////////////////////////////
const FVector2 UWidgetTextComponent::GetCharacterSize(void) const
{
    return m_charSize;
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetTextComponent::SetSpacing(const Float32 spacing)
{
    m_spacing = spacing;
    m_textShape.SetSpacing(m_spacing);
}

///////////////////////////////////////////////////////////////////////////////
Float32 UWidgetTextComponent::GetSpacing(void) const { return m_spacing; }

///////////////////////////////////////////////////////////////////////////////
void UWidgetTextComponent::BeginPlay(void)
{
    Super::BeginPlay();

    m_textShape.SetString(m_text);
    m_textShape.SetPosition(GetPosition());
    m_textShape.SetColor(m_color);
    m_textShape.SetCharacterSize(m_charSize);
    m_textShape.SetFont(m_fontPath);
    m_textShape.SetSpacing(m_spacing);
    m_textShape.SetOrigin(Super::GetOrigin());
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetTextComponent::EndPlay(void) { Super::EndPlay(); }

///////////////////////////////////////////////////////////////////////////////
void UWidgetTextComponent::Tick(Float32 deltaTime)
{
    Super::Tick(deltaTime);

    // Update text properties
    m_textShape.SetString(m_text);
    m_textShape.SetColor(m_color);
    m_textShape.SetCharacterSize(m_charSize);
    m_textShape.SetSpacing(m_spacing);
    m_textShape.SetPosition(GetPosition());

    // Calculate and set origin based on alignment
    if (GetAlignment() == EAlignment::None)
    {
        m_textShape.SetOrigin(FVector2::Zero);
    }
    else
    {
        int alignIndex = static_cast<int>(GetAlignment()) - 1;
        int alignX = alignIndex % 3;   // 0=Left, 1=Center, 2=Right
        int alignY = alignIndex / 3;   // 0=Top, 1=Center, 2=Bottom

        float textWidth = m_textShape.GetTextWidth();
        float textHeight = m_charSize.y;

        FVector2 textOrigin(
            alignX * textWidth * 0.5f, alignY * textHeight * 0.5f
        );

        m_textShape.SetOrigin(textOrigin);
    }
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetTextComponent::Render(IRenderer& renderer) const
{
    if (IsVisible()) { m_textShape.Draw(renderer); }
}

}   // namespace tkd
