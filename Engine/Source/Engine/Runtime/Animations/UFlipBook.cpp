///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Runtime/Animations/UFlipBook.hpp>
#include <Engine/Assets/URessource.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
UFlipBook::UFlipBook(
    const FString& name, ESourceMode sourceMode, EPlayMode playMode
)
    : UObject(name)
    , m_sourceMode(sourceMode)
    , m_playMode(playMode)
    , m_duration(0.0f)
    , m_currentTime(0.0f)
    , m_isLoaded(false)
    , m_currentFrameIndex(0)
    , m_hardDuration(false)
    , m_isPlaying(true)
    , m_isReversed(false)
    , m_playbackSpeed(1.0f)
    , m_hasFinished(false)
{}

///////////////////////////////////////////////////////////////////////////////
UFlipBook::ESourceMode UFlipBook::GetSourceMode(void) const
{
    return m_sourceMode;
}

///////////////////////////////////////////////////////////////////////////////
void UFlipBook::SetSourceMode(ESourceMode mode, bool resetFrames)
{
    m_sourceMode = mode;
    if (resetFrames) { ClearFrames(); }
}

///////////////////////////////////////////////////////////////////////////////
UFlipBook::EPlayMode UFlipBook::GetPlayMode(void) const { return m_playMode; }

///////////////////////////////////////////////////////////////////////////////
void UFlipBook::SetPlayMode(EPlayMode mode, bool resetTime)
{
    m_playMode = mode;
    if (resetTime)
    {
        m_currentTime = 0.0f;
        m_currentFrameIndex = 0;
        m_hasFinished = false;
        m_isReversed = false;
        m_isPlaying = true;
    }
}

///////////////////////////////////////////////////////////////////////////////
void UFlipBook::SetFramePaths(const std::vector<FilePath>& paths)
{
    if (m_sourceMode == ESourceMode::FromFiles)
    {
        m_framePaths = paths;
        m_isLoaded = false;   // Mark as not loaded since frame paths changed
        if (!m_hardDuration) { m_duration = m_framePaths.size() / 0.5f; }
    }
}

///////////////////////////////////////////////////////////////////////////////
void UFlipBook::SetFrameRects(const std::vector<FRectangle>& rects)
{
    if (m_sourceMode == ESourceMode::FromSprite)
    {
        m_frameRects = rects;
        m_isLoaded = m_spriteSheet.IsValid();
        if (!m_hardDuration) { m_duration = m_frameRects.size() / 0.5f; }
    }
}

///////////////////////////////////////////////////////////////////////////////
void UFlipBook::AddFrame(const FilePath& framePath)
{
    if (m_sourceMode == ESourceMode::FromFiles)
    {
        m_framePaths.push_back(framePath);
        m_isLoaded = false;   // Mark as not loaded since new frame is added
        if (!m_hardDuration) { m_duration = m_framePaths.size() / 0.5f; }
    }
}

///////////////////////////////////////////////////////////////////////////////
void UFlipBook::AddFrame(const FRectangle& frameRect)
{
    if (m_sourceMode == ESourceMode::FromSprite)
    {
        m_frameRects.push_back(frameRect);
        m_isLoaded = m_spriteSheet.IsValid();
        if (!m_hardDuration) { m_duration = m_frameRects.size() / 0.5f; }
    }
}

///////////////////////////////////////////////////////////////////////////////
void UFlipBook::SetDuration(Float32 duration)
{
    m_duration = duration;
    m_hardDuration = true;
}

///////////////////////////////////////////////////////////////////////////////
SizeT UFlipBook::GetFrameCount(void) const
{
    return m_sourceMode == ESourceMode::FromFiles ? m_framePaths.size()
                                                  : m_frameRects.size();
}

///////////////////////////////////////////////////////////////////////////////
const std::vector<FilePath>& UFlipBook::GetFramePaths(void) const
{
    return m_framePaths;
}

///////////////////////////////////////////////////////////////////////////////
const std::vector<FTextureHandle>& UFlipBook::GetFrames(void) const
{
    return m_frames;
}

