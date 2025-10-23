# Audio Manager

## Overview

The Audio Manager system in the TKD Game Engine serves as the central coordinator for all audio functionality, providing a high-level interface for managing audio resources, playback, and spatial audio features. It acts as a facade over the underlying audio components, offering convenient APIs for common audio operations while managing the complexity of resource lifecycle, caching, and thread safety.

## Architecture

### Core Components

The audio manager system follows an interface-based architecture:

- **`IAudioManager`**: Abstract interface defining the contract for audio manager implementations
- **`AudioManager`**: SFML-based concrete implementation providing comprehensive audio management
- **Audio Components**: Manages `IAudioBuffer`, `IAudioSource`, `IAudioStream`, and `IAudioEffect` instances
- **Resource Caching**: Intelligent buffer caching system to avoid redundant loading
- **Thread Safety**: Mutex-protected operations for multi-threaded environments

### Interface Design

```cpp
class IAudioManager
{
public:
    virtual ~IAudioManager() = default;

    // Lifecycle management
    virtual bool Initialize(void) = 0;
    virtual void Shutdown(void) = 0;
    virtual void Update(Float32 deltaTime) = 0;

    // Factory methods
    virtual TSharedPtr<IAudioBuffer> CreateBuffer(void) = 0;
    virtual TSharedPtr<IAudioSource> CreateSource(void) = 0;
    virtual TSharedPtr<IAudioStream> CreateStream(void) = 0;
    virtual TSharedPtr<IAudioEffect> CreateEffect(EAudioEffectType type) = 0;

    // Listener access
    virtual IAudioListener* GetListener(void) = 0;

    // Resource management
    virtual TSharedPtr<IAudioBuffer> LoadBuffer(const FilePath& filePath) = 0;
    virtual void UnloadBuffer(const FilePath& filePath) = 0;
    virtual void UnloadAllBuffers(void) = 0;

    // Playback methods
    virtual void PlaySound(const FilePath& filePath, Float32 volume = 1.0f, Bool loop = false) = 0;
    virtual void PlaySound3D(const FilePath& filePath, const FVector3& position, Float32 volume = 1.0f, Bool loop = false) = 0;

    // Audio properties
    virtual void SetSpeedOfSound(Float32 speed) = 0;
    virtual Float32 GetSpeedOfSound(void) const = 0;
    virtual void SetDopplerFactor(Float32 factor) = 0;
    virtual Float32 GetDopplerFactor(void) const = 0;

    // Device management
    virtual std::vector<FString> GetAvailableDevices(void) const = 0;
    virtual FString GetDefaultDevice(void) const = 0;
    virtual Bool SetDevice(const FString& deviceName) = 0;
};
```

## Lifecycle Management

### Initialization

The audio manager must be initialized before use and properly shut down when the application terminates:

```cpp
// Initialize the audio system
tkd::SFML::AudioManager* audioManager = new tkd::SFML::AudioManager();
if (!audioManager->Initialize()) {
    // Handle initialization failure
    return false;
}

// Use audio manager...

// Shutdown when done
audioManager->Shutdown();
delete audioManager;
```

### Update Loop

The audio manager requires periodic updates to handle cleanup and effects processing:

```cpp
void GameLoop(float deltaTime) {
    // Update audio manager
    audioManager->Update(deltaTime);

    // Other game updates...
}
```

## Factory Methods

### Creating Audio Components

The audio manager provides factory methods for creating all audio components:

```cpp
// Create audio buffer
auto buffer = audioManager->CreateBuffer();

// Create audio source
auto source = audioManager->CreateSource();

// Create audio stream
auto stream = audioManager->CreateStream();

// Create audio effect
auto effect = audioManager->CreateEffect(tkd::EAudioEffectType::Reverb);
```

### Automatic Resource Tracking

Sources created through the audio manager are automatically tracked and cleaned up:

```cpp
// Sources are automatically managed
auto source = audioManager->CreateSource();
// source is tracked internally and will be cleaned up
```

## Resource Management

