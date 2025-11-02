///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Menus/BP_MainMenu.hpp>
#include <AC_Pointer.hpp>
#include <BP_Sword.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
BP_MainMenu::BP_MainMenu(const FString& name)
    : AActor(name)
{
    //<! Main Menu Components
    AddComponent<UBillboardComponent>("BC_SinglePlayerIconBackground");
    AddComponent<UBillboardComponent>("BC_SinglePlayerIconHighlight");
    AddComponent<UBillboardComponent>("BC_SinglePlayerIconOverlay");
    AddComponent<UBillboardComponent>("BC_MultiPlayerIconBackground");
    AddComponent<UBillboardComponent>("BC_MultiPlayerIconHighlight");
    AddComponent<UBillboardComponent>("BC_MultiPlayerIconOverlay");
    AddComponent<UBillboardComponent>("BC_CampaignIconBackground");
    AddComponent<UBillboardComponent>("BC_CampaignIconHighlight");
    AddComponent<UBillboardComponent>("BC_CampaignIconOverlay");
    AddComponent<UBillboardComponent>("BC_PartyIconBackground");
    AddComponent<UBillboardComponent>("BC_PartyIconHighlight");
    AddComponent<UBillboardComponent>("BC_PartyIconOverlay");

    //<! Sub Menu Components
    AddComponent<UBillboardComponent>("BC_SettingsIcon");
    AddComponent<UBillboardComponent>("BC_HelpIcon");
    AddComponent<UBillboardComponent>("BC_EditIcon");
    AddComponent<UBillboardComponent>("BC_ExitIcon");

    //<! Logo Component
    AddComponent<UBillboardComponent>("BC_LogoBAT");
    AddComponent<UBillboardComponent>("BC_LogoE");
    AddComponent<UBillboardComponent>("BC_LogoSaber");

    //<! Add BoxCollision for Menu Interaction
    AddComponent<UBoxCollisionComponent>("BOXC_SinglePlayerIcon");
    AddComponent<UBoxCollisionComponent>("BOXC_MultiPlayerIcon");
    AddComponent<UBoxCollisionComponent>("BOXC_CampaignIcon");
    AddComponent<UBoxCollisionComponent>("BOXC_PartyIcon");
    AddComponent<UBoxCollisionComponent>("BOXC_SettingsIcon");
    AddComponent<UBoxCollisionComponent>("BOXC_HelpIcon");
    AddComponent<UBoxCollisionComponent>("BOXC_EditIcon");
    AddComponent<UBoxCollisionComponent>("BOXC_ExitIcon");
}

