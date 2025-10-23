# Window System Documentation

## Overview

The TKD Engine Window system provides a comprehensive, cross-platform window and input management solution built on top of SFML. It serves as the primary interface between the game engine and the host operating system, handling window creation, input processing, OpenGL context management, and event dispatching.

### Key Features

- **Cross-Platform Window Management**: Consistent API across Windows, Linux, and macOS
- **Complete Input System**: Keyboard, mouse, and gamepad support with deadzone handling
- **OpenGL Integration**: Automatic context creation and state management
- **Event-Driven Architecture**: Comprehensive event system for window state changes
- **ImGui Integration**: Built-in debug UI support for development
- **View Management**: 2D camera/viewport system integration
- **Thread-Safe Operations**: Proper synchronization for multi-threaded applications
- **Performance Optimized**: Efficient input polling and event processing

### Architecture

The window system follows a classic interface-implementation pattern for maximum portability:

```
┌─────────────────┐
│    IWindow      │ ← Abstract Interface
│                 │
│ • Window Mgmt   │
│ • Input System  │
│ • Event System  │
│ • OpenGL Context│
└─────────────────┘
         ▲
         │
┌─────────────────┐
│ SFML::Window    │ ← Concrete Implementation
│                 │
│ • SFML Backend  │
│ • OpenGL Setup  │
│ • Input Handling│
│ • Event Processing│
└─────────────────┘
```

## Core Components

### EWindowState Enumeration

Defines the possible window display states:

```cpp
enum class EWindowState : UInt8
{
    Minimized = 0,   // Window is minimized/iconified
    Maximized,       // Window is maximized (fills screen but not fullscreen)
    Fullscreen,      // Exclusive fullscreen mode
    Windowed         // Standard windowed mode
};
```

#### State Descriptions

- **`Minimized`**: Window is minimized to the taskbar/dock. No rendering occurs.
- **`Maximized`**: Window fills the screen but retains window decorations and taskbar.
- **`Fullscreen`**: Exclusive fullscreen mode with no window decorations or taskbar.
- **`Windowed`**: Standard resizable window with decorations and taskbar access.

### Input Enumerations

#### EKeyboardKeys

Comprehensive keyboard key enumeration covering all standard keys:

```cpp
enum class EKeyboardKeys
{
    Unknown = -1,
    A = 0, B, C, D, E, F, G, H, I, J, K, L, M,
    N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,
    Escape, LControl, LShift, LAlt, LSystem,
    RControl, RShift, RAlt, RSystem, Menu,
    LBracket, RBracket, Semicolon, Comma, Period, Quote, Slash, Backslash,
    Tilde, Equal, Hyphen, Space, Enter, Backspace, Tab,
    PageUp, PageDown, End, Home, Insert, Delete,
    Add, Subtract, Multiply, Divide,
    Left, Right, Up, Down,
    Numpad0, Numpad1, Numpad2, Numpad3, Numpad4,
    Numpad5, Numpad6, Numpad7, Numpad8, Numpad9,
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
    F13, F14, F15, Pause
};
```

#### EMouseButtons

Mouse button enumeration:

```cpp
enum class EMouseButtons
{
    Unknown = -1,
    Left = 101,      // Left mouse button (primary)
    Right,           // Right mouse button (secondary)
    Middle,          // Middle mouse button (wheel click)
    XButton1,        // First extended button
    XButton2         // Second extended button
};
```

#### EGamepadButtons

Standard gamepad button layout:

```cpp
enum class EGamepadButtons
{
    Unknown = -1,
    A = 106, B, X, Y,        // Face buttons
    Back, Guide, Start,      // Special buttons
    LeftStick, RightStick,   // Stick clicks
    LeftShoulder, RightShoulder, // Shoulder buttons
    DPadUp, DPadDown, DPadLeft, DPadRight // D-pad
};
```

#### EGamepadAxes

Analog axis enumeration:

```cpp
enum class EGamepadAxes
{
    Unknown = -1,
    LeftX = 121, LeftY,      // Left analog stick
    RightX, RightY,          // Right analog stick
    LeftTrigger, RightTrigger // Trigger buttons
};
```

