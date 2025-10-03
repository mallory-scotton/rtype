///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Static/Engine.hpp>
#include <Engine/Debug.hpp>
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
bool Engine::s_isDebugBuild = true;
FEngineSettings Engine::Settings = FEngineSettings();
UWorld Engine::World = UWorld();
FInputManager Engine::Inputs = FInputManager();

///////////////////////////////////////////////////////////////////////////////
void Engine::PrintStartupMessage(void)
{
    std::string gameName = "NOT_LOADED";
    if (TKD_GetEngineSettings)
    {
        gameName = Settings.game.title.empty() ? "NOT_SPECIFIED"
                                               : Settings.game.title;
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
    SteadyClock clock;
    TimePoint lastTime = clock.now();
    Float32 deltaTime = 0.0f;

    auto actor = Engine::World.SpawnActor<AActor>();

    actor->AddComponent<UActorComponent>("TestComponent");

    Engine::World.BeginPlay();

    while (s_isRunning)
    {
        TimePoint currentTime = clock.now();
        deltaTime =
            std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;
        Engine::World.Tick(deltaTime);

        // Slow down the loop to avoid high CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    Engine::World.EndPlay();
}

///////////////////////////////////////////////////////////////////////////////
#if TKD_ENGINE_CLIENT
void Engine::RenderThreadFunction(void)
{
    std::unique_ptr<IGraphicsFactory> factory =
        std::make_unique<SFML::GraphicsFactory>();

    auto window = factory->CreateWindow("TKD");
    auto renderer = factory->CreateRenderer(window.get());
    auto texture = factory->CreateTexture();

    texture->LoadFromFile("Game/Assets/Images/r-typesheet1.png");

    URectangleShape rect;
    UCircleShape circle;
    USprite sprite(*texture);

    rect.SetFillColor(FColor::Green);
    rect.SetSize(FVector2f(100.0f, 100.0f));
    rect.SetPosition(FVector2f(200.0f, 200.0f));
    rect.SetOutlineThickness(5.f);
    rect.SetOutlineColor(FColor::Red);

    circle.SetFillColor(FColor::Blue);
    circle.SetRadius(50.0f);
    circle.SetPosition(FVector2f(400.0f, 400.0f));
    circle.SetOutlineThickness(5.f);
    circle.SetOutlineColor(FColor::Yellow);

    sprite.SetPosition(FVector2f(300.0f, 300.0f));

    while (s_isRunning && window->IsOpen())
    {
        window->Update(0.0f);
        // TODO: Add rendering logic here
        window->Draw(
            [&rect, &circle, &renderer, &sprite]()
            {
                if (Engine::IsDebugBuild())
                {
                    debug::FDebug& debug = debug::FDebug::GetInstance();
                    debug.Show();
                }

                renderer->Draw(rect);
                renderer->Draw(circle);
                renderer->Draw(sprite);
            }
        );
    }

    window.reset();
    renderer.reset();

    s_isRunning = false;
}
#endif

///////////////////////////////////////////////////////////////////////////////
bool Engine::Initialize(int argc, char* argv[])
{
    if (s_isInitialized) { return false; }

    FArgs& args = FArgs::GetInstance();

    bool a_verbose = false;
    std::string a_gameModule;

    // Load game settings if the function is available
    if (TKD_GetEngineSettings)
    {
        s_isDebugBuild = false;
        Settings = TKD_GetEngineSettings();

        if (Settings.version != TKD_VERSION_STRING)
        {
            s_exitCode = TKD_EXIT_FAILURE;
            s_exitMessage = "Game module version mismatch. Expected " +
                            std::string(TKD_VERSION_STRING) + ", got " +
                            Settings.version + ".";
            return false;
        }

        args.AddFlags("debug", "Enable debug mode", s_isDebugBuild, false);
    }
    else
    {
        s_isDebugBuild = true;
        args.AddFlags("game", "Path to the game module", a_gameModule, true);
    }

    args.AddFlags("verbose", "Enable verbose logging", a_verbose, false);

#if TKD_ENGINE_SERVER
    std::string a_host = "localhost";
    UInt16 a_port = 8080;

    args.AddFlags("host", "Server hostname or IP address", a_host, false);
    args.AddFlags("port", "Server port number", a_port, false);
#endif

    // Try to process command-line arguments
    if (!args.Process(argc, argv))
    {
        // Error processing arguments
        if (args.GetExitCode() != 0)
        {
            s_exitCode = args.GetExitCode();
            s_exitMessage = "Failed to process command-line arguments.";
        }
        return false;
    }

    // Print startup message
    PrintStartupMessage();

    // Initialize input manager
    Inputs.Initialize(Settings);

#if TKD_ENGINE_SERVER
    // Initialize network subsystem if in server mode
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

    // Mark the engine as initialized
    s_isInitialized = true;
    return s_isInitialized;
}

///////////////////////////////////////////////////////////////////////////////
bool Engine::Shutdown(void)
{
    if (!s_isInitialized) { return false; }

    // Signal threads to stop
    s_isRunning = false;

#if TKD_ENGINE_SERVER
    // Shutdown network subsystem if in server mode
    Network::Shutdown();
#endif

    // Mark the engine as uninitialized
    s_isInitialized = false;
    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool Engine::IsInitialized(void) { return s_isInitialized; }

///////////////////////////////////////////////////////////////////////////////
bool Engine::IsDebugBuild(void) { return s_isDebugBuild; }

///////////////////////////////////////////////////////////////////////////////
void Engine::Run(void)
{
    if (!s_isInitialized || s_isRunning) { return; }

    s_isRunning = true;

    // Start the threads
    s_mainThread->Start();
    TKD_ENGINE_IF_CLIENT({ s_renderThread->Start(); })

    // Main loop
    while (s_isRunning)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // Shutdown threads if they are running
    if (s_mainThread && s_mainThread->running) { s_mainThread->Join(); }
    TKD_ENGINE_IF_CLIENT({
        if (s_renderThread && s_renderThread->running)
        {
            s_renderThread->Join();
        }
    })

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
