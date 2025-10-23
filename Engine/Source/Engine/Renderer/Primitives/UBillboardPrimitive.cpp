///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Renderer/Primitives/UBillboardPrimitive.hpp>
#include <Engine/Core/Math.hpp>
#if TKD_ENGINE_CLIENT
    #include <GL/glu.h>
    #include <SFML/OpenGL.hpp>
#endif

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
UBillboardPrimitive::UBillboardPrimitive(
    ITexture* texture, const FRectanglei& rect, const FColor& color
)
    : UPrimitive(EPrimitiveType::Quads, color)
    , m_texture(texture)
    , m_textureRect(rect)
{
    UpdateVertices();
}

///////////////////////////////////////////////////////////////////////////////
void UBillboardPrimitive::UpdateVertices()
{
    m_vertices.clear();

    // Determine the size based on texture rect or default
    float width = 1.0f;
    float height = 1.0f;

    if (m_texture && !m_textureRect.IsEmpty())
    {
        // Use the texture rect dimensions to maintain aspect ratio
        width = static_cast<float>(m_textureRect.width);
        height = static_cast<float>(m_textureRect.height);

        // Normalize to a reasonable size (e.g., 1 unit = 32 pixels)
        width /= 32.0f;
        height /= 32.0f;
    }
    else if (m_texture)
    {
        // Use full texture dimensions
        width = static_cast<float>(m_texture->GetWidth());
        height = static_cast<float>(m_texture->GetHeight());
        width /= 32.0f;
        height /= 32.0f;
    }

    float halfWidth = width * 0.5f;
    float halfHeight = height * 0.5f;

    // Calculate UV coordinates based on texture rect
    float u0 = 0.0f, v0 = 0.0f, u1 = 1.0f, v1 = 1.0f;

    if (m_texture && !m_textureRect.IsEmpty())
    {
        float texWidth = static_cast<float>(m_texture->GetWidth());
        float texHeight = static_cast<float>(m_texture->GetHeight());

        u0 = static_cast<float>(m_textureRect.left) / texWidth;
        v0 = static_cast<float>(m_textureRect.top) / texHeight;
        u1 = static_cast<float>(m_textureRect.left + m_textureRect.width) /
             texWidth;
        v1 = static_cast<float>(m_textureRect.top + m_textureRect.height) /
             texHeight;
    }

    // Define the four corners of the billboard quad with proper UVs
    m_vertices.push_back(FVertex(
        FVector3(-halfWidth, -halfHeight, 0.0f), m_color, FVector2(u0, v1)
    ));   // Bottom-left
    m_vertices.push_back(FVertex(
        FVector3(halfWidth, -halfHeight, 0.0f), m_color, FVector2(u1, v1)
    ));   // Bottom-right
    m_vertices.push_back(FVertex(
        FVector3(halfWidth, halfHeight, 0.0f), m_color, FVector2(u1, v0)
    ));   // Top-right
    m_vertices.push_back(FVertex(
        FVector3(-halfWidth, halfHeight, 0.0f), m_color, FVector2(u0, v0)
    ));   // Top-left
}

///////////////////////////////////////////////////////////////////////////////
void UBillboardPrimitive::Draw(IRenderer& renderer, FRenderStates states) const
{
    // Suppress unused variable warning
    TKD_UNUSED(states);

    // Get the camera from the renderer
    FCamera camera = renderer.GetCamera();

    // TODO: Align the billboard to face the camera

    // Bind the texture
    if (m_texture)
    {
#if TKD_ENGINE_CLIENT
        // Enable alpha blending for transparent backgrounds
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#endif
        m_texture->Bind();
        renderer.Draw(m_vertices, m_primitiveType, GetTransform());
        m_texture->Unbind();
#if TKD_ENGINE_CLIENT
        glDisable(GL_BLEND);
#endif
    }
    else { renderer.Draw(m_vertices, m_primitiveType, GetTransform()); }
}

///////////////////////////////////////////////////////////////////////////////
void UBillboardPrimitive::SetTexture(ITexture* texture)
{
    m_texture = texture;
}

///////////////////////////////////////////////////////////////////////////////
void UBillboardPrimitive::SetTextureRect(const FRectanglei& rect)
{
    m_textureRect = rect;
    UpdateVertices();
}

}   // namespace tkd
