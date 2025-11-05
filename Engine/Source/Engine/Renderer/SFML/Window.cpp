///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Renderer/SFML/Window.hpp>
#include <Engine/Renderer/FCamera.hpp>
#include <Engine/Renderer/VR.hpp>
#include <Engine/Static/FEngineInterface.hpp>
#if TKD_ENGINE_CLIENT
    #include <GL/glew.h>
    #include <GL/glu.h>
    #include <imgui-SFML.h>
    #include <imgui.h>
    #include <SFML/OpenGL.hpp>
#endif

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::SFML
///////////////////////////////////////////////////////////////////////////////
namespace tkd::SFML
{

#if TKD_ENGINE_CLIENT

///////////////////////////////////////////////////////////////////////////////
const float Window::DPAD_DEADZONE = 0.10f;      //<! Deadzone for D-Pad axes
const float Window::DEADZONE = 0.08f;           //<! Deadzone
const float Window::TRIGGER_DEADZONE = 0.10f;   //<! Deadzone

///////////////////////////////////////////////////////////////////////////////
Window::Window(
    const FString& title,
    bool openDefault,
    const FVector2i& position,
    const FVector2u& dimension,
    EWindowState state
)
    : m_window(nullptr)
    , m_state(state)
    , m_position(position)
    , m_dimension(dimension)
    , m_title(title)
    , m_vsync(false)
    , m_imguiInitialized(false)
    , m_view(GetDefaultView())
{
    if (openDefault) { this->Open(); }
}

///////////////////////////////////////////////////////////////////////////////
Window::~Window()
{
    // Ensure proper cleanup
    if (IsOpen()) { Close(); }
}

///////////////////////////////////////////////////////////////////////////////
sf::VideoMode Window::ToSFMLVideoMode(const FVector2u& dimension)
{
    return sf::VideoMode(dimension.x, dimension.y, 32);
}

///////////////////////////////////////////////////////////////////////////////
sf::Uint32 Window::ToSFMLStyle(const EWindowState& state)
{
    switch (state)
    {
    case EWindowState::Minimized: return sf::Style::None;
    case EWindowState::Maximized:
        return sf::Style::Titlebar | sf::Style::Resize | sf::Style::Close;
    case EWindowState::Fullscreen: return sf::Style::Fullscreen;
    case EWindowState::Windowed  :
    default                      : return sf::Style::Titlebar | sf::Style::Resize | sf::Style::Close;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Window::SetFPSLimit(UInt32 fps)
{
    if (IsOpen() && m_window) { m_window->setFramerateLimit(fps); }
}

///////////////////////////////////////////////////////////////////////////////
void Window::InitializeOpenGL(void)
{
    // Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK)
    {
        FLogger::SetNamespace("OpenGL");
        FLogger::Error(
            "Failed to initialize GLEW: {}",
            reinterpret_cast<const char*>(glewGetErrorString(glewError))
        );
        return;
    }

    FLogger::SetNamespace("OpenGL");
    FLogger::Info("GLEW initialized successfully");
    FLogger::Info(
        "Using OpenGL version: {}",
        reinterpret_cast<const char*>(glGetString(GL_VERSION))
    );

    if (!VR::FVRSystem::GetInstance().IsInitialized())
    {
        FLogger::SetNamespace("OpenGL");
        FLogger::Info(
            "No VR detected - performing standard OpenGL initialization"
        );

        // Enable depth testing
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        // Enable multisampling for anti-aliasing
        glEnable(GL_MULTISAMPLE);

        // Enable back-face culling
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);

        // Enable lighting
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_COLOR_MATERIAL);
        glEnable(GL_NORMALIZE);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

        // Set up light
        GLfloat lightPos[] = { 10.0f, 10.0f, 10.0f, 1.0f };
        GLfloat lightAmbient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
        GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
        glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);

        // Create a default camera
        FCamera defaultCamera;

        // Set up viewport and projection
        glViewport(0, 0, m_dimension.x, m_dimension.y);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(
            defaultCamera.fov,
            defaultCamera.aspectRatio,
            defaultCamera.nearPlane,
            defaultCamera.farPlane
        );
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Clear color (background)
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        // Enable smooth shading
        glShadeModel(GL_SMOOTH);
    }
    else
    {
        FLogger::SetNamespace("OpenGL");
        FLogger::Info("VR detected - skipping standard OpenGL initialization");

        // VR Mode - minimal OpenGL state setup
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_MULTISAMPLE);

