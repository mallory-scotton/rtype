///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Renderer/IShader.hpp>
#include <stdexcept>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
IShader::IShader(const FilePath& filePath, EShaderType type)
{
    if (!this->LoadFromFile(filePath, type))
    {
        throw std::runtime_error(
            "Failed to load shader from file: " + filePath.string()
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
IShader::IShader(const FilePath& vertexPath, const FilePath& fragmentPath)
{
    if (!this->LoadFromFile(vertexPath, fragmentPath))
    {
        throw std::runtime_error(
            "Failed to load shader from files: " + vertexPath.string() + ", " +
            fragmentPath.string()
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
IShader::IShader(
    const FilePath& vertexPath,
    const FilePath& geometryPath,
    const FilePath& fragmentPath
)
{
    if (!this->LoadFromFile(vertexPath, geometryPath, fragmentPath))
    {
        throw std::runtime_error(
            "Failed to load shader from files: " + vertexPath.string() + ", " +
            geometryPath.string() + ", " + fragmentPath.string()
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
IShader::IShader(const FString& shader, EShaderType type)
{
    if (!this->LoadFromMemory(shader, type))
    {
        throw std::runtime_error("Failed to load shader from memory");
    }
}

///////////////////////////////////////////////////////////////////////////////
IShader::IShader(const FString& vertex, const FString& fragment)
{
    if (!this->LoadFromMemory(vertex, fragment))
    {
        throw std::runtime_error("Failed to load shader from memory");
    }
}

///////////////////////////////////////////////////////////////////////////////
IShader::IShader(
    const FString& vertex, const FString& geometry, const FString& fragment
)
{
    if (!this->LoadFromMemory(vertex, geometry, fragment))
    {
        throw std::runtime_error("Failed to load shader from memory");
    }
}

///////////////////////////////////////////////////////////////////////////////
IShader::IShader(const TVector<Byte>& shader, EShaderType type)
{
    if (!this->LoadFromBytes(shader, type))
    {
        throw std::runtime_error("Failed to load shader from bytes");
    }
}

///////////////////////////////////////////////////////////////////////////////
IShader::IShader(const TVector<Byte>& vertex, const TVector<Byte>& fragment)
{
    if (!this->LoadFromBytes(vertex, fragment))
    {
        throw std::runtime_error("Failed to load shader from bytes");
    }
}

///////////////////////////////////////////////////////////////////////////////
IShader::IShader(
    const TVector<Byte>& vertex,
    const TVector<Byte>& geometry,
    const TVector<Byte>& fragment
)
{
    if (!this->LoadFromBytes(vertex, geometry, fragment))
    {
        throw std::runtime_error("Failed to load shader from bytes");
    }
}

///////////////////////////////////////////////////////////////////////////////
IShader::IShader(const UAsset& asset, EShaderType type)
{
    if (!this->LoadFromAsset(asset, type))
    {
        throw std::runtime_error("Failed to load shader from asset");
    }
}

///////////////////////////////////////////////////////////////////////////////
IShader::IShader(const UAsset& vertex, const UAsset& fragment)
{
    if (!this->LoadFromAsset(vertex, fragment))
    {
        throw std::runtime_error("Failed to load shader from assets");
    }
}

///////////////////////////////////////////////////////////////////////////////
IShader::IShader(
    const UAsset& vertex, const UAsset& geometry, const UAsset& fragment
)
{
    if (!this->LoadFromAsset(vertex, geometry, fragment))
    {
        throw std::runtime_error("Failed to load shader from assets");
    }
}

}   // namespace tkd
