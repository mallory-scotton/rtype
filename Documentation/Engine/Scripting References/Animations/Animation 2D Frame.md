# Animation 2D Frame Documentation

## Overview

The `FAnimation2DFrame` class is a fundamental building block of the TKD Game Engine's 2D animation system, representing a single frame within an animation sequence. It encapsulates the timing information and texture coordinates required to display one frame of a 2D sprite animation, providing the essential data structure for frame-based animation playback.

### Key Features

- **Timing Control**: Precise duration specification for each frame
- **Texture Mapping**: Rectangle-based texture coordinate specification
- **Lightweight Design**: Minimal memory footprint with simple data structure
- **Animation Integration**: Designed for use within 2D animation sequences
- **Flexible Construction**: Multiple constructor options for different use cases
- **Performance Optimized**: Efficient storage for real-time animation playback

### Architecture

```cpp
class FAnimation2DFrame
{
public:
    Float32 duration;          // Frame duration in seconds
    FRectanglei textureRect;   // Texture rectangle coordinates

    FAnimation2DFrame(
        const FRectanglei& rect = FRectanglei::Zero,
        Float32 duration = 0.0f
    );
};
```

The `FAnimation2DFrame` is a simple data structure that pairs timing information with texture coordinates, forming the atomic unit of 2D animation sequences.

## Core Components

### Timing Information

- **Duration**: Time in seconds this frame should be displayed
- **Float32 Precision**: High precision timing for smooth animations
- **Frame Rate Control**: Duration determines effective frame rate (1/duration)

### Texture Mapping

- **Rectangle Coordinates**: `FRectanglei` specifying texture region
- **Integer Precision**: Pixel-perfect texture coordinate specification
- **Atlas Support**: Enables texture atlas usage for efficient rendering

### Construction Options

- **Default Construction**: Zero rectangle, zero duration
- **Parameterized Construction**: Custom rectangle and duration
- **Copy Semantics**: Standard copy and move operations

## API Reference

### Constructor

#### `FAnimation2DFrame(const FRectanglei& rect = FRectanglei::Zero, Float32 duration = 0.0f)`

Creates a new 2D animation frame with specified texture rectangle and duration.

**Parameters:**
- `rect`: The texture rectangle defining the frame's region (default: `FRectanglei::Zero`)
- `duration`: The display duration in seconds (default: `0.0f`)

**Behavior:**
- Initializes `textureRect` with the provided rectangle
- Sets `duration` to the specified value
- Both parameters have sensible defaults for convenience

### Public Members

#### `Float32 duration`

The time in seconds that this frame should be displayed during animation playback.

**Type:** `Float32`
**Default:** `0.0f`
**Range:** `>= 0.0f`
**Units:** Seconds
**Usage:** Determines how long the frame appears before advancing to the next

#### `FRectanglei textureRect`

The rectangular region of the texture that contains this animation frame.

**Type:** `FRectanglei`
**Default:** `FRectanglei::Zero` (0, 0, 0, 0)
**Units:** Pixels
**Usage:** Defines which portion of the texture to render for this frame

## Usage Examples

### Basic Frame Creation

```cpp
// Create a frame with default values (not very useful)
FAnimation2DFrame defaultFrame;

// Create a frame for a 32x32 sprite at position (0,0) lasting 0.1 seconds
FRectanglei spriteRect(0, 0, 32, 32);
FAnimation2DFrame frame1(spriteRect, 0.1f);

// Create a frame from a texture atlas region
FRectanglei atlasRect(64, 128, 32, 32); // Region in atlas
FAnimation2DFrame frame2(atlasRect, 0.083f); // ~12 FPS
```

### Animation Sequence Construction

```cpp
class CharacterAnimation {
private:
    std::vector<FAnimation2DFrame> walkFrames;

public:
    void LoadWalkAnimation() {
        // Assuming 8-frame walking animation, 32x32 sprites
        const int FRAME_WIDTH = 32;
        const int FRAME_HEIGHT = 32;
        const Float32 FRAME_DURATION = 0.1f; // 10 FPS

        for (int i = 0; i < 8; ++i) {
            // Horizontal sprite sheet layout
            FRectanglei frameRect(i * FRAME_WIDTH, 0, FRAME_WIDTH, FRAME_HEIGHT);
            walkFrames.emplace_back(frameRect, FRAME_DURATION);
        }
    }

    const std::vector<FAnimation2DFrame>& GetWalkFrames() const {
        return walkFrames;
    }
};
```

