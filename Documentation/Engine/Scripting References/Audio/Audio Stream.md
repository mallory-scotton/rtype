# Audio Stream

## Overview

The Audio Stream system in the TKD Game Engine provides efficient streaming audio playback for longer audio content such as music tracks, ambient sounds, and voiceovers. Unlike buffered audio that loads entire sound files into memory, streaming audio processes audio data on-demand, making it ideal for large audio files and continuous playback scenarios.

## Architecture

### Core Components

The audio stream system follows an interface-based architecture:

- **`IAudioStream`**: Abstract interface defining the contract for audio stream implementations
- **`AudioStream`**: SFML-based concrete implementation providing streaming audio playback

### Interface Design

```cpp
class IAudioStream
{
public:
    virtual ~IAudioStream() = default;

    // Audio loading
    virtual bool OpenFromFile(const FilePath& filePath) = 0;
    virtual bool OpenFromMemory(const Byte* data, SizeT size) = 0;
    virtual bool OpenFromMemory(const std::vector<Byte>& data) = 0;
    virtual bool OpenFromAsset(const UAsset* asset) = 0;

    // Playback control
    virtual void Play(void) = 0;
    virtual void Pause(void) = 0;
    virtual void Stop(void) = 0;
    virtual EAudioSourceState GetState(void) const = 0;

    // Audio properties
    virtual void SetVolume(Float32 volume) = 0;
    virtual Float32 GetVolume(void) const = 0;
    virtual void SetLooping(Bool loop) = 0;
    virtual Bool IsLooping(void) const = 0;

    // Playback position
    virtual void SetPlaybackPosition(Float32 seconds) = 0;
    virtual Float32 GetPlaybackPosition(void) const = 0;
    virtual Float32 GetDuration(void) const = 0;
};
```

## Audio Loading Methods

### File-Based Loading

Audio streams can be loaded directly from audio files on disk:

```cpp
// Load from file path
bool success = stream->OpenFromFile("music/background_theme.ogg");
if (success) {
    // Audio stream is ready for playback
}
```

### Memory-Based Loading

Audio data can be streamed from memory buffers:

```cpp
// Load from raw memory buffer
const Byte* audioData = GetAudioData();
SizeT dataSize = GetAudioDataSize();
bool success = stream->OpenFromMemory(audioData, dataSize);

// Load from vector
std::vector<Byte> audioBuffer = LoadAudioIntoVector();
bool success = stream->OpenFromMemory(audioBuffer);
```

### Asset-Based Loading

Integration with the engine's asset system for packaged audio:

```cpp
// Load from engine asset
const UAsset* musicAsset = AssetManager::LoadAsset("music/main_theme");
bool success = stream->OpenFromAsset(musicAsset);
```

## Playback Control

### Basic Playback Operations

Audio streams support standard playback operations:

- **Play**: Start or resume audio streaming
- **Pause**: Temporarily halt streaming (can be resumed)
- **Stop**: Completely stop streaming (cannot be resumed from current position)

### State Management

```cpp
// Check current playback state
EAudioSourceState state = stream->GetState();

// Convenience methods
if (stream->IsPlaying()) {
    // Stream is actively playing
}
if (stream->IsPaused()) {
    // Stream is paused
}
if (stream->IsStopped()) {
    // Stream is stopped
}
```

## Audio Properties

### Volume Control

Volume can be set using normalized values (0.0 to 1.0) or percentage values (0 to 100):

```cpp
// Set volume as normalized value (0.0 = silent, 1.0 = full)
stream->SetVolume(0.75f); // 75% volume

// Set volume as percentage (0 = silent, 100 = full)
stream->SetVolume(75u);   // 75% volume

// Get current volume (always returns 0.0 to 1.0)
float currentVolume = stream->GetVolume();
```

### Looping Control

Enable or disable automatic looping of the audio stream:

```cpp
// Enable looping for continuous playback
stream->SetLooping(true);

// Check if looping is enabled
if (stream->IsLooping()) {
    // Audio will loop when it reaches the end
}
```

### Playback Position Control

Control the current playback position within the stream:

```cpp
// Seek to 30 seconds into the audio
stream->SetPlaybackPosition(30.0f);

// Get current playback position
float currentTime = stream->GetPlaybackPosition();

// Get total duration
float totalDuration = stream->GetDuration();
```

## Duration and Timing

### Stream Duration

Audio streams provide information about their total duration:

```cpp
// Get the total length of the audio stream
float duration = stream->GetDuration(); // Returns seconds

// Calculate progress percentage
float progress = (stream->GetPlaybackPosition() / stream->GetDuration()) * 100.0f;
```

