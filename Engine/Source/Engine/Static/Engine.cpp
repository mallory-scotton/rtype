///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Static/Engine.hpp>
#include <Engine/Renderer.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::__internal
///////////////////////////////////////////////////////////////////////////////
namespace tkd::__internal
{

///////////////////////////////////////////////////////////////////////////////
bool Engine::s_isInitialized = false;
int Engine::s_exitCode = TKD_EXIT_SUCCESS;
bool Engine::s_isRunning = false;

///////////////////////////////////////////////////////////////////////////////
bool Engine::Initialize(int argc, char* argv[])
{
    if (s_isInitialized) { return false; }

    (void)argc;
    (void)argv;

    // TODO: Initialization logic here (e.g., setting up subsystems, loading
    // resources)

    s_isInitialized = true;
    return s_isInitialized;
}

///////////////////////////////////////////////////////////////////////////////
bool Engine::Shutdown(void)
{
    if (!s_isInitialized) { return false; }

    // TODO: Shutdown logic here (e.g., releasing resources, shutting down
    // subsystems)

    s_isInitialized = false;
    return !s_isInitialized;
}

///////////////////////////////////////////////////////////////////////////////
bool Engine::IsInitialized(void) { return s_isInitialized; }

///////////////////////////////////////////////////////////////////////////////
void Engine::Run(void)
{
    if (!s_isInitialized || s_isRunning) { return; }

    s_isRunning = true;
    // TODO: Main loop logic here (e.g., processing events, updating state,
    // rendering)

    // TEMPORARY: Simple window creation and event loop using SFML
    std::unique_ptr<tkd::IWindow> window =
        std::make_unique<tkd::SFML::Window>("R-Type", true);

    while (window->IsOpen())
    {
        window->Update(0.0f);

        window->Draw(
            []()
            {
                // Drawing logic here (e.g., rendering game objects)
            }
        );
    }

    s_isRunning = false;
}

///////////////////////////////////////////////////////////////////////////////
int Engine::GetExitCode(void) { return s_exitCode; }

}   // namespace tkd::__internal
