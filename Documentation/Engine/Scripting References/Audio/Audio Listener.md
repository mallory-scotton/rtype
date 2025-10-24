# Audio Listener

## Overview

The Audio Listener system in the TKD Game Engine provides 3D spatial audio positioning and orientation management. As the virtual "ears" of the audio system, the listener represents the player's position and orientation in the 3D world, enabling realistic spatial audio rendering where sound sources appear to come from specific directions and distances.

## Architecture

### Core Components

The audio listener system follows an interface-based architecture:

- **`IAudioListener`**: Abstract interface defining the contract for audio listener implementations
- **`AudioListener`**: SFML-based concrete implementation providing 3D audio positioning

### Interface Design

```cpp
class IAudioListener
{
public:
    virtual ~IAudioListener() = default;

    virtual void SetPosition(const FVector3& position) = 0;
    virtual FVector3 GetPosition(void) const = 0;
    virtual void SetVelocity(const FVector3& velocity) = 0;
    virtual FVector3 GetVelocity(void) const = 0;
    virtual void SetOrientation(const FVector3& forward, const FVector3& up) = 0;
    virtual void GetOrientation(FVector3& forward, FVector3& up) const = 0;
    virtual void SetGlobalVolume(Float32 volume) = 0;
    virtual Float32 GetGlobalVolume(void) const = 0;
};
```

## 3D Audio Concepts

### Spatial Positioning

The audio listener uses a right-handed coordinate system where:
- **X-axis**: Points right
- **Y-axis**: Points up
- **Z-axis**: Points forward (out of the screen)

```
     Y (Up)
     ^
     |
     |   Z (Forward)
     |  /
     | /
     |/
     +---------> X (Right)
```

### Listener Properties

1. **Position**: The 3D location of the listener in world space
2. **Velocity**: Used for Doppler effect calculations
3. **Orientation**: Defined by forward and up vectors
4. **Global Volume**: Master volume control affecting all audio

### Spatial Audio Rendering

The listener position and orientation determine how 3D audio sources are spatialized:

- **Distance Attenuation**: Sounds get quieter with distance
- **Directional Audio**: Sounds appear to come from specific directions
- **Doppler Effect**: Frequency shifts based on relative velocity
- **Head-Related Transfer Function (HRTF)**: Binaural audio simulation

## API Reference

### IAudioListener Interface

#### Position Management

##### `SetPosition()`
```cpp
virtual void SetPosition(const FVector3& position) = 0;
```
Sets the 3D position of the audio listener.

**Parameters**:
- `position`: `const FVector3&` - The position vector in world coordinates.

##### `GetPosition()`
```cpp
virtual FVector3 GetPosition(void) const = 0;
```
Retrieves the current 3D position of the audio listener.

**Returns**: `FVector3` - The current position vector.

#### Velocity Management

##### `SetVelocity()`
```cpp
virtual void SetVelocity(const FVector3& velocity) = 0;
```
Sets the velocity vector used for Doppler effect calculations.

**Parameters**:
- `velocity`: `const FVector3&` - The velocity vector in world units per second.

##### `GetVelocity()`
```cpp
virtual FVector3 GetVelocity(void) const = 0;
```
Retrieves the current velocity vector.

**Returns**: `FVector3` - The current velocity vector.

#### Orientation Management

##### `SetOrientation()`
```cpp
virtual void SetOrientation(const FVector3& forward, const FVector3& up) = 0;
```
Sets the listener's orientation using forward and up direction vectors.

**Parameters**:
- `forward`: `const FVector3&` - The forward direction vector (must be normalized)
- `up`: `const FVector3&` - The up direction vector (must be normalized and perpendicular to forward)

##### `GetOrientation()`
```cpp
virtual void GetOrientation(FVector3& forward, FVector3& up) const = 0;
```
Retrieves the current orientation vectors.

**Parameters**:
- `forward`: `FVector3&` - Output parameter for the forward direction vector
- `up`: `FVector3&` - Output parameter for the up direction vector

#### Volume Control

##### `SetGlobalVolume()`
```cpp
virtual void SetGlobalVolume(Float32 volume) = 0;
```
Sets the master volume for all audio playback.

**Parameters**:
- `volume`: `Float32` - Volume level from 0.0 (silent) to 1.0 (full volume)

##### `GetGlobalVolume()`
```cpp
virtual Float32 GetGlobalVolume(void) const = 0;
```
Retrieves the current master volume level.

**Returns**: `Float32` - The current volume level (0.0 to 1.0)

### AudioListener Implementation

#### Constructor
```cpp
AudioListener(void);
```
Creates a new audio listener with default settings:
- Position: (0, 0, 0)
- Velocity: (0, 0, 0)
- Orientation: Forward (0, 0, -1), Up (0, 1, 0)
- Global Volume: 1.0 (full volume)

#### Member Variables
- `m_velocity`: `FVector3` - Cached velocity vector for Doppler calculations
- `m_globalVolume`: `Float32` - Cached global volume level

## Usage Examples

### Basic Listener Setup

