///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Core/Background/BP_Background.hpp>
#include <Engine/Assets/URessource.hpp>
#include <Engine/Runtime/Components/UBillboardComponent.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
BP_Background::BP_Background()
    : AActor("BP_Background")
    // Slow default scroll speed so background isn't too fast
    , m_scrollSpeed(1.0f)
    , m_parallaxFactor(0.25f)
    , m_texturePath("Assets/Images/bg-back.png")
    , m_textureWidth(272.0f)
    , m_scaleX(3.0f)
    , m_worldWidth(m_textureWidth / 32.0f * m_scaleX)
    , m_texturePathPlanet("Assets/Images/bg-planet.png")
    , m_texturePathStars("Assets/Images/bg-stars.png")
    , m_textureWidthPlanet(272.0f)
    , m_textureWidthStars(272.0f)
    , m_worldWidthPlanet(m_textureWidthPlanet / 32.0f * m_scaleX)
    , m_worldWidthStars(m_textureWidthStars / 32.0f * m_scaleX)
    , m_parallaxPlanetFactor(1.0f)
    , m_parallaxStarsFactor(0.6f)
{
    // Add two billboard components so we can alternate them for seamless
    // horizontal tiling (A then B placed to the right)
    AddComponent<UBillboardComponent>("BC_Background_A");
    AddComponent<UBillboardComponent>("BC_Background_B");
    // Planet layer billboards
    AddComponent<UBillboardComponent>("BC_Background_P_A");
    // Stars layer billboards
    AddComponent<UBillboardComponent>("BC_Background_S_A");
    AddComponent<UBillboardComponent>("BC_Background_S_B");
}

///////////////////////////////////////////////////////////////////////////////
void BP_Background::BeginPlay(void)
{
    // Call parent BeginPlay
    Super::BeginPlay();

    // Configure billboards and compute texture width for wrapping
    SetupBillboards();
}

///////////////////////////////////////////////////////////////////////////////
void BP_Background::Tick(Float32 deltaTime)
{
    // Call parent tick for components and interpolation
    Super::Tick(deltaTime);

    // Move each billboard left and wrap when it has moved past one texture
    // width so the two billboards can loop seamlessly.
    auto bbA = GetComponent<UBillboardComponent>("BC_Background_A");
    auto bbB = GetComponent<UBillboardComponent>("BC_Background_B");

    const float move = m_scrollSpeed * m_parallaxFactor * deltaTime;

    if (bbA)
    {
        FTransform t = bbA->GetLocalTransform();
        FVector3 p = t.GetPosition();
        p.x -= move;
        // If this billboard has moved fully left past one texture, jump it
        if (p.x <= -m_worldWidth) { p.x += m_worldWidth * 2.0f; }
        t.SetPosition(p);
        bbA->SetLocalTransform(t);
    }

    if (bbB)
    {
        FTransform t = bbB->GetLocalTransform();
        FVector3 p = t.GetPosition();
        p.x -= move;
        if (p.x <= -m_worldWidth) { p.x += m_worldWidth * 2.0f; }
        t.SetPosition(p);
        bbB->SetLocalTransform(t);
    }

    // Planet layer movement
    auto bbP_A = GetComponent<UBillboardComponent>("BC_Background_P_A");
    const float movePlanet =
        m_scrollSpeed * m_parallaxPlanetFactor * deltaTime;

    if (bbP_A)
    {
        FTransform t = bbP_A->GetLocalTransform();
        FVector3 p = t.GetPosition();
        p.x -= movePlanet;
        if (p.x <= -m_worldWidthPlanet) { p.x += m_worldWidthPlanet * 4.0f; }
        t.SetPosition(p);
        bbP_A->SetLocalTransform(t);
    }

    // Stars layer movement
    auto bbS_A = GetComponent<UBillboardComponent>("BC_Background_S_A");
    auto bbS_B = GetComponent<UBillboardComponent>("BC_Background_S_B");
    const float moveStars = m_scrollSpeed * m_parallaxStarsFactor * deltaTime;

    if (bbS_A)
    {
        FTransform t = bbS_A->GetLocalTransform();
        FVector3 p = t.GetPosition();
        p.x -= moveStars;
        if (p.x <= -m_worldWidthStars) { p.x += m_worldWidthStars * 2.0f; }
        t.SetPosition(p);
        bbS_A->SetLocalTransform(t);
    }

    if (bbS_B)
    {
        FTransform t = bbS_B->GetLocalTransform();
        FVector3 p = t.GetPosition();
        p.x -= moveStars;
        if (p.x <= -m_worldWidthStars) { p.x += m_worldWidthStars * 2.0f; }
        t.SetPosition(p);
        bbS_B->SetLocalTransform(t);
    }
}

