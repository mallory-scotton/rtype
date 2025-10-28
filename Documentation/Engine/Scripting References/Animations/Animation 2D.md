# 2D Animation Documentation

## Overview

The `FAnimation2D` class is a core component of the TKD Game Engine's 2D animation system, providing a container for managing sequences of animation frames. It encapsulates frame data, playback properties, and utility methods for creating, manipulating, and querying 2D sprite animations, serving as the primary data structure for frame-based animation playback systems.

### Key Features

- **Frame Sequence Management**: Ordered collection of animation frames with timing
- **Playback Control**: Looping and non-looping animation support
- **Frame Access**: Multiple methods for accessing individual frames
- **Duration Calculation**: Automatic total duration computation
- **Animation Utilities**: Reverse animation creation and frame manipulation
- **Memory Efficient**: Contiguous frame storage with minimal overhead
- **Type Safe**: Strong typing with compile-time safety guarantees

### Architecture

```cpp
class FAnimation2D
{
private:
    TVector<FAnimation2DFrame> m_frames;   // Frame sequence
    FString m_name;                        // Animation identifier
    bool m_isLooping;                      // Playback mode

public:
    FAnimation2D(const FString& name = "", bool isLooping = true);

    // Frame access and manipulation methods
    // Duration and property queries
    // Utility functions
};
```

The `FAnimation2D` serves as a high-level container that manages `FAnimation2DFrame` objects, providing the interface between animation data and playback systems.

## Core Components

### Frame Storage

- **Vector Container**: `TVector<FAnimation2DFrame>` for contiguous frame storage
- **Dynamic Sizing**: Frames can be added dynamically during runtime
- **Index-Based Access**: O(1) frame access by index

### Animation Properties

- **Name**: Unique string identifier for animation management
- **Looping Mode**: Boolean flag controlling playback behavior
- **Duration**: Computed total animation length from frame durations

### Frame Integration

Each animation contains multiple `FAnimation2DFrame` objects that define:
- **Texture Regions**: Rectangle coordinates for sprite rendering
- **Timing Information**: Individual frame display durations
- **Sequence Order**: Ordered progression through animation states

## API Reference

### Constructor

#### `FAnimation2D(const FString& name = "", bool isLooping = true)`

Creates a new 2D animation with specified name and looping behavior.

**Parameters:**
- `name`: Optional animation name (default: empty string)
- `isLooping`: Whether animation should loop (default: true)

**Behavior:**
- Initializes empty frame collection
- Sets animation name and looping mode
- Ready for frame addition

### Frame Access Operators

#### `const FAnimation2DFrame& operator[](SizeT index) const`

Accesses a frame by index using array-style syntax.

**Parameters:**
- `index`: Zero-based frame index

**Returns:** Const reference to the frame at specified index

**Behavior:**
- Bounds checking depends on `TVector` implementation
- Provides convenient array-like access

### Frame Management

#### `void AddFrame(const FAnimation2DFrame& frame)`

Adds a pre-constructed frame to the animation sequence.

**Parameters:**
- `frame`: The frame object to add

**Behavior:**
- Appends frame to the end of the sequence
- Maintains frame order for playback

#### `void AddFrame(const FRectanglei& rect, Float32 duration = 0.1f)`

Creates and adds a new frame with specified texture rectangle and duration.

**Parameters:**
- `rect`: Texture rectangle for the frame
- `duration`: Display duration in seconds (default: 0.1f)

**Behavior:**
- Constructs `FAnimation2DFrame` internally
- Appends to frame sequence
- Convenient for texture atlas workflows

### Property Queries

#### `SizeT GetFrameCount(void) const`

Returns the total number of frames in the animation.

**Returns:** Number of frames in the sequence

#### `const FAnimation2DFrame& GetFrame(SizeT index) const`

Retrieves a specific frame by index.

**Parameters:**
- `index`: Zero-based frame index

**Returns:** Const reference to the requested frame

**Behavior:**
- Alternative to operator[] with explicit method naming

#### `const FString& GetName(void) const`

Gets the animation's name identifier.

**Returns:** Const reference to the animation name

#### `bool IsLooping(void) const`

Checks if the animation is configured for looping playback.

**Returns:** `true` if looping, `false` otherwise

#### `Float32 GetTotalDuration(void) const`

Calculates the total duration of the entire animation.

**Returns:** Sum of all frame durations in seconds

**Behavior:**
- Iterates through all frames to compute total time
- Returns 0.0f for empty animations

### Property Modification

