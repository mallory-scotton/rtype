///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Renderer/SFML/GraphicsFactory.hpp>
#include <Engine/Renderer/IGraphicsFactory.hpp>
#include <Engine/Renderer/SFML/Renderer.hpp>
#include <Engine/Renderer/SFML/RenderTarget.hpp>
#include <Engine/Renderer/SFML/Shader.hpp>
#include <Engine/Renderer/SFML/Texture.hpp>
#include <Engine/Renderer/SFML/Window.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::SFML
///////////////////////////////////////////////////////////////////////////////
namespace tkd::SFML
{

#if TKD_ENGINE_CLIENT

///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<ITexture> GraphicsFactory::CreateTexture(void) const
{
    return std::make_unique<SFML::Texture>();
}

///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<ITexture> GraphicsFactory::CreateTexture(
    const FilePath& filePath, const FRectanglei& area
) const
{
    return std::make_unique<SFML::Texture>(filePath, area);
}

///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<ITexture> GraphicsFactory::CreateTexture(
    const void* data, SizeT size, const FRectanglei& area
) const
{
    return std::make_unique<SFML::Texture>(data, size, area);
}

///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<ITexture> GraphicsFactory::CreateTexture(
    const TVector<Byte>& bytes, const FRectanglei& area
) const
{
    return std::make_unique<SFML::Texture>(bytes, area);
}

///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<ITexture> GraphicsFactory::CreateTexture(
    const UAsset& asset, const FRectanglei& area
) const
{
    return std::make_unique<SFML::Texture>(asset, area);
}

///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<IRenderer> GraphicsFactory::CreateRenderer(IWindow* window
) const
{
    return std::make_unique<SFML::Renderer>(window);
}

///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<IWindow> GraphicsFactory::CreateWindow(
    const FString& title,
    bool openDefault,
    const FVector2i& position,
    const FVector2u& dimension,
    EWindowState state
) const
{
    return std::make_unique<SFML::Window>(
        title, openDefault, position, dimension, state
    );
}

///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<IShader> GraphicsFactory::CreateShader(void) const
{
    return std::make_unique<SFML::Shader>();
}

///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<IShader> GraphicsFactory::CreateShader(
    const FilePath& filePath, EShaderType type
) const
{
    return std::make_unique<SFML::Shader>(filePath, type);
}

///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<IShader> GraphicsFactory::CreateShader(
    const FilePath& vertexPath, const FilePath& fragmentPath
) const
{
    return std::make_unique<SFML::Shader>(vertexPath, fragmentPath);
}

///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<IShader> GraphicsFactory::CreateShader(
    const FilePath& vertexPath,
    const FilePath& geometryPath,
    const FilePath& fragmentPath
) const
{
    return std::make_unique<SFML::Shader>(
        vertexPath, geometryPath, fragmentPath
    );
}

///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<IShader> GraphicsFactory::CreateShader(
    const FString& shader, EShaderType type
) const
{
    return std::make_unique<SFML::Shader>(shader, type);
}

///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<IShader> GraphicsFactory::CreateShader(
    const FString& vertex, const FString& fragment
) const
{
    return std::make_unique<SFML::Shader>(vertex, fragment);
}

///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<IShader> GraphicsFactory::CreateShader(
    const FString& vertex, const FString& geometry, const FString& fragment
) const
{
    return std::make_unique<SFML::Shader>(vertex, geometry, fragment);
}

///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<IShader> GraphicsFactory::CreateShader(
    const TVector<Byte>& shader, EShaderType type
) const
{
    return std::make_unique<SFML::Shader>(shader, type);
}

///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<IShader> GraphicsFactory::CreateShader(
    const TVector<Byte>& vertex, const TVector<Byte>& fragment
) const
{
    return std::make_unique<SFML::Shader>(vertex, fragment);
}

///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<IShader> GraphicsFactory::CreateShader(
    const TVector<Byte>& vertex,
    const TVector<Byte>& geometry,
    const TVector<Byte>& fragment
) const
{
    return std::make_unique<SFML::Shader>(vertex, geometry, fragment);
}

///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<IShader>
    GraphicsFactory::CreateShader(const UAsset& asset, EShaderType type) const
{
    return std::make_unique<SFML::Shader>(asset, type);
}

///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<IShader> GraphicsFactory::CreateShader(
    const UAsset& vertex, const UAsset& fragment
) const
{
    return std::make_unique<SFML::Shader>(vertex, fragment);
}

///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<IShader> GraphicsFactory::CreateShader(
    const UAsset& vertex, const UAsset& geometry, const UAsset& fragment
) const
{
    return std::make_unique<SFML::Shader>(vertex, geometry, fragment);
}

///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<IRenderTarget> GraphicsFactory::CreateRenderTarget(void) const
{
    return std::make_unique<SFML::RenderTarget>();
}

#endif

}   // namespace tkd::SFML