### Variable Frame Rate Animation

```cpp
class DynamicAnimationBuilder {
public:
    static std::vector<FAnimation2DFrame> CreateVariableRateAnimation(
        const std::vector<FRectanglei>& rects,
        const std::vector<Float32>& durations
    ) {
        std::vector<FAnimation2DFrame> frames;
        frames.reserve(rects.size());

        for (size_t i = 0; i < rects.size(); ++i) {
            Float32 duration = (i < durations.size()) ? durations[i] : 0.1f;
            frames.emplace_back(rects[i], duration);
        }

        return frames;
    }

    // Example: Slow start, fast middle, slow end
    std::vector<FAnimation2DFrame> CreateAttackAnimation() {
        std::vector<FRectanglei> rects = {
            FRectanglei(0, 0, 32, 32),    // Wind-up
            FRectanglei(32, 0, 32, 32),   // Strike
            FRectanglei(64, 0, 32, 32),   // Follow-through
            FRectanglei(96, 0, 32, 32)    // Recovery
        };

        std::vector<Float32> durations = {
            0.2f,  // Slow wind-up
            0.05f, // Fast strike
            0.1f,  // Medium follow-through
            0.15f  // Slow recovery
        };

        return CreateVariableRateAnimation(rects, durations);
    }
};
```

### Texture Atlas Integration

```cpp
class TextureAtlasManager {
private:
    FTextureHandle atlasTexture;
    std::unordered_map<FString, std::vector<FAnimation2DFrame>> animations;

public:
    void LoadCharacterAnimations() {
        // Assuming atlas is 256x256 with 32x32 frames
        const int FRAME_SIZE = 32;
        const int ATLAS_WIDTH = 256;
        const int FRAMES_PER_ROW = ATLAS_WIDTH / FRAME_SIZE;

        // Load idle animation (4 frames in first row)
        std::vector<FAnimation2DFrame> idleFrames;
        for (int i = 0; i < 4; ++i) {
            FRectanglei rect(i * FRAME_SIZE, 0, FRAME_SIZE, FRAME_SIZE);
            idleFrames.emplace_back(rect, 0.15f);
        }
        animations["Idle"] = std::move(idleFrames);

        // Load run animation (6 frames in second row)
        std::vector<FAnimation2DFrame> runFrames;
        for (int i = 0; i < 6; ++i) {
            FRectanglei rect(i * FRAME_SIZE, FRAME_SIZE, FRAME_SIZE, FRAME_SIZE);
            runFrames.emplace_back(rect, 0.08f); // Faster animation
        }
        animations["Run"] = std::move(runFrames);
    }

    const std::vector<FAnimation2DFrame>* GetAnimation(const FString& name) const {
        auto it = animations.find(name);
        return it != animations.end() ? &it->second : nullptr;
    }
};
```

### Animation Playback System

```cpp
class FrameBasedAnimator {
private:
    const std::vector<FAnimation2DFrame>* currentAnimation;
    size_t currentFrameIndex;
    Float32 frameTimer;
    bool isPlaying;
    bool isLooping;

public:
    void PlayAnimation(const std::vector<FAnimation2DFrame>& animation, bool loop = true) {
        currentAnimation = &animation;
        currentFrameIndex = 0;
        frameTimer = 0.0f;
        isPlaying = true;
        isLooping = loop;
    }

    void Update(Float32 deltaTime) {
        if (!isPlaying || !currentAnimation || currentAnimation->empty()) {
            return;
        }

        frameTimer += deltaTime;

        const FAnimation2DFrame& currentFrame = (*currentAnimation)[currentFrameIndex];

        // Check if current frame duration has elapsed
        if (frameTimer >= currentFrame.duration) {
            frameTimer -= currentFrame.duration;
            currentFrameIndex++;

            // Handle animation completion
            if (currentFrameIndex >= currentAnimation->size()) {
                if (isLooping) {
                    currentFrameIndex = 0;
                } else {
                    isPlaying = false;
                    currentFrameIndex = currentAnimation->size() - 1; // Stay on last frame
                }
            }
        }
    }

    const FRectanglei* GetCurrentFrameRect() const {
        if (!currentAnimation || currentAnimation->empty() || !isPlaying) {
            return nullptr;
        }
        return &(*currentAnimation)[currentFrameIndex].textureRect;
    }

    bool IsPlaying() const { return isPlaying; }
    size_t GetCurrentFrameIndex() const { return currentFrameIndex; }
    Float32 GetFrameProgress() const {
        if (!currentAnimation || currentAnimation->empty()) return 0.0f;
        const FAnimation2DFrame& frame = (*currentAnimation)[currentFrameIndex];
        return frameTimer / frame.duration;
    }
};
```