#### `void SetLooping(bool isLooping)`

Sets the looping behavior of the animation.

**Parameters:**
- `isLooping`: New looping state

**Behavior:**
- Updates internal looping flag
- Affects playback system behavior

### Utility Methods

#### `FAnimation2D Reverse(const FString& name = "") const`

Creates a reversed version of the current animation.

**Parameters:**
- `name`: Optional name for the reversed animation (default: original_name + "_Reversed")

**Returns:** New `FAnimation2D` instance with frames in reverse order

**Behavior:**
- Preserves original animation
- Maintains same looping behavior
- Useful for bidirectional animations

## Usage Examples

### Basic Animation Creation

```cpp
// Create a simple walking animation
FAnimation2D walkAnimation("Character_Walk", true);

// Add frames from a sprite sheet (32x32 frames, 0.1s each)
for (int i = 0; i < 8; ++i) {
    FRectanglei frameRect(i * 32, 0, 32, 32);
    walkAnimation.AddFrame(frameRect, 0.1f);
}

// Create a non-looping attack animation
FAnimation2D attackAnimation("Character_Attack", false);
attackAnimation.AddFrame(FRectanglei(0, 32, 32, 32), 0.2f);   // Wind-up
attackAnimation.AddFrame(FRectanglei(32, 32, 32, 32), 0.05f);  // Strike
attackAnimation.AddFrame(FRectanglei(64, 32, 32, 32), 0.15f);  // Recovery
```

### Animation Management System

```cpp
class AnimationManager {
private:
    std::unordered_map<FString, FAnimation2D> animations;

public:
    void LoadCharacterAnimations() {
        // Idle animation
        FAnimation2D idleAnim("Idle", true);
        idleAnim.AddFrame(FRectanglei(0, 0, 32, 32), 0.5f);
        idleAnim.AddFrame(FRectanglei(32, 0, 32, 32), 0.5f);
        animations["Idle"] = std::move(idleAnim);

        // Run animation (faster)
        FAnimation2D runAnim("Run", true);
        for (int i = 0; i < 6; ++i) {
            runAnim.AddFrame(FRectanglei(i * 32, 64, 32, 32), 0.08f);
        }
        animations["Run"] = std::move(runAnim);

        // Jump animation (non-looping)
        FAnimation2D jumpAnim("Jump", false);
        jumpAnim.AddFrame(FRectanglei(0, 96, 32, 32), 0.1f);
        jumpAnim.AddFrame(FRectanglei(32, 96, 32, 32), 0.2f);  // Peak
        jumpAnim.AddFrame(FRectanglei(64, 96, 32, 32), 0.15f);
        animations["Jump"] = std::move(jumpAnim);
    }

    const FAnimation2D* GetAnimation(const FString& name) const {
        auto it = animations.find(name);
        return it != animations.end() ? &it->second : nullptr;
    }

    void AddAnimation(FAnimation2D&& animation) {
        animations[animation.GetName()] = std::move(animation);
    }
};
```

### Animation Playback Controller

```cpp
class AnimationController {
private:
    const FAnimation2D* currentAnimation;
    size_t currentFrameIndex;
    Float32 frameTimer;
    bool isPlaying;

public:
    void PlayAnimation(const FAnimation2D* animation) {
        if (!animation || animation->GetFrameCount() == 0) {
            StopAnimation();
            return;
        }

        currentAnimation = animation;
        currentFrameIndex = 0;
        frameTimer = 0.0f;
        isPlaying = true;
    }

    void Update(Float32 deltaTime) {
        if (!isPlaying || !currentAnimation) return;

        frameTimer += deltaTime;

        const FAnimation2DFrame& currentFrame =
            currentAnimation->GetFrame(currentFrameIndex);

        // Check if current frame duration has elapsed
        if (frameTimer >= currentFrame.duration) {
            frameTimer -= currentFrame.duration;
            currentFrameIndex++;

            // Handle animation completion
            if (currentFrameIndex >= currentAnimation->GetFrameCount()) {
                if (currentAnimation->IsLooping()) {
                    currentFrameIndex = 0;
                } else {
                    StopAnimation();
                }
            }
        }
    }

    void StopAnimation() {
        currentAnimation = nullptr;
        currentFrameIndex = 0;
        frameTimer = 0.0f;
        isPlaying = false;
    }

    const FRectanglei* GetCurrentFrameRect() const {
        if (!isPlaying || !currentAnimation) return nullptr;
        return &currentAnimation->GetFrame(currentFrameIndex).textureRect;
    }

    bool IsPlaying() const { return isPlaying; }
    Float32 GetProgress() const {
        if (!currentAnimation || !isPlaying) return 0.0f;
        return static_cast<Float32>(currentFrameIndex) /
               static_cast<Float32>(currentAnimation->GetFrameCount());
    }
};
```

