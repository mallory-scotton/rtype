///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Renderer/Primitives/UBillboardPrimitive.hpp>
#include <Engine/Core/Math.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
UBillboardPrimitive::UBillboardPrimitive(
    ITexture* texture, const FColor& color
)
    : UPrimitive(EPrimitiveType::Quads, color)
    , m_texture(texture)
{
    // Define the four corners of the billboard quad
    m_vertices.push_back(
        FVertex(FVector3(-0.5f, -0.5f, 0.0f), color, FVector2(0.0f, 1.0f))
    );   // Bottom-left
    m_vertices.push_back(
        FVertex(FVector3(0.5f, -0.5f, 0.0f), color, FVector2(1.0f, 1.0f))
    );   // Bottom-right
    m_vertices.push_back(
        FVertex(FVector3(0.5f, 0.5f, 0.0f), color, FVector2(1.0f, 0.0f))
    );   // Top-right
    m_vertices.push_back(
        FVertex(FVector3(-0.5f, 0.5f, 0.0f), color, FVector2(0.0f, 0.0f))
    );   // Top-left
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
    if (m_texture) { m_texture->Bind(); }

    // Draw the billboard quad
    renderer.Draw(m_vertices, m_primitiveType, GetTransform());

    // Unbind the texture
    if (m_texture) { m_texture->Unbind(); }
}

///////////////////////////////////////////////////////////////////////////////
void UBillboardPrimitive::SetTexture(ITexture* texture)
{
    m_texture = texture;
}

}   // namespace tkd
