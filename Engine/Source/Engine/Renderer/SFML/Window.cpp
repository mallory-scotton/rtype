///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Renderer/SFML/Window.hpp>
#include <Engine/Static/Engine.hpp>
#if TKD_ENGINE_CLIENT
    #include <imgui-SFML.h>
    #include <imgui.h>
#endif

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::SFML
///////////////////////////////////////////////////////////////////////////////
namespace tkd::SFML
{

#if TKD_ENGINE_CLIENT

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
bool Window::Open(void)
{
    // Check if the window is already open
    if (IsOpen()) { return false; }

    // Create the SFML window
    m_window = std::make_unique<sf::RenderWindow>(
        ToSFMLVideoMode(m_dimension), m_title.CStr(), ToSFMLStyle(m_state)
    );

    // Initialize ImGui-SFML if in debug build
    if (Engine::IsDebugBuild())
    {
        if (!ImGui::SFML::Init(*m_window))
        {
            m_window->close();
            return false;
        }
        m_imguiInitialized = true;
    }

    // Check if the window was created successfully
    if (!m_window || !m_window->isOpen()) { return false; }

    // Set initial position and VSync
    m_window->setPosition(sf::Vector2i(m_position.x, m_position.y));
    m_window->setVerticalSyncEnabled(m_vsync);

    // Emit the Opened event
    this->Emit(Events::Opened{ m_position, m_dimension, m_state });

    // Successfully opened the window
    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool Window::Close(void)
{
    // Check if the window is open
    if (!IsOpen()) { return false; }

    // Shutdown ImGui-SFML if it was initialized
    if (m_imguiInitialized)
    {
        ImGui::SFML::Shutdown();
        m_imguiInitialized = false;
    }

    // Close the SFML window
    m_window->close();
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

    // Shutdown ImGui before recreating window
    if (m_imguiInitialized)
    {
        ImGui::SFML::Shutdown();
        m_imguiInitialized = false;
    }

    m_window->create(
        ToSFMLVideoMode(m_dimension), m_title.CStr(), ToSFMLStyle(m_state)
    );
    m_window->setPosition(currentPosition);
    m_window->setVerticalSyncEnabled(m_vsync);

    // Reinitialize ImGui if in debug build
    if (Engine::IsDebugBuild())
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
const FVector2u& Window::GetDimension(void) const { return m_dimension; }

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

    // Check for window move events
    sf::Vector2i currentPosition = m_window->getPosition();
    if (m_position.x != currentPosition.x || m_position.y != currentPosition.y)
    {
        FVector2i oldPosition = m_position;
        m_position = FVector2i(currentPosition.x, currentPosition.y);
        this->Emit(Events::Moved{ oldPosition, m_position });
    }

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

    // Clear the window with a black color (or any other desired color)
    m_window->clear(sf::Color::Black);

    // Call the provided drawing function
    drawFunction();

    // Render ImGui if it was initialized
    if (m_imguiInitialized) { ImGui::SFML::Render(*m_window); }

    // Display the contents of the window
    m_window->display();
}

#endif

}   // namespace tkd::SFML
