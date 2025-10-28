# Flip Book Documentation

## Overview

The `UFlipBook` class is a specialized animation component in the TKD Game Engine that manages flipbook animations - sequences of static images played in rapid succession to create the illusion of motion. It provides comprehensive frame management, time-based playback control, and efficient resource handling for 2D sprite animations.

### Key Features

- **Frame Sequence Management**: Flexible frame path and texture handle storage
- **Time-Based Animation**: Precise control over animation timing and playback
- **Resource Management**: Lazy loading and unloading of texture resources
- **Frame Access**: Multiple ways to access frames (by index, time, or current position)
- **Memory Efficient**: Optional loading/unloading to manage memory usage
- **Loop Support**: Automatic wrapping for continuous playback
- **Duration Control**: Flexible duration settings with automatic calculation

### Architecture

```cpp
class UFlipBook : public UObject
{
private:
    std::vector<FilePath> m_framePaths;
    Float32 m_duration;
    std::vector<FTextureHandle> m_frames;
    Float32 m_currentTime;
    Bool m_isLoaded;
    SizeT m_currentFrameIndex;
    bool m_hardDuration;
};
```

The `UFlipBook` inherits from `UObject` and integrates with the engine's reflection system, resource management, and object lifecycle.

## Core Components

### Frame Storage System

The flipbook maintains two parallel storage systems:

- **Frame Paths**: File system paths to image files (`std::vector<FilePath>`)
- **Loaded Frames**: Texture handles for loaded resources (`std::vector<FTextureHandle>`)

### Time Management

- **Duration**: Total animation length in seconds
- **Current Time**: Current playback position within the animation
- **Frame Index**: Current frame being displayed

### Resource State

- **Load State**: Whether textures are currently loaded in memory
- **Hard Duration**: Whether duration is user-specified or auto-calculated

## API Reference

### Constructor & Destructor

#### `UFlipBook(const FString& name = "UFlipBook")`

Creates a new flipbook instance.

**Parameters:**
- `name`: Optional flipbook name (defaults to "UFlipBook")

**Behavior:**
- Initializes all member variables to default values
- Sets duration to 0.0f, current time to 0.0f
- Marks as not loaded, frame index to 0
- Hard duration set to false

#### `virtual ~UFlipBook() = default`

Default destructor with proper cleanup through base class.

### Frame Management

#### `void SetFramePaths(const std::vector<FilePath>& paths)`

Sets all frame paths for the flipbook animation.

**Parameters:**
- `paths`: Vector of file paths to frame images

**Behavior:**
- Replaces existing frame paths
- Marks flipbook as not loaded (invalidates loaded textures)
- Auto-calculates duration if not hard-set (frames / 0.5fps)

#### `void AddFrame(const FilePath& framePath)`

Adds a single frame to the flipbook sequence.

**Parameters:**
- `framePath`: File path to the frame image

**Behavior:**
- Appends frame path to existing sequence
- Marks flipbook as not loaded
- Auto-calculates duration if not hard-set

#### `void ClearFrames(void)`

Removes all frames and frame paths from the flipbook.

**Behavior:**
- Clears both frame paths and loaded textures
- Resets load state to false
- Preserves duration and time settings

### Duration Control

#### `void SetDuration(Float32 duration)`

Sets the total animation duration.

**Parameters:**
- `duration`: Animation length in seconds

**Behavior:**
- Sets duration to specified value
- Marks duration as "hard-set" (prevents auto-calculation)
- Does not affect current playback position

#### `Float32 GetDuration(void) const`

Retrieves the current animation duration.

**Returns:** Animation duration in seconds

### Frame Information

#### `SizeT GetFrameCount(void) const`

Gets the total number of frames in the flipbook.

**Returns:** Number of frames (size of frame paths vector)

#### `const std::vector<FilePath>& GetFramePaths(void) const`

Retrieves all frame file paths.

**Returns:** Const reference to frame paths vector

#### `const std::vector<FTextureHandle>& GetFrames(void) const`

Retrieves all loaded frame textures.

**Returns:** Const reference to texture handles vector

