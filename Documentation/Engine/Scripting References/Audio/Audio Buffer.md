# Audio Buffer System - IAudioBuffer & AudioBuffer

## Overview

The Audio Buffer system in the TKD Game Engine provides a unified interface for loading and managing audio data. It consists of the `IAudioBuffer` interface and its SFML-based implementation `AudioBuffer`, which handles audio data loading from various sources and provides access to audio properties.

Audio buffers are essential for:
- **Sound Effects**: Loading and storing short audio clips
- **Background Music**: Managing music tracks and ambient sounds
- **Audio Streaming**: Preparing audio data for playback sources
- **Resource Management**: Caching and sharing audio data across multiple sources
- **Cross-Platform Support**: Abstracting platform-specific audio formats

## Architecture

### Class Hierarchy
```
IAudioBuffer (Interface)
├── AudioBuffer (SFML Implementation)
    ├── Member Variables
    │   └── m_buffer (sf::SoundBuffer) - SFML audio buffer
    ├── Loading Methods
    ├── Property Accessors
    └── Native Handle Access
```

### Dependencies
- **IAudioBuffer**: Pure virtual interface defining the audio buffer contract
- **AudioBuffer**: SFML-based concrete implementation
- **SFML**: Audio library providing `sf::SoundBuffer` for audio data management
- **UAsset**: Engine asset system for packaged audio resources
- **FilePath**: Standard filesystem path handling
- **Engine Types**: `Bool`, `Float32`, `UInt32`, `SizeT`, `Byte` for type safety

### Memory Layout
The `AudioBuffer` class contains:
- **SFML SoundBuffer**: Variable size depending on audio data (samples × channels × bytes per sample)
- **Interface Overhead**: Virtual function table pointer
- **Total**: ~32 bytes base + audio data size

## Core Components

### Interface Definition (IAudioBuffer)

#### Pure Virtual Methods
The `IAudioBuffer` interface defines the contract for all audio buffer implementations:

```cpp
class IAudioBuffer {
public:
    virtual ~IAudioBuffer() = default;

    // Loading methods
    virtual Bool LoadFromAsset(const UAsset* asset) = 0;
    virtual Bool LoadFromFile(const FilePath& filePath) = 0;
    virtual Bool LoadFromMemory(const std::vector<Byte>& data) = 0;
    virtual Bool LoadFromMemory(const Byte* data, const SizeT size) = 0;

    // Property accessors
    virtual Float32 GetDuration(void) const = 0;
    virtual UInt32 GetSampleRate(void) const = 0;
    virtual UInt32 GetChannelCount(void) const = 0;

    // Native access
    virtual void* GetNativeHandle(void) const = 0;
};
```

### SFML Implementation (AudioBuffer)

#### Member Variables

##### SFML SoundBuffer (`m_buffer`)
```cpp
sf::SoundBuffer m_buffer;  //<! SFML SoundBuffer instance
```
- **Type**: `sf::SoundBuffer`
- **Purpose**: SFML's audio buffer implementation handling all audio data storage and format conversion
- **Memory**: Contains decoded PCM audio data
- **Thread Safety**: Not thread-safe, should be loaded on main thread

## API Reference

### Loading Methods

#### LoadFromAsset
```cpp
virtual Bool LoadFromAsset(const UAsset* asset) override;
```
Loads audio data from an engine asset.

**Parameters:**
- `asset`: Pointer to a loaded UAsset containing audio data

**Returns:** `true` if loading succeeded, `false` otherwise

**Implementation Details:**
- Validates asset pointer and data
- Calls SFML's `loadFromMemory()` with asset data
- Supports any audio format that SFML can decode

#### LoadFromFile
```cpp
virtual Bool LoadFromFile(const FilePath& filePath) override;
```
Loads audio data from a file on disk.

**Parameters:**
- `filePath`: Path to the audio file

**Returns:** `true` if loading succeeded, `false` otherwise

**Implementation Details:**
- Converts FilePath to string for SFML
- Calls SFML's `loadFromFile()` method
- Supports all SFML-supported audio formats (WAV, OGG, FLAC, etc.)