        // Enable lighting
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_COLOR_MATERIAL);
        glEnable(GL_NORMALIZE);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

        // Set up light
        GLfloat lightPos[] = { 10.0f, 10.0f, 10.0f, 1.0f };
        GLfloat lightAmbient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
        GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
        glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    }
}

///////////////////////////////////////////////////////////////////////////////
bool Window::Open(void)
{
    // Check if the window is already open
    if (IsOpen()) { return false; }

    // Create the SFML Context Settings
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 3;
    settings.minorVersion = 3;

    // Create the SFML window
    m_window = std::make_unique<sf::RenderWindow>(
        ToSFMLVideoMode(m_dimension),
        m_title.CStr(),
        ToSFMLStyle(m_state),
        settings
    );

    // Check if the window was created successfully
    if (!m_window || !m_window->isOpen()) { return false; }

    // Set the windows OpenGL context as active
    m_window->setActive(true);

    // Initialize OpenGL settings
    InitializeOpenGL();

    // Set initial position and VSync
    m_window->setPosition(Utils::Convert(m_position));
    m_window->setVerticalSyncEnabled(m_vsync);
    m_window->setView(Utils::Convert(m_view));

    // Emit the Opened event
    this->Emit(Events::Opened{ m_position, m_dimension, m_state });

    // Successfully opened the window
    return true;
}

///////////////////////////////////////////////////////////////////////////////
void Window::SetDebugMode(bool debugMode)
{
    if (debugMode && !m_imguiInitialized && IsOpen())
    {
        if (ImGui::SFML::Init(*m_window)) { m_imguiInitialized = true; }
    }
    else if (!debugMode && m_imguiInitialized)
    {
        ImGui::SFML::Shutdown();
        m_imguiInitialized = false;
    }
}

