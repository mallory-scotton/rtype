///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Runtime/Components/UAnimatedSpriteComponent.hpp>
#include <Engine/Assets/URessource.hpp>
#include <Engine/Runtime/Actor/AActor.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
UAnimatedSpriteComponent::UAnimatedSpriteComponent(const FString& name)
    : UActorComponent(name)
    , m_currentFrame(0)
    , m_elapsedTime(0.0f)
    , m_playbackSpeed(1.0f)
    , m_isPaused(false)
    , m_localTransform(FTransform2D::Identity)
{
    m_sprite.SetColor(FColor::White);
}

///////////////////////////////////////////////////////////////////////////////
USprite& UAnimatedSpriteComponent::GetSprite(void) { return m_sprite; }

///////////////////////////////////////////////////////////////////////////////
const USprite& UAnimatedSpriteComponent::GetSprite(void) const
{
    return m_sprite;
}

///////////////////////////////////////////////////////////////////////////////
const FTransform2D& UAnimatedSpriteComponent::GetLocalTransform(void) const
{
    return m_localTransform;
}

///////////////////////////////////////////////////////////////////////////////
void UAnimatedSpriteComponent::SetLocalTransform(const FTransform2D& transform)
{
    m_localTransform = transform;
}

///////////////////////////////////////////////////////////////////////////////
void UAnimatedSpriteComponent::Render(IRenderer& renderer) const
{
    FRenderStates states;
    FTransform2D ownerTransform = GetOwner()->GetTransform().To2D();
    states.transform = ownerTransform * m_localTransform;
    m_sprite.Draw(renderer, states);
}

///////////////////////////////////////////////////////////////////////////////
const FilePath& UAnimatedSpriteComponent::GetTexturePath(void) const
{
    return m_texturePath;
}

///////////////////////////////////////////////////////////////////////////////
void UAnimatedSpriteComponent::SetTexturePath(const FilePath& texturePath)
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

///////////////////////////////////////////////////////////////////////////////
void UAnimatedSpriteComponent::AddAnimation(const FAnimation2D& animation)
{
    m_animations[animation.GetName()] = animation;
}

///////////////////////////////////////////////////////////////////////////////
void UAnimatedSpriteComponent::CreateAnimationFromGrid(
    const FString& name,
    const FVector2i& frameSize,
    const FVector2i& startPos,
    SizeT frameCount,
    Float32 frameDuration,
    Bool isLooping
)
{
    FAnimation2D animation(name, isLooping);

    for (SizeT i = 0; i < frameCount; ++i)
    {
        Int32 x = startPos.x + (static_cast<Int32>(i) * frameSize.x);
        Int32 y = startPos.y;

        FRectanglei rect(x, y, frameSize.x, frameSize.y);
        animation.AddFrame(rect, frameDuration);
    }

    m_animations[name] = animation;
}

///////////////////////////////////////////////////////////////////////////////
void UAnimatedSpriteComponent::CreateAnimationFromGridWithWrap(
    const FString& name,
    const FVector2i& frameSize,
    const FVector2i& startPos,
    SizeT frameCount,
    SizeT columns,
    Float32 frameDuration,
    Bool isLooping
)
{
    FAnimation2D animation(name, isLooping);

    for (SizeT i = 0; i < frameCount; ++i)
    {
        SizeT col = i % columns;
        SizeT row = i / columns;

        Int32 x = startPos.x + (static_cast<Int32>(col) * frameSize.x);
        Int32 y = startPos.y + (static_cast<Int32>(row) * frameSize.y);

        FRectanglei rect(x, y, frameSize.x, frameSize.y);
        animation.AddFrame(rect, frameDuration);
    }

    m_animations[name] = animation;
}

