///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include <Engine/Assets/UAsset.hpp>
#include <Engine/Assets/UPak.hpp>
#include <Engine/Core/Utils/FArgs.hpp>
#include <Engine/Core/Utils/FileSystem.hpp>
#include <Engine/Core/Utils/FUUID.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
// Forward declarations
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief Detects asset type based on file extension
///
/// \param filePath The path to the file
///
/// \return The detected asset type
///
///////////////////////////////////////////////////////////////////////////////
EAssetType DetectAssetType(const std::filesystem::path& filePath);

///////////////////////////////////////////////////////////////////////////////
/// \brief Creates assets from files in a directory
///
/// \param inputPath Path to input file or directory
/// \param assets Output vector of created assets
///
/// \return True if successful, false otherwise
///
///////////////////////////////////////////////////////////////////////////////
bool CreateAssetsFromPath(
    const std::filesystem::path& inputPath, std::vector<UAsset*>& assets
);

///////////////////////////////////////////////////////////////////////////////
/// \brief Packs assets into a PAK file
///
/// \param inputPaths Input files or directories
/// \param outputPath Output PAK file path
///
/// \return True if successful, false otherwise
///
///////////////////////////////////////////////////////////////////////////////
bool PackAssets(
    const std::vector<std::string>& inputPaths, const std::string& outputPath
);

///////////////////////////////////////////////////////////////////////////////
/// \brief Unpacks assets from a PAK file
///
/// \param pakPath Input PAK file path
/// \param outputDir Output directory path
/// \param listOnly If true, only list contents without extracting
///
/// \return True if successful, false otherwise
///
///////////////////////////////////////////////////////////////////////////////
bool UnpackAssets(
    const std::string& pakPath,
    const std::string& outputDir,
    bool listOnly = false
);

///////////////////////////////////////////////////////////////////////////////
/// \brief Prints usage information and examples
///
///////////////////////////////////////////////////////////////////////////////
void PrintExamples();

}   // namespace tkd

///////////////////////////////////////////////////////////////////////////////
tkd::EAssetType tkd::DetectAssetType(const std::filesystem::path& filePath)
{
    auto extension = filePath.extension().string();

    // Convert to lowercase for case-insensitive comparison
    std::transform(
        extension.begin(), extension.end(), extension.begin(), ::tolower
    );

    // Image/Texture extensions
    if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" ||
        extension == ".bmp" || extension == ".tga" || extension == ".dds" ||
        extension == ".ktx" || extension == ".astc" || extension == ".pvr")
    {
        return EAssetType::Texture;
    }

    // Audio extensions
    if (extension == ".wav" || extension == ".mp3" || extension == ".ogg" ||
        extension == ".flac" || extension == ".aiff" || extension == ".m4a" ||
        extension == ".opus" || extension == ".wma")
    {
        return EAssetType::Audio;
    }

    // Level/Scene extensions
    if (extension == ".level" || extension == ".scene" ||
        extension == ".map" || extension == ".world" || extension == ".json" ||
        extension == ".xml" || extension == ".yaml" || extension == ".yml")
    {
        return EAssetType::Level;
    }

    return EAssetType::Unknown;
}

