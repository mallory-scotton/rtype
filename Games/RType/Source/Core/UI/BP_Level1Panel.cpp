///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Core/UI/BP_Level1Panel.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
BP_Level1Panel::BP_Level1Panel(void)
    : AHUD()
    , PanelColor(*this, "PanelColor", FColor::Red)
{
    AddComponent<UWidgetPanelComponent>("MainPanel");
}

///////////////////////////////////////////////////////////////////////////////
void BP_Level1Panel::BeginPlay(void)
{
    Super::BeginPlay();

    // Setup panel properties
    auto m_panel = GetComponent<UWidgetPanelComponent>("MainPanel");
    if (m_panel)
    {
        m_panel->SetColor(PanelColor.GetValue());     // Red color
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
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_Level1Panel, AHUD)

}   // namespace tkd
