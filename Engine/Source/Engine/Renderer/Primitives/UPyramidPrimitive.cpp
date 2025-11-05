///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Renderer/Primitives/UPyramidPrimitive.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
UPyramidPrimitive::UPyramidPrimitive(
    float height, bool capBase, const FColor& color
)
    : UPrimitive(EPrimitiveType::Triangles, color)
    , m_height(height)
    , m_capBase(capBase)
{
    UpdateVertices();
}

///////////////////////////////////////////////////////////////////////////////
void UPyramidPrimitive::SetHeight(float height)
{
    if (m_height != height)
    {
        m_height = height;
        UpdateVertices();
    }
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD float UPyramidPrimitive::GetHeight(void) const
{
    return m_height;
}

///////////////////////////////////////////////////////////////////////////////
void UPyramidPrimitive::SetCapBase(bool cap)
{
    if (m_capBase != cap)
    {
        m_capBase = cap;
        UpdateVertices();
    }
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD bool UPyramidPrimitive::IsCapBase(void) const
{
    return m_capBase;
}

///////////////////////////////////////////////////////////////////////////////
void UPyramidPrimitive::UpdateVertices(void)
{
    m_vertices.clear();

    const float halfHeight = m_height * 0.5f;

    // Define the 5 vertices of the pyramid
    FVector3 apex(0.0f, halfHeight, 0.0f);
    FVector3 base1(-0.5f, -halfHeight, -0.5f);
    FVector3 base2(0.5f, -halfHeight, -0.5f);
    FVector3 base3(0.5f, -halfHeight, 0.5f);
    FVector3 base4(-0.5f, -halfHeight, 0.5f);

    // Front face
    m_vertices.push_back(FVertex(apex, m_color));
    m_vertices.push_back(FVertex(base3, m_color));
    m_vertices.push_back(FVertex(base2, m_color));

    // Right face
    m_vertices.push_back(FVertex(apex, m_color));
    m_vertices.push_back(FVertex(base2, m_color));
    m_vertices.push_back(FVertex(base1, m_color));

    // Back face
    m_vertices.push_back(FVertex(apex, m_color));
    m_vertices.push_back(FVertex(base1, m_color));
    m_vertices.push_back(FVertex(base4, m_color));

    // Left face
    m_vertices.push_back(FVertex(apex, m_color));
    m_vertices.push_back(FVertex(base4, m_color));
    m_vertices.push_back(FVertex(base3, m_color));

    // Base (if capped)
    if (m_capBase)
    {
        // Base triangle 1
        m_vertices.push_back(FVertex(base1, m_color));
        m_vertices.push_back(FVertex(base2, m_color));
        m_vertices.push_back(FVertex(base3, m_color));

        // Base triangle 2
        m_vertices.push_back(FVertex(base1, m_color));
        m_vertices.push_back(FVertex(base3, m_color));
        m_vertices.push_back(FVertex(base4, m_color));
    }

    // Apply the origin offset to center the billboard
    for (auto& vertex: m_vertices) { vertex.position -= m_origin; }
}

}   // namespace tkd
