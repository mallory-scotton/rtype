///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Renderer/Primitives/USpherePrimitive.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
USpherePrimitive::USpherePrimitive(
    SizeT sectorCount, SizeT stackCount, const FColor& color
)
    : UPrimitive(EPrimitiveType::QuadStrip, color)
    , m_sectorCount(sectorCount)
    , m_stackCount(stackCount)
{
    GenerateVertices();
}

///////////////////////////////////////////////////////////////////////////////
void USpherePrimitive::SetSectorCount(SizeT sectorCount)
{
    if (m_sectorCount != sectorCount)
    {
        m_sectorCount = sectorCount;
        GenerateVertices();
    }
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD SizeT USpherePrimitive::GetSectorCount(void) const
{
    return m_sectorCount;
}

///////////////////////////////////////////////////////////////////////////////
void USpherePrimitive::SetStackCount(SizeT stackCount)
{
    if (m_stackCount != stackCount)
    {
        m_stackCount = stackCount;
        GenerateVertices();
    }
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD SizeT USpherePrimitive::GetStackCount(void) const
{
    return m_stackCount;
}

///////////////////////////////////////////////////////////////////////////////
void USpherePrimitive::Draw(IRenderer& renderer, FRenderStates states) const
{
    // State parameter is currently unused
    TKD_UNUSED(states);

    // Draw the vertices using the renderer
    for (const auto& stack: m_stacks)
    {
        renderer.Draw(stack, m_primitiveType, GetTransform());
    }
}

///////////////////////////////////////////////////////////////////////////////
void USpherePrimitive::GenerateVertices(void)
{
    m_vertices.clear();

    for (SizeT lat = 0; lat < m_stackCount; lat++)
    {
        std::vector<FVertex> stack;

        for (SizeT lon = 0; lon <= m_sectorCount; lon++)
        {
            for (int k = 0; k < 2; k++)
            {
                float theta = (lat + k) * M_PI / m_stackCount;
                float phi = lon * 2.0f * M_PI / m_sectorCount;

                float x = std::cos(phi) * std::sin(theta);
                float y = std::cos(theta);
                float z = std::sin(phi) * std::sin(theta);

                stack.push_back(FVertex(FVector3(x, y, z), m_color));
            }
        }

        m_stacks.push_back(stack);
    }
}

///////////////////////////////////////////////////////////////////////////////
void USpherePrimitive::SetColor(const FColor& color)
{
    m_color = color;
    for (auto& stack: m_stacks)
    {
        for (FVertex& vertex: stack) { vertex.color = color; }
    }
}

}   // namespace tkd
