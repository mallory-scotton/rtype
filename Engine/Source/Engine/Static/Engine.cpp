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
std::atomic<bool> Engine::s_isRunning = false;
FString Engine::s_exitMessage = "";
Engine::UThread Engine::s_mainThread;
Engine::UThread Engine::s_networkThread;
#if TKD_ENGINE_CLIENT
Engine::UThread Engine::s_renderThread;
#endif

///////////////////////////////////////////////////////////////////////////////
void Engine::MainThreadFunction(void)
{
    // TODO: Implement main thread logic here
}

///////////////////////////////////////////////////////////////////////////////
void Engine::NetworkThreadFunction(void)
{
    // TODO: Implement network thread logic here
}

///////////////////////////////////////////////////////////////////////////////
#if TKD_ENGINE_CLIENT
void Engine::RenderThreadFunction(void)
{
    std::unique_ptr<IWindow> window = std::make_unique<SFML::Window>("TKD");

    while (s_isRunning && window->IsOpen())
    {
        window->Update(0.0f);
        // TODO: Add rendering logic here
    }

    s_isRunning = false;
}
#endif

///////////////////////////////////////////////////////////////////////////////
bool Engine::Initialize(int argc, char* argv[])
{
    if (s_isInitialized) { return false; }

    FArgs& args = FArgs::GetInstance();

    bool a_verbose = false;
    bool a_debug = false;

    args.AddFlags("verbose", "Enable verbose logging", a_verbose, false);
    args.AddFlags("debug", "Enable debug mode", a_debug, false);

#if TKD_ENGINE_SERVER
    std::string a_host = "localhost";
    UInt16 a_port = 8080;

    args.AddFlags("host", "Server hostname or IP address", a_host, false);
    args.AddFlags("port", "Server port number", a_port, false);
#endif

    if (!args.Process(argc, argv))
    {
        if (args.GetExitCode() != 0)
        {
            s_exitCode = args.GetExitCode();
            s_exitMessage = "Failed to process command-line arguments.";
        }
        return false;
    }

    // TODO: Initialization logic here (e.g., setting up subsystems, loading
    // resources)

    // Create the threads but do not start them yet
    s_mainThread =
        std::make_unique<FThread>(std::bind(&Engine::MainThreadFunction));
    s_networkThread =
        std::make_unique<FThread>(std::bind(&Engine::NetworkThreadFunction));
    TKD_ENGINE_IF_CLIENT({
        s_renderThread =
            std::make_unique<FThread>(std::bind(&Engine::RenderThreadFunction)
            );
    })

    s_isInitialized = true;
    return s_isInitialized;
}

///////////////////////////////////////////////////////////////////////////////
bool Engine::Shutdown(void)
{
    if (!s_isInitialized) { return false; }

    // TODO: Shutdown logic here (e.g., releasing resources, shutting down
    // subsystems)

    // Shutdown threads if they are running
    if (s_mainThread && s_mainThread->running) { s_mainThread->Join(); }
    if (s_networkThread && s_networkThread->running)
    {
        s_networkThread->Join();
    }
    TKD_ENGINE_IF_CLIENT({
        if (s_renderThread && s_renderThread->running)
        {
            s_renderThread->Join();
        }
    })

    s_isInitialized = false;
    return !s_isInitialized;
}

///////////////////////////////////////////////////////////////////////////////
bool Engine::IsInitialized(void) { return s_isInitialized; }

///////////////////////////////////////////////////////////////////////////////
void Engine::Run(void)
{
    if (!s_isInitialized || s_isRunning) { return; }
    // if (World::Get() == nullptr)
    //{
    //     s_exitCode = TKD_EXIT_FAILURE;
    //     s_exitMessage = "No world loaded. Cannot run the engine.";
    //     return;
    // }

    s_isRunning = true;

    // Start the threads
    s_mainThread->Start();
    s_networkThread->Start();
    TKD_ENGINE_IF_CLIENT({ s_renderThread->Start(); })

    while (s_isRunning)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    s_isRunning = false;
}

///////////////////////////////////////////////////////////////////////////////
int Engine::GetExitCode(void) { return s_exitCode; }

///////////////////////////////////////////////////////////////////////////////
void Engine::PrintExitMessage(void)
{
    std::ostream& out =
        (s_exitCode == TKD_EXIT_SUCCESS) ? std::cout : std::cerr;

    if (!s_exitMessage.IsEmpty()) { out << s_exitMessage << std::endl; }
}

///////////////////////////////////////////////////////////////////////////////
void Engine::SetExitCode(int code) { s_exitCode = code; }

///////////////////////////////////////////////////////////////////////////////
void Engine::SetExitMessage(const FString& message)
{
    s_exitMessage = message;
}

}   // namespace tkd::__internal
