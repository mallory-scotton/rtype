///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Assets/UResourceHandle.hpp>
#include <Engine/Audio.hpp>
#include <shared_mutex>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief Audio interface for managing and utilizing the Audio API of the
/// TKD Engine
///
///////////////////////////////////////////////////////////////////////////////
class FAudioInterface final
{
private:
    ///////////////////////////////////////////////////////////////////////////
    // Class Static Member
    ///////////////////////////////////////////////////////////////////////////
    static TUniquePtr<IAudioManager> s_manager;   //<! The audio manager
    static std::shared_mutex s_mutex;             //<! Mutex for thread safety

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Initialize the audio interface
    ///
    ///////////////////////////////////////////////////////////////////////////
    static Bool Initialize(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the raw pointer of the audio manager
    ///
    /// \return The pointer of the audio manager, or nullptr if not initialized
    ///
    ///////////////////////////////////////////////////////////////////////////
    static IAudioManager* GetAudioManager(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Play a sound
    ///
    /// \param filePath The path to the audio file
    /// \param volume The volume of the sound (0.0 to 1.0)
    /// \param loop Whether to loop the sound
    ///
    ///////////////////////////////////////////////////////////////////////////
    static TSharedPtr<IAudioSource> PlaySound(
        const FilePath& filePath, Float32 volume = 1.0f, Bool loop = false
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Play a sound
    ///
    /// \param data Vector containing the raw audio data in memory.
    /// \param volume The volume of the sound (0.0 to 1.0)
    /// \param loop Whether to loop the sound
    ///
    ///////////////////////////////////////////////////////////////////////////
    static TSharedPtr<IAudioSource> PlaySound(
        const std::vector<Byte>& data, Float32 volume = 1.0f, Bool loop = false
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Play a sound
    ///
    /// \param data Pointer to the raw audio data in memory.
    /// \param size Size of the data buffer in bytes.
    /// \param volume The volume of the sound (0.0 to 1.0)
    /// \param loop Whether to loop the sound
    ///
    ///////////////////////////////////////////////////////////////////////////
    static TSharedPtr<IAudioSource> PlaySound(
        const Byte* data, SizeT size, Float32 volume = 1.0f, Bool loop = false
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Play a sound
    ///
    /// \param asset Asset to load audio data from.
    /// \param volume The volume of the sound (0.0 to 1.0)
    /// \param loop Whether to loop the sound
    ///
    ///////////////////////////////////////////////////////////////////////////
    static TSharedPtr<IAudioSource> PlaySound(
        const UAsset* asset, Float32 volume = 1.0f, Bool loop = false
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Play a sound from a resource handle
    ///
    /// \param bufferHandle Handle to the audio buffer resource
    /// \param volume The volume of the sound (0.0 to 1.0)
    /// \param loop Whether to loop the sound
    ///
    ///////////////////////////////////////////////////////////////////////////
    static TSharedPtr<IAudioSource> PlaySound(
        const FAudioBufferHandle& bufferHandle,
        Float32 volume = 1.0f,
        Bool loop = false
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Play a sound from a PAK file or resource system
    ///
    /// \param soundPath Path to the sound file (will search in PAK files)
    /// \param volume The volume of the sound (0.0 to 1.0)
    /// \param loop Whether to loop the sound
    ///
    ///////////////////////////////////////////////////////////////////////////
    static TSharedPtr<IAudioSource> PlaySoundFromPak(
        const FilePath& soundPath, Float32 volume = 1.0f, Bool loop = false
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Play a 3D sound
    ///
    /// \param filePath The path to the audio file
    /// \param position The position of the sound in 3D space
    /// \param volume The volume of the sound (0.0 to 1.0)
    /// \param loop Whether to loop the sound
    ///
    ///////////////////////////////////////////////////////////////////////////
    static TSharedPtr<IAudioSource> PlaySound3D(
        const FilePath& filePath,
        const FVector3& position,
        Float32 volume = 1.0f,
        Bool loop = false
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Play a 3D sound
    ///
    /// \param data Vector containing the raw audio data in memory.
    /// \param position The position of the sound in 3D space
    /// \param volume The volume of the sound (0.0 to 1.0)
    /// \param loop Whether to loop the sound
    ///
    ///////////////////////////////////////////////////////////////////////////
    static TSharedPtr<IAudioSource> PlaySound3D(
        const std::vector<Byte>& data,
        const FVector3& position,
        Float32 volume = 1.0f,
        Bool loop = false
    );

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
    static TSharedPtr<IAudioSource> PlaySound3D(
        const Byte* data,
        SizeT size,
        const FVector3& position,
        Float32 volume = 1.0f,
        Bool loop = false
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Play a 3D sound
    ///
    /// \param asset Asset to load audio data from.
    /// \param position The position of the sound in 3D space
    /// \param volume The volume of the sound (0.0 to 1.0)
    /// \param loop Whether to loop the sound
    ///
    ///////////////////////////////////////////////////////////////////////////
    static TSharedPtr<IAudioSource> PlaySound3D(
        const UAsset* asset,
        const FVector3& position,
        Float32 volume = 1.0f,
        Bool loop = false
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Play a 3D sound from a resource handle
    ///
    /// \param bufferHandle Handle to the audio buffer resource
    /// \param position The position of the sound in 3D space
    /// \param volume The volume of the sound (0.0 to 1.0)
    /// \param loop Whether to loop the sound
    ///
    ///////////////////////////////////////////////////////////////////////////
    static TSharedPtr<IAudioSource> PlaySound3D(
        const FAudioBufferHandle& bufferHandle,
        const FVector3& position,
        Float32 volume = 1.0f,
        Bool loop = false
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Play a 3D sound from a PAK file or resource system
    ///
    /// \param soundPath Path to the sound file (will search in PAK files)
    /// \param position The position of the sound in 3D space
    /// \param volume The volume of the sound (0.0 to 1.0)
    /// \param loop Whether to loop the sound
    ///
    ///////////////////////////////////////////////////////////////////////////
    static TSharedPtr<IAudioSource> PlaySound3DFromPak(
        const FilePath& soundPath,
        const FVector3& position,
        Float32 volume = 1.0f,
        Bool loop = false
    );
};

}   // namespace tkd

///////////////////////////////////////////////////////////////////////////////
// Alias for easier access
///////////////////////////////////////////////////////////////////////////////
using Audio = tkd::FAudioInterface;
