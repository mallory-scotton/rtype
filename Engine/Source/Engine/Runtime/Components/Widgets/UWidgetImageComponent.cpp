///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Runtime/Components/Widgets/UWidgetImageComponent.hpp>
#include <Engine/Assets/UResourceHandle.hpp>
#include <Engine/Assets/URessource.hpp>
#include <Engine/Runtime/Actor/AActor.hpp>

#if TKD_ENGINE_CLIENT
    #include <Engine/Static/FWindowInterface.hpp>
#endif

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

        FVector2 widgetSize = GetSize();
        if (widgetSize.x <= 0.0f || widgetSize.y <= 0.0f)
        {
            widgetSize = textureSize;
            SetSize(widgetSize);
        }

        bool isFullScreen =
            (widgetSize.x >= 1920.0f && widgetSize.y >= 1080.0f);

        FVector2 targetSize;
        if (isFullScreen)
        {
#if TKD_ENGINE_CLIENT
            FVector2u windowDims = Window::GetDimensions();
            if (windowDims.x > 0 && windowDims.y > 0)
            {
                targetSize = FVector2(windowDims.x, windowDims.y);
            }
            else
#endif
            {
                float uniformScale = GetUniformScale();
                targetSize = widgetSize * uniformScale;
            }
        }
        else
        {
            float uniformScale = GetUniformScale();
            targetSize = widgetSize * uniformScale;
        }

        FVector2 scaleRatio = targetSize / textureSize;
        float aspectScale = isFullScreen
                                ? std::max(scaleRatio.x, scaleRatio.y)
                                : std::min(scaleRatio.x, scaleRatio.y);

        FVector2 finalScale(aspectScale, aspectScale);
        m_sprite.SetScale(finalScale * GetScale());

        FVector2 displayedSize = textureSize * aspectScale;

        FVector2 origin = FVector2::Zero;
        if (GetAlignment() != EAlignment::None)
        {
            int alignIndex = static_cast<int>(GetAlignment()) - 1;
            int alignX = alignIndex % 3;
            int alignY = alignIndex / 3;
            origin = FVector2(
                alignX * displayedSize.x * 0.5f,
                alignY * displayedSize.y * 0.5f
            );
        }

        FVector2 spriteOrigin = origin / (finalScale * GetScale());
        m_sprite.SetOrigin(spriteOrigin);
    }

    // Use scaled position
    m_sprite.SetPosition(GetScaledPosition());
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