### Buffer Loading and Caching

The audio manager provides intelligent buffer caching to avoid redundant loading:

```cpp
// First load - buffer is created and cached
auto buffer1 = audioManager->LoadBuffer("sound.wav");

// Second load - returns cached buffer
auto buffer2 = audioManager->LoadBuffer("sound.wav");
// buffer1 and buffer2 point to the same cached buffer
```

### Loading from Multiple Sources

Buffers can be loaded from files, memory, or assets:

```cpp
// Load from file
auto buffer1 = audioManager->LoadBuffer("assets/sound.wav");

// Load from memory vector
std::vector<Byte> audioData = LoadAudioData();
auto buffer2 = audioManager->LoadBuffer(audioData);

// Load from raw memory
const Byte* data = GetAudioData();
SizeT size = GetAudioDataSize();
auto buffer3 = audioManager->LoadBuffer(data, size);

// Load from asset
const UAsset* asset = AssetManager::LoadAsset("sound");
auto buffer4 = audioManager->LoadBuffer(asset);
```

### Buffer Unloading

Buffers can be unloaded individually or all at once:

```cpp
// Unload specific buffer
audioManager->UnloadBuffer("sound.wav");

// Unload all buffers
audioManager->UnloadAllBuffers();
```

## Playback Methods

### 2D Sound Playback

Simple playback methods for non-spatial audio:

```cpp
// Play sound effect
audioManager->PlaySound("ui_click.wav", 0.8f, false);

// Play looping ambient sound
audioManager->PlaySound("ambient_rain.ogg", 0.5f, true);
```

### 3D Spatial Sound Playback

Advanced playback with 3D positioning:

```cpp
// Play 3D sound at specific position
tkd::FVector3 explosionPos(10.0f, 0.0f, 5.0f);
audioManager->PlaySound3D("explosion.wav", explosionPos, 1.0f, false);

// Play positional ambient sound
tkd::FVector3 riverPos(-20.0f, 0.0f, 30.0f);
audioManager->PlaySound3D("river_flow.ogg", riverPos, 0.6f, true);
```

### Playback from Memory and Assets

All playback methods support memory and asset sources:

```cpp
// Play from memory
std::vector<Byte> soundData = LoadSoundData();
audioManager->PlaySound(soundData, 0.7f, false);

// Play 3D from asset
const UAsset* soundAsset = AssetManager::LoadAsset("footstep");
tkd::FVector3 playerPos = GetPlayerPosition();
audioManager->PlaySound3D(soundAsset, playerPos, 0.9f, false);
```

## Audio Properties

### Speed of Sound

Controls the speed of sound for Doppler effect calculations:

```cpp
// Set realistic speed of sound (343 m/s at 20°C)
audioManager->SetSpeedOfSound(343.0f);

// Get current speed of sound
float speed = audioManager->GetSpeedOfSound();
```

### Doppler Factor

Controls the strength of Doppler effect:

```cpp
// Normal Doppler effect
audioManager->SetDopplerFactor(1.0f);

// Exaggerated Doppler effect
audioManager->SetDopplerFactor(2.0f);

// Disabled Doppler effect
audioManager->SetDopplerFactor(0.0f);

// Get current Doppler factor
float factor = audioManager->GetDopplerFactor();
```

## Device Management

### Audio Device Enumeration

Query available audio output devices:

```cpp
// Get list of available devices
std::vector<tkd::FString> devices = audioManager->GetAvailableDevices();
for (const auto& device : devices) {
    std::cout << "Available device: " << device << std::endl;
}

// Get default device
tkd::FString defaultDevice = audioManager->GetDefaultDevice();
std::cout << "Default device: " << defaultDevice << std::endl;
```

### Device Selection

Currently, device selection is not supported by the SFML backend:

```cpp
// Note: Device selection not currently supported
// This method returns false
bool success = audioManager->SetDevice("Desired Device Name");
```

## Thread Safety

### Mutex Protection

All audio manager operations are thread-safe:

```cpp
// Thread-safe operations
std::thread audioThread([audioManager]() {
    audioManager->PlaySound("background.wav");
});

std::thread updateThread([audioManager]() {
    while (running) {
        audioManager->Update(0.016f); // 60 FPS
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
});
```

### Automatic Cleanup

The update method handles automatic cleanup of stopped sources:

```cpp
void AudioManager::Update(Float32 deltaTime) {
    // Remove stopped non-looping sources
    // Apply Doppler effects
    // Other maintenance tasks
}
```

## Doppler Effect Implementation

### Manual Doppler Calculation

The audio manager implements Doppler effect calculation for enhanced realism:

```cpp
// Doppler effect is automatically applied during Update()
// Based on relative velocities between sources and listener
```

### Doppler State Tracking

Each audio source maintains Doppler state for smooth transitions:

```cpp
struct SourceDopplerState {
    FVector3 lastPosition;          // Previous position for velocity calculation
    Float32 lastDistance;           // Previous distance for smoothing
    Float32 smoothedPitch = 1.0f;   // Smoothed pitch adjustment
};
```

## API Reference

### IAudioManager Interface

#### Lifecycle Methods

##### `Initialize()`
```cpp
virtual bool Initialize(void) = 0;
```
Initializes the audio manager and underlying audio system.

**Returns**: `bool` - True if initialization successful, false otherwise.

##### `Shutdown()`
```cpp
virtual void Shutdown(void) = 0;
```
Shuts down the audio manager and releases all resources.

##### `Update(Float32)`
```cpp
virtual void Update(Float32 deltaTime) = 0;
```
Updates the audio manager. Should be called regularly (e.g., every frame).

**Parameters**:
- `deltaTime`: `Float32` - Time elapsed since last update in seconds.

#### Factory Methods

##### `CreateBuffer()`
```cpp
virtual TSharedPtr<IAudioBuffer> CreateBuffer(void) = 0;
```
Creates a new audio buffer instance.

**Returns**: `TSharedPtr<IAudioBuffer>` - Shared pointer to the created buffer.

##### `CreateSource()`
```cpp
virtual TSharedPtr<IAudioSource> CreateSource(void) = 0;
```
Creates a new audio source instance. The source is automatically tracked by the manager.

**Returns**: `TSharedPtr<IAudioSource>` - Shared pointer to the created source.

##### `CreateStream()`
```cpp
virtual TSharedPtr<IAudioStream> CreateStream(void) = 0;
```
Creates a new audio stream instance.

**Returns**: `TSharedPtr<IAudioStream>` - Shared pointer to the created stream.

##### `CreateEffect(EAudioEffectType)`
```cpp
virtual TSharedPtr<IAudioEffect> CreateEffect(EAudioEffectType type) = 0;
```
Creates a new audio effect of the specified type.

**Parameters**:
- `type`: `EAudioEffectType` - The type of effect to create.

**Returns**: `TSharedPtr<IAudioEffect>` - Shared pointer to the created effect.

##### `GetListener()`
```cpp
virtual IAudioListener* GetListener(void) = 0;
```
Retrieves the audio listener instance.

**Returns**: `IAudioListener*` - Pointer to the audio listener.

#### Buffer Management

##### `LoadBuffer(FilePath)`
```cpp
virtual TSharedPtr<IAudioBuffer> LoadBuffer(const FilePath& filePath) = 0;
```
Loads an audio buffer from a file. Buffers are cached to avoid redundant loading.

**Parameters**:
- `filePath`: `const FilePath&` - Path to the audio file.

**Returns**: `TSharedPtr<IAudioBuffer>` - Shared pointer to the loaded buffer, or nullptr on failure.

##### `LoadBuffer(UAsset*)`
```cpp
virtual TSharedPtr<IAudioBuffer> LoadBuffer(const UAsset* asset) = 0;
```
Loads an audio buffer from an engine asset.

**Parameters**:
- `asset`: `const UAsset*` - Pointer to the asset containing audio data.

**Returns**: `TSharedPtr<IAudioBuffer>` - Shared pointer to the loaded buffer, or nullptr on failure.

