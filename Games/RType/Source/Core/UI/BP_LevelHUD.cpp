///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Core/UI/BP_LevelHUD.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
BP_LevelHUD::BP_LevelHUD(void)
    : AHUD()
{
    AddComponent<UWidgetPanelComponent>("MainPanel");
    AddComponent<UWidgetTextComponent>("textPlayer");
    AddComponent<UWidgetTextComponent>("textPlayerScore");
    AddComponent<UWidgetTextComponent>("textHigh");
    AddComponent<UWidgetTextComponent>("textHighScore");
    AddComponent<UWidgetTextComponent>("textBEAM");
}

///////////////////////////////////////////////////////////////////////////////
void BP_LevelHUD::BeginPlay(void)
{
    Super::BeginPlay();

    // Setup panel properties
    auto m_bottomPanel = GetComponent<UWidgetPanelComponent>("MainPanel");
    if (m_bottomPanel)
    {
        m_bottomPanel->SetColor(
            FColor{ 0.1, 0.1, 0.1, 1 }
        );                                                  // Dark Grey color
        m_bottomPanel->SetSize(FVector2(2000.0f, 40.0f));   // 200x40 pixels
        m_bottomPanel->SetAlignment(
            UWidgetComponent::EAlignment::BottomCenter
        );
        m_bottomPanel->SetPosition(
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
        FString lol;
        int lal = atoi(lol.CStr());
    }

    // Setup Text properties
    auto m_textPlayer = GetComponent<UWidgetTextComponent>("textPlayer");
    if (m_textPlayer)
    {
        m_textPlayer->SetCharacterSize(TextSize);
        m_textPlayer->SetSpacing(TextSpacing);
        m_textPlayer->SetText("1P-");           // Set display text
        m_textPlayer->SetColor(FColor::Blue);   // Red text color
        m_textPlayer->SetFontPath("Assets/Font/mainFont.png");
        m_textPlayer->SetAlignment(UWidgetComponent::EAlignment::Center);
        m_textPlayer->SetPosition(
            // FVector2(640.0f, 360.0f)   // Center of screen (1280x720)
            // FVector2(0.0f, 0.0f)   // Top-left corner
            // FVector2(0.0f, 720.0f)   // Top-right corner
            // FVector2(1280.0f, 720.0f) // Bottom-right corner
            // FVector2(1280.0f, 0.0f)  // Bottom-left corner
            // FVector2(0.0f, 360.0f)   // Middle-left edge
            // FVector2(1280.0f, 360.0f)  // Middle-right edge
            FVector2(50.0f, 700.0f)   // Bottom-center edge
            // FVector2(640.0f, 0.0f)   // Top-center edge
        );
    }

    auto m_textPlayerScore =
        GetComponent<UWidgetTextComponent>("textPlayerScore");
    if (m_textPlayerScore)
    {
        m_textPlayerScore->SetCharacterSize(TextSize);
        m_textPlayerScore->SetSpacing(TextSpacing);
        m_textPlayerScore->SetText("Score");          // Set display text
        m_textPlayerScore->SetColor(FColor::White);   // White text color
        m_textPlayerScore->SetFontPath("Assets/Font/mainFont.png");
        m_textPlayerScore->SetAlignment(UWidgetComponent::EAlignment::Center);
        m_textPlayerScore->SetPosition(
            // FVector2(640.0f, 360.0f)   // Center of screen (1280x720)
            // FVector2(0.0f, 0.0f)   // Top-left corner
            // FVector2(0.0f, 720.0f)   // Top-right corner
            // FVector2(1280.0f, 720.0f) // Bottom-right corner
            // FVector2(1280.0f, 0.0f)  // Bottom-left corner
            // FVector2(0.0f, 360.0f)   // Middle-left edge
            // FVector2(1280.0f, 360.0f)  // Middle-right edge
            FVector2(175.0f, 700.0f)   // Bottom-center edge
            // FVector2(640.0f, 0.0f)   // Top-center edge
        );
    }

    auto m_textHigh = GetComponent<UWidgetTextComponent>("textHigh");
    if (m_textHigh)
    {
        m_textHigh->SetCharacterSize(TextSize);
        m_textHigh->SetSpacing(TextSpacing);
        m_textHigh->SetText("HI-");           // Set display text
        m_textHigh->SetColor(FColor::Blue);   // Red text color
        m_textHigh->SetFontPath("Assets/Font/mainFont.png");
        m_textHigh->SetAlignment(UWidgetComponent::EAlignment::Center);
        m_textHigh->SetPosition(
            // FVector2(640.0f, 360.0f)   // Center of screen (1280x720)
            // FVector2(0.0f, 0.0f)   // Top-left corner
            // FVector2(0.0f, 720.0f)   // Top-right corner
            // FVector2(1280.0f, 720.0f) // Bottom-right corner
            // FVector2(1280.0f, 0.0f)  // Bottom-left corner
            // FVector2(0.0f, 360.0f)   // Middle-left edge
            // FVector2(1280.0f, 360.0f)  // Middle-right edge
            FVector2(400.0f, 700.0f)   // Bottom-center edge
            // FVector2(640.0f, 0.0f)   // Top-center edge
        );
    }

    auto m_textHighScore = GetComponent<UWidgetTextComponent>("textHighScore");
    if (m_textHighScore)
    {
        m_textHighScore->SetCharacterSize(TextSize);
        m_textHighScore->SetSpacing(TextSpacing);
        m_textHighScore->SetText("Score");          // Set display text
        m_textHighScore->SetColor(FColor::White);   // White text color
        m_textHighScore->SetFontPath("Assets/Font/mainFont.png");
        m_textHighScore->SetAlignment(UWidgetComponent::EAlignment::Center);
        m_textHighScore->SetPosition(
            // FVector2(640.0f, 360.0f)   // Center of screen (1280x720)
            // FVector2(0.0f, 0.0f)   // Top-left corner
            // FVector2(0.0f, 720.0f)   // Top-right corner
            // FVector2(1280.0f, 720.0f) // Bottom-right corner
            // FVector2(1280.0f, 0.0f)  // Bottom-left corner
            // FVector2(0.0f, 360.0f)   // Middle-left edge
            // FVector2(1280.0f, 360.0f)  // Middle-right edge
            FVector2(525.0f, 700.0f)   // Bottom-center edge
            // FVector2(640.0f, 0.0f)   // Top-center edge
        );
    }

    auto m_textBEAM = GetComponent<UWidgetTextComponent>("textBEAM");
    if (m_textBEAM)
    {
        m_textBEAM->SetCharacterSize(TextSize);
        m_textBEAM->SetSpacing(TextSpacing);
        m_textBEAM->SetText("BEAM");          // Set display text
        m_textBEAM->SetColor(FColor::Blue);   // White text color
        m_textBEAM->SetFontPath("Assets/Font/mainFont.png");
        m_textBEAM->SetAlignment(UWidgetComponent::EAlignment::Center);
        m_textBEAM->SetPosition(
            // FVector2(640.0f, 360.0f)   // Center of screen (1280x720)
            // FVector2(0.0f, 0.0f)   // Top-left corner
            // FVector2(0.0f, 720.0f)   // Top-right corner
            // FVector2(1280.0f, 720.0f) // Bottom-right corner
            // FVector2(1280.0f, 0.0f)  // Bottom-left corner
            // FVector2(0.0f, 360.0f)   // Middle-left edge
            // FVector2(1280.0f, 360.0f)  // Middle-right edge
            FVector2(185.0f, 680.0f)   // Bottom-center edge
            // FVector2(640.0f, 0.0f)   // Top-center edge
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_LevelHUD, AHUD)

}   // namespace tkd
