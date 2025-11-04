///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Core/UI/BP_Level1HUD.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
BP_Level1HUD::BP_Level1HUD(void)
    : AHUD()
    , PanelColor(FColor::Red)
    , ButtonColor(FColor::Green)
    , ImageTexturePath("Assets/Images/r-typesheet41.png")
{
    AddComponent<UWidgetPanelComponent>("MainPanel");
    AddComponent<UWidgetButtonComponent>("MainButton");
    AddComponent<UWidgetImageComponent>("MainImage");
}

///////////////////////////////////////////////////////////////////////////////
void BP_Level1HUD::BeginPlay(void)
{
    Super::BeginPlay();

    // Setup panel properties
    auto m_panel = GetComponent<UWidgetPanelComponent>("MainPanel");
    if (m_panel)
    {
        m_panel->SetColor(PanelColor);                // Red color
        m_panel->SetSize(FVector2(200.0f, 200.0f));   // 200x200 pixels
        m_panel->SetAlignment(UWidgetComponent::EAlignment::TopCenter);
        m_panel->SetPosition(
            // FVector2(640.0f, 360.0f) // Center of screen (1280x720)
            // FVector2(0.0f, 0.0f)   // Top-left corner
            // FVector2(0.0f, 720.0f)   // Top-right corner
            // FVector2(1280.0f, 720.0f) // Bottom-right corner
            // FVector2(1280.0f, 0.0f)  // Bottom-left corner
            // FVector2(0.0f, 360.0f)   // Middle-left edge
            // FVector2(1280.0f, 360.0f)  // Middle-right edge
            // FVector2(640.0f, 720.0f)   // Bottom-center edge
            FVector2(640.0f, 0.0f)   // Top-center edge
        );
    }

    // Setup button properties
    auto m_button = GetComponent<UWidgetButtonComponent>("MainButton");
    if (m_button)
    {
        m_button->SetColor(ButtonColor);               // Red color
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

    // Setup image properties
    auto m_image = GetComponent<UWidgetImageComponent>("MainImage");
    if (m_image)
    {
        m_image->SetTexturePath(ImageTexturePath);   // Set image texture path
        m_image->SetAlignment(UWidgetComponent::EAlignment::CenterLeft);
        m_image->SetPosition(
            // FVector2(640.0f, 360.0f) // Center of screen (1280x720)
            // FVector2(0.0f, 0.0f)   // Top-left corner
            // FVector2(0.0f, 720.0f)   // Top-right corner
            // FVector2(1280.0f, 720.0f) // Bottom-right corner
            // FVector2(1280.0f, 0.0f)  // Bottom-left corner
            FVector2(0.0f, 360.0f)   // Middle-left edge
            // FVector2(1280.0f, 360.0f)  // Middle-right edge
            // FVector2(640.0f, 720.0f)   // Bottom-center edge
            // FVector2(640.0f, 0.0f)   // Top-center edge
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_Level1HUD, AHUD)

}   // namespace tkd