### Frame Access by Index

#### `FilePath GetFramePath(SizeT index) const`

Gets the file path for a frame at the specified index.

**Parameters:**
- `index`: Frame index (will wrap around if out of bounds)

**Returns:** File path to the frame image

**Behavior:**
- Uses modulo arithmetic for index wrapping
- Returns empty path if no frames exist

#### `FTextureHandle GetFrame(SizeT index) const`

Gets the texture handle for a frame at the specified index.

**Parameters:**
- `index`: Frame index (will wrap around if out of bounds)

**Returns:** Texture handle to the loaded frame

**Behavior:**
- Uses modulo arithmetic for index wrapping
- Returns invalid handle if no frames loaded

### Frame Access by Time

#### `FilePath GetFramePathAt(Float32 time) const`

Gets the file path for the frame at the specified animation time.

**Parameters:**
- `time`: Time position in seconds (will wrap around duration)

**Returns:** File path to the frame at that time

**Behavior:**
- Calculates frame index based on time/duration ratio
- Uses modulo for time wrapping
- Returns empty path if no frames or invalid duration

#### `FTextureHandle GetFrameAt(Float32 time) const`

Gets the texture handle for the frame at the specified animation time.

**Parameters:**
- `time`: Time position in seconds (will wrap around duration)

**Returns:** Texture handle to the frame at that time

**Behavior:**
- Calculates frame index based on time/duration ratio
- Uses modulo for time wrapping
- Returns invalid handle if no frames loaded or invalid duration

### Current Frame Access

#### `FilePath GetCurrentFramePath(void) const`

Gets the file path of the currently active frame.

**Returns:** File path to the current frame

**Behavior:**
- Based on current frame index
- Returns empty path if no frames exist

#### `FTextureHandle GetCurrentFrame(void) const`

Gets the texture handle of the currently active frame.

**Returns:** Texture handle to the current frame

**Behavior:**
- Based on current frame index
- Returns invalid handle if no frames loaded

### Resource Management

#### `Bool LoadFrames(void)`

Loads all frame textures into memory.

**Returns:** `true` if all frames loaded successfully, `false` otherwise

**Behavior:**
- Clears existing loaded frames
- Loads each frame path as texture resource
- Sets load state based on success
- Fails if any frame cannot be loaded

#### `void UnloadFrames(void)`

Unloads all frame textures from memory.

**Behavior:**
- Clears texture handles vector
- Sets load state to false
- Preserves frame paths for later reloading

#### `Bool IsLoaded(void) const`

Checks if frame textures are currently loaded.

**Returns:** `true` if frames are loaded, `false` otherwise

### Animation Control

#### `void SetCurrentTime(Float32 time)`

Sets the current playback position.

**Parameters:**
- `time`: New time position in seconds

**Behavior:**
- Uses modulo to wrap time within duration
- Does not update frame index (call Update() for that)

#### `Float32 GetCurrentTime(void) const`

Gets the current playback position.

**Returns:** Current time in seconds

#### `void Update(Float32 deltaTime)`

Advances the animation by the specified time delta.

**Parameters:**
- `deltaTime`: Time elapsed since last update in seconds

**Behavior:**
- Increments current time
- Calculates new frame index based on time/duration ratio
- Wraps time around duration for looping
- No-op if no frames or invalid duration

#### `void Restart(void)`

Resets the animation to the beginning.

**Behavior:**
- Sets current time to 0.0f
- Frame index will update on next Update() call

## Usage Examples

### Basic Flipbook Creation

```cpp
// Create a simple explosion animation
UFlipBook* explosion = new UFlipBook("ExplosionAnim");

// Add frames (assuming 8 frames)
explosion->AddFrame("Assets/Textures/Explosion/001.png");
explosion->AddFrame("Assets/Textures/Explosion/002.png");
explosion->AddFrame("Assets/Textures/Explosion/003.png");
// ... add remaining frames

// Set custom duration (2 seconds for 8 frames = 4 fps)
explosion->SetDuration(2.0f);

// Load textures
if (explosion->LoadFrames()) {
    FLogger::Log(ELogLevel::Info, "Explosion animation loaded successfully");
} else {
    FLogger::Log(ELogLevel::Error, "Failed to load explosion frames");
}
```