### Playback Timing

Streams support precise timing control for synchronization:

```cpp
// Synchronize multiple streams
void SynchronizePlayback(AudioStream* stream1, AudioStream* stream2) {
    float position = stream1->GetPlaybackPosition();
    stream2->SetPlaybackPosition(position);
}

// Create a progress callback
void UpdateProgressBar(AudioStream* stream, UIProgressBar* progressBar) {
    float progress = stream->GetPlaybackPosition() / stream->GetDuration();
    progressBar->SetProgress(progress);
}
```

## API Reference

### IAudioStream Interface

#### Audio Loading Methods

##### `OpenFromFile()`
```cpp
virtual bool OpenFromFile(const FilePath& filePath) = 0;
```
Opens an audio stream from a file on disk.

**Parameters**:
- `filePath`: `const FilePath&` - Path to the audio file.

**Returns**: `bool` - True if the stream was opened successfully, false otherwise.

##### `OpenFromMemory(Byte*, SizeT)`
```cpp
virtual bool OpenFromMemory(const Byte* data, SizeT size) = 0;
```
Opens an audio stream from a memory buffer.

**Parameters**:
- `data`: `const Byte*` - Pointer to the audio data.
- `size`: `SizeT` - Size of the audio data in bytes.

**Returns**: `bool` - True if the stream was opened successfully, false otherwise.

##### `OpenFromMemory(vector<Byte>&)`
```cpp
virtual bool OpenFromMemory(const std::vector<Byte>& data) = 0;
```
Opens an audio stream from a vector containing audio data.

**Parameters**:
- `data`: `const std::vector<Byte>&` - Vector containing the audio data.

**Returns**: `bool` - True if the stream was opened successfully, false otherwise.

##### `OpenFromAsset()`
```cpp
virtual bool OpenFromAsset(const UAsset* asset) = 0;
```
Opens an audio stream from an engine asset.

**Parameters**:
- `asset`: `const UAsset*` - Pointer to the asset containing audio data.

**Returns**: `bool` - True if the stream was opened successfully, false otherwise.

#### Playback Control Methods

##### `Play()`
```cpp
virtual void Play(void) = 0;
```
Starts or resumes audio streaming.

##### `Pause()`
```cpp
virtual void Pause(void) = 0;
```
Pauses audio streaming. Can be resumed with `Play()`.

##### `Stop()`
```cpp
virtual void Stop(void) = 0;
```
Stops audio streaming completely. Cannot be resumed from current position.

##### `GetState()`
```cpp
virtual EAudioSourceState GetState(void) const = 0;
```
Returns the current playback state.

**Returns**: `EAudioSourceState` - Current state (Stopped, Playing, or Paused).

#### State Query Methods

##### `IsPlaying()`
```cpp
virtual Bool IsPlaying(void) const = 0;
```
Checks if the audio stream is currently playing.

**Returns**: `Bool` - True if playing, false otherwise.

##### `IsPaused()`
```cpp
virtual Bool IsPaused(void) const = 0;
```
Checks if the audio stream is currently paused.

**Returns**: `Bool` - True if paused, false otherwise.

##### `IsStopped()`
```cpp
virtual Bool IsStopped(void) const = 0;
```
Checks if the audio stream is currently stopped.

**Returns**: `Bool` - True if stopped, false otherwise.

#### Volume Control

##### `SetVolume(Float32)`
```cpp
virtual void SetVolume(Float32 volume) = 0;
```
Sets the playback volume using a normalized value.

**Parameters**:
- `volume`: `Float32` - Volume level from 0.0 (silent) to 1.0 (full volume).

##### `SetVolume(UInt32)`
```cpp
virtual void SetVolume(UInt32 volume) = 0;
```
Sets the playback volume using a percentage value.

**Parameters**:
- `volume`: `UInt32` - Volume level from 0 (silent) to 100 (full volume).

##### `GetVolume()`
```cpp
virtual Float32 GetVolume(void) const = 0;
```
Retrieves the current volume level.

**Returns**: `Float32` - Volume level from 0.0 to 1.0.

#### Looping Control

##### `SetLooping()`
```cpp
virtual void SetLooping(Bool loop) = 0;
```
Enables or disables automatic looping.

**Parameters**:
- `loop`: `Bool` - True to enable looping, false to disable.

##### `IsLooping()`
```cpp
virtual Bool IsLooping(void) const = 0;
```
Checks if looping is enabled.

**Returns**: `Bool` - True if looping is enabled, false otherwise.

