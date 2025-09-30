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
    , m_texture(nullptr)
    , m_origin(FVector2f::Zero)
    , m_vertices(EPrimitiveType::TriangleFan)
    , m_outlineVertices(EPrimitiveType::TriangleStrip)
{}

///////////////////////////////////////////////////////////////////////////////
void UShape::SetFillColor(const FColor& color)
{
    m_fillColor = color;
    m_vertices.ForEach([&color](FVertex2D& vertex) { vertex.color = color; });
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
    m_outlineVertices.ForEach([&color](FVertex2D& vertex)
                              { vertex.color = color; });
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
    UpdateOutlineGeometry();
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD float UShape::GetOutlineThickness(void) const
{
    return m_outlineThickness;
}

///////////////////////////////////////////////////////////////////////////////
void UShape::SetTexture(const ITexture* texture, bool resetRect)
{
    if (texture)
    {
        if (resetRect || (!m_texture && m_textureRect == FRectanglei::Zero))
        {
            // Reset texture rectangle to full texture size
            m_textureRect = FRectanglei(
                0,
                0,
                static_cast<int>(texture->GetWidth()),
                static_cast<int>(texture->GetHeight())
            );
        }
    }

    m_texture = texture;
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD const ITexture* UShape::GetTexture(void) const
{
    return m_texture;
}

///////////////////////////////////////////////////////////////////////////////
void UShape::SetPosition(const FVector2f& position)
{
    m_transform.SetPosition(position);
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD FVector2f UShape::GetPosition(void) const
{
    return m_transform.GetPosition();
}

///////////////////////////////////////////////////////////////////////////////
void UShape::SetRotation(float rotation) { m_transform.SetRotation(rotation); }

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD float UShape::GetRotation(void) const
{
    return m_transform.GetRotation().GetAngle();
}

///////////////////////////////////////////////////////////////////////////////
void UShape::SetScale(const FVector2f& scale) { m_transform.SetScale(scale); }

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD FVector2f UShape::GetScale(void) const
{
    return m_transform.GetScale();
}

///////////////////////////////////////////////////////////////////////////////
void UShape::SetOrigin(const FVector2f& origin) { m_origin = origin; }

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD const FVector2f& UShape::GetOrigin(void) const
{
    return m_origin;
}

///////////////////////////////////////////////////////////////////////////////
void UShape::SetTransform(const FTransform2D& transform)
{
    m_transform = transform;
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD const FTransform2D& UShape::GetTransform(void) const
{
    return m_transform;
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD FTransform2D UShape::GetOriginTransform(void) const
{
    FTransform2D originTransform = m_transform;
    originTransform.Translate((GetLocalBounds().GetSize() * 0.5f) - m_origin);
    return originTransform;
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD FRectangle UShape::GetLocalBounds(void) const
{
    return m_bounds;
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD FRectangle UShape::GetGlobalBounds(void) const
{
    return GetOriginTransform().TransformRectangle(GetLocalBounds());
}

///////////////////////////////////////////////////////////////////////////////
void UShape::Draw(IRenderer& renderer, FRenderStates states) const
{
    states.transform = states.transform * GetOriginTransform();
    states.texture = m_texture;

    renderer.Draw(
        m_vertices.Data(),
        static_cast<UInt32>(m_vertices.Size()),
        EPrimitiveType::TriangleFan,
        states
    );

    if (m_outlineThickness > 0.0f)
    {
        states.texture = nullptr;
        renderer.Draw(
            m_outlineVertices.Data(),
            static_cast<UInt32>(m_outlineVertices.Size()),
            EPrimitiveType::TriangleStrip,
            states
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
void UShape::UpdateGeometry(void)
{
    const SizeT count = GetPointCount();
    if (count < 3)
    {
        m_vertices.Clear();
        m_outlineVertices.Clear();
        return;
    }

    m_vertices.Resize(count + 2);

    for (SizeT i = 0; i < count; i++)
    {
        m_vertices[i + 1].position = GetPoint(i);
    }
    m_vertices[count + 1].position = m_vertices[1].position;

    m_vertices[0] = m_vertices[1];
    m_insideBounds = m_vertices.GetBounds();

    m_vertices[0].position = m_insideBounds.GetCenter();

    m_vertices.ForEach([this](FVertex2D& vertex)
                       { vertex.color = m_fillColor; });
    UpdateUVs();
    UpdateOutlineGeometry();
}

///////////////////////////////////////////////////////////////////////////////
void UShape::UpdateOutlineGeometry(void)
{
    if (m_outlineThickness == 0.0f || m_vertices.Size() < 2)
    {
        m_outlineVertices.Clear();
        m_bounds = m_insideBounds;
        return;
    }
}

///////////////////////////////////////////////////////////////////////////////
void UShape::UpdateUVs(void)
{
    const FRectanglef convertexTextureRect(m_textureRect);
    const FVector2f size = m_insideBounds.GetSize();
    const FVector2f safeInsideSize(
        size.x > 0 ? size.x : 1.f, size.y > 0 ? size.y : 1.f
    );

    m_vertices.ForEach(
        [&convertexTextureRect, &safeInsideSize, this](FVertex2D& vertex)
        {
            const FVector2f ratio =
                (vertex.position - m_insideBounds.GetPosition()) /
                safeInsideSize;
            vertex.uv = convertexTextureRect.GetPosition() +
                        convertexTextureRect.GetSize() * ratio;
        }
    );
}

}   // namespace tkd
