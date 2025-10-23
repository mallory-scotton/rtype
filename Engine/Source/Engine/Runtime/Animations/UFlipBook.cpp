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
UFlipBook::UFlipBook(const FString& name)
    : UObject(name)
    , m_duration(0.0f)
    , m_currentTime(0.0f)
    , m_isLoaded(false)
    , m_currentFrameIndex(0)
    , m_hardDuration(false)
{}

///////////////////////////////////////////////////////////////////////////////
void UFlipBook::SetFramePaths(const std::vector<FilePath>& paths)
{
    m_framePaths = paths;
    m_isLoaded = false;   // Mark as not loaded since frame paths changed
    if (!m_hardDuration) { m_duration = m_framePaths.size() / 0.5f; }
}

///////////////////////////////////////////////////////////////////////////////
void UFlipBook::AddFrame(const FilePath& framePath)
{
    m_framePaths.push_back(framePath);
    m_isLoaded = false;   // Mark as not loaded since new frame is added
    if (!m_hardDuration) { m_duration = m_framePaths.size() / 0.5f; }
}

///////////////////////////////////////////////////////////////////////////////
void UFlipBook::SetDuration(Float32 duration)
{
    m_duration = duration;
    m_hardDuration = true;
}

///////////////////////////////////////////////////////////////////////////////
SizeT UFlipBook::GetFrameCount(void) const { return m_framePaths.size(); }

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
    if (m_framePaths.empty()) { return FilePath(); }
    return m_framePaths[m_currentFrameIndex % m_framePaths.size()];
}

///////////////////////////////////////////////////////////////////////////////
FilePath UFlipBook::GetFramePath(SizeT index) const
{
    if (m_framePaths.empty()) { return FilePath(); }
    return m_framePaths[index % m_framePaths.size()];
}

///////////////////////////////////////////////////////////////////////////////
FilePath UFlipBook::GetFramePathAt(Float32 time) const
{
    if (m_framePaths.empty() || m_duration <= 0.0f) { return FilePath(); }
    Float32 clampedTime = std::fmod(time, m_duration);
    SizeT frameIndex =
        static_cast<SizeT>((clampedTime / m_duration) * m_framePaths.size()) %
        m_framePaths.size();
    return m_framePaths[frameIndex];
}

///////////////////////////////////////////////////////////////////////////////
FTextureHandle UFlipBook::GetCurrentFrame(void) const
{
    if (m_frames.empty()) { return FTextureHandle(); }
    return m_frames[m_currentFrameIndex % m_frames.size()];
}

///////////////////////////////////////////////////////////////////////////////
FTextureHandle UFlipBook::GetFrame(SizeT index) const
{
    if (m_frames.empty()) { return FTextureHandle(); }
    return m_frames[index % m_frames.size()];
}

///////////////////////////////////////////////////////////////////////////////
FTextureHandle UFlipBook::GetFrameAt(Float32 time) const
{
    if (m_frames.empty() || m_duration <= 0.0f) { return FTextureHandle(); }
    Float32 clampedTime = std::fmod(time, m_duration);
    SizeT frameIndex =
        static_cast<SizeT>((clampedTime / m_duration) * m_framePaths.size()) %
        m_framePaths.size();
    return m_frames[frameIndex];
}

///////////////////////////////////////////////////////////////////////////////
Bool UFlipBook::LoadFrames(void)
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
    m_isLoaded = true;
    return true;
}

///////////////////////////////////////////////////////////////////////////////
void UFlipBook::UnloadFrames(void)
{
    m_frames.clear();
    m_isLoaded = false;
}

///////////////////////////////////////////////////////////////////////////////
Bool UFlipBook::IsLoaded(void) const { return m_isLoaded; }

///////////////////////////////////////////////////////////////////////////////
void UFlipBook::ClearFrames(void)
{
    m_framePaths.clear();
    m_frames.clear();
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
    if (m_duration <= 0.0f || m_frames.empty()) { return; }

    m_currentTime += deltaTime;

    // Update current frame index based on current time
    m_currentFrameIndex =
        static_cast<SizeT>((m_currentTime / m_duration) * m_frames.size()) %
        m_frames.size();

    // Wrap around the current time if it exceeds duration
    if (m_currentTime > m_duration)
    {
        m_currentTime = std::fmod(m_currentTime, m_duration);
    }
}

///////////////////////////////////////////////////////////////////////////////
void UFlipBook::Restart(void) { m_currentTime = 0.0f; }

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(UFlipBook, UObject)

}   // namespace tkd
