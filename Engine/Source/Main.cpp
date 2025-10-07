///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <csignal>
#include <Engine.hpp>

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
