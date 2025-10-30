///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Runtime/UI/Slate/SWidget.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
SWidget::SWidget(AHUD* ownerHUD)
    : m_ownerHUD(ownerHUD)
    , m_spriteComponent("WidgetSprite")
    , m_position(FVector2::Zero)
    , m_visible(true)
{
    // Initialize the sprite component with identity transform
    m_spriteComponent.SetLocalTransform(FTransform2D::Identity);
}

///////////////////////////////////////////////////////////////////////////////
void SWidget::SetPosition(const FVector2& position)
{
    m_position = position;

    // Update sprite component transform to match new position
    FTransform2D transform = FTransform2D::Identity;
    transform.SetPosition(position);
    m_spriteComponent.SetLocalTransform(transform);
}

///////////////////////////////////////////////////////////////////////////////
void SWidget::Tick(Float32 deltaTime)
{
    // Update sprite animation if using a flipbook
    if (m_visible) { m_spriteComponent.Update(deltaTime); }
}

///////////////////////////////////////////////////////////////////////////////
void SWidget::Paint(IRenderer& renderer) const
{
    // Only render if visible and attached to a HUD
    if (m_visible && m_ownerHUD) { m_spriteComponent.Render(renderer); }
}

}   // namespace tkd
