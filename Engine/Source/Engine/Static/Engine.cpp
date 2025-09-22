///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Static/Engine.hpp>
#include <Engine/Renderer.hpp>

///////////////////////////////////////////////////////////////////////////////
// Weak declaration of game functions to be defined in the game
///////////////////////////////////////////////////////////////////////////////
TKD_EXPORT_WEAK

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
#if TKD_ENGINE_CLIENT
Engine::UThread Engine::s_renderThread;
#endif

///////////////////////////////////////////////////////////////////////////////
void Engine::PrintStartupMessage(void)
{
    std::string gameName = "NOT_LOADED";
    if (TKD_GetGameName)
    {
        gameName = TKD_GetGameName();
        std::transform(
            gameName.begin(),
            gameName.end(),
            gameName.begin(),
            [](unsigned char c) { return std::toupper(c); }
        );
        size_t pos = gameName.find(' ');
        while (pos != std::string::npos)
        {
            gameName.replace(pos, 1, "_");
            pos = gameName.find(' ', pos + 1);
        }
        gameName = "\"" + gameName + "\"";
    }

    std::cout << "\n████████╗██╗  ██╗██████╗\n"
              << "╚══██╔══╝██║ ██╔╝██╔══██╗"
              << " Initializing TKD Core Systems...\n"
              << "   ██║   █████╔╝ ██║  ██║\n"
              << "   ██║   ██╔═██╗ ██║  ██║"
              << " Welcome, Developer.\n"
              << "   ██║   ██║  ██╗██████╔╝\n"
              << "   ╚═╝   ╚═╝  ╚═╝╚═════╝\n"
              << std::endl;

    std::cout << "+" << std::string(67, '-') << "+" << std::endl;
    std::cout << "| " << std::setw(65) << std::left
              << " Engine Version : " TKD_VERSION_STRING << " |" << std::endl;
    std::cout << "| " << std::setw(65) << std::left
              << " Game Module    : " + gameName << " |" << std::endl;
    std::cout << "| " << std::setw(65) << std::left
              << " Lead Authors   : " TKD_ENGINE_GROUP << " |" << std::endl;
    std::cout << "+" << std::string(67, '-') << "+" << std::endl;
    std::cout << "| " << std::setw(65) << std::left
              << " Build Date     : " TKD_BUILD_DATE << " |" << std::endl;
    std::cout << "| " << std::setw(65) << std::left
              << " Build Type     : " TKD_ENGINE_TYPE << " |" << std::endl;
    std::cout << "| " << std::setw(65) << std::left
              << " Website        : " TKD_ENGINE_WEBSITE << " |" << std::endl;
    std::cout << "| " << std::setw(65) << std::left
              << " System         : " TKD_SYSTEM_NAME << " |" << std::endl;
    std::cout << "+" << std::string(67, '-') << "+" << std::endl;
    std::cout << ">>> Type 'help' for a list of available console commands."
              << std::endl;
}

///////////////////////////////////////////////////////////////////////////////
void Engine::MainThreadFunction(void)
{
    // TODO: Implement main thread logic here
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

    PrintStartupMessage();

#if TKD_ENGINE_SERVER
    if (!Network::Initialize(a_port))
    {
        s_exitCode = TKD_EXIT_FAILURE;
        s_exitMessage = "Failed to initialize the network subsystem.";
        return false;
    }
#endif

    // Create the threads but do not start them yet
    s_mainThread =
        std::make_unique<FThread>(std::bind(&Engine::MainThreadFunction));
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

#if TKD_ENGINE_SERVER
    Network::Shutdown();
#endif

    // Shutdown threads if they are running
    if (s_mainThread && s_mainThread->running) { s_mainThread->Join(); }
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