## Interface Definition (IWindow)

The `IWindow` class defines the abstract interface for all window implementations:

```cpp
namespace tkd
{
class IWindow : public TEventEmitter<TEvents<IWindow>::All>
{
public:
    // Window lifecycle
    virtual bool Open(void) = 0;
    virtual bool Close(void) = 0;
    virtual bool IsOpen(void) const = 0;

    // Window properties
    virtual bool SetState(const EWindowState& state) = 0;
    virtual const EWindowState& GetState(void) const = 0;
    virtual bool SetPosition(const FVector2i& position) = 0;
    virtual const FVector2i& GetPosition(void) const = 0;
    virtual bool SetDimension(const FVector2u& dimension) = 0;
    virtual const FVector2u& GetDimensions(void) const = 0;
    virtual bool SetTitle(const FString& title) = 0;
    virtual const FString& GetTitle(void) const = 0;

    // Graphics settings
    virtual bool SetVSync(bool enabled) = 0;
    virtual bool IsVSync(void) const = 0;
    virtual void SetDebugMode(bool debugMode) = 0;

    // OpenGL context
    virtual void* GetNativeHandle(void) const = 0;
    virtual bool SetActive(bool active = true) = 0;

    // Input system
    virtual bool IsKeyPressed(EKeyboardKeys key) const = 0;
    virtual bool IsMouseButtonPressed(EMouseButtons button) const = 0;
    virtual bool IsGamepadConnected(UInt32 gamepadIndex = 0) const = 0;
    virtual bool IsGamepadButtonPressed(EGamepadButtons button, UInt32 gamepadIndex = 0) const = 0;
    virtual float GetGamepadAxis(EGamepadAxes axis, UInt32 gamepadIndex = 0) const = 0;
    virtual FVector2i GetMousePosition(void) const = 0;
    virtual void SetMousePosition(const FVector2i& position) = 0;

    // View management
    virtual FView GetDefaultView(void) const = 0;
    virtual const FView& GetCurrentView(void) const = 0;
    virtual void SetCurrentView(const FView& view) = 0;

    // Main loop
    virtual void Update(float deltaTime) = 0;
    virtual void Draw(const std::function<void(void)>& drawFunction) = 0;
};
}
```

## Implementation (SFML::Window)

The `SFML::Window` class provides the concrete SFML-based implementation:

```cpp
namespace tkd::SFML
{
class Window : public IWindow
{
private:
    static const float DPAD_DEADZONE;      // 0.10f - D-pad deadzone
    static const float DEADZONE;           // 0.08f - Analog stick deadzone
    static const float TRIGGER_DEADZONE;   // 0.10f - Trigger deadzone

    std::unique_ptr<sf::RenderWindow> m_window;
    sf::Clock m_clock;
    EWindowState m_state;
    FVector2i m_position;
    FVector2u m_dimension;
    FString m_title;
    bool m_vsync;
    bool m_imguiInitialized;
    FView m_view;

public:
    Window(const FString& title, bool openDefault = true,
           const FVector2i& position = FVector2i::Zero,
           const FVector2u& dimension = FVector2u(1280, 720),
           EWindowState state = EWindowState::Windowed);

    virtual ~Window();

    // Implementation of all IWindow methods
    virtual bool Open(void) override;
    virtual bool Close(void) override;
    virtual bool IsOpen(void) const override;
    // ... all other interface methods
};
}
```

### Key Implementation Details

- **SFML Backend**: Uses `sf::RenderWindow` for cross-platform window management
- **OpenGL Context**: Automatic OpenGL 2.1 context creation with depth/stencil buffers
- **Input Deadzones**: Configurable deadzones for analog inputs to prevent drift
- **Event Processing**: Comprehensive SFML event handling with custom event emission
- **ImGui Integration**: Optional debug UI overlay for development
- **State Caching**: Internal state tracking to minimize redundant SFML calls

## Window Lifecycle

### Creation and Initialization

#### Constructor

```cpp
Window::Window(
    const FString& title,
    bool openDefault = true,
    const FVector2i& position = FVector2i::Zero,
    const FVector2u& dimension = FVector2u(1280, 720),
    EWindowState state = EWindowState::Windowed
)
```