#### LoadFromMemory (Vector)
```cpp
virtual Bool LoadFromMemory(const std::vector<Byte>& data) override;
```
Loads audio data from a memory buffer vector.

**Parameters:**
- `data`: Vector containing raw audio data

**Returns:** `true` if loading succeeded, `false` otherwise

**Implementation Details:**
- Validates vector is not empty
- Calls SFML's `loadFromMemory()` with vector data and size

#### LoadFromMemory (Raw Pointer)
```cpp
virtual Bool LoadFromMemory(const Byte* data, const SizeT size) override;
```
Loads audio data from a raw memory buffer.

**Parameters:**
- `data`: Pointer to raw audio data in memory
- `size`: Size of the data buffer in bytes

**Returns:** `true` if loading succeeded, `false` otherwise

**Implementation Details:**
- Validates pointer and size are valid
- Calls SFML's `loadFromMemory()` directly

### Property Accessors

#### GetDuration
```cpp
virtual Float32 GetDuration(void) const override;
```
Returns the duration of the audio buffer in seconds.

**Returns:** Duration in seconds as a floating-point value

**Implementation Details:**
- Calls SFML's `getDuration().asSeconds()`
- Returns 0.0f if buffer is not loaded

#### GetSampleRate
```cpp
virtual UInt32 GetSampleRate(void) const override;
```
Returns the sample rate of the audio buffer.

**Returns:** Sample rate in Hz (e.g., 44100, 48000)

**Implementation Details:**
- Calls SFML's `getSampleRate()` method
- Returns 0 if buffer is not loaded

#### GetChannelCount
```cpp
virtual UInt32 GetChannelCount(void) const override;
```
Returns the number of audio channels in the buffer.

**Returns:** Number of channels (1 for mono, 2 for stereo, etc.)

**Implementation Details:**
- Calls SFML's `getChannelCount()` method
- Returns 0 if buffer is not loaded

### Native Handle Access

#### GetNativeHandle
```cpp
virtual void* GetNativeHandle(void) const override;
```
Provides access to the underlying SFML SoundBuffer.

**Returns:** Pointer to the SFML `sf::SoundBuffer` instance

**Implementation Details:**
- Returns address of `m_buffer` member
- Allows direct SFML API access when needed
- Use with caution, bypasses interface abstraction

## Usage Examples

### Basic Audio Buffer Creation and Loading
```cpp
// Create an audio buffer instance
AudioBuffer* buffer = new AudioBuffer();

// Load from file
if (buffer->LoadFromFile("sound_effect.wav")) {
    std::cout << "Loaded audio: " << buffer->GetDuration() << " seconds"
              << " at " << buffer->GetSampleRate() << " Hz"
              << " with " << buffer->GetChannelCount() << " channels"
              << std::endl;
} else {
    std::cout << "Failed to load audio file" << std::endl;
}
```

### Loading from Engine Assets
```cpp
URessource& resourceManager = URessource::GetInstance();

// Load asset first
UAsset* audioAsset = resourceManager.LoadAsset("sounds/explosion.uasset");
if (audioAsset) {
    // Create buffer and load from asset
    AudioBuffer* buffer = new AudioBuffer();
    if (buffer->LoadFromAsset(audioAsset)) {
        // Audio loaded successfully from asset
        std::cout << "Asset audio duration: " << buffer->GetDuration() << "s" << std::endl;
    }
}
```

### Memory-Based Audio Loading
```cpp
// Load audio data into memory buffer
std::vector<Byte> audioData = LoadAudioDataFromNetwork();
AudioBuffer* buffer = new AudioBuffer();

if (buffer->LoadFromMemory(audioData)) {
    std::cout << "Loaded " << audioData.size() << " bytes of audio data" << std::endl;
    std::cout << "Duration: " << buffer->GetDuration() << " seconds" << std::endl;
}

// Or load from raw memory
const Byte* rawData = GetAudioDataPointer();
SizeT dataSize = GetAudioDataSize();

AudioBuffer* rawBuffer = new AudioBuffer();
if (rawBuffer->LoadFromMemory(rawData, dataSize)) {
    std::cout << "Loaded raw audio data successfully" << std::endl;
}
```

