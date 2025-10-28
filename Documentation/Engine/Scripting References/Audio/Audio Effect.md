# Audio Effect

## Overview

The Audio Effect system in the TKD Game Engine provides a comprehensive framework for applying real-time audio effects to sound sources. Built on top of SFML's audio capabilities, the system supports various built-in effects including reverb, echo, chorus, distortion, and frequency filters, as well as custom user-defined effects.

## Architecture

### Core Components

The audio effect system follows an interface-based architecture with the following key components:

- **`IAudioEffect`**: Abstract interface defining the contract for all audio effects
- **`EAudioEffectType`**: Enumeration of supported effect types
- **`FReverbParameters`**: Structure containing reverb effect parameters
- **`AudioEffect`**: SFML-based concrete implementation of audio effects

### Interface Design

```cpp
class IAudioEffect
{
public:
    virtual ~IAudioEffect() = default;
    virtual EAudioEffectType GetType(void) const = 0;
    virtual void SetEnabled(Bool enabled) = 0;
    virtual Bool IsEnabled(void) const = 0;
};
```

## Effect Types

The engine supports the following audio effect types through the `EAudioEffectType` enumeration:

| Effect Type | Description |
|-------------|-------------|
| `Reverb` | Adds spatial ambiance and room simulation |
| `Echo` | Creates repeating sound reflections |
| `Chorus` | Adds richness through pitch modulation |
| `Distortion` | Applies harmonic distortion effects |
| `LowPass` | Filters out high frequencies |
| `HighPass` | Filters out low frequencies |
| `Custom` | User-defined effect implementations |

## Reverb Parameters

The `FReverbParameters` structure provides fine-grained control over reverb effects:

```cpp
struct FReverbParameters
{
    float density = 1.0f;           //<! Reverberation density (0.0 to 1.0)
    float diffusion = 1.0f;         //<! Reverberation diffusion (0.0 to 1.0)
    float gain = 0.32f;             //<! Overall reverb gain (0.0 to 1.0)
    float gainHF = 0.89f;           //<! High frequency reverb gain (0.0 to 1.0)
    float decayTime = 1.49f;        //<! Reverberation decay time in seconds
    float decayHFRatio = 0.83f;     //<! High frequency decay ratio
    float reflectionsGain = 0.05f;  //<! Early reflections gain
    float reflectionsDelay = 0.007f;//<! Early reflections delay in seconds
    float lateReverbGain = 1.26f;   //<! Late reverb gain
    float lateReverbDelay = 0.011f; //<! Late reverb delay in seconds
    float airAbsorptionGainHF = 0.994f; //<! Air absorption high frequency gain
    float roomRolloffFactor = 0.0f; //<! Room rolloff factor
};
```

### Parameter Descriptions

- **Density**: Controls the density of the reverberation. Higher values create more dense reverberation.
- **Diffusion**: Controls how the reverberation spreads. Higher values create more diffuse sound.
- **Gain**: Overall volume of the reverb effect.
- **GainHF**: High frequency content of the reverb.
- **DecayTime**: Time for the reverb to decay to -60dB.
- **DecayHFRatio**: Ratio of high frequency decay time to overall decay time.
- **ReflectionsGain**: Gain of early reflections.
- **ReflectionsDelay**: Delay time of early reflections.
- **LateReverbGain**: Gain of late reverberation.
- **LateReverbDelay**: Delay time of late reverberation.
- **AirAbsorptionGainHF**: High frequency air absorption effect.
- **RoomRolloffFactor**: Attenuation of reverb with distance.

## API Reference

### IAudioEffect Interface

#### Methods

##### `GetType()`
```cpp
virtual EAudioEffectType GetType(void) const = 0;
```
Returns the type of the audio effect.

**Returns**: `EAudioEffectType` - The effect type enumeration value.

##### `SetEnabled()`
```cpp
virtual void SetEnabled(Bool enabled) = 0;
```
Enables or disables the audio effect.

**Parameters**:
- `enabled`: `Bool` - True to enable the effect, false to disable it.

##### `IsEnabled()`
```cpp
virtual Bool IsEnabled(void) const = 0;
```
Checks if the audio effect is currently enabled.

**Returns**: `Bool` - True if the effect is enabled, false otherwise.

### AudioEffect Implementation

#### Constructor
```cpp
AudioEffect(EAudioEffectType type);
```
Creates a new audio effect instance.

**Parameters**:
- `type`: `EAudioEffectType` - The type of effect to create.

#### Member Variables
- `m_type`: `EAudioEffectType` - The effect type (read-only)
- `m_enabled`: `bool` - Effect enable/disable state

## Usage Examples

### Creating and Using Audio Effects

```cpp
#include <Engine/Audio/SFML/AudioEffect.hpp>

// Create a reverb effect
tkd::SFML::AudioEffect* reverbEffect = new tkd::SFML::AudioEffect(tkd::EAudioEffectType::Reverb);

// Enable the effect
reverbEffect->SetEnabled(true);

// Check if effect is active
if (reverbEffect->IsEnabled()) {
    // Effect is active
}

// Get effect type
tkd::EAudioEffectType type = reverbEffect->GetType();
assert(type == tkd::EAudioEffectType::Reverb);
```

### Applying Effects to Audio Sources