#### Playback Position

##### `SetPlaybackPosition()`
```cpp
virtual void SetPlaybackPosition(Float32 seconds) = 0;
```
Sets the current playback position within the stream.

**Parameters**:
- `seconds`: `Float32` - Position in seconds from the start of the stream.

##### `GetPlaybackPosition()`
```cpp
virtual Float32 GetPlaybackPosition(void) const = 0;
```
Retrieves the current playback position.

**Returns**: `Float32` - Current position in seconds.

##### `GetDuration()`
```cpp
virtual Float32 GetDuration(void) const = 0;
```
Retrieves the total duration of the stream.

**Returns**: `Float32` - Total duration in seconds.

### AudioStream Implementation

#### Constructor
```cpp
// Default constructor - no initialization needed
AudioStream(void);
```

#### Member Variables
- `m_music`: `sf::Music` - Underlying SFML music object for streaming audio

## Usage Examples

### Basic Music Playback

```cpp
#include <Engine/Audio/SFML/AudioStream.hpp>

// Create audio stream
tkd::SFML::AudioStream* musicStream = new tkd::SFML::AudioStream();

// Load and play background music
if (musicStream->OpenFromFile("assets/music/background.ogg")) {
    musicStream->SetVolume(0.6f);  // 60% volume
    musicStream->SetLooping(true); // Loop continuously
    musicStream->Play();
}

// Control playback
if (musicStream->IsPlaying()) {
    musicStream->Pause();  // Pause music
    musicStream->Play();   // Resume music
    musicStream->Stop();   // Stop completely
}
```

### Dynamic Music System

```cpp
class MusicManager {
private:
    tkd::SFML::AudioStream* m_currentTrack;
    std::map<std::string, std::string> m_tracks;

public:
    void Initialize() {
        m_tracks["menu"] = "music/menu_theme.ogg";
        m_tracks["gameplay"] = "music/gameplay_theme.ogg";
        m_tracks["victory"] = "music/victory_fanfare.ogg";
    }

    void PlayTrack(const std::string& trackName) {
        auto it = m_tracks.find(trackName);
        if (it == m_tracks.end()) return;

        // Stop current track
        if (m_currentTrack && m_currentTrack->IsPlaying()) {
            m_currentTrack->Stop();
        }

        // Load and play new track
        if (m_currentTrack->OpenFromFile(it->second)) {
            m_currentTrack->SetLooping(true);
            m_currentTrack->Play();
        }
    }

    void FadeOut(float duration) {
        if (!m_currentTrack) return;

        float startVolume = m_currentTrack->GetVolume();
        float elapsed = 0.0f;

        while (elapsed < duration) {
            float t = elapsed / duration;
            float volume = startVolume * (1.0f - t);
            m_currentTrack->SetVolume(volume);

            // Update elapsed time
            elapsed += deltaTime;
        }

        m_currentTrack->Stop();
    }
};
```

### Streaming from Memory

```cpp
#include <Engine/Audio/SFML/AudioStream.hpp>
#include <Engine/Assets/UAsset.hpp>

// Stream audio from memory buffer
void PlayAudioFromMemory(const std::vector<Byte>& audioData) {
    tkd::SFML::AudioStream* stream = new tkd::SFML::AudioStream();

    if (stream->OpenFromMemory(audioData)) {
        stream->SetVolume(0.8f);
        stream->Play();
    }
}

// Stream audio from asset
void PlayAudioFromAsset(const UAsset* asset) {
    tkd::SFML::AudioStream* stream = new tkd::SFML::AudioStream();

    if (stream->OpenFromAsset(asset)) {
        stream->SetLooping(true);
        stream->Play();
    }
}
```

### Audio Progress Tracking

```cpp
class AudioProgressTracker {
private:
    tkd::SFML::AudioStream* m_stream;
    float m_lastPosition;

public:
    void Update() {
        if (!m_stream) return;

        float currentPosition = m_stream->GetPlaybackPosition();
        float duration = m_stream->GetDuration();

        // Check for track completion
        if (currentPosition < m_lastPosition && !m_stream->IsLooping()) {
            OnTrackCompleted();
        }

        // Update progress
        float progress = currentPosition / duration;
        UpdateProgressUI(progress);

        m_lastPosition = currentPosition;
    }

    void SeekToProgress(float progress) {
        if (!m_stream) return;

        float duration = m_stream->GetDuration();
        float targetPosition = progress * duration;
        m_stream->SetPlaybackPosition(targetPosition);
    }
};
```

