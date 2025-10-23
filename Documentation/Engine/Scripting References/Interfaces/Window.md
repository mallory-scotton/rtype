# Window Interface Documentation

## Overview

The `FWindowInterface` provides thread-safe access to window management and rendering functionality in the TKD Engine. This interface is only available in client builds (`TKD_ENGINE_CLIENT`) and serves as the primary gateway for window operations, input management, and performance monitoring.

### Key Features

- **Client-Only**: Only available in client builds, not server builds
- **Thread-Safe Operations**: All methods protected by mutexes for cross-thread safety
- **Performance Monitoring**: Real-time FPS and frame time tracking
- **Input Integration**: Direct access to the input management system
- **Window State Management**: Title, dimensions, and open/close state

### Architecture

```cpp
#if TKD_ENGINE_CLIENT
class FWindowInterface final
{
private:
    static std::mutex s_mutex;
    static __internal::FWindowSubsystem* GetWindowSubsystem(void);

public:
    // Window state
    static bool IsOpen(void);
    static FVector2u GetDimensions(void);
    static UInt32 GetWidth(void);
    static UInt32 GetHeight(void);
    static void SetTitle(const FString& title);

    // Performance
    static float GetFPS(void);
    static float GetAverageFrameTime(void);

    // Input
    static FInputManager* GetInputManager(void);

    // Status
    static bool IsInitialized(void);
};
#endif
```

## Window State Management

### Window Status

#### `static bool IsOpen(void)`

Checks if the window is currently open and valid.

**Returns:** `true` if the window exists and is open, `false` otherwise

**Use Cases:**
- Main game loop condition
- Graceful shutdown detection
- Window recreation logic

**Example:**
```cpp
// Main game loop
while (Window::IsOpen()) {
    // Process input
    ProcessInput();

    // Update game logic
    UpdateGame();

    // Render frame
    Render();
}

// Cleanup after window closes
ShutdownGame();
```

#### `static bool IsInitialized(void)`

Checks if the window subsystem has been properly initialized.

**Returns:** `true` if initialized, `false` otherwise

**Use Cases:**
- Startup validation
- Feature availability checking
- Error handling

**Example:**
```cpp
bool InitializeRendering() {
    if (!Window::IsInitialized()) {
        FLogger::Log(ELogLevel::Error, "Window subsystem not initialized");
        return false;
    }

    // Safe to use window functions
    SetupRenderer();
    return true;
}
```

### Window Properties

#### `static FVector2u GetDimensions(void)`

Gets the current window dimensions.

**Returns:** `FVector2u` containing width (x) and height (y) in pixels

**Thread Safety:** Safe to call from any thread

**Example:**
```cpp
// Get window size
FVector2u windowSize = Window::GetDimensions();
std::cout << "Window: " << windowSize.x << "x" << windowSize.y << std::endl;

// Calculate aspect ratio
float aspectRatio = static_cast<float>(windowSize.x) / windowSize.y;

// Adjust camera for aspect ratio
camera.SetAspectRatio(aspectRatio);
```

#### `static UInt32 GetWidth(void)`

Gets the current window width.

**Returns:** Window width in pixels

**Convenience Method:** Equivalent to `GetDimensions().x`

**Example:**
```cpp
// Check if window is wide enough for split-screen
if (Window::GetWidth() >= 1280) {
    EnableSplitScreen();
} else {
    DisableSplitScreen();
}
```

#### `static UInt32 GetHeight(void)`

Gets the current window height.

**Returns:** Window height in pixels

**Convenience Method:** Equivalent to `GetDimensions().y`

**Example:**
```cpp
// Position UI elements relative to window height
float uiY = Window::GetHeight() * 0.1f; // 10% from top
scoreDisplay->SetPosition(FVector2(10, uiY));
```

#### `static void SetTitle(const FString& title)`

Sets the window title displayed in the title bar.

**Parameters:**
- `title`: The new window title string

**Behavior:**
- Updates the window title immediately
- Visible in taskbar and window decorations
- Can be changed dynamically during runtime

