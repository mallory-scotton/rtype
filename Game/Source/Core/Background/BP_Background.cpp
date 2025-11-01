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
    , m_parallaxFactor(1.0f)
    , m_texturePath("Assets/Images/bg-back.png")
    , m_textureWidth(272.0f)
    , m_scaleX(3.0f)
    , m_worldWidth(m_textureWidth / 32.0f * m_scaleX)
{
    // Add two billboard components so we can alternate them for seamless
    // horizontal tiling (A then B placed to the right)
    AddComponent<UBillboardComponent>("BC_Background_A");
    AddComponent<UBillboardComponent>("BC_Background_B");
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
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_Background, AActor)

}   // namespace tkd
