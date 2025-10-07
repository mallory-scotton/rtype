///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Renderer/SFML/Renderer.hpp>
#include <Engine/Config.hpp>
#include <Engine/Renderer/SFML/Utils.hpp>
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
    m_currentTarget->clear(Utils::Convert(color));
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::SetView(const FView& view)
{
    m_currentView = view;
    m_currentTarget->setView(Utils::Convert(view));
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

    std::vector<sf::Vertex> sfmlVertices;
    sfmlVertices.reserve(vertexCount);
    for (UInt32 i = 0; i < vertexCount; ++i)
    {
        sfmlVertices.push_back(Utils::Convert(vertices[i]));
    }

    m_currentTarget->draw(
        sfmlVertices.data(),
        vertexCount,
        Utils::Convert(type),
        Utils::Convert(states)
    );
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
    // Apply current view
    m_currentTarget->setView(Utils::Convert(m_currentView));
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::EndFrame(void)
{
    if (m_currentTarget == m_window) { m_window->display(); }
}

#endif

}   // namespace tkd::SFML