**Example:**
```cpp
// Set initial title
Window::SetTitle("My Game v1.0");

// Update title with game state
void UpdateWindowTitle() {
    FString title = "My Game v1.0";
    if (isPaused) {
        title += " [PAUSED]";
    }
    if (currentLevel) {
        title += " - " + currentLevel->GetName();
    }
    Window::SetTitle(title);
}
```

## Performance Monitoring

### Frame Rate

#### `static float GetFPS(void)`

Gets the current frames per second.

**Returns:** Current FPS as a float value

**Calculation:** Based on the time between recent frames

**Update Frequency:** Updated every frame

**Example:**
```cpp
// Display FPS in debug UI
float fps = Window::GetFPS();
debugText->SetText("FPS: " + std::to_string(static_cast<int>(fps)));

// Performance warnings
if (fps < 30.0f) {
    FLogger::Log(ELogLevel::Warning, "Low FPS detected: " + std::to_string(fps));
    EnablePerformanceMode();
}
```

#### `static float GetAverageFrameTime(void)`

Gets the average time spent per frame in milliseconds.

**Returns:** Average frame time in milliseconds

**Calculation:** Rolling average of recent frame times

**Use Cases:**
- Performance profiling
- Frame time budgeting
- Optimization monitoring

**Example:**
```cpp
// Monitor frame time budget
float avgFrameTime = Window::GetAverageFrameTime();
const float targetFrameTime = 1000.0f / 60.0f; // 60 FPS target

if (avgFrameTime > targetFrameTime) {
    std::cout << "Frame time over budget: " << avgFrameTime << "ms" << std::endl;

    // Enable optimizations
    ReduceDrawDistance();
    LowerTextureQuality();
}
```

### Performance Analysis

```cpp
class FPerformanceAnalyzer {
private:
    float m_minFPS = FLT_MAX;
    float m_maxFPS = 0.0f;
    float m_totalFrameTime = 0.0f;
    int m_frameCount = 0;

public:
    void Update() {
        float fps = Window::GetFPS();
        float frameTime = Window::GetAverageFrameTime();

        m_minFPS = std::min(m_minFPS, fps);
        m_maxFPS = std::max(m_maxFPS, fps);
        m_totalFrameTime += frameTime;
        m_frameCount++;

        // Log every 60 frames
        if (m_frameCount % 60 == 0) {
            LogPerformanceStats();
        }
    }

    void LogPerformanceStats() {
        float avgFPS = m_frameCount * 60.0f / (m_totalFrameTime / 1000.0f);
        float avgFrameTime = m_totalFrameTime / m_frameCount;

        FLogger::Log(ELogLevel::Info, FString::Format(
            "Performance Stats - Avg FPS: %.1f, Min FPS: %.1f, Max FPS: %.1f, Avg Frame Time: %.2fms",
            avgFPS, m_minFPS, m_maxFPS, avgFrameTime
        ));

        // Reset for next period
        Reset();
    }

    void Reset() {
        m_minFPS = FLT_MAX;
        m_maxFPS = 0.0f;
        m_totalFrameTime = 0.0f;
        m_frameCount = 0;
    }
};
```

## Input Management

#### `static FInputManager* GetInputManager(void)`

Gets the input manager for handling keyboard, mouse, and gamepad input.

**Returns:** Pointer to the `FInputManager` instance, or `nullptr` if not available

**Thread Safety:** Safe to call from any thread, but input processing should be done on main thread

**Example:**
```cpp
// Get input manager
FInputManager* input = Window::GetInputManager();
if (!input) {
    FLogger::Log(ELogLevel::Error, "Input manager not available");
    return;
}

// Check keyboard input
if (input->IsKeyPressed(EKey::Space)) {
    player->Jump();
}

// Check mouse input
FVector2i mousePos = input->GetMousePosition();
FVector2i mouseDelta = input->GetMouseDelta();

// Check gamepad input (if available)
if (input->IsGamepadConnected(0)) {
    float leftStickX = input->GetGamepadAxisValue(0, EGamepadAxis::LeftX);
    player->Move(FVector3(leftStickX, 0, 0));
}
```

### Input Processing Patterns

