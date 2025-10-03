///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Renderer/IRenderer.hpp>
#include <Engine/Renderer/IRenderTarget.hpp>
#include <Engine/Renderer/IShader.hpp>
#include <Engine/Renderer/ITexture.hpp>
#include <Engine/Renderer/IWindow.hpp>
#include <memory>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief Graphics factory interface for creating renderers, windows,
/// textures, and shaders.
///
///////////////////////////////////////////////////////////////////////////////
class IGraphicsFactory
{
public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Virtual destructor
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual ~IGraphicsFactory() = default;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Create an empty texture
    ///
    /// \return unique pointer to the Created texture
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual std::unique_ptr<ITexture> CreateTexture(void) const = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Create a texture from a file
    ///
    /// \param filePath path to the texture file
    /// \param area rectangular area of the texture to load
    ///
    /// \return unique pointer to the Created texture
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual std::unique_ptr<ITexture> CreateTexture(
        const FilePath& filePath, const FRectanglei& area = FRectanglei::Zero
    ) const = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Create a texture from raw data
    ///
    /// \param data pointer to the raw texture data
    /// \param size size of the data in bytes
    /// \param area rectangular area of the texture to load
    ///
    /// \return unique pointer to the Created texture
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual std::unique_ptr<ITexture> CreateTexture(
        const void* data,
        SizeT size,
        const FRectanglei& area = FRectanglei::Zero
    ) const = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Create a texture from byte vector
    ///
    /// \param bytes vector containing the texture data
    /// \param area rectangular area of the texture to load
    ///
    /// \return unique pointer to the Created texture
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual std::unique_ptr<ITexture> CreateTexture(
        const TVector<Byte>& bytes, const FRectanglei& area = FRectanglei::Zero
    ) const = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Create a texture from an asset
    ///
    /// \param asset asset containing the texture data
    /// \param area rectangular area of the texture to load
    ///
    /// \return unique pointer to the Created texture
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual std::unique_ptr<ITexture> CreateTexture(
        const UAsset& asset, const FRectanglei& area = FRectanglei::Zero
    ) const = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Create a renderer for a window
    ///
    /// \param window pointer to the window to attach the renderer to
    ///
    /// \return unique pointer to the Created renderer
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual std::unique_ptr<IRenderer> CreateRenderer(IWindow* window
    ) const = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Create a window
    ///
    /// \param title window title
    /// \param openDefault whether to open the window by default
    /// \param position window position on screen
    /// \param dimension window dimensions (width, height)
    /// \param state window state (windowed, fullscreen, etc.)
    ///
    /// \return unique pointer to the Created window
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual std::unique_ptr<IWindow> CreateWindow(
        const FString& title,
        bool openDefault = true,
        const FVector2i& position = FVector2i::Zero,
        const FVector2u& dimension = FVector2u(1280, 720),
        EWindowState state = EWindowState::Windowed
    ) const = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Create an empty shader
    ///
    /// \return unique pointer to the Created shader
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual std::unique_ptr<IShader> CreateShader(void) const = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Create a shader from a file
    ///
    /// \param filePath path to the shader file
    /// \param type shader type (vertex, fragment, geometry, etc.)
    ///
    /// \return unique pointer to the Created shader
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual std::unique_ptr<IShader>
        CreateShader(const FilePath& filePath, EShaderType type) const = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Create a shader from vertex and fragment files
    ///
    /// \param vertexPath path to the vertex shader file
    /// \param fragmentPath path to the fragment shader file
    ///
    /// \return unique pointer to the Created shader
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual std::unique_ptr<IShader> CreateShader(
        const FilePath& vertexPath, const FilePath& fragmentPath
    ) const = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Create a shader from vertex, geometry and fragment files
    ///
    /// \param vertexPath path to the vertex shader file
    /// \param geometryPath path to the geometry shader file
    /// \param fragmentPath path to the fragment shader file
    ///
    /// \return unique pointer to the Created shader
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual std::unique_ptr<IShader> CreateShader(
        const FilePath& vertexPath,
        const FilePath& geometryPath,
        const FilePath& fragmentPath
    ) const = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Create a shader from source string
    ///
    /// \param shader shader source code
    /// \param type shader type (vertex, fragment, geometry, etc.)
    ///
    /// \return unique pointer to the Created shader
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual std::unique_ptr<IShader>
        CreateShader(const FString& shader, EShaderType type) const = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Create a shader from vertex and fragment source strings
    ///
    /// \param vertex vertex shader source code
    /// \param fragment fragment shader source code
    ///
    /// \return unique pointer to the Created shader
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual std::unique_ptr<IShader>
        CreateShader(const FString& vertex, const FString& fragment) const = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Create a shader from vertex, geometry and fragment source
    /// strings
    ///
    /// \param vertex vertex shader source code
    /// \param geometry geometry shader source code
    /// \param fragment fragment shader source code
    ///
    /// \return unique pointer to the Created shader
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual std::unique_ptr<IShader> CreateShader(
        const FString& vertex, const FString& geometry, const FString& fragment
    ) const = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Create a shader from byte vector
    ///
    /// \param shader shader source code in bytes
    /// \param type shader type (vertex, fragment, geometry, etc.)
    ///
    /// \return unique pointer to the Created shader
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual std::unique_ptr<IShader>
        CreateShader(const TVector<Byte>& shader, EShaderType type) const = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Create a shader from vertex and fragment byte vectors
    ///
    /// \param vertex vertex shader source code in bytes
    /// \param fragment fragment shader source code in bytes
    ///
    /// \return unique pointer to the Created shader
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual std::unique_ptr<IShader> CreateShader(
        const TVector<Byte>& vertex, const TVector<Byte>& fragment
    ) const = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Create a shader from vertex, geometry and fragment byte vectors
    ///
    /// \param vertex vertex shader source code in bytes
    /// \param geometry geometry shader source code in bytes
    /// \param fragment fragment shader source code in bytes
    ///
    /// \return unique pointer to the Created shader
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual std::unique_ptr<IShader> CreateShader(
        const TVector<Byte>& vertex,
        const TVector<Byte>& geometry,
        const TVector<Byte>& fragment
    ) const = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Create a shader from an asset
    ///
    /// \param asset asset containing the shader source code
    /// \param type shader type (vertex, fragment, geometry, etc.)
    ///
    /// \return unique pointer to the Created shader
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual std::unique_ptr<IShader>
        CreateShader(const UAsset& asset, EShaderType type) const = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Create a shader from vertex and fragment assets
    ///
    /// \param vertex asset containing the vertex shader source code
    /// \param fragment asset containing the fragment shader source code
    ///
    /// \return unique pointer to the Created shader
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual std::unique_ptr<IShader>
        CreateShader(const UAsset& vertex, const UAsset& fragment) const = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Create a shader from vertex, geometry and fragment assets
    ///
    /// \param vertex asset containing the vertex shader source code
    /// \param geometry asset containing the geometry shader source code
    /// \param fragment asset containing the fragment shader source code
    ///
    /// \return unique pointer to the Created shader
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual std::unique_ptr<IShader> CreateShader(
        const UAsset& vertex, const UAsset& geometry, const UAsset& fragment
    ) const = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Create a render target
    ///
    /// \return unique pointer to the Created render target
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual std::unique_ptr<IRenderTarget> CreateRenderTarget(void) const = 0;
};

}   // namespace tkd