///////////////////////////////////////////////////////////////////////////////
bool Window::Close(void)
{
    // Check if the window is open
    if (!IsOpen()) { return false; }

    // Close the SFML window first (before ImGui cleanup to avoid X11 cursor
    // errors)
    m_window->close();

    // Shutdown ImGui-SFML after closing the window
    if (m_imguiInitialized)
    {
        ImGui::SFML::Shutdown();
        m_imguiInitialized = false;
    }

    // Reset the window pointer
    m_window.reset();

    // Emit the Closed event
    this->Emit(Events::Closed{});

    // Successfully closed the window
    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool Window::IsOpen(void) const { return m_window && m_window->isOpen(); }

///////////////////////////////////////////////////////////////////////////////
const EWindowState& Window::GetState(void) const { return m_state; }

///////////////////////////////////////////////////////////////////////////////
bool Window::SetState(const EWindowState& state)
{
    // Check if the window is open
    if (!IsOpen()) { return false; }

    // Check if the new state is different from the current state
    if (m_state == state) { return true; }

    // Store the old state for event emission
    EWindowState oldState = m_state;

    // Update the window state
    m_state = state;

    // Recreate the SFML window with the new state
    sf::Vector2i currentPosition = m_window->getPosition();

    // Close the window first (before ImGui cleanup to avoid X11 cursor errors)
    m_window->close();

    // Shutdown ImGui after closing the window
    if (m_imguiInitialized)
    {
        ImGui::SFML::Shutdown();
        m_imguiInitialized = false;
    }

    // Recreate the window with new state
    m_window->create(
        ToSFMLVideoMode(m_dimension), m_title.CStr(), ToSFMLStyle(m_state)
    );
    m_window->setPosition(currentPosition);
    m_window->setVerticalSyncEnabled(m_vsync);
    m_window->setView(Utils::Convert(m_view));

    // Reinitialize ImGui if in debug build
    if (Engine::GetInstance().GetSettings().debug)
    {
        if (ImGui::SFML::Init(*m_window)) { m_imguiInitialized = true; }
    }

    // Emit the StateChanged event
    this->Emit(Events::StateChanged{ oldState, m_state });

    // Successfully changed the window state
    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool Window::SetPosition(const FVector2i& position)
{
    // Check if the window is open
    if (!IsOpen()) { return false; }

    // Check if the new position is different from the current position
    if (m_position == position) { return true; }

    // Store the old position for event emission
    FVector2i oldPosition = m_position;

    // Update the window position
    m_position = position;
    m_window->setPosition(sf::Vector2i(m_position.x, m_position.y));

    // Emit the Moved event
    this->Emit(Events::Moved{ oldPosition, m_position });

    // Successfully changed the window position
    return true;
}

///////////////////////////////////////////////////////////////////////////////
const FVector2i& Window::GetPosition(void) const { return m_position; }

///////////////////////////////////////////////////////////////////////////////
bool Window::SetDimension(const FVector2u& dimension)
{
    // Check if the window is open
    if (!IsOpen()) { return false; }

    // Check if the new dimension is different from the current dimension
    if (m_dimension == dimension) { return true; }

    // Store the old dimension for event emission
    FVector2u oldDimension = m_dimension;

    // Update the window dimension
    m_dimension = dimension;
    m_window->setSize(sf::Vector2u(m_dimension.x, m_dimension.y));

    // Emit the Resized event
    this->Emit(Events::Resized{ oldDimension, m_dimension });

    // Successfully changed the window dimension
    return true;
}

///////////////////////////////////////////////////////////////////////////////
const FVector2u& Window::GetDimensions(void) const { return m_dimension; }

///////////////////////////////////////////////////////////////////////////////
bool Window::SetTitle(const FString& title)
{
    // Check if the window is open
    if (!IsOpen()) { return false; }

    // Check if the new title is different from the current title
    if (m_title == title) { return true; }

    // Update the window title
    m_title = title;
    m_window->setTitle(m_title.CStr());

    // Successfully changed the window title
    return true;
}

///////////////////////////////////////////////////////////////////////////////
const FString& Window::GetTitle(void) const { return m_title; }

///////////////////////////////////////////////////////////////////////////////
bool Window::SetVSync(bool enabled)
{
    // Check if the window is open
    if (!IsOpen()) { return false; }

    // Check if the new VSync setting is different from the current setting
    if (m_vsync == enabled) { return true; }

    // Update the VSync setting
    m_vsync = enabled;
    m_window->setVerticalSyncEnabled(m_vsync);

    // Successfully changed the VSync setting
    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool Window::IsVSync(void) const { return m_vsync; }

///////////////////////////////////////////////////////////////////////////////
void* Window::GetNativeHandle(void) const
{
    // Check if the window is open
    if (!IsOpen()) { return nullptr; }

    // Return the native handle of the SFML window
    return static_cast<void*>(m_window.get());
}

///////////////////////////////////////////////////////////////////////////////
void Window::Update(TKD_MAYBE_UNUSED float deltaTime)
{
    // Check if the window is open
    if (!IsOpen()) { return; }

    // Update the current view in case it was changed externally
    m_window->setView(Utils::Convert(m_view));

    // Check for window move events
    sf::Vector2i currentPosition = m_window->getPosition();
    if (m_position.x != currentPosition.x || m_position.y != currentPosition.y)
    {
        FVector2i oldPosition = m_position;
        m_position = FVector2i(currentPosition.x, currentPosition.y);
        this->Emit(Events::Moved{ oldPosition, m_position });
    }

    // Restart the clock and get the elapsed time since the last update
    sf::Time delta = m_clock.restart();

    // Update ImGui-SFML if it was initialized
    if (m_imguiInitialized) { ImGui::SFML::Update(*m_window, delta); }

    // Process SFML events
    sf::Event event;
    while (m_window->pollEvent(event))
    {
        if (m_imguiInitialized)
        {
            ImGui::SFML::ProcessEvent(*m_window, event);
        }

        switch (event.type)
        {
        case sf::Event::Closed: this->Close(); return;

        case sf::Event::Resized:
        {
            FVector2u oldSize = m_dimension;
            m_dimension = FVector2u(event.size.width, event.size.height);
            this->Emit(Events::Resized{ oldSize, m_dimension });

            // Update the SFML view to match the new window size
            m_view.SetSize(
                static_cast<float>(m_dimension.x),
                static_cast<float>(m_dimension.y)
            );
            m_window->setView(Utils::Convert(m_view));

            if (!VR::FVRSystem::GetInstance().IsInitialized())
            {
                // Default camera
                FCamera defaultCamera;

                glViewport(0, 0, event.size.width, event.size.height);
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                gluPerspective(
                    defaultCamera.fov,
                    (float)event.size.width / event.size.height,
                    defaultCamera.nearPlane,
                    defaultCamera.farPlane
                );
                glMatrixMode(GL_MODELVIEW);
            }

            break;
        }

        case sf::Event::LostFocus  : this->Emit(Events::LostFocus{}); break;

        case sf::Event::GainedFocus: this->Emit(Events::GainedFocus{}); break;

        default                    : break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Window::Draw(const std::function<void(void)>& drawFunction)
{
    // Check if the window is open
    if (!IsOpen() || !drawFunction) { return; }

    // Call the provided drawing function
    drawFunction();

    // Render ImGui if it was initialized
    // NOTE: Skip ImGui rendering when rendering to VR framebuffers
    // ImGui should only be rendered to the main window, not to VR eye buffers
    if (m_imguiInitialized)
    {
        // Check if we're rendering to a framebuffer (VR mode)
        GLint currentFramebuffer = 0;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFramebuffer);

        // Only render ImGui when rendering to the default framebuffer (window)
        if (currentFramebuffer == 0)
        {
            // Save OpenGL State
            glPushAttrib(GL_ALL_ATTRIB_BITS);
            glPushMatrix();

            ImGui::SFML::Render(*m_window);

            // Restore OpenGL State
            glPopMatrix();
            glPopAttrib();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
bool Window::SetActive(bool active)
{
    // Check if the window is open
    if (!IsOpen()) { return false; }

    // Set the OpenGL context as active or inactive for the current thread
    return m_window->setActive(active);
}

///////////////////////////////////////////////////////////////////////////////
bool Window::IsKeyPressed(EKeyboardKeys key) const
{
    // Check if the window is open
    if (!IsOpen()) { return false; }

    // Convert EKeyboardKeys to sf::Keyboard::Key
    sf::Keyboard::Key sfmlKey = Utils::Convert(key);

    // Check if the specified key is currently pressed
    return sf::Keyboard::isKeyPressed(sfmlKey);
}

///////////////////////////////////////////////////////////////////////////////
bool Window::IsMouseButtonPressed(EMouseButtons button) const
{
    // Check if the window is open
    if (!IsOpen()) { return false; }

    // Convert EMouseButtons to sf::Mouse::Button
    sf::Mouse::Button sfmlButton = Utils::Convert(button);

    // Check if the specified mouse button is currently pressed
    return sf::Mouse::isButtonPressed(sfmlButton);
}

///////////////////////////////////////////////////////////////////////////////
bool Window::IsGamepadConnected(UInt32 gamepadIndex) const
{
    // Check if the window is open
    if (!IsOpen()) { return false; }

    // Check if the specified gamepad is connected
    return sf::Joystick::isConnected(gamepadIndex);
}

///////////////////////////////////////////////////////////////////////////////
bool Window::IsGamepadButtonPressed(
    EGamepadButtons button, UInt32 gamepadIndex
) const
{
    // Check if the window is open
    if (!IsOpen()) { return 0.0f; }

    // Check if the specified gamepad is connected
    if (!IsGamepadConnected(gamepadIndex)) { return false; }

    // Convert EGamepadButtons to sf::Joystick::Button
    UInt32 sfmlButton = Utils::Convert(button);

    if (button >= EGamepadButtons::DPadUp &&
        button <= EGamepadButtons::DPadRight)
    {
        // For DPad buttons, check the axis position instead of button press
        float axisPosition = 0.0f;
        switch (button)
        {
        case EGamepadButtons::DPadUp:
            axisPosition = sf::Joystick::getAxisPosition(
                gamepadIndex, sf::Joystick::PovY
            );
            break;
        case EGamepadButtons::DPadDown:
            axisPosition = -sf::Joystick::getAxisPosition(
                gamepadIndex, sf::Joystick::PovY
            );
            break;
        case EGamepadButtons::DPadLeft:
            axisPosition = -sf::Joystick::getAxisPosition(
                gamepadIndex, sf::Joystick::PovX
            );
            break;
        case EGamepadButtons::DPadRight:
            axisPosition = sf::Joystick::getAxisPosition(
                gamepadIndex, sf::Joystick::PovX
            );
            break;
        default: break;
        }
        return axisPosition > DPAD_DEADZONE;
    }

    // Check if the specified button on the specified gamepad is currently
    // pressed
    return sf::Joystick::isButtonPressed(gamepadIndex, sfmlButton);
}

///////////////////////////////////////////////////////////////////////////////
float Window::GetGamepadAxis(EGamepadAxes axis, UInt32 gamepadIndex) const
{
    // Check if the window is open
    if (!IsOpen()) { return 0.0f; }

    // Check if the specified gamepad is connected
    if (!IsGamepadConnected(gamepadIndex)) { return 0.0f; }

    // Convert EGamepadAxes to sf::Joystick::Axis
    sf::Joystick::Axis sfmlAxis = Utils::Convert(axis);

    return sf::Joystick::getAxisPosition(gamepadIndex, sfmlAxis);
}

///////////////////////////////////////////////////////////////////////////////
FVector2i Window::GetMousePosition(void) const
{
    // Check if the window is open
    if (!IsOpen()) { return FVector2i::Zero; }

    // Get the current mouse position relative to the window
    return Utils::Convert(sf::Mouse::getPosition(*m_window));
}

///////////////////////////////////////////////////////////////////////////////
void Window::SetMousePosition(const FVector2i& position)
{
    // Check if the window is open
    if (!IsOpen()) { return; }

    // Set the mouse position relative to the window
    sf::Mouse::setPosition(Utils::Convert(position), *m_window);
}

///////////////////////////////////////////////////////////////////////////////
FView Window::GetDefaultView(void) const
{
    FVector2u size = this->GetDimensions();
    FView defaultView(FRectangle(
        -static_cast<Float32>(size.x) / 2.0f,
        -static_cast<Float32>(size.y) / 2.0f,
        static_cast<Float32>(size.x),
        static_cast<Float32>(size.y)
    ));
    return defaultView;
}

///////////////////////////////////////////////////////////////////////////////
const FView& Window::GetCurrentView(void) const { return m_view; }

///////////////////////////////////////////////////////////////////////////////
void Window::SetCurrentView(const FView& view)
{
    m_view = view;
    if (IsOpen()) { m_window->setView(Utils::Convert(m_view)); }
}

#endif

}   // namespace tkd::SFML
