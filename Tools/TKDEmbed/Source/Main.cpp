///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Core.hpp>
#include <fstream>
#include <iomanip>
#include <vector>
#include <zlib.h>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
bool FileToHeader(const FilePath& filePath, const FString& arrayName)
{
    FilePath headerPath = (arrayName + ".hpp").ToStdString();
    FilePath sourcePath = (arrayName + ".cpp").ToStdString();

    std::cout << "Converting file: " << filePath
              << " to header: " << headerPath << " and source: " << sourcePath
              << std::endl;

    // Open input file in binary mode
    std::ifstream inputFile(
        filePath.c_str(), std::ios::binary | std::ios::ate
    );
    if (!inputFile.is_open())
    {
        std::cerr << "Error: Cannot open input file: " << filePath
                  << std::endl;
        return false;
    }

    std::cout << "Processing file data..." << std::endl;

    // Get file size
    std::streamsize fileSize = inputFile.tellg();
    inputFile.seekg(0, std::ios::beg);

    std::cout << "Original file size: " << fileSize << " bytes" << std::endl;

    std::cout << "Reading file..." << std::endl;

    // Read file contents
    std::vector<uint8_t> buffer(fileSize);
    if (!inputFile.read(reinterpret_cast<char*>(buffer.data()), fileSize))
    {
        std::cerr << "Error: Failed to read input file: " << filePath
                  << std::endl;
        inputFile.close();
        return false;
    }
    inputFile.close();

    std::cout << "Compressing data..." << std::endl;

    uLongf compressedSize = compressBound(fileSize);
    std::vector<uint8_t> compressedBuffer(compressedSize);

    int result = compress2(
        compressedBuffer.data(),
        &compressedSize,
        buffer.data(),
        fileSize,
        Z_BEST_COMPRESSION
    );

    if (result != Z_OK)
    {
        std::cerr << "Error: Compression failed with error code: " << result
                  << std::endl;
        return false;
    }

    std::cout << "Compression successful. Compressed size: " << compressedSize
              << " bytes" << std::endl;

    // Generate header guard name
    std::string guardName = arrayName;
    for (char& c: guardName)
    {
        c = std::toupper(static_cast<unsigned char>(c));
    }
    guardName += "_H";

    // Write header file
    std::ofstream headerFile(headerPath.c_str());
    if (!headerFile.is_open())
    {
        std::cerr << "Error: Cannot create output file: " << headerPath
                  << std::endl;
        return false;
    }

    std::cout << "Writing header file..." << std::endl;

    headerFile << "// Auto-generated header file from: " << filePath << "\n";
    headerFile << "// Original file size: " << fileSize << " bytes\n";
    headerFile << "// Compressed size: " << compressedSize << " bytes\n\n";
    headerFile << "#ifndef " << guardName << "\n";
    headerFile << "#define " << guardName << "\n\n";
    headerFile << "#include <cstdint>\n\n";

    // Write declarations only
    headerFile << "extern const uint8_t " << arrayName << "[];\n";
    headerFile << "extern const size_t " << arrayName << "Size;\n\n";

    headerFile << "#endif // " << guardName << "\n";
    headerFile.close();

    std::cout << "Header file written successfully: " << headerPath
              << std::endl;

    // Write source file
    std::ofstream sourceFile(sourcePath.c_str());
    if (!sourceFile.is_open())
    {
        std::cerr << "Error: Cannot create output file: " << sourcePath
                  << std::endl;
        return false;
    }

    std::cout << "Writing source file..." << std::endl;

    sourceFile << "// Auto-generated source file from: " << filePath << "\n";
    sourceFile << "// Original file size: " << fileSize << " bytes\n";
    sourceFile << "// Compressed size: " << compressedSize << " bytes\n\n";
    sourceFile << "#include \"" << arrayName << ".hpp\"\n\n";

    // Write array definition
    sourceFile << "const uint8_t " << arrayName << "[] = {\n";

    // Write byte data (16 bytes per line)
    const size_t bytesPerLine = 16;
    for (size_t i = 0; i < compressedSize; ++i)
    {
        if (i % bytesPerLine == 0) { sourceFile << "    "; }

        sourceFile << "0x" << std::hex << std::setw(2) << std::setfill('0')
                   << static_cast<int>(compressedBuffer[i]);

        if (i < compressedSize - 1)
        {
            sourceFile << ",";
            if ((i + 1) % bytesPerLine == 0) { sourceFile << "\n"; }
            else { sourceFile << " "; }
        }
        else { sourceFile << "\n"; }
    }

    sourceFile << "};\n\n";
    sourceFile << "const size_t " << arrayName << "Size = sizeof(" << arrayName
               << ");\n";

    sourceFile.close();

    std::cout << "Source file written successfully: " << sourcePath
              << std::endl;

    return true;
}

}   // namespace tkd

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    tkd::FArgs& args = tkd::FArgs::GetInstance();

    std::string inputFile;
    std::string arrayName = "fileData";

    args.AddFlags(
        "input", "Input file to convert to a C++ header", inputFile, true
    );
    args.AddFlags(
        "name", "Name of the generated byte array", arrayName, false
    );

    if (!args.Process(argc, argv)) { return args.GetExitCode(); }

    arrayName = "EMBEDDED_" + arrayName;

    if (!tkd::FileToHeader(inputFile, arrayName.c_str()))
    {
        std::cerr << "Failed to convert file to header." << std::endl;
        return TKD_EXIT_FAILURE;
    }

    return TKD_EXIT_SUCCESS;
}