##### `LoadBuffer(vector<Byte>&)`
```cpp
virtual TSharedPtr<IAudioBuffer> LoadBuffer(const std::vector<Byte>& data) = 0;
```
Loads an audio buffer from a memory vector.

**Parameters**:
- `data`: `const std::vector<Byte>&` - Vector containing audio data.

**Returns**: `TSharedPtr<IAudioBuffer>` - Shared pointer to the loaded buffer, or nullptr on failure.

##### `LoadBuffer(Byte*, SizeT)`
```cpp
virtual TSharedPtr<IAudioBuffer> LoadBuffer(const Byte* data, const SizeT size) = 0;
```
Loads an audio buffer from a raw memory buffer.

**Parameters**:
- `data`: `const Byte*` - Pointer to audio data.
- `size`: `SizeT` - Size of the data in bytes.

**Returns**: `TSharedPtr<IAudioBuffer>` - Shared pointer to the loaded buffer, or nullptr on failure.

##### `UnloadBuffer(FilePath)`
```cpp
virtual void UnloadBuffer(const FilePath& filePath) = 0;
```
Unloads a specific buffer from the cache.

**Parameters**:
- `filePath`: `const FilePath&` - Path of the buffer to unload.

##### `UnloadAllBuffers()`
```cpp
virtual void UnloadAllBuffers(void) = 0;
```
Unloads all cached buffers.

#### Playback Methods

##### `PlaySound(FilePath, Float32, Bool)`
```cpp
virtual void PlaySound(const FilePath& filePath, Float32 volume = 1.0f, Bool loop = false) = 0;
```
Plays a 2D sound from a file.

**Parameters**:
- `filePath`: `const FilePath&` - Path to the audio file.
- `volume`: `Float32` - Volume level (0.0 to 1.0), default 1.0.
- `loop`: `Bool` - Whether to loop the sound, default false.

##### `PlaySound(vector<Byte>&, Float32, Bool)`
```cpp
virtual void PlaySound(const std::vector<Byte>& data, Float32 volume = 1.0f, Bool loop = false) = 0;
```
Plays a 2D sound from memory.

**Parameters**:
- `data`: `const std::vector<Byte>&` - Audio data vector.
- `volume`: `Float32` - Volume level (0.0 to 1.0), default 1.0.
- `loop`: `Bool` - Whether to loop the sound, default false.

##### `PlaySound(Byte*, SizeT, Float32, Bool)`
```cpp
virtual void PlaySound(const Byte* data, SizeT size, Float32 volume = 1.0f, Bool loop = false) = 0;
```
Plays a 2D sound from raw memory.

**Parameters**:
- `data`: `const Byte*` - Pointer to audio data.
- `size`: `SizeT` - Size of the data in bytes.
- `volume`: `Float32` - Volume level (0.0 to 1.0), default 1.0.
- `loop`: `Bool` - Whether to loop the sound, default false.

##### `PlaySound(UAsset*, Float32, Bool)`
```cpp
virtual void PlaySound(const UAsset* asset, Float32 volume = 1.0f, Bool loop = false) = 0;
```
Plays a 2D sound from an asset.

**Parameters**:
- `asset`: `const UAsset*` - Asset containing audio data.
- `volume`: `Float32` - Volume level (0.0 to 1.0), default 1.0.
- `loop`: `Bool` - Whether to loop the sound, default false.

##### `PlaySound3D(FilePath, FVector3, Float32, Bool)`
```cpp
virtual void PlaySound3D(const FilePath& filePath, const FVector3& position, Float32 volume = 1.0f, Bool loop = false) = 0;
```
Plays a 3D spatial sound from a file.

**Parameters**:
- `filePath`: `const FilePath&` - Path to the audio file.
- `position`: `const FVector3&` - 3D position of the sound.
- `volume`: `Float32` - Volume level (0.0 to 1.0), default 1.0.
- `loop`: `Bool` - Whether to loop the sound, default false.