### Advanced Animation Building

```cpp
class AnimationBuilder {
public:
    static FAnimation2D CreateFromSpriteSheet(const FString& name,
                                            int frameWidth, int frameHeight,
                                            int frameCount, int framesPerRow,
                                            Float32 frameDuration = 0.1f,
                                            bool looping = true) {
        FAnimation2D animation(name, looping);

        for (int i = 0; i < frameCount; ++i) {
            int row = i / framesPerRow;
            int col = i % framesPerRow;

            FRectanglei rect(col * frameWidth, row * frameHeight,
                           frameWidth, frameHeight);
            animation.AddFrame(rect, frameDuration);
        }

        return animation;
    }

    static FAnimation2D CreateVariableTimingAnimation(const FString& name,
                                                    const std::vector<FRectanglei>& rects,
                                                    const std::vector<Float32>& durations,
                                                    bool looping = true) {
        FAnimation2D animation(name, looping);

        size_t count = std::min(rects.size(), durations.size());
        for (size_t i = 0; i < count; ++i) {
            animation.AddFrame(rects[i], durations[i]);
        }

        return animation;
    }

    static std::vector<FAnimation2D> CreateDirectionalAnimations(
        const FString& baseName,
        const std::vector<FRectanglei>& frames,
        Float32 frameDuration = 0.1f)
    {
        std::vector<FAnimation2D> directions;

        // Create 4-directional animations (up, down, left, right)
        const char* directionNames[] = {"_Up", "_Down", "_Left", "_Right"};

        for (int dir = 0; dir < 4; ++dir) {
            FAnimation2D anim(baseName + directionNames[dir], true);

            // Assuming frames are ordered: up, down, left, right
            for (size_t i = dir * 4; i < (dir + 1) * 4 && i < frames.size(); ++i) {
                anim.AddFrame(frames[i], frameDuration);
            }

            directions.push_back(std::move(anim));
        }

        return directions;
    }
};
```

### Animation State Machine Integration

```cpp
class CharacterAnimator {
private:
    AnimationController controller;
    std::unordered_map<FString, const FAnimation2D*> animationMap;
    FString currentState;

public:
    void SetAnimation(const FString& stateName) {
        auto it = animationMap.find(stateName);
        if (it != animationMap.end()) {
            controller.PlayAnimation(it->second);
            currentState = stateName;
        }
    }

    void Update(Float32 deltaTime) {
        controller.Update(deltaTime);
    }

    // State-based animation switching
    void HandleMovement(bool isMoving, const FVector2& direction) {
        if (!isMoving) {
            if (currentState != "Idle") SetAnimation("Idle");
        } else {
            // Determine direction-based animation
            if (std::abs(direction.x) > std::abs(direction.y)) {
                SetAnimation(direction.x > 0 ? "Walk_Right" : "Walk_Left");
            } else {
                SetAnimation(direction.y > 0 ? "Walk_Up" : "Walk_Down");
            }
        }
    }

    void HandleJump() {
        if (currentState != "Jump") {
            SetAnimation("Jump");
        }
    }

    void HandleAttack() {
        // Create attack animation dynamically
        FAnimation2D attackAnim = AnimationBuilder::CreateVariableTimingAnimation(
            "Attack",
            {FRectanglei(0, 128, 32, 32), FRectanglei(32, 128, 32, 32)},
            {0.1f, 0.05f},
            false
        );

        animationMap["Attack"] = &attackAnim;
        SetAnimation("Attack");
    }
};
```

## Animation Playback Integration

### Playback System Architecture

The `FAnimation2D` integrates with playback systems through:

1. **Frame Iteration**: Sequential access to animation frames
2. **Timing Control**: Duration-based frame progression
3. **Loop Management**: Automatic wrapping for continuous playback
4. **State Queries**: Current frame and progress information

### Rendering Integration