**Parameters:**
- `title`: Window title string
- `openDefault`: Whether to open window immediately (default: true)
- `position`: Initial window position (default: (0,0))
- `dimension`: Initial window size (default: 1280×720)
- `state`: Initial window state (default: Windowed)

#### Open()

Creates and displays the window with the specified parameters:

```cpp
bool Window::Open(void)
{
    // Create SFML context settings
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 2;
    settings.minorVersion = 1;

    // Create window
    m_window = std::make_unique<sf::RenderWindow>(
        ToSFMLVideoMode(m_dimension),
        m_title.CStr(),
        ToSFMLStyle(m_state),
        settings
    );

    // Initialize OpenGL
    InitializeOpenGL();

    // Setup window properties
    m_window->setPosition(Utils::Convert(m_position));
    m_window->setVerticalSyncEnabled(m_vsync);
    m_window->setView(Utils::Convert(m_view));

    // Emit opened event
    this->Emit(Events::Opened{ m_position, m_dimension, m_state });

    return true;
}
```

### State Management

#### SetState()

Changes the window display state (windowed, maximized, fullscreen, minimized):

```cpp
bool Window::SetState(const EWindowState& state)
{
    if (!IsOpen() || m_state == state) return false;

    EWindowState oldState = m_state;
    m_state = state;

    // Close and recreate window with new state
    sf::Vector2i currentPosition = m_window->getPosition();
    m_window->close();

    // Shutdown ImGui if active
    if (m_imguiInitialized) {
        ImGui::SFML::Shutdown();
        m_imguiInitialized = false;
    }

    // Recreate with new state
    m_window->create(ToSFMLVideoMode(m_dimension), m_title.CStr(), ToSFMLStyle(m_state));
    m_window->setPosition(currentPosition);
    m_window->setVerticalSyncEnabled(m_vsync);
    m_window->setView(Utils::Convert(m_view));

    // Reinitialize ImGui if in debug mode
    if (Engine::GetInstance().GetSettings().debug) {
        SetDebugMode(true);
    }

    // Emit state change event
    this->Emit(Events::StateChanged{ oldState, m_state });

    return true;
}
```

### Property Management

#### Position and Size

```cpp
bool SetPosition(const FVector2i& position);
const FVector2i& GetPosition(void) const;

bool SetDimension(const FVector2u& dimension);
const FVector2u& GetDimensions(void) const;
```

#### Title Management

```cpp
bool SetTitle(const FString& title);
const FString& GetTitle(void) const;
```

#### VSync Control

```cpp
bool SetVSync(bool enabled);
bool IsVSync(void) const;
```

### Cleanup

#### Close()

Properly closes the window and cleans up resources:

```cpp
bool Window::Close(void)
{
    if (!IsOpen()) return false;

    // Close SFML window
    m_window->close();

    // Shutdown ImGui
    if (m_imguiInitialized) {
        ImGui::SFML::Shutdown();
        m_imguiInitialized = false;
    }

    // Reset window pointer
    m_window.reset();

    // Emit closed event
    this->Emit(Events::Closed{});

    return true;
}
```

## Input System

### Keyboard Input

#### IsKeyPressed()

Checks if a specific keyboard key is currently pressed:

```cpp
bool Window::IsKeyPressed(EKeyboardKeys key) const
{
    if (!IsOpen()) return false;

    sf::Keyboard::Key sfmlKey = Utils::Convert(key);
    return sf::Keyboard::isKeyPressed(sfmlKey);
}
```

**Supported Keys:** All standard keyboard keys including modifiers, function keys, and numpad.

### Mouse Input

#### Button State

```cpp
bool IsMouseButtonPressed(EMouseButtons button) const;
```

#### Position Management

```cpp
FVector2i GetMousePosition(void) const;
void SetMousePosition(const FVector2i& position);
```

**Note:** Mouse position is relative to the window's client area.

### Gamepad Input

#### Connection Detection

```cpp
bool IsGamepadConnected(UInt32 gamepadIndex = 0) const;
```

#### Button State