## Animation Integration

### Frame Sequence Management

Frames are typically managed as collections within animation sequences:

```cpp
class AnimationSequence {
private:
    std::vector<FAnimation2DFrame> frames;
    FString name;

public:
    void AddFrame(const FRectanglei& rect, Float32 duration) {
        frames.emplace_back(rect, duration);
    }

    Float32 GetTotalDuration() const {
        Float32 total = 0.0f;
        for (const auto& frame : frames) {
            total += frame.duration;
        }
        return total;
    }

    Float32 GetFrameRate() const {
        if (frames.empty()) return 0.0f;
        return frames.size() / GetTotalDuration();
    }

    const std::vector<FAnimation2DFrame>& GetFrames() const {
        return frames;
    }
};
```

### Rendering Integration

Frames are used during rendering to specify texture coordinates:

```cpp
class SpriteRenderer {
public:
    void RenderAnimatedSprite(const FrameBasedAnimator& animator,
                             const FVector2& position,
                             const FTextureHandle& texture) {
        const FRectanglei* frameRect = animator.GetCurrentFrameRect();
        if (frameRect) {
            // Render using the frame's texture rectangle
            RenderTextureRegion(texture, position, *frameRect);
        }
    }

private:
    void RenderTextureRegion(const FTextureHandle& texture,
                           const FVector2& position,
                           const FRectanglei& rect) {
        // Implementation depends on rendering system
        // Use rect to specify UV coordinates for texture sampling
    }
};
```

## Performance Considerations

### Memory Usage

- **Per Frame**: Minimal memory footprint (~16 bytes)
- **Frame Collections**: Linear scaling with animation complexity
- **Texture Sharing**: Frames reference shared texture resources

### CPU Performance

- **Frame Access**: O(1) for indexed frame access
- **Animation Update**: Minimal computation per frame
- **Memory Layout**: Cache-friendly contiguous storage

### Optimization Strategies

1. **Frame Reuse**: Share common frames across animations
2. **Atlas Packing**: Optimize texture rectangle layouts
3. **Batch Operations**: Process multiple frames simultaneously

### Best Practices

- **Consistent Frame Sizes**: Use uniform dimensions for better performance
- **Reasonable Durations**: Keep frame durations in practical ranges (0.01f - 1.0f)
- **Atlas Usage**: Prefer texture atlases over individual textures
- **Frame Limits**: Limit animation frame counts (8-32 frames typical)

## Architecture Diagrams

### Class Structure

```
FAnimation2DFrame
├── duration: Float32          // Frame display time in seconds
└── textureRect: FRectanglei   // Texture coordinate rectangle
    ├── left: int             // Left coordinate
    ├── top: int              // Top coordinate
    ├── width: int            // Rectangle width
    └── height: int           // Rectangle height
```

### Animation Sequence Flow

```
Animation Sequence ──► Frame Collection ──► Current Frame ──► Texture Rect
         │                        │                │              │
         ▼                        ▼                ▼              ▼
    Frame Count ──► Frame Index ──► Duration ──► Timer ──► UV Coordinates
         ▲                        ▲                ▲              ▲
         │                        │                │              │
         └───── Playback ─────────┴───── Update ───┴───── Render ──┘
```

### Frame Timing Diagram

