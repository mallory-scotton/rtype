///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine.hpp>

using namespace tkd;

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    FString greeting = "RType Game starting...";
    FString farewell = "RType Game stopped.";
    FVector2f vector(1.0f, 2.0f);
    IWindow& window = IWindow::GetInstance();

    struct FakeEvent
    {
        int value;
    };

    window.On<IWindow::Events::Opened>(
        [](const IWindow::Events::Opened&)
        { std::cout << "Window opened event received!" << std::endl; }
    );

    std::cout << greeting << std::endl;

    std::cout << vector << std::endl;

    window.Start();

    std::cout << farewell << std::endl;

    return TKD_EXIT_SUCCESS;
}