### Audio Buffer Pool Management
```cpp
class AudioBufferPool {
private:
    std::unordered_map<std::string, std::unique_ptr<AudioBuffer>> m_buffers;

public:
    AudioBuffer* GetOrLoadBuffer(const std::string& audioPath) {
        auto it = m_buffers.find(audioPath);
        if (it != m_buffers.end()) {
            return it->second.get();
        }

        // Create new buffer and load
        auto buffer = std::make_unique<AudioBuffer>();
        if (buffer->LoadFromFile(audioPath)) {
            AudioBuffer* ptr = buffer.get();
            m_buffers[audioPath] = std::move(buffer);
            return ptr;
        }

        return nullptr;
    }

    void PreloadCommonSounds() {
        const std::vector<std::string> commonSounds = {
            "ui_click.wav",
            "ui_hover.wav",
            "explosion.wav",
            "jump.wav"
        };

        for (const auto& sound : commonSounds) {
            GetOrLoadBuffer(sound);
        }
    }

    void GetStats() const {
        SizeT totalBuffers = m_buffers.size();
        Float32 totalDuration = 0.0f;

        for (const auto& pair : m_buffers) {
            totalDuration += pair.second->GetDuration();
        }

        std::cout << "Audio Buffer Pool Stats:" << std::endl;
        std::cout << "- Total buffers: " << totalBuffers << std::endl;
        std::cout << "- Total duration: " << totalDuration << " seconds" << std::endl;
    }
};
```

### Integration with Audio Sources
```cpp
class AudioManager {
private:
    std::vector<std::unique_ptr<AudioBuffer>> m_buffers;
    std::vector<std::unique_ptr<AudioSource>> m_sources;

public:
    AudioBuffer* LoadAudioBuffer(const std::string& path) {
        auto buffer = std::make_unique<AudioBuffer>();
        if (buffer->LoadFromFile(path)) {
            AudioBuffer* ptr = buffer.get();
            m_buffers.push_back(std::move(buffer));
            return ptr;
        }
        return nullptr;
    }

    AudioSource* CreateAudioSource(AudioBuffer* buffer) {
        if (!buffer) return nullptr;

        auto source = std::make_unique<AudioSource>();
        if (source->SetBuffer(buffer)) {
            AudioSource* ptr = source.get();
            m_sources.push_back(std::move(source));
            return ptr;
        }
        return nullptr;
    }

    void PlaySound(const std::string& soundPath) {
        AudioBuffer* buffer = LoadAudioBuffer(soundPath);
        if (buffer) {
            AudioSource* source = CreateAudioSource(buffer);
            if (source) {
                source->Play();
            }
        }
    }
};
```

### Audio Format Detection and Validation
```cpp
class AudioValidator {
public:
    struct AudioInfo {
        bool isValid;
        Float32 duration;
        UInt32 sampleRate;
        UInt32 channels;
        std::string format;
    };

    static AudioInfo ValidateAudioBuffer(const AudioBuffer* buffer) {
        AudioInfo info = {false, 0.0f, 0, 0, "unknown"};

        if (!buffer) return info;

        // Check if buffer has valid data
        if (buffer->GetDuration() > 0.0f) {
            info.isValid = true;
            info.duration = buffer->GetDuration();
            info.sampleRate = buffer->GetSampleRate();
            info.channels = buffer->GetChannelCount();

            // Determine format based on properties
            if (info.channels == 1) {
                info.format = "Mono";
            } else if (info.channels == 2) {
                info.format = "Stereo";
            } else {
                info.format = std::to_string(info.channels) + " channels";
            }

            // Add sample rate info
            info.format += " " + std::to_string(info.sampleRate) + "Hz";
        }

        return info;
    }

    static void PrintAudioInfo(const std::string& name, const AudioBuffer* buffer) {
        auto info = ValidateAudioInfo(buffer);

        std::cout << "Audio Buffer: " << name << std::endl;
        std::cout << "- Valid: " << (info.isValid ? "Yes" : "No") << std::endl;

        if (info.isValid) {
            std::cout << "- Duration: " << info.duration << " seconds" << std::endl;
            std::cout << "- Sample Rate: " << info.sampleRate << " Hz" << std::endl;
            std::cout << "- Channels: " << info.channels << std::endl;
            std::cout << "- Format: " << info.format << std::endl;
        }
    }
};
```

