///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Audio/Interfaces.hpp>
#include <Engine/Audio/SFML/AudioListener.hpp>
#include <Engine/Config.hpp>
#include <map>
#include <mutex>
#include <vector>
#if TKD_ENGINE_CLIENT
    #include <SFML/Audio.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::SFML
///////////////////////////////////////////////////////////////////////////////
namespace tkd::SFML
{

///////////////////////////////////////////////////////////////////////////////
/// \brief Audio manager using SFML
///
///////////////////////////////////////////////////////////////////////////////
class AudioManager : public IAudioManager
{
private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Structure to hold Doppler effect state for each source
    ///
    ///////////////////////////////////////////////////////////////////////////
    struct SourceDopplerState
    {
        FVector3 lastPosition;          //<! Last position of the source
        Float32 lastDistance;           //<! Last distance from listener
        Float32 smoothedPitch = 1.0f;   //<! Smoothed pitch value
    };

private:
    ///////////////////////////////////////////////////////////////////////////
    // Class Member
    ///////////////////////////////////////////////////////////////////////////
    AudioListener m_listener;   //<! Audio listener
    std::map<FString, TSharedPtr<IAudioBuffer>>
        m_buffers;              //<! Loaded audio buffers
    std::vector<TSharedPtr<IAudioSource>>
        m_sources;              //<! Active audio sources
    std::mutex m_mutex;         //<! Mutex for thread safety
    Float32 m_speedOfSound;     //<! Speed of sound
    Float32 m_dopplerFactor;    //<! Doppler factor
    Bool m_initialized;         //<! Whether the audio manager is initialized
    std::unordered_map<IAudioSource*, SourceDopplerState>
        m_dopplerStates;        //<! Doppler states

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor.
    ///
    ///////////////////////////////////////////////////////////////////////////
    AudioManager(void);

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Initialize the audio manager.
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual bool Initialize(void) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Shutdown the audio manager.
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void Shutdown(void) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Update the audio manager.
    ///
    /// \param deltaTime Time elapsed since the last update in seconds.
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void Update(Float32 deltaTime) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Create an audio buffer
    ///
    /// \return Shared pointer to the created audio buffer
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual TSharedPtr<IAudioBuffer> CreateBuffer(void) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Create an audio source
    ///
    /// \return Shared pointer to the created audio source
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual TSharedPtr<IAudioSource> CreateSource(void) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Create an audio stream
    ///
    /// \return Shared pointer to the created audio stream
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual TSharedPtr<IAudioStream> CreateStream(void) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Create an audio effect of the specified type
    ///
    /// \param type Type of effect to create
    ///
    /// \return Shared pointer to the created effect
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual TSharedPtr<IAudioEffect>
        CreateEffect(EAudioEffectType type) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the audio listener
    ///
    /// \return Pointer to the audio listener
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual IAudioListener* GetListener(void) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Load an audio buffer from a file
    ///
    /// \param filePath Path to the audio file
    ///
    /// \return Shared pointer to the loaded audio buffer
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual TSharedPtr<IAudioBuffer>
        LoadBuffer(const FilePath& filePath) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Load audio data from an asset.
    ///
    /// \param asset Asset to load audio data from.
    ///
    /// \return True if loading was successful, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual TSharedPtr<IAudioBuffer> LoadBuffer(const UAsset* asset) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Load audio data from memory buffer.
    ///
    /// \param data Vector containing the raw audio data in memory.
    ///
    /// \return True if loading was successful, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual TSharedPtr<IAudioBuffer>
        LoadBuffer(const std::vector<Byte>& data) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Load audio data from raw memory buffer.
    ///
    /// \param data Pointer to the raw audio data in memory.
    /// \param size Size of the data buffer in bytes.
    ///
    /// \return True if loading was successful, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual TSharedPtr<IAudioBuffer>
        LoadBuffer(const Byte* data, const SizeT size) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Unload an audio buffer
    ///
    /// \param filePath The path to the audio file
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void UnloadBuffer(const FilePath& filePath) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Unload all audio buffers
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void UnloadAllBuffers(void) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Play a sound
    ///
    /// \param filePath The path to the audio file
    /// \param volume The volume of the sound (0.0 to 1.0)
    /// \param loop Whether to loop the sound
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual TSharedPtr<IAudioSource> PlaySound(
        const FilePath& filePath, Float32 volume = 1.0f, Bool loop = false
    ) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Play a sound
    ///
    /// \param data Vector containing the raw audio data in memory.
    /// \param volume The volume of the sound (0.0 to 1.0)
    /// \param loop Whether to loop the sound
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual TSharedPtr<IAudioSource> PlaySound(
        const std::vector<Byte>& data, Float32 volume = 1.0f, Bool loop = false
    ) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Play a sound
    ///
    /// \param data Pointer to the raw audio data in memory.
    /// \param size Size of the data buffer in bytes.
    /// \param volume The volume of the sound (0.0 to 1.0)
    /// \param loop Whether to loop the sound
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual TSharedPtr<IAudioSource> PlaySound(
        const Byte* data, SizeT size, Float32 volume = 1.0f, Bool loop = false
    ) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Play a sound
    ///
    /// \param asset Asset to load audio data from.
    /// \param volume The volume of the sound (0.0 to 1.0)
    /// \param loop Whether to loop the sound
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual TSharedPtr<IAudioSource> PlaySound(
        const UAsset* asset, Float32 volume = 1.0f, Bool loop = false
    ) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Play a 3D sound
    ///
    /// \param filePath The path to the audio file
    /// \param position The position of the sound in 3D space
    /// \param volume The volume of the sound (0.0 to 1.0)
    /// \param loop Whether to loop the sound
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual TSharedPtr<IAudioSource> PlaySound3D(
        const FilePath& filePath,
        const FVector3& position,
        Float32 volume = 1.0f,
        Bool loop = false
    ) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Play a 3D sound
    ///
    /// \param data Vector containing the raw audio data in memory.
    /// \param position The position of the sound in 3D space
    /// \param volume The volume of the sound (0.0 to 1.0)
    /// \param loop Whether to loop the sound
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual TSharedPtr<IAudioSource> PlaySound3D(
        const std::vector<Byte>& data,
        const FVector3& position,
        Float32 volume = 1.0f,
        Bool loop = false
    ) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Play a 3D sound
    ///
    /// \param data Pointer to the raw audio data in memory.
    /// \param size Size of the data buffer in bytes.
    /// \param position The position of the sound in 3D space
    /// \param volume The volume of the sound (0.0 to 1.0)
    /// \param loop Whether to loop the sound
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual TSharedPtr<IAudioSource> PlaySound3D(
        const Byte* data,
        SizeT size,
        const FVector3& position,
        Float32 volume = 1.0f,
        Bool loop = false
    ) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Play a 3D sound
    ///
    /// \param asset Asset to load audio data from.
    /// \param position The position of the sound in 3D space
    /// \param volume The volume of the sound (0.0 to 1.0)
    /// \param loop Whether to loop the sound
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual TSharedPtr<IAudioSource> PlaySound3D(
        const UAsset* asset,
        const FVector3& position,
        Float32 volume = 1.0f,
        Bool loop = false
    ) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the speed of sound
    ///
    /// \param speed The speed of sound to set (in units per second)
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void SetSpeedOfSound(Float32 speed) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the speed of sound
    ///
    /// \return The current speed of sound
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual Float32 GetSpeedOfSound(void) const override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the Doppler factor
    ///
    /// \param factor The Doppler factor to set
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void SetDopplerFactor(Float32 factor) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the Doppler factor
    ///
    /// \return The current Doppler factor
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual Float32 GetDopplerFactor(void) const override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get a list of available audio devices
    ///
    /// \return Vector of available audio device names
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual std::vector<FString> GetAvailableDevices(void) const override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the default audio device
    ///
    /// \return Name of the default audio device
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual FString GetDefaultDevice(void) const override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the current audio device
    ///
    /// \param deviceName The name of the audio device to set
    ///
    /// \return True if the device was successfully set, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual Bool SetDevice(const FString& deviceName) override;

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Apply Doppler effect to a 3D audio source
    ///
    /// \param source The audio source to apply the Doppler effect to
    /// \param deltaTime The time elapsed since the last update
    ///
    ///////////////////////////////////////////////////////////////////////////
    void ApplyDopplerEffect(IAudioSource* source, float deltaTime);
};

}   // namespace tkd::SFML

#endif
