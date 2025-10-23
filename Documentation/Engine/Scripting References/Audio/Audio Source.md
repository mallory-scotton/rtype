# Audio Source

## Overview

The Audio Source system in the TKD Game Engine provides comprehensive audio playback and spatialization capabilities. Audio sources represent individual sound emitters in the 3D world, supporting both simple 2D playback and complex 3D spatial audio with distance attenuation, Doppler effects, and directional sound cones.

## Architecture

### Core Components

The audio source system follows an interface-based architecture:

- **`IAudioSource`**: Abstract interface defining the contract for audio source implementations
- **`EAudioSourceState`**: Enumeration of playback states
- **`EAttenuationModel`**: Enumeration of distance attenuation models
- **`AudioSource`**: SFML-based concrete implementation providing audio playback and spatialization

### Interface Design

```cpp
class IAudioSource
{
public:
    virtual ~IAudioSource() = default;

    // Playback control
    virtual void Play(void) = 0;
    virtual void Pause(void) = 0;
    virtual void Stop(void) = 0;
    virtual EAudioSourceState GetState(void) const = 0;

    // Audio buffer management
    virtual void SetBuffer(TSharedPtr<IAudioBuffer> buffer) = 0;
    virtual TSharedPtr<IAudioBuffer> GetBuffer(void) const = 0;

    // Audio properties
    virtual void SetVolume(Float32 volume) = 0;
    virtual Float32 GetVolume(void) const = 0;
    virtual void SetPitch(Float32 pitch) = 0;
    virtual Float32 GetPitch(void) const = 0;
    virtual void SetLooping(Bool loop) = 0;
    virtual Bool IsLooping(void) const = 0;

    // Playback position
    virtual void SetPlaybackPosition(Float32 seconds) = 0;
    virtual Float32 GetPlaybackPosition(void) const = 0;

    // 3D spatial audio
    virtual void SetPosition(const FVector3& position) = 0;
    virtual FVector3 GetPosition(void) const = 0;
    virtual void SetVelocity(const FVector3& velocity) = 0;
    virtual FVector3 GetVelocity(void) const = 0;
    virtual void SetDirection(const FVector3& direction) = 0;
    virtual FVector3 GetDirection(void) const = 0;

    // Directional audio
    virtual void SetConeAngles(Float32 innerAngle, Float32 outerAngle) = 0;
    virtual void SetConeOuterGain(Float32 gain) = 0;

    // Spatialization control
    virtual void SetSpatial(Bool spatial) = 0;
    virtual Bool IsSpatial(void) const = 0;

    // Attenuation
    virtual void SetAttenuationModel(EAttenuationModel model) = 0;
    virtual EAttenuationModel GetAttenuationModel(void) const = 0;
    virtual void SetMinDistance(Float32 distance) = 0;
    virtual Float32 GetMinDistance(void) const = 0;
    virtual void SetMaxDistance(Float32 distance) = 0;
    virtual Float32 GetMaxDistance(void) const = 0;
    virtual void SetRolloffFactor(Float32 factor) = 0;
    virtual Float32 GetRolloffFactor(void) const = 0;

    // Doppler effect
    virtual void SetDopplerFactor(Float32 factor) = 0;
    virtual Float32 GetDopplerFactor(void) const = 0;
};
```

## Enumerations

### EAudioSourceState

Defines the current playback state of an audio source:

| State | Description |
|-------|-------------|
| `Stopped` | Audio source is not playing |
| `Playing` | Audio source is currently playing |
| `Paused` | Audio source is paused but can resume |

### EAttenuationModel

Defines how audio volume decreases with distance:

| Model | Description | Formula |
|-------|-------------|---------|
| `None` | No distance attenuation | `V(d) = V₀` |
| `Inverse` | Inverse distance attenuation | `V(d) = V₀ / d` |
| `Linear` | Linear fade over distance | `V(d) = V₀ * (1 - d/max)` |
| `Exponential` | Exponential decay | `V(d) = V₀ * e^(-kd)` |

