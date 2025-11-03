///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Core/UI/BP_Level1Button.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
BP_Level1Button::BP_Level1Button(void)
    : AHUD()
    , ButtonColor(*this, "ButtonColor", FColor::Green)
{
    AddComponent<UWidgetButtonComponent>("MainButton");
}

///////////////////////////////////////////////////////////////////////////////
void BP_Level1Button::BeginPlay(void)
{
    Super::BeginPlay();

    // Setup button properties
    auto m_button = GetComponent<UWidgetButtonComponent>("MainButton");
    if (m_button)
    {
        m_button->SetColor(ButtonColor.GetValue());    // Red color
        m_button->SetSize(FVector2(200.0f, 200.0f));   // 200x200 pixels
        m_button->SetAlignment(UWidgetComponent::EAlignment::BottomCenter);
        m_button->SetPosition(
            // FVector2(640.0f, 360.0f) // Center of screen (1280x720)
            // FVector2(0.0f, 0.0f)   // Top-left corner
            // FVector2(0.0f, 720.0f)   // Top-right corner
            // FVector2(1280.0f, 720.0f) // Bottom-right corner
            // FVector2(1280.0f, 0.0f)  // Bottom-left corner
            // FVector2(0.0f, 360.0f)   // Middle-left edge
            // FVector2(1280.0f, 360.0f)  // Middle-right edge
            FVector2(640.0f, 720.0f)   // Bottom-center edge
            // FVector2(640.0f, 0.0f)   // Top-center edge
        );
        m_button->SetOnHeld([m_button]()
                            { m_button->SetColor(FColor::Yellow); });
        m_button->SetOnReleased([m_button]()
                                { m_button->SetColor(FColor::Green); });
    }
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_Level1Button, AHUD)

}   // namespace tkd
