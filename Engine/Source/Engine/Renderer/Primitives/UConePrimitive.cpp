///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Renderer/Primitives/UConePrimitive.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
UConePrimitive::UConePrimitive(
    SizeT sectorCount, float height, bool capBase, const FColor& color
)
    : UPrimitive(EPrimitiveType::TriangleStrip, color)
    , m_sectorCount(sectorCount)
    , m_height(height)
    , m_capBase(capBase)
{
    GenerateVertices();
}

///////////////////////////////////////////////////////////////////////////////
void UConePrimitive::SetSectorCount(SizeT sectorCount)
{
    if (m_sectorCount != sectorCount)
    {
        m_sectorCount = sectorCount;
        GenerateVertices();
    }
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD SizeT UConePrimitive::GetSectorCount(void) const
{
    return m_sectorCount;
}

///////////////////////////////////////////////////////////////////////////////
void UConePrimitive::SetHeight(float height)
{
    if (m_height != height)
    {
        m_height = height;
        GenerateVertices();
    }
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD float UConePrimitive::GetHeight(void) const { return m_height; }

///////////////////////////////////////////////////////////////////////////////
void UConePrimitive::SetCapBase(bool cap)
{
    if (m_capBase != cap)
    {
        m_capBase = cap;
        GenerateVertices();
    }
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD bool UConePrimitive::IsCapBase(void) const { return m_capBase; }

///////////////////////////////////////////////////////////////////////////////
void UConePrimitive::Draw(IRenderer& renderer, FRenderStates states) const
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
void UConePrimitive::GenerateVertices(void)
{
    m_vertices.clear();
    m_strips.clear();

    const float halfHeight = m_height * 0.5f;

    // Generate side surface
    std::vector<FVertex> sideStrip;
    FVector3 apex(0.0f, halfHeight, 0.0f);

    for (SizeT i = 0; i <= m_sectorCount; i++)
    {
        float angle = i * 2.0f * M_PI / m_sectorCount;
        float x = std::cos(angle);
        float z = std::sin(angle);

        // Apex vertex
        sideStrip.push_back(FVertex(apex, m_color));
        // Base vertex
        sideStrip.push_back(FVertex(FVector3(x, -halfHeight, z), m_color));
    }
    m_strips.push_back(sideStrip);

    // Generate base cap if needed
    if (m_capBase)
    {
        std::vector<FVertex> baseCap;
        baseCap.push_back(FVertex(FVector3(0.0f, -halfHeight, 0.0f), m_color));
        for (SizeT i = 0; i <= m_sectorCount; i++)
        {
            float angle = -i * 2.0f * M_PI / m_sectorCount;
            float x = std::cos(angle);
            float z = std::sin(angle);
            baseCap.push_back(FVertex(FVector3(x, -halfHeight, z), m_color));
        }
        m_strips.push_back(baseCap);
    }
}

///////////////////////////////////////////////////////////////////////////////
void UConePrimitive::SetColor(const FColor& color)
{
    m_color = color;
    for (auto& strip: m_strips)
    {
        for (FVertex& vertex: strip) { vertex.color = color; }
    }
}

}   // namespace tkd