```cpp
class AInputHandler : public AActor {
private:
    FInputManager* m_inputManager = nullptr;

public:
    virtual void BeginPlay() override {
        m_inputManager = Window::GetInputManager();
        if (!m_inputManager) {
            FLogger::Log(ELogLevel::Error, "Failed to get input manager");
            return;
        }

        // Optional: Set up input processing mode
        m_inputManager->SetMouseMode(EMouseMode::Captured);
    }

    void ProcessInput() {
        if (!m_inputManager) return;

        // Keyboard movement
        FVector3 movement(0, 0, 0);
        if (m_inputManager->IsKeyPressed(EKey::W)) movement.y += 1;
        if (m_inputManager->IsKeyPressed(EKey::S)) movement.y -= 1;
        if (m_inputManager->IsKeyPressed(EKey::A)) movement.x -= 1;
        if (m_inputManager->IsKeyPressed(EKey::D)) movement.x += 1;

        if (movement.LengthSquared() > 0) {
            GetOwner()->Move(movement.Normalized());
        }

        // Mouse look
        FVector2i mouseDelta = m_inputManager->GetMouseDelta();
        if (mouseDelta.x != 0 || mouseDelta.y != 0) {
            float sensitivity = 0.1f;
            GetOwner()->Rotate(FVector3(0, mouseDelta.x * sensitivity, 0));
            camera->Pitch(mouseDelta.y * sensitivity);
        }
    }

    virtual void Tick(float deltaTime) override {
        ProcessInput();
    }
};
```

## Usage Examples

### Basic Window Setup

```cpp
#include <Engine/Static/FWindowInterface.hpp>

bool InitializeWindow() {
    // Check if window is available (client build)
    #if TKD_ENGINE_CLIENT
    if (!Window::IsInitialized()) {
        FLogger::Log(ELogLevel::Error, "Window subsystem not initialized");
        return false;
    }

    // Set window title
    Window::SetTitle("My Awesome Game");

    // Log window information
    FVector2u size = Window::GetDimensions();
    FLogger::Log(ELogLevel::Info, FString::Format(
        "Window initialized: %dx%d", size.x, size.y
    ));

    return true;
    #else
    // Server build - no window
    FLogger::Log(ELogLevel::Info, "Running in server mode - no window");
    return true;
    #endif
}
```

### Game Loop with Window Management

```cpp
class AGameInstance {
private:
    bool m_isRunning = true;
    FPerformanceAnalyzer m_performanceAnalyzer;

public:
    void Run() {
        if (!InitializeGame()) {
            return;
        }

        // Main game loop
        while (m_isRunning && IsWindowOpen()) {
            float deltaTime = CalculateDeltaTime();

            // Update performance analyzer
            m_performanceAnalyzer.Update();

            // Process input
            ProcessInput();

            // Update game logic
            UpdateGame(deltaTime);

            // Render frame
            Render();

            // Check for exit conditions
            if (ShouldExit()) {
                m_isRunning = false;
            }
        }

        ShutdownGame();
    }

private:
    bool IsWindowOpen() {
        #if TKD_ENGINE_CLIENT
        return Window::IsOpen();
        #else
        return true; // Server always "open"
        #endif
    }

    void ProcessInput() {
        #if TKD_ENGINE_CLIENT
        FInputManager* input = Window::GetInputManager();
        if (input && input->IsKeyPressed(EKey::Escape)) {
            m_isRunning = false;
        }
        #endif
    }
};
```

### Dynamic Window Management

```cpp
class AWindowManager : public AActor {
public:
    void ToggleFullscreen() {
        #if TKD_ENGINE_CLIENT
        // Note: Actual fullscreen implementation depends on window backend
        // This is a conceptual example
        if (m_isFullscreen) {
            SetWindowedMode();
        } else {
            SetFullscreenMode();
        }
        m_isFullscreen = !m_isFullscreen;
        #endif
    }

    void AdjustWindowSize(int width, int height) {
        #if TKD_ENGINE_CLIENT
        // Note: Actual resize implementation depends on window backend
        // This is a conceptual example
        if (width > 0 && height > 0) {
            ResizeWindow(width, height);
            UpdateUIForNewSize(width, height);
        }
        #endif
    }

    void UpdateWindowTitleWithStats() {
        #if TKD_ENGINE_CLIENT
        float fps = Window::GetFPS();
        FVector2u size = Window::GetDimensions();

        FString title = FString::Format(
            "Game - %dx%d - %.0f FPS",
            size.x, size.y, fps
        );

        Window::SetTitle(title);
        #endif
    }

private:
    bool m_isFullscreen = false;

    // Platform-specific implementations would go here
    void SetWindowedMode() { /* Implementation */ }
    void SetFullscreenMode() { /* Implementation */ }
    void ResizeWindow(int width, int height) { /* Implementation */ }
    void UpdateUIForNewSize(int width, int height) { /* Implementation */ }
};
```

