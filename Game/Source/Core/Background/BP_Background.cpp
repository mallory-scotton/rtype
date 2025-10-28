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
    , m_scrollSpeed(10.0f)
    , m_parallaxFactor(1.0f)
    , m_texturePath("Assets/Images/canard.jpg")
    , m_textureWidth(259.0f)
{
    // Add two billboard components so we can alternate them for seamless
    // horizontal tiling (A then B placed to the right)
    AddComponent<UBillboardComponent>("BC_Background_A");
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

    // Translate(FVector3(-m_scrollSpeed * deltaTime, 0.0f, 0.0f));

    // if (GetTransform().GetPosition().x <= -m_textureWidth)
    // {
    //     FTransform t = GetTransform();
    //     t.SetPosition(FVector3(250.0f, t.GetPosition().y,
    //     t.GetPosition().z)); SetTransform(t);
    // }
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

    if (m_billboardA)
    {
        m_billboardA->SetDisplayMode(
            UBillboardComponent::EDisplayMode::StaticTexture
        );
        m_billboardA->SetTexturePath(m_texturePath);
        FTransform t = m_billboardA->GetLocalTransform();
        t.SetPosition(FVector3(0.0f, 0.0f, 0.0f));
        m_billboardA->SetLocalTransform(t);
        Scale(FVector3(2.0f, 2.0f, 1.0f));
    }
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_Background, AActor)

}   // namespace tkd
