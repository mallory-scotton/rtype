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
    , m_rectangleShape(nullptr)
{}

///////////////////////////////////////////////////////////////////////////////
const FColor& UWidgetPanelComponent::GetColor(void) const { return m_color; }

///////////////////////////////////////////////////////////////////////////////
void UWidgetPanelComponent::SetColor(const FColor& color)
{
    m_color = color;
    if (m_rectangleShape)
    {
        m_rectangleShape->SetFillColor(m_color);   // update live shape color
    }
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetPanelComponent::BeginPlay(void)
{
    Super::BeginPlay();

    m_rectangleShape = new URectangleShape();
    if (m_rectangleShape)
    {
        m_rectangleShape->SetFillColor(m_color);
        m_rectangleShape->SetPosition(GetPosition());
        m_rectangleShape->SetSize(GetSize());
        m_rectangleShape->SetOrigin(GetSize() / 2.0f);
    }
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetPanelComponent::EndPlay(void)
{
    if (m_rectangleShape)
    {
        delete m_rectangleShape;
        m_rectangleShape = nullptr;
    }
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetPanelComponent::Tick(Float32 deltaTime)
{
    // If the widget can move / resize at runtime, make sure the shape follows.
    // Only update when there is a shape to avoid null checks elsewhere.
    if (m_rectangleShape)
    {
        m_rectangleShape->SetPosition(GetPosition());
        m_rectangleShape->SetSize(GetSize());
        m_rectangleShape->SetOrigin(GetOrigin());
    }

    Super::Tick(deltaTime);
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetPanelComponent::Render(IRenderer& renderer) const
{
    if (m_rectangleShape) { m_rectangleShape->Draw(renderer); }
}

}   // namespace tkd