### Performance Monitoring System

```cpp
class APerformanceMonitor : public AActor {
private:
    struct PerformanceSnapshot {
        float fps;
        float frameTime;
        float timestamp;
    };

    std::vector<PerformanceSnapshot> m_snapshots;
    float m_lastUpdateTime = 0.0f;
    const float UPDATE_INTERVAL = 1.0f; // Update every second

public:
    virtual void Tick(float deltaTime) override {
        float currentTime = World::GetTime();

        if (currentTime - m_lastUpdateTime >= UPDATE_INTERVAL) {
            TakePerformanceSnapshot(currentTime);
            m_lastUpdateTime = currentTime;

            AnalyzePerformance();
        }
    }

private:
    void TakePerformanceSnapshot(float timestamp) {
        #if TKD_ENGINE_CLIENT
        PerformanceSnapshot snapshot;
        snapshot.fps = Window::GetFPS();
        snapshot.frameTime = Window::GetAverageFrameTime();
        snapshot.timestamp = timestamp;

        m_snapshots.push_back(snapshot);

        // Keep only last 60 snapshots (1 minute at 1Hz)
        if (m_snapshots.size() > 60) {
            m_snapshots.erase(m_snapshots.begin());
        }
        #endif
    }

    void AnalyzePerformance() {
        if (m_snapshots.empty()) return;

        #if TKD_ENGINE_CLIENT
        // Calculate averages
        float avgFPS = 0.0f;
        float avgFrameTime = 0.0f;
        float minFPS = FLT_MAX;
        float maxFPS = 0.0f;

        for (const auto& snapshot : m_snapshots) {
            avgFPS += snapshot.fps;
            avgFrameTime += snapshot.frameTime;
            minFPS = std::min(minFPS, snapshot.fps);
            maxFPS = std::max(maxFPS, snapshot.fps);
        }

        avgFPS /= m_snapshots.size();
        avgFrameTime /= m_snapshots.size();

        // Log performance stats
        FLogger::Log(ELogLevel::Info, FString::Format(
            "Performance - Avg FPS: %.1f (%.1f-%.1f), Avg Frame Time: %.2fms",
            avgFPS, minFPS, maxFPS, avgFrameTime
        ));

        // Performance warnings
        if (avgFPS < 30.0f) {
            FLogger::Log(ELogLevel::Warning, "Low average FPS detected");
            OnLowPerformanceDetected();
        }

        if (avgFrameTime > 33.33f) { // > 30 FPS frame time
            FLogger::Log(ELogLevel::Warning, "High average frame time detected");
            OnHighFrameTimeDetected();
        }
        #endif
    }

    void OnLowPerformanceDetected() {
        // Enable performance optimizations
        ReduceParticleEffects();
        LowerShadowQuality();
        DecreaseDrawDistance();
    }

    void OnHighFrameTimeDetected() {
        // Profile and optimize
        StartPerformanceProfiling();
    }

    void StartPerformanceProfiling() {
        // Implementation for detailed profiling
        // Could integrate with external profiling tools
    }

    // Performance adjustment methods
    void ReduceParticleEffects() { /* Implementation */ }
    void LowerShadowQuality() { /* Implementation */ }
    void DecreaseDrawDistance() { /* Implementation */ }
};
```

## Advanced Input System