### Animation Playback

```cpp
class AnimatedSprite : public UObject
{
private:
    UFlipBook* m_flipbook;
    Float32 m_animationTime;

public:
    void Initialize()
    {
        m_flipbook = new UFlipBook("CharacterWalk");
        // Setup frames...
        m_flipbook->LoadFrames();
        m_animationTime = 0.0f;
    }

    void Update(Float32 deltaTime)
    {
        if (m_flipbook && m_flipbook->IsLoaded())
        {
            m_animationTime += deltaTime;
            m_flipbook->Update(deltaTime);

            // Get current frame for rendering
            FTextureHandle currentFrame = m_flipbook->GetCurrentFrame();
            if (currentFrame.IsValid())
            {
                // Render the current frame
                RenderTexture(currentFrame);
            }
        }
    }

    void PlayAnimation()
    {
        m_flipbook->Restart();
    }
};
```

### Time-Based Frame Sampling

```cpp
class AnimationController
{
private:
    UFlipBook* m_animation;

public:
    void ScrubAnimation(Float32 time)
    {
        if (m_animation)
        {
            // Get frame at specific time for preview/scrubbing
            FTextureHandle frame = m_animation->GetFrameAt(time);
            if (frame.IsValid())
            {
                PreviewFrame(frame);
            }
        }
    }

    void GetAnimationInfo()
    {
        if (m_animation)
        {
            SizeT frameCount = m_animation->GetFrameCount();
            Float32 duration = m_animation->GetDuration();
            Float32 fps = frameCount / duration;

            FLogger::Log(ELogLevel::Info,
                FString::Format("Animation: {} frames, {}s duration, {} fps",
                    frameCount, duration, fps));
        }
    }
};
```

### Memory Management

```cpp
class LevelManager
{
private:
    std::vector<UFlipBook*> m_animations;

public:
    void LoadLevelAnimations()
    {
        // Load critical animations
        for (auto* anim : m_animations)
        {
            if (anim->LoadFrames())
            {
                FLogger::Log(ELogLevel::Info,
                    FString::Format("Loaded animation: {}", anim->GetName()));
            }
        }
    }

    void UnloadLevelAnimations()
    {
        // Free memory when level unloads
        for (auto* anim : m_animations)
        {
            anim->UnloadFrames();
            FLogger::Log(ELogLevel::Info,
                FString::Format("Unloaded animation: {}", anim->GetName()));
        }
    }

    void OnLowMemory()
    {
        // Unload non-critical animations
        for (auto* anim : m_animations)
        {
            if (!IsCriticalAnimation(anim))
            {
                anim->UnloadFrames();
            }
        }
    }
};
```

## Resource Management

### Loading Strategy

The flipbook supports two loading strategies:

1. **Eager Loading**: Load all frames immediately with `LoadFrames()`
2. **Lazy Loading**: Load frames on-demand when first accessed

### Memory Considerations

- **Frame Storage**: Each frame path stored as `FilePath` (lightweight)
- **Texture Storage**: Loaded frames consume GPU memory
- **Load State**: Tracks whether textures are resident in memory

### Resource Dependencies

Flipbook frames are loaded through the `URessource` system:

```cpp
// Frames loaded via resource manager
m_frames.push_back(URessource::LoadResource<ITexture>(path));
```

This provides automatic resource management, caching, and reference counting.

## Animation System Integration

### Time-Based Animation

The flipbook uses normalized time for frame calculation:

```
frameIndex = (currentTime / duration) * frameCount
```

This ensures smooth animation regardless of frame count or duration.

### Looping Behavior

- **Time Wrapping**: `currentTime = fmod(currentTime, duration)`
- **Frame Wrapping**: `frameIndex = frameIndex % frameCount`
- **Seamless Loops**: No visible stuttering at loop boundaries

### Frame Interpolation

While the flipbook doesn't perform temporal interpolation between frames, the time-based access methods allow for:

- **Scrubbing**: Jump to any point in the animation
- **Variable Speed**: Control playback speed via deltaTime
- **Reverse Playback**: Use negative deltaTime values

## Performance Considerations

### Memory Usage

- **Unloaded State**: Minimal memory (only file paths)
- **Loaded State**: GPU memory for all frame textures
- **Frame Count**: Linear scaling with number of frames

### CPU Performance

- **Update Cost**: Minimal (simple arithmetic)
- **Frame Access**: O(1) for index-based, O(1) for time-based
- **Loading Cost**: O(n) where n = frame count

### Optimization Strategies

1. **Unload Unused Animations**: Free memory when animations not in use
2. **Preload Critical Assets**: Load essential animations at level start
3. **Texture Atlasing**: Consider combining frames into texture atlases
4. **Compression**: Use appropriate texture compression formats

### Best Practices

- **Load Timing**: Load animations during loading screens, not gameplay
- **Unload Strategy**: Unload when switching levels or states
- **Frame Limits**: Keep frame counts reasonable (16-64 frames typical)
- **Resolution Consistency**: Use consistent frame resolutions

## Architecture Diagrams

### Class Hierarchy

```
UObject (Base Object)
    ├── UUID: Unique identifier
    ├── Name: Object name
    ├── Properties: Reflection system
    └── Events: Object lifecycle
    └── RPC System
    └── Serialization

UFlipBook (Animation Component)
    ├── Frame Paths: std::vector<FilePath>
    ├── Duration: Float32
    ├── Loaded Frames: std::vector<FTextureHandle>
    ├── Current Time: Float32
    ├── Load State: Bool
    ├── Frame Index: SizeT
    └── Hard Duration: bool
```

### Animation Flow

```
Frame Paths ──► LoadFrames() ──► Texture Handles
       │                │
       │                ▼
       │           IsLoaded = true
       ▼
   Duration ──► Update(deltaTime) ──► Current Frame
       ▲                │                    │
       │                ▼                    ▼
   Auto-calc     Current Time        Rendering System
   (frames/0.5)  (modulo duration)    (ITexture*)
```

### Resource Management Flow

```
Create FlipBook ──► SetFramePaths() ──► Frame Paths Stored
          │                    │
          ▼                    ▼
     LoadFrames() ──► Resource Manager ──► Textures Loaded
          │                    │
          ▼                    ▼
     IsLoaded = true     Memory Allocated
          │
          ▼
     Update() ──► Animation Playback
          │
          ▼
     UnloadFrames() ──► Memory Freed
```

## Troubleshooting

### Common Issues

#### Frames Not Loading

**Symptoms:** `LoadFrames()` returns false, `IsLoaded()` returns false

**Possible Causes:**
- Invalid file paths
- Missing image files
- Unsupported image formats
- Resource manager not initialized

**Solutions:**
```cpp
// Check file existence
for (const auto& path : flipbook->GetFramePaths()) {
    if (!std::filesystem::exists(path)) {
        FLogger::Log(ELogLevel::Error,
            FString::Format("Frame file not found: {}", path.string()));
    }
}

// Verify resource manager
if (!URessource::IsInitialized()) {
    FLogger::Log(ELogLevel::Error, "Resource manager not initialized");
    UResource::Initialize();
}
```

#### Animation Not Playing

**Symptoms:** Current frame doesn't change, animation appears static

**Possible Causes:**
- Duration is 0 or negative
- No frames loaded
- Update() not called
- Delta time is 0

**Solutions:**
```cpp
// Check animation state
if (flipbook->GetDuration() <= 0.0f) {
    FLogger::Log(ELogLevel::Warning, "Animation duration is zero or negative");
    flipbook->SetDuration(1.0f); // Set default duration
}

if (!flipbook->IsLoaded()) {
    FLogger::Log(ELogLevel::Warning, "Animation frames not loaded");
    flipbook->LoadFrames();
}

// Ensure update is called
void GameLoop() {
    Float32 deltaTime = CalculateDeltaTime();
    if (deltaTime > 0.0f) {
        flipbook->Update(deltaTime);
    }
}
```

