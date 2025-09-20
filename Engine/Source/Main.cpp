///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine.hpp>

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
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
