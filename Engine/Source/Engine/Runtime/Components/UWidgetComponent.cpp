///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Runtime/Components/UWidgetComponent.hpp>
#include <Engine/Runtime/Actor/AActor.hpp>
#include <Engine/Static/FWindowInterface.hpp>
#include <Engine/Static/FWindowSubsystem.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
UWidgetComponent::UWidgetComponent(const FString& name)
    : UActorComponent(name)
    , m_inputManager(nullptr)
    , m_position(FVector2::Zero)
    , m_rawPosition(FVector2::Zero)
    , m_size(FVector2::One)
    , m_rawSize(FVector2::One)
    , m_scale(FVector2::One)
    , m_zIndex(0)
    , m_align(EAlignment::Center)
    , m_visible(true)
    , m_enabled(true)
    , m_rawModeEnabled(false)
    , m_isHovered(false)   // input state tracking
    , m_isClicked(false)
    , m_isUnclicked(false)
    , m_isReleased(false)
    , m_isHeld(false)
    , m_isFocused(false)
    , m_wasMousePressed(false)
{}

///////////////////////////////////////////////////////////////////////////////
const FVector2& UWidgetComponent::GetPosition(void) const
{
    return m_position;
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetComponent::SetPosition(const FVector2& position)
{
    m_position = position;
}

///////////////////////////////////////////////////////////////////////////////
const FVector2& UWidgetComponent::GetRawPosition(void) const
{
    return m_rawPosition;
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetComponent::SetRawPosition(const FVector2& position)
{
    m_rawPosition = position;
}

///////////////////////////////////////////////////////////////////////////////
bool UWidgetComponent::GetRawMode(void) const { return m_rawModeEnabled; }

void UWidgetComponent::SetRawMode(bool enabled) { m_rawModeEnabled = enabled; }

///////////////////////////////////////////////////////////////////////////////
bool UWidgetComponent::IsVisible(void) const { return m_visible; }

///////////////////////////////////////////////////////////////////////////////
void UWidgetComponent::SetVisible(bool visible) { m_visible = visible; }

///////////////////////////////////////////////////////////////////////////////
void UWidgetComponent::SetInputManager(FInputManager* inputManager)
{
    m_inputManager = inputManager;
}

///////////////////////////////////////////////////////////////////////////////
const FVector2& UWidgetComponent::GetSize(void) const { return m_size; }

///////////////////////////////////////////////////////////////////////////////
void UWidgetComponent::SetSize(const FVector2& size) { m_size = size; }

///////////////////////////////////////////////////////////////////////////////
const FVector2& UWidgetComponent::GetScale(void) const { return m_scale; }

///////////////////////////////////////////////////////////////////////////////
void UWidgetComponent::SetScale(const FVector2& scale) { m_scale = scale; }

///////////////////////////////////////////////////////////////////////////////
Int32 UWidgetComponent::GetZIndex(void) const { return m_zIndex; }

///////////////////////////////////////////////////////////////////////////////
void UWidgetComponent::SetZIndex(Int32 zIndex) { m_zIndex = zIndex; }

///////////////////////////////////////////////////////////////////////////////
UWidgetComponent::EAlignment UWidgetComponent::GetAlignment(void) const
{
    return m_align;
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetComponent::SetAlignment(EAlignment alignment)
{
    m_align = alignment;
}

///////////////////////////////////////////////////////////////////////////////
FVector2 UWidgetComponent::GetOrigin() const
{
    if (m_align == EAlignment::None) { return FVector2::Zero; }

    int alignIndex = static_cast<int>(m_align) - 1;
    int alignX = alignIndex % 3;   // 0=Left, 1=Center, 2=Right
    int alignY = alignIndex / 3;   // 0=Top, 1=Center, 2=Bottom

    return FVector2(alignX * m_size.x * 0.5f, alignY * m_size.y * 0.5f);
}

///////////////////////////////////////////////////////////////////////////////
bool UWidgetComponent::IsEnabled(void) const { return m_enabled; }

///////////////////////////////////////////////////////////////////////////////
void UWidgetComponent::SetEnabled(Bool enabled) { m_enabled = enabled; }

///////////////////////////////////////////////////////////////////////////////
bool UWidgetComponent::ContainsPoint(const FVector2& point) const
{
    // Use scaled position and size for hit detection
    FVector2 scaledPos = GetScaledPosition();
    FVector2 scaledSize = GetScaledSize();

    // Calculate origin based on alignment
    FVector2 origin = FVector2::Zero;
    if (m_align != EAlignment::None)
    {
        int alignIndex = static_cast<int>(m_align) - 1;
        int alignX = alignIndex % 3;   // 0=Left, 1=Center, 2=Right
        int alignY = alignIndex / 3;   // 0=Top, 1=Center, 2=Bottom
        origin = FVector2(
            alignX * scaledSize.x * 0.5f, alignY * scaledSize.y * 0.5f
        );
    }

    FVector2 topLeft = scaledPos - origin;
    FVector2 bottomRight = topLeft + scaledSize;

    return point.x >= topLeft.x && point.x <= bottomRight.x &&
           point.y >= topLeft.y && point.y <= bottomRight.y;
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetComponent::UpdateInputStates()
{
    if (!IsEnabled() || !m_inputManager)
    {
        m_isHovered = false;
        m_wasMousePressed = false;
        m_isClicked = false;
        m_isUnclicked = false;
        m_isReleased = true;
        m_isHeld = false;
        m_isFocused = false;
        return;
    }

    if (!m_inputManager) { return; }

    FVector2 mousePos = m_inputManager->GetMousePosition();
    m_isHovered = ContainsPoint(mousePos);

    bool isMousePressed = m_inputManager->IsPressed(EInput::Mouse_Left) ||
                          m_inputManager->IsJustReleased(EInput::Mouse_Left);

    m_isHeld = m_isHovered && isMousePressed;
    m_isReleased = !m_isHeld;

    if (isMousePressed && !m_wasMousePressed)
    {
        if (!m_isHovered) { m_isFocused = false; }
        else
        {
            m_isFocused = true;
            m_isClicked = true;
            m_wasMousePressed = true;
        }
    }
    else if (!isMousePressed && m_wasMousePressed)
    {
        m_isUnclicked = true;
        m_wasMousePressed = false;
    }
    else
    {
        m_isClicked = false;
        m_isUnclicked = false;
        m_wasMousePressed = isMousePressed;
    }
#if TKD_ENGINE_CLIENT
    auto* windowSubsystem = Window::GetWindowSubsystem();
    if (windowSubsystem && windowSubsystem->GetWindow())
    {
        FVector2u physicalSize = Window::GetDimensions();
        const FView& view = windowSubsystem->GetWindow()->GetCurrentView();
        FVector2 gameSize = view.GetSize();
        const FRectangle& viewport = view.GetViewport();

        std::cout << "Physical: " << physicalSize.x << "x" << physicalSize.y
                  << " | Game: " << gameSize.x << "x" << gameSize.y
                  << " | Viewport: [" << viewport.left << "," << viewport.top
                  << "," << viewport.width << "," << viewport.height << "]"
                  << std::endl;
    }
#endif
}

///////////////////////////////////////////////////////////////////////////////
FVector2 UWidgetComponent::GetScaledPosition(void) const
{
#if TKD_ENGINE_CLIENT
    FVector2u actualSize = Window::GetDimensions();
    constexpr float refWidth = 1920.0f;
    constexpr float refHeight = 1080.0f;

    // If window dimensions are not yet initialized, return unscaled position
    if (actualSize.x == 0 || actualSize.y == 0) { return m_position; }

    float scaleX = static_cast<float>(actualSize.x) / refWidth;
    float scaleY = static_cast<float>(actualSize.y) / refHeight;

    return FVector2(m_position.x * scaleX, m_position.y * scaleY);
#else
    return m_position;
#endif
}

///////////////////////////////////////////////////////////////////////////////
FVector2 UWidgetComponent::GetScaledSize(void) const
{
#if TKD_ENGINE_CLIENT
    FVector2u actualSize = Window::GetDimensions();
    constexpr float refWidth = 1920.0f;
    constexpr float refHeight = 1080.0f;

    // If window dimensions are not yet initialized, return unscaled size
    if (actualSize.x == 0 || actualSize.y == 0) { return m_size; }

    float scaleX = static_cast<float>(actualSize.x) / refWidth;
    float scaleY = static_cast<float>(actualSize.y) / refHeight;

    return FVector2(m_size.x * scaleX, m_size.y * scaleY);
#else
    return m_size;
#endif
}

///////////////////////////////////////////////////////////////////////////////
float UWidgetComponent::GetUniformScale(void) const
{
#if TKD_ENGINE_CLIENT
    FVector2u actualSize = Window::GetDimensions();
    constexpr float refWidth = 1920.0f;
    constexpr float refHeight = 1080.0f;

    // If window dimensions are not yet initialized, return 1.0 (no scale)
    if (actualSize.x == 0 || actualSize.y == 0) { return 1.0f; }

    float scaleX = static_cast<float>(actualSize.x) / refWidth;
    float scaleY = static_cast<float>(actualSize.y) / refHeight;

    // Use average of both scales to maintain aspect ratio
    return (scaleX + scaleY) * 0.5f;
#else
    return 1.0f;
#endif
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetComponent::BeginPlay(void)
{
#if TKD_ENGINE_CLIENT
    SetInputManager(Window::GetInputManager());
#endif
    Super::BeginPlay();
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetComponent::EndPlay(void)
{
    // Cleanup logic can be added here if needed
    Super::EndPlay();
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetComponent::Tick(Float32 deltaTime) { Super::Tick(deltaTime); }

}   // namespace tkd
