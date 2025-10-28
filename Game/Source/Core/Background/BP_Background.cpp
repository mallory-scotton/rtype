///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Core/Background/BP_Background.hpp>
#include <Engine/Runtime/Components/UBillboardComponent.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
BP_Background::BP_Background()
    : AActor("BP_Background")
    , m_scrollSpeed(30.0f)   // default scroll speed (units per second)
    , m_parallaxFactor(1.0f)
    , m_texturePath("Assets/Images/r-typesheet34.png")
{
    // Add a billboard component to render the background image
    AddComponent<UBillboardComponent>("BC_Background");
}

///////////////////////////////////////////////////////////////////////////////
void BP_Background::BeginPlay(void)
{
    // Call parent BeginPlay
    Super::BeginPlay();

    // Cache the billboard component and set texture
    auto m_billboard = GetComponent<UBillboardComponent>("BC_Background");
    if (m_billboard)
    {
        // Use a static texture by default
        m_billboard->SetDisplayMode(
            UBillboardComponent::EDisplayMode::StaticTexture
        );
        m_billboard->SetTexturePath(m_texturePath);

        // Optionally adjust local transform or size here. By default the
        // billboard primitive will use the texture size.
    }
}

///////////////////////////////////////////////////////////////////////////////
void BP_Background::Tick(Float32 deltaTime)
{
    // Move the actor from right to left by scroll speed, applying parallax
    if (deltaTime > 0.0f)
    {
        const Float32 moveAmount =
            -m_scrollSpeed * m_parallaxFactor * deltaTime;
        // Translate along X axis (negative = right-to-left)
        Translate(moveAmount, 0.0f, 0.0f);
    }

    // Call parent tick for components and interpolation
    Super::Tick(deltaTime);
}

///////////////////////////////////////////////////////////////////////////////
void BP_Background::ChangeTexture(const FilePath& newTexturePath)
{
    m_texturePath = newTexturePath;

    // Update the texture on the billboard component if it exists
    auto m_billboard = GetComponent<UBillboardComponent>("BC_Background");
    if (m_billboard) { m_billboard->SetTexturePath(m_texturePath); }
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_Background, AActor)

}   // namespace tkd
