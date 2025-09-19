///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine.hpp>

///////////////////////////////////////////////////////////////////////////////
void SaveTest(void)
{
    std::unique_ptr<tkd::FSaveArchive> save =
        std::make_unique<tkd::FSaveArchive>("TestFile.dat");

    int info = 42;
    float pi = 3.14159f;
    std::string message = "Hello World!";

    *save << info << pi << message;

    std::cout << "Data saved successfully" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////
void LoadTest(void)
{
    std::unique_ptr<tkd::FLoadArchive> load =
        std::make_unique<tkd::FLoadArchive>("TestFile.dat");

    std::cout << "Archive loaded, size: " << load->TotalSize() << std::endl;

    if (load->TotalSize() == 0)
    {
        std::cout << "ERROR: Archive is empty!" << std::endl;
        return;
    }

    int info = 0;
    float pi = 0.0f;
    std::string message;

    *load << info << pi << message;

    std::cout << "Info: " << info << std::endl;
    std::cout << "Pi: " << pi << std::endl;
    std::cout << "Message: " << message << std::endl;
}

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    TKD_UNUSED(argc);
    TKD_UNUSED(argv);

    std::cout << "\n\nSave Archive Test" << std::endl;
    std::cout << "-----------------" << std::endl;
    SaveTest();
    std::cout << "-----------------" << std::endl;

    std::cout << "\n\nLoad Archive Test" << std::endl;
    std::cout << "-----------------" << std::endl;
    LoadTest();
    std::cout << "-----------------" << std::endl;

    return TKD_EXIT_SUCCESS;
}