##### `PlaySound3D(vector<Byte>&, FVector3, Float32, Bool)`
```cpp
virtual void PlaySound3D(const std::vector<Byte>& data, const FVector3& position, Float32 volume = 1.0f, Bool loop = false) = 0;
```
Plays a 3D spatial sound from memory.

**Parameters**:
- `data`: `const std::vector<Byte>&` - Audio data vector.
- `position`: `const FVector3&` - 3D position of the sound.
- `volume`: `Float32` - Volume level (0.0 to 1.0), default 1.0.
- `loop`: `Bool` - Whether to loop the sound, default false.

##### `PlaySound3D(Byte*, SizeT, FVector3, Float32, Bool)`
```cpp
virtual void PlaySound3D(const Byte* data, SizeT size, const FVector3& position, Float32 volume = 1.0f, Bool loop = false) = 0;
```
Plays a 3D spatial sound from raw memory.

**Parameters**:
- `data`: `const Byte*` - Pointer to audio data.
- `size`: `SizeT` - Size of the data in bytes.
- `position`: `const FVector3&` - 3D position of the sound.
- `volume`: `Float32` - Volume level (0.0 to 1.0), default 1.0.
- `loop`: `Bool` - Whether to loop the sound, default false.

##### `PlaySound3D(UAsset*, FVector3, Float32, Bool)`
```cpp
virtual void PlaySound3D(const UAsset* asset, const FVector3& position, Float32 volume = 1.0f, Bool loop = false) = 0;
```
Plays a 3D spatial sound from an asset.

**Parameters**:
- `asset`: `const UAsset*` - Asset containing audio data.
- `position`: `const FVector3&` - 3D position of the sound.
- `volume`: `Float32` - Volume level (0.0 to 1.0), default 1.0.
- `loop`: `Bool` - Whether to loop the sound, default false.

#### Audio Properties

##### `SetSpeedOfSound(Float32)`
```cpp
virtual void SetSpeedOfSound(Float32 speed) = 0;
```
Sets the speed of sound for Doppler effect calculations.

**Parameters**:
- `speed`: `Float32` - Speed of sound in units per second.

##### `GetSpeedOfSound()`
```cpp
virtual Float32 GetSpeedOfSound(void) const = 0;
```
Retrieves the current speed of sound.

**Returns**: `Float32` - Speed of sound in units per second.

##### `SetDopplerFactor(Float32)`
```cpp
virtual void SetDopplerFactor(Float32 factor) = 0;
```
Sets the Doppler effect strength factor.

**Parameters**:
- `factor`: `Float32` - Doppler factor (0.0 = disabled, 1.0 = normal).

##### `GetDopplerFactor()`
```cpp
virtual Float32 GetDopplerFactor(void) const = 0;
```
Retrieves the current Doppler factor.

**Returns**: `Float32` - Current Doppler factor.

#### Device Management

##### `GetAvailableDevices()`
```cpp
virtual std::vector<FString> GetAvailableDevices(void) const = 0;
```
Retrieves a list of available audio output devices.

**Returns**: `std::vector<FString>` - Vector of device names.

##### `GetDefaultDevice()`
```cpp
virtual FString GetDefaultDevice(void) const = 0;
```
Retrieves the name of the default audio output device.

**Returns**: `FString` - Name of the default device.

##### `SetDevice(FString)`
```cpp
virtual Bool SetDevice(const FString& deviceName) = 0;
```
Attempts to set the active audio output device.

**Parameters**:
- `deviceName`: `const FString&` - Name of the device to select.

**Returns**: `Bool` - True if device was set successfully, false otherwise.

### AudioManager Implementation

#### Constructor
```cpp
AudioManager(void);
```
Creates an audio manager with default settings:
- Speed of sound: 343.3 m/s (at 20°C)
- Doppler factor: 1.0
- Initialized: false