```
Animation Timeline
├── Frame 0: [0.0s ─── 0.1s] ──► textureRect(0,0,32,32)
├── Frame 1: [0.1s ─── 0.2s] ──► textureRect(32,0,32,32)
├── Frame 2: [0.2s ─── 0.25s] ──► textureRect(64,0,32,32)
└── Frame 3: [0.25s ── 0.3s] ──► textureRect(96,0,32,32)

Frame Timer: 0.0 ──► duration ──► Next Frame
```

## Troubleshooting

### Common Issues

#### Invalid Texture Rectangles

**Symptoms:** Animation frames show wrong or no texture regions

**Possible Causes:**
- Rectangle coordinates outside texture bounds
- Negative width/height values
- Incorrect coordinate system (origin issues)

**Solutions:**
```cpp
// Validate texture rectangle
bool IsValidFrameRect(const FRectanglei& rect, const FTextureHandle& texture) {
    if (rect.width <= 0 || rect.height <= 0) {
        FLogger::Log(ELogLevel::Error, "Invalid frame rectangle dimensions");
        return false;
    }

    // Assuming texture has GetWidth() and GetHeight() methods
    if (rect.left + rect.width > texture.GetWidth() ||
        rect.top + rect.height > texture.GetHeight()) {
        FLogger::Log(ELogLevel::Error, "Frame rectangle exceeds texture bounds");
        return false;
    }

    return true;
}

// Clamp rectangle to texture bounds
FRectanglei ClampToTextureBounds(const FRectanglei& rect, const FTextureHandle& texture) {
    FRectanglei clamped = rect;
    clamped.left = std::max(0, std::min(clamped.left, texture.GetWidth() - clamped.width));
    clamped.top = std::max(0, std::min(clamped.top, texture.GetHeight() - clamped.height));
    return clamped;
}
```

#### Zero Duration Frames

**Symptoms:** Animation advances too quickly or instantly

**Possible Causes:**
- Duration set to 0.0f
- Uninitialized duration values
- Incorrect duration calculations

**Solutions:**
```cpp
// Validate frame duration
const Float32 MIN_FRAME_DURATION = 0.016f; // ~60 FPS minimum

bool ValidateFrameDuration(Float32 duration) {
    if (duration <= 0.0f) {
        FLogger::Log(ELogLevel::Warning,
            FString::Format("Invalid frame duration: {}", duration));
        return false;
    }

    if (duration < MIN_FRAME_DURATION) {
        FLogger::Log(ELogLevel::Warning,
            FString::Format("Frame duration too short: {}s (min: {}s)",
                duration, MIN_FRAME_DURATION));
    }

    return true;
}

// Auto-calculate reasonable duration
Float32 CalculateFrameDuration(int frameCount, Float32 targetFPS = 12.0f) {
    if (frameCount <= 0) return 0.1f;
    return 1.0f / targetFPS;
}
```

#### Memory Issues

**Symptoms:** High memory usage with many animation frames

**Possible Causes:**
- Duplicate frame data
- Inefficient frame storage
- Large texture rectangles

**Solutions:**
```cpp
class FrameOptimizer {
public:
    // Remove duplicate frames
    static void OptimizeFrameList(std::vector<FAnimation2DFrame>& frames) {
        std::vector<FAnimation2DFrame> optimized;

        for (const auto& frame : frames) {
            // Check if frame already exists
            auto it = std::find_if(optimized.begin(), optimized.end(),
                [&frame](const FAnimation2DFrame& existing) {
                    return existing.textureRect == frame.textureRect &&
                           std::abs(existing.duration - frame.duration) < 0.001f;
                });

            if (it == optimized.end()) {
                optimized.push_back(frame);
            }
        }

        frames = std::move(optimized);
    }

    // Compress frame durations (group similar durations)
    static void CompressDurations(std::vector<FAnimation2DFrame>& frames,
                                Float32 tolerance = 0.01f) {
        for (auto& frame : frames) {
            // Round duration to nearest tolerance
            frame.duration = std::round(frame.duration / tolerance) * tolerance;
        }
    }
};
```

### Performance Issues

**Symptoms:** Animation playback is choppy or slow

**Possible Causes:**
- Too many frames in animation
- Small frame durations causing rapid updates
- Inefficient frame access patterns

