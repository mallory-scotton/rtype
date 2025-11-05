///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Config.hpp>
#include <Engine/Renderer/Interfaces/IWindow.hpp>
#include <memory>
#if TKD_ENGINE_CLIENT
    #include <SFML/Graphics.hpp>
    #include <SFML/Window.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::SFML
///////////////////////////////////////////////////////////////////////////////
namespace tkd::SFML
{

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
class Window : public IWindow
{
private:
    ///////////////////////////////////////////////////////////////////////////
    // Class Member
    ///////////////////////////////////////////////////////////////////////////
    static const float DPAD_DEADZONE;      //<! Deadzone for D-Pad axes
    static const float DEADZONE;           //<! Deadzone for analog sticks
    static const float TRIGGER_DEADZONE;   //<! Deadzone for triggers

private:
    ///////////////////////////////////////////////////////////////////////////
    // Class Member
    ///////////////////////////////////////////////////////////////////////////
    std::unique_ptr<sf::RenderWindow> m_window;   //<! The SFML window instance
    sf::Clock m_clock;                            //<! Clock for timing
    EWindowState m_state;                         //<! The current window state
    FVector2i m_position;      //<! The current window position
    FVector2u m_dimension;     //<! The current window dimensions
    FString m_title;           //<! The current window title
    bool m_vsync;              //<! Whether VSync is enabled
    bool m_imguiInitialized;   //<! Whether ImGui-SFML has been initialized
    FView m_view;              //<! The default view of the window

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Parameterized constructor
    ///
    /// \param title The title of the window
    /// \param openDefault Whether to open the window upon construction
    /// \param position The initial position of the window
    /// \param dimension The initial dimensions of the window
    /// \param state The initial state of the window
    ///
    ///////////////////////////////////////////////////////////////////////////
    Window(
        const FString& title,
        bool openDefault = true,
        const FVector2i& position = FVector2i::Zero,
        const FVector2u& dimension = FVector2u(1280, 720),
        EWindowState state = EWindowState::Windowed
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Destructor - ensures proper cleanup
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual ~Window();

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Convert window dimensions to SFML VideoMode
    ///
    /// \param dimension The dimensions to convert
    ///
    /// \return The corresponding SFML VideoMode
    ///
    ///////////////////////////////////////////////////////////////////////////
    static sf::VideoMode ToSFMLVideoMode(const FVector2u& dimension);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Convert window state to SFML style flags
    ///
    /// \param state The window state to convert
    ///
    /// \return The corresponding SFML style flags
    ///
    ///////////////////////////////////////////////////////////////////////////
    static sf::Uint32 ToSFMLStyle(const EWindowState& state);

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Open the window with specified parameters
    ///
    /// \return True if the window was successfully opened, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual bool Open(void) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Close the window
    ///
    /// \return True if the window was successfully closed, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual bool Close(void) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Check if the window is currently open
    ///
    /// \return True if the window is open, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    TKD_NODISCARD virtual bool IsOpen(void) const override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the frames per second (FPS) limit for the window
    ///
    /// \param fps The new FPS value to set
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void SetFPSLimit(UInt32 fps) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the window state
    ///
    /// \param state The new state to set
    ///
    /// \return True if the state was successfully set, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual bool SetState(const EWindowState& state) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the current window state
    ///
    /// \return A constant reference to the current window state
    ///
    ///////////////////////////////////////////////////////////////////////////
    TKD_NODISCARD virtual const EWindowState& GetState(void) const override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the window position
    ///
    /// \param position The new position to set for the window
    ///
    /// \return True if the position was successfully set, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual bool SetPosition(const FVector2i& position) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the current window position
    ///
    /// \return A constant reference to the current window position
    ///
    ///////////////////////////////////////////////////////////////////////////
    TKD_NODISCARD virtual const FVector2i& GetPosition(void) const override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the window dimensions
    ///
    /// \param dimension The new dimensions to set for the window
    ///
    /// \return True if the dimensions were successfully set, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual bool SetDimension(const FVector2u& dimension) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the current window dimensions
    ///
    /// \return A constant reference to the current window dimensions
    ///
    ///////////////////////////////////////////////////////////////////////////
    TKD_NODISCARD virtual const FVector2u& GetDimensions(void) const override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the debug mode for the window
    ///
    /// \param debugMode Whether to enable debug mode features
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void SetDebugMode(bool debugMode) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the window title
    ///
    /// \param title The new title to set for the window
    ///
    /// \return True if the title was successfully set, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual bool SetTitle(const FString& title) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the current window title
    ///
    /// \return A constant reference to the current window title
    ///
    ///////////////////////////////////////////////////////////////////////////
    TKD_NODISCARD virtual const FString& GetTitle(void) const override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the vertical synchronization (VSync) state
    ///
    /// \param enabled True to enable VSync, false to disable it
    ///
    /// \return True if VSync state was successfully set, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual bool SetVSync(bool enabled) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Check if vertical synchronization (VSync) is enabled
    ///
    /// \return True if VSync is enabled, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    TKD_NODISCARD virtual bool IsVSync(void) const override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the native window handle
    ///
    /// \return A pointer to the native window handle (platform-specific)
    ///
    ///////////////////////////////////////////////////////////////////////////
    TKD_NODISCARD virtual void* GetNativeHandle(void) const override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Update the window (process events, etc.)
    ///
    /// \param deltaTime Time elapsed since the last update (in seconds)
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void Update(float deltaTime) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Draw using a provided function
    ///
    /// \param drawFunction The drawing function to call
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void Draw(const std::function<void(void)>& drawFunction) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the OpenGL context as active or inactive for the current
    /// thread
    ///
    /// \param active True to make the context active, false to make it
    /// inactive
    ///
    /// \return True if the operation was successful, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual bool SetActive(bool active = true) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Check if a specific keyboard key is currently pressed
    ///
    /// \param key The keyboard key to check
    ///
    /// \return True if the key is pressed, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual bool IsKeyPressed(EKeyboardKeys key) const override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Check if a specific mouse button is currently pressed
    ///
    /// \param button The mouse button to check
    ///
    /// \return True if the button is pressed, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual bool IsMouseButtonPressed(EMouseButtons button) const override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Check if a gamepad is connected
    ///
    /// \param gamepadIndex The index of the gamepad to check (default is 0)
    ///
    /// \return True if the gamepad is connected, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual bool IsGamepadConnected(UInt32 gamepadIndex = 0) const override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Check if a specific gamepad button is currently pressed
    ///
    /// \param button The gamepad button to check
    /// \param gamepadIndex The index of the gamepad to check (default is 0)
    ///
    /// \return True if the button is pressed, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual bool IsGamepadButtonPressed(
        EGamepadButtons button, UInt32 gamepadIndex = 0
    ) const override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the current position of a specific gamepad axis
    ///
    /// \param axis The gamepad axis to check
    /// \param gamepadIndex The index of the gamepad to check (default is 0)
    ///
    /// \return The current position of the axis in the range [-1.0, 1.0]
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual float GetGamepadAxis(EGamepadAxes axis, UInt32 gamepadIndex = 0)
        const override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the current mouse position relative to the window
    ///
    /// \return The current mouse position as a 2D vector
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual FVector2i GetMousePosition(void) const override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the mouse position relative to the window
    ///
    /// \param position The new mouse position to set
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void SetMousePosition(const FVector2i& position) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the default view of the window
    ///
    /// \return A constant reference to the default view
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual FView GetDefaultView(void) const override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the current view of the window
    ///
    /// \return A constant reference to the current view
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual const FView& GetCurrentView(void) const override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the current view of the window
    ///
    /// \param view The new view to set
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void SetCurrentView(const FView& view) override;

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Initialize OpenGL settings for the window
    ///
    ///////////////////////////////////////////////////////////////////////////
    void InitializeOpenGL(void);
};

}   // namespace tkd::SFML
#endif