### Streaming Audio Preparation
```cpp
class AudioStreamer {
private:
    AudioBuffer* m_buffer;
    size_t m_currentSample;
    size_t m_samplesPerChunk;

public:
    AudioStreamer(AudioBuffer* buffer, size_t samplesPerChunk = 44100)
        : m_buffer(buffer), m_currentSample(0), m_samplesPerChunk(samplesPerChunk) {}

    bool GetNextChunk(std::vector<Int16>& chunk) {
        if (!m_buffer || m_buffer->GetDuration() == 0.0f) return false;

        // Get native SFML buffer for direct sample access
        sf::SoundBuffer* sfmlBuffer = static_cast<sf::SoundBuffer*>(
            m_buffer->GetNativeHandle()
        );

        const Int16* samples = sfmlBuffer->getSamples();
        size_t totalSamples = sfmlBuffer->getSampleCount();

        if (m_currentSample >= totalSamples) return false;

        size_t remainingSamples = totalSamples - m_currentSample;
        size_t chunkSize = std::min(m_samplesPerChunk, remainingSamples);

        chunk.assign(
            samples + m_currentSample,
            samples + m_currentSample + chunkSize
        );

        m_currentSample += chunkSize;
        return true;
    }

    void Reset() {
        m_currentSample = 0;
    }

    bool IsFinished() const {
        if (!m_buffer) return true;
        sf::SoundBuffer* sfmlBuffer = static_cast<sf::SoundBuffer*>(
            m_buffer->GetNativeHandle()
        );
        return m_currentSample >= sfmlBuffer->getSampleCount();
    }
};
```

### Audio Buffer Effects Processing
```cpp
class AudioProcessor {
public:
    static bool ApplyFadeIn(AudioBuffer* buffer, Float32 fadeTimeSeconds) {
        if (!buffer || fadeTimeSeconds <= 0.0f) return false;

        sf::SoundBuffer* sfmlBuffer = static_cast<sf::SoundBuffer*>(
            buffer->GetNativeHandle()
        );

        Int16* samples = const_cast<Int16*>(sfmlBuffer->getSamples());
        size_t sampleCount = sfmlBuffer->getSampleCount();
        UInt32 sampleRate = sfmlBuffer->getSampleRate();
        UInt32 channelCount = sfmlBuffer->getChannelCount();

        size_t fadeSamples = static_cast<size_t>(fadeTimeSeconds * sampleRate);

        for (size_t i = 0; i < fadeSamples && i < sampleCount; ++i) {
            Float32 fadeFactor = static_cast<Float32>(i) / fadeSamples;

            for (UInt32 channel = 0; channel < channelCount; ++channel) {
                size_t sampleIndex = i * channelCount + channel;
                samples[sampleIndex] = static_cast<Int16>(
                    samples[sampleIndex] * fadeFactor
                );
            }
        }

        return true;
    }

    static bool NormalizeAudio(AudioBuffer* buffer) {
        if (!buffer) return false;

        sf::SoundBuffer* sfmlBuffer = static_cast<sf::SoundBuffer*>(
            buffer->GetNativeHandle()
        );

        const Int16* samples = sfmlBuffer->getSamples();
        size_t sampleCount = sfmlBuffer->getSampleCount();

        // Find peak amplitude
        Int16 peak = 0;
        for (size_t i = 0; i < sampleCount; ++i) {
            peak = std::max(peak, static_cast<Int16>(std::abs(samples[i])));
        }

        if (peak == 0) return true;  // Already normalized or silent

        // Calculate normalization factor
        Float32 normalizeFactor = 32767.0f / peak;

        // Apply normalization
        Int16* mutableSamples = const_cast<Int16*>(samples);
        for (size_t i = 0; i < sampleCount; ++i) {
            mutableSamples[i] = static_cast<Int16>(
                mutableSamples[i] * normalizeFactor
            );
        }

        return true;
    }
};
```

## Mathematical Foundation

### Audio Properties
The audio buffer provides fundamental audio properties:

**Duration**: `duration = sampleCount / (sampleRate × channelCount)`
**Sample Rate**: Frequency of audio samples (Hz)
**Channel Count**: Number of audio channels (1 = mono, 2 = stereo)
**Bit Depth**: Audio resolution (16-bit PCM in SFML)

### Memory Calculation
```
Memory Usage = sampleCount × channelCount × bytesPerSample
               + SFML overhead (~100 bytes)

Where:
- sampleCount = duration × sampleRate
- bytesPerSample = 2 (for 16-bit audio)
```

### Audio Format Support
SFML supports multiple audio formats through format detection:
- **WAV**: Uncompressed PCM audio
- **OGG**: Compressed with Vorbis codec
- **FLAC**: Lossless compression
- **MP3**: MPEG-1 Audio Layer III (limited support)

## Performance Considerations

### Memory Usage
- **Per Buffer**: ~100 bytes SFML overhead + audio data size
- **16-bit Stereo**: ~172 KB per second of audio
- **16-bit Mono**: ~86 KB per second of audio
- **Compressed**: Variable (depends on codec efficiency)

### Loading Performance
- **File Loading**: I/O bound, depends on file size and disk speed
- **Memory Loading**: Fast, limited by memory copy speed
- **Asset Loading**: Depends on asset decompression
- **Format Decoding**: Varies by codec complexity

### Optimization Strategies
1. **Buffer Pooling**: Reuse AudioBuffer instances
2. **Streaming**: Load audio in chunks for long tracks
3. **Compression**: Use compressed formats for storage
4. **Preloading**: Load commonly used audio at startup
5. **Reference Counting**: Share buffers across multiple sources

### Best Practices
- Load audio on background threads when possible
- Use compressed formats for large audio files
- Implement audio buffer pooling for frequently used sounds
- Validate audio properties after loading
- Handle loading failures gracefully

## Diagrams

### Audio Buffer Loading Flow
```
Load Request ──► Source Type Check
                      │
                      ▼
            ┌─────────────────────┐
            │ FilePath:           │
            │ - Validate path     │
            │ - Call SFML loadFromFile │
            └─────────────────────┘
                      │
                      ▼
            ┌─────────────────────┐
            │ UAsset*:            │
            │ - Check asset valid │
            │ - Get asset data    │
            │ - Call SFML loadFromMemory │
            └─────────────────────┘
                      │
                      ▼
            ┌─────────────────────┐
            │ Memory:             │
            │ - Validate buffer   │
            │ - Call SFML loadFromMemory │
            └─────────────────────┘
                      │
                      ▼
            Success ──► AudioBuffer Ready
               │
               ▼
            Failure ──► Return false
```

### Audio Buffer Architecture
```
┌─────────────────┐
│   Application   │
│                 │
│ ┌─────────────┐ │
│ │ AudioBuffer │ │ ← IAudioBuffer implementation
│ │             │ │
│ └─────────────┘ │
└─────────────────┘
         │
         ▼
┌─────────────────┐
│ SFML           │
│ SoundBuffer    │ ← Native audio buffer
│                │
│ ┌────────────┐ │
│ │ Audio Data │ │ ← PCM samples
│ │ (16-bit)   │ │
│ └────────────┘ │
└─────────────────┘
         │
         ▼
┌─────────────────┐
│ Audio Source   │ ← Playback interface
│                │
│ ┌────────────┐ │
│ │ Playback   │ │
│ │ Controls   │ │
│ └────────────┐ │
└─────────────────┘
```

### Audio Data Flow
```
Audio File/Asset/Memory
           │
           ▼
    ┌─────────────┐     ┌─────────────┐
    │  Decoding   │────▶│  SFML       │
    │  (SFML)     │     │  SoundBuffer│
    └─────────────┘     └─────────────┘
           │                    │
           ▼                    ▼
    ┌─────────────┐     ┌─────────────┐
    │ Properties  │     │  Samples    │
    │ - Duration  │     │  - 16-bit   │
    │ - Channels  │     │  - PCM      │
    │ - SampleRate│     │  - Raw data │
    └─────────────┘     └─────────────┘
           │                    │
           ▼                    ▼
    ┌─────────────┐     ┌─────────────┐
    │ AudioBuffer │     │ AudioSource │
    │ Interface   │────▶│ Playback    │
    └─────────────┘     └─────────────┘
```

