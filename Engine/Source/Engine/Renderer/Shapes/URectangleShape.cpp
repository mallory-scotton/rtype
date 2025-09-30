///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Renderer/Shapes/URectangleShape.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
URectangleShape::URectangleShape(void)
    : UShape()
    , m_size(FVector2f::One)
{
    m_needsUpdate = true;
}

///////////////////////////////////////////////////////////////////////////////
URectangleShape::URectangleShape(const FVector2f& size)
    : UShape()
    , m_size(size)
{
    m_needsUpdate = true;
}

///////////////////////////////////////////////////////////////////////////////
void URectangleShape::SetSize(const FVector2f& size)
{
    m_size = size;
    m_needsUpdate = true;
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD const FVector2f& URectangleShape::GetSize(void) const
{
    return m_size;
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD FRectangle URectangleShape::GetLocalBounds(void) const
{
    return FRectangle::FromCenter(FVector2f::Zero, m_size);
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD FRectangle URectangleShape::GetGlobalBounds(void) const
{
    // TODO: Use real transform operation here
    // return m_transform.TransformRect(GetLocalBounds());
    return GetLocalBounds();
}

///////////////////////////////////////////////////////////////////////////////
void URectangleShape::Draw(IRenderer* renderer, const FRenderStates& states)
    const
{
    if (m_needsUpdate) { UpdateGeometry(); }

    FRenderStates finalStates = states;
    if (m_blendMode != EBlendMode::None)
    {
        finalStates.blendMode = m_blendMode;
    }
    if (m_texture != nullptr) { finalStates.texture = m_texture; }
    finalStates.transform = GetOriginTransform() * states.transform;

    renderer->Draw(
        m_vertices.data(),
        m_vertices.size(),
        EPrimitiveType::Quads,
        finalStates
    );
}

///////////////////////////////////////////////////////////////////////////////
void URectangleShape::UpdateGeometry(void) const
{
    m_vertices.clear();

    if (m_size.x == 0.f || m_size.y == 0.f) { return; }
    if (m_outlineThickness > 0.f)
    {
        m_vertices.resize(8);

        // Define the four corners of the outer rectangle
        FVector2f halfSize = m_size * 0.5f;
        FVector2f halfOutline = halfSize + FVector2f(m_outlineThickness);
        m_vertices[0] = FVertex2D(
            FVector2f(-halfOutline.x, -halfOutline.y),
            m_outlineColor,
            FVector2f(0.0f, 0.0f)
        );   // Top-left outer

        m_vertices[1] = FVertex2D(
            FVector2f(halfOutline.x, -halfOutline.y),
            m_outlineColor,
            FVector2f(1.0f, 0.0f)
        );   // Top-right outer

        m_vertices[2] = FVertex2D(
            FVector2f(halfOutline.x, halfOutline.y),
            m_outlineColor,
            FVector2f(1.0f, 1.0f)
        );   // Bottom-right outer

        m_vertices[3] = FVertex2D(
            FVector2f(-halfOutline.x, halfOutline.y),
            m_outlineColor,
            FVector2f(0.0f, 1.0f)
        );   // Bottom-left outer

        // Define the four corners of the inner rectangle
        m_vertices[4] = FVertex2D(
            FVector2f(-halfSize.x, -halfSize.y),
            m_fillColor,
            FVector2f(0.0f, 0.0f)
        );   // Top-left inner

        m_vertices[5] = FVertex2D(
            FVector2f(halfSize.x, -halfSize.y),
            m_fillColor,
            FVector2f(1.0f, 0.0f)
        );   // Top-right inner

        m_vertices[6] = FVertex2D(
            FVector2f(halfSize.x, halfSize.y),
            m_fillColor,
            FVector2f(1.0f, 1.0f)
        );   // Bottom-right inner

        m_vertices[7] = FVertex2D(
            FVector2f(-halfSize.x, halfSize.y),
            m_fillColor,
            FVector2f(0.0f, 1.0f)
        );   // Bottom-left inner
    }
    else
    {
        m_vertices.resize(4);

        // Define the four corners of the rectangle
        FVector2f halfSize = m_size * 0.5f;
        m_vertices[0] = FVertex2D(
            FVector2f(-halfSize.x, -halfSize.y),
            m_fillColor,
            FVector2f(0.0f, 0.0f)
        );   // Top-left
        m_vertices[1] = FVertex2D(
            FVector2f(halfSize.x, -halfSize.y),
            m_fillColor,
            FVector2f(1.0f, 0.0f)
        );   // Top-right
        m_vertices[2] = FVertex2D(
            FVector2f(halfSize.x, halfSize.y),
            m_fillColor,
            FVector2f(1.0f, 1.0f)
        );   // Bottom-right
        m_vertices[3] = FVertex2D(
            FVector2f(-halfSize.x, halfSize.y),
            m_fillColor,
            FVector2f(0.0f, 1.0f)
        );   // Bottom-left
    }

    m_needsUpdate = false;
}

}   // namespace tkd
