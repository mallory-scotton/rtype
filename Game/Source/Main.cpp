///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine.hpp>

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    tkd::FString greeting = "RType Game starting...";
    tkd::FString farewell = "RType Game stopped.";
    tkd::FVector2f vector(1.0f, 2.0f);

    std::cout << greeting << std::endl;

    std::cout << vector << std::endl;

    std::cout << farewell << std::endl;

    return TKD_EXIT_SUCCESS;
}
