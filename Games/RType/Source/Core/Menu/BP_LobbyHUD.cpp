///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Core/Menu/BP_LobbyHUD.hpp>

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
}

///////////////////////////////////////////////////////////////////////////////
void BP_LobbyHUD::BeginPlay(void)
{
    Super::BeginPlay();

    // Create title text
    m_titleText = AddComponent<UWidgetTextComponent>("TitleText");
    if (m_titleText)
    {
        m_titleText->SetText("LOBBY");
        m_titleText->SetPosition(FVector2(960.0f, 300.0f));
        m_titleText->SetCharacterSize(FVector2(48.0f, 48.0f));
        m_titleText->SetColor(FColor::White);
        m_titleText->SetAlignment(UWidgetComponent::EAlignment::Center);
        m_titleText->SetFontPath("Assets/Fonts/Font.ttf");
    }

    // Create ready status text
    m_readyStatusText = AddComponent<UWidgetTextComponent>("ReadyStatusText");
    if (m_readyStatusText)
    {
        m_readyStatusText->SetText("Waiting for players...");
        m_readyStatusText->SetPosition(FVector2(960.0f, 500.0f));
        m_readyStatusText->SetCharacterSize(FVector2(32.0f, 32.0f));
        m_readyStatusText->SetColor(FColor::Yellow);
        m_readyStatusText->SetAlignment(UWidgetComponent::EAlignment::Center);
        m_readyStatusText->SetFontPath("Assets/Fonts/Font.ttf");
    }

    // Create instruction text
    m_instructionText = AddComponent<UWidgetTextComponent>("InstructionText");
    if (m_instructionText)
    {
        m_instructionText->SetText("Press SPACE to toggle ready");
        m_instructionText->SetPosition(FVector2(960.0f, 700.0f));
        m_instructionText->SetCharacterSize(FVector2(20.0f, 20.0f));
        m_instructionText->SetColor(FColor(200, 200, 200));
        m_instructionText->SetAlignment(UWidgetComponent::EAlignment::Center);
        m_instructionText->SetFontPath("Assets/Fonts/Font.ttf");
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
        FString statusText =
            FString::Format("Players Ready: {}/{}", readyCount, totalCount);
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
