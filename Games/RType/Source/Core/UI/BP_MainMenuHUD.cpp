///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Core/UI/BP_MainMenuHUD.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
BP_MainMenuHUD::BP_MainMenuHUD(void)
    : AHUD()
{
    SetName("BP_MainMenuHUD");
    AddComponent<UWidgetImageComponent>("MainImage");
    AddComponent<UWidgetPanelComponent>("MainPanel");
    AddComponent<UWidgetButtonComponent>("MainButton");
    AddComponent<UWidgetTextComponent>("textPlayer");
    AddComponent<UWidgetTextboxComponent>("textBox");
    AddComponent<UWidgetImageComponent>("Circle");
    AddComponent<UWidgetImageComponent>("Search");
}

///////////////////////////////////////////////////////////////////////////////
void BP_MainMenuHUD::BeginPlay(void)
{
    Super::BeginPlay();

    auto m_mainImage = GetComponent<UWidgetImageComponent>("MainImage");
    if (m_mainImage)
    {
        m_mainImage->SetTexturePath("Assets/UI/Background.png");
        m_mainImage->SetSize(FVector2(1280.0f, 720.0f));
        m_mainImage->SetAlignment(UWidgetComponent::EAlignment::Center);
        m_mainImage->SetPosition(
            FVector2(640.0f, 360.0f)   // Center of screen (1280x720)
            // FVector2(0.0f, 0.0f)   // Top-left corner
            // FVector2(0.0f, 720.0f)   // Top-right corner
            // FVector2(1280.0f, 720.0f) // Bottom-right corner
            // FVector2(1280.0f, 0.0f)  // Bottom-left corner
            // FVector2(0.0f, 360.0f)   // Middle-left edge
            // FVector2(1280.0f, 360.0f)  // Middle-right edge
            // FVector2(640.0f, 720.0f)   // Bottom-center edge
            // FVector2(640.0f, 0.0f)   // Top-center edge
        );
    }

    auto m_mainPanel = GetComponent<UWidgetPanelComponent>("MainPanel");
    if (m_mainPanel)
    {
        m_mainPanel->SetColor(FLinearColor(0, 0, 0, 150));
        m_mainPanel->SetSize(FVector2(500.0f, 150.0f));
        m_mainPanel->SetAlignment(UWidgetComponent::EAlignment::Center);
        m_mainPanel->SetPosition(
            FVector2(640.0f, 360.0f)   // Center of screen (1280x720)
            // FVector2(0.0f, 0.0f)   // Top-left corner
            // FVector2(0.0f, 720.0f)   // Top-right corner
            // FVector2(1280.0f, 720.0f) // Bottom-right corner
            // FVector2(1280.0f, 0.0f)  // Bottom-left corner
            // FVector2(0.0f, 360.0f)   // Middle-left edge
            // FVector2(1280.0f, 360.0f)  // Middle-right edge
            // FVector2(640.0f, 720.0f)   // Bottom-center edge
            // FVector2(640.0f, 0.0f)   // Top-center edge
        );
    }

    // Setup Text properties
    auto m_textPlayer = GetComponent<UWidgetTextComponent>("textPlayer");
    if (m_textPlayer)
    {
        m_textPlayer->SetCharacterSize(TextSize);
        m_textPlayer->SetSpacing(TextSpacing);
        m_textPlayer->SetText("Type IP you wanna connect to");
        m_textPlayer->SetColor(FColor::White);
        m_textPlayer->SetFontPath("Assets/Font/mainFont.png");
        m_textPlayer->SetAlignment(UWidgetComponent::EAlignment::Center);
        m_textPlayer->SetPosition(
            FVector2(640.0f, 320.0f)
            // FVector2(640.0f, 360.0f)   // Center of screen (1280x720)
            // FVector2(0.0f, 0.0f)   // Top-left corner
            // FVector2(0.0f, 720.0f)   // Top-right corner
            // FVector2(1280.0f, 720.0f) // Bottom-right corner
            // FVector2(1280.0f, 0.0f)  // Bottom-left corner
            // FVector2(0.0f, 360.0f)   // Middle-left edge
            // FVector2(1280.0f, 360.0f)  // Middle-right edge
            // FVector2(50.0f, 700.0f)   // Bottom-center edge
            // FVector2(640.0f, 0.0f)   // Top-center edge
        );
    }

    auto m_textBox = GetComponent<UWidgetTextboxComponent>("textBox");
    if (m_textBox)
    {
        m_textBox->SetCharacterSize(TextSize);
        m_textBox->SetSpacing(TextSpacing);
        m_textBox->SetText("Enter Input");
        m_textBox->SetBackgroundColor(FColor(0.3, 0.3, 0.3, 1.0));
        m_textBox->SetFocusedBackgroundColor(FColor(0.5, 0.5, 0.5, 1.0));
        m_textBox->SetFontPath("Assets/Font/mainFont.png");
        m_textBox->SetAlignment(UWidgetComponent::EAlignment::Center);
        m_textBox->SetPosition(
            FVector2(640.0f, 400.0f)
            // FVector2(640.0f, 360.0f)   // Center of screen (1280x720)
            // FVector2(0.0f, 0.0f)   // Top-left corner
            // FVector2(0.0f, 720.0f)   // Top-right corner
            // FVector2(1280.0f, 720.0f) // Bottom-right corner
            // FVector2(1280.0f, 0.0f)  // Bottom-left corner
            // FVector2(0.0f, 360.0f)   // Middle-left edge
            // FVector2(1280.0f, 360.0f)  // Middle-right edge
            // FVector2(640.0f, 720.0f)   // Bottom-center edge
            // FVector2(640.0f, 0.0f)   // Top-center edge
        );
        m_textBox->SetMaxCharacters(16);
        m_textBox->SetMinBoxChars(16);
        m_textBox->SetMaxBoxChars(16);
        m_textBox->SetTextColor(FColor::White);
    }

    auto m_searchCircle = GetComponent<UWidgetImageComponent>("Circle");
    if (m_searchCircle)
    {
        m_searchCircle->SetTexturePath("Assets/UI/Circle.png");
        m_searchCircle->SetSize(FVector2(50.0f, 50.0f));
        m_searchCircle->SetColor(FLinearColor(100, 100, 100, 255));
        m_searchCircle->SetAlignment(UWidgetComponent::EAlignment::Center);
        m_searchCircle->SetPosition(
            FVector2(825.0f, 400.0f)
            // FVector2(640.0f, 360.0f) // Center of screen (1280x720)
            // FVector2(0.0f, 0.0f)   // Top-left corner
            // FVector2(0.0f, 720.0f)   // Top-right corner
            // FVector2(1280.0f, 720.0f) // Bottom-right corner
            // FVector2(1280.0f, 0.0f)  // Bottom-left corner
            // FVector2(0.0f, 360.0f)   // Middle-left edge
            // FVector2(1280.0f, 360.0f)  // Middle-right edge
            // FVector2(640.0f, 720.0f)   // Bottom-center edge
            // FVector2(640.0f, 0.0f)   // Top-center edge
        );
    }

    auto m_searchIcon = GetComponent<UWidgetImageComponent>("Search");
    if (m_searchIcon)
    {
        m_searchIcon->SetTexturePath("Assets/UI/Search.png");
        m_searchIcon->SetSize(FVector2(50.0f, 50.0f));
        m_searchIcon->SetColor(FLinearColor(50, 50, 50, 255));
        m_searchIcon->SetAlignment(UWidgetComponent::EAlignment::Center);
        m_searchIcon->SetPosition(
            FVector2(825.0f, 400.0f)
            // FVector2(640.0f, 360.0f) // Center of screen (1280x720)
            // FVector2(0.0f, 0.0f)   // Top-left corner
            // FVector2(0.0f, 720.0f)   // Top-right corner
            // FVector2(1280.0f, 720.0f) // Bottom-right corner
            // FVector2(1280.0f, 0.0f)  // Bottom-left corner
            // FVector2(0.0f, 360.0f)   // Middle-left edge
            // FVector2(1280.0f, 360.0f)  // Middle-right edge
            // FVector2(640.0f, 720.0f)   // Bottom-center edge
            // FVector2(640.0f, 0.0f)   // Top-center edge
        );
    }

    // Setup button properties
    auto m_buttonWidget = GetComponent<UWidgetButtonComponent>("MainButton");
    if (m_buttonWidget)
    {
        m_buttonWidget->SetColor(FColor::Green);           // Red color
        m_buttonWidget->SetVisible(false);
        m_buttonWidget->SetSize(FVector2(50.0f, 50.0f));   // 50x50
        m_buttonWidget->SetAlignment(UWidgetComponent::EAlignment::Center);
        m_buttonWidget->SetPosition(
            FVector2(825.0f, 400.0f)
            // FVector2(640.0f, 360.0f) // Center of screen (1280x720)
            // FVector2(0.0f, 0.0f)   // Top-left corner
            // FVector2(0.0f, 720.0f)   // Top-right corner
            // FVector2(1280.0f, 720.0f) // Bottom-right corner
            // FVector2(1280.0f, 0.0f)  // Bottom-left corner
            // FVector2(0.0f, 360.0f)   // Middle-left edge
            // FVector2(1280.0f, 360.0f)  // Middle-right edge
            // FVector2(640.0f, 720.0f)   // Bottom-center edge
            // FVector2(640.0f, 0.0f)   // Top-center edge
        );
        m_buttonWidget->SetOnClick(
            [m_textBox, m_searchCircle, m_searchIcon]()
            {
                std::cout << "Ip : " << m_textBox->GetText() << std::endl;
                m_searchCircle->SetSize(FVector2(45, 45));
                m_searchIcon->SetSize(FVector2(45, 45));
            }
        );
        m_buttonWidget->SetOnUnclicked(
            [m_textBox, m_searchCircle, m_searchIcon]()
            {
                m_searchCircle->SetSize(FVector2(50, 50));
                m_searchIcon->SetSize(FVector2(50, 50));
            }
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
void BP_MainMenuHUD::Tick(Float32 deltaTime)
{
    Super::Tick(deltaTime);

    // // Make instruction text blink
    // if (m_instructionText)
    // {
    //     static Float32 blinkTimer = 0.0f;
    //     blinkTimer += deltaTime;

    //     UInt8 alpha = static_cast<UInt8>(
    //         (std::sin(blinkTimer * 2.0f) * 0.5f + 0.5f) * 255.0f
    //     );
    //     m_instructionText->SetColor(FColor(200, 200, 200, alpha));
    // }
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_MainMenuHUD, AHUD)

}   // namespace tkd
