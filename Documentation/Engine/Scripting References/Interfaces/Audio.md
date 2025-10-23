# Audio Interface Documentation

## Overview

The `FAudioInterface` provides high-level audio management and playback capabilities for the TKD Engine. It serves as the primary interface for playing sounds, managing audio resources, and controlling the audio system in a thread-safe manner.

### Key Features

- **Simple Playback API**: Easy-to-use sound playback with volume and loop control
- **Audio Manager Integration**: Direct access to the underlying audio manager for advanced operations
- **Thread-Safe Operations**: All methods protected by mutexes for cross-thread safety
- **Resource Management**: Automatic audio resource handling and cleanup
- **Format Agnostic**: Supports various audio formats through the underlying audio system

### Architecture

```cpp
class FAudioInterface final
{
private:
    static TUniquePtr<IAudioManager> s_manager;
    static std::mutex s_mutex;

public:
    static Bool Initialize(void);
    static IAudioManager* GetAudioManager(void);
    static void PlaySound(const FilePath& filePath, Float32 volume = 1.0f, Bool loop = false);
};
```

## Initialization

#### `static Bool Initialize(void)`

Initializes the audio interface and underlying audio system.

**Returns:** `true` if initialization succeeded, `false` otherwise

**Behavior:**
- Creates and initializes the audio manager
- Sets up audio device and context
- Must be called before using audio functionality

**Example:**
```cpp
// Initialize audio during engine startup
if (!Audio::Initialize()) {
    FLogger::Log(ELogLevel::Error, "Failed to initialize audio system");
    // Continue without audio or show error
} else {
    FLogger::Log(ELogLevel::Info, "Audio system initialized successfully");
}
```

## Audio Playback

#### `static void PlaySound(const FilePath& filePath, Float32 volume = 1.0f, Bool loop = false)`

Plays a sound file with specified parameters.

**Parameters:**
- `filePath`: Path to the audio file to play
- `volume`: Playback volume (0.0 = silent, 1.0 = full volume)
- `loop`: Whether to loop the sound continuously

**Behavior:**
- Loads the audio file if not already cached
- Creates an audio source for playback
- Starts playback immediately
- Non-blocking (returns immediately while audio plays in background)

**Supported Formats:** Depends on underlying audio backend (typically WAV, OGG, MP3, etc.)

**Example:**
```cpp
// Play background music
Audio::PlaySound("Assets/Audio/BackgroundMusic.ogg", 0.7f, true);

// Play sound effect
Audio::PlaySound("Assets/Audio/Jump.wav", 1.0f, false);

// Play UI sound with low volume
Audio::PlaySound("Assets/Audio/ButtonClick.wav", 0.3f, false);
```

### Playback Patterns

```cpp
class AAudioManager : public AActor {
public:
    void PlayJumpSound() {
        Audio::PlaySound("Assets/Audio/Jump.wav", 0.8f, false);
    }

    void PlayBackgroundMusic() {
        // Stop current music first
        StopBackgroundMusic();

        // Play new music
        Audio::PlaySound("Assets/Audio/Level1Music.ogg", 0.6f, true);
        m_currentMusic = "Assets/Audio/Level1Music.ogg";
    }

    void StopBackgroundMusic() {
        if (!m_currentMusic.empty()) {
            // Note: Simple interface doesn't provide stop functionality
            // Use advanced interface for stopping
            IAudioManager* audioMgr = Audio::GetAudioManager();
            if (audioMgr) {
                // Find and stop the music source
                // Implementation depends on audio manager capabilities
            }
        }
    }

    void PlayRandomFootstep() {
        // Random footstep sounds
        static const std::vector<FString> footsteps = {
            "Assets/Audio/Footstep1.wav",
            "Assets/Audio/Footstep2.wav",
            "Assets/Audio/Footstep3.wav"
        };

        int randomIndex = rand() % footsteps.size();
        Audio::PlaySound(footsteps[randomIndex], 0.5f, false);
    }

private:
    FString m_currentMusic;
};
```

## Advanced Audio Management

#### `static IAudioManager* GetAudioManager(void)`

Gets the underlying audio manager for advanced audio operations.

**Returns:** Pointer to the `IAudioManager` instance, or `nullptr` if not initialized

**Use Cases:**
- Creating custom audio sources
- Managing audio buffers
- Applying audio effects
- 3D spatial audio
- Advanced playback control

