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
    , m_transform()
    , m_textureRect(FRectangle::Zero)
    , m_color(FColor::White)
    , m_blendMode(EBlendMode::Alpha)
    , m_flipX(false)
    , m_flipY(false)
{}

///////////////////////////////////////////////////////////////////////////////
USprite::USprite(const ITexture* texture)
    : m_texture(texture)
    , m_transform()
    , m_textureRect(FRectangle::Zero)
    , m_color(FColor::White)
    , m_blendMode(EBlendMode::Alpha)
    , m_flipX(false)
    , m_flipY(false)
{
    if (texture && texture->IsValid())
    {
        FVector2u size = texture->GetSize();
        m_textureRect = FRectangle(
            0.0f, 0.0f, static_cast<float>(size.x), static_cast<float>(size.y)
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
void USprite::SetTexture(const ITexture* texture, bool resetRect)
{
    m_texture = texture;

    if (resetRect && texture && texture->IsValid())
    {
        FVector2u size = texture->GetSize();
        m_textureRect = FRectangle(
            0.0f, 0.0f, static_cast<float>(size.x), static_cast<float>(size.y)
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
const ITexture* USprite::GetTexture(void) const { return m_texture; }

///////////////////////////////////////////////////////////////////////////////
void USprite::SetTextureRect(const FRectangle& rect) { m_textureRect = rect; }

///////////////////////////////////////////////////////////////////////////////
const FRectangle& USprite::GetTextureRect(void) const { return m_textureRect; }

///////////////////////////////////////////////////////////////////////////////
void USprite::SetColor(const FColor& color) { m_color = color; }

///////////////////////////////////////////////////////////////////////////////
const FColor& USprite::GetColor(void) const { return m_color; }

///////////////////////////////////////////////////////////////////////////////
void USprite::SetPosition(const FVector2f& position)
{
    m_transform.SetPosition(position);
}

///////////////////////////////////////////////////////////////////////////////
const FVector2f& USprite::GetPosition(void) const
{
    return m_transform.GetPosition();
}

///////////////////////////////////////////////////////////////////////////////
void USprite::SetRotation(float rotation)
{
    m_transform.SetRotation(rotation);
}

///////////////////////////////////////////////////////////////////////////////
float USprite::GetRotation(void) const
{
    return m_transform.GetRotation().GetAngle();
}

///////////////////////////////////////////////////////////////////////////////
void USprite::SetScale(const FVector2f& scale) { m_transform.SetScale(scale); }

///////////////////////////////////////////////////////////////////////////////
const FVector2f& USprite::GetScale(void) const
{
    return m_transform.GetScale();
}

///////////////////////////////////////////////////////////////////////////////
void USprite::SetOrigin(const FVector2f& origin)
{
    // TODO: Update when the transform has been implemented
    // m_transform.SetOrigin(origin);
}

///////////////////////////////////////////////////////////////////////////////
const FVector2f& USprite::GetOrigin(void) const
{
    // TODO: Update when transform has been implemented
    // return m_transform.GetOrigin();
    static const FVector2f defaultOrigin(0.0f, 0.0f);
    return defaultOrigin;
}

///////////////////////////////////////////////////////////////////////////////
void USprite::SetTransform(const FTransform2D& transform)
{
    m_transform = transform;
}

///////////////////////////////////////////////////////////////////////////////
const FTransform2D& USprite::GetTransform(void) const { return m_transform; }

///////////////////////////////////////////////////////////////////////////////
void USprite::SetBlendMode(EBlendMode blendMode) { m_blendMode = blendMode; }

///////////////////////////////////////////////////////////////////////////////
EBlendMode USprite::GetBlendMode(void) const { return m_blendMode; }

///////////////////////////////////////////////////////////////////////////////
void USprite::SetFlipX(bool flip) { m_flipX = flip; }

///////////////////////////////////////////////////////////////////////////////
bool USprite::IsFlipX(void) const { return m_flipX; }

///////////////////////////////////////////////////////////////////////////////
void USprite::SetFlipY(bool flip) { m_flipY = flip; }

///////////////////////////////////////////////////////////////////////////////
bool USprite::IsFlipY(void) const { return m_flipY; }

///////////////////////////////////////////////////////////////////////////////
FRectangle USprite::GetLocalBounds(void) const
{
    // Return the texture rectangle dimensions as local bounds
    return FRectangle(0.0f, 0.0f, m_textureRect.width, m_textureRect.height);
}

///////////////////////////////////////////////////////////////////////////////
FRectangle USprite::GetGlobalBounds(void) const
{
    // TODO: Update when transform has been implemented
    // Apply transform to local bounds to get global bounds
    // FRectangle localBounds = GetLocalBounds();
    // return m_transform.TransformRect(localBounds);
    return GetLocalBounds();
}

///////////////////////////////////////////////////////////////////////////////
void USprite::Draw(IRenderer* renderer, const FRenderStates& states) const
{
    if (!renderer || !m_texture || !m_texture->IsValid()) { return; }

    // Create vertices for the sprite quad
    FVertex2D vertices[4];
    UpdateVertices(vertices);

    // Combine render states
    FRenderStates finalStates = states;
    finalStates.texture = m_texture;
    finalStates.blendMode = m_blendMode;

    // TODO: Update when transform has been implemented
    // finalStates.transform = m_transform * states.transform;

    // Draw the quad
    renderer->Draw(vertices, 4, EPrimitiveType::TriangleStrip, finalStates);
}

///////////////////////////////////////////////////////////////////////////////
void USprite::UpdateVertices(FVertex2D* vertices) const
{
    if (!vertices) { return; }

    // Get texture rectangle
    float left = m_textureRect.left;
    float top = m_textureRect.top;
    float right = left + m_textureRect.width;
    float bottom = top + m_textureRect.height;

    // Handle flipping
    if (m_flipX)
    {
        float temp = left;
        left = right;
        right = temp;
    }

    if (m_flipY)
    {
        float temp = top;
        top = bottom;
        bottom = temp;
    }

    // Normalize texture coordinates if texture is valid
    if (m_texture && m_texture->IsValid())
    {
        FVector2u textureSize = m_texture->GetSize();
        if (textureSize.x > 0 && textureSize.y > 0)
        {
            left /= static_cast<float>(textureSize.x);
            right /= static_cast<float>(textureSize.x);
            top /= static_cast<float>(textureSize.y);
            bottom /= static_cast<float>(textureSize.y);
        }
    }

    // Set up quad vertices (triangle strip order)
    // Top-left
    vertices[0].position = FVector2f(0.0f, 0.0f);
    vertices[0].uv = FVector2f(left, top);
    vertices[0].color = m_color;

    // Top-right
    vertices[1].position = FVector2f(m_textureRect.width, 0.0f);
    vertices[1].uv = FVector2f(right, top);
    vertices[1].color = m_color;

    // Bottom-left
    vertices[2].position = FVector2f(0.0f, m_textureRect.height);
    vertices[2].uv = FVector2f(left, bottom);
    vertices[2].color = m_color;

    // Bottom-right
    vertices[3].position =
        FVector2f(m_textureRect.width, m_textureRect.height);
    vertices[3].uv = FVector2f(right, bottom);
    vertices[3].color = m_color;
}

}   // namespace tkd