```cpp
bool IsGamepadButtonPressed(EGamepadButtons button, UInt32 gamepadIndex = 0) const;
```

#### Analog Axes

```cpp
float GetGamepadAxis(EGamepadAxes axis, UInt32 gamepadIndex = 0) const;
```

**Deadzone Handling:** The window system automatically applies deadzones to prevent drift:
- **Analog Sticks**: 8% deadzone (0.08f)
- **D-pad**: 10% deadzone (0.10f)
- **Triggers**: 10% deadzone (0.10f)

## OpenGL Integration

### Context Management

#### SetActive()

Makes the OpenGL context current for the calling thread:

```cpp
bool Window::SetActive(bool active)
{
    if (!IsOpen()) return false;
    return m_window->setActive(active);
}
```

#### GetNativeHandle()

Returns the platform-specific window handle for advanced OpenGL operations:

```cpp
void* Window::GetNativeHandle(void) const
{
    if (!IsOpen()) return nullptr;
    return m_window->getSystemHandle();
}
```

### OpenGL Initialization

The window automatically configures OpenGL state on creation:

```cpp
void Window::InitializeOpenGL(void)
{
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Enable back-face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // Enable lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);

    // Configure default light
    GLfloat lightPos[] = { 10.0f, 10.0f, 10.0f, 1.0f };
    GLfloat lightAmbient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);

    // Set viewport and projection
    glViewport(0, 0, m_dimension.x, m_dimension.y);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)m_dimension.x / m_dimension.y, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Clear color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Smooth shading
    glShadeModel(GL_SMOOTH);
}
```

## Event System

The window system integrates with the engine's event emitter for state change notifications:

### Window Events

```cpp
struct Events
{
    struct Opened {
        FVector2i position;
        FVector2u dimension;
        EWindowState state;
    };

    struct Closed {};

    struct StateChanged {
        EWindowState oldState;
        EWindowState newState;
    };

    struct Resized {
        FVector2u oldDimension;
        FVector2u newDimension;
    };

    struct Moved {
        FVector2i oldPosition;
        FVector2i newPosition;
    };

    struct LostFocus {};
    struct GainedFocus {};
};
```

### Event Usage

```cpp
// Subscribe to window events
window->On<Window::Events::Resized>([](const Window::Events::Resized& event) {
    FLogger::Info("Window resized from {}x{} to {}x{}",
                  event.oldDimension.x, event.oldDimension.y,
                  event.newDimension.x, event.newDimension.y);

    // Update viewport
    glViewport(0, 0, event.newDimension.x, event.newDimension.y);
});

window->On<Window::Events::StateChanged>([](const Window::Events::StateChanged& event) {
    FLogger::Info("Window state changed from {} to {}",
                  (int)event.oldState, (int)event.newState);
});
```

## View Management

### Default View

```cpp
FView Window::GetDefaultView(void) const
{
    sf::View sfmlView = m_window->getDefaultView();
    return FView(FRectangle(
        sfmlView.getCenter().x - sfmlView.getSize().x / 2.0f,
        sfmlView.getCenter().y - sfmlView.getSize().y / 2.0f,
        sfmlView.getSize().x,
        sfmlView.getSize().y
    ));
}
```

### Current View

```cpp
const FView& GetCurrentView(void) const;
void SetCurrentView(const FView& view);
```

## Debug Integration

### ImGui Support

The window system includes built-in ImGui integration for development:

```cpp
void Window::SetDebugMode(bool debugMode)
{
    if (debugMode && !m_imguiInitialized && IsOpen()) {
        ImGui::SFML::Init(*m_window);
        m_imguiInitialized = true;
    } else if (!debugMode && m_imguiInitialized) {
        ImGui::SFML::Shutdown();
        m_imguiInitialized = false;
    }
}
```

### Debug UI Features

- **Performance Metrics**: Frame time, FPS, memory usage
- **Input Display**: Real-time input state visualization
- **Window Information**: Current state, position, size
- **OpenGL State**: Current OpenGL context information

## Usage Examples

### Basic Window Creation

