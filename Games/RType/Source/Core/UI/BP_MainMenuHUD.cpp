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

    // Background
    AddComponent<UWidgetImageComponent>("MainImage");

    // Stage 1: Main menu buttons
    AddComponent<UWidgetButtonComponent>("PlaySoloButton");
    AddComponent<UWidgetButtonComponent>("ConnectButton");
    AddComponent<UWidgetTextComponent>("PlaySoloText");
    AddComponent<UWidgetTextComponent>("ConnectText");

    // Stage 2: Connection UI
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

    // Setup both UIs (configure all widgets)
    ShowMenuUI();
    ShowConnectionUI();

    // Start with menu visible, connection UI hidden
    ClearConnectionUI();
}

///////////////////////////////////////////////////////////////////////////////
void BP_MainMenuHUD::ClearMainMenuWidgets(void)
{
    // Disable and hide Stage 1 widgets (main menu buttons)
    auto playSoloButton =
        GetComponent<UWidgetButtonComponent>("PlaySoloButton");
    if (playSoloButton)
    {
        playSoloButton->SetVisible(false);
        playSoloButton->SetEnabled(false);   // Disable interaction
    }

    auto playSoloText = GetComponent<UWidgetTextComponent>("PlaySoloText");
    if (playSoloText)
    {
        playSoloText->SetVisible(false);
        playSoloText->SetEnabled(false);
    }

    auto connectButton = GetComponent<UWidgetButtonComponent>("ConnectButton");
    if (connectButton)
    {
        connectButton->SetVisible(false);
        connectButton->SetEnabled(false);   // Disable interaction
    }

    auto connectText = GetComponent<UWidgetTextComponent>("ConnectText");
    if (connectText)
    {
        connectText->SetVisible(false);
        connectText->SetEnabled(false);
    }

    // Background (MainImage) remains visible - no changes needed
}

///////////////////////////////////////////////////////////////////////////////
void BP_MainMenuHUD::ClearConnectionUI(void)
{
    // Disable and hide all connection UI widgets
    auto mainPanel = GetComponent<UWidgetPanelComponent>("MainPanel");
    if (mainPanel)
    {
        mainPanel->SetVisible(false);
        mainPanel->SetEnabled(false);
    }

    auto textPlayer = GetComponent<UWidgetTextComponent>("textPlayer");
    if (textPlayer)
    {
        textPlayer->SetVisible(false);
        textPlayer->SetEnabled(false);
    }

    auto textBox = GetComponent<UWidgetTextboxComponent>("textBox");
    if (textBox)
    {
        textBox->SetVisible(false);
        textBox->SetEnabled(false);
    }

    auto searchCircle = GetComponent<UWidgetImageComponent>("Circle");
    if (searchCircle)
    {
        searchCircle->SetVisible(false);
        searchCircle->SetEnabled(false);
    }

    auto searchIcon = GetComponent<UWidgetImageComponent>("Search");
    if (searchIcon)
    {
        searchIcon->SetVisible(false);
        searchIcon->SetEnabled(false);
    }

    auto mainButton = GetComponent<UWidgetButtonComponent>("MainButton");
    if (mainButton)
    {
        mainButton->SetVisible(false);
        mainButton->SetEnabled(false);
    }
}

///////////////////////////////////////////////////////////////////////////////
void BP_MainMenuHUD::SwitchToConnectionMode(void)
{
    // Hide main menu widgets
    ClearMainMenuWidgets();

    // Show connection UI widgets
    auto mainPanel = GetComponent<UWidgetPanelComponent>("MainPanel");
    if (mainPanel)
    {
        mainPanel->SetVisible(true);
        mainPanel->SetEnabled(true);
    }

    auto textPlayer = GetComponent<UWidgetTextComponent>("textPlayer");
    if (textPlayer)
    {
        textPlayer->SetVisible(true);
        textPlayer->SetEnabled(true);
    }

    auto textBox = GetComponent<UWidgetTextboxComponent>("textBox");
    if (textBox)
    {
        textBox->SetVisible(true);
        textBox->SetEnabled(true);
    }

    auto searchCircle = GetComponent<UWidgetImageComponent>("Circle");
    if (searchCircle)
    {
        searchCircle->SetVisible(true);
        searchCircle->SetEnabled(true);
    }

    auto searchIcon = GetComponent<UWidgetImageComponent>("Search");
    if (searchIcon)
    {
        searchIcon->SetVisible(true);
        searchIcon->SetEnabled(true);
    }

    auto mainButton = GetComponent<UWidgetButtonComponent>("MainButton");
    if (mainButton)
    {
        // Note: MainButton is set to invisible in ShowConnectionUI
        // but we enable it for interaction
        mainButton->SetEnabled(true);
    }
}

///////////////////////////////////////////////////////////////////////////////
void BP_MainMenuHUD::SwitchToMenuMode(void)
{
    // Hide connection UI
    ClearConnectionUI();

    // Show main menu widgets
    auto playSoloButton =
        GetComponent<UWidgetButtonComponent>("PlaySoloButton");
    if (playSoloButton)
    {
        playSoloButton->SetVisible(true);
        playSoloButton->SetEnabled(true);
    }

    auto playSoloText = GetComponent<UWidgetTextComponent>("PlaySoloText");
    if (playSoloText)
    {
        playSoloText->SetVisible(true);
        playSoloText->SetEnabled(true);
    }

    auto connectButton = GetComponent<UWidgetButtonComponent>("ConnectButton");
    if (connectButton)
    {
        connectButton->SetVisible(true);
        connectButton->SetEnabled(true);
    }

    auto connectText = GetComponent<UWidgetTextComponent>("ConnectText");
    if (connectText)
    {
        connectText->SetVisible(true);
        connectText->SetEnabled(true);
    }
}

