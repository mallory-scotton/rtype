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
    , m_vrInitialized(false)
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

        // VR Initialization
        if (m_settings.vr.capability != EVRCapability::Disabled)
        {
            // Check for VR initialization
            VR::FVRSystem& vrSystem = VR::FVRSystem::GetInstance();

            FLogger::SetNamespace("Virtual Reality");
            FLogger::Info("Initializing VR System...");

            // Try to initialize VR system
            if (!vrSystem.Initialize())
            {
                FLogger::SetNamespace("Virtual Reality");
                FLogger::Warn("VR System failed to initialize.");

                if (m_settings.vr.capability == EVRCapability::Required)
                {
                    FLogger::Error("VR is required but failed to initialize.");
                    return false;
                }
                else
                {
                    FLogger::Info(
                        "VR is not required. Continuing without VR support."
                    );
                }
            }
            else
            {
                m_vrInitialized = true;
                FLogger::SetNamespace("Virtual Reality");
                FLogger::Info("VR System initialized successfully.");
            }
        }

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

        // Set V-Sync
        m_window->SetVSync(m_settings.window.enableVSync);
        if (!m_settings.window.enableVSync)
        {
            m_window->SetFPSLimit(m_settings.window.targetFPS);
        }

        // Set active debug mode
        if (m_settings.debug) { m_window->SetDebugMode(true); }

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

    #ifndef TKD_SYSTEM_WINDOWS
        // Deactivate the OpenGL context in the main thread
        // This is crucial for multi-threaded rendering with SFML/OpenGL
        // The context will be activated in the rendering thread
        m_window->SetActive(false);
    #endif

        m_initialized.store(true, std::memory_order_release);
        return true;
    }
    catch (...)
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
void FWindowSubsystem::ThreadSetup(void)
{
    // Activate the OpenGL context for this thread
    // This is crucial for multi-threaded rendering with SFML/OpenGL
    if (m_window) { m_window->SetActive(true); }
}

///////////////////////////////////////////////////////////////////////////////
void FWindowSubsystem::ThreadTeardown(void)
{
    std::unique_lock lock(m_windowMutex);
    if (m_window && m_window->IsOpen()) { m_window->Close(); }
    m_renderer.reset();
    m_window.reset();

    // VR Teardown
    if (m_vrInitialized)
    {
        VR::FVRSystem::GetInstance().Shutdown();
        m_vrInitialized = false;
    }
}

///////////////////////////////////////////////////////////////////////////////
void FWindowSubsystem::ThreadLoop(void)
{
    static TimePoint lastTime = SteadyClock::now();
    static TimePoint fpsUpdateTime = lastTime;
    static int frameCount = 0;

    if (!m_window->IsOpen())
    {
        RequestShutdown();
        return;
    }

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

        if (m_renderer->IsUsingVirtualReality())
        {
            // Execute render callback for left eye
            m_renderer->SetupVirtualRealityLeftEye();
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
            m_renderer->ResolveVirtualRealityLeftEye();

            // Execute render callback for right eye
            m_renderer->SetupVirtualRealityRightEye();
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
            m_renderer->ResolveVirtualRealityRightEye();
        }
        else
        {
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
        }

        // End rendering
        m_renderer->EndFrame();
    }

    // Update performance metrics
    frameCount++;
    float elapsed = TDuration<float>(frameStart - fpsUpdateTime).count();
    if (elapsed >= 1.0f)
    {
        m_currentFPS.store(
            static_cast<float>(frameCount) / elapsed, std::memory_order_release
        );
        frameCount = 0;
        fpsUpdateTime = frameStart;
    }

    #ifndef TKD_SYSTEM_WINDOWS
    // Track frame time
    float frameTime =
        TDuration<float>(SteadyClock::now() - frameStart).count() * 1000.0f;
    m_averageFrameTime.store(
        m_averageFrameTime.load(std::memory_order_acquire) * 0.95f +
            frameTime * 0.05f,
        std::memory_order_release
    );

    // Yield CPU if frame was very fast to prevent 100% CPU usage
    // This is especially important when VSync is disabled
    if (frameTime < 1.0f) { std::this_thread::yield(); }
    #endif
}

///////////////////////////////////////////////////////////////////////////////
IRenderer* FWindowSubsystem::GetRenderer(void) const noexcept
{
    try
    {
        std::shared_lock lock(m_windowMutex);
        return m_renderer.get();
    }
    catch (const std::exception&)
    {
        return nullptr;
    }
}

///////////////////////////////////////////////////////////////////////////////
IGraphicsFactory* FWindowSubsystem::GetGraphicsFactory(void) const noexcept
{
    try
    {
        std::shared_lock lock(m_windowMutex);
        return m_graphicsFactory.get();
    }
    catch (const std::exception&)
    {
        return nullptr;
    }
}

///////////////////////////////////////////////////////////////////////////////
FCamera& FWindowSubsystem::GetCamera(void) const noexcept
{
    try
    {
        std::shared_lock lock(m_windowMutex);
        if (m_renderer) { return m_renderer->GetCamera(); }
        static FCamera defaultCamera;
        return defaultCamera;
    }
    catch (const std::exception&)
    {
        static FCamera defaultCamera;
        return defaultCamera;
    }
}

///////////////////////////////////////////////////////////////////////////////
void FWindowSubsystem::Shutdown(void)
{
    FThreadedSubsystem::Shutdown();
    {
        std::unique_lock lock(m_windowMutex);
        if (m_window && m_window->IsOpen()) { m_window->Close(); }
        // std::this_thread::sleep_for(std::chrono::milliseconds(300));

        m_renderer.reset();
        m_window.reset();
    }
}

#endif

}   // namespace tkd::__internal