///////////////////////////////////////////////////////////////////////////////
void BP_MainMenu::BeginPlay(void)
{
    // Call the Begin Play of the Super Class
    Super::BeginPlay();

    // Initialize Main Menu Icons
    float offsetZ = -5.0f;

    auto* SinglePlayerIconBackground =
        GetComponent<UBillboardComponent>("BC_SinglePlayerIconBackground");
    auto* SinglePlayerIconHighlight =
        GetComponent<UBillboardComponent>("BC_SinglePlayerIconHighlight");
    auto* SinglePlayerIconOverlay =
        GetComponent<UBillboardComponent>("BC_SinglePlayerIconOverlay");
    auto* SinglePlayerBox =
        GetComponent<UBoxCollisionComponent>("BOXC_SinglePlayerIcon");
    if (SinglePlayerIconBackground && SinglePlayerIconHighlight &&
        SinglePlayerIconOverlay && SinglePlayerBox)
    {
        FTransform transform = FTransform::Identity;
        transform.Translate(FVector3(-1.5f, 1.1f, offsetZ));
        transform.Scale(FVector3(0.1f));

        SinglePlayerIconBackground->SetTexturePath(
            "Assets/Textures/SoloButton.png"
        );
        SinglePlayerIconHighlight->SetTexturePath(
            "Assets/Textures/SoloHighlightButton.png"
        );
        SinglePlayerIconOverlay->SetTexturePath(
            "Assets/Textures/SoloButtonOverlay.png"
        );

        SinglePlayerBox->SetLocalTransform(transform);
        SinglePlayerBox->SetBoxExtent(FVector3f(4.75f, 7.8125f, 0.1f));

        SinglePlayerIconBackground->SetLocalTransform(transform);
        transform.Translate(FVector3(0.f, 0.f, 0.01f));
        SinglePlayerIconHighlight->SetLocalTransform(transform);
        transform.Translate(FVector3(0.f, 0.f, 0.01f));
        SinglePlayerIconOverlay->SetLocalTransform(transform);

        SinglePlayerIconHighlight->SetHiddenInGame(true);

        SinglePlayerBox->SetHiddenInGame(false);   //?TEMP
        SinglePlayerBox->SetShowDebug(true);       //?TEMP
        SetupHighlighting(SinglePlayerBox, "BC_SinglePlayerIconHighlight");
    }

    auto* MultiPlayerIconBackground =
        GetComponent<UBillboardComponent>("BC_MultiPlayerIconBackground");
    auto* MultiPlayerIconHighlight =
        GetComponent<UBillboardComponent>("BC_MultiPlayerIconHighlight");
    auto* MultiPlayerIconOverlay =
        GetComponent<UBillboardComponent>("BC_MultiPlayerIconOverlay");
    auto* MultiPlayerBox =
        GetComponent<UBoxCollisionComponent>("BOXC_MultiPlayerIcon");
    if (MultiPlayerIconBackground && MultiPlayerIconHighlight &&
        MultiPlayerIconOverlay && MultiPlayerBox)
    {
        FTransform transform = FTransform::Identity;
        transform.Translate(FVector3(-0.5f, 1.1f, offsetZ));
        transform.Scale(FVector3(0.1f));

        MultiPlayerIconBackground->SetTexturePath(
            "Assets/Textures/OnlineButton.png"
        );
        MultiPlayerIconHighlight->SetTexturePath(
            "Assets/Textures/OnlineHighlightButton.png"
        );
        MultiPlayerIconOverlay->SetTexturePath(
            "Assets/Textures/OnlineButtonOverlay.png"
        );

        MultiPlayerBox->SetLocalTransform(transform);
        MultiPlayerBox->SetBoxExtent(FVector3f(4.75f, 7.8125f, 0.1f));

        MultiPlayerIconBackground->SetLocalTransform(transform);
        transform.Translate(FVector3(0.f, 0.f, 0.01f));
        MultiPlayerIconHighlight->SetLocalTransform(transform);
        transform.Translate(FVector3(0.f, 0.f, 0.01f));
        MultiPlayerIconOverlay->SetLocalTransform(transform);

        MultiPlayerIconHighlight->SetHiddenInGame(true);

        MultiPlayerBox->SetHiddenInGame(false);   //?TEMP
        MultiPlayerBox->SetShowDebug(true);       //?TEMP
        SetupHighlighting(MultiPlayerBox, "BC_MultiPlayerIconHighlight");
    }

    auto* CampaignIconBackground =
        GetComponent<UBillboardComponent>("BC_CampaignIconBackground");
    auto* CampaignIconHighlight =
        GetComponent<UBillboardComponent>("BC_CampaignIconHighlight");
    auto* CampaignIconOverlay =
        GetComponent<UBillboardComponent>("BC_CampaignIconOverlay");
    auto* CampaignBox =
        GetComponent<UBoxCollisionComponent>("BOXC_CampaignIcon");
    if (CampaignIconBackground && CampaignIconHighlight &&
        CampaignIconOverlay && CampaignBox)
    {
        FTransform transform = FTransform::Identity;
        transform.Translate(FVector3(0.5f, 1.1f, offsetZ));
        transform.Scale(FVector3(0.1f));

        CampaignIconBackground->SetTexturePath(
            "Assets/Textures/CampaignButton.png"
        );
        CampaignIconHighlight->SetTexturePath(
            "Assets/Textures/CampaignHighlightButton.png"
        );
        CampaignIconOverlay->SetTexturePath(
            "Assets/Textures/CampaignButtonOverlay.png"
        );

        CampaignBox->SetLocalTransform(transform);
        CampaignBox->SetBoxExtent(FVector3f(4.75f, 7.8125f, 0.1f));

        CampaignIconBackground->SetLocalTransform(transform);
        transform.Translate(FVector3(0.f, 0.f, 0.01f));
        CampaignIconHighlight->SetLocalTransform(transform);
        transform.Translate(FVector3(0.f, 0.f, 0.01f));
        CampaignIconOverlay->SetLocalTransform(transform);

        CampaignIconHighlight->SetHiddenInGame(true);

        CampaignBox->SetHiddenInGame(false);   //?TEMP
        CampaignBox->SetShowDebug(true);       //?TEMP
        SetupHighlighting(CampaignBox, "BC_CampaignIconHighlight");
    }

    auto* PartyIconBackground =
        GetComponent<UBillboardComponent>("BC_PartyIconBackground");
    auto* PartyIconHighlight =
        GetComponent<UBillboardComponent>("BC_PartyIconHighlight");
    auto* PartyIconOverlay =
        GetComponent<UBillboardComponent>("BC_PartyIconOverlay");
    auto* PartyBox = GetComponent<UBoxCollisionComponent>("BOXC_PartyIcon");
    if (PartyIconBackground && PartyIconHighlight && PartyIconOverlay &&
        PartyBox)
    {
        FTransform transform = FTransform::Identity;
        transform.Translate(FVector3(1.5f, 1.1f, offsetZ));
        transform.Scale(FVector3(0.1f));

        PartyIconBackground->SetTexturePath(
            "Assets/Textures/LocalPartyButton.png"
        );
        PartyIconHighlight->SetTexturePath(
            "Assets/Textures/LocalPartyHighlightButton.png"
        );
        PartyIconOverlay->SetTexturePath(
            "Assets/Textures/LocalPartyButtonOverlay.png"
        );

        PartyBox->SetLocalTransform(transform);
        PartyBox->SetBoxExtent(FVector3f(4.75f, 7.8125f, 0.1f));

        PartyIconBackground->SetLocalTransform(transform);
        transform.Translate(FVector3(0.f, 0.f, 0.01f));
        PartyIconHighlight->SetLocalTransform(transform);
        transform.Translate(FVector3(0.f, 0.f, 0.01f));
        PartyIconOverlay->SetLocalTransform(transform);

        PartyIconHighlight->SetHiddenInGame(true);

        PartyBox->SetHiddenInGame(false);   //?TEMP
        PartyBox->SetShowDebug(true);       //?TEMP
        SetupHighlighting(PartyBox, "BC_PartyIconHighlight");
    }

    auto* SettingsIcon = GetComponent<UBillboardComponent>("BC_SettingsIcon");
    auto* SettingsBox =
        GetComponent<UBoxCollisionComponent>("BOXC_SettingsIcon");
    if (SettingsIcon)
    {
        FTransform transform = FTransform::Identity;
        transform.Translate(FVector3(-0.75f, 0.f, offsetZ));
        transform.Scale(FVector3(0.1f));

        SettingsIcon->SetTexturePath("Assets/Textures/OptionsButton.png");

        FTransform boxTransform = transform;
        boxTransform.Translate(FVector3(0.075f, 0.f, 0.5f));
        SettingsBox->SetLocalTransform(boxTransform);
        SettingsBox->SetBoxExtent(FVector3f(1.65625f, 1.65625f, 0.5f));

        SettingsIcon->SetLocalTransform(transform);

        SettingsBox->SetHiddenInGame(false);   //?TEMP
        SettingsBox->SetShowDebug(true);       //?TEMP
        SetupSubMenuHighlighting(
            SettingsBox,
            SettingsIcon,
            "Assets/Textures/OptionsHighlightButton.png",
            "Assets/Textures/OptionsButton.png"
        );
    }

    auto* HelpIcon = GetComponent<UBillboardComponent>("BC_HelpIcon");
    auto* HelpBox = GetComponent<UBoxCollisionComponent>("BOXC_HelpIcon");
    if (HelpIcon)
    {
        FTransform transform = FTransform::Identity;
        transform.Translate(FVector3(-0.25f, 0.f, offsetZ));
        transform.Scale(FVector3(0.1f));

        HelpIcon->SetTexturePath("Assets/Textures/HelpButton.png");

        FTransform boxTransform = transform;
        boxTransform.Translate(FVector3(0.025f, 0.f, 0.5f));
        HelpBox->SetLocalTransform(boxTransform);
        HelpBox->SetBoxExtent(FVector3f(1.65625f, 1.65625f, 0.5f));

        HelpIcon->SetLocalTransform(transform);

        HelpBox->SetHiddenInGame(false);   //?TEMP
        HelpBox->SetShowDebug(true);       //?TEMP
        SetupSubMenuHighlighting(
            HelpBox,
            HelpIcon,
            "Assets/Textures/HelpHighlightButton.png",
            "Assets/Textures/HelpButton.png"
        );
    }

    auto* EditIcon = GetComponent<UBillboardComponent>("BC_EditIcon");
    auto* EditBox = GetComponent<UBoxCollisionComponent>("BOXC_EditIcon");
    if (EditIcon)
    {
        FTransform transform = FTransform::Identity;
        transform.Translate(FVector3(0.25f, 0.f, offsetZ));
        transform.Scale(FVector3(0.1f));

        EditIcon->SetTexturePath("Assets/Textures/EditorButton.png");

        FTransform boxTransform = transform;
        boxTransform.Translate(FVector3(-0.025f, 0.f, 0.5f));
        EditBox->SetLocalTransform(boxTransform);
        EditBox->SetBoxExtent(FVector3f(1.65625f, 1.65625f, 0.5f));

        EditIcon->SetLocalTransform(transform);

        EditBox->SetHiddenInGame(false);   //?TEMP
        EditBox->SetShowDebug(true);       //?TEMP
        SetupSubMenuHighlighting(
            EditBox,
            EditIcon,
            "Assets/Textures/EditorHighlightButton.png",
            "Assets/Textures/EditorButton.png"
        );
    }

    auto* ExitIcon = GetComponent<UBillboardComponent>("BC_ExitIcon");
    auto* ExitBox = GetComponent<UBoxCollisionComponent>("BOXC_ExitIcon");
    if (ExitIcon)
    {
        FTransform transform = FTransform::Identity;
        transform.Translate(FVector3(0.75f, 0.f, offsetZ));
        transform.Scale(FVector3(0.1f));

        ExitIcon->SetTexturePath("Assets/Textures/ExitButton.png");

        FTransform boxTransform = transform;
        boxTransform.Translate(FVector3(-0.075f, 0.f, 0.5f));
        ExitBox->SetLocalTransform(boxTransform);
        ExitBox->SetBoxExtent(FVector3f(1.65625f, 1.65625f, 0.5f));

        ExitIcon->SetLocalTransform(transform);

        ExitBox->SetHiddenInGame(false);   //?TEMP
        ExitBox->SetShowDebug(true);       //?TEMP
        SetupSubMenuHighlighting(
            ExitBox,
            ExitIcon,
            "Assets/Textures/ExitHighlightButton.png",
            "Assets/Textures/ExitButton.png"
        );
    }

    auto* LogoBAT = GetComponent<UBillboardComponent>("BC_LogoBAT");
    if (LogoBAT)
    {
        FTransform transform = FTransform::Identity;
        transform.Translate(FVector3(0.f, 5.f, offsetZ));
        transform.Scale(FVector3(0.1f));

        LogoBAT->SetTexturePath("Assets/Textures/LogoBat.png");

        LogoBAT->SetLocalTransform(transform);
    }

    auto* LogoE = GetComponent<UBillboardComponent>("BC_LogoE");
    if (LogoE)
    {
        FTransform transform = FTransform::Identity;
        transform.Translate(FVector3(0.f, 5.f, offsetZ + 0.01f));
        transform.Scale(FVector3(0.1f));

        LogoE->SetTexturePath("Assets/Textures/LogoE.png");

        LogoE->SetLocalTransform(transform);
    }

    auto* LogoSaber = GetComponent<UBillboardComponent>("BC_LogoSaber");
    if (LogoSaber)
    {
        FTransform transform = FTransform::Identity;
        transform.Translate(FVector3(0.f, 4.f, offsetZ));
        transform.Scale(FVector3(0.1f));

        LogoSaber->SetTexturePath("Assets/Textures/LogoSaber.png");

        LogoSaber->SetLocalTransform(transform);
    }
}