///////////////////////////////////////////////////////////////////////////////
Float32 UFlipBook::GetDuration(void) const { return m_duration; }

///////////////////////////////////////////////////////////////////////////////
FilePath UFlipBook::GetCurrentFramePath(void) const
{
    if (m_sourceMode == ESourceMode::FromFiles)
    {
        if (m_framePaths.empty()) { return FilePath(); }
        return m_framePaths[m_currentFrameIndex % m_framePaths.size()];
    }
    else { return m_spriteSheetPath; }
}

///////////////////////////////////////////////////////////////////////////////
FilePath UFlipBook::GetFramePath(SizeT index) const
{
    if (m_sourceMode == ESourceMode::FromFiles)
    {
        if (m_framePaths.empty()) { return FilePath(); }
        return m_framePaths[index % m_framePaths.size()];
    }
    else { return m_spriteSheetPath; }
}

///////////////////////////////////////////////////////////////////////////////
FilePath UFlipBook::GetFramePathAt(Float32 time) const
{
    if (m_sourceMode == ESourceMode::FromFiles)
    {
        if (m_framePaths.empty() || m_duration <= 0.0f) { return FilePath(); }
        Float32 clampedTime = std::fmod(time, m_duration);
        SizeT frameIndex = static_cast<SizeT>(
                               (clampedTime / m_duration) * m_framePaths.size()
                           ) %
                           m_framePaths.size();
        return m_framePaths[frameIndex];
    }
    else { return m_spriteSheetPath; }
}

///////////////////////////////////////////////////////////////////////////////
FTextureHandle UFlipBook::GetCurrentFrame(void) const
{
    if (m_sourceMode == ESourceMode::FromFiles)
    {
        if (m_frames.empty()) { return FTextureHandle(); }
        return m_frames[m_currentFrameIndex % m_frames.size()];
    }
    else { return m_spriteSheet; }
}

///////////////////////////////////////////////////////////////////////////////
FTextureHandle UFlipBook::GetFrame(SizeT index) const
{
    if (m_sourceMode == ESourceMode::FromFiles)
    {
        if (m_frames.empty()) { return FTextureHandle(); }
        return m_frames[index % m_frames.size()];
    }
    else { return m_spriteSheet; }
}

///////////////////////////////////////////////////////////////////////////////
FTextureHandle UFlipBook::GetFrameAt(Float32 time) const
{
    if (m_sourceMode == ESourceMode::FromFiles)
    {
        if (m_frames.empty() || m_duration <= 0.0f)
        {
            return FTextureHandle();
        }
        Float32 clampedTime = std::fmod(time, m_duration);
        SizeT frameIndex = static_cast<SizeT>(
                               (clampedTime / m_duration) * m_framePaths.size()
                           ) %
                           m_framePaths.size();
        return m_frames[frameIndex];
    }
    else { return m_spriteSheet; }
}

///////////////////////////////////////////////////////////////////////////////
const std::vector<FRectangle>& UFlipBook::GetFrameRects(void) const
{
    return m_frameRects;
}

///////////////////////////////////////////////////////////////////////////////
FRectangle UFlipBook::GetFrameRect(SizeT index) const
{
    if (m_sourceMode == ESourceMode::FromSprite)
    {
        if (m_frameRects.empty()) { return FRectangle(); }
        return m_frameRects[index % m_frameRects.size()];
    }
    else
    {
        // For FromFiles mode, return empty rect (each file is a full texture)
        return FRectangle();
    }
}

///////////////////////////////////////////////////////////////////////////////
FRectangle UFlipBook::GetFrameRectAt(Float32 time) const
{
    if (m_sourceMode == ESourceMode::FromSprite)
    {
        if (m_frameRects.empty() || m_duration <= 0.0f)
        {
            return FRectangle();
        }
        Float32 clampedTime = std::fmod(time, m_duration);
        SizeT frameIndex = static_cast<SizeT>(
                               (clampedTime / m_duration) * m_frameRects.size()
                           ) %
                           m_frameRects.size();
        return m_frameRects[frameIndex];
    }
    else { return FRectangle(); }
}

