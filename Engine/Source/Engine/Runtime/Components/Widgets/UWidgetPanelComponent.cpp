///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Runtime/Components/Widgets/UWidgetPanelComponent.hpp>
#include <Engine/Renderer/Shapes/URectangleShape.hpp>   // add rectangle shape
#include <Engine/Runtime/Actor/AActor.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
UWidgetPanelComponent::UWidgetPanelComponent(const FString& name)
    : UWidgetComponent(name)
    , m_color(FColor::White)   // Default to white color
    , m_rectangleShape()
{}

///////////////////////////////////////////////////////////////////////////////
const FColor& UWidgetPanelComponent::GetColor(void) const { return m_color; }

///////////////////////////////////////////////////////////////////////////////
void UWidgetPanelComponent::SetColor(const FColor& color)
{
    m_color = color;
    m_rectangleShape.SetFillColor(m_color);   // update live shape color
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetPanelComponent::BeginPlay(void)
{
    Super::BeginPlay();

    // how do I create a Urectangleshape without new ?
    m_rectangleShape.SetFillColor(m_color);
    m_rectangleShape.SetPosition(GetPosition());
    m_rectangleShape.SetSize(GetSize());
    m_rectangleShape.SetOrigin(GetSize() / 2.0f);
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetPanelComponent::EndPlay(void) { Super::EndPlay(); }

///////////////////////////////////////////////////////////////////////////////
void UWidgetPanelComponent::Tick(Float32 deltaTime)
{
    // Call parent Tick
    Super::Tick(deltaTime);

    m_rectangleShape.SetPosition(GetPosition());
    m_rectangleShape.SetSize(GetSize());
    m_rectangleShape.SetOrigin(GetOrigin());
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetPanelComponent::Render(IRenderer& renderer) const
{
    if (IsVisible()) { m_rectangleShape.Draw(renderer); }
}

}   // namespace tkd