## Playback Control

### Basic Playback

Audio sources support standard playback operations:

- **Play**: Start or resume audio playback
- **Pause**: Temporarily halt playback (can be resumed)
- **Stop**: Completely stop playback (cannot be resumed from current position)

### State Management

```cpp
// Check current playback state
EAudioSourceState state = source->GetState();

// Convenience methods
if (source->IsPlaying()) {
    // Source is actively playing
}
if (source->IsPaused()) {
    // Source is paused
}
if (source->IsStopped()) {
    // Source is stopped
}
```

## Audio Properties

### Volume Control

Volume can be set using normalized values (0.0 to 1.0) or percentage values (0 to 100):

```cpp
// Set volume as normalized value (0.0 = silent, 1.0 = full)
source->SetVolume(0.75f); // 75% volume

// Set volume as percentage (0 = silent, 100 = full)
source->SetVolume(75u);   // 75% volume

// Get current volume (always returns 0.0 to 1.0)
float currentVolume = source->GetVolume();
```

### Pitch Control

Pitch affects the playback speed and frequency of the audio:

```cpp
// Set pitch (0.5 = half speed, 1.0 = normal, 2.0 = double speed)
source->SetPitch(1.2f); // 20% higher pitch

// Get current pitch
float currentPitch = source->GetPitch();
```

### Looping

Enable or disable automatic looping of the audio:

```cpp
// Enable looping
source->SetLooping(true);

// Check if looping is enabled
if (source->IsLooping()) {
    // Audio will loop when it reaches the end
}
```

### Playback Position

Control the current playback position within the audio buffer:

```cpp
// Seek to 5 seconds into the audio
source->SetPlaybackPosition(5.0f);

// Get current playback position
float currentTime = source->GetPlaybackPosition();
```

## 3D Spatial Audio

### Position and Orientation

Audio sources can be positioned in 3D space relative to the audio listener:

```cpp
// Set source position in world space
tkd::FVector3 sourcePos(10.0f, 0.0f, 5.0f);
source->SetPosition(sourcePos);

// Set source velocity for Doppler effect
tkd::FVector3 velocity(0.0f, 0.0f, 2.0f); // Moving forward at 2 units/sec
source->SetVelocity(velocity);

// Set source direction (for directional audio)
tkd::FVector3 direction(0.0f, 0.0f, 1.0f); // Facing forward
source->SetDirection(direction);
```

### Spatialization Control

Toggle between spatialized (3D) and non-spatialized (2D) audio:

```cpp
// Enable 3D spatial audio
source->SetSpatial(true);

// Disable spatial audio (plays at full volume regardless of position)
source->SetSpatial(false);

// Check spatialization state
if (source->IsSpatial()) {
    // 3D audio is enabled
}
```

## Attenuation Models

### Distance-Based Volume Falloff

The attenuation model determines how audio volume decreases with distance:

#### None Attenuation
```
Volume remains constant regardless of distance
Best for: UI sounds, music, ambient effects
```

#### Inverse Attenuation
```
Volume = ReferenceVolume / Distance
Best for: Realistic sound propagation
```

#### Linear Attenuation
```
Volume = ReferenceVolume * (1 - Distance/MaxDistance)
Best for: Game audio with predictable falloff
```

#### Exponential Attenuation
```
Volume = ReferenceVolume * e^(-k * Distance)
Best for: Natural-sounding distance falloff
```

### Attenuation Parameters

```cpp
// Set attenuation model
source->SetAttenuationModel(tkd::EAttenuationModel::Inverse);

// Configure distance parameters
source->SetMinDistance(1.0f);    // Full volume within 1 unit
source->SetMaxDistance(50.0f);   // Silent beyond 50 units
source->SetRolloffFactor(1.0f);  // Standard rolloff rate
```

## API Reference

### IAudioSource Interface

#### Playback Control Methods

##### `Play()`
```cpp
virtual void Play(void) = 0;
```
Starts or resumes audio playback.

