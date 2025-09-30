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
    // finalStates.transform = states.transform * m_transform;

    renderer->Draw(
        m_vertices.data(),
        m_vertices.size(),
        EPrimitiveType::Quads,
        finalStates
    );
}

}   // namespace tkd
