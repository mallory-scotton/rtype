///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Renderer/UShape.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
UShape::UShape(void)
    : m_transform()
    , m_fillColor(FColor::White)
    , m_outlineColor(FColor::Black)
    , m_outlineThickness(0.0f)
    , m_blendMode(EBlendMode::Alpha)
    , m_texture(nullptr)
    , m_needsUpdate(true)
{}

///////////////////////////////////////////////////////////////////////////////
void UShape::SetFillColor(const FColor& color)
{
    m_fillColor = color;
    m_needsUpdate = true;
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD const FColor& UShape::GetFillColor(void) const
{
    return m_fillColor;
}

///////////////////////////////////////////////////////////////////////////////
void UShape::SetOutlineColor(const FColor& color)
{
    m_outlineColor = color;
    m_needsUpdate = true;
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD const FColor& UShape::GetOutlineColor(void) const
{
    return m_outlineColor;
}

///////////////////////////////////////////////////////////////////////////////
void UShape::SetOutlineThickness(float thickness)
{
    m_outlineThickness = thickness;
    m_needsUpdate = true;
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD float UShape::GetOutlineThickness(void) const
{
    return m_outlineThickness;
}

///////////////////////////////////////////////////////////////////////////////
void UShape::SetTexture(const ITexture* texture)
{
    m_texture = texture;
    m_needsUpdate = true;
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD const ITexture* UShape::GetTexture(void) const
{
    return m_texture;
}

///////////////////////////////////////////////////////////////////////////////
void UShape::SetPosition(const FVector2f& position)
{
    // TODO: Change with real transform operation
    // m_transform.SetPosition(position);
    m_needsUpdate = true;
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD const FVector2f& UShape::GetPosition(void) const
{
    // TODO: Change with real transform operation
    return FVector2();
    // return m_transform.GetPosition();
}

///////////////////////////////////////////////////////////////////////////////
void UShape::SetRotation(float rotation)
{
    // TODO: Change with real transform operation
    // m_transform.SetRotation(rotation);
    m_needsUpdate = true;
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD float UShape::GetRotation(void) const
{
    // TODO: Change with real transform operation
    return 0.0f;
    // return m_transform.GetRotation();
}

///////////////////////////////////////////////////////////////////////////////
void UShape::SetScale(const FVector2f& scale)
{
    // TODO: Change with real transform operation
    // m_transform.SetScale(scale);
    m_needsUpdate = true;
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD const FVector2f& UShape::GetScale(void) const
{
    // TODO: Change with real transform operation
    return FVector2f::One;
    // return m_transform.GetScale();
}

///////////////////////////////////////////////////////////////////////////////
void UShape::SetOrigin(const FVector2f& origin)
{
    // TODO: Change with real transform operation
    // m_transform.SetOrigin(origin);
    m_needsUpdate = true;
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD const FVector2f& UShape::GetOrigin(void) const
{
    // TODO: Change with real transform operation
    return FVector2f::Zero;
    // return m_transform.GetOrigin();
}

///////////////////////////////////////////////////////////////////////////////
void UShape::SetTransform(const FTransform2D& transform)
{
    m_transform = transform;
    m_needsUpdate = true;
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD const FTransform2D& UShape::GetTransform(void) const
{
    return m_transform;
}

///////////////////////////////////////////////////////////////////////////////
void UShape::SetBlendMode(EBlendMode blendMode)
{
    m_blendMode = blendMode;
    m_needsUpdate = true;
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD EBlendMode UShape::GetBlendMode(void) const
{
    return m_blendMode;
}

}   // namespace tkd
