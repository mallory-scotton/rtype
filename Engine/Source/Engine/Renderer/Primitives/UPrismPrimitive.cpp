///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Renderer/Primitives/UPrismPrimitive.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
UPrismPrimitive::UPrismPrimitive(
    float length, bool capEnds, const FColor& color
)
    : UPrimitive(EPrimitiveType::Quads, color)
    , m_length(length)
    , m_capEnds(capEnds)
{
    GenerateVertices();
}

///////////////////////////////////////////////////////////////////////////////
void UPrismPrimitive::SetLength(float length)
{
    if (m_length != length)
    {
        m_length = length;
        GenerateVertices();
    }
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD float UPrismPrimitive::GetLength(void) const { return m_length; }

///////////////////////////////////////////////////////////////////////////////
void UPrismPrimitive::SetCapEnds(bool cap)
{
    if (m_capEnds != cap)
    {
        m_capEnds = cap;
        GenerateVertices();
    }
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD bool UPrismPrimitive::AreEndsCapped(void) const
{
    return m_capEnds;
}

///////////////////////////////////////////////////////////////////////////////
void UPrismPrimitive::GenerateVertices(void)
{
    m_vertices.clear();

    const float halfLength = m_length * 0.5f;

    // Define the 6 vertices of the triangular prism
    // Front triangle
    FVector3 front1(-0.5f, -0.5f, halfLength);
    FVector3 front2(0.5f, -0.5f, halfLength);
    FVector3 front3(0.0f, 0.5f, halfLength);

    // Back triangle
    FVector3 back1(-0.5f, -0.5f, -halfLength);
    FVector3 back2(0.5f, -0.5f, -halfLength);
    FVector3 back3(0.0f, 0.5f, -halfLength);

    // Bottom face
    m_vertices.push_back(FVertex(front1, m_color));
    m_vertices.push_back(FVertex(front2, m_color));
    m_vertices.push_back(FVertex(back2, m_color));
    m_vertices.push_back(FVertex(back1, m_color));

    // Left face
    m_vertices.push_back(FVertex(front1, m_color));
    m_vertices.push_back(FVertex(back1, m_color));
    m_vertices.push_back(FVertex(back3, m_color));
    m_vertices.push_back(FVertex(front3, m_color));

    // Right face
    m_vertices.push_back(FVertex(front2, m_color));
    m_vertices.push_back(FVertex(front3, m_color));
    m_vertices.push_back(FVertex(back3, m_color));
    m_vertices.push_back(FVertex(back2, m_color));

    // Cap the triangular ends if needed
    if (m_capEnds)
    {
        // We need to use triangles for the end caps
        // Change primitive type temporarily or add triangle vertices
        // For simplicity, we'll add degenerate quads that form triangles

        // Front triangle cap (as degenerate quad)
        m_vertices.push_back(FVertex(front1, m_color));
        m_vertices.push_back(FVertex(front2, m_color));
        m_vertices.push_back(FVertex(front3, m_color));
        m_vertices.push_back(FVertex(front3, m_color));

        // Back triangle cap (as degenerate quad)
        m_vertices.push_back(FVertex(back1, m_color));
        m_vertices.push_back(FVertex(back3, m_color));
        m_vertices.push_back(FVertex(back2, m_color));
        m_vertices.push_back(FVertex(back2, m_color));
    }
}

}   // namespace tkd
