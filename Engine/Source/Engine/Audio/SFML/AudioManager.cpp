///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Audio/SFML/AudioManager.hpp>
#include <Engine/Audio/SFML.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::SFML
///////////////////////////////////////////////////////////////////////////////
namespace tkd::SFML
{

#if TKD_ENGINE_CLIENT

///////////////////////////////////////////////////////////////////////////////
AudioManager::AudioManager(void)
    : m_speedOfSound(343.3f)
    , m_dopplerFactor(1.0f)
    , m_initialized(false)
{}

///////////////////////////////////////////////////////////////////////////////
Bool AudioManager::Initialize(void)
{
    if (m_initialized) { return true; }

    m_listener.SetPosition(FVector3::Zero);
    m_listener.SetVelocity(FVector3::Zero);
    m_listener.SetOrientation(FVector3(0.0f, 0.0f, -1.0f), FVector3::UnitY);
    m_listener.SetGlobalVolume(1.0f);

    m_initialized = true;
    return true;
}

///////////////////////////////////////////////////////////////////////////////
void AudioManager::Shutdown(void)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_sources.clear();
    m_buffers.clear();
    m_initialized = false;
}

///////////////////////////////////////////////////////////////////////////////
void AudioManager::Update(Float32 deltaTime)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    // Remove stopped sources that are not looping
    m_sources.erase(
        std::remove_if(
            m_sources.begin(),
            m_sources.end(),
            [](const std::shared_ptr<IAudioSource>& source)
            {
                return source->GetState() == EAudioSourceState::Stopped &&
                       !source->IsLooping();
            }
        ),
        m_sources.end()
    );

    // Apply doppler effect manually if needed
    for (auto& source: m_sources)
    {
        if (source->IsSpatial() && source->GetDopplerFactor() > 0.0f)
        {
            ApplyDopplerEffect(source.get(), deltaTime);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
TSharedPtr<IAudioBuffer> AudioManager::CreateBuffer(void)
{
    return std::make_shared<AudioBuffer>();
}

///////////////////////////////////////////////////////////////////////////////
TSharedPtr<IAudioSource> AudioManager::CreateSource(void)
{
    auto source = std::make_shared<AudioSource>();
    std::lock_guard<std::mutex> lock(m_mutex);
    m_sources.push_back(source);
    return source;
}

///////////////////////////////////////////////////////////////////////////////
TSharedPtr<IAudioStream> AudioManager::CreateStream(void)
{
    return std::make_shared<AudioStream>();
}

///////////////////////////////////////////////////////////////////////////////
TSharedPtr<IAudioEffect> AudioManager::CreateEffect(EAudioEffectType type)
{
    return std::make_shared<AudioEffect>(type);
}

///////////////////////////////////////////////////////////////////////////////
IAudioListener* AudioManager::GetListener(void) { return &m_listener; }

///////////////////////////////////////////////////////////////////////////////
TSharedPtr<IAudioBuffer> AudioManager::LoadBuffer(const FilePath& filePath)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    // Check if the buffer is already loaded
    auto it = m_buffers.find(filePath.string());
    if (it != m_buffers.end()) { return it->second; }

    // Create and load a new buffer
    auto buffer = CreateBuffer();
    if (buffer->LoadFromFile(filePath))
    {
        m_buffers[filePath.string()] = buffer;
        return buffer;
    }

    return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
void AudioManager::UnloadBuffer(const FilePath& filePath)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_buffers.erase(filePath.string());
}

///////////////////////////////////////////////////////////////////////////////
void AudioManager::UnloadAllBuffers(void)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_buffers.clear();
}

///////////////////////////////////////////////////////////////////////////////
void AudioManager::PlaySound(
    const FilePath& filePath, Float32 volume, Bool loop
)
{
    auto buffer = LoadBuffer(filePath);
    if (!buffer) { return; }

    auto source = CreateSource();
    source->SetBuffer(buffer);
    source->SetVolume(volume);
    source->SetLooping(loop);
    source->SetSpatial(false);
    source->Play();
}

///////////////////////////////////////////////////////////////////////////////
void AudioManager::PlaySound3D(
    const FilePath& filePath,
    const FVector3& position,
    Float32 volume,
    Bool loop
)
{
    auto buffer = LoadBuffer(filePath);
    if (!buffer) { return; }

    auto source = CreateSource();
    source->SetBuffer(buffer);
    source->SetPosition(position);
    source->SetVolume(volume);
    source->SetMinDistance(1.0f);
    source->SetMaxDistance(100.0f);
    source->SetRolloffFactor(1.0f);
    source->SetLooping(loop);
    source->SetSpatial(true);
    source->Play();
}

///////////////////////////////////////////////////////////////////////////////
void AudioManager::SetSpeedOfSound(Float32 speed) { m_speedOfSound = speed; }

///////////////////////////////////////////////////////////////////////////////
Float32 AudioManager::GetSpeedOfSound(void) const { return m_speedOfSound; }

///////////////////////////////////////////////////////////////////////////////
void AudioManager::SetDopplerFactor(Float32 factor)
{
    m_dopplerFactor = factor;
}

///////////////////////////////////////////////////////////////////////////////
Float32 AudioManager::GetDopplerFactor(void) const { return m_dopplerFactor; }

///////////////////////////////////////////////////////////////////////////////
std::vector<FString> AudioManager::GetAvailableDevices(void) const
{
    auto devices = sf::SoundRecorder::getAvailableDevices();
    return std::vector<FString>(devices.begin(), devices.end());
}

///////////////////////////////////////////////////////////////////////////////
FString AudioManager::GetDefaultDevice(void) const
{
    return sf::SoundRecorder::getDefaultDevice();
}

///////////////////////////////////////////////////////////////////////////////
Bool AudioManager::SetDevice(const FString& name)
{
    // SFML does not support setting the output device for playback
    return false;
}

///////////////////////////////////////////////////////////////////////////////
void AudioManager::ApplyDopplerEffect(IAudioSource* source, Float32 deltaTime)
{
    FVector3 sourcePos = source->GetPosition();
    FVector3 sourceVel = source->GetVelocity();
    FVector3 listenerPos = m_listener.GetPosition();
    FVector3 listenerVel = m_listener.GetVelocity();

    FVector3 toSource = sourcePos - listenerPos;
    Float32 distance = toSource.Length();

    if (distance < 0.001f) { return; }

    FVector3 toSourceNorm = toSource.Normalized();

    Float32 sourceSpeed = sourceVel.x * toSourceNorm.x +
                          sourceVel.y * toSourceNorm.y +
                          sourceVel.z * toSourceNorm.z;

    Float32 listenerSpeed = listenerVel.x * toSourceNorm.x +
                            listenerVel.y * toSourceNorm.y +
                            listenerVel.z * toSourceNorm.z;

    Float32 dopplerShift =
        (m_speedOfSound - listenerSpeed) / (m_speedOfSound - sourceSpeed);

    dopplerShift = std::max(0.5f, std::min(2.0f, dopplerShift));

    Float32 finalPitch =
        dopplerShift * m_dopplerFactor * source->GetDopplerFactor();
    source->SetPitch(finalPitch);
}

#endif

}   // namespace tkd::SFML
