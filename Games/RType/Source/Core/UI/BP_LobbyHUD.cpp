///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Core/UI/BP_LobbyHUD.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
BP_LobbyHUD::BP_LobbyHUD(void)
    : AHUD()
    , m_titleText(nullptr)
    , m_readyStatusText(nullptr)
    , m_instructionText(nullptr)
{
    SetName("BP_LobbyHUD");

    // Background
    AddComponent<UWidgetImageComponent>("BackgroundImage");
}

///////////////////////////////////////////////////////////////////////////////
void BP_LobbyHUD::BeginPlay(void)
{
    Super::BeginPlay();

    // Setup background
    auto backgroundImage =
        GetComponent<UWidgetImageComponent>("BackgroundImage");
    if (backgroundImage)
    {
        backgroundImage->SetTexturePath("Assets/UI/Background.png");
        backgroundImage->SetSize(FVector2(1280.0f, 720.0f));
        backgroundImage->SetAlignment(UWidgetComponent::EAlignment::Center);
        backgroundImage->SetPosition(FVector2(640.0f, 360.0f)
        );   // Center of screen (1280x720)
    }

    // Create title text - "LOBBY" at top center
    m_titleText = AddComponent<UWidgetTextComponent>("TitleText");
    if (m_titleText)
    {
        m_titleText->SetText("LOBBY");
        m_titleText->SetPosition(FVector2(640.0f, 150.0f));   // Top center
        m_titleText->SetCharacterSize(FVector2(48.0f, 48.0f));
        m_titleText->SetSpacing(-24.0f);   // Similar spacing to main menu
        m_titleText->SetColor(FColor::White);
        m_titleText->SetAlignment(UWidgetComponent::EAlignment::Center);
        m_titleText->SetFontPath("Assets/Font/mainFont.png");
    }

    // Create ready status text - player count centered
    m_readyStatusText = AddComponent<UWidgetTextComponent>("ReadyStatusText");
    if (m_readyStatusText)
    {
        m_readyStatusText->SetText("0/0"
        );   // Will be updated with actual counts
        m_readyStatusText->SetPosition(FVector2(640.0f, 360.0f)
        );   // Dead center
        m_readyStatusText->SetCharacterSize(FVector2(32.0f, 32.0f));
        m_readyStatusText->SetSpacing(-16.0f);
        m_readyStatusText->SetColor(FColor::Yellow);
        m_readyStatusText->SetAlignment(UWidgetComponent::EAlignment::Center);
        m_readyStatusText->SetFontPath("Assets/Font/mainFont.png");
    }

    // Create instruction text - centered below
    m_instructionText = AddComponent<UWidgetTextComponent>("InstructionText");
    if (m_instructionText)
    {
        m_instructionText->SetText("Press SPACE to toggle ready");
        m_instructionText->SetPosition(FVector2(640.0f, 500.0f)
        );   // Centered below status
        m_instructionText->SetCharacterSize(FVector2(20.0f, 20.0f));
        m_instructionText->SetSpacing(-10.0f);
        m_instructionText->SetColor(FColor(200, 200, 200));
        m_instructionText->SetAlignment(UWidgetComponent::EAlignment::Center);
        m_instructionText->SetFontPath("Assets/Font/mainFont.png");
    }

    FLogger::Info("[LobbyHUD] Initialized");
}

///////////////////////////////////////////////////////////////////////////////
void BP_LobbyHUD::Tick(Float32 deltaTime) { Super::Tick(deltaTime); }

///////////////////////////////////////////////////////////////////////////////
void BP_LobbyHUD::UpdateReadyStatus(UInt32 readyCount, UInt32 totalCount)
{
    if (m_readyStatusText)
    {
        FString statusText = FString::Format("{}/{}", readyCount, totalCount);
        m_readyStatusText->SetText(statusText);

        // Change color based on ready state
        if (readyCount == totalCount && totalCount > 0)
        {
            m_readyStatusText->SetColor(FColor::Green);
        }
        else { m_readyStatusText->SetColor(FColor::Yellow); }
    }
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_LobbyHUD, AHUD)

}   // namespace tkd
