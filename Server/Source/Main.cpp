///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine.hpp>

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    TKD_UNUSED(argc);
    TKD_UNUSED(argv);

    if (argc > 1 && argv[1] == std::string("--save"))
    {
        tkd::FSaveArchive save("TestFile.dat");

        int info = 12345;
        float pi = 3.14159f;
        tkd::FString message = "Hello, Archive!";

        save << info << pi << message;
    }
    else if (argc > 1 && argv[1] == std::string("--load"))
    {
        tkd::FLoadArchive load("TestFile.dat");

        int info = 0;
        float pi = 0.0f;
        tkd::FString message;

        load << info << pi << message;

        std::cout << "Info: " << info << std::endl;
        std::cout << "Pi: " << pi << std::endl;
        std::cout << "Message: " << message.CStr() << std::endl;
    }
    else
    {
        std::cout << "Usage: " << argv[0] << " --save | --load" << std::endl;
    }

    return TKD_EXIT_SUCCESS;
}
