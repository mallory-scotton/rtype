///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <csignal>
#include <Engine.hpp>

#ifndef TKD_ENGINE_STANDALONE

///////////////////////////////////////////////////////////////////////////////
// External Functions
///////////////////////////////////////////////////////////////////////////////
extern "C" TKD_API_IMPORT void TKD_EnsureGameLibraryLoaded(void);

#endif

///////////////////////////////////////////////////////////////////////////////
// Graceful exit to ensure shutdown of the engine when SIGINT or SIGTERM
///////////////////////////////////////////////////////////////////////////////
void SignalHandler(int signal)
{
    if (signal == SIGINT || signal == SIGTERM)
    {
        std::cout << "\nReceived shutdown signal..." << std::endl;
        Engine::RequestShutdown();
    }
}

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    std::signal(SIGINT, SignalHandler);
    std::signal(SIGTERM, SignalHandler);

    // Setting the random seed
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Force loading of the game library
#ifndef TKD_ENGINE_STANDALONE
    TKD_EnsureGameLibraryLoaded();
#endif

    // Initialize engine
    if (!Engine::Initialize(argc, argv))
    {
        Engine::PrintExitMessage();
        return Engine::GetExitCode();
    }

    // Run engine (blocks until shutdown)
    Engine::Run();

    // Print exit message
    Engine::PrintExitMessage();

    // Cleanup
    Engine::Shutdown();

    return Engine::GetExitCode();
}