```cpp
#include <Engine/Audio/SFML/AudioListener.hpp>

// Create audio listener
tkd::SFML::AudioListener* listener = new tkd::SFML::AudioListener();

// Set listener position (e.g., camera position)
tkd::FVector3 cameraPosition(10.0f, 5.0f, 0.0f);
listener->SetPosition(cameraPosition);

// Set listener orientation (e.g., camera forward and up vectors)
tkd::FVector3 cameraForward(0.0f, 0.0f, -1.0f);
tkd::FVector3 cameraUp(0.0f, 1.0f, 0.0f);
listener->SetOrientation(cameraForward, cameraUp);

// Set master volume
listener->SetGlobalVolume(0.8f); // 80% volume
```

### Camera Integration

```cpp
#include <Engine/Audio/SFML/AudioListener.hpp>
#include <Engine/Graphics/FCamera.hpp> // Assuming camera class exists

class GameAudioManager {
private:
    tkd::SFML::AudioListener* m_listener;
    FCamera* m_camera;

public:
    void UpdateListenerFromCamera() {
        if (m_listener && m_camera) {
            // Update position
            m_listener->SetPosition(m_camera->GetPosition());

            // Update orientation
            tkd::FVector3 forward, up;
            m_camera->GetOrientation(forward, up);
            m_listener->SetOrientation(forward, up);

            // Update velocity (for Doppler effect)
            tkd::FVector3 velocity = m_camera->GetVelocity();
            m_listener->SetVelocity(velocity);
        }
    }
};
```

### Dynamic Volume Control

```cpp
#include <Engine/Audio/SFML/AudioListener.hpp>

// Fade out audio over time
void FadeOutAudio(tkd::SFML::AudioListener* listener, float duration) {
    float startVolume = listener->GetGlobalVolume();
    float elapsed = 0.0f;

    while (elapsed < duration) {
        float t = elapsed / duration;
        float volume = startVolume * (1.0f - t);
        listener->SetGlobalVolume(volume);

        // Update elapsed time
        elapsed += deltaTime;
    }

    listener->SetGlobalVolume(0.0f); // Ensure complete silence
}
```

### 3D Audio Source Positioning

```cpp
#include <Engine/Audio/SFML/AudioListener.hpp>
#include <Engine/Audio/SFML/AudioSource.hpp>

// Position audio source relative to listener
void PositionAudioSource(tkd::SFML::AudioListener* listener,
                        tkd::SFML::AudioSource* source,
                        const tkd::FVector3& worldPosition) {

    // Calculate relative position to listener
    tkd::FVector3 listenerPos = listener->GetPosition();
    tkd::FVector3 relativePos = worldPosition - listenerPos;

    // Set source position for 3D spatialization
    source->SetPosition(relativePos);
}
```

## Mathematical Foundation

### Coordinate System Transformation

The audio listener uses standard 3D Cartesian coordinates with the following conventions:

**Position Vector**: `P = (Px, Py, Pz)`
- `Px`: Right/left displacement
- `Py`: Up/down displacement
- `Pz`: Forward/backward displacement

**Orientation Matrix**: The listener's orientation is defined by forward and up vectors:

```
Forward Vector: F = (Fx, Fy, Fz)
Up Vector:      U = (Ux, Uy, Uz)
Right Vector:   R = F × U  (cross product)
```

### Distance Attenuation

Sound intensity decreases with distance following the inverse square law:

```
I(d) = I₀ / d²
```

Where:
- `I(d)`: Intensity at distance d
- `I₀`: Reference intensity
- `d`: Distance from source to listener

### Doppler Effect

Frequency shift due to relative motion between source and listener:

```
f' = f * (v + vl) / (v + vs)
```

Where:
- `f`: Original frequency
- `f'`: Perceived frequency
- `v`: Speed of sound in air
- `vl`: Component of listener velocity towards source
- `vs`: Component of source velocity towards listener

### Spatial Audio Processing

The listener position and orientation determine the HRTF (Head-Related Transfer Function) applied to each audio source:

```
HRTF(θ, φ) = Transfer function for azimuth θ and elevation φ
```

Where:
- `θ`: Azimuth angle (horizontal rotation)
- `φ`: Elevation angle (vertical rotation)

## Performance Considerations

### Memory Usage
- Minimal memory footprint: stores only velocity vector and volume scalar
- No dynamic memory allocation during normal operation
- SFML backend manages underlying audio processing resources

### CPU Overhead
- Position/orientation updates are lightweight operations
- Global volume changes require SFML state synchronization
- Doppler effect calculations performed by SFML audio engine

### Thread Safety
- Listener state is not thread-safe by default
- Audio processing occurs on dedicated audio thread
- State modifications should be synchronized in multi-threaded applications

### Update Frequency
- Listener position/orientation should be updated every frame for smooth audio
- Velocity updates needed only when listener movement changes significantly
- Global volume changes can be less frequent

## Integration with Game Systems

### Camera System Integration

