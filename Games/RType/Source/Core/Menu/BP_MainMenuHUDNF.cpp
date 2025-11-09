///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Core/Menu/BP_MainMenuHUDNF.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
BP_MainMenuHUDNF::BP_MainMenuHUDNF(void)
    : AHUD()
    , m_titleText(nullptr)
    , m_instructionText(nullptr)
{
    SetName("BP_MainMenuHUD");
}

///////////////////////////////////////////////////////////////////////////////
void BP_MainMenuHUDNF::BeginPlay(void)
{
    Super::BeginPlay();

    // Create title text
    m_titleText = AddComponent<UWidgetTextComponent>("TitleText");
    if (m_titleText)
    {
        m_titleText->SetText("R-TYPE");
        m_titleText->SetPosition(FVector2(960.0f, 400.0f));
        m_titleText->SetCharacterSize(FVector2(48.0f, 48.0f));
        m_titleText->SetColor(FColor::White);
        m_titleText->SetAlignment(UWidgetComponent::EAlignment::Center);
        m_titleText->SetFontPath("Assets/Fonts/Font.ttf");
    }

    // Create instruction text
    m_instructionText = AddComponent<UWidgetTextComponent>("InstructionText");
    if (m_instructionText)
    {
        m_instructionText->SetText("Press SPACE to continue");
        m_instructionText->SetPosition(FVector2(960.0f, 600.0f));
        m_instructionText->SetCharacterSize(FVector2(24.0f, 24.0f));
        m_instructionText->SetColor(FColor(200, 200, 200));
        m_instructionText->SetAlignment(UWidgetComponent::EAlignment::Center);
        m_instructionText->SetFontPath("Assets/Fonts/Font.ttf");
    }

    FLogger::Info("[MainMenuHUD] Initialized");
}

///////////////////////////////////////////////////////////////////////////////
void BP_MainMenuHUDNF::Tick(Float32 deltaTime)
{
    Super::Tick(deltaTime);

    // Make instruction text blink
    if (m_instructionText)
    {
        static Float32 blinkTimer = 0.0f;
        blinkTimer += deltaTime;

        UInt8 alpha = static_cast<UInt8>(
            (std::sin(blinkTimer * 2.0f) * 0.5f + 0.5f) * 255.0f
        );
        m_instructionText->SetColor(FColor(200, 200, 200, alpha));
    }
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_MainMenuHUDNF, AHUD)

}   // namespace tkd