```cpp
#include <Engine/Audio/SFML/AudioSource.hpp>
#include <Engine/Audio/SFML/AudioEffect.hpp>

// Create audio source and effect
tkd::SFML::AudioSource* source = new tkd::SFML::AudioSource();
tkd::SFML::AudioEffect* effect = new tkd::SFML::AudioEffect(tkd::EAudioEffectType::Echo);

// Enable effect
effect->SetEnabled(true);

// Apply effect to audio source (implementation depends on audio manager)
audioManager->ApplyEffectToSource(source, effect);
```

### Configuring Reverb Parameters

```cpp
#include <Engine/Audio/Interfaces.hpp>

// Create custom reverb parameters
tkd::FReverbParameters reverbParams;
reverbParams.density = 0.8f;        // Less dense reverb
reverbParams.decayTime = 2.0f;     // Longer decay
reverbParams.gain = 0.5f;          // Moderate volume

// Apply parameters to reverb effect
// (This would typically be done through the audio manager)
```

## Mathematical Foundation

### Reverb Processing

Reverb effects simulate acoustic spaces through convolution with an impulse response. The `FReverbParameters` structure controls various aspects of this impulse response:

- **Early Reflections**: Modeled using delay lines with gains specified by `reflectionsGain` and `reflectionsDelay`
- **Late Reverberation**: Dense reverberation tail controlled by `lateReverbGain` and `lateReverbDelay`
- **Frequency Response**: High frequency attenuation controlled by `gainHF` and `decayHFRatio`
- **Air Absorption**: High frequency rolloff modeled by `airAbsorptionGainHF`

### Filter Effects

Low-pass and high-pass filters use IIR (Infinite Impulse Response) filter designs:

**Low-Pass Filter Transfer Function**:
```
H(z) = b0 / (1 + a1*z^(-1) + a2*z^(-2))
```

**High-Pass Filter Transfer Function**:
```
H(z) = b0 / (1 + a1*z^(-1) + a2*z^(-2))
```

Where filter coefficients are calculated based on cutoff frequency and resonance parameters.

## Performance Considerations

### Memory Usage
- Each `AudioEffect` instance maintains minimal state (effect type and enable flag)
- Reverb parameters are passed by value, avoiding dynamic allocation
- SFML backend manages underlying audio processing resources

### CPU Overhead
- Effects are processed in real-time during audio playback
- Multiple effects can be chained but may impact performance
- Consider effect enable/disable state to avoid unnecessary processing

### Thread Safety
- Effect state changes are not thread-safe by default
- Audio processing typically occurs on a dedicated audio thread
- State modifications should be synchronized if accessed from multiple threads

## Integration with Audio System

### Audio Manager Integration

The audio effect system integrates with the broader audio management system:

```cpp
// Conceptual integration (actual implementation in AudioManager)
class AudioManager {
public:
    void ApplyEffect(IAudioSource* source, IAudioEffect* effect);
    void RemoveEffect(IAudioSource* source, IAudioEffect* effect);
    void UpdateEffectParameters(IAudioEffect* effect, const void* params);
};
```

### Effect Chaining

Multiple effects can be applied to a single audio source:

```cpp
// Apply multiple effects in sequence
audioManager->ApplyEffect(source, reverbEffect);
audioManager->ApplyEffect(source, chorusEffect);
audioManager->ApplyEffect(source, lowPassFilter);
```

## Platform Considerations

### Client-Only Compilation
```cpp
#if TKD_ENGINE_CLIENT
    #include <SFML/Audio.hpp>
    // AudioEffect implementation
#endif
```

The audio effect system is compiled only for client builds (`TKD_ENGINE_CLIENT`), as server builds typically don't require audio processing.

### SFML Backend
- Relies on SFML's audio effect capabilities
- Platform-specific optimizations handled by SFML
- Cross-platform compatibility maintained through SFML abstraction

## Troubleshooting

### Common Issues

1. **Effects Not Applying**
   - Ensure `SetEnabled(true)` has been called
   - Verify audio source is properly initialized
   - Check that audio manager integration is correct

2. **Performance Degradation**
   - Disable unused effects with `SetEnabled(false)`
   - Limit number of simultaneous effects per source
   - Monitor CPU usage during audio processing

3. **Memory Leaks**
   - Properly delete `AudioEffect` instances when no longer needed
   - Use smart pointers for automatic memory management

### Debug Information

```cpp
// Debug effect state
std::cout << "Effect Type: " << static_cast<int>(effect->GetType()) << std::endl;
std::cout << "Effect Enabled: " << (effect->IsEnabled() ? "Yes" : "No") << std::endl;
```

## Future Enhancements

### Planned Features
- **Effect Presets**: Predefined parameter sets for common scenarios
- **Real-time Parameter Modulation**: Dynamic effect parameter changes
- **Effect Automation**: Keyframe-based parameter animation
- **Spatial Audio Effects**: 3D positional effects
- **Custom Effect API**: User-defined effect implementations

### Extension Points
- **Plugin System**: Support for third-party effect plugins
- **GPU Acceleration**: Hardware-accelerated effect processing
- **Network Synchronization**: Multiplayer effect state synchronization

## Dependencies

- **SFML Audio**: Core audio processing backend
- **Engine Config**: Platform and build configuration
- **Audio Interfaces**: Base interface definitions

## See Also

- [Audio Buffer](Audio Buffer.md) - Audio data management
- [Audio Source](Audio Source.md) - Audio playback sources
- [Audio Manager](Audio Manager.md) - Audio system coordination
- [SFML Audio Documentation](https://www.sfml-dev.org/documentation/2.6.0/classsf_1_1SoundEffect.php) - Underlying SFML implementation