#### Member Variables
- `m_listener`: `AudioListener` - The audio listener instance
- `m_buffers`: `std::map<FString, TSharedPtr<IAudioBuffer>>` - Cached audio buffers
- `m_sources`: `std::vector<TSharedPtr<IAudioSource>>` - Active audio sources
- `m_mutex`: `std::mutex` - Thread safety mutex
- `m_speedOfSound`: `Float32` - Speed of sound for Doppler calculations
- `m_dopplerFactor`: `Float32` - Doppler effect strength
- `m_initialized`: `Bool` - Initialization state
- `m_dopplerStates`: `std::unordered_map<IAudioSource*, SourceDopplerState>` - Doppler state tracking

## Usage Examples

### Basic Audio Manager Setup

```cpp
#include <Engine/Audio/SFML/AudioManager.hpp>

// Create and initialize audio manager
tkd::SFML::AudioManager* audioManager = new tkd::SFML::AudioManager();

if (!audioManager->Initialize()) {
    std::cerr << "Failed to initialize audio manager" << std::endl;
    return 1;
}

// Configure audio properties
audioManager->SetSpeedOfSound(343.0f);  // Realistic speed of sound
audioManager->SetDopplerFactor(1.0f);   // Normal Doppler effect

// Main game loop
bool running = true;
while (running) {
    float deltaTime = 0.016f; // 60 FPS
    audioManager->Update(deltaTime);

    // Game logic...
}

// Cleanup
audioManager->Shutdown();
delete audioManager;
```

### Resource Management

```cpp
class GameAudioSystem {
private:
    tkd::SFML::AudioManager* m_audioManager;

public:
    void LoadAudioResources() {
        // Load commonly used sound effects
        m_audioManager->LoadBuffer("sounds/jump.wav");
        m_audioManager->LoadBuffer("sounds/land.wav");
        m_audioManager->LoadBuffer("sounds/collect.wav");
    }

    void PlaySoundEffect(const std::string& soundName) {
        // Buffers are cached, so this is efficient
        m_audioManager->PlaySound(soundName, 0.8f, false);
    }

    void Cleanup() {
        // Unload all cached buffers
        m_audioManager->UnloadAllBuffers();
    }
};
```

### 3D Audio Integration

```cpp
class AudioSystem3D {
private:
    tkd::SFML::AudioManager* m_audioManager;

public:
    void Initialize() {
        // Configure listener
        auto listener = m_audioManager->GetListener();
        listener->SetPosition(tkd::FVector3(0.0f, 0.0f, 0.0f));
        listener->SetOrientation(
            tkd::FVector3(0.0f, 0.0f, -1.0f), // Forward
            tkd::FVector3(0.0f, 1.0f, 0.0f)   // Up
        );
    }

    void UpdateListener(const tkd::FVector3& position,
                       const tkd::FVector3& forward,
                       const tkd::FVector3& up) {
        auto listener = m_audioManager->GetListener();
        listener->SetPosition(position);
        listener->SetOrientation(forward, up);
    }

    void PlayFootstep(const tkd::FVector3& position) {
        m_audioManager->PlaySound3D("sounds/footstep.wav", position, 0.6f, false);
    }

    void PlayExplosion(const tkd::FVector3& position) {
        m_audioManager->PlaySound3D("sounds/explosion.wav", position, 1.0f, false);
    }
};
```

### Advanced Audio Management

```cpp
class AdvancedAudioManager {
private:
    tkd::SFML::AudioManager* m_audioManager;
    std::map<std::string, TSharedPtr<IAudioSource>> m_loopingSources;

public:
    void PlayBackgroundMusic(const std::string& musicFile) {
        auto stream = m_audioManager->CreateStream();
        if (stream->OpenFromFile(musicFile)) {
            stream->SetVolume(0.5f);
            stream->SetLooping(true);
            stream->Play();
            m_loopingSources["background"] = stream;
        }
    }

    void PlayAmbientSound(const std::string& soundFile, const tkd::FVector3& position) {
        auto source = m_audioManager->CreateSource();
        auto buffer = m_audioManager->LoadBuffer(soundFile);

        if (buffer) {
            source->SetBuffer(buffer);
            source->SetPosition(position);
            source->SetVolume(0.3f);
            source->SetLooping(true);
            source->SetSpatial(true);
            source->SetMinDistance(5.0f);
            source->SetMaxDistance(50.0f);
            source->Play();

            m_loopingSources[soundFile] = source;
        }
    }

    void StopAllLoopingSounds() {
        for (auto& pair : m_loopingSources) {
            pair.second->Stop();
        }
        m_loopingSources.clear();
    }
};
```