**Solutions:**
```cpp
// Profile animation performance
class AnimationProfiler {
public:
    static void ProfileAnimation(const std::vector<FAnimation2DFrame>& frames) {
        if (frames.empty()) return;

        Float32 totalDuration = 0.0f;
        Float32 minDuration = std::numeric_limits<Float32>::max();
        Float32 maxDuration = 0.0f;

        for (const auto& frame : frames) {
            totalDuration += frame.duration;
            minDuration = std::min(minDuration, frame.duration);
            maxDuration = std::max(maxDuration, frame.duration);
        }

        Float32 avgDuration = totalDuration / frames.size();
        Float32 frameRate = frames.size() / totalDuration;

        FLogger::Log(ELogLevel::Info,
            FString::Format("Animation Profile: {} frames, {}s total, {} FPS avg",
                frames.size(), totalDuration, frameRate));
        FLogger::Log(ELogLevel::Info,
            FString::Format("Duration Range: {}s - {}s (avg: {}s)",
                minDuration, maxDuration, avgDuration));
    }
};
```

### Debug Tools

Enable frame debugging:

```cpp
class FrameDebugger {
public:
    static void LogFrameInfo(const FAnimation2DFrame& frame, size_t index) {
        FLogger::Log(ELogLevel::Debug, "=== Animation Frame Info ===");
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Frame {}: Duration {}s", index, frame.duration));
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Texture Rect: ({}, {}, {}, {})",
                frame.textureRect.left, frame.textureRect.top,
                frame.textureRect.width, frame.textureRect.height));
    }

    static void LogAnimationSequence(const std::vector<FAnimation2DFrame>& frames) {
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Animation Sequence: {} frames", frames.size()));

        Float32 totalTime = 0.0f;
        for (size_t i = 0; i < frames.size(); ++i) {
            LogFrameInfo(frames[i], i);
            totalTime += frames[i].duration;
        }

        FLogger::Log(ELogLevel::Debug,
            FString::Format("Total Duration: {}s, Avg FPS: {}",
                totalTime, frames.size() / totalTime));
    }
};
```

## Future Enhancements

### Planned Features

1. **Frame Interpolation**: Smooth blending between frames
2. **Variable Timing**: Non-linear frame timing curves
3. **Frame Events**: Callbacks at specific frame times
4. **Frame Metadata**: Additional frame-specific data storage
5. **Compression**: Frame data compression for memory efficiency
6. **Streaming**: Progressive frame loading for large animations
7. **Frame Effects**: Per-frame visual effects and transformations

### Extension Points

The frame system allows for easy extension:

```cpp
class AdvancedAnimationFrame : public FAnimation2DFrame {
public:
    // Add frame events
    TFunction<void()> onFrameEnter;
    TFunction<void()> onFrameExit;

    // Add frame metadata
    std::unordered_map<FString, FString> metadata;

    // Add transformation data
    FVector2 offset;
    Float32 rotation;
    FVector2 scale;

    // Add blending information
    enum class BlendMode { Alpha, Additive, Multiply };
    BlendMode blendMode;

    AdvancedAnimationFrame(const FRectanglei& rect = FRectanglei::Zero,
                          Float32 duration = 0.0f)
        : FAnimation2DFrame(rect, duration)
        , offset(0.0f, 0.0f)
        , rotation(0.0f)
        , scale(1.0f, 1.0f)
        , blendMode(BlendMode::Alpha)
    {}

    void SetMetadata(const FString& key, const FString& value) {
        metadata[key] = value;
    }

    const FString* GetMetadata(const FString& key) const {
        auto it = metadata.find(key);
        return it != metadata.end() ? &it->second : nullptr;
    }
};
```

### Advanced Features

Future versions may include:
- **GPU Acceleration**: Hardware-accelerated frame processing
- **Procedural Frames**: Runtime-generated frame data
- **Frame Prediction**: Predictive frame loading
- **Multi-resolution**: Adaptive frame quality based on distance
- **Frame Analysis**: Automatic frame optimization and analysis

---

*This documentation covers the complete FAnimation2DFrame implementation as of TKD Engine v1.0.0. For the latest updates and additional examples, refer to the engine's source code and test suites.*