```cpp
class AAdvancedInputSystem : public AActor {
private:
    FInputManager* m_inputManager = nullptr;
    std::unordered_map<FString, std::function<void()>> m_actionBindings;

public:
    virtual void BeginPlay() override {
        #if TKD_ENGINE_CLIENT
        m_inputManager = Window::GetInputManager();
        if (m_inputManager) {
            SetupInputBindings();
        }
        #endif
    }

    void SetupInputBindings() {
        // Movement actions
        BindAction("MoveForward", [this]() { MovePlayer(FVector3(0, 1, 0)); });
        BindAction("MoveBackward", [this]() { MovePlayer(FVector3(0, -1, 0)); });
        BindAction("MoveLeft", [this]() { MovePlayer(FVector3(-1, 0, 0)); });
        BindAction("MoveRight", [this]() { MovePlayer(FVector3(1, 0, 0)); });

        // Combat actions
        BindAction("Jump", [this]() { GetPlayer()->Jump(); });
        BindAction("Attack", [this]() { GetPlayer()->Attack(); });
        BindAction("Block", [this]() { GetPlayer()->Block(); });

        // UI actions
        BindAction("Pause", [this]() { TogglePauseMenu(); });
        BindAction("Inventory", [this]() { ToggleInventory(); });
    }

    void BindAction(const FString& actionName, std::function<void()> callback) {
        m_actionBindings[actionName] = callback;
    }

    virtual void Tick(float deltaTime) override {
        #if TKD_ENGINE_CLIENT
        if (!m_inputManager) return;

        // Process action bindings
        for (const auto& binding : m_actionBindings) {
            if (IsActionPressed(binding.first)) {
                binding.second();
            }
        }

        // Process continuous input
        ProcessAnalogInput(deltaTime);
        #endif
    }

private:
    bool IsActionPressed(const FString& actionName) {
        // Map action names to keys (in a real implementation,
        // this would use an input mapping system)
        if (actionName == "MoveForward") return m_inputManager->IsKeyPressed(EKey::W);
        if (actionName == "MoveBackward") return m_inputManager->IsKeyPressed(EKey::S);
        if (actionName == "MoveLeft") return m_inputManager->IsKeyPressed(EKey::A);
        if (actionName == "MoveRight") return m_inputManager->IsKeyPressed(EKey::D);
        if (actionName == "Jump") return m_inputManager->IsKeyPressed(EKey::Space);
        if (actionName == "Attack") return m_inputManager->IsMouseButtonPressed(EMouseButton::Left);
        if (actionName == "Pause") return m_inputManager->IsKeyPressed(EKey::Escape);

        return false;
    }

    void ProcessAnalogInput(float deltaTime) {
        // Mouse look
        FVector2i mouseDelta = m_inputManager->GetMouseDelta();
        if (mouseDelta.LengthSquared() > 0) {
            RotateCamera(mouseDelta, deltaTime);
        }

        // Controller input (if available)
        if (m_inputManager->IsGamepadConnected(0)) {
            float rightStickX = m_inputManager->GetGamepadAxisValue(0, EGamepadAxis::RightX);
            float rightStickY = m_inputManager->GetGamepadAxisValue(0, EGamepadAxis::RightY);

            if (std::abs(rightStickX) > 0.1f || std::abs(rightStickY) > 0.1f) {
                RotateCamera(FVector2i(rightStickX * 100, rightStickY * 100), deltaTime);
            }
        }
    }

    void MovePlayer(const FVector3& direction) {
        if (APawn* pawn = GetPlayerPawn()) {
            pawn->AddMovementInput(direction, 1.0f);
        }
    }

    void RotateCamera(const FVector2i& delta, float deltaTime) {
        float sensitivity = 0.1f;
        FVector3 rotation(0, delta.x * sensitivity * deltaTime, 0);
        GetCamera()->AddRotation(rotation);
    }

    APawn* GetPlayerPawn() { /* Return player pawn */ return nullptr; }
    ACamera* GetCamera() { /* Return active camera */ return nullptr; }
    void TogglePauseMenu() { /* Implementation */ }
    void ToggleInventory() { /* Implementation */ }
};
```

## Platform-Specific Considerations

### Windows-Specific

```cpp
#if defined(TKD_PLATFORM_WINDOWS)
void SetupWindowsSpecific() {
    // Windows-specific window setup
    // Set DPI awareness
    // Configure window styles
}
#endif
```

### Linux-Specific

```cpp
#if defined(TKD_PLATFORM_LINUX)
void SetupLinuxSpecific() {
    // Linux-specific window setup
    // X11 or Wayland configuration
    // Window manager integration
}
#endif
```

