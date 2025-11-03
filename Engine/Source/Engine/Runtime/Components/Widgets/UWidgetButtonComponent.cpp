///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Runtime/Components/Widgets/UWidgetButtonComponent.hpp>
#include <Engine/Renderer/Shapes/URectangleShape.hpp>   // add rectangle shape
#include <Engine/Runtime/Actor/AActor.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
UWidgetButtonComponent::UWidgetButtonComponent(const FString& name)
    : UWidgetComponent(name)
    , m_color(FColor::White)   // Default to white color
    , m_rectangleShape(nullptr)
    , m_isHovered(false)
    , m_isClicked(false)
    , m_clicks(0)
    , m_wasMousePressed(false)
    , m_onClickCallback(nullptr)
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
    if (m_rectangleShape)
    {
        m_rectangleShape->SetFillColor(m_color);   // update live shape color
    }
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetButtonComponent::BeginPlay(void)
{
    // Call parent BeginPlay
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
void UWidgetButtonComponent::EndPlay(void)
{
    // Call parent EndPlay
    Super::EndPlay();

    if (m_rectangleShape)
    {
        delete m_rectangleShape;
        m_rectangleShape = nullptr;
    }
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetButtonComponent::Tick(Float32 deltaTime)
{
    Super::Tick(deltaTime);

    // Update shape for debug rendering
    if (m_rectangleShape)
    {
        m_rectangleShape->SetPosition(GetPosition());
        m_rectangleShape->SetSize(GetSize());
        m_rectangleShape->SetOrigin(GetOrigin());
    }

    if (!IsEnabled())
    {
        m_isHovered = false;
        m_wasMousePressed = false;
        return;
    }

    if (!m_inputManager) { return; }

    // Get mouse position and check if inside bounds
    FVector2 mousePos = m_inputManager->GetMousePosition();
    m_isHovered = ContainsPoint(mousePos);

    // Check for NEW click (just pressed this frame while hovering)
    bool isMousePressed = m_inputManager->IsPressed(EInput::Mouse_Left) ||
                          m_inputManager->IsJustReleased(EInput::Mouse_Left);

    m_isHeld = m_isHovered && isMousePressed;
    if (m_isHovered && isMousePressed && !m_wasMousePressed)
    {
        m_isClicked = true;
        m_wasMousePressed = true;
        m_clicks++;
    }
    else
    {
        m_isClicked = false;
        m_wasMousePressed = isMousePressed;
    }

    if (m_isClicked && m_onClickCallback) { m_onClickCallback(); }
    if (m_isHeld && m_onHeldCallback) { m_onHeldCallback(); }
    if (m_isHovered && m_onHoverCallback) { m_onHoverCallback(); }

    // Debug output (can be removed later)
    // if (m_isHovered) { std::cout << GetName() << ": hovered!" << std::endl;
    // } if (m_isClicked) { std::cout << GetName() << ": clicked!" <<
    // std::endl; } if (m_isHeld) { std::cout << GetName() << ": held!" <<
    // std::endl; }
}

///////////////////////////////////////////////////////////////////////////////
bool UWidgetButtonComponent::ContainsPoint(const FVector2& point) const
{
    FVector2 origin = GetOrigin();
    FVector2 topLeft = GetPosition() - origin;
    FVector2 bottomRight = topLeft + GetSize();

    return point.x >= topLeft.x && point.x <= bottomRight.x &&
           point.y >= topLeft.y && point.y <= bottomRight.y;
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetButtonComponent::Render(IRenderer& renderer) const
{
    // Only render debug shape if visible (can be toggled off)
    if (IsVisible() && m_rectangleShape) { m_rectangleShape->Draw(renderer); }
}

}   // namespace tkd