##### `Pause()`
```cpp
virtual void Pause(void) = 0;
```
Pauses audio playback. Can be resumed with `Play()`.

##### `Stop()`
```cpp
virtual void Stop(void) = 0;
```
Stops audio playback completely. Cannot be resumed from current position.

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
Checks if the audio source is currently playing.

**Returns**: `Bool` - True if playing, false otherwise.

##### `IsPaused()`
```cpp
virtual Bool IsPaused(void) const = 0;
```
Checks if the audio source is currently paused.

**Returns**: `Bool` - True if paused, false otherwise.

##### `IsStopped()`
```cpp
virtual Bool IsStopped(void) const = 0;
```
Checks if the audio source is currently stopped.

**Returns**: `Bool` - True if stopped, false otherwise.

#### Audio Buffer Management

##### `SetBuffer()`
```cpp
virtual void SetBuffer(TSharedPtr<IAudioBuffer> buffer) = 0;
```
Associates an audio buffer with this source for playback.

**Parameters**:
- `buffer`: `TSharedPtr<IAudioBuffer>` - Audio buffer containing the sound data.

##### `GetBuffer()`
```cpp
virtual TSharedPtr<IAudioBuffer> GetBuffer(void) const = 0;
```
Retrieves the currently associated audio buffer.

**Returns**: `TSharedPtr<IAudioBuffer>` - The audio buffer, or nullptr if none set.

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

#### Pitch Control

##### `SetPitch()`
```cpp
virtual void SetPitch(Float32 pitch) = 0;
```
Sets the playback pitch/speed multiplier.

**Parameters**:
- `pitch`: `Float32` - Pitch multiplier (0.5 = half speed, 1.0 = normal, 2.0 = double speed).

##### `GetPitch()`
```cpp
virtual Float32 GetPitch(void) const = 0;
```
Retrieves the current pitch multiplier.

**Returns**: `Float32` - Current pitch multiplier.

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
Sets the current playback position within the audio buffer.

**Parameters**:
- `seconds`: `Float32` - Position in seconds from the start of the audio.

##### `GetPlaybackPosition()`
```cpp
virtual Float32 GetPlaybackPosition(void) const = 0;
```
Retrieves the current playback position.

**Returns**: `Float32` - Current position in seconds.

#### 3D Positioning

##### `SetPosition()`
```cpp
virtual void SetPosition(const FVector3& position) = 0;
```
Sets the 3D position of the audio source.

**Parameters**:
- `position`: `const FVector3&` - Position vector in world coordinates.

##### `GetPosition()`
```cpp
virtual FVector3 GetPosition(void) const = 0;
```
Retrieves the current 3D position.

**Returns**: `FVector3` - Current position vector.

##### `SetVelocity()`
```cpp
virtual void SetVelocity(const FVector3& velocity) = 0;
```
Sets the velocity vector for Doppler effect calculations.

**Parameters**:
- `velocity`: `const FVector3&` - Velocity vector in world units per second.

##### `GetVelocity()`
```cpp
virtual FVector3 GetVelocity(void) const = 0;
```
Retrieves the current velocity vector.

**Returns**: `FVector3` - Current velocity vector.

##### `SetDirection()`
```cpp
virtual void SetDirection(const FVector3& direction) = 0;
```
Sets the direction vector for directional audio.

**Parameters**:
- `direction`: `const FVector3&` - Direction vector (should be normalized).

##### `GetDirection()`
```cpp
virtual FVector3 GetDirection(void) const = 0;
```
Retrieves the current direction vector.

**Returns**: `FVector3` - Current direction vector.

#### Directional Audio

##### `SetConeAngles()`
```cpp
virtual void SetConeAngles(Float32 innerAngle, Float32 outerAngle) = 0;
```
Sets the inner and outer cone angles for directional audio.

**Parameters**:
- `innerAngle`: `Float32` - Inner cone angle in degrees (full volume)
- `outerAngle`: `Float32` - Outer cone angle in degrees (attenuated volume)

