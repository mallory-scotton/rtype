///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Audio/SFML/AudioSource.hpp>
#include <Engine/Audio/SFML/AudioBuffer.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::SFML
///////////////////////////////////////////////////////////////////////////////
namespace tkd::SFML
{

#if TKD_ENGINE_CLIENT

///////////////////////////////////////////////////////////////////////////////
AudioSource::AudioSource(void)
    : m_spatial(true)
    , m_attenuationModel(EAttenuationModel::Inverse)
    , m_velocity(FVector3::Zero)
    , m_direction(FVector3::UnitZ)
    , m_rolloff(1.0f)
    , m_dopplerFactor(1.0f)
    , m_minDistance(0.0f)
    , m_maxDistance(100.0f)
{}

///////////////////////////////////////////////////////////////////////////////
void AudioSource::Play(void) { m_sound.play(); }

///////////////////////////////////////////////////////////////////////////////
void AudioSource::Pause(void) { m_sound.pause(); }

///////////////////////////////////////////////////////////////////////////////
void AudioSource::Stop(void) { m_sound.stop(); }

///////////////////////////////////////////////////////////////////////////////
EAudioSourceState AudioSource::GetState(void) const
{
    switch (m_sound.getStatus())
    {
    case sf::Sound::Status::Stopped: return EAudioSourceState::Stopped;
    case sf::Sound::Status::Paused : return EAudioSourceState::Paused;
    case sf::Sound::Status::Playing: return EAudioSourceState::Playing;
    default                        : return EAudioSourceState::Stopped;
    }
}

///////////////////////////////////////////////////////////////////////////////
Bool AudioSource::IsPlaying(void) const
{
    return m_sound.getStatus() == sf::Sound::Status::Playing;
}

///////////////////////////////////////////////////////////////////////////////
Bool AudioSource::IsPaused(void) const
{
    return m_sound.getStatus() == sf::Sound::Status::Paused;
}

///////////////////////////////////////////////////////////////////////////////
Bool AudioSource::IsStopped(void) const
{
    return m_sound.getStatus() == sf::Sound::Status::Stopped;
}

///////////////////////////////////////////////////////////////////////////////
void AudioSource::SetBuffer(TSharedPtr<IAudioBuffer> buffer)
{
    m_buffer = buffer;
    if (m_buffer)
    {
        try
        {
            void* sfBuffer = std::dynamic_pointer_cast<AudioBuffer>(m_buffer)
                                 ->GetNativeHandle();
            m_sound.setBuffer(*reinterpret_cast<sf::SoundBuffer*>(sfBuffer));
        }
        catch (...)
        {}
    }
}

///////////////////////////////////////////////////////////////////////////////
TSharedPtr<IAudioBuffer> AudioSource::GetBuffer(void) const
{
    return m_buffer;
}

///////////////////////////////////////////////////////////////////////////////
void AudioSource::SetVolume(Float32 volume)
{
    m_sound.setVolume(volume * 100.0f);
}

///////////////////////////////////////////////////////////////////////////////
void AudioSource::SetVolume(UInt32 volume)
{
    m_sound.setVolume(static_cast<Float32>(volume));
}

///////////////////////////////////////////////////////////////////////////////
Float32 AudioSource::GetVolume(void) const
{
    return m_sound.getVolume() / 100.0f;
}

///////////////////////////////////////////////////////////////////////////////
void AudioSource::SetPitch(Float32 pitch) { m_sound.setPitch(pitch); }

///////////////////////////////////////////////////////////////////////////////
Float32 AudioSource::GetPitch(void) const { return m_sound.getPitch(); }

///////////////////////////////////////////////////////////////////////////////
void AudioSource::SetLooping(Bool loop) { m_sound.setLoop(loop); }

///////////////////////////////////////////////////////////////////////////////
Bool AudioSource::IsLooping(void) const { return m_sound.getLoop(); }

///////////////////////////////////////////////////////////////////////////////
void AudioSource::SetPlaybackPosition(Float32 seconds)
{
    m_sound.setPlayingOffset(sf::seconds(seconds));
}

///////////////////////////////////////////////////////////////////////////////
Float32 AudioSource::GetPlaybackPosition(void) const
{
    return m_sound.getPlayingOffset().asSeconds();
}

///////////////////////////////////////////////////////////////////////////////
void AudioSource::SetPosition(const FVector3& position)
{
    m_sound.setPosition(position.x, position.y, position.z);
}

///////////////////////////////////////////////////////////////////////////////
FVector3 AudioSource::GetPosition(void) const
{
    sf::Vector3f pos = m_sound.getPosition();
    return FVector3(pos.x, pos.y, pos.z);
}

///////////////////////////////////////////////////////////////////////////////
void AudioSource::SetVelocity(const FVector3& velocity)
{
    m_velocity = velocity;
    // Not supported by SFML
}

///////////////////////////////////////////////////////////////////////////////
FVector3 AudioSource::GetVelocity(void) const { return m_velocity; }

///////////////////////////////////////////////////////////////////////////////
void AudioSource::SetDirection(const FVector3& direction)
{
    m_direction = direction;
    // Not supported by SFML
}

///////////////////////////////////////////////////////////////////////////////
FVector3 AudioSource::GetDirection(void) const { return m_direction; }

///////////////////////////////////////////////////////////////////////////////
void AudioSource::SetConeAngles(Float32, Float32)
{
    // Not supported by SFML
}

///////////////////////////////////////////////////////////////////////////////
void AudioSource::SetConeOuterGain(Float32)
{
    // Not supported by SFML
}

///////////////////////////////////////////////////////////////////////////////
void AudioSource::SetSpatial(Bool spatial)
{
    m_spatial = spatial;
    m_sound.setRelativeToListener(!m_spatial);
}

///////////////////////////////////////////////////////////////////////////////
Bool AudioSource::IsSpatial(void) const { return m_spatial; }

///////////////////////////////////////////////////////////////////////////////
void AudioSource::SetAttenuationModel(EAttenuationModel model)
{
    m_attenuationModel = model;
    switch (m_attenuationModel)
    {
    case EAttenuationModel::None   : m_sound.setAttenuation(0.0f); break;
    case EAttenuationModel::Inverse: m_sound.setAttenuation(m_rolloff); break;
    case EAttenuationModel::Linear:
        // Not directly supported by SFML
        m_sound.setAttenuation(m_rolloff * 0.5f);
        break;
    case EAttenuationModel::Exponential:
        // Not directly supported by SFML
        m_sound.setAttenuation(m_rolloff * 2.0f);
        break;
    default: m_sound.setAttenuation(m_rolloff); break;
    }
}

///////////////////////////////////////////////////////////////////////////////
EAttenuationModel AudioSource::GetAttenuationModel(void) const
{
    return m_attenuationModel;
}

///////////////////////////////////////////////////////////////////////////////
void AudioSource::SetMinDistance(Float32 distance)
{
    m_minDistance = distance;
    // Not supported by SFML
}

///////////////////////////////////////////////////////////////////////////////
Float32 AudioSource::GetMinDistance(void) const { return m_minDistance; }

///////////////////////////////////////////////////////////////////////////////
void AudioSource::SetMaxDistance(Float32 distance)
{
    m_maxDistance = distance;
    // Not supported by SFML
}

///////////////////////////////////////////////////////////////////////////////
Float32 AudioSource::GetMaxDistance(void) const { return m_maxDistance; }

///////////////////////////////////////////////////////////////////////////////
void AudioSource::SetRolloffFactor(Float32 rolloff)
{
    m_rolloff = rolloff;
    SetAttenuationModel(m_attenuationModel);
}

///////////////////////////////////////////////////////////////////////////////
Float32 AudioSource::GetRolloffFactor(void) const { return m_rolloff; }

///////////////////////////////////////////////////////////////////////////////
void AudioSource::SetDopplerFactor(Float32 factor)
{
    m_dopplerFactor = factor;
    // Not supported by SFML
}

///////////////////////////////////////////////////////////////////////////////
Float32 AudioSource::GetDopplerFactor(void) const { return m_dopplerFactor; }

#endif

}   // namespace tkd::SFML
