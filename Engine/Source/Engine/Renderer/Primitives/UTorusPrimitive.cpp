///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Renderer/Primitives/UTorusPrimitive.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
UTorusPrimitive::UTorusPrimitive(
    SizeT majorSectors,
    SizeT minorSectors,
    float majorRadius,
    float minorRadius,
    const FColor& color
)
    : UPrimitive(EPrimitiveType::QuadStrip, color)
    , m_majorSectors(majorSectors)
    , m_minorSectors(minorSectors)
    , m_majorRadius(majorRadius)
    , m_minorRadius(minorRadius)
{
    UpdateVertices();
}

///////////////////////////////////////////////////////////////////////////////
void UTorusPrimitive::SetMajorSectors(SizeT sectors)
{
    if (m_majorSectors != sectors)
    {
        m_majorSectors = sectors;
        UpdateVertices();
    }
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD SizeT UTorusPrimitive::GetMajorSectors(void) const
{
    return m_majorSectors;
}

///////////////////////////////////////////////////////////////////////////////
void UTorusPrimitive::SetMinorSectors(SizeT sectors)
{
    if (m_minorSectors != sectors)
    {
        m_minorSectors = sectors;
        UpdateVertices();
    }
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD SizeT UTorusPrimitive::GetMinorSectors(void) const
{
    return m_minorSectors;
}

///////////////////////////////////////////////////////////////////////////////
void UTorusPrimitive::SetMajorRadius(float radius)
{
    if (m_majorRadius != radius)
    {
        m_majorRadius = radius;
        UpdateVertices();
    }
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD float UTorusPrimitive::GetMajorRadius(void) const
{
    return m_majorRadius;
}

///////////////////////////////////////////////////////////////////////////////
void UTorusPrimitive::SetMinorRadius(float radius)
{
    if (m_minorRadius != radius)
    {
        m_minorRadius = radius;
        UpdateVertices();
    }
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD float UTorusPrimitive::GetMinorRadius(void) const
{
    return m_minorRadius;
}

///////////////////////////////////////////////////////////////////////////////
void UTorusPrimitive::Draw(IRenderer& renderer, FRenderStates states) const
{
    // State parameter is currently unused
    TKD_UNUSED(states);

    // Draw the vertex strips using the renderer
    for (const auto& strip: m_strips)
    {
        renderer.Draw(strip, m_primitiveType, GetTransform());
    }
}

///////////////////////////////////////////////////////////////////////////////
void UTorusPrimitive::UpdateVertices(void)
{
    m_vertices.clear();
    m_strips.clear();

    for (SizeT i = 0; i < m_majorSectors; i++)
    {
        std::vector<FVertex> strip;

        for (SizeT j = 0; j <= m_minorSectors; j++)
        {
            for (int k = 0; k < 2; k++)
            {
                float u = (i + k) * 2.0f * M_PI / m_majorSectors;
                float v = j * 2.0f * M_PI / m_minorSectors;

                float x = (m_majorRadius + m_minorRadius * std::cos(v)) *
                          std::cos(u);
                float y = m_minorRadius * std::sin(v);
                float z = (m_majorRadius + m_minorRadius * std::cos(v)) *
                          std::sin(u);

                strip.push_back(FVertex(FVector3(x, y, z), m_color));
            }
        }

        m_strips.push_back(strip);
    }

    // Apply the origin offset to center the torus
    for (auto& strip: m_strips)
    {
        for (auto& vertex: strip) { vertex.position -= m_origin; }
    }
}

///////////////////////////////////////////////////////////////////////////////
void UTorusPrimitive::SetColor(const FColor& color)
{
    m_color = color;
    for (auto& strip: m_strips)
    {
        for (FVertex& vertex: strip) { vertex.color = color; }
    }
}

}   // namespace tkd