##### `SetConeOuterGain()`
```cpp
virtual void SetConeOuterGain(Float32 gain) = 0;
```
Sets the volume reduction outside the outer cone.

**Parameters**:
- `gain`: `Float32` - Gain reduction in decibels outside outer cone.

#### Spatialization Control

##### `SetSpatial()`
```cpp
virtual void SetSpatial(Bool spatial) = 0;
```
Enables or disables 3D spatialization.

**Parameters**:
- `spatial`: `Bool` - True for 3D spatial audio, false for 2D audio.

##### `IsSpatial()`
```cpp
virtual Bool IsSpatial(void) const = 0;
```
Checks if spatialization is enabled.

**Returns**: `Bool` - True if spatialized, false otherwise.

#### Attenuation Control

##### `SetAttenuationModel()`
```cpp
virtual void SetAttenuationModel(EAttenuationModel model) = 0;
```
Sets the distance attenuation model.

**Parameters**:
- `model`: `EAttenuationModel` - Attenuation model to use.

##### `GetAttenuationModel()`
```cpp
virtual EAttenuationModel GetAttenuationModel(void) const = 0;
```
Retrieves the current attenuation model.

**Returns**: `EAttenuationModel` - Current attenuation model.

##### `SetMinDistance()`
```cpp
virtual void SetMinDistance(Float32 distance) = 0;
```
Sets the minimum distance for attenuation calculations.

**Parameters**:
- `distance`: `Float32` - Distance in world units.

##### `GetMinDistance()`
```cpp
virtual Float32 GetMinDistance(void) const = 0;
```
Retrieves the minimum distance.

**Returns**: `Float32` - Minimum distance in world units.

##### `SetMaxDistance()`
```cpp
virtual void SetMaxDistance(Float32 distance) = 0;
```
Sets the maximum distance for attenuation calculations.

**Parameters**:
- `distance`: `Float32` - Distance in world units.

##### `GetMaxDistance()`
```cpp
virtual Float32 GetMaxDistance(void) const = 0;
```
Retrieves the maximum distance.

**Returns**: `Float32` - Maximum distance in world units.

##### `SetRolloffFactor()`
```cpp
virtual void SetRolloffFactor(Float32 factor) = 0;
```
Sets the rolloff factor for attenuation calculations.

**Parameters**:
- `factor`: `Float32` - Rolloff factor (higher values = faster attenuation).

##### `GetRolloffFactor()`
```cpp
virtual Float32 GetRolloffFactor(void) const = 0;
```
Retrieves the current rolloff factor.

**Returns**: `Float32` - Current rolloff factor.

#### Doppler Effect

##### `SetDopplerFactor()`
```cpp
virtual void SetDopplerFactor(Float32 factor) = 0;
```
Sets the Doppler effect strength.

**Parameters**:
- `factor`: `Float32` - Doppler factor (0.0 = disabled, 1.0 = normal, >1.0 = exaggerated).

##### `GetDopplerFactor()`
```cpp
virtual Float32 GetDopplerFactor(void) const = 0;
```
Retrieves the current Doppler factor.

**Returns**: `Float32` - Current Doppler factor.

### AudioSource Implementation

#### Constructor
```cpp
AudioSource(void);
```
Creates a new audio source with default settings:
- Spatial: true (3D audio enabled)
- Attenuation: Inverse model
- Velocity: (0, 0, 0)
- Direction: (0, 0, 1) - forward
- Rolloff: 1.0
- Doppler: 1.0
- Min Distance: 0.0
- Max Distance: 100.0