```cpp
class SpriteRenderer {
public:
    void RenderAnimatedSprite(const AnimationController& animator,
                             const FVector2& position,
                             const FTextureHandle& texture) {
        const FRectanglei* frameRect = animator.GetCurrentFrameRect();
        if (frameRect) {
            // Convert rectangle to UV coordinates and render
            RenderTextureRegion(texture, position, *frameRect);
        }
    }

private:
    void RenderTextureRegion(const FTextureHandle& texture,
                           const FVector2& position,
                           const FRectanglei& rect) {
        // Implementation depends on rendering API
        // Calculate UV coordinates from rectangle
        Float32 u0 = static_cast<Float32>(rect.left) / texture.GetWidth();
        Float32 v0 = static_cast<Float32>(rect.top) / texture.GetHeight();
        Float32 u1 = static_cast<Float32>(rect.left + rect.width) / texture.GetWidth();
        Float32 v1 = static_cast<Float32>(rect.top + rect.height) / texture.GetHeight();

        // Render quad with calculated UVs
    }
};
```

## Performance Considerations

### Memory Usage

- **Frame Storage**: Linear scaling with frame count
- **Frame Data**: Each frame contains rectangle and duration
- **Animation Metadata**: Minimal overhead per animation

### CPU Performance

- **Frame Access**: O(1) indexed access
- **Duration Calculation**: O(n) where n = frame count (cached if needed)
- **Reverse Operation**: O(n) frame copying

### Optimization Strategies

1. **Frame Reuse**: Share common frames across animations
2. **Animation Pooling**: Reuse animation objects
3. **Lazy Evaluation**: Cache computed properties

### Best Practices

- **Frame Limits**: Keep animation frame counts reasonable (4-32 frames)
- **Duration Consistency**: Use consistent frame durations within animations
- **Naming Conventions**: Use descriptive, consistent animation names
- **Memory Management**: Pool frequently used animations

## Architecture Diagrams

### Class Structure

```
FAnimation2D
├── m_frames: TVector<FAnimation2DFrame>    // Frame sequence
├── m_name: FString                         // Animation identifier
└── m_isLooping: bool                       // Playback mode
    └── FAnimation2DFrame
        ├── duration: Float32               // Frame timing
        └── textureRect: FRectanglei        // Texture region
```

### Animation Playback Flow

```
Animation Start ──► Frame 0 ──► Display Duration ──► Frame 1 ──► Display Duration
         ▲              │              │              │              │
         │              ▼              ▼              ▼              ▼
         └───── Loop ───┴───── Timer ───┴───── Timer ───┴───── Next Frame
                        ▲              ▲              ▲              ▲
                        │              │              │              │
                IsLooping = true ─────┴───── Time >= Duration ──────┘
```

### Animation Creation Flow

```
Create Animation ──► Set Name & Looping ──► Add Frames ──► Frame Sequence
         │                    │                    │              │
         ▼                    ▼                    ▼              ▼
   FAnimation2D() ──► Constructor ──► AddFrame() ──► TVector Push ──► Complete
         ▲                    ▲                    ▲              ▲
         │                    │                    │              │
         └───── Builder ──────┴───── Builder ──────┴───── Builder ──┘
```

## Troubleshooting

### Common Issues

#### Empty Animations

**Symptoms:** Animation has no frames, playback fails

**Possible Causes:**
- No frames added after creation
- Frame addition failed
- Animation construction incomplete

**Solutions:**
```cpp
// Validate animation before use
bool IsValidAnimation(const FAnimation2D& animation) {
    if (animation.GetFrameCount() == 0) {
        FLogger::Log(ELogLevel::Error,
            FString::Format("Animation '{}' has no frames", animation.GetName()));
        return false;
    }

    if (animation.GetTotalDuration() <= 0.0f) {
        FLogger::Log(ELogLevel::Warning,
            FString::Format("Animation '{}' has zero duration", animation.GetName()));
    }

    return true;
}

// Safe animation creation
FAnimation2D CreateSafeAnimation(const FString& name,
                               const std::vector<FRectanglei>& rects,
                               Float32 frameDuration = 0.1f) {
    FAnimation2D animation(name, true);

    for (const auto& rect : rects) {
        if (rect.width > 0 && rect.height > 0) {
            animation.AddFrame(rect, frameDuration);
        } else {
            FLogger::Log(ELogLevel::Warning,
                FString::Format("Skipping invalid frame rect: {}x{}", rect.width, rect.height));
        }
    }

    if (animation.GetFrameCount() == 0) {
        FLogger::Log(ELogLevel::Error,
            FString::Format("Failed to create animation '{}' - no valid frames", name));
    }

    return animation;
}
```

#### Frame Access Errors

**Symptoms:** Crashes when accessing frames by index

**Possible Causes:**
- Index out of bounds
- Empty animation access
- Incorrect frame counting