```cpp
#include <Engine/Renderer/SFML/Window.hpp>

// Create a window
auto window = std::make_unique<tkd::SFML::Window>(
    "My Game",
    true,  // Open immediately
    FVector2i(100, 100),  // Position
    FVector2u(1280, 720), // Size
    EWindowState::Windowed
);

// Check if window opened successfully
if (!window->IsOpen()) {
    FLogger::Fatal("Failed to create window!");
    return EXIT_FAILURE;
}
```

### Main Game Loop

```cpp
// Main game loop
sf::Clock clock;
while (window->IsOpen()) {
    float deltaTime = clock.restart().asSeconds();

    // Process window events and input
    window->Update(deltaTime);

    // Handle input
    if (window->IsKeyPressed(EKeyboardKeys::Escape)) {
        window->Close();
        break;
    }

    // Toggle fullscreen
    if (window->IsKeyPressed(EKeyboardKeys::F11)) {
        static bool fullscreen = false;
        fullscreen = !fullscreen;
        window->SetState(fullscreen ? EWindowState::Fullscreen : EWindowState::Windowed);
    }

    // Render frame
    window->Draw([]() {
        // Clear screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Your rendering code here
        renderer->BeginFrame();
        // ... draw game objects ...
        renderer->EndFrame();
    });
}
```

### Input Handling

```cpp
void ProcessInput(const IWindow& window) {
    // Keyboard input
    if (window.IsKeyPressed(EKeyboardKeys::W)) {
        player.MoveForward();
    }
    if (window.IsKeyPressed(EKeyboardKeys::S)) {
        player.MoveBackward();
    }

    // Mouse input
    FVector2i mousePos = window.GetMousePosition();
    if (window.IsMouseButtonPressed(EMouseButtons::Left)) {
        player.Shoot(mousePos);
    }

    // Gamepad input
    if (window.IsGamepadConnected(0)) {
        float leftX = window.GetGamepadAxis(EGamepadAxes::LeftX, 0);
        float leftY = window.GetGamepadAxis(EGamepadAxes::LeftY, 0);

        if (std::abs(leftX) > 0.1f || std::abs(leftY) > 0.1f) {
            player.Move(leftX, leftY);
        }

        if (window.IsGamepadButtonPressed(EGamepadButtons::A, 0)) {
            player.Jump();
        }
    }
}
```

### Event-Driven Architecture

```cpp
void SetupWindowEvents(IWindow& window) {
    // Window resize handling
    window.On<IWindow::Events::Resized>([](const auto& event) {
        FLogger::Info("Window resized to {}x{}",
                      event.newDimension.x, event.newDimension.y);

        // Update renderer viewport
        renderer->SetViewport(0, 0, event.newDimension.x, event.newDimension.y);

        // Update camera aspect ratio
        camera.SetAspectRatio((float)event.newDimension.x / event.newDimension.y);
    });

    // Focus change handling
    window.On<IWindow::Events::LostFocus>([]() {
        FLogger::Info("Window lost focus - pausing game");
        game.Pause();
    });

    window.On<IWindow::Events::GainedFocus>([]() {
        FLogger::Info("Window gained focus - resuming game");
        game.Resume();
    });

    // State change handling
    window.On<IWindow::Events::StateChanged>([](const auto& event) {
        FLogger::Info("Window state changed from {} to {}",
                      (int)event.oldState, (int)event.newState);
    });
}
```

### Advanced Configuration

```cpp
void ConfigureWindow(IWindow& window) {
    // Set window properties
    window.SetTitle("Advanced Game Engine");
    window.SetPosition(FVector2i(50, 50));
    window.SetDimension(FVector2u(1920, 1080));
    window.SetVSync(true);

    // Enable debug mode in development
    #ifdef DEBUG
    window.SetDebugMode(true);
    #endif

    // Set up event handlers
    SetupWindowEvents(window);

    // Configure input settings
    // (Deadzone constants are defined in the Window class)
}
```

## Performance Considerations

### Input Polling

- **Cached State**: Input state is polled once per frame in `Update()`
- **Efficient Queries**: `IsKeyPressed()` etc. return cached values
- **Minimal Overhead**: SFML's input functions are highly optimized

### Event Processing

