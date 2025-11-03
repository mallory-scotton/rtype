///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Runtime/Components/UWidgetComponent.hpp>
#include <Engine/Runtime/Actor/AActor.hpp>
#include <Engine/Static/FWindowInterface.hpp>

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
void UWidgetComponent::Tick(Float32 deltaTime) { Super::Tick(deltaTime); }

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
void UWidgetComponent::BeginPlay(void)
{
    std::cout << "UWidgetComponent::BeginPlay - Initializing for: "
              << GetName() << std::endl;
#if TKD_ENGINE_CLIENT
    SetInputManager(Window::GetInputManager());
    std::cout << "UWidgetComponent::BeginPlay - InputManager set for: "
              << GetName() << std::endl;
    std::cout << "UWidgetComponent::BeginPlay - inputManager address: "
              << m_inputManager << std::endl;
#endif
    std::cout << "UWidgetComponent::BeginPlay - Finished initializing for: "
              << GetName() << std::endl;
    Super::BeginPlay();
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetComponent::EndPlay(void)
{
    // Cleanup logic can be added here if needed
    Super::EndPlay();
}

}   // namespace tkd