**Solutions:**
```cpp
// Safe frame access
const FAnimation2DFrame* GetFrameSafe(const FAnimation2D& animation, SizeT index) {
    if (animation.GetFrameCount() == 0) {
        FLogger::Log(ELogLevel::Warning, "Attempting to access frame from empty animation");
        return nullptr;
    }

    if (index >= animation.GetFrameCount()) {
        FLogger::Log(ELogLevel::Warning,
            FString::Format("Frame index {} out of bounds (max: {})",
                index, animation.GetFrameCount() - 1));
        return nullptr;
    }

    return &animation.GetFrame(index);
}

// Bounds-checked iteration
void ProcessAnimationFrames(const FAnimation2D& animation) {
    for (SizeT i = 0; i < animation.GetFrameCount(); ++i) {
        const FAnimation2DFrame& frame = animation.GetFrame(i);
        // Process frame safely
        ProcessFrame(frame);
    }
}
```

#### Duration Calculation Issues

**Symptoms:** Incorrect animation timing or playback speed

**Possible Causes:**
- Zero or negative frame durations
- Incorrect duration summation
- Floating point precision issues

**Solutions:**
```cpp
// Validate frame durations
void ValidateAnimationDurations(FAnimation2D& animation) {
    const Float32 MIN_DURATION = 0.001f; // 1ms minimum

    for (SizeT i = 0; i < animation.GetFrameCount(); ++i) {
        const FAnimation2DFrame& frame = animation.GetFrame(i);

        if (frame.duration <= 0.0f) {
            FLogger::Log(ELogLevel::Warning,
                FString::Format("Frame {} has invalid duration: {}", i, frame.duration));
            // Fix invalid duration
            const_cast<FAnimation2DFrame&>(frame).duration = MIN_DURATION;
        } else if (frame.duration < MIN_DURATION) {
            FLogger::Log(ELogLevel::Warning,
                FString::Format("Frame {} duration very short: {}s", i, frame.duration));
        }
    }
}

// Debug animation timing
void DebugAnimationTiming(const FAnimation2D& animation) {
    FLogger::Log(ELogLevel::Debug,
        FString::Format("Animation '{}': {} frames, {}s total",
            animation.GetName(),
            animation.GetFrameCount(),
            animation.GetTotalDuration()));

    Float32 cumulativeTime = 0.0f;
    for (SizeT i = 0; i < animation.GetFrameCount(); ++i) {
        const FAnimation2DFrame& frame = animation.GetFrame(i);
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Frame {}: {}s ({}s - {}s)",
                i, frame.duration, cumulativeTime, cumulativeTime + frame.duration));
        cumulativeTime += frame.duration;
    }
}
```

### Memory Issues

**Symptoms:** High memory usage with many animations

**Possible Causes:**
- Duplicate animation data
- Inefficient frame storage
- Animation objects not cleaned up

**Solutions:**
```cpp
class AnimationPool {
private:
    std::unordered_map<FString, std::unique_ptr<FAnimation2D>> pool;

public:
    const FAnimation2D* GetOrCreateAnimation(const FString& name,
                                           std::function<FAnimation2D()> creator) {
        auto it = pool.find(name);
        if (it == pool.end()) {
            auto animation = std::make_unique<FAnimation2D>(creator());
            auto* ptr = animation.get();
            pool[name] = std::move(animation);
            return ptr;
        }
        return it->second.get();
    }

    void ClearUnusedAnimations() {
        // Implementation for cleanup based on usage tracking
    }
};
```

### Performance Issues

**Symptoms:** Animation updates are slow

**Possible Causes:**
- Too many animations updating simultaneously
- Expensive duration calculations
- Frequent animation switching

**Solutions:**
```cpp
// Cache total duration
class CachedAnimation : public FAnimation2D {
private:
    mutable Float32 cachedDuration;
    mutable bool durationValid;

public:
    CachedAnimation(const FString& name, bool looping)
        : FAnimation2D(name, looping)
        , cachedDuration(0.0f)
        , durationValid(false)
    {}

    Float32 GetTotalDuration() const override {
        if (!durationValid) {
            cachedDuration = FAnimation2D::GetTotalDuration();
            durationValid = true;
        }
        return cachedDuration;
    }

    void AddFrame(const FAnimation2DFrame& frame) override {
        FAnimation2D::AddFrame(frame);
        durationValid = false; // Invalidate cache
    }
};
```

### Debug Tools

Enable animation debugging:

```cpp
class AnimationDebugger {
public:
    static void LogAnimationInfo(const FAnimation2D& animation) {
        FLogger::Log(ELogLevel::Debug, "=== Animation Debug Info ===");
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Name: {}", animation.GetName()));
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Frames: {}", animation.GetFrameCount()));
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Looping: {}", animation.IsLooping() ? "Yes" : "No"));
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Total Duration: {}s", animation.GetTotalDuration()));
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Avg FPS: {}", animation.GetFrameCount() / animation.GetTotalDuration()));
    }

    static void LogFrameSequence(const FAnimation2D& animation) {
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Frame sequence for '{}':", animation.GetName()));

        Float32 time = 0.0f;
        for (SizeT i = 0; i < animation.GetFrameCount(); ++i) {
            const FAnimation2DFrame& frame = animation.GetFrame(i);
            FLogger::Log(ELogLevel::Debug,
                FString::Format("  Frame {}: Rect({}, {}, {}, {}), Duration: {}s, Time: {}s",
                    i, frame.textureRect.left, frame.textureRect.top,
                    frame.textureRect.width, frame.textureRect.height,
                    frame.duration, time));
            time += frame.duration;
        }
    }
};
```

## Future Enhancements

### Planned Features

1. **Animation Blending**: Smooth transitions between different animations
2. **Variable Playback**: Speed ramping and time scaling
3. **Animation Events**: Callbacks at specific frames or times
4. **Layered Animation**: Multiple animation layers with blending
5. **Procedural Animation**: Runtime-generated frame sequences
6. **Animation Compression**: Memory-efficient frame storage
7. **Animation Streaming**: Progressive loading for large animations

### Extension Points

The animation system allows for easy extension:

```cpp
class AdvancedAnimation2D : public FAnimation2D {
public:
    // Add animation events
    struct AnimationEvent {
        SizeT frameIndex;
        TFunction<void()> callback;
    };
    std::vector<AnimationEvent> events;

    // Add playback modifiers
    Float32 playbackSpeed;
    bool reversePlayback;

    // Add blending support
    Float32 blendWeight;
    const FAnimation2D* blendTarget;

    AdvancedAnimation2D(const FString& name, bool looping)
        : FAnimation2D(name, looping)
        , playbackSpeed(1.0f)
        , reversePlayback(false)
        , blendWeight(0.0f)
        , blendTarget(nullptr)
    {}

    void AddEvent(SizeT frameIndex, TFunction<void()> callback) {
        events.push_back({frameIndex, callback});
    }

    void SetBlendTarget(const FAnimation2D* target, Float32 weight) {
        blendTarget = target;
        blendWeight = weight;
    }

    // Override for advanced playback
    const FAnimation2DFrame& GetCurrentFrame(Float32 time) const {
        if (blendTarget && blendWeight > 0.0f) {
            // Return blended frame
            return GetBlendedFrame(time);
        }
        return GetFrameAtTime(time);
    }

private:
    FAnimation2DFrame GetBlendedFrame(Float32 time) const {
        // Implement frame blending logic
        const FAnimation2DFrame& frame1 = GetFrameAtTime(time);
        const FAnimation2DFrame& frame2 = blendTarget->GetFrameAtTime(time);

        // Blend texture rects and durations
        FAnimation2DFrame blended;
        blended.duration = frame1.duration * (1.0f - blendWeight) +
                          frame2.duration * blendWeight;
        // Blend rectangle coordinates...
        return blended;
    }

    const FAnimation2DFrame& GetFrameAtTime(Float32 time) const {
        // Implement time-based frame lookup
        Float32 normalizedTime = std::fmod(time, GetTotalDuration());
        Float32 accumulatedTime = 0.0f;

        for (SizeT i = 0; i < GetFrameCount(); ++i) {
            const FAnimation2DFrame& frame = GetFrame(i);
            accumulatedTime += frame.duration;
            if (normalizedTime < accumulatedTime) {
                return frame;
            }
        }

        return GetFrame(0); // Fallback
    }
};
```

### Advanced Features

Future versions may include:
- **GPU Acceleration**: Hardware-accelerated frame processing
- **Animation Graphs**: Node-based animation state machines
- **Motion Matching**: Data-driven animation transitions
- **Real-time Editing**: Runtime animation modification
- **Animation Analytics**: Performance and usage statistics

---

*This documentation covers the complete FAnimation2D implementation as of TKD Engine v1.0.0. For the latest updates and additional examples, refer to the engine's source code and test suites.*
