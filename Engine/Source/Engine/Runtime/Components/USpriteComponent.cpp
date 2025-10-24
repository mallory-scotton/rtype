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
    m_texturePath = texturePath;

    // Try to get cached texture first
    auto textureHandle =
        URessource::GetInstance().GetTexture(m_texturePath.string());

    // If not cached, load it
    if (!textureHandle.IsValid())
    {
        textureHandle = URessource::GetInstance().LoadTexture(m_texturePath);
    }

    // Store the handle
    m_textureHandle = textureHandle;

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

}   // namespace tkd
