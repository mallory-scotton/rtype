///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Renderer/SFML/Renderer.hpp>
#include <Engine/Config.hpp>
#include <Engine/Renderer/SFML/Window.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::SFML
///////////////////////////////////////////////////////////////////////////////
namespace tkd::SFML
{

#if TKD_ENGINE_CLIENT

///////////////////////////////////////////////////////////////////////////////
Renderer::Renderer(IWindow* window)
    : m_window(reinterpret_cast<sf::RenderWindow*>(window->GetNativeHandle()))
    , m_currentTarget(m_window)
    , m_currentView(/*FView::GetDefaultView()*/)
{}

///////////////////////////////////////////////////////////////////////////////
void Renderer::Clear(const FColor& color)
{
    m_currentTarget->clear(ToSFMLColor(color));
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::SetView(const FView& view)
{
    m_currentView = view;
    m_currentTarget->setView(ToSFMLView(view));
}

///////////////////////////////////////////////////////////////////////////////
const FView& Renderer::GetView(void) const { return m_currentView; }

///////////////////////////////////////////////////////////////////////////////
FView Renderer::GetDefaultView(void) const
{
    sf::View sfmlView = m_currentTarget->getDefaultView();
    return FView(FRectangle(
        sfmlView.getCenter().x - sfmlView.getSize().x / 2.0f,
        sfmlView.getCenter().y - sfmlView.getSize().y / 2.0f,
        sfmlView.getSize().x,
        sfmlView.getSize().y
    ));
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::Draw(
    const FVertex2D* vertices,
    UInt32 vertexCount,
    EPrimitiveType type,
    const FRenderStates& states
)
{
    if (vertices == nullptr || vertexCount == 0) { return; }
    SetupSFMLStates(states);
    m_currentTarget->draw(
        reinterpret_cast<const sf::Vertex*>(vertices),
        vertexCount,
        ToSFMLPrimitiveType(type),
        m_cachedStates
    );
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::DrawIndexed(
    const FVertex2D* vertices,
    UInt32 vertexCount,
    const UInt32* indices,
    UInt32 indexCount,
    EPrimitiveType type,
    const FRenderStates& states
)
{
    TKD_UNUSED(vertices);
    TKD_UNUSED(vertexCount);
    TKD_UNUSED(indices);
    TKD_UNUSED(indexCount);
    TKD_UNUSED(type);
    TKD_UNUSED(states);
    // TODO: Implement indexed drawing with SFML
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::SetRenderTarget(IRenderTarget* target)
{
    if (target == nullptr) { m_currentTarget = m_window; }
    else
    {
        m_currentTarget =
            reinterpret_cast<sf::RenderTarget*>(target->GetNativeHandle());
    }
    // Reset view to default when changing target
    m_currentView = FView();
    m_currentTarget->setView(m_currentTarget->getDefaultView());
}

///////////////////////////////////////////////////////////////////////////////
FVector2u Renderer::GetRenderTargetSize(void) const
{
    sf::Vector2u size = m_currentTarget->getSize();
    return FVector2u(size.x, size.y);
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::PushScissorTest(const FRectangle& rect)
{
    TKD_UNUSED(rect);
    // TODO: Implement scissor test using SFML views or OpenGL directly
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::PopScissorTest(void)
{
    // TODO: Implement scissor test pop
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::BeginFrame(void)
{
    // Reset states cache
    m_cachedStates = sf::RenderStates::Default;
    // Apply current view
    m_currentTarget->setView(ToSFMLView(m_currentView));
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::EndFrame(void)
{
    if (m_currentTarget == m_window) { m_window->display(); }
}

///////////////////////////////////////////////////////////////////////////////
sf::BlendMode Renderer::ToSFMLBlendMode(EBlendMode mode)
{
    switch (mode)
    {
    case EBlendMode::Alpha   : return sf::BlendAlpha;
    case EBlendMode::Add     : return sf::BlendAdd;
    case EBlendMode::Multiply: return sf::BlendMultiply;
    case EBlendMode::None    : return sf::BlendNone;
    default                  : return sf::BlendAlpha;
    }
}

///////////////////////////////////////////////////////////////////////////////
sf::PrimitiveType Renderer::ToSFMLPrimitiveType(EPrimitiveType type)
{
    switch (type)
    {
    case EPrimitiveType::Points   : return sf::PrimitiveType::Points;
    case EPrimitiveType::Lines    : return sf::PrimitiveType::Lines;
    case EPrimitiveType::LineStrip: return sf::PrimitiveType::LineStrip;
    case EPrimitiveType::Triangles: return sf::PrimitiveType::Triangles;
    case EPrimitiveType::TriangleStrip:
        return sf::PrimitiveType::TriangleStrip;
    case EPrimitiveType::TriangleFan: return sf::PrimitiveType::TriangleFan;
    case EPrimitiveType::Quads      : return sf::PrimitiveType::Quads;
    default                         : return sf::PrimitiveType::Triangles;
    }
}

///////////////////////////////////////////////////////////////////////////////
sf::Color Renderer::ToSFMLColor(const FColor& color)
{
    return sf::Color(color.r, color.g, color.b, color.a);
}

///////////////////////////////////////////////////////////////////////////////
sf::Vertex Renderer::ToSFMLVertex(const FVertex2D& vertex)
{
    return sf::Vertex(
        sf::Vector2f(vertex.position.x, vertex.position.y),
        ToSFMLColor(vertex.color),
        sf::Vector2f(vertex.uv.x, vertex.uv.y)
    );
}

///////////////////////////////////////////////////////////////////////////////
sf::View Renderer::ToSFMLView(const FView& view)
{
    sf::View sfmlView;
    FRectangle rect = view.GetViewport();
    sfmlView.setCenter(
        rect.left + rect.width / 2.0f, rect.top + rect.height / 2.0f
    );
    sfmlView.setSize(rect.width, rect.height);
    sfmlView.setRotation(view.GetRotation());
    return sfmlView;
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::SetupSFMLStates(const FRenderStates& states)
{
    m_cachedStates.blendMode = ToSFMLBlendMode(states.blendMode);
    // TODO: Handle transform, texture, and shader
}

#endif

}   // namespace tkd::SFML