///////////////////////////////////////////////////////////////////////////////
void BP_MainMenu::SetupHighlighting(
    UBoxCollisionComponent* box, const FString& highlightName
)
{
    if (box && box->GetCollisionSystem())
    {
        box->GetCollisionSystem()->BindOnOverlapBegin(
            box,
            [this, highlightName](const FCollisionInfo& info)
            {
                if (info.otherActor && info.otherComponent &&
                    info.otherActor->Is<BP_Sword>() &&
                    info.otherComponent->Is<AC_Pointer>())
                {
                    auto* hghl =
                        GetComponent<UBillboardComponent>(highlightName);
                    hghl->SetHiddenInGame(false);
                }
            }
        );

        box->GetCollisionSystem()->BindOnOverlapEnd(
            box,
            [this, highlightName](const FCollisionInfo& info)
            {
                if (info.otherActor && info.otherComponent &&
                    info.otherActor->Is<BP_Sword>() &&
                    info.otherComponent->Is<AC_Pointer>())
                {
                    auto* hghl =
                        GetComponent<UBillboardComponent>(highlightName);
                    hghl->SetHiddenInGame(true);
                }
            }
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
void BP_MainMenu::SetupSubMenuHighlighting(
    UBoxCollisionComponent* box,
    UBillboardComponent* billboard,
    const FilePath& highlightTexture,
    const FilePath& normalTexture
)
{
    if (box && box->GetCollisionSystem() && billboard)
    {
        box->GetCollisionSystem()->BindOnOverlapBegin(
            box,
            [billboard, highlightTexture](const FCollisionInfo& info)
            {
                if (info.otherActor && info.otherComponent &&
                    info.otherActor->Is<BP_Sword>() &&
                    info.otherComponent->Is<AC_Pointer>())
                {
                    billboard->SetTexturePath(highlightTexture);
                }
            }
        );

        box->GetCollisionSystem()->BindOnOverlapEnd(
            box,
            [billboard, normalTexture](const FCollisionInfo& info)
            {
                if (info.otherActor && info.otherComponent &&
                    info.otherActor->Is<BP_Sword>() &&
                    info.otherComponent->Is<AC_Pointer>())
                {
                    billboard->SetTexturePath(normalTexture);
                }
            }
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
void BP_MainMenu::Tick(Float32 deltaTime)
{
    // Call the Tick of the Super Class
    Super::Tick(deltaTime);
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_MainMenu, AActor)

}   // namespace tkd
