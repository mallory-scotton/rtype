///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Renderer/Primitives/UCapsulePrimitive.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
UCapsulePrimitive::UCapsulePrimitive(
    SizeT sectorCount,
    SizeT stackCount,
    float cylinderHeight,
    float radius,
    const FColor& color
)
    : UPrimitive(EPrimitiveType::QuadStrip, color)
    , m_sectorCount(sectorCount)
    , m_stackCount(stackCount)
    , m_cylinderHeight(cylinderHeight)
    , m_radius(radius)
{
    GenerateVertices();
}

///////////////////////////////////////////////////////////////////////////////
void UCapsulePrimitive::SetSectorCount(SizeT sectorCount)
{
    if (m_sectorCount != sectorCount)
    {
        m_sectorCount = sectorCount;
        GenerateVertices();
    }
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD SizeT UCapsulePrimitive::GetSectorCount(void) const
{
    return m_sectorCount;
}

///////////////////////////////////////////////////////////////////////////////
void UCapsulePrimitive::SetStackCount(SizeT stackCount)
{
    if (m_stackCount != stackCount)
    {
        m_stackCount = stackCount;
        GenerateVertices();
    }
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD SizeT UCapsulePrimitive::GetStackCount(void) const
{
    return m_stackCount;
}

///////////////////////////////////////////////////////////////////////////////
void UCapsulePrimitive::SetCylinderHeight(float height)
{
    if (m_cylinderHeight != height)
    {
        m_cylinderHeight = height;
        GenerateVertices();
    }
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD float UCapsulePrimitive::GetCylinderHeight(void) const
{
    return m_cylinderHeight;
}

///////////////////////////////////////////////////////////////////////////////
void UCapsulePrimitive::SetRadius(float radius)
{
    if (m_radius != radius)
    {
        m_radius = radius;
        GenerateVertices();
    }
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD float UCapsulePrimitive::GetRadius(void) const
{
    return m_radius;
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD float UCapsulePrimitive::GetTotalHeight(void) const
{
    return m_cylinderHeight + 2.0f * m_radius;
}

///////////////////////////////////////////////////////////////////////////////
void UCapsulePrimitive::Draw(IRenderer& renderer, FRenderStates states) const
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
void UCapsulePrimitive::GenerateVertices(void)
{
    m_vertices.clear();
    m_strips.clear();

    const float halfCylinderHeight = m_cylinderHeight * 0.5f;

    // Generate top hemisphere
    for (SizeT lat = 0; lat < m_stackCount / 2; lat++)
    {
        std::vector<FVertex> strip;

        for (SizeT lon = 0; lon <= m_sectorCount; lon++)
        {
            for (int k = 0; k < 2; k++)
            {
                float theta = (lat + k) * M_PI / m_stackCount;
                float phi = lon * 2.0f * M_PI / m_sectorCount;

                float x = m_radius * std::cos(phi) * std::sin(theta);
                float y = m_radius * std::cos(theta) + halfCylinderHeight;
                float z = m_radius * std::sin(phi) * std::sin(theta);

                strip.push_back(FVertex(FVector3(x, y, z), m_color));
            }
        }

        m_strips.push_back(strip);
    }

    // Generate cylindrical middle section
    std::vector<FVertex> cylinderStrip;
    for (SizeT lon = 0; lon <= m_sectorCount; lon++)
    {
        float phi = lon * 2.0f * M_PI / m_sectorCount;
        float x = m_radius * std::cos(phi);
        float z = m_radius * std::sin(phi);

        cylinderStrip.push_back(
            FVertex(FVector3(x, halfCylinderHeight, z), m_color)
        );
        cylinderStrip.push_back(
            FVertex(FVector3(x, -halfCylinderHeight, z), m_color)
        );
    }
    m_strips.push_back(cylinderStrip);

    // Generate bottom hemisphere
    for (SizeT lat = m_stackCount / 2; lat < m_stackCount; lat++)
    {
        std::vector<FVertex> strip;

        for (SizeT lon = 0; lon <= m_sectorCount; lon++)
        {
            for (int k = 0; k < 2; k++)
            {
                float theta = (lat + k) * M_PI / m_stackCount;
                float phi = lon * 2.0f * M_PI / m_sectorCount;

                float x = m_radius * std::cos(phi) * std::sin(theta);
                float y = m_radius * std::cos(theta) - halfCylinderHeight;
                float z = m_radius * std::sin(phi) * std::sin(theta);

                strip.push_back(FVertex(FVector3(x, y, z), m_color));
            }
        }

        m_strips.push_back(strip);
    }
}

///////////////////////////////////////////////////////////////////////////////
void UCapsulePrimitive::SetColor(const FColor& color)
{
    m_color = color;
    for (auto& strip: m_strips)
    {
        for (FVertex& vertex: strip) { vertex.color = color; }
    }
}

}   // namespace tkd
