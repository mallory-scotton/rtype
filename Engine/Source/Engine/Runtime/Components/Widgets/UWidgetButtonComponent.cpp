///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Runtime/Components/Widgets/UWidgetButtonComponent.hpp>
#include <Engine/Runtime/Actor/AActor.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
UWidgetButtonComponent::UWidgetButtonComponent(const FString& name)
    : UWidgetComponent(name)
    , m_color(FColor::White)
    , m_rectangleShape()
    , m_clicks(0)
    , m_onClickCallback(nullptr)
    , m_onUnclickedCallback(nullptr)
    , m_onReleasedCallback(nullptr)
    , m_onHeldCallback(nullptr)
    , m_onHoverCallback(nullptr)
{}

///////////////////////////////////////////////////////////////////////////////
const FColor& UWidgetButtonComponent::GetColor(void) const { return m_color; }

///////////////////////////////////////////////////////////////////////////////
void UWidgetButtonComponent::SetOnClick(FOnClickCallback callback)
{
    m_onClickCallback = callback;
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetButtonComponent::SetOnUnclicked(FOnUnclickedCallback callback)
{
    m_onUnclickedCallback = callback;
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetButtonComponent::SetOnReleased(FOnReleasedCallback callback)
{
    m_onReleasedCallback = callback;
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetButtonComponent::SetOnHeld(FOnHeldCallback callback)
{
    m_onHeldCallback = callback;
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetButtonComponent::SetOnHover(FOnHoverCallback callback)
{
    m_onHoverCallback = callback;
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetButtonComponent::SetColor(const FColor& color)
{
    m_color = color;
    m_rectangleShape.SetFillColor(m_color);   // update live shape color
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetButtonComponent::BeginPlay(void)
{
    Super::BeginPlay();

    m_rectangleShape.SetFillColor(m_color);
    m_rectangleShape.SetPosition(GetScaledPosition());

    // Use uniform scale to maintain aspect ratio
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
void UWidgetButtonComponent::EndPlay(void) { Super::EndPlay(); }

///////////////////////////////////////////////////////////////////////////////
void UWidgetButtonComponent::Tick(Float32 deltaTime)
{
    Super::Tick(deltaTime);

    // Update shape with uniform scaled values
    m_rectangleShape.SetPosition(GetScaledPosition());

    // Use uniform scale to maintain aspect ratio
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

    UpdateInputStates();

    // Button-specific: track click count
    if (Super::IsClicked()) { m_clicks++; }

    // Trigger callbacks
    if (Super::IsClicked() && m_onClickCallback) { m_onClickCallback(); }
    if (Super::IsUnclicked() && m_onUnclickedCallback)
    {
        m_onUnclickedCallback();
    }
    if (Super::IsReleased() && m_onReleasedCallback)
    {
        m_onReleasedCallback();
    }
    if (Super::IsHeld() && m_onHeldCallback) { m_onHeldCallback(); }
    if (Super::IsHovered() && m_onHoverCallback) { m_onHoverCallback(); }

    // Debug output (can be removed later)
    // std::cout << "WidgetButtonComponent Tick: Hovered=" << m_isHovered
    //           << " Clicked=" << m_isClicked << " Unclicked=" <<
    //           m_isUnclicked
    //           << " Released=" << m_isReleased << " Held=" << m_isHeld
    //           << " Clicks=" << m_clicks << std::endl;
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetButtonComponent::Render(IRenderer& renderer) const
{
    // Only render debug shape if visible (can be toggled off)
    if (IsVisible()) { m_rectangleShape.Draw(renderer); }
}

}   // namespace tkd
