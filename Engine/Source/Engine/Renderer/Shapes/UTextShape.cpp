// UTextShape.cpp
///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Renderer/Shapes/UTextShape.hpp>
#include <Engine/Assets/URessource.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
UTextShape::UTextShape()
    : m_text("")
    , m_position(FVector2::Zero)
    , m_characterSize(16.0f, 16.0f)           // Default character size
    , m_sourceCharacterSize(100.0f, 100.0f)   // Default source character size
    , m_spacing(2.0f)                         // Default spacing
    , m_color(FColor::White)
    , m_fontTexture()
    , m_fontPath()
    , m_origin(FVector2::Zero)
{}

///////////////////////////////////////////////////////////////////////////////
void UTextShape::SetFont(const FilePath& fontPath)
{
    if (fontPath.empty())
    {
        m_fontTexture = {};
        m_fontPath = "";
        return;
    }

    m_fontPath = fontPath;
    m_fontTexture = URessource::GetTextureHandle(fontPath);

    if (!m_fontTexture.IsValid()) { return; }

    ITexture* texture = m_fontTexture.Get();
    if (!texture) { return; }

    // Calculate SOURCE size from texture (for texture rect calculation)
    FVector2u texSize = texture->GetSize();
    m_sourceCharacterSize.x = static_cast<Float32>(texSize.x) / CHARS_PER_ROW;
    m_sourceCharacterSize.y = static_cast<Float32>(texSize.y) / TOTAL_ROWS;

    // Display size stays whatever was set with SetCharacterSize()
}

///////////////////////////////////////////////////////////////////////////////
void UTextShape::SetString(const FString& text) { m_text = text; }

///////////////////////////////////////////////////////////////////////////////
void UTextShape::SetPosition(const FVector2& position)
{
    m_position = position;
}

///////////////////////////////////////////////////////////////////////////////
void UTextShape::SetCharacterSize(const FVector2& size)
{
    m_characterSize = size;
}

///////////////////////////////////////////////////////////////////////////////
void UTextShape::SetSpacing(Float32 spacing) { m_spacing = spacing; }

///////////////////////////////////////////////////////////////////////////////
void UTextShape::SetColor(const FColor& color) { m_color = color; }

///////////////////////////////////////////////////////////////////////////////
Float32 UTextShape::GetTextWidth() const
{
    if (m_text.IsEmpty()) { return 0.0f; }

    return (m_characterSize.x + m_spacing) * m_text.Length() - m_spacing;
}

///////////////////////////////////////////////////////////////////////////////
FRectanglei UTextShape::GetCharacterRect(char character) const
{
    Int32 charIndex = static_cast<Int32>(character) - FIRST_CHAR;

    if (charIndex < 0 || charIndex > (LAST_CHAR - FIRST_CHAR))
    {
        charIndex = 0;
    }

    Int32 row = charIndex / CHARS_PER_ROW;
    Int32 col = charIndex % CHARS_PER_ROW;

    // Use SOURCE size for texture coordinates
    Int32 charWidth = static_cast<Int32>(m_sourceCharacterSize.x);
    Int32 charHeight = static_cast<Int32>(m_sourceCharacterSize.y);

    return FRectanglei(
        col * charWidth, row * charHeight, charWidth, charHeight
    );
}

///////////////////////////////////////////////////////////////////////////////
void UTextShape::SetOrigin(const FVector2& origin) { m_origin = origin; }

///////////////////////////////////////////////////////////////////////////////
void UTextShape::Draw(IRenderer& renderer) const
{
    if (!m_fontTexture.IsValid() || m_text.IsEmpty()) { return; }

    ITexture* fontTexture = m_fontTexture.Get();
    if (!fontTexture) { return; }

    Float32 currentX = m_position.x - m_origin.x;
    Float32 currentY = m_position.y - m_origin.y;

    for (char c: m_text.ToStdString())
    {
        FRectanglei charRect = GetCharacterRect(c);

        USprite charSprite;
        charSprite.SetTexture(*fontTexture);
        charSprite.SetTextureRect(charRect);
        charSprite.SetPosition(FVector2(currentX, currentY));
        charSprite.SetColor(m_color);

        // Scale from SOURCE size (in file) to DISPLAY size (on screen)
        FVector2 scale(
            m_characterSize.x / m_sourceCharacterSize.x,
            m_characterSize.y / m_sourceCharacterSize.y
        );
        charSprite.SetScale(scale);

        FRenderStates states;
        states.transform = FTransform2D::Identity;
        charSprite.Draw(renderer, states);

        // Move by DISPLAY size
        currentX += m_characterSize.x + m_spacing;
    }
}

}   // namespace tkd
