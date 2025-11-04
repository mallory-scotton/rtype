///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Runtime/Components/Widgets/UWidgetImageComponent.hpp>
#include <Engine/Assets/UResourceHandle.hpp>
#include <Engine/Assets/URessource.hpp>
#include <Engine/Runtime/Actor/AActor.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
UWidgetImageComponent::UWidgetImageComponent(const FString& name)
    : UWidgetComponent(name)
    , m_sprite()
    , m_textureHandle()
    , m_texturePath()
{}

///////////////////////////////////////////////////////////////////////////////
void UWidgetImageComponent::SetTexturePath(const FilePath& path)
{
    m_texturePath = path;
    m_textureHandle = URessource::GetTextureHandle(m_texturePath);

    if (m_textureHandle.IsValid())
    {
        ITexture* texture = m_textureHandle.Get();
        m_sprite.SetTexture(*texture);

        // Optional: auto-size widget to match texture
        FVector2u textureSize = texture->GetSize();
        SetSize(FVector2(textureSize.x, textureSize.y));
    }
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetImageComponent::BeginPlay() { Super::BeginPlay(); }

///////////////////////////////////////////////////////////////////////////////
void UWidgetImageComponent::EndPlay() { Super::EndPlay(); }

///////////////////////////////////////////////////////////////////////////////
void UWidgetImageComponent::Tick(Float32 deltaTime)
{
    Super::Tick(deltaTime);

    // Update sprite position/size to match widget
    m_sprite.SetPosition(GetPosition());
    m_sprite.SetScale(
        GetSize() /
        FVector2(
            m_textureHandle.IsValid() ? m_textureHandle.Get()->GetSize().x
                                      : 1.0f,
            m_textureHandle.IsValid() ? m_textureHandle.Get()->GetSize().y
                                      : 1.0f
        )
    );
    m_sprite.SetOrigin(GetOrigin());
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetImageComponent::Render(IRenderer& renderer) const
{
    if (!IsVisible() || !m_textureHandle.IsValid()) { return; }

    // Render sprite directly in screen space (2D, no transform)
    FRenderStates states;
    states.transform =
        FTransform2D::Identity;   // No world transform, just screen position
    m_sprite.Draw(renderer, states);
}

}   // namespace tkd