#### Member Variables
- `m_sound`: `sf::Sound` - Underlying SFML sound object
- `m_buffer`: `TSharedPtr<IAudioBuffer>` - Associated audio buffer
- `m_spatial`: `Bool` - Spatialization enable flag
- `m_attenuationModel`: `EAttenuationModel` - Current attenuation model
- `m_velocity`: `FVector3` - Velocity vector for Doppler effect
- `m_direction`: `FVector3` - Direction vector for directional audio
- `m_rolloff`: `Float32` - Rolloff factor
- `m_dopplerFactor`: `Float32` - Doppler effect strength
- `m_minDistance`: `Float32` - Minimum attenuation distance
- `m_maxDistance`: `Float32` - Maximum attenuation distance

## Usage Examples

### Basic Audio Playback

```cpp
#include <Engine/Audio/SFML/AudioSource.hpp>
#include <Engine/Audio/SFML/AudioBuffer.hpp>

// Create audio source and buffer
tkd::SFML::AudioSource* source = new tkd::SFML::AudioSource();
tkd::SFML::AudioBuffer* buffer = new tkd::SFML::AudioBuffer();

// Load audio data
buffer->LoadFromFile("sound.wav");

// Associate buffer with source
source->SetBuffer(std::shared_ptr<tkd::IAudioBuffer>(buffer));

// Configure playback
source->SetVolume(0.8f);
source->SetLooping(true);

// Start playback
source->Play();

// Control playback
if (source->IsPlaying()) {
    source->Pause();  // Pause
    source->Play();   // Resume
    source->Stop();   // Stop completely
}
```

### 3D Spatial Audio Setup

```cpp
#include <Engine/Audio/SFML/AudioSource.hpp>
#include <Engine/Audio/SFML/AudioListener.hpp>

// Create source and listener
tkd::SFML::AudioSource* source = new tkd::SFML::AudioSource();
tkd::SFML::AudioListener* listener = new tkd::SFML::AudioListener();

// Configure listener (usually matches camera position)
listener->SetPosition(tkd::FVector3(0.0f, 0.0f, 0.0f));
listener->SetOrientation(
    tkd::FVector3(0.0f, 0.0f, -1.0f), // Forward
    tkd::FVector3(0.0f, 1.0f, 0.0f)   // Up
);

// Configure spatial audio source
source->SetPosition(tkd::FVector3(10.0f, 0.0f, 5.0f)); // 10 units right, 5 forward
source->SetSpatial(true);
source->SetAttenuationModel(tkd::EAttenuationModel::Inverse);
source->SetMinDistance(1.0f);
source->SetMaxDistance(50.0f);

// Load and play spatial audio
auto buffer = std::make_shared<tkd::SFML::AudioBuffer>();
buffer->LoadFromFile("spatial_sound.wav");
source->SetBuffer(buffer);
source->Play();
```

### Dynamic Audio Source Management

```cpp
class SoundEmitter {
private:
    tkd::SFML::AudioSource* m_source;
    tkd::FVector3 m_position;
    tkd::FVector3 m_velocity;

public:
    void Update(float deltaTime) {
        // Update position based on velocity
        m_position += m_velocity * deltaTime;

        // Update audio source position
        m_source->SetPosition(m_position);
        m_source->SetVelocity(m_velocity);
    }

    void SetPosition(const tkd::FVector3& position) {
        m_position = position;
        m_source->SetPosition(position);
    }

    void SetVelocity(const tkd::FVector3& velocity) {
        m_velocity = velocity;
        m_source->SetVelocity(velocity);
    }
};
```

### Audio Source Pool Management

```cpp
class AudioSourcePool {
private:
    std::vector<tkd::SFML::AudioSource*> m_sources;
    std::queue<tkd::SFML::AudioSource*> m_availableSources;

public:
    tkd::SFML::AudioSource* GetSource() {
        if (m_availableSources.empty()) {
            // Create new source if pool is empty
            auto source = new tkd::SFML::AudioSource();
            m_sources.push_back(source);
            return source;
        }

        auto source = m_availableSources.front();
        m_availableSources.pop();
        return source;
    }

    void ReturnSource(tkd::SFML::AudioSource* source) {
        source->Stop();
        source->SetBuffer(nullptr);
        m_availableSources.push(source);
    }
};
```