### Memory Layout
```
AudioBuffer Instance:
┌─────────────────────────────────────┐
│ vtable* (8 bytes)                   │ ← Virtual function table
├─────────────────────────────────────┤
│ m_buffer (sf::SoundBuffer)          │ ← SFML buffer object
│ ├─────────────────────────────────┤ │
│ │ Control Block (~32 bytes)       │ │
│ │ - Sample count                  │ │
│ │ - Sample rate                   │ │
│ │ - Channel count                 │ │
│ │ - Duration                      │ │
│ └─────────────────────────────────┘ │
│ ├─────────────────────────────────┤ │
│ │ Sample Data (variable)          │ │
│ │ - Int16 samples[]               │ │
│ │ - Size = samples × channels     │ │
│ └─────────────────────────────────┘ │
└─────────────────────────────────────┘
```

## Integration with Audio Pipeline

### Audio System Architecture
```cpp
class AudioSystem {
private:
    std::unordered_map<std::string, std::unique_ptr<AudioBuffer>> m_buffers;
    std::vector<std::unique_ptr<AudioSource>> m_sources;

public:
    AudioBuffer* LoadBuffer(const std::string& path) {
        auto it = m_buffers.find(path);
        if (it != m_buffers.end()) {
            return it->second.get();
        }

        auto buffer = std::make_unique<AudioBuffer>();
        if (buffer->LoadFromFile(path)) {
            AudioBuffer* ptr = buffer.get();
            m_buffers[path] = std::move(buffer);
            return ptr;
        }
        return nullptr;
    }

    AudioSource* PlaySound(const std::string& path, bool loop = false) {
        AudioBuffer* buffer = LoadBuffer(path);
        if (!buffer) return nullptr;

        auto source = std::make_unique<AudioSource>();
        if (source->SetBuffer(buffer) && source->Play()) {
            if (loop) source->SetLooping(true);
            AudioSource* ptr = source.get();
            m_sources.push_back(std::move(source));
            return ptr;
        }
        return nullptr;
    }

    void Update() {
        // Remove finished sources
        m_sources.erase(
            std::remove_if(m_sources.begin(), m_sources.end(),
                [](const std::unique_ptr<AudioSource>& source) {
                    return !source->IsPlaying();
                }),
            m_sources.end()
        );
    }
};
```

### Resource Manager Integration
```cpp
// Integration with UResource (when audio buffer support is added)
class AudioResourceManager {
private:
    URessource& m_resourceManager;

public:
    AudioResourceManager() : m_resourceManager(URessource::GetInstance()) {}

    FAudioBufferHandle LoadAudioBuffer(const std::string& path) {
        // This would be added to UResource when audio support is implemented
        // For now, create directly
        auto buffer = std::make_shared<AudioBuffer>();
        if (buffer->LoadFromFile(path)) {
            return FAudioBufferHandle(buffer, path);
        }
        return FAudioBufferHandle();
    }

    FAudioBufferHandle LoadAudioFromAsset(const std::string& assetPath) {
        UAsset* asset = m_resourceManager.LoadAsset(assetPath);
        if (asset) {
            auto buffer = std::make_shared<AudioBuffer>();
            if (buffer->LoadFromAsset(asset)) {
                return FAudioBufferHandle(buffer, asset->GetUUID());
            }
        }
        return FAudioBufferHandle();
    }
};
```

## Troubleshooting

### Common Issues

#### Loading Failures
**Problem**: `LoadFromFile()` or `LoadFromMemory()` returns false
**Solutions:**
- Verify file exists and is readable
- Check audio format is supported by SFML
- Validate memory buffer is not empty/corrupted
- Ensure asset data is valid

#### Invalid Audio Properties
**Problem**: `GetDuration()`, `GetSampleRate()`, or `GetChannelCount()` return 0
**Solutions:**
- Check if audio loaded successfully first
- Verify audio file is not corrupted
- Ensure correct audio format
- Check SFML audio support

