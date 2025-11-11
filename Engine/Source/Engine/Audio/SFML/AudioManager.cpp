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
TSharedPtr<IAudioBuffer>
    AudioManager::LoadBuffer(const std::vector<Byte>& data)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    // Create and load a new buffer
    auto buffer = CreateBuffer();
    if (buffer->LoadFromMemory(data)) { return buffer; }

    return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
TSharedPtr<IAudioBuffer>
    AudioManager::LoadBuffer(const Byte* data, const SizeT size)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    // Create and load a new buffer
    auto buffer = CreateBuffer();
    if (buffer->LoadFromMemory(data, size)) { return buffer; }

    return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
TSharedPtr<IAudioBuffer> AudioManager::LoadBuffer(const UAsset* asset)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (!asset) { return nullptr; }

    // Check if the buffer is already loaded
    auto it = m_buffers.find(asset->GetName());
    if (it != m_buffers.end()) { return it->second; }

    // Check if the buffer is already loaded
    it = m_buffers.find(asset->GetPath().string());
    if (it != m_buffers.end()) { return it->second; }

    // Create and load a new buffer
    auto buffer = CreateBuffer();
    if (buffer->LoadFromAsset(asset)) { return buffer; }

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
TSharedPtr<IAudioSource> AudioManager::PlaySound(
    const FilePath& filePath, Float32 volume, Bool loop
)
{
    auto buffer = LoadBuffer(filePath);
    if (!buffer) { return nullptr; }

    auto source = CreateSource();
    source->SetBuffer(buffer);
    source->SetVolume(volume);
    source->SetLooping(loop);
    source->SetSpatial(false);
    source->Play();
    return source;
}

///////////////////////////////////////////////////////////////////////////////
TSharedPtr<IAudioSource> AudioManager::PlaySound(
    const std::vector<Byte>& data, Float32 volume, Bool loop
)
{
    auto buffer = LoadBuffer(data);
    if (!buffer) { return nullptr; }

    auto source = CreateSource();
    source->SetBuffer(buffer);
    source->SetVolume(volume);
    source->SetLooping(loop);
    source->SetSpatial(false);
    source->Play();
    return source;
}

///////////////////////////////////////////////////////////////////////////////
TSharedPtr<IAudioSource> AudioManager::PlaySound(
    const Byte* data, SizeT size, Float32 volume, Bool loop
)
{
    auto buffer = LoadBuffer(data, size);
    if (!buffer) { return nullptr; }

    auto source = CreateSource();
    source->SetBuffer(buffer);
    source->SetVolume(volume);
    source->SetLooping(loop);
    source->SetSpatial(false);
    source->Play();
    return source;
}

///////////////////////////////////////////////////////////////////////////////
TSharedPtr<IAudioSource>
    AudioManager::PlaySound(const UAsset* asset, Float32 volume, Bool loop)
{
    auto buffer = LoadBuffer(asset);
    if (!buffer) { return nullptr; }

    auto source = CreateSource();
    source->SetBuffer(buffer);
    source->SetVolume(volume);
    source->SetLooping(loop);
    source->SetSpatial(false);
    source->Play();
    return source;
}

///////////////////////////////////////////////////////////////////////////////
TSharedPtr<IAudioSource> AudioManager::PlaySound3D(
    const FilePath& filePath,
    const FVector3& position,
    Float32 volume,
    Bool loop
)
{
    auto buffer = LoadBuffer(filePath);
    if (!buffer) { return nullptr; }

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
    return source;
}

///////////////////////////////////////////////////////////////////////////////
TSharedPtr<IAudioSource> AudioManager::PlaySound3D(
    const std::vector<Byte>& data,
    const FVector3& position,
    Float32 volume,
    Bool loop
)
{
    auto buffer = LoadBuffer(data);
    if (!buffer) { return nullptr; }

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
    return source;
}

///////////////////////////////////////////////////////////////////////////////
TSharedPtr<IAudioSource> AudioManager::PlaySound3D(
    const Byte* data,
    SizeT size,
    const FVector3& position,
    Float32 volume,
    Bool loop
)
{
    auto buffer = LoadBuffer(data, size);
    if (!buffer) { return nullptr; }

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
    return source;
}

///////////////////////////////////////////////////////////////////////////////
TSharedPtr<IAudioSource> AudioManager::PlaySound3D(
    const UAsset* asset, const FVector3& position, Float32 volume, Bool loop
)
{
    auto buffer = LoadBuffer(asset);
    if (!buffer) { return nullptr; }

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
    return source;
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
Bool AudioManager::SetDevice(const FString&)
{
    // SFML does not support setting the output device for playback
    return false;
}

///////////////////////////////////////////////////////////////////////////////
void AudioManager::ApplyDopplerEffect(IAudioSource* source, Float32 deltaTime)
{
    FVector3 sourcePos = source->GetPosition();
    FVector3 listenerPos = m_listener.GetPosition();

    FVector3 toSource = sourcePos - listenerPos;
    Float32 distance = toSource.Length();

    if (distance < 0.001f)
    {
        source->SetPitch(1.0f);
        return;
    }

    // Get or create doppler state for this source
    auto& state = m_dopplerStates[source];

    // Calculate velocity from position change if not explicitly set
    FVector3 sourceVel = source->GetVelocity();
    FVector3 listenerVel = m_listener.GetVelocity();

    // If velocity is zero, calculate it from position delta
    if (sourceVel.Length() < 0.001f && deltaTime > 0.0f)
    {
        sourceVel = (sourcePos - state.lastPosition) * (1.0f / deltaTime);
    }

    // Calculate radial velocity (velocity along the line connecting listener
    // and source)
    FVector3 toSourceNorm = toSource.Normalized();

    Float32 sourceRadialSpeed = sourceVel.x * toSourceNorm.x +
                                sourceVel.y * toSourceNorm.y +
                                sourceVel.z * toSourceNorm.z;

    Float32 listenerRadialSpeed = listenerVel.x * toSourceNorm.x +
                                  listenerVel.y * toSourceNorm.y +
                                  listenerVel.z * toSourceNorm.z;

    // Calculate doppler shift using the standard formula
    // f_observed = f_emitted * (c + v_listener) / (c + v_source)
    // where velocities are positive when moving towards each other
    Float32 relativeVelocity = listenerRadialSpeed - sourceRadialSpeed;
    Float32 dopplerShift =
        (m_speedOfSound + relativeVelocity) / m_speedOfSound;

    // Clamp to reasonable values to avoid extreme pitch shifts
    dopplerShift = std::max(0.5f, std::min(2.0f, dopplerShift));

    // Apply global and per-source doppler factors
    Float32 targetPitch = 1.0f + (dopplerShift - 1.0f) * m_dopplerFactor *
                                     source->GetDopplerFactor();

    // Smooth pitch transitions to avoid jarring changes
    Float32 smoothingFactor = std::min(1.0f, deltaTime * 10.0f);
    state.smoothedPitch = state.smoothedPitch * (1.0f - smoothingFactor) +
                          targetPitch * smoothingFactor;

    source->SetPitch(state.smoothedPitch);

    // Store state for next frame
    state.lastPosition = sourcePos;
    state.lastDistance = distance;
}

#endif

}   // namespace tkd::SFML