**Example:**
```cpp
// Get audio manager for advanced operations
IAudioManager* audioMgr = Audio::GetAudioManager();
if (!audioMgr) {
    FLogger::Log(ELogLevel::Error, "Audio manager not available");
    return;
}

// Create a custom audio source
IAudioSource* source = audioMgr->CreateAudioSource();
if (source) {
    // Load audio buffer
    IAudioBuffer* buffer = audioMgr->CreateAudioBuffer("Assets/Audio/Engine.wav");
    if (buffer) {
        source->SetBuffer(buffer);
        source->SetVolume(0.8f);
        source->SetLooping(true);
        source->Play();
    }
}
```

### Audio Source Management

```cpp
class AEngineAudio : public AActor {
private:
    IAudioSource* m_engineSource = nullptr;
    float m_currentRPM = 0.0f;

public:
    virtual void BeginPlay() override {
        IAudioManager* audioMgr = Audio::GetAudioManager();
        if (audioMgr) {
            // Create engine sound source
            m_engineSource = audioMgr->CreateAudioSource();
            if (m_engineSource) {
                IAudioBuffer* buffer = audioMgr->CreateAudioBuffer("Assets/Audio/Engine.wav");
                if (buffer) {
                    m_engineSource->SetBuffer(buffer);
                    m_engineSource->SetLooping(true);
                    m_engineSource->Play();
                }
            }
        }
    }

    void UpdateEngineSound(float rpm) {
        if (m_engineSource) {
            m_currentRPM = rpm;

            // Adjust pitch based on RPM
            float pitch = 0.5f + (rpm / 8000.0f) * 2.0f; // 0.5 to 2.5 pitch range
            m_engineSource->SetPitch(pitch);

            // Adjust volume based on RPM
            float volume = 0.3f + (rpm / 4000.0f) * 0.7f; // 0.3 to 1.0 volume range
            m_engineSource->SetVolume(volume);
        }
    }

    virtual void EndPlay() override {
        if (m_engineSource) {
            m_engineSource->Stop();
            // Note: Cleanup depends on audio manager implementation
            m_engineSource = nullptr;
        }
    }
};
```

### 3D Spatial Audio

```cpp
class ASpatialAudioSystem : public AActor {
private:
    IAudioManager* m_audioManager = nullptr;
    std::vector<IAudioSource*> m_audioSources;

public:
    virtual void BeginPlay() override {
        m_audioManager = Audio::GetAudioManager();
        if (!m_audioManager) {
            FLogger::Log(ELogLevel::Error, "Audio manager not available for spatial audio");
        }
    }

    void PlaySoundAtLocation(const FString& soundFile, const FVector3& location, float volume = 1.0f) {
        if (!m_audioManager) return;

        IAudioSource* source = m_audioManager->CreateAudioSource();
        if (source) {
            IAudioBuffer* buffer = m_audioManager->CreateAudioBuffer(soundFile);
            if (buffer) {
                source->SetBuffer(buffer);
                source->SetPosition(location);
                source->SetVolume(volume);
                source->Play();

                // Store for cleanup
                m_audioSources.push_back(source);

                // Schedule cleanup after sound duration
                ScheduleCallback([this, source]() {
                    CleanupAudioSource(source);
                }, buffer->GetDuration());
            }
        }
    }

    void UpdateListenerPosition(const FVector3& position, const FVector3& forward, const FVector3& up) {
        if (!m_audioManager) return;

        IAudioListener* listener = m_audioManager->GetListener();
        if (listener) {
            listener->SetPosition(position);
            listener->SetOrientation(forward, up);
        }
    }

private:
    void CleanupAudioSource(IAudioSource* source) {
        if (source) {
            source->Stop();
            // Remove from tracking
            m_audioSources.erase(
                std::remove(m_audioSources.begin(), m_audioSources.end(), source),
                m_audioSources.end()
            );
            // Note: Actual cleanup depends on audio manager implementation
        }
    }

    void ScheduleCallback(std::function<void()> callback, float delay) {
        // Implementation depends on game's timer system
    }
};
```

## Audio Effects and Processing

