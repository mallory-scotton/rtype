///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Assets/UAsset.hpp>
#include <Engine/Audio/Interfaces/IAudioBuffer.hpp>
#include <Engine/Audio/Interfaces/IAudioEffect.hpp>
#include <Engine/Audio/Interfaces/IAudioListener.hpp>
#include <Engine/Audio/Interfaces/IAudioSource.hpp>
#include <Engine/Audio/Interfaces/IAudioStream.hpp>
#include <Engine/Config.hpp>
#include <Engine/Core.hpp>
#include <memory>
#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief Interface for the Audio Manager
///
///////////////////////////////////////////////////////////////////////////////
class IAudioManager
{
public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Virtual destructor.
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual ~IAudioManager() = default;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Initialize the audio manager.
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual bool Initialize(void) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Shutdown the audio manager.
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void Shutdown(void) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Update the audio manager.
    ///
    /// \param deltaTime Time elapsed since the last update in seconds.
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void Update(Float32 deltaTime) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Create an audio buffer
    ///
    /// \return Shared pointer to the created audio buffer
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual TSharedPtr<IAudioBuffer> CreateBuffer(void) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Create an audio source
    ///
    /// \return Shared pointer to the created audio source
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual TSharedPtr<IAudioSource> CreateSource(void) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Create an audio stream
    ///
    /// \return Shared pointer to the created audio stream
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual TSharedPtr<IAudioStream> CreateStream(void) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Create an audio effect of the specified type
    ///
    /// \param type Type of effect to create
    ///
    /// \return Shared pointer to the created effect
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual TSharedPtr<IAudioEffect> CreateEffect(EAudioEffectType type) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the audio listener
    ///
    /// \return Pointer to the audio listener
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual IAudioListener* GetListener(void) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Load an audio buffer from a file
    ///
    /// \param filePath Path to the audio file
    ///
    /// \return Shared pointer to the loaded audio buffer
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual TSharedPtr<IAudioBuffer> LoadBuffer(const FilePath& filePath) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Load audio data from an asset.
    ///
    /// \param asset Asset to load audio data from.
    ///
    /// \return True if loading was successful, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual TSharedPtr<IAudioBuffer> LoadBuffer(const UAsset* asset) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Load audio data from memory buffer.
    ///
    /// \param data Vector containing the raw audio data in memory.
    ///
    /// \return True if loading was successful, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual TSharedPtr<IAudioBuffer> LoadBuffer(const std::vector<Byte>& data
    ) = 0;

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
        LoadBuffer(const Byte* data, const SizeT size) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Unload an audio buffer
    ///
    /// \param filePath The path to the audio file
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void UnloadBuffer(const FilePath& filePath) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Unload all audio buffers
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void UnloadAllBuffers(void) = 0;

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
    ) = 0;

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
    ) = 0;

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
    ) = 0;

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
    ) = 0;

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
    ) = 0;

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
    ) = 0;

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
    ) = 0;

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
    ) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the speed of sound
    ///
    /// \param speed The speed of sound to set (in units per second)
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void SetSpeedOfSound(Float32 speed) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the speed of sound
    ///
    /// \return The current speed of sound
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual Float32 GetSpeedOfSound(void) const = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the Doppler factor
    ///
    /// \param factor The Doppler factor to set
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void SetDopplerFactor(Float32 factor) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the Doppler factor
    ///
    /// \return The current Doppler factor
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual Float32 GetDopplerFactor(void) const = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get a list of available audio devices
    ///
    /// \return Vector of available audio device names
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual std::vector<FString> GetAvailableDevices(void) const = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the default audio device
    ///
    /// \return Name of the default audio device
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual FString GetDefaultDevice(void) const = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the current audio device
    ///
    /// \param deviceName The name of the audio device to set
    ///
    /// \return True if the device was successfully set, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual Bool SetDevice(const FString& deviceName) = 0;
};

}   // namespace tkd
