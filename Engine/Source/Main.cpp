///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine.hpp>

///////////////////////////////////////////////////////////////////////////////
// Weak declaration of game functions to be defined in the game
///////////////////////////////////////////////////////////////////////////////
TKD_EXPORT_WEAK

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    if (TKD_CreateGame)
    {
        std::cout << "Game creation function is defined." << std::endl;
    }
    else
    {
        std::cout << "Game creation function is NOT defined." << std::endl;
    }

    if (TKD_GetEngineVersion)
    {
        std::cout << "Engine version function is defined." << std::endl;
    }
    else
    {
        std::cout << "Engine version function is NOT defined." << std::endl;
    }

    try
    {
        if (Engine::Initialize(argc, argv)) { Engine::Run(); }
        Engine::Shutdown();
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