```cpp
class AAudioEffectsProcessor : public AActor {
private:
    IAudioManager* m_audioManager = nullptr;
    std::unordered_map<FString, IAudioEffect*> m_effects;

public:
    virtual void BeginPlay() override {
        m_audioManager = Audio::GetAudioManager();
        if (m_audioManager) {
            InitializeEffects();
        }
    }

    void ApplyReverbToSource(IAudioSource* source, float roomSize, float damping) {
        if (!source || !m_audioManager) return;

        IAudioEffect* reverbEffect = m_audioManager->CreateEffect(EAudioEffectType::Reverb);
        if (reverbEffect) {
            // Configure reverb parameters
            reverbEffect->SetParameter("roomSize", roomSize);
            reverbEffect->SetParameter("damping", damping);

            // Apply to source
            source->AddEffect(reverbEffect);
        }
    }

    void ApplyEchoToSource(IAudioSource* source, float delay, float decay) {
        if (!source || !m_audioManager) return;

        IAudioEffect* echoEffect = m_audioManager->CreateEffect(EAudioEffectType::Echo);
        if (echoEffect) {
            echoEffect->SetParameter("delay", delay);
            echoEffect->SetParameter("decay", decay);
            source->AddEffect(echoEffect);
        }
    }

    void CreateUnderwaterEffect(IAudioSource* source) {
        if (!source || !m_audioManager) return;

        // Apply low-pass filter for underwater effect
        IAudioEffect* lowPassFilter = m_audioManager->CreateEffect(EAudioEffectType::LowPassFilter);
        if (lowPassFilter) {
            lowPassFilter->SetParameter("cutoff", 1000.0f); // Low frequency cutoff
            source->AddEffect(lowPassFilter);
        }

        // Add slight reverb
        ApplyReverbToSource(source, 0.1f, 0.8f);
    }

    void RemoveAllEffects(IAudioSource* source) {
        if (source) {
            source->ClearEffects();
        }
    }

private:
    void InitializeEffects() {
        // Pre-create commonly used effects
        m_effects["reverb"] = m_audioManager->CreateEffect(EAudioEffectType::Reverb);
        m_effects["echo"] = m_audioManager->CreateEffect(EAudioEffectType::Echo);
        m_effects["lowpass"] = m_audioManager->CreateEffect(EAudioEffectType::LowPassFilter);
    }
};
```

## Audio Streaming

```cpp
class AAudioStreamer : public AActor {
private:
    IAudioManager* m_audioManager = nullptr;
    IAudioStream* m_currentStream = nullptr;
    FString m_currentStreamFile;

public:
    virtual void BeginPlay() override {
        m_audioManager = Audio::GetAudioManager();
    }

    void StartStreaming(const FString& audioFile) {
        if (!m_audioManager) return;

        // Stop current stream
        StopStreaming();

        // Create new stream
        m_currentStream = m_audioManager->CreateAudioStream(audioFile);
        if (m_currentStream) {
            m_currentStreamFile = audioFile;
            m_currentStream->Play();
            FLogger::Log(ELogLevel::Info, "Started streaming: " + audioFile);
        } else {
            FLogger::Log(ELogLevel::Error, "Failed to create audio stream: " + audioFile);
        }
    }

    void StopStreaming() {
        if (m_currentStream) {
            m_currentStream->Stop();
            m_currentStream = nullptr;
            m_currentStreamFile.clear();
        }
    }

    void SetStreamVolume(float volume) {
        if (m_currentStream) {
            m_currentStream->SetVolume(volume);
        }
    }

    void PauseStream() {
        if (m_currentStream) {
            m_currentStream->Pause();
        }
    }

    void ResumeStream() {
        if (m_currentStream) {
            m_currentStream->Resume();
        }
    }

    bool IsStreamPlaying() const {
        return m_currentStream && m_currentStream->IsPlaying();
    }

    float GetStreamProgress() const {
        if (m_currentStream) {
            return m_currentStream->GetPlaybackPosition() / m_currentStream->GetDuration();
        }
        return 0.0f;
    }

    virtual void EndPlay() override {
        StopStreaming();
    }
};
```

## Audio Resource Management