#### Memory Issues
**Problem**: High memory usage or crashes with large audio files
**Solutions:**
- Use compressed audio formats (OGG, MP3)
- Implement streaming for long audio tracks
- Limit concurrent loaded audio buffers
- Monitor memory usage and unload unused buffers

#### Performance Problems
**Problem**: Audio loading causes frame drops
**Solutions:**
- Load audio asynchronously on background threads
- Preload commonly used audio at startup
- Use audio buffer pooling
- Cache loaded buffers

### Debug Visualization
```cpp
class AudioBufferDebugger {
public:
    static void PrintBufferInfo(const AudioBuffer* buffer, const std::string& name) {
        if (!buffer) {
            std::cout << "AudioBuffer [" << name << "]: NULL" << std::endl;
            return;
        }

        std::cout << "AudioBuffer [" << name << "]:" << std::endl;
        std::cout << "  - Duration: " << buffer->GetDuration() << " seconds" << std::endl;
        std::cout << "  - Sample Rate: " << buffer->GetSampleRate() << " Hz" << std::endl;
        std::cout << "  - Channels: " << buffer->GetChannelCount() << std::endl;

        // Calculate memory usage
        sf::SoundBuffer* sfmlBuffer = static_cast<sf::SoundBuffer*>(
            buffer->GetNativeHandle()
        );
        size_t sampleCount = sfmlBuffer->getSampleCount();
        size_t memoryUsage = sampleCount * sizeof(Int16);

        std::cout << "  - Samples: " << sampleCount << std::endl;
        std::cout << "  - Memory: " << (memoryUsage / 1024) << " KB" << std::endl;

        // Validate buffer
        bool isValid = buffer->GetDuration() > 0.0f &&
                      buffer->GetSampleRate() > 0 &&
                      buffer->GetChannelCount() > 0;

        std::cout << "  - Valid: " << (isValid ? "Yes" : "No") << std::endl;
    }

    static void AnalyzeAudioQuality(const AudioBuffer* buffer) {
        if (!buffer) return;

        UInt32 sampleRate = buffer->GetSampleRate();
        UInt32 channels = buffer->GetChannelCount();

        std::cout << "Audio Quality Analysis:" << std::endl;

        // Sample rate quality
        if (sampleRate >= 48000) {
            std::cout << "  - Sample Rate: High quality (≥48kHz)" << std::endl;
        } else if (sampleRate >= 44100) {
            std::cout << "  - Sample Rate: Standard quality (44.1kHz)" << std::endl;
        } else {
            std::cout << "  - Sample Rate: Low quality (<44.1kHz)" << std::endl;
        }

        // Channel configuration
        if (channels == 1) {
            std::cout << "  - Channels: Mono" << std::endl;
        } else if (channels == 2) {
            std::cout << "  - Channels: Stereo" << std::endl;
        } else {
            std::cout << "  - Channels: " << channels << " channels (surround)" << std::endl;
        }
    }
};
```

## Future Enhancements

### Potential Features
- **Streaming Support**: Load audio in chunks for memory efficiency
- **Audio Effects**: Built-in reverb, echo, filtering
- **Format Conversion**: Runtime audio format transcoding
- **MIDI Support**: Musical instrument digital interface
- **3D Audio**: Positional audio with HRTF
- **Procedural Audio**: Algorithmically generated audio

### API Extensions
```cpp
class IAudioBuffer {
    // Future methods
    virtual bool SaveToFile(const FilePath& filePath, AudioFormat format) = 0;
    virtual void* GetSampleData() const = 0;
    virtual size_t GetSampleCount() const = 0;
    virtual AudioFormat GetFormat() const = 0;
    virtual bool ConvertFormat(AudioFormat newFormat) = 0;
    virtual bool ApplyEffect(AudioEffect effect) = 0;
};

enum class AudioFormat {
    PCM_16,
    PCM_24,
    PCM_32,
    Float_32,
    ADPCM,
    MP3,
    Vorbis,
    FLAC
};

enum class AudioEffect {
    Normalize,
    FadeIn,
    FadeOut,
    Reverb,
    Echo,
    LowPass,
    HighPass
};
```
