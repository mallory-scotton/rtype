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
        m_mainImage->SetSize(FVector2(1920.0f, 1080.0f));
        m_mainImage->SetAlignment(UWidgetComponent::EAlignment::Center);
        m_mainImage->SetPosition(
            FVector2(960.0f, 540.0f)
        );   // Center of screen (1920x1080)
    }

    auto m_mainPanel = GetComponent<UWidgetPanelComponent>("MainPanel");
    if (m_mainPanel)
    {
        m_mainPanel->SetColor(FLinearColor(0, 0, 0, 150));
        m_mainPanel->SetSize(FVector2(750.0f, 225.0f));
        m_mainPanel->SetAlignment(UWidgetComponent::EAlignment::Center);
        m_mainPanel->SetPosition(
            FVector2(960.0f, 540.0f)
        );   // Center of screen (1920x1080)
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
            FVector2(960.0f, 480.0f)
        );   // Above center (1920x1080)
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
            FVector2(960.0f, 600.0f)
        );   // Below center (1920x1080)
        m_textBox->SetMaxCharacters(16);
        m_textBox->SetMinBoxChars(16);
        m_textBox->SetMaxBoxChars(16);
        m_textBox->SetTextColor(FColor::White);
    }

    auto m_searchCircle = GetComponent<UWidgetImageComponent>("Circle");
    if (m_searchCircle)
    {
        m_searchCircle->SetTexturePath("Assets/UI/Circle.png");
        m_searchCircle->SetSize(FVector2(75.0f, 75.0f));
        m_searchCircle->SetColor(FLinearColor(100, 100, 100, 255));
        m_searchCircle->SetAlignment(UWidgetComponent::EAlignment::Center);
        m_searchCircle->SetPosition(
            FVector2(1237.5f, 600.0f)
        );   // Right of center (1920x1080)
    }

    auto m_searchIcon = GetComponent<UWidgetImageComponent>("Search");
    if (m_searchIcon)
    {
        m_searchIcon->SetTexturePath("Assets/UI/Search.png");
        m_searchIcon->SetSize(FVector2(75.0f, 75.0f));
        m_searchIcon->SetColor(FLinearColor(50, 50, 50, 255));
        m_searchIcon->SetAlignment(UWidgetComponent::EAlignment::Center);
        m_searchIcon->SetPosition(
            FVector2(1237.5f, 600.0f)
        );   // Right of center (1920x1080)
    }

    // Setup button properties
    auto m_buttonWidget = GetComponent<UWidgetButtonComponent>("MainButton");
    if (m_buttonWidget)
    {
        m_buttonWidget->SetColor(FColor::Green);
        m_buttonWidget->SetVisible(false);
        m_buttonWidget->SetSize(FVector2(75.0f, 75.0f));
        m_buttonWidget->SetAlignment(UWidgetComponent::EAlignment::Center);
        m_buttonWidget->SetPosition(
            FVector2(1237.5f, 600.0f)
        );   // Right of center (1920x1080)
        m_buttonWidget->SetOnClick(
            [m_textBox, m_searchCircle, m_searchIcon]()
            {
                std::cout << "Ip : " << m_textBox->GetText() << std::endl;
                m_searchCircle->SetSize(FVector2(67.5f, 67.5f));
                m_searchIcon->SetSize(FVector2(67.5f, 67.5f));
            }
        );
        m_buttonWidget->SetOnUnclicked(
            [m_textBox, m_searchCircle, m_searchIcon]()
            {
                m_searchCircle->SetSize(FVector2(75.0f, 75.0f));
                m_searchIcon->SetSize(FVector2(75.0f, 75.0f));
            }
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_MainMenuHUD, AHUD)

}   // namespace tkd