///////////////////////////////////////////////////////////////////////////////
void BP_Background::ChangeTexture(const FilePath& newTexturePath)
{
    m_texturePath = newTexturePath;

    // Reconfigure billboards with the new texture
    SetupBillboards();
}

///////////////////////////////////////////////////////////////////////////////
void BP_Background::SetupBillboards(void)
{
    auto m_billboardA = GetComponent<UBillboardComponent>("BC_Background_A");
    auto m_billboardB = GetComponent<UBillboardComponent>("BC_Background_B");

    auto m_billboardP_A =
        GetComponent<UBillboardComponent>("BC_Background_P_A");

    auto m_billboardS_A =
        GetComponent<UBillboardComponent>("BC_Background_S_A");
    auto m_billboardS_B =
        GetComponent<UBillboardComponent>("BC_Background_S_B");

    if (m_billboardA)
    {
        m_billboardA->SetDisplayMode(
            UBillboardComponent::EDisplayMode::StaticTexture
        );
        m_billboardA->SetTexturePath(m_texturePath);
        FTransform t = m_billboardA->GetLocalTransform();
        t.SetPosition(FVector3(0.0f, 2.5f, -1.0f));
        t.SetScale(FVector3(m_scaleX, m_scaleX, 1.0f));
        m_billboardA->SetLocalTransform(t);
    }

    if (m_billboardB)
    {
        m_billboardB->SetDisplayMode(
            UBillboardComponent::EDisplayMode::StaticTexture
        );
        m_billboardB->SetTexturePath(m_texturePath);
        FTransform t2 = m_billboardB->GetLocalTransform();
        t2.SetPosition(FVector3(m_worldWidth, 2.5f, -1.0f));
        t2.SetScale(FVector3(m_scaleX, m_scaleX, 1.0f));
        m_billboardB->SetLocalTransform(t2);
    }

    // Planet layer setup (middle distance)
    if (m_billboardP_A)
    {
        m_billboardP_A->SetDisplayMode(
            UBillboardComponent::EDisplayMode::StaticTexture
        );
        m_billboardP_A->SetTexturePath(m_texturePathPlanet);
        FTransform tp = m_billboardP_A->GetLocalTransform();
        tp.SetPosition(FVector3(0.0f, 0.0f, -0.8f));
        tp.SetScale(FVector3(m_scaleX, m_scaleX, 1.0f));
        m_billboardP_A->SetLocalTransform(tp);
    }

    // Stars layer setup (far distance)
    if (m_billboardS_A)
    {
        m_billboardS_A->SetDisplayMode(
            UBillboardComponent::EDisplayMode::StaticTexture
        );
        m_billboardS_A->SetTexturePath(m_texturePathStars);
        FTransform ts = m_billboardS_A->GetLocalTransform();
        ts.SetPosition(FVector3(0.0f, 0.0f, -0.5f));
        ts.SetScale(FVector3(m_scaleX, m_scaleX, 1.0f));
        m_billboardS_A->SetLocalTransform(ts);
    }

    if (m_billboardS_B)
    {
        m_billboardS_B->SetDisplayMode(
            UBillboardComponent::EDisplayMode::StaticTexture
        );
        m_billboardS_B->SetTexturePath(m_texturePathStars);
        FTransform ts2 = m_billboardS_B->GetLocalTransform();
        ts2.SetPosition(FVector3(m_worldWidthStars, 0.0f, -0.5f));
        ts2.SetScale(FVector3(m_scaleX, m_scaleX, 1.0f));
        m_billboardS_B->SetLocalTransform(ts2);
    }
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_Background, AActor)

}   // namespace tkd