## Mathematical Foundation

### Distance Attenuation

#### Inverse Distance Model
```
V(d) = V₀ / (d × rolloff)
```
Where:
- `V(d)`: Volume at distance d
- `V₀`: Reference volume
- `d`: Distance from listener to source
- `rolloff`: Rolloff factor

#### Linear Distance Model
```
V(d) = V₀ × (1 - min(d, d_max) / d_max)
```
Where:
- `d_max`: Maximum distance

#### Exponential Distance Model
```
V(d) = V₀ × e^(-rolloff × (d - d_min))
```
Where:
- `d_min`: Minimum distance

### Doppler Effect

The Doppler effect causes frequency shifts based on relative motion:

```
f' = f × (v_sound + v_listener) / (v_sound - v_source)
```

Where:
- `f`: Original frequency
- `f'`: Perceived frequency
- `v_sound`: Speed of sound (343 m/s at 20°C)
- `v_listener`: Component of listener velocity towards source
- `v_source`: Component of source velocity towards listener

### Directional Audio Cones

Audio sources can have directional radiation patterns:

```
Inner Cone: Full volume within inner angle
Outer Cone: Attenuated volume between inner and outer angles
Outside: Further attenuated volume outside outer angle
```

## Performance Considerations

### Memory Usage
- Each `AudioSource` instance wraps an SFML `Sound` object
- Audio buffers are shared via smart pointers
- Minimal additional memory for spatial audio parameters

### CPU Overhead
- SFML handles audio mixing and spatialization
- 3D calculations performed during audio processing
- Multiple sources can impact performance

### Thread Safety
- Audio playback occurs on dedicated audio thread
- State changes should be synchronized
- Buffer assignment is not thread-safe

### Optimization Tips
- Reuse audio sources when possible
- Disable spatialization for UI sounds
- Use appropriate attenuation models
- Limit active sources based on hardware capabilities

## Integration with Game Systems

### Entity-Component System

```cpp
class AudioComponent {
private:
    tkd::SFML::AudioSource* m_source;
    std::string m_soundName;

public:
    void PlaySound(const std::string& soundName) {
        if (soundName != m_soundName) {
            // Load new sound buffer
            auto buffer = AudioManager::GetInstance()->LoadBuffer(soundName);
            m_source->SetBuffer(buffer);
            m_soundName = soundName;
        }
        m_source->Play();
    }

    void UpdatePosition(const tkd::FVector3& position) {
        m_source->SetPosition(position);
    }
};
```

### Audio Manager Integration

```cpp
class AudioManager {
private:
    std::vector<tkd::SFML::AudioSource*> m_sources;
    tkd::SFML::AudioListener* m_listener;

public:
    tkd::SFML::AudioSource* PlaySound3D(const std::string& soundName,
                                       const tkd::FVector3& position,
                                       bool loop = false) {
        auto source = GetAvailableSource();
        auto buffer = LoadBuffer(soundName);

        source->SetBuffer(buffer);
        source->SetPosition(position);
        source->SetSpatial(true);
        source->SetLooping(loop);
        source->Play();

        return source;
    }

    void UpdateListener(const tkd::FVector3& position,
                       const tkd::FVector3& forward,
                       const tkd::FVector3& up) {
        m_listener->SetPosition(position);
        m_listener->SetOrientation(forward, up);
    }
};
```

## Platform Considerations

### Client-Only Compilation
```cpp
#if TKD_ENGINE_CLIENT
    #include <SFML/Audio.hpp>
    // AudioSource implementation
#endif
```

The audio source system is compiled only for client builds, as server applications typically don't require audio playback.

### SFML Limitations
- Some advanced features (cone angles, Doppler) are not fully supported by SFML
- Extended features are prepared for future SFML versions or alternative backends
- Cross-platform audio support through SFML

## Diagrams

### Audio Source State Diagram

```
Stopped ────► Playing
    ▲           │
    │           │
    └──── Paused ◄────┘
        ▲       │
        │       │
        └───────┘
```

