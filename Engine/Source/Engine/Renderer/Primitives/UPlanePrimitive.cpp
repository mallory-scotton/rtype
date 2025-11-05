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
    UpdateVertices();
}

///////////////////////////////////////////////////////////////////////////////
void UPlanePrimitive::SetBottomFaceCulled(bool culled)
{
    if (m_bottomFaceCulled != culled)
    {
        m_bottomFaceCulled = culled;
        UpdateVertices();
    }
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD bool UPlanePrimitive::IsBottomFaceCulled(void) const
{
    return m_bottomFaceCulled;
}

///////////////////////////////////////////////////////////////////////////////
void UPlanePrimitive::UpdateVertices(void)
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

    // Apply the origin offset to center the billboard
    for (auto& vertex: m_vertices) { vertex.position -= m_origin; }
}

}   // namespace tkd
