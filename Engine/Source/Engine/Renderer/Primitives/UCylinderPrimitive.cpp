///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Renderer/Primitives/UCylinderPrimitive.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
UCylinderPrimitive::UCylinderPrimitive(
    SizeT sectorCount,
    float height,
    bool capTop,
    bool capBottom,
    const FColor& color
)
    : UPrimitive(EPrimitiveType::QuadStrip, color)
    , m_sectorCount(sectorCount)
    , m_height(height)
    , m_capTop(capTop)
    , m_capBottom(capBottom)
{
    GenerateVertices();
}

///////////////////////////////////////////////////////////////////////////////
void UCylinderPrimitive::SetSectorCount(SizeT sectorCount)
{
    if (m_sectorCount != sectorCount)
    {
        m_sectorCount = sectorCount;
        GenerateVertices();
    }
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD SizeT UCylinderPrimitive::GetSectorCount(void) const
{
    return m_sectorCount;
}

///////////////////////////////////////////////////////////////////////////////
void UCylinderPrimitive::SetHeight(float height)
{
    if (m_height != height)
    {
        m_height = height;
        GenerateVertices();
    }
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD float UCylinderPrimitive::GetHeight(void) const
{
    return m_height;
}

///////////////////////////////////////////////////////////////////////////////
void UCylinderPrimitive::SetCapTop(bool cap)
{
    if (m_capTop != cap)
    {
        m_capTop = cap;
        GenerateVertices();
    }
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD bool UCylinderPrimitive::IsCapTop(void) const
{
    return m_capTop;
}

///////////////////////////////////////////////////////////////////////////////
void UCylinderPrimitive::SetCapBottom(bool cap)
{
    if (m_capBottom != cap)
    {
        m_capBottom = cap;
        GenerateVertices();
    }
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD bool UCylinderPrimitive::IsCapBottom(void) const
{
    return m_capBottom;
}

///////////////////////////////////////////////////////////////////////////////
void UCylinderPrimitive::Draw(IRenderer& renderer, FRenderStates states) const
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
void UCylinderPrimitive::GenerateVertices(void)
{
    m_vertices.clear();
    m_strips.clear();

    const float halfHeight = m_height * 0.5f;

    // Generate side surface
    std::vector<FVertex> sideStrip;
    for (SizeT i = 0; i <= m_sectorCount; i++)
    {
        float angle = i * 2.0f * M_PI / m_sectorCount;
        float x = std::cos(angle);
        float z = std::sin(angle);

        // Bottom vertex
        sideStrip.push_back(FVertex(FVector3(x, -halfHeight, z), m_color));
        // Top vertex
        sideStrip.push_back(FVertex(FVector3(x, halfHeight, z), m_color));
    }
    m_strips.push_back(sideStrip);

    // Generate top cap if needed
    if (m_capTop)
    {
        std::vector<FVertex> topCap;
        topCap.push_back(FVertex(FVector3(0.0f, halfHeight, 0.0f), m_color));
        for (SizeT i = 0; i <= m_sectorCount; i++)
        {
            float angle = i * 2.0f * M_PI / m_sectorCount;
            float x = std::cos(angle);
            float z = std::sin(angle);
            topCap.push_back(FVertex(FVector3(x, halfHeight, z), m_color));
        }
        m_strips.push_back(topCap);
    }

    // Generate bottom cap if needed
    if (m_capBottom)
    {
        std::vector<FVertex> bottomCap;
        bottomCap.push_back(FVertex(FVector3(0.0f, -halfHeight, 0.0f), m_color)
        );
        for (SizeT i = 0; i <= m_sectorCount; i++)
        {
            float angle = -i * 2.0f * M_PI / m_sectorCount;
            float x = std::cos(angle);
            float z = std::sin(angle);
            bottomCap.push_back(FVertex(FVector3(x, -halfHeight, z), m_color));
        }
        m_strips.push_back(bottomCap);
    }
}

///////////////////////////////////////////////////////////////////////////////
void UCylinderPrimitive::SetColor(const FColor& color)
{
    m_color = color;
    for (auto& strip: m_strips)
    {
        for (FVertex& vertex: strip) { vertex.color = color; }
    }
}

}   // namespace tkd
