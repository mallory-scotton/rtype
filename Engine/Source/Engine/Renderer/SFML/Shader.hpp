///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Config.hpp>
#include <Engine/Renderer/IShader.hpp>
#include <memory>
#if TKD_ENGINE_CLIENT
    #include <SFML/Graphics.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::SFML
///////////////////////////////////////////////////////////////////////////////
namespace tkd::SFML
{

///////////////////////////////////////////////////////////////////////////////
/// \brief SFML implementation of the shader interface
///
///////////////////////////////////////////////////////////////////////////////
class Shader : public IShader
{
private:
    ///////////////////////////////////////////////////////////////////////////
    // Class Members
    ///////////////////////////////////////////////////////////////////////////
    std::unique_ptr<sf::Shader> m_shader;   //<! SFML shader
    bool m_isLoaded;                        //<! Is the shader loaded

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor
    ///
    ///////////////////////////////////////////////////////////////////////////
    Shader(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Destructor
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual ~Shader() = default;

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
    virtual bool
        LoadFromFile(const FilePath& filePath, EShaderType type) override;

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
    ) override;

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
    ) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Load shader from memory
    ///
    /// \param shader shader source code
    /// \param type shader type
    ///
    /// \return true if loading was successful, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual bool
        LoadFromMemory(const FString& shader, EShaderType type) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Load shader from memory
    ///
    /// \param vertex vertex shader source code
    /// \param fragment fragment shader source code
    ///
    /// \return true if loading was successful, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual bool LoadFromMemory(const FString& vertex, const FString& fragment)
        override;

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
    ) override;

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
        LoadFromBytes(const TVector<Byte>& shader, EShaderType type) override;

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
    ) override;

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
    ) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Load shader from assets
    ///
    /// \param asset shader source code in asset
    /// \param type shader type
    ///
    /// \return true if loading was successful, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual bool LoadFromAsset(const UAsset& asset, EShaderType type) override;

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
        LoadFromAsset(const UAsset& vertex, const UAsset& fragment) override;

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
    ) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Uniform setters
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void SetUniform(const FString& name, float value) override;
    virtual void SetUniform(const FString& name, int value) override;
    virtual void SetUniform(const FString& name, bool value) override;
    virtual void
        SetUniform(const FString& name, const FVector2f& value) override;
    virtual void
        SetUniform(const FString& name, const FVector2i& value) override;
    virtual void
        SetUniform(const FString& name, const FVector2b& value) override;
    virtual void
        SetUniform(const FString& name, const FVector3f& value) override;
    virtual void
        SetUniform(const FString& name, const FVector3i& value) override;
    virtual void
        SetUniform(const FString& name, const FVector3b& value) override;
    virtual void
        SetUniform(const FString& name, const FVector4f& value) override;
    virtual void
        SetUniform(const FString& name, const FVector4i& value) override;
    virtual void
        SetUniform(const FString& name, const FVector4b& value) override;
    virtual void
        SetUniform(const FString& name, const FMatrix3x3f& value) override;
    virtual void
        SetUniform(const FString& name, const FMatrix4x4f& value) override;
    virtual void
        SetUniform(const FString& name, const FLinearColor& value) override;
    virtual void SetUniform(const FString& name, const FColor& value) override;
    virtual void
        SetUniform(const FString& name, const ITexture& value) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Uniform array setters
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void SetUniformArray(
        const FString& name, const float* values, SizeT length
    ) override;
    virtual void SetUniformArray(
        const FString& name, const TVector<float>& values
    ) override;
    virtual void SetUniformArray(
        const FString& name, const FVector2f* values, SizeT length
    ) override;
    virtual void SetUniformArray(
        const FString& name, const TVector<FVector2f>& values
    ) override;
    virtual void SetUniformArray(
        const FString& name, const FVector3f* values, SizeT length
    ) override;
    virtual void SetUniformArray(
        const FString& name, const TVector<FVector3f>& values
    ) override;
    virtual void SetUniformArray(
        const FString& name, const FVector4f* values, SizeT length
    ) override;
    virtual void SetUniformArray(
        const FString& name, const TVector<FVector4f>& values
    ) override;
    virtual void SetUniformArray(
        const FString& name, const FMatrix3x3f* values, SizeT length
    ) override;
    virtual void SetUniformArray(
        const FString& name, const TVector<FMatrix3x3f>& values
    ) override;
    virtual void SetUniformArray(
        const FString& name, const FMatrix4x4f* values, SizeT length
    ) override;
    virtual void SetUniformArray(
        const FString& name, const TVector<FMatrix4x4f>& values
    ) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get native handle
    ///
    /// \return native handle (e.g., OpenGL shader program ID)
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual UInt32 GetNativeHandle(void) const override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get native pointer
    ///
    /// \return native pointer (e.g., DirectX shader interface, SFML shader)
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void* GetNativePointer(void) const override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Bind the shader
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void Bind(void) const override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Unbind the shader
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void Unbind(void) const override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Check if the shader is valid
    ///
    /// \return true if the shader is valid, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual bool IsValid(void) const override;
};

}   // namespace tkd::SFML
#endif
