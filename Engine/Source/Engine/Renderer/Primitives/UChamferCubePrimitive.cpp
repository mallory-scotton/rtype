///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Renderer/Primitives/UChamferCubePrimitive.hpp>
#include <cmath>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
UChamferCubePrimitive::UChamferCubePrimitive(
    const FColor& color, Float32 chamferSize, SizeT segments
)
    : UPrimitive(EPrimitiveType::Triangles, color)
    , m_texture(nullptr)
    , m_chamferSize(chamferSize)
    , m_segments(segments)
{
    // Clamp chamfer size to valid range
    if (m_chamferSize < 0.0f) { m_chamferSize = 0.0f; }
    if (m_chamferSize > 0.5f) { m_chamferSize = 0.5f; }

    // Ensure at least 1 segment
    if (m_segments < 1) { m_segments = 1; }

    UpdateVertices();
}

///////////////////////////////////////////////////////////////////////////////
void UChamferCubePrimitive::UpdateVertices()
{
    m_vertices.clear();

    const Float32 c = m_chamferSize;   // Chamfer size
    const Float32 h = 0.5f;            // Half size of cube
    const Float32 ic = h - c;          // Inner corner position

    // Helper lambda to add a triangle
    auto addTriangle = [this](
                           const FVector3& v1,
                           const FVector3& v2,
                           const FVector3& v3,
                           const FVector2& uv1,
                           const FVector2& uv2,
                           const FVector2& uv3
                       )
    {
        m_vertices.push_back(FVertex(v1, m_color, uv1));
        m_vertices.push_back(FVertex(v2, m_color, uv2));
        m_vertices.push_back(FVertex(v3, m_color, uv3));
    };

    // Helper lambda to add a quad as two triangles
    auto addQuad = [&addTriangle](
                       const FVector3& v1,
                       const FVector3& v2,
                       const FVector3& v3,
                       const FVector3& v4,
                       const FVector2& uv1,
                       const FVector2& uv2,
                       const FVector2& uv3,
                       const FVector2& uv4
                   )
    {
        addTriangle(v1, v2, v3, uv1, uv2, uv3);
        addTriangle(v1, v3, v4, uv1, uv3, uv4);
    };

    // Helper lambda to create a smooth rounded corner
    auto addRoundedCorner = [this,
                             c](const FVector3& center,
                                const FVector3& axis1,
                                const FVector3& axis2,
                                const FVector3& axis3)
    {
        // Create a smooth corner using spherical interpolation
        for (SizeT i = 0; i < m_segments; ++i)
        {
            for (SizeT j = 0; j < m_segments; ++j)
            {
                // Calculate angles for spherical corner (0 to 90 degrees)
                Float32 theta1 = (i * M_PI / 2.0f) / m_segments;
                Float32 theta2 = ((i + 1) * M_PI / 2.0f) / m_segments;
                Float32 phi1 = (j * M_PI / 2.0f) / m_segments;
                Float32 phi2 = ((j + 1) * M_PI / 2.0f) / m_segments;

                // Calculate vertices on the sphere surface
                FVector3 v1 = center +
                              axis1 * (c * std::sin(theta1) * std::cos(phi1)) +
                              axis2 * (c * std::sin(theta1) * std::sin(phi1)) +
                              axis3 * (c * std::cos(theta1));

                FVector3 v2 = center +
                              axis1 * (c * std::sin(theta2) * std::cos(phi1)) +
                              axis2 * (c * std::sin(theta2) * std::sin(phi1)) +
                              axis3 * (c * std::cos(theta2));

                FVector3 v3 = center +
                              axis1 * (c * std::sin(theta2) * std::cos(phi2)) +
                              axis2 * (c * std::sin(theta2) * std::sin(phi2)) +
                              axis3 * (c * std::cos(theta2));

                FVector3 v4 = center +
                              axis1 * (c * std::sin(theta1) * std::cos(phi2)) +
                              axis2 * (c * std::sin(theta1) * std::sin(phi2)) +
                              axis3 * (c * std::cos(theta1));

                // Simple UV mapping for corners
                Float32 u1 = static_cast<Float32>(j) / m_segments;
                Float32 u2 = static_cast<Float32>(j + 1) / m_segments;
                Float32 v1_uv = static_cast<Float32>(i) / m_segments;
                Float32 v2_uv = static_cast<Float32>(i + 1) / m_segments;

                // Add two triangles to form a quad (counter-clockwise winding
                // for outward normal)
                m_vertices.push_back(
                    FVertex(v1, m_color, FVector2(u1, v1_uv))
                );
                m_vertices.push_back(
                    FVertex(v4, m_color, FVector2(u2, v1_uv))
                );
                m_vertices.push_back(
                    FVertex(v3, m_color, FVector2(u2, v2_uv))
                );

                m_vertices.push_back(
                    FVertex(v1, m_color, FVector2(u1, v1_uv))
                );
                m_vertices.push_back(
                    FVertex(v3, m_color, FVector2(u2, v2_uv))
                );
                m_vertices.push_back(
                    FVertex(v2, m_color, FVector2(u1, v2_uv))
                );
            }
        }
    };

    // Helper lambda to create a smooth rounded edge (cylindrical)
    auto addRoundedEdge = [this,
                           c](const FVector3& start,
                              const FVector3& end,
                              const FVector3& normalAxis1,
                              const FVector3& normalAxis2)
    {
        // Create a smooth cylindrical edge
        FVector3 edgeDir = (end - start).Normalized();

        for (SizeT i = 0; i < m_segments; ++i)
        {
            Float32 angle1 = (i * M_PI / 2.0f) / m_segments;
            Float32 angle2 = ((i + 1) * M_PI / 2.0f) / m_segments;

            // Calculate normal directions for the cylinder (normalized outward
            // direction)
            FVector3 normal1 = (normalAxis1 * std::cos(angle1) +
                                normalAxis2 * std::sin(angle1))
                                   .Normalized();
            FVector3 normal2 = (normalAxis1 * std::cos(angle2) +
                                normalAxis2 * std::sin(angle2))
                                   .Normalized();

            // Create quad strip along the edge
            FVector3 v1 = start + normal1 * c;
            FVector3 v2 = end + normal1 * c;
            FVector3 v3 = end + normal2 * c;
            FVector3 v4 = start + normal2 * c;

            // UV mapping
            Float32 u1 = static_cast<Float32>(i) / m_segments;
            Float32 u2 = static_cast<Float32>(i + 1) / m_segments;

            // Add triangles with proper winding for outward normals
            m_vertices.push_back(FVertex(v1, m_color, FVector2(0.0f, u1)));
            m_vertices.push_back(FVertex(v4, m_color, FVector2(0.0f, u2)));
            m_vertices.push_back(FVertex(v3, m_color, FVector2(1.0f, u2)));

            m_vertices.push_back(FVertex(v1, m_color, FVector2(0.0f, u1)));
            m_vertices.push_back(FVertex(v3, m_color, FVector2(1.0f, u2)));
            m_vertices.push_back(FVertex(v2, m_color, FVector2(1.0f, u1)));
        }
    };

    // =======================================================================
    // FRONT FACE (+Z)
    // =======================================================================
    // Center quad
    addQuad(
        FVector3(-ic, -ic, h),
        FVector3(ic, -ic, h),
        FVector3(ic, ic, h),
        FVector3(-ic, ic, h),
        FVector2(c, 1.0f - c),
        FVector2(1.0f - c, 1.0f - c),
        FVector2(1.0f - c, c),
        FVector2(c, c)
    );

    // =======================================================================
    // BACK FACE (-Z)
    // =======================================================================
    // Center quad
    addQuad(
        FVector3(-ic, -ic, -h),
        FVector3(-ic, ic, -h),
        FVector3(ic, ic, -h),
        FVector3(ic, -ic, -h),
        FVector2(c, 1.0f - c),
        FVector2(c, c),
        FVector2(1.0f - c, c),
        FVector2(1.0f - c, 1.0f - c)
    );

    // =======================================================================
    // TOP FACE (+Y)
    // =======================================================================
    // Center quad
    addQuad(
        FVector3(-ic, h, -ic),
        FVector3(-ic, h, ic),
        FVector3(ic, h, ic),
        FVector3(ic, h, -ic),
        FVector2(c, c),
        FVector2(c, 1.0f - c),
        FVector2(1.0f - c, 1.0f - c),
        FVector2(1.0f - c, c)
    );

    // =======================================================================
    // BOTTOM FACE (-Y)
    // =======================================================================
    // Center quad
    addQuad(
        FVector3(-ic, -h, -ic),
        FVector3(ic, -h, -ic),
        FVector3(ic, -h, ic),
        FVector3(-ic, -h, ic),
        FVector2(c, c),
        FVector2(1.0f - c, c),
        FVector2(1.0f - c, 1.0f - c),
        FVector2(c, 1.0f - c)
    );

    // =======================================================================
    // RIGHT FACE (+X)
    // =======================================================================
    // Center quad
    addQuad(
        FVector3(h, -ic, -ic),
        FVector3(h, ic, -ic),
        FVector3(h, ic, ic),
        FVector3(h, -ic, ic),
        FVector2(c, 1.0f - c),
        FVector2(c, c),
        FVector2(1.0f - c, c),
        FVector2(1.0f - c, 1.0f - c)
    );

    // =======================================================================
    // LEFT FACE (-X)
    // =======================================================================
    // Center quad
    addQuad(
        FVector3(-h, -ic, -ic),
        FVector3(-h, -ic, ic),
        FVector3(-h, ic, ic),
        FVector3(-h, ic, -ic),
        FVector2(c, 1.0f - c),
        FVector2(1.0f - c, 1.0f - c),
        FVector2(1.0f - c, c),
        FVector2(c, c)
    );

    // =======================================================================
    // ROUNDED EDGES (12 edges with cylindrical rounding)
    // =======================================================================

    // Front face edges
    addRoundedEdge(
        FVector3(-ic, -ic, ic),
        FVector3(ic, -ic, ic),
        FVector3(0, -1, 0),
        FVector3(0, 0, 1)
    );   // Bottom
    addRoundedEdge(
        FVector3(-ic, ic, ic),
        FVector3(ic, ic, ic),
        FVector3(0, 1, 0),
        FVector3(0, 0, 1)
    );   // Top
    addRoundedEdge(
        FVector3(-ic, -ic, ic),
        FVector3(-ic, ic, ic),
        FVector3(-1, 0, 0),
        FVector3(0, 0, 1)
    );   // Left
    addRoundedEdge(
        FVector3(ic, -ic, ic),
        FVector3(ic, ic, ic),
        FVector3(1, 0, 0),
        FVector3(0, 0, 1)
    );   // Right

    // Back face edges
    addRoundedEdge(
        FVector3(-ic, -ic, -ic),
        FVector3(ic, -ic, -ic),
        FVector3(0, -1, 0),
        FVector3(0, 0, -1)
    );   // Bottom
    addRoundedEdge(
        FVector3(-ic, ic, -ic),
        FVector3(ic, ic, -ic),
        FVector3(0, 1, 0),
        FVector3(0, 0, -1)
    );   // Top
    addRoundedEdge(
        FVector3(-ic, -ic, -ic),
        FVector3(-ic, ic, -ic),
        FVector3(-1, 0, 0),
        FVector3(0, 0, -1)
    );   // Left
    addRoundedEdge(
        FVector3(ic, -ic, -ic),
        FVector3(ic, ic, -ic),
        FVector3(1, 0, 0),
        FVector3(0, 0, -1)
    );   // Right

    // Vertical edges (connecting top and bottom)
    addRoundedEdge(
        FVector3(-ic, -ic, -ic),
        FVector3(-ic, -ic, ic),
        FVector3(-1, 0, 0),
        FVector3(0, -1, 0)
    );   // Front-left
    addRoundedEdge(
        FVector3(ic, -ic, -ic),
        FVector3(ic, -ic, ic),
        FVector3(1, 0, 0),
        FVector3(0, -1, 0)
    );   // Front-right
    addRoundedEdge(
        FVector3(-ic, ic, -ic),
        FVector3(-ic, ic, ic),
        FVector3(-1, 0, 0),
        FVector3(0, 1, 0)
    );   // Back-left
    addRoundedEdge(
        FVector3(ic, ic, -ic),
        FVector3(ic, ic, ic),
        FVector3(1, 0, 0),
        FVector3(0, 1, 0)
    );   // Back-right

    // =======================================================================
    // CORNER CHAMFERS (8 corners, each smoothly rounded)
    // =======================================================================

    // Corner: (+X, +Y, +Z) - Octant 1
    addRoundedCorner(
        FVector3(ic, ic, ic),
        FVector3(1, 0, 0),
        FVector3(0, 1, 0),
        FVector3(0, 0, 1)
    );

    // Corner: (-X, +Y, +Z) - Octant 2
    addRoundedCorner(
        FVector3(-ic, ic, ic),
        FVector3(-1, 0, 0),
        FVector3(0, 1, 0),
        FVector3(0, 0, 1)
    );

    // Corner: (+X, -Y, +Z) - Octant 4
    addRoundedCorner(
        FVector3(ic, -ic, ic),
        FVector3(1, 0, 0),
        FVector3(0, -1, 0),
        FVector3(0, 0, 1)
    );

    // Corner: (-X, -Y, +Z) - Octant 3
    addRoundedCorner(
        FVector3(-ic, -ic, ic),
        FVector3(-1, 0, 0),
        FVector3(0, -1, 0),
        FVector3(0, 0, 1)
    );

    // Corner: (+X, +Y, -Z) - Octant 5
    addRoundedCorner(
        FVector3(ic, ic, -ic),
        FVector3(1, 0, 0),
        FVector3(0, 1, 0),
        FVector3(0, 0, -1)
    );

    // Corner: (-X, +Y, -Z) - Octant 6
    addRoundedCorner(
        FVector3(-ic, ic, -ic),
        FVector3(-1, 0, 0),
        FVector3(0, 1, 0),
        FVector3(0, 0, -1)
    );

    // Corner: (+X, -Y, -Z) - Octant 8
    addRoundedCorner(
        FVector3(ic, -ic, -ic),
        FVector3(1, 0, 0),
        FVector3(0, -1, 0),
        FVector3(0, 0, -1)
    );

    // Corner: (-X, -Y, -Z) - Octant 7
    addRoundedCorner(
        FVector3(-ic, -ic, -ic),
        FVector3(-1, 0, 0),
        FVector3(0, -1, 0),
        FVector3(0, 0, -1)
    );

    // Apply the origin offset to center the chamfer cube
    for (auto& vertex: m_vertices) { vertex.position -= m_origin; }
}

