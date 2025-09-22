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
        if (Engine::IsInitialized())
        {
            Engine::SetExitCode(TKD_EXIT_SUCCESS);
            Engine::SetExitMessage("Graceful shutdown");
            Engine::Shutdown();
        }
        std::cerr
            << "\nEngine terminated unexpectedly, graceful shutdown initiated"
            << std::endl;
        std::exit(Engine::GetExitCode());
    }
}

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    std::signal(SIGINT, SignalHandler);
    std::signal(SIGTERM, SignalHandler);

    try
    {
        if (Engine::Initialize(argc, argv))
        {
            Engine::Run();
            Engine::Shutdown();
        }
    }
    catch (const std::exception& error)
    {
        Engine::SetExitCode(TKD_EXIT_FAILURE);
        Engine::SetExitMessage(
            tkd::FString("Unhandled exception: ") + tkd::FString(error.what())
        );
    }

    Engine::PrintExitMessage();
    return Engine::GetExitCode();
}
