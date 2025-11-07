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
    , m_color(FColor::White)
{}

///////////////////////////////////////////////////////////////////////////////
void UWidgetImageComponent::SetColor(const FColor& color)
{
    m_color = color;
    m_sprite.SetColor(color);
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetImageComponent::SetColor(const FLinearColor& color)
{
    SetColor(FColor(color));
}

///////////////////////////////////////////////////////////////////////////////
const FColor& UWidgetImageComponent::GetColor() const { return m_color; }

///////////////////////////////////////////////////////////////////////////////
void UWidgetImageComponent::BeginPlay() { Super::BeginPlay(); }

///////////////////////////////////////////////////////////////////////////////
void UWidgetImageComponent::EndPlay() { Super::EndPlay(); }

///////////////////////////////////////////////////////////////////////////////
void UWidgetImageComponent::SetTexturePath(const FilePath& path)
{
    m_texturePath = path;
    m_textureHandle = URessource::GetTextureHandle(m_texturePath);

    if (m_textureHandle.IsValid())
    {
        ITexture* texture = m_textureHandle.Get();
        m_sprite.SetTexture(*texture);
        m_sprite.SetColor(m_color);

        // Set widget size to match texture's native size by default
        FVector2u textureSize = texture->GetSize();
        SetSize(FVector2(textureSize.x, textureSize.y));
    }
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetImageComponent::Tick(Float32 deltaTime)
{
    Super::Tick(deltaTime);

    if (m_textureHandle.IsValid())
    {
        FVector2u texSize = m_textureHandle.Get()->GetSize();
        FVector2 textureSize(texSize.x, texSize.y);
        FVector2 sizeBasedScale = GetSize() / textureSize;

        m_sprite.SetScale(sizeBasedScale * GetScale());
        FVector2 widgetOrigin = GetOrigin();

        FVector2 spriteOrigin = widgetOrigin / (sizeBasedScale * GetScale());
        m_sprite.SetOrigin(spriteOrigin);
    }

    m_sprite.SetPosition(GetPosition());
}

///////////////////////////////////////////////////////////////////////////////
void UWidgetImageComponent::Render(IRenderer& renderer) const
{
    if (!IsVisible() || !m_textureHandle.IsValid()) { return; }

    FRenderStates states;
    states.transform = FTransform2D::Identity;
    m_sprite.Draw(renderer, states);
}

}   // namespace tkd