### macOS-Specific

```cpp
#if defined(TKD_PLATFORM_MACOS)
void SetupMacOSSpecific() {
    // macOS-specific window setup
    // NSWindow configuration
    // Menu bar integration
}
#endif
```

## Performance Considerations

### Frame Rate Optimization

- **VSync Management**: Control VSync through window backend
- **Frame Pacing**: Use frame time for consistent updates
- **GPU Synchronization**: Proper GPU-CPU synchronization

### Input Processing Efficiency

- **Event Polling**: Minimize input polling frequency
- **State Caching**: Cache input states to avoid redundant queries
- **Debouncing**: Implement proper input debouncing

### Memory Management

- **Input Buffer Sizing**: Appropriate buffer sizes for input queues
- **Event Cleanup**: Proper cleanup of event handlers
- **Resource Pooling**: Reuse input-related objects

## Error Handling

### Window Creation Failures

```cpp
bool CreateWindowSafely(int width, int height, const FString& title) {
    #if TKD_ENGINE_CLIENT
    try {
        // Attempt window operations
        Window::SetTitle(title);

        // Verify window is valid
        if (!Window::IsOpen()) {
            FLogger::Log(ELogLevel::Error, "Window creation failed");
            return false;
        }

        // Check dimensions
        FVector2u actualSize = Window::GetDimensions();
        if (actualSize.x == 0 || actualSize.y == 0) {
            FLogger::Log(ELogLevel::Warning, "Window has invalid dimensions");
        }

        return true;
    } catch (const std::exception& e) {
        FLogger::Log(ELogLevel::Error, "Exception during window setup: " + std::string(e.what()));
        return false;
    }
    #else
    // Server mode - always succeed
    return true;
    #endif
}
```

### Input System Failures

```cpp
bool InitializeInputSafely() {
    #if TKD_ENGINE_CLIENT
    FInputManager* input = Window::GetInputManager();
    if (!input) {
        FLogger::Log(ELogLevel::Error, "Failed to get input manager");
        return false;
    }

    // Test input functionality
    try {
        // Test basic input queries
        bool keyState = input->IsKeyPressed(EKey::A);
        FVector2i mousePos = input->GetMousePosition();

        // Test gamepad if available
        if (input->IsGamepadConnected(0)) {
            float axisValue = input->GetGamepadAxisValue(0, EGamepadAxis::LeftX);
        }

        return true;
    } catch (const std::exception& e) {
        FLogger::Log(ELogLevel::Error, "Input system test failed: " + std::string(e.what()));
        return false;
    }
    #else
    // Server mode - no input needed
    return true;
    #endif
}
```

## Architecture Diagrams

### Window Interface Flow

```
Game Code
    │
    ▼
FWindowInterface (Static Methods)
    │
    ▼
std::mutex (Thread Safety)
    │
    ▼
FWindowSubsystem (Internal)
    │
    ├── IWindow (SFML/OpenGL)
    ├── IRenderer (Graphics API)
    ├── FInputManager (Input System)
    └── Performance Tracking
```

### Input Processing Pipeline

```
Physical Input (Keyboard/Mouse/Gamepad)
    │
    ▼
Window Backend (SFML/Platform)
    │
    ▼
FInputManager (Event Processing)
    │
    ▼
FWindowInterface::GetInputManager()
    │
    ▼
Game Code (Input Handling)
    │
    ▼
Actor/AI Response
```

### Performance Monitoring Flow

```
Frame Rendered
    │
    ▼
FWindowSubsystem (Calculates FPS)
    │
    ▼
Performance Data Updated
    │
    ▼
FWindowInterface::GetFPS()
    │
    ▼
Game Code (Monitoring/Display)
    │
    ▼
Performance Adjustments
```

## Troubleshooting

### Common Issues

#### Window Not Opening

**Symptoms:** `IsOpen()` returns `false` immediately

**Possible Causes:**
- Window subsystem not initialized
- Graphics driver issues
- Platform-specific problems
- Invalid window parameters

