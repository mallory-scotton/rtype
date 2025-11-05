///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Renderer/Primitives/UCubePrimitive.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
UCubePrimitive::UCubePrimitive(const FColor& color)
    : UPrimitive(EPrimitiveType::Quads, color)
    , m_texture(nullptr)
{
    UpdateVertices();
}

///////////////////////////////////////////////////////////////////////////////
void UCubePrimitive::UpdateVertices()
{
    m_vertices.clear();

    // FRONT FACE
    m_vertices.push_back(
        FVertex(FVector3(-0.5f, -0.5f, 0.5f), m_color, FVector2(0.0f, 1.0f))
    );
    m_vertices.push_back(
        FVertex(FVector3(0.5f, -0.5f, 0.5f), m_color, FVector2(1.0f, 1.0f))
    );
    m_vertices.push_back(
        FVertex(FVector3(0.5f, 0.5f, 0.5f), m_color, FVector2(1.0f, 0.0f))
    );
    m_vertices.push_back(
        FVertex(FVector3(-0.5f, 0.5f, 0.5f), m_color, FVector2(0.0f, 0.0f))
    );

    // BACK FACE
    m_vertices.push_back(
        FVertex(FVector3(-0.5f, -0.5f, -0.5f), m_color, FVector2(1.0f, 1.0f))
    );
    m_vertices.push_back(
        FVertex(FVector3(-0.5f, 0.5f, -0.5f), m_color, FVector2(1.0f, 0.0f))
    );
    m_vertices.push_back(
        FVertex(FVector3(0.5f, 0.5f, -0.5f), m_color, FVector2(0.0f, 0.0f))
    );
    m_vertices.push_back(
        FVertex(FVector3(0.5f, -0.5f, -0.5f), m_color, FVector2(0.0f, 1.0f))
    );

    // TOP FACE
    m_vertices.push_back(
        FVertex(FVector3(-0.5f, 0.5f, -0.5f), m_color, FVector2(0.0f, 1.0f))
    );
    m_vertices.push_back(
        FVertex(FVector3(-0.5f, 0.5f, 0.5f), m_color, FVector2(0.0f, 0.0f))
    );
    m_vertices.push_back(
        FVertex(FVector3(0.5f, 0.5f, 0.5f), m_color, FVector2(1.0f, 0.0f))
    );
    m_vertices.push_back(
        FVertex(FVector3(0.5f, 0.5f, -0.5f), m_color, FVector2(1.0f, 1.0f))
    );

    // BOTTOM FACE
    m_vertices.push_back(
        FVertex(FVector3(-0.5f, -0.5f, -0.5f), m_color, FVector2(0.0f, 0.0f))
    );
    m_vertices.push_back(
        FVertex(FVector3(0.5f, -0.5f, -0.5f), m_color, FVector2(1.0f, 0.0f))
    );
    m_vertices.push_back(
        FVertex(FVector3(0.5f, -0.5f, 0.5f), m_color, FVector2(1.0f, 1.0f))
    );
    m_vertices.push_back(
        FVertex(FVector3(-0.5f, -0.5f, 0.5f), m_color, FVector2(0.0f, 1.0f))
    );

    // RIGHT FACE
    m_vertices.push_back(
        FVertex(FVector3(0.5f, -0.5f, -0.5f), m_color, FVector2(1.0f, 1.0f))
    );
    m_vertices.push_back(
        FVertex(FVector3(0.5f, 0.5f, -0.5f), m_color, FVector2(1.0f, 0.0f))
    );
    m_vertices.push_back(
        FVertex(FVector3(0.5f, 0.5f, 0.5f), m_color, FVector2(0.0f, 0.0f))
    );
    m_vertices.push_back(
        FVertex(FVector3(0.5f, -0.5f, 0.5f), m_color, FVector2(0.0f, 1.0f))
    );

    // LEFT FACE
    m_vertices.push_back(
        FVertex(FVector3(-0.5f, -0.5f, -0.5f), m_color, FVector2(0.0f, 1.0f))
    );
    m_vertices.push_back(
        FVertex(FVector3(-0.5f, -0.5f, 0.5f), m_color, FVector2(1.0f, 1.0f))
    );
    m_vertices.push_back(
        FVertex(FVector3(-0.5f, 0.5f, 0.5f), m_color, FVector2(1.0f, 0.0f))
    );
    m_vertices.push_back(
        FVertex(FVector3(-0.5f, 0.5f, -0.5f), m_color, FVector2(0.0f, 0.0f))
    );

    // Apply the origin offset to center the billboard
    for (auto& vertex: m_vertices) { vertex.position -= m_origin; }
}

///////////////////////////////////////////////////////////////////////////////
void UCubePrimitive::Draw(IRenderer& renderer, FRenderStates states) const
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
void UCubePrimitive::SetTexture(ITexture* texture) { m_texture = texture; }

///////////////////////////////////////////////////////////////////////////////
ITexture* UCubePrimitive::GetTexture(void) const { return m_texture; }

}   // namespace tkd