#### Memory Leaks

**Symptoms:** Memory usage increases over time, textures not freed

**Possible Causes:**
- Not calling `UnloadFrames()`
- Flipbook objects not destroyed
- Resource manager holding references

**Solutions:**
```cpp
// Proper cleanup
class AnimationManager {
public:
    ~AnimationManager() {
        for (auto* anim : m_animations) {
            anim->UnloadFrames();
            delete anim;
        }
        m_animations.clear();
    }

    void UnloadAll() {
        for (auto* anim : m_animations) {
            anim->UnloadFrames();
        }
    }
};
```

#### Frame Index Issues

**Symptoms:** Wrong frames displayed, animation jumps

**Possible Causes:**
- Frame index calculation errors
- Time wrapping issues
- Frame count changes during playback

**Solutions:**
```cpp
// Debug frame calculation
void DebugAnimation(UFlipBook* flipbook) {
    SizeT frameCount = flipbook->GetFrameCount();
    Float32 duration = flipbook->GetDuration();
    Float32 currentTime = flipbook->GetCurrentTime();

    SizeT expectedFrame = static_cast<SizeT>(
        (currentTime / duration) * frameCount) % frameCount;

    FLogger::Log(ELogLevel::Debug,
        FString::Format("Frame: {}/{}, Time: {}/{}",
            expectedFrame, frameCount, currentTime, duration));
}
```

### Performance Issues

#### High Memory Usage

**Symptoms:** Large memory consumption with many flipbooks

**Solutions:**
- Unload unused animations
- Use texture atlasing
- Implement LOD system for distant animations

#### Low Frame Rate

**Symptoms:** Animation playback appears choppy

**Solutions:**
- Reduce frame count
- Increase duration for slower playback
- Optimize texture loading
- Use lower resolution frames

### Debug Tools

Enable animation debugging:

```cpp
class FlipBookDebugger {
public:
    static void LogAnimationState(UFlipBook* flipbook) {
        FLogger::Log(ELogLevel::Debug, "=== FlipBook Debug ===");
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Name: {}", flipbook->GetName()));
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Frames: {}", flipbook->GetFrameCount()));
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Duration: {}s", flipbook->GetDuration()));
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Current Time: {}s", flipbook->GetCurrentTime()));
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Loaded: {}", flipbook->IsLoaded() ? "Yes" : "No"));
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Hard Duration: {}", flipbook->m_hardDuration ? "Yes" : "No"));
    }
};
```

## Future Enhancements

### Planned Features

1. **Frame Interpolation**: Smooth blending between frames
2. **Variable Frame Rates**: Different timing per frame
3. **Animation Events**: Callbacks at specific frames/times
4. **Reverse Playback**: Built-in reverse animation support
5. **Ping-Pong Loops**: Forward-backward animation loops
6. **Texture Atlasing**: Automatic texture atlas generation
7. **Compression**: Runtime texture compression options

### Extension Points

The flipbook design allows for easy extension:

```cpp
class AdvancedFlipBook : public UFlipBook
{
public:
    // Add interpolation between frames
    FTextureHandle GetInterpolatedFrame(Float32 time) {
        // Implement frame blending
    }

    // Add animation events
    void AddEvent(Float32 time, TFunction<void()> callback) {
        m_events[time] = callback;
    }

    virtual void Update(Float32 deltaTime) override {
        UFlipBook::Update(deltaTime);

        // Check for events
        for (const auto& event : m_events) {
            if (std::abs(GetCurrentTime() - event.first) < 0.01f) {
                event.second();
            }
        }
    }

private:
    std::map<Float32, TFunction<void()>> m_events;
};
```

### Performance Optimizations

Future versions may include:
- **GPU Acceleration**: Hardware-accelerated frame blending
- **Streaming**: Progressive frame loading
- **Caching**: Smart frame caching strategies
- **Multithreading**: Parallel frame loading

---

*This documentation covers the complete UFlipBook implementation as of TKD Engine v1.0.0. For the latest updates and additional examples, refer to the engine's source code and test suites.*
