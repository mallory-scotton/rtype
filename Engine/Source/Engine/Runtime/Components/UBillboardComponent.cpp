///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Runtime/Components/UBillboardComponent.hpp>
#include <Engine/Assets/URessource.hpp>
#include <Engine/Runtime/Actor/AActor.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
UBillboardComponent::UBillboardComponent(const FString& name)
    : UActorComponent(name)
    , m_billboard()
    , m_localTransform(FTransform::Identity)
    , m_displayMode(EDisplayMode::StaticTexture)
    , m_flipBook(nullptr)
{}

///////////////////////////////////////////////////////////////////////////////
UBillboardPrimitive& UBillboardComponent::GetBillboard(void)
{
    return m_billboard;
}

///////////////////////////////////////////////////////////////////////////////
const UBillboardPrimitive& UBillboardComponent::GetBillboard(void) const
{
    return m_billboard;
}

///////////////////////////////////////////////////////////////////////////////
const FTransform& UBillboardComponent::GetLocalTransform(void) const
{
    return m_localTransform;
}

///////////////////////////////////////////////////////////////////////////////
void UBillboardComponent::SetLocalTransform(const FTransform& transform)
{
    m_localTransform = transform;
}

///////////////////////////////////////////////////////////////////////////////
const FilePath& UBillboardComponent::GetTexturePath(void) const
{
    return m_texturePath;
}

///////////////////////////////////////////////////////////////////////////////
void UBillboardComponent::SetTexturePath(const FilePath& texturePath)
{
    // Update the texture path
    m_texturePath = texturePath;

    // Try to get cached texture first
    m_textureHandle = URessource::GetTextureHandle(m_texturePath);

    // Set the texture on the billboard if valid
    if (m_textureHandle.IsValid())
    {
        ITexture* texture = m_textureHandle.Get();
        m_billboard.SetTexture(texture);

        // Set the texture rectangle to cover the entire texture
        FVector2u textureSize = texture->GetSize();
        m_billboard.SetTextureRect(
            FRectanglei(0, 0, textureSize.x, textureSize.y)
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
void UBillboardComponent::Render(IRenderer& renderer) const
{
    if (IsHiddenInGame()) { return; }

    // Update billboard texture and rect based on display mode
    if (m_displayMode == EDisplayMode::FlipBook && m_flipBook &&
        m_flipBook->IsLoaded())
    {
        // Use FlipBook texture and rect
        if (m_flipBook->GetSourceMode() == UFlipBook::ESourceMode::FromFiles)
        {
            FTextureHandle currentFrame = m_flipBook->GetCurrentFrame();
            if (currentFrame.IsValid())
            {
                m_billboard.SetTexture(currentFrame.Get());
                // Use full texture for individual frame files
                FVector2u textureSize = currentFrame.Get()->GetSize();
                m_billboard.SetTextureRect(
                    FRectanglei(0, 0, textureSize.x, textureSize.y)
                );
            }
        }
        else if (m_flipBook->GetSourceMode() ==
                 UFlipBook::ESourceMode::FromSprite)
        {
            FTextureHandle spriteSheet = m_flipBook->GetCurrentFrame();
            FRectangle currentRect = m_flipBook->GetCurrentFrameRect();
            if (spriteSheet.IsValid())
            {
                m_billboard.SetTexture(spriteSheet.Get());
                m_billboard.SetTextureRect(FRectanglei(
                    static_cast<int>(currentRect.left),
                    static_cast<int>(currentRect.top),
                    static_cast<int>(currentRect.width),
                    static_cast<int>(currentRect.height)
                ));
            }
        }
    }

    // Update the billboard transform with owner + local transform
    FTransform ownerTransform = GetOwner()->GetTransform();
    m_billboard.SetTransform(ownerTransform * m_localTransform);

    FRenderStates states;
    m_billboard.Draw(renderer, states);
}

///////////////////////////////////////////////////////////////////////////////
UBillboardComponent::EDisplayMode UBillboardComponent::GetDisplayMode(void
) const
{
    return m_displayMode;
}

///////////////////////////////////////////////////////////////////////////////
void UBillboardComponent::SetDisplayMode(EDisplayMode mode)
{
    m_displayMode = mode;
}

///////////////////////////////////////////////////////////////////////////////
void UBillboardComponent::SetFlipBook(UFlipBook* flipBook)
{
    if (m_flipBook != flipBook)
    {
        m_flipBook = flipBook;
        if (m_flipBook)
        {
            // Switch to FlipBook display mode
            m_displayMode = EDisplayMode::FlipBook;
            // Load frames if not already loaded
            if (!m_flipBook->IsLoaded()) { m_flipBook->LoadFrames(); }
            // Reset FlipBook to start
            m_flipBook->Restart();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
UFlipBook* UBillboardComponent::GetFlipBook(void) const { return m_flipBook; }

///////////////////////////////////////////////////////////////////////////////
void UBillboardComponent::Tick(Float32 deltaTime)
{
    if (m_displayMode == EDisplayMode::FlipBook && m_flipBook)
    {
        m_flipBook->Update(deltaTime);
    }
}

}   // namespace tkd
