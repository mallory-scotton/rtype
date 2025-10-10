///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Static/Engine.hpp>
#include <Engine/Assets/URessource.hpp>
#include <Engine/Core.hpp>
#include <Engine/Debug.hpp>
#include <Engine/Static/FNetworkInterface.hpp>

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
Engine::~Engine(void)
{
    if (m_running.load(std::memory_order_acquire)) { Shutdown(); }
}

///////////////////////////////////////////////////////////////////////////////
bool Engine::Initialize(int argc, char* argv[])
{
    if (m_initialized) { return false; }

    if (TKD_CreateGame)
    {
        m_game = std::move(TKD_CreateGame());
        if (!m_game)
        {
            m_exitCode = TKD_EXIT_FAILURE;
            m_exitMessage = "Failed to create game instance";
            return false;
        }
    }

    // Process command line
    if (!ProcessCommandLine(argc, argv)) { return false; }

    // Print startup message
    PrintStartupMessage();

    // Load Packed Ressources if any
    if (m_game)
    {
        FilePath exeDir = FileSystem::GetExecutableDirectory();

        for (const auto& resource: m_game->GetPackedResources())
        {
            if (!URessource::GetInstance().LoadPak(exeDir / resource))
            {
                m_exitCode = TKD_EXIT_FAILURE;
                m_exitMessage =
                    "Failed to load resource pack: " + resource.string();
                return false;
            }
            else
            {
                std::cout << "[Engine] Loaded resource pack: "
                          << resource.string() << std::endl;
            }
        }
    }

    try
    {
        std::cout << "[Engine] Configuring subsystems..." << std::endl;

        // Initialize world subsystem (always required)
        m_world = std::make_unique<FWorldSubsystem>(120.0f);
        if (!m_world || !m_world->Initialize())
        {
            m_exitCode = TKD_EXIT_FAILURE;
            m_exitMessage = "Failed to initialize world subsystem";
            return false;
        }

#if TKD_ENGINE_CLIENT
        std::cout << "[Engine] Configuring window subsystem..." << std::endl;

        // Initialize window subsystem (client only)
        m_window = std::make_unique<FWindowSubsystem>(m_settings);
        if (!m_window || !m_window->Initialize())
        {
            m_exitCode = TKD_EXIT_FAILURE;
            m_exitMessage = "Failed to initialize window subsystem";
            return false;
        }

        // Set debug mode for window
        if (m_settings.debug) { m_window->GetWindow()->SetDebugMode(true); }

        // Setup render callback
        SetupRenderCallback();

        std::cout << "[Engine] Connecting to server..." << std::endl;

        // Initialize network interface and connect to server (client only)
        std::string serverHost = "127.0.0.1";          // Default localhost
        UInt16 serverPort = m_settings.network.port;   // Use server port

        if (!Network::Connect(serverHost, serverPort))
        {
            std::cout
                << "[Engine] Warning: Failed to initialize network interface"
                << std::endl;
        }
        else
        {
            std::cout << "[Engine] Successfully connected to server at "
                      << serverHost << ":" << serverPort << std::endl;
        }
#endif

#if TKD_ENGINE_SERVER
        std::cout << "[Engine] Configuring network subsystem..." << std::endl;

        FNetworkSubsystem::Config networkConfig;

        networkConfig.mode = FNetworkSubsystem::Mode::Server;
        networkConfig.maxClients = m_settings.network.maxClients;
        networkConfig.port = m_settings.network.port;

        // Initialize network subsystem (server only)
        m_network = std::make_unique<FNetworkSubsystem>(networkConfig);
        if (!m_network || !m_network->Initialize())
        {
            m_exitCode = TKD_EXIT_FAILURE;
            m_exitMessage = "Failed to initialize network subsystem";
            return false;
        }
#endif

        std::cout << "[Engine] All subsystems initialized successfully"
                  << std::endl;
        m_initialized = true;
        return true;
    }
    catch (const std::exception& e)
    {
        m_exitCode = 1;
        m_exitMessage =
            FString("Exception during initialization: ") + e.what();
        return false;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Engine::Run(void)
{
    if (!m_initialized || m_running.load(std::memory_order_acquire))
    {
        return;
    }

    m_running.store(true, std::memory_order_release);

    // Start all subsystems
    m_world->Start();
    TKD_ENGINE_IF_CLIENT({ m_window->Start(); })
    TKD_ENGINE_IF_SERVER({ m_network->Start(); })

    std::cout << "[Engine] All subsystems started" << std::endl;

    // Main monitoring loop
    while (m_running.load(std::memory_order_acquire))
    {
#if TKD_ENGINE_CLIENT
        // Check if window was closed
        if (m_window && !m_window->IsOpen())
        {
            RequestShutdown();
            break;
        }
#endif

        // Sleep to reduce CPU usage in monitoring loop
        std::this_thread::sleep_for(Milliseconds(100));
    }

    // Set running to false to ensure all subsystems stop
    m_running.store(false, std::memory_order_release);

    // Shutdown subsystems in reverse order
    std::cout << "[Engine] Shutting down subsystems..." << std::endl;

    // Signal resource manager to stop accepting new loads
    std::cout << "[Engine] Signaling resource manager to shutdown..."
              << std::endl;
    URessource::GetInstance().BeginShutdown();

    // Shutdown world FIRST (before window) so actors can properly clean up
    // their input bindings while the input manager still exists
    if (m_world)
    {
        std::cout << "[Engine] Shutting down world subsystem..." << std::endl;
        m_world->Shutdown();
        m_world.reset();
    }

    TKD_ENGINE_IF_SERVER({
        if (m_network)
        {
            std::cout << "[Engine] Shutting down network subsystem..."
                      << std::endl;
            m_network->Shutdown();
            m_network.reset();
        }
    })

    TKD_ENGINE_IF_CLIENT({
        std::cout << "[Engine] Shutting down network..." << std::endl;
        if (Network::IsInitialized()) { Network::Shutdown(); }

        if (m_window)
        {
            std::cout << "[Engine] Shutting down window subsystem..."
                      << std::endl;
            m_window->Shutdown();
            m_window.reset();
        }
    })

    std::cout << "[Engine] Shutdown complete" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////
void Engine::RequestShutdown(void)
{
    m_running.store(false, std::memory_order_release);
}

///////////////////////////////////////////////////////////////////////////////
void Engine::Shutdown(void)
{
    RequestShutdown();

    // Signal resource manager to stop accepting new loads
    URessource::GetInstance().BeginShutdown();

    // Force shutdown if Run() wasn't called
    TKD_ENGINE_IF_SERVER({
        if (m_network) { m_network->Shutdown(); }
    })

    TKD_ENGINE_IF_CLIENT({
        // Shutdown network interface
        std::cout << "[ENGINE] Shutting down network..." << std::endl;
        if (Network::IsInitialized()) { Network::Shutdown(); }

        if (m_window) { m_window->Shutdown(); }
    })

    if (m_world) { m_world->Shutdown(); }

    m_initialized = false;
}

///////////////////////////////////////////////////////////////////////////////
void Engine::PrintExitMessage(void) const
{
    std::ostream& out = (m_exitCode == 0) ? std::cout : std::cerr;

    if (!m_exitMessage.IsEmpty()) { out << m_exitMessage << std::endl; }
}

///////////////////////////////////////////////////////////////////////////////
void Engine::PrintStartupMessage(void) const
{
    std::string gameName = "NOT_LOADED";
    if (m_game)
    {
        gameName = m_settings.game.title.empty() ? "NOT_SPECIFIED"
                                                 : m_settings.game.title;
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
}

///////////////////////////////////////////////////////////////////////////////
void Engine::SetupRenderCallback(void)
{
#if TKD_ENGINE_CLIENT
    if (!m_window || !m_world) { return; }

    m_window->SetRenderCallback(
        [this](IRenderer& renderer)
        {
            TKD_UNUSED(renderer);

            // Render world with read-only access
            if (auto* world = m_world->GetWorld())
            {
                // Render world entities
                world->Render(renderer);

                // Show debug UI if in debug build
                if (m_settings.debug)
                {
                    debug::FDebug& debug = debug::FDebug::GetInstance();
                    debug.Show(m_settings, world);
                }
            }
        }
    );
#endif
}

///////////////////////////////////////////////////////////////////////////////
bool Engine::ProcessCommandLine(int argc, char* argv[])
{
    FArgs& args = FArgs::GetInstance();

    bool debugMode = false;
    bool verbose = false;
    std::string gameModule;

    // Load game settings if available
    if (m_game)
    {
        m_settings = m_game->GetEngineSettings();

        if (m_settings.version != TKD_VERSION_STRING)
        {
            m_exitCode = 1;
            m_exitMessage = "Game module version mismatch. Expected " +
                            std::string(TKD_VERSION_STRING) + ", got " +
                            m_settings.version + ".";
            return false;
        }

        args.AddFlags("debug", "Enable debug mode", debugMode, false);
    }
    else
    {
        args.AddFlags("game", "Path to the game module", gameModule, true);
    }

    args.AddFlags("verbose", "Enable verbose logging", verbose, false);

#if TKD_ENGINE_SERVER
    std::string host = "localhost";
    UInt16 port = 8080;

    args.AddFlags("host", "Server hostname or IP address", host, false);
    args.AddFlags("port", "Server port number", port, false);
#endif

    // Process arguments
    if (!args.Process(argc, argv))
    {
        if (args.GetExitCode() != 0)
        {
            m_exitCode = args.GetExitCode();
            m_exitMessage = "Failed to process command-line arguments.";
        }
        return false;
    }

    if (!m_game)
    {
        if (!FileSystem::FileExists(gameModule))
        {
            m_exitCode = 1;
            m_exitMessage = "Game module not found: " + gameModule;
            return false;
        }

        // Attempt to load the game module
        auto gameLib = FLibrary::Load(gameModule);

        // Check if library loaded successfully
        if (!gameLib || !gameLib->IsLoaded() ||
            !gameLib->HasFunction("TKD_CreateGame"))
        {
            m_exitCode = 1;
            m_exitMessage =
                "Failed to load game module: " + gameLib->GetLastError();
            return false;
        }

        // Get the factory function
        auto CreateGame = gameLib->GetFunctionWrapper<TUniquePtr<UGame>(void)>(
            "TKD_CreateGame"
        );

        // Create the game instance
        if (!CreateGame)
        {
            m_exitCode = 1;
            m_exitMessage = "Failed to find TKD_CreateGame in module: " +
                            gameLib->GetLastError();
            return false;
        }

        // Create the game instance
        m_game = std::move(CreateGame());

        // Validate the game instance
        if (!m_game)
        {
            m_exitCode = 1;
            m_exitMessage = "Failed to create game instance from module.";
            return false;
        }

        // Retrieve engine settings from the game
        m_settings = m_game->GetEngineSettings();

        // Check for version mismatch
        if (m_settings.version != TKD_VERSION_STRING)
        {
            m_exitCode = 1;
            m_exitMessage = "Game module version mismatch. Expected " +
                            std::string(TKD_VERSION_STRING) + ", got " +
                            m_settings.version + ".";
            return false;
        }
    }

    // Apply debug and verbose mode if specified
    if (debugMode) { m_settings.debug = true; }
    if (verbose) { m_settings.logging.enableConsoleLogging = true; }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD bool Engine::IsRunning(void) const noexcept
{
    return m_running.load(std::memory_order_acquire);
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD bool Engine::IsInitialized(void) const noexcept
{
    return m_initialized;
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD const FEngineSettings& Engine::GetSettings(void) const noexcept
{
    return m_settings;
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD int Engine::GetExitCode(void) const noexcept
{
    return m_exitCode;
}

#if TKD_ENGINE_CLIENT
///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD FWindowSubsystem* Engine::GetWindow(void) noexcept
{
    return m_window.get();
}
#endif

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD FNetworkSubsystem* Engine::GetNetwork(void) noexcept
{
    return m_network.get();
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD FWorldSubsystem* Engine::GetWorld(void) noexcept
{
    return m_world.get();
}

}   // namespace tkd::__internal