### Crossfading Between Tracks

```cpp
class Crossfader {
private:
    tkd::SFML::AudioStream* m_trackA;
    tkd::SFML::AudioStream* m_trackB;
    float m_fadeTime;
    float m_fadeProgress;

public:
    void CrossfadeToTrack(tkd::SFML::AudioStream* newTrack, float fadeTime) {
        m_trackB = newTrack;
        m_fadeTime = fadeTime;
        m_fadeProgress = 0.0f;

        // Start new track at zero volume
        m_trackB->SetVolume(0.0f);
        m_trackB->Play();
    }

    void Update(float deltaTime) {
        if (!m_trackA || !m_trackB) return;

        m_fadeProgress += deltaTime;

        if (m_fadeProgress >= m_fadeTime) {
            // Fade complete - stop old track
            m_trackA->Stop();
            m_trackA = m_trackB;
            m_trackB = nullptr;
            return;
        }

        // Update volumes
        float t = m_fadeProgress / m_fadeTime;
        m_trackA->SetVolume(1.0f - t);  // Fade out
        m_trackB->SetVolume(t);         // Fade in
    }
};
```

## Performance Considerations

### Memory Usage
- Streams audio data on-demand rather than loading entire files into memory
- Minimal memory footprint compared to buffered audio
- Suitable for large audio files (music tracks, ambient sounds)

### CPU Overhead
- Streaming introduces slight CPU overhead for data processing
- SFML handles audio decoding and mixing
- Generally lower CPU usage than multiple simultaneous buffered sources

### I/O Considerations
- File-based streaming may cause slight loading delays
- Memory-based streaming eliminates I/O bottlenecks
- Asset-based streaming depends on asset loading performance

### Thread Safety
- Audio streaming occurs on dedicated audio thread
- State changes should be synchronized in multi-threaded applications
- Playback position queries are thread-safe

### Optimization Tips
- Use memory-based streaming for frequently played audio
- Pre-load critical audio streams during loading screens
- Limit simultaneous streaming sources based on hardware capabilities
- Use appropriate audio formats (Ogg Vorbis recommended for streaming)

## Integration with Game Systems

### Audio Manager Integration

```cpp
class AudioManager {
private:
    tkd::SFML::AudioStream* m_backgroundMusic;
    std::vector<tkd::SFML::AudioStream*> m_ambientStreams;

public:
    void PlayBackgroundMusic(const std::string& musicFile) {
        if (m_backgroundMusic->OpenFromFile(musicFile)) {
            m_backgroundMusic->SetVolume(0.5f);
            m_backgroundMusic->SetLooping(true);
            m_backgroundMusic->Play();
        }
    }

    void AddAmbientStream(const std::string& ambientFile) {
        auto stream = new tkd::SFML::AudioStream();
        if (stream->OpenFromFile(ambientFile)) {
            stream->SetVolume(0.3f);
            stream->SetLooping(true);
            stream->Play();
            m_ambientStreams.push_back(stream);
        }
    }

    void SetMasterVolume(float volume) {
        m_backgroundMusic->SetVolume(volume * 0.5f);  // Background at 50% of master
        for (auto stream : m_ambientStreams) {
            stream->SetVolume(volume * 0.3f);  // Ambient at 30% of master
        }
    }
};
```

### Scene-Based Audio Management

```cpp
class SceneAudioController {
private:
    tkd::SFML::AudioStream* m_sceneMusic;
    std::map<std::string, tkd::SFML::AudioStream*> m_sceneStreams;

public:
    void LoadSceneAudio(const std::string& sceneName) {
        // Load scene-specific music
        std::string musicFile = "music/scenes/" + sceneName + "_music.ogg";
        if (m_sceneMusic->OpenFromFile(musicFile)) {
            m_sceneMusic->SetLooping(true);
        }

        // Load ambient streams for scene
        LoadAmbientStreams(sceneName);
    }

    void PlaySceneAudio() {
        m_sceneMusic->Play();
        for (auto& pair : m_sceneStreams) {
            pair.second->Play();
        }
    }

    void StopSceneAudio() {
        m_sceneMusic->Stop();
        for (auto& pair : m_sceneStreams) {
            pair.second->Stop();
        }
    }
};
```

## Platform Considerations

### Client-Only Compilation
```cpp
#if TKD_ENGINE_CLIENT
    #include <SFML/Audio.hpp>
    // AudioStream implementation
#endif
```

The audio stream system is compiled only for client builds, as server applications typically don't require audio playback.

### SFML Backend
- Cross-platform audio support through SFML
- Hardware acceleration where available
- Consistent behavior across supported platforms

