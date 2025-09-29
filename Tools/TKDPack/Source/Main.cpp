///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Assets/UAsset.hpp>
#include <Engine/Assets/UPak.hpp>
#include <Engine/Core/Utils/FArgs.hpp>
#include <Engine/Core/Utils/FileSystem.hpp>
#include <Engine/Core/Utils/FUUID.hpp>

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    tkd::FArgs args;

    if (!args.Process(argc, argv)) { return args.GetExitCode(); }

    return TKD_EXIT_SUCCESS;
}