///////////////////////////////////////////////////////////////////////////////
void UAnimatedSpriteComponent::Play(const FString& name, Bool restart)
{
    if (m_animations.contains(name))
    {
        if (m_currentAnimation != name || restart)
        {
            m_currentAnimation = name;
            m_currentFrame = 0;
            m_elapsedTime = 0.0f;
            UpdateSpriteFrame();
        }
        m_isPaused = false;
    }
}

///////////////////////////////////////////////////////////////////////////////
void UAnimatedSpriteComponent::Pause(void) { m_isPaused = true; }

///////////////////////////////////////////////////////////////////////////////
void UAnimatedSpriteComponent::Resume(void) { m_isPaused = false; }

///////////////////////////////////////////////////////////////////////////////
void UAnimatedSpriteComponent::Stop(void)
{
    m_isPaused = true;
    m_currentFrame = 0;
    m_elapsedTime = 0.0f;
    UpdateSpriteFrame();
}

///////////////////////////////////////////////////////////////////////////////
void UAnimatedSpriteComponent::SetPlaybackSpeed(Float32 speed)
{
    m_playbackSpeed = speed;
    if (m_playbackSpeed < 0.0f) { m_playbackSpeed = 0.0f; }
}

///////////////////////////////////////////////////////////////////////////////
Float32 UAnimatedSpriteComponent::GetPlaybackSpeed(void) const
{
    return m_playbackSpeed;
}

///////////////////////////////////////////////////////////////////////////////
Bool UAnimatedSpriteComponent::IsPlaying(void) const
{
    return !m_isPaused && !m_currentAnimation.IsEmpty();
}

///////////////////////////////////////////////////////////////////////////////
const FString& UAnimatedSpriteComponent::GetCurrentAnimation(void) const
{
    return m_currentAnimation;
}

///////////////////////////////////////////////////////////////////////////////
void UAnimatedSpriteComponent::Tick(Float32 deltaTime)
{
    if (!IsPlaying() || m_currentAnimation.IsEmpty()) { return; }

    if (!m_animations.contains(m_currentAnimation)) { return; }

    const FAnimation2D& anim = m_animations[m_currentAnimation];
    if (anim.GetFrameCount() == 0) { return; }

    // Update elapsed time
    m_elapsedTime += deltaTime * m_playbackSpeed;

    // Get current frame
    const FAnimation2DFrame& frame = anim.GetFrame(m_currentFrame);

    // Check if we need to advance to next frame
    if (m_elapsedTime >= frame.duration)
    {
        m_elapsedTime -= frame.duration;
        m_currentFrame++;

        // Check if animation is complete
        if (m_currentFrame >= anim.GetFrameCount())
        {
            if (anim.IsLooping())
            {
                m_currentFrame = 0;
                this->Emit<Events::AnimationLoop>({ m_currentAnimation });
            }
            else
            {
                m_currentFrame = anim.GetFrameCount() - 1;
                m_isPaused = true;
                this->Emit<Events::AnimationComplete>({ m_currentAnimation });
            }
        }

        UpdateSpriteFrame();
    }
}

///////////////////////////////////////////////////////////////////////////////
void UAnimatedSpriteComponent::ClearAnimations(void)
{
    m_animations.clear();
    m_currentAnimation.Clear();
    m_currentFrame = 0;
    m_elapsedTime = 0.0f;
    m_isPaused = true;
    m_sprite.SetTextureRect(FRectanglei(0, 0, 0, 0));
}

///////////////////////////////////////////////////////////////////////////////
void UAnimatedSpriteComponent::UpdateSpriteFrame(void)
{
    if (!m_animations.contains(m_currentAnimation)) { return; }

    const FAnimation2D& anim = m_animations[m_currentAnimation];
    if (m_currentFrame >= anim.GetFrameCount()) { return; }

    const FAnimation2DFrame& frame = anim.GetFrame(m_currentFrame);
    m_sprite.SetTextureRect(frame.textureRect);
    m_sprite.SetOrigin(
        frame.textureRect.width / 2.0f, frame.textureRect.height / 2.0f
    );
}

}   // namespace tkd
