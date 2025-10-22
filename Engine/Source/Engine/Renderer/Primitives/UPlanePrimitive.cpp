///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Renderer/Primitives/UPlanePrimitive.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
UPlanePrimitive::UPlanePrimitive(bool bottomFaceCulled, const FColor& color)
    : UPrimitive(EPrimitiveType::Quads, color)
    , m_bottomFaceCulled(bottomFaceCulled)
{
    GenerateVertices();
}

///////////////////////////////////////////////////////////////////////////////
void UPlanePrimitive::SetBottomFaceCulled(bool culled)
{
    if (m_bottomFaceCulled != culled)
    {
        m_bottomFaceCulled = culled;
        GenerateVertices();
    }
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD bool UPlanePrimitive::IsBottomFaceCulled(void) const
{
    return m_bottomFaceCulled;
}

///////////////////////////////////////////////////////////////////////////////
void UPlanePrimitive::GenerateVertices(void)
{
    m_vertices.clear();
    m_vertices.push_back(FVertex(FVector3(-0.5f, 0.0f, 0.5f), m_color));
    m_vertices.push_back(FVertex(FVector3(0.5f, 0.0f, 0.5f), m_color));
    m_vertices.push_back(FVertex(FVector3(0.5f, 0.0f, -0.5f), m_color));
    m_vertices.push_back(FVertex(FVector3(-0.5f, 0.0f, -0.5f), m_color));

    if (!m_bottomFaceCulled)
    {
        m_vertices.push_back(FVertex(FVector3(-0.5f, 0.0f, -0.5f), m_color));
        m_vertices.push_back(FVertex(FVector3(0.5f, 0.0f, -0.5f), m_color));
        m_vertices.push_back(FVertex(FVector3(0.5f, 0.0f, 0.5f), m_color));
        m_vertices.push_back(FVertex(FVector3(-0.5f, 0.0f, 0.5f), m_color));
    }
}

}   // namespace tkd