### Supported Audio Formats
- **Ogg Vorbis**: Recommended for streaming (good compression, quality)
- **FLAC**: Lossless compression, larger file sizes
- **WAV**: Uncompressed, largest file sizes
- **MP3**: Patent-encumbered, avoid for open-source projects

## Diagrams

### Audio Stream State Diagram

```
Stopped ────► Playing
    ▲           │
    │           │
    └──── Paused ◄────┘
        ▲       │
        │       │
        └───────┘
```

### Audio Loading Methods

```
File System ───► OpenFromFile()
        │
Memory Buffer ──► OpenFromMemory(Byte*, SizeT)
        │
Vector Data ───► OpenFromMemory(vector<Byte>&)
        │
Engine Asset ──► OpenFromAsset(UAsset*)
```

### Streaming vs Buffered Audio

```
Buffered Audio (AudioSource):
┌─────────────────┐
│ Load entire     │
│ file into       │
│ memory          │
│                 │
│ [SoundBuffer]   │
└─────────────────┘
        │
        ▼
┌─────────────────┐
│ Play from       │
│ memory          │
└─────────────────┘

Streaming Audio (AudioStream):
┌─────────────────┐
│ Stream data     │◄──┐
│ chunks from     │   │
│ source          │   │
│                 │   │
│ [Music Stream]  │   │
└─────────────────┘   │
        │             │
        ▼             │
┌─────────────────┐   │
│ Decode & play   │───┘
│ on-demand       │
└─────────────────┘
```

### Audio Stream Timeline

```
Audio Stream Timeline:
0s ──────────────────────► Duration
    ▲
    │
    └── Playback Position

Progress = Position / Duration
```

## Troubleshooting

### Common Issues

1. **Stream Won't Open**
   - Verify file path is correct and file exists
   - Check audio format is supported by SFML
   - Ensure audio data is valid (for memory loading)
   - Check asset is properly loaded (for asset loading)

2. **No Audio Playback**
   - Ensure audio device is available
   - Check that volume is not set to zero
   - Verify stream is in Playing state
   - Check for audio format compatibility

3. **Performance Issues**
   - Limit number of simultaneous streams
   - Use memory-based streaming for performance-critical audio
   - Profile I/O performance for file-based streaming
   - Consider audio format compression ratios

4. **Memory Issues**
   - Streams use less memory than buffered audio
   - Monitor for memory leaks in stream management
   - Properly clean up stream instances when done

### Debug Information

```cpp
// Debug stream state
std::cout << "Stream State: ";
switch (stream->GetState()) {
    case tkd::EAudioSourceState::Stopped: std::cout << "Stopped"; break;
    case tkd::EAudioSourceState::Playing: std::cout << "Playing"; break;
    case tkd::EAudioSourceState::Paused:  std::cout << "Paused"; break;
}
std::cout << std::endl;

std::cout << "Volume: " << stream->GetVolume() << std::endl;
std::cout << "Looping: " << (stream->IsLooping() ? "Yes" : "No") << std::endl;
std::cout << "Position: " << stream->GetPlaybackPosition() << "s / "
          << stream->GetDuration() << "s" << std::endl;
```

## Future Enhancements

### Planned Features
- **Multiple Format Support**: Enhanced codec support
- **Dynamic Streaming**: Adaptive quality based on performance
- **Network Streaming**: Stream audio over network connections
- **Gapless Playback**: Seamless transitions between tracks
- **Audio Analysis**: Real-time frequency analysis and beat detection

### Extension Points
- **Custom Decoders**: Support for proprietary audio formats
- **Streaming Sources**: HTTP/FTP audio streaming
- **Audio Effects**: Real-time processing during streaming
- **Cross-Platform Codecs**: Platform-specific optimizations

## Dependencies

- **SFML Audio**: Core streaming audio functionality
- **Engine Assets**: UAsset integration for packaged audio
- **Engine Config**: Platform and build configuration
- **File System**: File path handling for file-based loading

## See Also

- [Audio Buffer](Audio Buffer.md) - Audio data management for short sounds
- [Audio Source](Audio Source.md) - 3D spatial audio for sound effects
- [Audio Listener](Audio Listener.md) - 3D audio listener management
- [Audio Effect](Audio Effect.md) - Audio processing effects
- [Audio Manager](Audio Manager.md) - Audio system coordination
- [SFML Music Documentation](https://www.sfml-dev.org/documentation/2.6.0/classsf_1_1Music.php) - Underlying SFML implementation