## Performance Considerations

### Memory Management
- Buffer caching prevents redundant loading of the same audio files
- Automatic cleanup of stopped sources prevents memory leaks
- Shared pointers ensure proper resource lifetime management

### CPU Overhead
- Update method should be called regularly but not excessively
- Doppler effect calculations add minimal overhead
- SFML handles the heavy lifting of audio processing

### Thread Safety
- All operations are mutex-protected for thread safety
- Suitable for use in multi-threaded game engines
- Audio processing occurs on dedicated threads

### Optimization Tips
- Pre-load commonly used audio buffers during loading screens
- Use streaming for background music to reduce memory usage
- Limit the number of simultaneous 3D sources for performance
- Cache frequently used buffer references

## Integration with Game Systems

### Entity-Component System

```cpp
class AudioComponent {
private:
    tkd::SFML::AudioManager* m_audioManager;
    TSharedPtr<IAudioSource> m_source;

public:
    void Initialize(tkd::SFML::AudioManager* audioManager) {
        m_audioManager = audioManager;
        m_source = m_audioManager->CreateSource();
    }

    void PlaySound(const std::string& soundName, float volume = 1.0f) {
        auto buffer = m_audioManager->LoadBuffer(soundName);
        if (buffer) {
            m_source->SetBuffer(buffer);
            m_source->SetVolume(volume);
            m_source->Play();
        }
    }

    void UpdatePosition(const tkd::FVector3& position) {
        if (m_source) {
            m_source->SetPosition(position);
        }
    }
};
```

### Scene Management

```cpp
class SceneAudioManager {
private:
    tkd::SFML::AudioManager* m_audioManager;
    std::vector<TSharedPtr<IAudioStream>> m_sceneStreams;

public:
    void LoadSceneAudio(const std::string& sceneName) {
        // Load scene-specific background music
        auto music = m_audioManager->CreateStream();
        std::string musicFile = "music/scenes/" + sceneName + ".ogg";
        if (music->OpenFromFile(musicFile)) {
            music->SetVolume(0.4f);
            music->SetLooping(true);
            m_sceneStreams.push_back(music);
        }

        // Load ambient sounds
        LoadAmbientSounds(sceneName);
    }

    void PlaySceneAudio() {
        for (auto& stream : m_sceneStreams) {
            stream->Play();
        }
    }

    void StopSceneAudio() {
        for (auto& stream : m_sceneStreams) {
            stream->Stop();
        }
        m_sceneStreams.clear();
    }
};
```

## Platform Considerations

### Client-Only Compilation
```cpp
#if TKD_ENGINE_CLIENT
    #include <SFML/Audio.hpp>
    // AudioManager implementation
#endif
```

The audio manager is compiled only for client builds, as server applications typically don't require audio playback.

### SFML Backend
- Cross-platform audio support through SFML
- Hardware acceleration where available
- Consistent behavior across supported platforms

### Device Limitations
- Audio device selection is not currently supported by SFML
- Default system audio device is used
- Device enumeration is available for informational purposes

## Diagrams

### Audio Manager Architecture

```
Game Engine
     │
     ▼
Audio Manager (IAudioManager)
     │
     ├─► Audio Listener (IAudioListener)
     │
     ├─► Buffer Cache ──► IAudioBuffer[]
     │
     ├─► Source Pool ──► IAudioSource[]
     │
     ├─► Stream Factory ─► IAudioStream[]
     │
     └─► Effect Factory ─► IAudioEffect[]
```

### Resource Loading Flow

