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
    : m_fillColor(FColor::White)
    , m_outlineColor(FColor::Black)
    , m_outlineThickness(0.0f)
    , m_texture(nullptr)
    , m_vertices(EPrimitiveType::TriangleFan)
    , m_outlineVertices(EPrimitiveType::TriangleStrip)
    , m_miterLimit(10.0f)
{}

///////////////////////////////////////////////////////////////////////////////
void UShape::SetMiterLimit(float limit)
{
    m_miterLimit = limit;
    UpdateOutlineGeometry();
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD float UShape::GetMiterLimit(void) const { return m_miterLimit; }

///////////////////////////////////////////////////////////////////////////////
void UShape::SetFillColor(const FColor& color)
{
    m_fillColor = color;
    for (auto& vertex: m_vertices) { vertex.color = color; }
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
    for (auto& vertex: m_outlineVertices) { vertex.color = color; }
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
TKD_NODISCARD FRectangle UShape::GetLocalBounds(void) const
{
    return m_bounds;
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD FRectangle UShape::GetGlobalBounds(void) const
{
    return GetTransform().TransformRectangle(GetLocalBounds());
}

///////////////////////////////////////////////////////////////////////////////
void UShape::Draw(IRenderer& renderer, FRenderStates states) const
{
    states.transform *= GetTransform();
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
FVector2f UShape::GetGeometricCenter(void) const
{
    const SizeT count = GetPointCount();

    switch (count)
    {
    case 0: return FVector2f::Zero;
    case 1: return GetPoint(0);
    case 2: return (GetPoint(0) + GetPoint(1)) * 0.5f;
    default:
    {
        FVector2f centroid;
        float twiceArea = 0.0f;

        FVector2f previousPoint = GetPoint(count - 1);
        for (SizeT i = 0; i < count; i++)
        {
            const FVector2f currentPoint = GetPoint(i);
            const float product = previousPoint.Cross(currentPoint);
            twiceArea += product;
            centroid += (previousPoint + currentPoint) * product;
            previousPoint = currentPoint;
        }

        if (twiceArea != 0.f) { return centroid / 3.f / twiceArea; }

        FVector2f minPoint = GetPoint(0);
        FVector2f maxPoint = minPoint;
        for (SizeT i = 1; i < count; i++)
        {
            const FVector2f point = GetPoint(i);
            minPoint.x = std::min(minPoint.x, point.x);
            minPoint.y = std::min(minPoint.y, point.y);
            maxPoint.x = std::max(maxPoint.x, point.x);
            maxPoint.y = std::max(maxPoint.y, point.y);
        }
        return (minPoint + maxPoint) * 0.5f;
    }
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

    for (auto& vertex: m_vertices) { vertex.color = m_fillColor; }
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

    const SizeT count = m_vertices.Size() - 2;
    m_outlineVertices.Resize((count + 1) * 2);

    const bool flipNormals = [this, count]()
    {
        const FVector2f p0 = m_vertices[0].position;
        for (SizeT i = 0; i < count; i++)
        {
            const FVector2f p1 = m_vertices[i + 1].position;
            const FVector2f p2 = m_vertices[i + 2].position;
            const float product = (p1 - p0).Cross(p2 - p0);
            if (product == 0.f) { continue; }
            return product > 0.f;
        }
        return true;
    }();

    SizeT outlineIndex = 0;
    for (SizeT i = 0; i < count; i++)
    {
        const SizeT index = i + 1;

        const FVector2f p0 = (i == 0) ? m_vertices[count].position
                                      : m_vertices[index - 1].position;
        const FVector2f p1 = m_vertices[index].position;
        const FVector2f p2 = m_vertices[index + 1].position;

        const FVector2f d1 = ComputeDirection(p0, p1);
        const FVector2f d2 = ComputeDirection(p1, p2);

        const FVector2f n1 =
            flipNormals ? -d1.Perpendicular() : d1.Perpendicular();
        const FVector2f n2 =
            flipNormals ? -d2.Perpendicular() : d2.Perpendicular();

        const float twoCos2 = 1.f + n1.Dot(n2);
        const float squaredLengthRatio =
            m_miterLimit * m_miterLimit * twoCos2 / 2.f;
        const bool isConvexCorner = d1.Dot(n2) * m_outlineThickness >= 0.f;
        const bool needsBevel =
            twoCos2 == 0.0f || (squaredLengthRatio < 1.f && isConvexCorner);

        if (needsBevel)
        {
            m_outlineVertices.Resize(m_outlineVertices.Size() + 2);

            const float twoSin2 = 1.F - n1.Dot(n2);
            const FVector2f direction = (n2 - n1) / twoSin2;
            const FVector2f extrusion =
                (flipNormals != (d1.Dot(n2) >= 0.f) ? direction : -direction)
                    .Perpendicular();

            const float sin = std::sqrt(twoSin2 * 0.5f);
            const float u = m_miterLimit * sin;
            const float v = 1.f - std::sqrt(squaredLengthRatio);

            m_outlineVertices[outlineIndex++].position = p1;
            m_outlineVertices[outlineIndex++].position =
                p1 + (u * extrusion - v * direction) * m_outlineThickness;
            m_outlineVertices[outlineIndex++].position = p1;
            m_outlineVertices[outlineIndex++].position =
                p1 + (u * extrusion + v * direction) * m_outlineThickness;
        }
        else
        {
            const FVector2f extrusion = (n1 + n2) / twoCos2;

            m_outlineVertices[outlineIndex++].position = p1;
            m_outlineVertices[outlineIndex++].position =
                p1 + extrusion * m_outlineThickness;
        }
    }

    m_outlineVertices[outlineIndex++].position = m_outlineVertices[0].position;
    m_outlineVertices[outlineIndex++].position = m_outlineVertices[1].position;

    for (auto& vertex: m_outlineVertices) { vertex.color = m_outlineColor; }

    m_bounds = m_outlineVertices.GetBounds();
}

///////////////////////////////////////////////////////////////////////////////
FVector2f UShape::ComputeDirection(FVector2f p1, FVector2f p2)
{
    FVector2f direction = p2 - p1;
    const float length = direction.Length();
    if (length != 0.f) { direction /= length; }
    return direction;
}

///////////////////////////////////////////////////////////////////////////////
void UShape::UpdateUVs(void)
{
    const FRectanglef convertexTextureRect(m_textureRect);
    const FVector2f size = m_insideBounds.GetSize();
    const FVector2f safeInsideSize(
        size.x > 0 ? size.x : 1.f, size.y > 0 ? size.y : 1.f
    );

    for (auto& vertex: m_vertices)
    {
        const FVector2f ratio =
            (vertex.position - m_insideBounds.GetPosition()) / safeInsideSize;
        vertex.uv = convertexTextureRect.GetPosition() +
                    convertexTextureRect.GetSize() * ratio;
    }
}

}   // namespace tkd
