///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Assets.hpp>
#include <Engine/Config.hpp>
#include <Engine/Core/Containers.hpp>
#include <Engine/Core/Math.hpp>
#include <Engine/Renderer/ITexture.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief Shader type enumeration
///
///////////////////////////////////////////////////////////////////////////////
enum class EShaderType : UInt8
{
    Vertex = 0,   //<! Vertex shader
    Fragment,     //<! Fragment/Pixel shader
    Geometry,     //<! Geometry shader
    Compute       //<! Compute shader
};

///////////////////////////////////////////////////////////////////////////////
/// \brief Shader interface
///
///////////////////////////////////////////////////////////////////////////////
class IShader
{
public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Virtual destructor
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual ~IShader() = default;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Load shader from file
    ///
    /// \param filePath shader file path
    /// \param type shader type
    ///
    /// \return true if loading was successful, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual bool LoadFromFile(const FilePath& filePath, EShaderType type) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Load shader from file
    ///
    /// \param vertexPath vertex shader file path
    /// \param fragmentPath fragment shader file path
    ///
    /// \return true if loading was successful, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual bool LoadFromFile(
        const FilePath& vertexPath, const FilePath& fragmentPath
    ) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Load shader from file
    ///
    /// \param vertexPath vertex shader file path
    /// \param geometryPath geometry shader file path
    /// \param fragmentPath fragment shader file path
    ///
    /// \return true if loading was successful, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual bool LoadFromFile(
        const FilePath& vertexPath,
        const FilePath& geometryPath,
        const FilePath& fragmentPath
    ) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Load shader from memory
    ///
    /// \param shader shader source code
    /// \param type shader type
    ///
    /// \return true if loading was successful, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual bool LoadFromMemory(const FString& shader, EShaderType type) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Load shader from memory
    ///
    /// \param vertex vertex shader source code
    /// \param fragment fragment shader source code
    ///
    /// \return true if loading was successful, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual bool
        LoadFromMemory(const FString& vertex, const FString& fragment) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Load shader from memory
    ///
    /// \param vertex vertex shader source code
    /// \param geometry geometry shader source code
    /// \param fragment fragment shader source code
    ///
    /// \return true if loading was successful, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual bool LoadFromMemory(
        const FString& vertex, const FString& geometry, const FString& fragment
    ) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Load shader from bytes
    ///
    /// \param shader shader source code in bytes
    /// \param type shader type
    ///
    /// \return true if loading was successful, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual bool
        LoadFromBytes(const TVector<Byte>& shader, EShaderType type) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Load shader from bytes
    ///
    /// \param vertex vertex shader source code in bytes
    /// \param fragment fragment shader source code in bytes
    ///
    /// \return true if loading was successful, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual bool LoadFromBytes(
        const TVector<Byte>& vertex, const TVector<Byte>& fragment
    ) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Load shader from bytes
    ///
    /// \param vertex vertex shader source code in bytes
    /// \param geometry geometry shader source code in bytes
    /// \param fragment fragment shader source code in bytes
    ///
    /// \return true if loading was successful, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual bool LoadFromBytes(
        const TVector<Byte>& vertex,
        const TVector<Byte>& geometry,
        const TVector<Byte>& fragment
    ) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Load shader from assets
    ///
    /// \param asset shader source code in asset
    /// \param type shader type
    ///
    /// \return true if loading was successful, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual bool LoadFromAsset(const UAsset& asset, EShaderType type) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Load shader from assets
    ///
    /// \param vertex vertex shader source code in asset
    /// \param fragment fragment shader source code in asset
    ///
    /// \return true if loading was successful, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual bool
        LoadFromAsset(const UAsset& vertex, const UAsset& fragment) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Load shader from assets
    ///
    /// \param vertex vertex shader source code in asset
    /// \param geometry geometry shader source code in asset
    /// \param fragment fragment shader source code in asset
    ///
    /// \return true if loading was successful, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual bool LoadFromAsset(
        const UAsset& vertex, const UAsset& geometry, const UAsset& fragment
    ) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Uniform setters
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void SetUniform(const FString& name, float value) = 0;
    virtual void SetUniform(const FString& name, int value) = 0;
    virtual void SetUniform(const FString& name, bool value) = 0;
    virtual void SetUniform(const FString& name, const FVector2f& value) = 0;
    virtual void SetUniform(const FString& name, const FVector2i& value) = 0;
    virtual void SetUniform(const FString& name, const FVector2b& value) = 0;
    virtual void SetUniform(const FString& name, const FVector3f& value) = 0;
    virtual void SetUniform(const FString& name, const FVector3i& value) = 0;
    virtual void SetUniform(const FString& name, const FVector3b& value) = 0;
    virtual void SetUniform(const FString& name, const FVector4f& value) = 0;
    virtual void SetUniform(const FString& name, const FVector4i& value) = 0;
    virtual void SetUniform(const FString& name, const FVector4b& value) = 0;
    virtual void SetUniform(const FString& name, const FMatrix3x3f& value) = 0;
    virtual void SetUniform(const FString& name, const FMatrix4x4f& value) = 0;
    virtual void
        SetUniform(const FString& name, const FLinearColor& value) = 0;
    virtual void SetUniform(const FString& name, const FColor& value) = 0;
    virtual void SetUniform(const FString& name, const ITexture& value) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Uniform array setters
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void SetUniformArray(
        const FString& name, const float* values, SizeT length
    ) = 0;
    virtual void
        SetUniformArray(const FString& name, const TVector<float>& values) = 0;
    virtual void SetUniformArray(
        const FString& name, const FVector2f* values, SizeT length
    ) = 0;
    virtual void SetUniformArray(
        const FString& name, const TVector<FVector2f>& values
    ) = 0;
    virtual void SetUniformArray(
        const FString& name, const FVector3f* values, SizeT length
    ) = 0;
    virtual void SetUniformArray(
        const FString& name, const TVector<FVector3f>& values
    ) = 0;
    virtual void SetUniformArray(
        const FString& name, const FVector4f* values, SizeT length
    ) = 0;
    virtual void SetUniformArray(
        const FString& name, const TVector<FVector4f>& values
    ) = 0;
    virtual void SetUniformArray(
        const FString& name, const FMatrix3x3f* values, SizeT length
    ) = 0;
    virtual void SetUniformArray(
        const FString& name, const TVector<FMatrix3x3f>& values
    ) = 0;
    virtual void SetUniformArray(
        const FString& name, const FMatrix4x4f* values, SizeT length
    ) = 0;
    virtual void SetUniformArray(
        const FString& name, const TVector<FMatrix4x4f>& values
    ) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get native handle
    ///
    /// \return native handle (e.g., OpenGL shader program ID)
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual UInt32 GetNativeHandle(void) const = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get native pointer
    ///
    /// \return native pointer (e.g., DirectX shader interface, SFML shader)
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void* GetNativePointer(void) const = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Bind the shader
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void Bind(void) const = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Unbind the shader
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void Unbind(void) const = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Check if the shader is valid
    ///
    /// \return true if the shader is valid, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual bool IsValid(void) const = 0;
};

}   // namespace tkd