```
LoadBuffer(filePath)
        │
        ▼
    Check Cache
        │
        ├─► Cache Hit ──► Return Cached Buffer
        │
        └─► Cache Miss ──► Create New Buffer
                                │
                                ├─► Load from File
                                ├─► Load from Memory
                                └─► Load from Asset
                                        │
                                        ▼
                                Add to Cache
                                        │
                                        ▼
                                Return Buffer
```

### Playback Method Flow

```
PlaySound(filePath, volume, loop)
        │
        ▼
    LoadBuffer(filePath)
        │
        ▼
    CreateSource()
        │
        ▼
    Configure Source:
    ├──► SetBuffer(buffer)
    ├──► SetVolume(volume)
    ├──► SetLooping(loop)
    └──► SetSpatial(false)
        │
        ▼
    Play()
```

### Update Cycle

```
Update(deltaTime)
        │
        ▼
    Lock Mutex
        │
        ▼
    Cleanup Stopped Sources
        │
        ▼
    Apply Doppler Effects
        │
        ▼
    Update Source States
        │
        ▼
    Unlock Mutex
```

## Troubleshooting

### Common Issues

1. **No Audio Output**
   - Ensure `Initialize()` was called successfully
   - Check that audio device is available and not muted
   - Verify audio files exist and are valid

2. **Memory Leaks**
   - Call `UnloadAllBuffers()` when switching scenes
   - Ensure sources are properly cleaned up
   - Monitor source count in debug builds

3. **Performance Issues**
   - Limit simultaneous 3D sources to reasonable numbers
   - Pre-load buffers during loading screens
   - Profile Update() call frequency

4. **Doppler Effect Problems**
   - Ensure source velocities are set correctly
   - Check speed of sound value is realistic
   - Verify Doppler factor is not zero

### Debug Information

```cpp
// Debug audio manager state
std::cout << "Audio Manager Status:" << std::endl;
std::cout << "  Initialized: " << (audioManager->GetInitializedState() ? "Yes" : "No") << std::endl;
std::cout << "  Speed of Sound: " << audioManager->GetSpeedOfSound() << " m/s" << std::endl;
std::cout << "  Doppler Factor: " << audioManager->GetDopplerFactor() << std::endl;

// Debug buffer cache
std::cout << "  Cached Buffers: " << GetBufferCacheSize() << std::endl;

// Debug active sources
std::cout << "  Active Sources: " << GetActiveSourceCount() << std::endl;

// Debug devices
auto devices = audioManager->GetAvailableDevices();
std::cout << "  Available Devices: " << devices.size() << std::endl;
for (const auto& device : devices) {
    std::cout << "    - " << device << std::endl;
}
```

## Future Enhancements

### Planned Features
- **Audio Device Selection**: Support for choosing specific output devices
- **Advanced Mixing**: Per-source volume controls and effects chains
- **Audio Analysis**: Real-time frequency analysis and beat detection
- **Network Audio**: Synchronized audio playback across network
- **Audio Middleware Integration**: Support for third-party audio engines

### Extension Points
- **Plugin System**: Custom audio backends and effects
- **GPU Audio**: Hardware-accelerated audio processing
- **Spatial Audio**: Advanced HRTF and ambisonic support
- **Audio Scripting**: Lua/Python audio control interfaces

## Dependencies

- **SFML Audio**: Core audio functionality and device management
- **Engine Core**: Vector3, shared pointers, and threading utilities
- **Engine Assets**: UAsset integration for packaged audio
- **Engine Config**: Platform and build configuration
- **File System**: File path handling for audio loading

## See Also

- [Audio Buffer](Audio Buffer.md) - Audio data management
- [Audio Source](Audio Source.md) - 3D spatial audio sources
- [Audio Stream](Audio Stream.md) - Streaming audio playback
- [Audio Listener](Audio Listener.md) - 3D audio listener management
- [Audio Effect](Audio Effect.md) - Audio processing effects
- [SFML Audio Documentation](https://www.sfml-dev.org/documentation/2.6.0/) - Underlying SFML implementation