```cpp
class AAudioResourceManager : public AActor {
private:
    IAudioManager* m_audioManager = nullptr;
    std::unordered_map<FString, IAudioBuffer*> m_audioBuffers;
    std::unordered_map<FString, IAudioSource*> m_audioSources;

public:
    virtual void BeginPlay() override {
        m_audioManager = Audio::GetAudioManager();
    }

    IAudioBuffer* GetOrLoadBuffer(const FString& audioFile) {
        if (!m_audioManager) return nullptr;

        // Check cache first
        auto it = m_audioBuffers.find(audioFile);
        if (it != m_audioBuffers.end()) {
            return it->second;
        }

        // Load new buffer
        IAudioBuffer* buffer = m_audioManager->CreateAudioBuffer(audioFile);
        if (buffer) {
            m_audioBuffers[audioFile] = buffer;
            FLogger::Log(ELogLevel::Info, "Loaded audio buffer: " + audioFile);
        } else {
            FLogger::Log(ELogLevel::Error, "Failed to load audio buffer: " + audioFile);
        }

        return buffer;
    }

    IAudioSource* CreateReusableSource(const FString& audioFile) {
        if (!m_audioManager) return nullptr;

        IAudioBuffer* buffer = GetOrLoadBuffer(audioFile);
        if (!buffer) return nullptr;

        IAudioSource* source = m_audioManager->CreateAudioSource();
        if (source) {
            source->SetBuffer(buffer);
            m_audioSources[audioFile] = source;
        }

        return source;
    }

    void PlaySoundCached(const FString& audioFile, float volume = 1.0f, bool loop = false) {
        IAudioSource* source = CreateReusableSource(audioFile);
        if (source) {
            source->SetVolume(volume);
            source->SetLooping(loop);
            source->Play();
        }
    }

    void CleanupUnusedResources() {
        // Remove stopped sources
        for (auto it = m_audioSources.begin(); it != m_audioSources.end();) {
            if (it->second && !it->second->IsPlaying()) {
                // Note: Actual cleanup depends on audio manager
                it = m_audioSources.erase(it);
            } else {
                ++it;
            }
        }

        // Optional: Unload unused buffers based on LRU or reference counting
    }

    virtual void EndPlay() override {
        // Cleanup all resources
        m_audioSources.clear();
        m_audioBuffers.clear();
    }
};
```

## Performance Considerations

### Resource Management

- **Buffer Caching**: Reuse loaded audio buffers to reduce loading overhead
- **Source Pooling**: Maintain a pool of audio sources for frequently played sounds
- **Streaming for Large Files**: Use streaming for background music and long audio files
- **Format Optimization**: Choose appropriate audio formats for different use cases

### Playback Optimization

- **Distance Culling**: Don't play sounds that are too far from the listener
- **Volume Attenuation**: Reduce volume of distant sounds
- **Effect Prioritization**: Apply effects only to important audio sources
- **Batch Operations**: Group audio operations to reduce API calls

### Memory Management

- **Buffer Compression**: Use compressed audio formats to reduce memory usage
- **Reference Counting**: Properly track and release audio resources
- **Garbage Collection**: Periodically clean up unused audio resources

## Error Handling

### Audio Initialization Failures

```cpp
bool InitializeAudioSafely() {
    try {
        if (!Audio::Initialize()) {
            FLogger::Log(ELogLevel::Error, "Audio initialization failed");

            // Check for common issues
            if (!CheckAudioDeviceAvailable()) {
                FLogger::Log(ELogLevel::Error, "No audio device available");
                ShowNoAudioDeviceMessage();
                return false;
            }

            if (!CheckAudioDriver()) {
                FLogger::Log(ELogLevel::Error, "Audio driver issue detected");
                ShowAudioDriverIssueMessage();
                return false;
            }

            // Try alternative initialization
            return TryAlternativeAudioSetup();
        }

        FLogger::Log(ELogLevel::Info, "Audio initialized successfully");
        return true;
    } catch (const std::exception& e) {
        FLogger::Log(ELogLevel::Error, "Exception during audio initialization: " + std::string(e.what()));
        return false;
    }
}

bool CheckAudioDeviceAvailable() {
    // Platform-specific audio device detection
    return true; // Placeholder
}

bool CheckAudioDriver() {
    // Platform-specific driver checking
    return true; // Placeholder
}

bool TryAlternativeAudioSetup() {
    // Try different audio configurations
    return false; // Placeholder
}
```

### Sound Playback Failures