///////////////////////////////////////////////////////////////////////////////
FRectangle UFlipBook::GetCurrentFrameRect(void) const
{
    if (m_sourceMode == ESourceMode::FromSprite)
    {
        if (m_frameRects.empty()) { return FRectangle(); }
        return m_frameRects[m_currentFrameIndex % m_frameRects.size()];
    }
    else { return FRectangle(); }
}

///////////////////////////////////////////////////////////////////////////////
Bool UFlipBook::LoadFrames(void)
{
    if (m_sourceMode == ESourceMode::FromFiles)
    {
        m_frames.clear();
        for (const FilePath& path: m_framePaths)
        {
            m_frames.push_back(URessource::LoadResource<ITexture>(path));
            if (!m_frames.back().IsValid())
            {
                m_frames.clear();
                m_isLoaded = false;
                return false;
            }
        }
    }
    else
    {
        m_spriteSheet = URessource::LoadResource<ITexture>(m_spriteSheetPath);
        if (!m_spriteSheet.IsValid())
        {
            m_isLoaded = false;
            return false;
        }
    }
    m_isLoaded = true;
    return true;
}

///////////////////////////////////////////////////////////////////////////////
void UFlipBook::UnloadFrames(void)
{
    m_frames.clear();
    m_spriteSheet.Reset();
    m_isLoaded = false;
}

///////////////////////////////////////////////////////////////////////////////
Bool UFlipBook::IsLoaded(void) const { return m_isLoaded; }

///////////////////////////////////////////////////////////////////////////////
void UFlipBook::ClearFrames(void)
{
    m_framePaths.clear();
    m_frames.clear();
    m_spriteSheet.Reset();
    m_frameRects.clear();
    m_isLoaded = false;
}

///////////////////////////////////////////////////////////////////////////////
void UFlipBook::SetCurrentTime(Float32 time)
{
    m_currentTime = std::fmod(time, m_duration);
}

///////////////////////////////////////////////////////////////////////////////
Float32 UFlipBook::GetCurrentTime(void) const { return m_currentTime; }