- **Batch Processing**: All events processed in single `Update()` call
- **Callback Efficiency**: Event callbacks should be lightweight
- **Thread Safety**: Event emission is thread-safe

### OpenGL Context

- **Single Context**: One OpenGL context per window
- **Context Switching**: Use `SetActive()` when switching between windows
- **Resource Management**: Proper cleanup in destructor

### Memory Management

- **RAII Design**: Window resources automatically cleaned up
- **Unique Pointers**: SFML window managed with `std::unique_ptr`
- **Minimal Allocations**: No dynamic allocation during normal operation

## Platform-Specific Notes

### Windows
- Uses Win32 API for native handle
- Supports high-DPI displays
- Compatible with Windows Game Bar and overlays

### Linux
- Uses X11 for window management
- Supports Wayland through SFML
- Compatible with desktop environments (GNOME, KDE, etc.)

### macOS
- Uses Cocoa framework
- Native fullscreen support
- Compatible with Mission Control and Spaces

## Troubleshooting

### Common Issues

1. **Window Won't Open**
   - Check OpenGL drivers
   - Verify SFML installation
   - Check console for error messages

2. **Input Not Working**
   - Ensure window has focus
   - Check for input deadzone issues
   - Verify gamepad connection

3. **Performance Issues**
   - Enable VSync to prevent screen tearing
   - Check for excessive event callbacks
   - Profile OpenGL operations

4. **OpenGL Errors**
   - Ensure context is active before OpenGL calls
   - Check for deprecated OpenGL functions
   - Verify viewport settings

### Debug Features

- **ImGui Integration**: Built-in debug UI
- **Event Logging**: All events can be logged
- **Input Visualization**: Real-time input state display
- **Performance Metrics**: Frame time and FPS monitoring

## Architecture Diagrams

### Window Lifecycle

```
┌─────────────┐
│ Constructor │
│ (Create)    │
└─────────────┘
       │
       ▼
┌─────────────┐    ┌─────────────┐
│ Open()      │───▶│ Initialize  │
│ Called      │    │ OpenGL      │
└─────────────┘    └─────────────┘
       │
       ▼
┌─────────────┐    ┌─────────────┐
│ Main Loop   │───▶│ Update()    │
│ Running     │    │ Process      │
│             │    │ Events       │
└─────────────┘    └─────────────┘
       │
       ▼
┌─────────────┐    ┌─────────────┐
│ Close()     │───▶│ Cleanup     │
│ Called      │    │ Resources   │
└─────────────┘    └─────────────┘
```

### Input Processing Flow

```
┌─────────────┐
│ Update()    │
│ Called      │
└─────────────┘
       │
       ▼
┌─────────────┐    ┌─────────────┐
│ Poll SFML   │───▶│ Process     │
│ Events      │    │ Window      │
└─────────────┘    │ Events      │
       │           └─────────────┘
       ▼
┌─────────────┐    ┌─────────────┐
│ Cache Input │───▶│ Apply       │
│ State       │    │ Deadzones   │
└─────────────┘    └─────────────┘
       │
       ▼
┌─────────────┐
│ Game Logic  │
│ Uses Input  │
└─────────────┘
```

### Event System Architecture

```
┌─────────────┐
│ Window      │
│ State       │
│ Changes     │
└─────────────┘
       │
       ▼
┌─────────────┐    ┌─────────────┐
│ Emit Event  │───▶│ Event       │
│ (Thread-safe│    │ Emitter     │
│ )           │    └─────────────┘
└─────────────┘           │
                          ▼
                ┌────────▼────────┐
                │ Event Callbacks │
                │ (User-defined)  │
                └─────────────────┘
```

## Future Extensions

The window system architecture supports future enhancements:

- **Multiple Windows**: Support for multiple simultaneous windows
- **Custom Cursors**: Programmable cursor shapes and animations
- **Touch Input**: Multi-touch gesture recognition
- **VR Integration**: Head-mounted display support
- **High-DPI Support**: Enhanced scaling for 4K+ displays
- **Window Compositing**: Advanced window effects and transparency

---

*This documentation covers the complete TKD Engine Window system as of the current version. For the latest updates and additional examples, refer to the engine's source code and test suites.*