### 3D Audio Coordinate System

```
Listener at origin, source positioned in space:

    Y (Up)
    ^
    │
    │   Source Position (x, y, z)
    │  /
    │ /
    │/
    +----------------> X (Right)
   /
  /
 Z (Forward)
```

### Attenuation Models Comparison

```
Distance Attenuation Curves:

Linear:     Volume │
            1.0 ───┼───────
                 │      \
                 │       \
                 │        \
            0.0 ─┼─────────┼─► Distance
                      Max

Inverse:    Volume │
            1.0 ───┼───────
                 │     \
                 │      \
                 │       \
            0.0 ─┼────────┼─► Distance
                    Rolloff

Exponential: Volume │
            1.0 ───┼───────
                 │    \
                 │     \
                 │      \
            0.0 ─┼───────┼─► Distance
                   Rolloff
```

### Directional Audio Cone

```
Sound Source with Directional Cone:

         Outer Cone (attenuated)
        ↗         ↖
       ↗           ↖
      ↗             ↖
     ↗               ↖
    ↗    Inner Cone    ↖
   ↗     (full volume)   ↖
  ↗                       ↖
 ↗                         ↖
▼                           ▼
```

## Troubleshooting

### Common Issues

1. **No Audio Playback**
   - Ensure audio buffer is loaded and assigned
   - Check that audio device is available
   - Verify volume levels are not set to zero

2. **3D Audio Not Working**
   - Confirm spatialization is enabled
   - Check listener position and orientation
   - Ensure source position is set correctly

3. **Performance Problems**
   - Limit number of active sources
   - Use appropriate attenuation models
   - Disable spatialization for non-positional audio

4. **Memory Leaks**
   - Properly manage audio source lifetimes
   - Use smart pointers for buffer management
   - Return sources to pools when done

### Debug Information

```cpp
// Debug audio source state
std::cout << "Source State: ";
switch (source->GetState()) {
    case tkd::EAudioSourceState::Stopped: std::cout << "Stopped"; break;
    case tkd::EAudioSourceState::Playing: std::cout << "Playing"; break;
    case tkd::EAudioSourceState::Paused:  std::cout << "Paused"; break;
}
std::cout << std::endl;

std::cout << "Volume: " << source->GetVolume() << std::endl;
std::cout << "Spatial: " << (source->IsSpatial() ? "Yes" : "No") << std::endl;
std::cout << "Looping: " << (source->IsLooping() ? "Yes" : "No") << std::endl;

auto pos = source->GetPosition();
std::cout << "Position: (" << pos.x << ", " << pos.y << ", " << pos.z << ")" << std::endl;
```

## Future Enhancements

### Planned Features
- **Advanced Cone Audio**: Full directional audio support with inner/outer cones
- **Reverb Zones**: Environmental audio effects based on location
- **Dynamic Mixing**: Real-time audio parameter modulation
- **Audio Occlusion**: Sound blocking and filtering
- **GPU Audio Processing**: Hardware-accelerated spatial audio

### Extension Points
- **Plugin Architecture**: Third-party audio effect plugins
- **Network Synchronization**: Multiplayer audio state sync
- **Advanced DSP**: Custom digital signal processing effects
- **VR Audio**: Binaural and ambisonic audio support

## Dependencies

- **SFML Audio**: Core audio playback and spatialization
- **Engine Core**: Vector3 and math utilities
- **Engine Config**: Platform and build configuration
- **Audio Buffer**: Audio data management

## See Also

- [Audio Buffer](Audio Buffer.md) - Audio data management
- [Audio Listener](Audio Listener.md) - 3D audio listener management
- [Audio Effect](Audio Effect.md) - Audio processing effects
- [Audio Manager](Audio Manager.md) - Audio system coordination
- [SFML Sound Documentation](https://www.sfml-dev.org/documentation/2.6.0/classsf_1_1Sound.php) - Underlying SFML implementation