```cpp
class CameraController {
private:
    IAudioListener* m_audioListener;

public:
    void Update(float deltaTime) {
        // Update camera transform
        UpdateCameraTransform(deltaTime);

        // Sync audio listener with camera
        if (m_audioListener) {
            m_audioListener->SetPosition(GetCameraPosition());
            m_audioListener->SetVelocity(GetCameraVelocity());

            FVector3 forward, up;
            GetCameraOrientation(forward, up);
            m_audioListener->SetOrientation(forward, up);
        }
    }
};
```

### Audio Manager Integration

```cpp
class AudioManager {
private:
    IAudioListener* m_listener;
    std::vector<IAudioSource*> m_sources;

public:
    void SetListener(IAudioListener* listener) {
        m_listener = listener;
    }

    void UpdateSpatialAudio() {
        if (!m_listener) return;

        FVector3 listenerPos = m_listener->GetPosition();

        for (auto* source : m_sources) {
            if (source->IsSpatial()) {
                // Update relative positioning
                FVector3 sourcePos = source->GetWorldPosition();
                FVector3 relativePos = sourcePos - listenerPos;
                source->SetRelativePosition(relativePos);
            }
        }
    }
};
```

## Platform Considerations

### Client-Only Compilation
```cpp
#if TKD_ENGINE_CLIENT
    #include <SFML/Audio.hpp>
    // AudioListener implementation
#endif
```

The audio listener is compiled only for client builds, as server applications typically don't require audio output.

### SFML Backend
- Cross-platform audio support through SFML
- Hardware acceleration where available
- Consistent behavior across supported platforms

## Diagrams

### 3D Audio Coordinate System

```
Listener Coordinate System:
                      Up (Y)
                       ^
                       |
                       |     Forward (Z)
                       |    /
                       |   /
                       |  /
                       | /
                       |/
                       +----------------> Right (X)
                      /
                     /
                    /
                   /
            Backward (-Z)
```

### Listener Orientation Vectors

```
Listener at origin, facing forward:

    Up Vector (0, 1, 0)
         ^
         |
         |   Forward Vector (0, 0, -1)
         |  /
         | /
         |/
         +---------> Right Vector (1, 0, 0)
        /
       /
      /
     /
    Left Vector (-1, 0, 0)
```

### Spatial Audio Zones

```
High Volume Zone    Medium Volume Zone    Low Volume Zone
     |                     |                     |
     v                     v                     v
  +-----+               +-----+               +-----+
  |     |               |     |               |     |
  |  X  | <--- Source   |     |               |     |
  |     |               |     |               |     |
  +-----+               +-----+               +-----+
     ^                     ^                     ^
     |                     |                     |
  Listener              Distance             Far Distance
  Position            Attenuation          Attenuation
```

## Troubleshooting

### Common Issues

1. **No Spatial Audio Effect**
   - Ensure listener position is set correctly
   - Verify audio sources are configured for 3D spatialization
   - Check that source positions are relative to listener

2. **Orientation Problems**
   - Ensure forward and up vectors are normalized
   - Verify vectors are perpendicular (forward • up = 0)
   - Check coordinate system consistency

3. **Volume Issues**
   - Confirm global volume is not set to 0.0
   - Check individual source volumes
   - Verify audio device is not muted

4. **Performance Problems**
   - Update listener position only when necessary
   - Avoid frequent orientation changes
   - Profile audio thread CPU usage

### Debug Information

```cpp
// Debug listener state
std::cout << "Listener Position: (" << listener->GetPosition().x << ", "
          << listener->GetPosition().y << ", "
          << listener->GetPosition().z << ")" << std::endl;

std::cout << "Listener Volume: " << listener->GetGlobalVolume() << std::endl;

FVector3 forward, up;
listener->GetOrientation(forward, up);
std::cout << "Forward: (" << forward.x << ", " << forward.y << ", " << forward.z << ")" << std::endl;
std::cout << "Up: (" << up.x << ", " << up.y << ", " << up.z << ")" << std::endl;
```

## Future Enhancements

### Planned Features
- **Advanced HRTF**: Custom head-related transfer functions
- **Room Acoustics**: Environmental audio simulation
- **Dynamic Range Compression**: Automatic volume leveling
- **Audio Ray Tracing**: Realistic occlusion and reflection
- **Binaural Rendering**: Enhanced 3D audio precision

### Extension Points
- **Plugin System**: Third-party spatial audio plugins
- **GPU Acceleration**: Hardware-accelerated audio processing
- **Network Synchronization**: Multiplayer listener state sync
- **VR/AR Support**: Advanced spatial audio for immersive experiences

## Dependencies

- **SFML Audio**: Core audio processing and spatialization
- **Engine Core**: Vector3 and math utilities
- **Engine Config**: Platform and build configuration

## See Also

- [Audio Source](Audio Source.md) - 3D audio source management
- [Audio Manager](Audio Manager.md) - Audio system coordination
- [Audio Buffer](Audio Buffer.md) - Audio data management
- [Audio Effect](Audio Effect.md) - Audio processing effects
- [SFML Listener Documentation](https://www.sfml-dev.org/documentation/2.6.0/classsf_1_1Listener.php) - Underlying SFML implementation