///////////////////////////////////////////////////////////////////////////////
bool tkd::CreateAssetsFromPath(
    const std::filesystem::path& inputPath, std::vector<UAsset*>& assets
)
{
    try
    {
        if (std::filesystem::is_regular_file(inputPath))
        {
            // Single file
            auto assetType = DetectAssetType(inputPath);
            if (assetType == EAssetType::Unknown)
            {
                std::cout << "Warning: Unknown file type for " << inputPath
                          << ", treating as Unknown\n";
            }

            // Read file data
            auto data = tkd::FileSystem::ReadFileToBytes(inputPath);
            if (data.empty())
            {
                std::cerr << "Error: Could not read file " << inputPath
                          << "\n";
                return false;
            }

            // Create asset
            auto asset = new tkd::UAsset();
            asset->SetMetadata(
                tkd::UUID::V4().ToString(), inputPath.string(), assetType
            );
            asset->SetData(std::move(data));

            assets.push_back(asset);
            std::cout << "Added: " << inputPath
                      << " (Type: " << static_cast<int>(assetType)
                      << ", Size: " << data.size() << " bytes)\n";
        }
        else if (std::filesystem::is_directory(inputPath))
        {
            // Directory - process recursively
            for (const auto& entry:
                 std::filesystem::recursive_directory_iterator(inputPath))
            {
                if (entry.is_regular_file())
                {
                    if (!CreateAssetsFromPath(entry.path(), assets))
                    {
                        return false;
                    }
                }
            }
        }
        else
        {
            std::cerr << "Error: Path does not exist or is not accessible: "
                      << inputPath << "\n";
            return false;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error processing path " << inputPath << ": " << e.what()
                  << "\n";
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool tkd::PackAssets(
    const std::vector<std::string>& inputPaths, const std::string& outputPath
)
{
    std::vector<tkd::UAsset*> assets;

    std::cout << "TKDPack - Packing assets...\n";
    std::cout << "Output: " << outputPath << "\n\n";

    // Process all input paths
    for (const auto& inputPath: inputPaths)
    {
        std::cout << "Processing: " << inputPath << "\n";
        if (!CreateAssetsFromPath(inputPath, assets))
        {
            // Clean up allocated assets
            for (auto* asset: assets) { delete asset; }
            return false;
        }
    }

    if (assets.empty())
    {
        std::cerr << "Error: No assets found to pack\n";
        return false;
    }

    std::cout << "\nTotal assets to pack: " << assets.size() << "\n";

    // Create PAK file
    tkd::UPak pak;
    bool success = pak.Create(outputPath, assets);

    // Clean up allocated assets
    for (auto* asset: assets) { delete asset; }

    if (success)
    {
        std::cout << "Successfully created: " << outputPath << "\n";

        // Show file size
        auto fileSize = tkd::FileSystem::GetFileSize(outputPath);
        if (fileSize != static_cast<tkd::SizeT>(-1))
        {
            std::cout << "File size: " << fileSize << " bytes\n";
        }
    }
    else { std::cerr << "Error: Failed to create PAK file\n"; }

    return success;
}

///////////////////////////////////////////////////////////////////////////////
bool tkd::UnpackAssets(
    const std::string& pakPath, const std::string& outputDir, bool listOnly
)
{
    std::cout << "TKDPack - " << (listOnly ? "Listing" : "Unpacking")
              << " assets...\n";
    std::cout << "Input: " << pakPath << "\n";
    if (!listOnly) { std::cout << "Output: " << outputDir << "\n"; }
    std::cout << "\n";

    // Open PAK file
    tkd::UPak pak;
    if (!pak.Open(pakPath))
    {
        std::cerr << "Error: Could not open PAK file: " << pakPath << "\n";
        return false;
    }

    // Get all entries
    auto entries = pak.GetAllEntries();
    if (entries.empty())
    {
        std::cout << "PAK file is empty\n";
        return true;
    }

    std::cout << "Found " << entries.size() << " assets:\n\n";

    // List or extract entries
    for (const auto& entry: entries)
    {
        const char* typeStr = "Unknown";
        switch (entry.type)
        {
        case tkd::EAssetType::Texture: typeStr = "Texture"; break;
        case tkd::EAssetType::Audio  : typeStr = "Audio"; break;
        case tkd::EAssetType::Level  : typeStr = "Level"; break;
        default                      : typeStr = "Unknown"; break;
        }

        std::cout << "- " << entry.name << " (UUID: " << entry.uuid
                  << ", Type: " << typeStr << ", Size: " << entry.size
                  << " bytes)\n";

        if (!listOnly)
        {
            // Create output directory if it doesn't exist
            std::filesystem::path outPath(outputDir);
            std::filesystem::create_directories(outPath);

            // Determine file extension based on asset type
            std::string filename = entry.name;
            if (entry.type == tkd::EAssetType::Texture &&
                filename.find('.') == std::string::npos)
            {
                filename += ".png";   // Default extension for textures
            }
            else if (entry.type == tkd::EAssetType::Audio &&
                     filename.find('.') == std::string::npos)
            {
                filename += ".wav";   // Default extension for audio
            }
            else if (entry.type == tkd::EAssetType::Level &&
                     filename.find('.') == std::string::npos)
            {
                filename += ".json";   // Default extension for levels
            }

            auto fullPath = outPath / filename;

            // Load asset data
            std::vector<tkd::Byte> data;
            if (pak.LoadAssetData(entry.uuid, data))
            {
                // Write to file
                std::ofstream file(fullPath, std::ios::binary);
                if (file.is_open())
                {
                    file.write(
                        reinterpret_cast<const char*>(data.data()), data.size()
                    );
                    file.close();
                    std::cout << "  -> Extracted to: " << fullPath << "\n";
                }
                else
                {
                    std::cerr << "  -> Error: Could not write to " << fullPath
                              << "\n";
                }
            }
            else
            {
                std::cerr << "  -> Error: Could not load asset data for "
                          << entry.name << "\n";
            }
        }
    }

    pak.Close();

    if (!listOnly) { std::cout << "\nExtraction completed\n"; }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
void tkd::PrintExamples()
{
    std::cout << "\nTKDPack - Asset Packing Tool\n";
    std::cout << "============================\n\n";

    std::cout << "Pack single file:\n";
    std::cout
        << "  TKDPack --mode pack --input texture.png --output game.pak\n\n";

    std::cout << "Pack multiple files:\n";
    std::cout
        << "  TKDPack --mode pack --input \"texture.png,sound.wav,level.json\" --output game.pak\n\n";

    std::cout << "Pack entire directory:\n";
    std::cout << "  TKDPack --mode pack --input Assets/ --output game.pak\n\n";

    std::cout << "List PAK contents:\n";
    std::cout << "  TKDPack --mode list --input game.pak\n\n";

    std::cout << "Extract PAK file:\n";
    std::cout
        << "  TKDPack --mode extract --input game.pak --output extracted/\n\n";

    std::cout << "Short form examples:\n";
    std::cout << "  TKDPack -m pack -i texture.png -o game.pak\n";
    std::cout << "  TKDPack -m list -i game.pak\n";
    std::cout << "  TKDPack -m extract -i game.pak -o extracted/\n\n";

    std::cout << "Supported file types (auto-detected):\n";
    std::cout
        << "  Textures: .png, .jpg, .jpeg, .bmp, .tga, .dds, .ktx, .astc, .pvr\n";
    std::cout
        << "  Audio: .wav, .mp3, .ogg, .flac, .aiff, .m4a, .opus, .wma\n";
    std::cout
        << "  Levels: .level, .scene, .map, .world, .json, .xml, .yaml, .yml\n";
    std::cout
        << "  Unknown types are supported but will be marked as 'Unknown'\n\n";

    std::cout << "Notes:\n";
    std::cout
        << "  - Files are automatically assigned UUIDs and asset types based on extension\n";
    std::cout << "  - Directory processing is recursive\n";
    std::cout << "  - Use --help to see available options\n";
    std::cout << "  - Use --examples to see this help again\n\n";
}

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    // Command line arguments
    std::vector<std::string> inputPaths;
    std::string outputPath;
    std::string mode = "pack";
    bool showExamples = false;

    // Setup argument parser
    tkd::FArgs& args = tkd::FArgs::GetInstance();

    args.AddVectorFlags(
        "input",
        "Input file(s) or directory to pack, or PAK file to list/extract",
        inputPaths,
        true
    );

    args.AddFlags(
        "output",
        "Output PAK file (for pack) or directory (for extract)",
        outputPath,
        false
    );

    args.AddFlags(
        "mode", "Operation mode: 'pack', 'list', or 'extract'", mode, false
    );

    args.AddFlags("examples", "Show usage examples", showExamples, false);

    // Process arguments
    if (!args.Process(argc, argv)) { return args.GetExitCode(); }

    // Show examples if requested
    if (showExamples)
    {
        tkd::PrintExamples();
        return TKD_EXIT_SUCCESS;
    }

    // Validate arguments based on mode
    if (inputPaths.empty())
    {
        std::cerr << "Error: No input specified. Use --input or -i\n";
        tkd::PrintExamples();
        return 84;
    }

    // Execute operation based on mode
    try
    {
        if (mode == "list")
        {
            // List PAK contents
            if (inputPaths.size() != 1)
            {
                std::cerr
                    << "Error: List mode requires exactly one PAK file\n";
                return 84;
            }

            return tkd::UnpackAssets(inputPaths[0], "", true)
                       ? TKD_EXIT_SUCCESS
                       : 84;
        }
        else if (mode == "extract")
        {
            // Extract PAK file
            if (inputPaths.size() != 1)
            {
                std::cerr
                    << "Error: Extract mode requires exactly one PAK file\n";
                return 84;
            }

            if (outputPath.empty()) { outputPath = "extracted"; }

            return tkd::UnpackAssets(inputPaths[0], outputPath, false)
                       ? TKD_EXIT_SUCCESS
                       : 84;
        }
        else if (mode == "pack")
        {
            // Pack mode
            if (outputPath.empty())
            {
                std::cerr
                    << "Error: Output PAK file not specified for pack mode. Use --output or -o\n";
                return 84;
            }

            return tkd::PackAssets(inputPaths, outputPath) ? TKD_EXIT_SUCCESS
                                                           : 84;
        }
        else
        {
            std::cerr << "Error: Invalid mode '" << mode
                      << "'. Use 'pack', 'list', or 'extract'\n";
            tkd::PrintExamples();
            return 84;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return 84;
    }

    return TKD_EXIT_SUCCESS;
}
