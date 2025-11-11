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
    m_rectangleShape.SetFillColor(m_color);
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetPanelComponent::SetColor(const FLinearColor& color)
{
    SetColor(FColor(color));
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetPanelComponent::BeginPlay(void)
{
    Super::BeginPlay();

    // Initialize rectangle with uniform scaled values
    m_rectangleShape.SetFillColor(m_color);
    m_rectangleShape.SetPosition(GetScaledPosition());

    // Use uniform scale to maintain aspect ratio like text
    float uniformScale = GetUniformScale();
    FVector2 scaledSize = GetSize() * uniformScale;
    m_rectangleShape.SetSize(scaledSize);

    // Calculate scaled origin
    FVector2 origin = FVector2::Zero;
    if (GetAlignment() != EAlignment::None)
    {
        int alignIndex = static_cast<int>(GetAlignment()) - 1;
        int alignX = alignIndex % 3;
        int alignY = alignIndex / 3;
        origin = FVector2(
            alignX * scaledSize.x * 0.5f, alignY * scaledSize.y * 0.5f
        );
    }
    m_rectangleShape.SetOrigin(origin);
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetPanelComponent::EndPlay(void) { Super::EndPlay(); }

///////////////////////////////////////////////////////////////////////////////
void UWidgetPanelComponent::Tick(Float32 deltaTime)
{
    // Call parent Tick
    Super::Tick(deltaTime);

    // Update rectangle with uniform scaled values
    m_rectangleShape.SetPosition(GetScaledPosition());

    // Use uniform scale to maintain aspect ratio like text
    float uniformScale = GetUniformScale();
    FVector2 scaledSize = GetSize() * uniformScale;
    m_rectangleShape.SetSize(scaledSize);

    // Calculate scaled origin
    FVector2 origin = FVector2::Zero;
    if (GetAlignment() != EAlignment::None)
    {
        int alignIndex = static_cast<int>(GetAlignment()) - 1;
        int alignX = alignIndex % 3;
        int alignY = alignIndex / 3;
        origin = FVector2(
            alignX * scaledSize.x * 0.5f, alignY * scaledSize.y * 0.5f
        );
    }
    m_rectangleShape.SetOrigin(origin);
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetPanelComponent::Render(IRenderer& renderer) const
{
    if (IsVisible()) { m_rectangleShape.Draw(renderer); }
}

}   // namespace tkd