///////////////////////////////////////////////////////////////////////////////
void UChamferCubePrimitive::Draw(
    IRenderer& renderer, FRenderStates states
) const
{
    // Suppress unused variable warning
    TKD_UNUSED(states);

    // Bind the texture if available
    if (m_texture)
    {
        renderer.SetBlend(true);
        renderer.SetBlendFunc(
            EBlendFactor::SrcAlpha, EBlendFactor::OneMinusSrcAlpha
        );
        m_texture->Bind();
        renderer.Draw(m_vertices, m_primitiveType, GetTransform());
        m_texture->Unbind();
        renderer.SetBlend(false);
    }
    else { renderer.Draw(m_vertices, m_primitiveType, GetTransform()); }
}

///////////////////////////////////////////////////////////////////////////////
void UChamferCubePrimitive::SetTexture(ITexture* texture)
{
    m_texture = texture;
}

///////////////////////////////////////////////////////////////////////////////
ITexture* UChamferCubePrimitive::GetTexture(void) const { return m_texture; }

///////////////////////////////////////////////////////////////////////////////
void UChamferCubePrimitive::SetChamferSize(Float32 size)
{
    // Clamp to valid range
    if (size < 0.0f) { size = 0.0f; }
    if (size > 0.5f) { size = 0.5f; }

    if (m_chamferSize != size)
    {
        m_chamferSize = size;
        UpdateVertices();
    }
}

///////////////////////////////////////////////////////////////////////////////
Float32 UChamferCubePrimitive::GetChamferSize(void) const
{
    return m_chamferSize;
}

///////////////////////////////////////////////////////////////////////////////
void UChamferCubePrimitive::SetSegments(SizeT segments)
{
    // Ensure at least 1 segment
    if (segments < 1) { segments = 1; }

    if (m_segments != segments)
    {
        m_segments = segments;
        UpdateVertices();
    }
}

///////////////////////////////////////////////////////////////////////////////
SizeT UChamferCubePrimitive::GetSegments(void) const { return m_segments; }

}   // namespace tkd