```cpp
void PlaySoundSafely(const FString& soundFile, float volume = 1.0f, bool loop = false) {
    if (soundFile.empty()) {
        FLogger::Log(ELogLevel::Warning, "Empty sound file path");
        return;
    }

    // Validate volume range
    volume = std::clamp(volume, 0.0f, 1.0f);

    // Check if file exists
    if (!FileExists(soundFile)) {
        FLogger::Log(ELogLevel::Error, "Sound file not found: " + soundFile);
        return;
    }

    // Check audio system availability
    if (!Audio::GetAudioManager()) {
        FLogger::Log(ELogLevel::Warning, "Audio manager not available, skipping sound: " + soundFile);
        return;
    }

    try {
        Audio::PlaySound(soundFile, volume, loop);
        FLogger::Log(ELogLevel::Debug, "Playing sound: " + soundFile);
    } catch (const std::exception& e) {
        FLogger::Log(ELogLevel::Error, "Exception playing sound " + soundFile + ": " + std::string(e.what()));
    }
}

bool FileExists(const FString& filePath) {
    // Platform-specific file existence check
    return true; // Placeholder
}
```

## Platform-Specific Considerations

### Windows Audio

```cpp
#if defined(TKD_PLATFORM_WINDOWS)
void SetupWindowsAudio() {
    // Windows-specific audio setup
    // WASAPI configuration
    // Exclusive mode for low latency
}
#endif
```

### Linux Audio

```cpp
#if defined(TKD_PLATFORM_LINUX)
void SetupLinuxAudio() {
    // Linux-specific audio setup
    // ALSA/PulseAudio configuration
    // JACK audio server integration
}
#endif
```

### macOS Audio

```cpp
#if defined(TKD_PLATFORM_MACOS)
void SetupMacOSAudio() {
    // macOS-specific audio setup
    // Core Audio configuration
    // AVAudioEngine integration
}
#endif
```

## Architecture Diagrams

### Audio Interface Flow

```
Game Code
    │
    ▼
FAudioInterface (Static Methods)
    │
    ▼
std::mutex (Thread Safety)
    │
    ▼
IAudioManager (Audio Backend)
    │
    ├── IAudioBuffer (Loaded Audio Data)
    ├── IAudioSource (Playback Instance)
    ├── IAudioEffect (Sound Processing)
    ├── IAudioListener (3D Audio Position)
    └── IAudioStream (Streaming Audio)
```

### Sound Playback Flow

```
PlaySound() Called
    │
    ▼
File Path Validation
    │
    ▼
Buffer Loading/Caching
    │
    ▼
Source Creation
    │
    ▼
Parameter Application
    │
    ├── Volume Setting
    ├── Loop Configuration
    └── Position Setting
    │
    ▼
Playback Start
```

### Audio Resource Management

```
Audio File Request
    │
    ▼
Check Buffer Cache
    │
    ├── Cache Hit ──► Return Cached Buffer
    │
    └── Cache Miss ──► Load from Disk
    │
    ▼
Create Audio Source
    │
    ▼
Configure Source
    │
    ├── Attach Buffer
    ├── Set Parameters
    └── Apply Effects
    │
    ▼
Return Configured Source
```

## Troubleshooting

### Common Issues

#### No Audio Output

**Symptoms:** Sounds don't play, no audio heard

**Possible Causes:**
- Audio system not initialized
- Audio device not available
- Volume set to zero
- Audio files corrupted or missing

**Debug:**
```cpp
// Test audio system
if (!Audio::GetAudioManager()) {
    FLogger::Log(ELogLevel::Error, "Audio manager not initialized");
}

// Test simple sound
Audio::PlaySound("Assets/Audio/Test.wav", 1.0f, false);

// Check audio device
IAudioManager* audioMgr = Audio::GetAudioManager();
if (audioMgr) {
    // Check if audio device is available
    // Implementation depends on audio backend
}
```

#### Distorted Audio

**Symptoms:** Audio sounds crackly or distorted

**Possible Causes:**
- Incorrect audio format
- Buffer underrun
- Sample rate mismatch
- Driver issues

**Debug:**
```cpp
// Check audio format
IAudioManager* audioMgr = Audio::GetAudioManager();
if (audioMgr) {
    // Log audio device information
    FLogger::Log(ELogLevel::Info, "Audio device info: " + GetAudioDeviceInfo());

    // Test with different formats
    TestAudioFormats();
}
```

#### Memory Issues

**Symptoms:** Memory usage grows during audio playback

**Possible Causes:**
- Audio buffers not released
- Streaming resources not cleaned up
- Effect chains not cleared