**Debug:**
```cpp
// Check initialization
if (!Window::IsInitialized()) {
    FLogger::Log(ELogLevel::Error, "Window subsystem not initialized");
    return;
}

// Check window state
if (!Window::IsOpen()) {
    FLogger::Log(ELogLevel::Error, "Window failed to open");

    // Try to get more information
    FVector2u size = Window::GetDimensions();
    FLogger::Log(ELogLevel::Info, FString::Format(
        "Window dimensions: %dx%d", size.x, size.y
    ));
}
```

#### Black/Empty Window

**Symptoms:** Window opens but shows black screen

**Possible Causes:**
- No rendering setup
- Renderer not initialized
- Graphics context issues
- Rendering thread problems

**Debug:**
```cpp
// Check renderer availability
FWindowSubsystem* windowSys = Window::GetWindowSubsystem();
if (windowSys) {
    IRenderer* renderer = windowSys->GetRenderer();
    if (!renderer) {
        FLogger::Log(ELogLevel::Error, "Renderer not available");
    }
}
```

#### Input Not Working

**Symptoms:** Input methods return default values

**Possible Causes:**
- Input manager not initialized
- Window not focused
- Platform input issues
- Input polling problems

**Debug:**
```cpp
FInputManager* input = Window::GetInputManager();
if (!input) {
    FLogger::Log(ELogLevel::Error, "Input manager not available");
    return;
}

// Test basic input
bool keyPressed = input->IsKeyPressed(EKey::A);
FVector2i mousePos = input->GetMousePosition();

FLogger::Log(ELogLevel::Info, FString::Format(
    "Input test - Key A: %s, Mouse: (%d, %d)",
    keyPressed ? "pressed" : "not pressed",
    mousePos.x, mousePos.y
));
```

#### Low Performance

**Symptoms:** Low FPS, high frame times

**Possible Causes:**
- Inefficient rendering
- Too many draw calls
- CPU-bound operations
- Memory issues

**Debug:**
```cpp
// Monitor performance
float fps = Window::GetFPS();
float frameTime = Window::GetAverageFrameTime();

if (fps < 30.0f) {
    FLogger::Log(ELogLevel::Warning, FString::Format(
        "Low FPS: %.1f, Frame Time: %.2fms", fps, frameTime
    ));

    // Enable performance profiling
    StartDetailedProfiling();
}
```

### Platform-Specific Issues

#### Windows Issues

- **DPI Scaling**: Ensure DPI awareness is set
- **Window Focus**: Check window focus for input
- **Graphics Drivers**: Verify DirectX/OpenGL support

#### Linux Issues

- **Display Server**: X11 vs Wayland compatibility
- **Window Manager**: Compatibility with desktop environments
- **Graphics Drivers**: Mesa vs proprietary drivers

#### macOS Issues

- **App Lifecycle**: Proper NSApplication setup
- **Window Focus**: macOS focus policies
- **Graphics**: Metal vs OpenGL selection

## Future Enhancements

### Planned Features

1. **Multiple Windows**: Support for multiple simultaneous windows
2. **Window Modes**: Borderless, fullscreen, windowed fullscreen
3. **Advanced Input**: Gesture recognition, multi-touch
4. **VR Support**: VR headset integration
5. **Performance Profiling**: Built-in detailed profiling tools
6. **Accessibility**: Screen reader and input method support

### Extension Points

```cpp
// Custom window management
class FExtendedWindowInterface {
public:
    static void SetWindowIcon(const FString& iconPath) {
        #if TKD_ENGINE_CLIENT
        // Implementation for setting window icon
        #endif
    }

    static void FlashWindow() {
        #if TKD_ENGINE_CLIENT
        // Implementation for window flashing (taskbar)
        #endif
    }

    static void SetWindowOpacity(float opacity) {
        #if TKD_ENGINE_CLIENT
        // Implementation for window transparency
        #endif
    }

    static FString GetClipboardText() {
        #if TKD_ENGINE_CLIENT
        // Implementation for clipboard access
        return "";
        #else
        return "";
        #endif
    }

    static void SetClipboardText(const FString& text) {
        #if TKD_ENGINE_CLIENT
        // Implementation for clipboard setting
        #endif
    }
};
```

---

*This documentation covers the complete FWindowInterface as of TKD Engine v1.0.0. The interface provides thread-safe access to window operations and is only available in client builds for optimal server performance.*