///////////////////////////////////////////////////////////////////////////////
void UFlipBook::Update(Float32 deltaTime)
{
    // Don't update if not playing or no frames loaded
    if (!m_isPlaying || m_duration <= 0.0f) { return; }

    if ((m_frames.empty() && m_sourceMode == ESourceMode::FromFiles) ||
        (!m_spriteSheet.IsValid() && m_sourceMode == ESourceMode::FromSprite))
    {
        return;
    }

    // Apply playback speed (can be negative for reverse playback)
    Float32 adjustedDelta = deltaTime * m_playbackSpeed;

    // Get the frame count based on source mode
    SizeT frameCount = m_sourceMode == ESourceMode::FromFiles
                           ? m_frames.size()
                           : m_frameRects.size();
    if (frameCount == 0) { return; }

    // Handle different play modes
    switch (m_playMode)
    {
    case EPlayMode::Once:
    {
        if (!m_hasFinished)
        {
            m_currentTime += adjustedDelta;

            // Handle forward playback
            if (adjustedDelta >= 0.0f && m_currentTime >= m_duration)
            {
                m_currentTime = m_duration;
                m_currentFrameIndex = frameCount - 1;
                m_hasFinished = true;
                m_isPlaying = false;
            }
            // Handle reverse playback
            else if (adjustedDelta < 0.0f && m_currentTime <= 0.0f)
            {
                m_currentTime = 0.0f;
                m_currentFrameIndex = 0;
                m_hasFinished = true;
                m_isPlaying = false;
            }
            else
            {
                // Clamp time to valid range
                m_currentTime =
                    std::max(0.0f, std::min(m_duration, m_currentTime));

                m_currentFrameIndex = static_cast<SizeT>(
                    (m_currentTime / m_duration) * frameCount
                );
                if (m_currentFrameIndex >= frameCount)
                {
                    m_currentFrameIndex = frameCount - 1;
                }
            }
        }
        break;
    }

    case EPlayMode::Loop:
    {
        m_currentTime += adjustedDelta;

        // Handle wrapping for both forward and reverse
        while (m_currentTime >= m_duration) { m_currentTime -= m_duration; }
        while (m_currentTime < 0.0f) { m_currentTime += m_duration; }

        m_currentFrameIndex =
            static_cast<SizeT>((m_currentTime / m_duration) * frameCount) %
            frameCount;
        break;
    }

    case EPlayMode::PingPong:
    {
        if (!m_isReversed)
        {
            // Playing forward
            m_currentTime += adjustedDelta;
            if (m_currentTime >= m_duration)
            {
                m_currentTime = m_duration - (m_currentTime - m_duration);
                m_isReversed = true;
                if (m_currentTime < 0.0f) { m_currentTime = 0.0f; }
            }
            else if (m_currentTime < 0.0f)
            {
                // Negative speed pushed us backwards
                m_currentTime = -m_currentTime;
                m_isReversed = true;
            }
        }
        else
        {
            // Playing backward
            m_currentTime -= adjustedDelta;
            if (m_currentTime <= 0.0f)
            {
                m_currentTime = -m_currentTime;
                m_isReversed = false;
                if (m_currentTime > m_duration) { m_currentTime = m_duration; }
            }
            else if (m_currentTime > m_duration)
            {
                // Negative speed (which is forward in reverse mode) pushed us
                // forward
                m_currentTime = m_duration - (m_currentTime - m_duration);
                m_isReversed = false;
            }
        }

        // Calculate frame index
        Float32 normalizedTime =
            std::max(0.0f, std::min(1.0f, m_currentTime / m_duration));
        m_currentFrameIndex = static_cast<SizeT>(normalizedTime * frameCount);
        if (m_currentFrameIndex >= frameCount)
        {
            m_currentFrameIndex = frameCount - 1;
        }
        break;
    }
    }
}

///////////////////////////////////////////////////////////////////////////////
void UFlipBook::Restart(void)
{
    m_currentTime = 0.0f;
    m_currentFrameIndex = 0;
    m_hasFinished = false;
    m_isReversed = false;
    m_isPlaying = true;
}

///////////////////////////////////////////////////////////////////////////////
void UFlipBook::Pause(void) { m_isPlaying = false; }

///////////////////////////////////////////////////////////////////////////////
void UFlipBook::Resume(void)
{
    if (!m_hasFinished) { m_isPlaying = true; }
}

///////////////////////////////////////////////////////////////////////////////
Bool UFlipBook::IsPlaying(void) const { return m_isPlaying; }

///////////////////////////////////////////////////////////////////////////////
Bool UFlipBook::IsPaused(void) const { return !m_isPlaying; }

///////////////////////////////////////////////////////////////////////////////
void UFlipBook::SetPlaybackSpeed(Float32 speed)
{
    m_playbackSpeed = speed;   // Allow negative values for reverse playback
}

///////////////////////////////////////////////////////////////////////////////
Float32 UFlipBook::GetPlaybackSpeed(void) const { return m_playbackSpeed; }

///////////////////////////////////////////////////////////////////////////////
Float32 UFlipBook::GetProgress(void) const
{
    if (m_duration <= 0.0f) { return 0.0f; }
    return std::max(0.0f, std::min(1.0f, m_currentTime / m_duration));
}

///////////////////////////////////////////////////////////////////////////////
Bool UFlipBook::HasFinished(void) const { return m_hasFinished; }

///////////////////////////////////////////////////////////////////////////////
void UFlipBook::SetSpriteSheetPath(const FilePath& path)
{
    m_spriteSheetPath = path;
    if (m_sourceMode == ESourceMode::FromSprite) { m_isLoaded = false; }
}

///////////////////////////////////////////////////////////////////////////////
const FilePath& UFlipBook::GetSpriteSheetPath(void) const
{
    return m_spriteSheetPath;
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(UFlipBook, UObject)

}   // namespace tkd