**Debug:**
```cpp
// Monitor audio resource usage
IAudioManager* audioMgr = Audio::GetAudioManager();
if (audioMgr) {
    // Log resource counts
    FLogger::Log(ELogLevel::Info, "Active audio sources: " + std::to_string(GetActiveSourceCount()));
    FLogger::Log(ELogLevel::Info, "Loaded audio buffers: " + std::to_string(GetLoadedBufferCount()));
}

// Force cleanup
ForceAudioCleanup();
```

#### Performance Issues

**Symptoms:** Audio causes frame rate drops

**Possible Causes:**
- Too many simultaneous sounds
- Inefficient audio processing
- Large audio files
- Complex effect chains

**Debug:**
```cpp
// Profile audio performance
float audioTime = MeasureAudioProcessingTime();
if (audioTime > 5.0f) { // 5ms budget
    FLogger::Log(ELogLevel::Warning, "Audio processing too slow: " + std::to_string(audioTime) + "ms");

    // Reduce audio quality
    ReduceAudioQuality();
}

// Count active sources
int activeSources = CountActiveAudioSources();
if (activeSources > 32) {
    FLogger::Log(ELogLevel::Warning, "Too many active audio sources: " + std::to_string(activeSources));
}
```

### Audio Format Support

Different platforms and audio backends support different formats:

- **WAV**: Universally supported, uncompressed
- **OGG**: Good compression, supported on most platforms
- **MP3**: Patent-encumbered, check licensing
- **FLAC**: Lossless compression, limited platform support

### Driver Issues

- **Update Drivers**: Ensure latest audio drivers are installed
- **Exclusive Mode**: Try exclusive mode for better performance
- **Sample Rate**: Match system sample rate (usually 44100 or 48000 Hz)
- **Buffer Size**: Adjust buffer sizes for latency vs stability trade-off

## Future Enhancements

### Planned Features

1. **3D Audio**: Full HRTF-based 3D spatialization
2. **Audio Middleware Integration**: Wwise, FMOD, or similar
3. **Procedural Audio**: Runtime audio generation and synthesis
4. **Voice Chat**: Integrated voice communication system
5. **Audio Analysis**: Real-time frequency analysis and beat detection
6. **Cross-Platform Consistency**: Improved audio behavior across platforms

### Extension Points

```cpp
// Custom audio interface extensions
class FExtendedAudioInterface {
public:
    static void PlaySoundWithCallback(const FString& soundFile,
                                     float volume,
                                     std::function<void()> onComplete) {
        IAudioManager* audioMgr = Audio::GetAudioManager();
        if (audioMgr) {
            IAudioSource* source = audioMgr->CreateAudioSource();
            if (source) {
                IAudioBuffer* buffer = audioMgr->CreateAudioBuffer(soundFile);
                if (buffer) {
                    source->SetBuffer(buffer);
                    source->SetVolume(volume);

                    // Set completion callback
                    source->SetCompletionCallback(onComplete);

                    source->Play();
                }
            }
        }
    }

    static IAudioSource* CreatePositionalSound(const FString& soundFile,
                                              const FVector3& position,
                                              float volume = 1.0f) {
        IAudioManager* audioMgr = Audio::GetAudioManager();
        if (audioMgr) {
            IAudioSource* source = audioMgr->CreateAudioSource();
            if (source) {
                IAudioBuffer* buffer = audioMgr->CreateAudioBuffer(soundFile);
                if (buffer) {
                    source->SetBuffer(buffer);
                    source->SetPosition(position);
                    source->SetVolume(volume);
                    return source;
                }
            }
        }
        return nullptr;
    }

    static void SetGlobalAudioVolume(float masterVolume, float musicVolume, float sfxVolume) {
        IAudioManager* audioMgr = Audio::GetAudioManager();
        if (audioMgr) {
            // Implementation depends on audio manager capabilities
            // Set category volumes
        }
    }

    static FAudioDeviceInfo GetAudioDeviceInfo() {
        IAudioManager* audioMgr = Audio::GetAudioManager();
        FAudioDeviceInfo info;

        if (audioMgr) {
            // Query device information
            info.name = audioMgr->GetDeviceName();
            info.sampleRate = audioMgr->GetSampleRate();
            info.channels = audioMgr->GetChannelCount();
        }

        return info;
    }
};
```

---

*This documentation covers the complete FAudioInterface as of TKD Engine v1.0.0. The interface provides simple yet powerful audio capabilities with thread-safe operations and extensibility for advanced use cases.*
