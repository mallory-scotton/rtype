///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Renderer/USprite.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
USprite::USprite(void)
    : m_texture(nullptr)
    , m_textureRect(FRectanglei::Zero)
{}

///////////////////////////////////////////////////////////////////////////////
USprite::USprite(const ITexture& texture)
    : m_texture(nullptr)
    , m_textureRect(FRectanglei::Zero)
{
    SetTexture(texture, true);
}

///////////////////////////////////////////////////////////////////////////////
USprite::USprite(const ITexture& texture, const FRectanglei& rect)
    : m_texture(nullptr)
    , m_textureRect(FRectanglei::Zero)
{
    SetTextureRect(rect);
    SetTexture(texture, false);
}

///////////////////////////////////////////////////////////////////////////////
void USprite::SetTexture(const ITexture& texture, bool resetRect)
{
    if (resetRect || (!m_texture && (m_textureRect == FRectanglei::Zero)))
    {
        FVector2u size = texture.GetSize();
        SetTextureRect(FRectanglei(
            0, 0, static_cast<int>(size.x), static_cast<int>(size.y)
        ));
    }
    m_texture = &texture;
}

///////////////////////////////////////////////////////////////////////////////
void USprite::SetTextureRect(const FRectanglei& rect)
{
    if (rect != m_textureRect)
    {
        m_textureRect = rect;
        UpdatePositions();
        UpdateUVs();
    }
}

///////////////////////////////////////////////////////////////////////////////
void USprite::SetColor(const FColor& color)
{
    m_vertices[0].color = color;
    m_vertices[1].color = color;
    m_vertices[2].color = color;
    m_vertices[3].color = color;
}

///////////////////////////////////////////////////////////////////////////////
void USprite::SetColor(const FLinearColor& color) { SetColor(FColor(color)); }

///////////////////////////////////////////////////////////////////////////////
const ITexture* USprite::GetTexture(void) const { return m_texture; }

///////////////////////////////////////////////////////////////////////////////
const FRectanglei USprite::GetTextureRect(void) const { return m_textureRect; }

///////////////////////////////////////////////////////////////////////////////
const FColor& USprite::GetColor(void) const { return m_vertices[0].color; }

///////////////////////////////////////////////////////////////////////////////
FRectangle USprite::GetLocalBounds(void) const
{
    float width = Math<float>::Abs(static_cast<float>(m_textureRect.width));
    float height = Math<float>::Abs(static_cast<float>(m_textureRect.height));
    return FRectangle(0.0f, 0.0f, width, height);
}

///////////////////////////////////////////////////////////////////////////////
FRectangle USprite::GetGlobalBounds(void) const
{
    return GetTransform().TransformRectangle(GetLocalBounds());
}

///////////////////////////////////////////////////////////////////////////////
void USprite::Draw(IRenderer& target, FRenderStates states) const
{
    if (m_texture)
    {
        states.texture = m_texture;
        states.transform *= GetTransform();
        target.Draw(m_vertices, 4, EPrimitiveType::TriangleStrip, states);
    }
}

///////////////////////////////////////////////////////////////////////////////
void USprite::UpdatePositions(void)
{
    FRectangle bounds = GetLocalBounds();

    m_vertices[0].position = FVector2(0.f, 0.f);
    m_vertices[1].position = FVector2(0.f, bounds.height);
    m_vertices[2].position = FVector2(bounds.width, 0.f);
    m_vertices[3].position = FVector2(bounds.width, bounds.height);
}

///////////////////////////////////////////////////////////////////////////////
void USprite::UpdateUVs(void)
{
    FRectangle convertedTextureRect = FRectangle(m_textureRect);

    float left = convertedTextureRect.left;
    float right = left + convertedTextureRect.width;
    float top = convertedTextureRect.top;
    float bottom = top + convertedTextureRect.height;

    m_vertices[0].uv = FVector2f(left, top);
    m_vertices[1].uv = FVector2f(left, bottom);
    m_vertices[2].uv = FVector2f(right, top);
    m_vertices[3].uv = FVector2f(right, bottom);
}

}   // namespace tkd
