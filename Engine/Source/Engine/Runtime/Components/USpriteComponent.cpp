///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Runtime/Components/USpriteComponent.hpp>
#include <Engine/Assets/URessource.hpp>
#include <Engine/Runtime/Actor/AActor.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
USpriteComponent::USpriteComponent(const FString& name)
    : UActorComponent(name)
    , m_sprite()
    , m_localTransform(FTransform2D::Identity)
    , m_displayMode(EDisplayMode::StaticTexture)
    , m_flipBook(nullptr)
{}

///////////////////////////////////////////////////////////////////////////////
USprite& USpriteComponent::GetSprite(void) { return m_sprite; }

///////////////////////////////////////////////////////////////////////////////
const USprite& USpriteComponent::GetSprite(void) const { return m_sprite; }

///////////////////////////////////////////////////////////////////////////////
const FTransform2D& USpriteComponent::GetLocalTransform(void) const
{
    return m_localTransform;
}

///////////////////////////////////////////////////////////////////////////////
void USpriteComponent::SetLocalTransform(const FTransform2D& transform)
{
    m_localTransform = transform;
}

///////////////////////////////////////////////////////////////////////////////
void USpriteComponent::Render(IRenderer& renderer) const
{
    if (IsHiddenInGame()) { return; }

    // Update sprite based on display mode
    if (m_displayMode == EDisplayMode::FlipBook && m_flipBook &&
        m_flipBook->IsLoaded())
    {
        // Use FlipBook texture and rect
        if (m_flipBook->GetSourceMode() == UFlipBook::ESourceMode::FromFiles)
        {
            FTextureHandle currentFrame = m_flipBook->GetCurrentFrame();
            if (currentFrame.IsValid())
            {
                m_sprite.SetTexture(*currentFrame.Get(), false);
            }
        }
        else if (m_flipBook->GetSourceMode() ==
                 UFlipBook::ESourceMode::FromSprite)
        {
            FTextureHandle spriteSheet = m_flipBook->GetCurrentFrame();
            FRectangle currentRect = m_flipBook->GetCurrentFrameRect();
            if (spriteSheet.IsValid())
            {
                m_sprite.SetTexture(*spriteSheet.Get(), false);
                m_sprite.SetTextureRect(FRectanglei(
                    static_cast<int>(currentRect.left),
                    static_cast<int>(currentRect.top),
                    static_cast<int>(currentRect.width),
                    static_cast<int>(currentRect.height)
                ));
            }
        }
    }

    FRenderStates states;
    FTransform2D ownerTransform = GetOwner()->GetTransform().To2D();
    states.transform = ownerTransform * m_localTransform;
    m_sprite.Draw(renderer, states);
}

///////////////////////////////////////////////////////////////////////////////
const FilePath& USpriteComponent::GetTexturePath(void) const
{
    return m_texturePath;
}

///////////////////////////////////////////////////////////////////////////////
void USpriteComponent::SetTexturePath(const FilePath& texturePath)
{
    // Update the texture path
    m_texturePath = texturePath;

    // Try to get cached texture first
    m_textureHandle = URessource::GetTextureHandle(m_texturePath);

    // Set the texture on the sprite if valid
    if (m_textureHandle.IsValid())
    {
        ITexture* texture = m_textureHandle.Get();
        m_sprite.SetTexture(*texture);
        FVector2u textureSize = texture->GetSize();
        m_sprite.SetOrigin(FVector2(
            textureSize.x / 2.0f, textureSize.y * m_sprite.GetScale().y / 2.0f
        ));
    }
}

///////////////////////////////////////////////////////////////////////////////
USpriteComponent::EDisplayMode USpriteComponent::GetDisplayMode(void) const
{
    return m_displayMode;
}

///////////////////////////////////////////////////////////////////////////////
void USpriteComponent::SetDisplayMode(EDisplayMode mode)
{
    m_displayMode = mode;
}

///////////////////////////////////////////////////////////////////////////////
void USpriteComponent::SetFlipBook(UFlipBook* flipBook)
{
    m_flipBook = flipBook;
    if (m_flipBook)
    {
        m_displayMode = EDisplayMode::FlipBook;
        // Load frames if not already loaded
        if (!m_flipBook->IsLoaded()) { m_flipBook->LoadFrames(); }
    }
}

///////////////////////////////////////////////////////////////////////////////
UFlipBook* USpriteComponent::GetFlipBook(void) const { return m_flipBook; }

///////////////////////////////////////////////////////////////////////////////
void USpriteComponent::Update(Float32 deltaTime)
{
    if (m_displayMode == EDisplayMode::FlipBook && m_flipBook)
    {
        m_flipBook->Update(deltaTime);
    }
}

}   // namespace tkd
