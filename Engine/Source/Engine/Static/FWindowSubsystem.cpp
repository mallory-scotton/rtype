///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Static/FWindowSubsystem.hpp>
#include <Engine/Assets/URessource.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::__internal
///////////////////////////////////////////////////////////////////////////////
namespace tkd::__internal
{

#if TKD_ENGINE_CLIENT

///////////////////////////////////////////////////////////////////////////////
FWindowSubsystem::FWindowSubsystem(const FEngineSettings& settings)
    : m_settings(settings)
{}

///////////////////////////////////////////////////////////////////////////////
bool FWindowSubsystem::Initialize(void)
{
    if (m_initialized.load(std::memory_order_acquire)) { return false; }

    try
    {
        // Create graphics factory
        m_graphicsFactory = std::make_unique<SFML::GraphicsFactory>();

        // Setup the factory for the ressource manager
        URessource::GetInstance().SetGraphicsFactory(m_graphicsFactory.get());

        // Create window
        m_window = m_graphicsFactory->CreateWindow(
            m_settings.game.title,
            true,
            FVector2i::Zero,
            FVector2u(
                m_settings.window.windowWidth, m_settings.window.windowHeight
            ),
            m_settings.window.isFullscreen ? EWindowState::Fullscreen
                                           : EWindowState::Windowed
        );

        if (!m_window || !m_window->IsOpen()) { return false; }

        // Create renderer
        m_renderer = m_graphicsFactory->CreateRenderer(m_window.get());
        if (!m_renderer)
        {
            m_window->Close();
            return false;
        }

        // Initialize input manager
        m_inputManager = std::make_unique<FInputManager>();
        m_inputManager->Initialize(m_settings);

        // Deactivate the OpenGL context in the main thread
        // This is crucial for multi-threaded rendering with SFML/OpenGL
        // The context will be activated in the rendering thread
        m_window->SetActive(false);

        m_initialized.store(true, std::memory_order_release);
        return true;
    }
    catch (const std::exception& e)
    {
        return false;
    }
}

///////////////////////////////////////////////////////////////////////////////
void FWindowSubsystem::SetRenderCallback(RenderCallback callback)
{
    std::lock_guard lock(m_dataMutex);
    m_renderCallback = std::move(callback);
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD bool FWindowSubsystem::IsOpen(void) const noexcept
{
    std::shared_lock lock(m_windowMutex);
    return m_window && m_window->IsOpen();
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD FVector2u FWindowSubsystem::GetDimensions(void) const
{
    std::shared_lock lock(m_windowMutex);
    if (m_window) { return m_window->GetDimensions(); }
    return FVector2u::Zero;
}

///////////////////////////////////////////////////////////////////////////////
void FWindowSubsystem::SetTitle(const FString& title)
{
    std::unique_lock lock(m_windowMutex);
    if (m_window) { m_window->SetTitle(title); }
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD FInputManager* FWindowSubsystem::GetInputManager(void) noexcept
{
    return m_inputManager.get();
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD float FWindowSubsystem::GetFPS(void) const noexcept
{
    return m_currentFPS.load(std::memory_order_acquire);
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD float FWindowSubsystem::GetAverageFrameTime(void) const noexcept
{
    return m_averageFrameTime.load(std::memory_order_acquire);
}

///////////////////////////////////////////////////////////////////////////////
IWindow* FWindowSubsystem::GetWindow(void) const noexcept
{
    std::shared_lock lock(m_windowMutex);
    return m_window.get();
}

///////////////////////////////////////////////////////////////////////////////
void FWindowSubsystem::ThreadLoop(void)
{
    // Activate the OpenGL context for this thread
    // This is crucial for multi-threaded rendering with SFML/OpenGL
    if (m_window) { m_window->SetActive(true); }

    // VR Initialization
    if (m_settings.vr.capability != EVRCapability::Disabled)
    {
        // Check for VR initialization
        VR::FVRSystem& vrSystem = VR::FVRSystem::GetInstance();

        // Try to initialize VR system
        if (!vrSystem.Initialize())
        {
            FLogger::SetNamespace("Virtual Reality");
            FLogger::Warn("VR System failed to initialize.");

            if (m_settings.vr.capability == EVRCapability::Required)
            {
                FLogger::Error("VR is required but failed to initialize.");
                RequestShutdown();
            }
            else
            {
                FLogger::Info(
                    "VR is not required. Continuing without VR support."
                );
            }
        }
    }

    TimePoint lastTime = SteadyClock::now();
    TimePoint fpsUpdateTime = lastTime;
    int frameCount = 0;

    while (m_running.load(std::memory_order_acquire) && m_window->IsOpen())
    {
        TimePoint frameStart = SteadyClock::now();
        float deltaTime = TDuration<float>(frameStart - lastTime).count();
        lastTime = frameStart;

        // Process window events and input
        {
            std::unique_lock lock(m_windowMutex);
            m_window->Update(deltaTime);

            if (m_inputManager) { m_inputManager->Update(m_window.get()); }
        }

        // Render frame
        {
            std::shared_lock lock(m_windowMutex);

            // Begin rendering
            m_renderer->BeginFrame();

            // Execute render callback
            m_window->Draw(
                [this]()
                {
                    std::shared_lock callbackLock(m_dataMutex);
                    if (m_renderCallback && m_renderer)
                    {
                        m_renderCallback(*m_renderer);
                    }
                }
            );

            // End rendering
            m_renderer->EndFrame();
        }

        // Update performance metrics
        frameCount++;
        float elapsed = TDuration<float>(frameStart - fpsUpdateTime).count();
        if (elapsed >= 1.0f)
        {
            m_currentFPS.store(
                static_cast<float>(frameCount) / elapsed,
                std::memory_order_release
            );
            frameCount = 0;
            fpsUpdateTime = frameStart;
        }

        // Track frame time
        float frameTime =
            TDuration<float>(SteadyClock::now() - frameStart).count() *
            1000.0f;
        m_averageFrameTime.store(
            m_averageFrameTime.load(std::memory_order_acquire) * 0.95f +
                frameTime * 0.05f,
            std::memory_order_release
        );
    }

    // Cleanup
    {
        std::unique_lock lock(m_windowMutex);
        if (m_window && m_window->IsOpen()) { m_window->Close(); }
        m_renderer.reset();
        m_window.reset();
    }
}

#endif

}   // namespace tkd::__internal