///////////////////////////////////////////////////////////////////////////////
void BP_MainMenuHUD::ShowMenuUI(void)
{
    auto m_mainImage = GetComponent<UWidgetImageComponent>("MainImage");
    if (m_mainImage)
    {
        m_mainImage->SetTexturePath("Assets/UI/Background.png");
        m_mainImage->SetSize(FVector2(1280.0f, 720.0f));
        m_mainImage->SetAlignment(UWidgetComponent::EAlignment::Center);
        m_mainImage->SetPosition(
            FVector2(640.0f, 360.0f)   // Center of screen (1280x720)
        );
    }

    // "Play Solo" Button
    auto playSoloButton =
        GetComponent<UWidgetButtonComponent>("PlaySoloButton");
    if (playSoloButton)
    {
        playSoloButton->SetColor(FColor(0.2f, 0.6f, 0.2f, 1.0f));   // Green
        playSoloButton->SetSize(FVector2(300.0f, 60.0f));
        playSoloButton->SetAlignment(UWidgetComponent::EAlignment::Center);
        playSoloButton->SetPosition(FVector2(640.0f, 320.0f));
        playSoloButton->SetVisible(true);
    }

    // "Play Solo" Text
    auto playSoloText = GetComponent<UWidgetTextComponent>("PlaySoloText");
    if (playSoloText)
    {
        playSoloText->SetText("PLAY SOLO");
        playSoloText->SetCharacterSize(FVector2(24.0f, 16.0f));
        playSoloText->SetSpacing(-12.0f);
        playSoloText->SetColor(FColor::White);
        playSoloText->SetFontPath("Assets/Font/mainFont.png");
        playSoloText->SetAlignment(UWidgetComponent::EAlignment::Center);
        playSoloText->SetPosition(FVector2(640.0f, 320.0f));
        playSoloText->SetVisible(true);
    }

    // "Connect to Server" Button
    auto connectButton = GetComponent<UWidgetButtonComponent>("ConnectButton");
    if (connectButton)
    {
        connectButton->SetColor(FColor(0.2f, 0.4f, 0.8f, 1.0f));   // Blue
        connectButton->SetSize(FVector2(300.0f, 60.0f));
        connectButton->SetAlignment(UWidgetComponent::EAlignment::Center);
        connectButton->SetPosition(FVector2(640.0f, 400.0f));
        connectButton->SetVisible(true);
    }

    // "Connect to Server" Text
    auto connectText = GetComponent<UWidgetTextComponent>("ConnectText");
    if (connectText)
    {
        connectText->SetText("CONNECT TO SERVER");
        connectText->SetCharacterSize(FVector2(20.0f, 14.0f));
        connectText->SetSpacing(-10.0f);
        connectText->SetColor(FColor::White);
        connectText->SetFontPath("Assets/Font/mainFont.png");
        connectText->SetAlignment(UWidgetComponent::EAlignment::Center);
        connectText->SetPosition(FVector2(640.0f, 400.0f));
        connectText->SetVisible(true);
    }
}

///////////////////////////////////////////////////////////////////////////////
void BP_MainMenuHUD::ShowConnectionUI(void)
{
    auto m_mainPanel = GetComponent<UWidgetPanelComponent>("MainPanel");
    if (m_mainPanel)
    {
        m_mainPanel->SetColor(FLinearColor(0, 0, 0, 150));
        m_mainPanel->SetSize(FVector2(500.0f, 150.0f));
        m_mainPanel->SetAlignment(UWidgetComponent::EAlignment::Center);
        m_mainPanel->SetPosition(FVector2(640.0f, 360.0f)
        );   // Center of screen (1280x720)
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
        m_textPlayer->SetPosition(FVector2(640.0f, 320.0f));
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
        m_textBox->SetPosition(FVector2(640.0f, 400.0f));
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
        m_searchCircle->SetPosition(FVector2(825.0f, 400.0f));
    }

    auto m_searchIcon = GetComponent<UWidgetImageComponent>("Search");
    if (m_searchIcon)
    {
        m_searchIcon->SetTexturePath("Assets/UI/Search.png");
        m_searchIcon->SetSize(FVector2(50.0f, 50.0f));
        m_searchIcon->SetColor(FLinearColor(50, 50, 50, 255));
        m_searchIcon->SetAlignment(UWidgetComponent::EAlignment::Center);
        m_searchIcon->SetPosition(FVector2(825.0f, 400.0f)

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
        m_buttonWidget->SetPosition(FVector2(825.0f, 400.0f)

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

    if (m_isInConnectionMode && !m_hasSwitched)
    {
        // Switch to connection mode
        SwitchToConnectionMode();
        m_hasSwitched = true;
    }
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_MainMenuHUD, AHUD)

}   // namespace tkd

// FVector2(640.0f, 360.0f) // Center of screen (1280x720)
// FVector2(0.0f, 0.0f)   // Top-left corner
// FVector2(0.0f, 720.0f)   // Top-right corner
// FVector2(1280.0f, 720.0f) // Bottom-right corner
// FVector2(1280.0f, 0.0f)  // Bottom-left corner
// FVector2(0.0f, 360.0f)   // Middle-left edge
// FVector2(1280.0f, 360.0f)  // Middle-right edge
// FVector2(640.0f, 720.0f)   // Bottom-center edge
// FVector2(640.0f, 0.0f)   // Top-center edge
