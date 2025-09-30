///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Renderer/SFML/Shader.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::SFML
///////////////////////////////////////////////////////////////////////////////
namespace tkd::SFML
{

///////////////////////////////////////////////////////////////////////////////
Shader::Shader(void)
    : m_shader(nullptr)
    , m_isValid(false)
{}

///////////////////////////////////////////////////////////////////////////////
bool Shader::LoadFromFile(
    const FilePath& vertexPath,
    const FilePath& fragmentPath,
    const FilePath& geometryPath
)
{
    TKD_UNUSED(vertexPath);
    TKD_UNUSED(fragmentPath);
    TKD_UNUSED(geometryPath);
    // TODO: Implement shader loading from files
    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool Shader::LoadFromSource(
    const FString& vertexSource,
    const FString& fragmentSource,
    const FString& geometrySource
)
{
    TKD_UNUSED(vertexSource);
    TKD_UNUSED(fragmentSource);
    TKD_UNUSED(geometrySource);
    // TODO: Implement shader loading from source
    return true;
}

///////////////////////////////////////////////////////////////////////////////
void Shader::Bind(void) const
{
    // TODO: Implement shader binding
}

///////////////////////////////////////////////////////////////////////////////
void Shader::Unbind(void) const
{
    // TODO: Implement shader unbinding
}

///////////////////////////////////////////////////////////////////////////////
bool Shader::IsValid(void) const
{
    // TODO: Implement validity check
    return m_isValid;
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniform(const FString& name, Int32 value)
{
    TKD_UNUSED(name);
    TKD_UNUSED(value);
    // TODO: Implement uniform setting
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniform(const FString& name, float value)
{
    TKD_UNUSED(name);
    TKD_UNUSED(value);
    // TODO: Implement uniform setting
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniform(const FString& name, const FVector2f& value)
{
    TKD_UNUSED(name);
    TKD_UNUSED(value);
    // TODO: Implement uniform setting
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniform(const FString& name, const FVector3f& value)
{
    TKD_UNUSED(name);
    TKD_UNUSED(value);
    // TODO: Implement uniform setting
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniform(const FString& name, const FVector4f& value)
{
    TKD_UNUSED(name);
    TKD_UNUSED(value);
    // TODO: Implement uniform setting
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniform(const FString& name, const FColor& value)
{
    TKD_UNUSED(name);
    TKD_UNUSED(value);
    // TODO: Implement uniform setting
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniform(const FString& name, const FMatrix3x3& value)
{
    TKD_UNUSED(name);
    TKD_UNUSED(value);
    // TODO: Implement uniform setting
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniform(const FString& name, const FMatrix4x4& value)
{
    TKD_UNUSED(name);
    TKD_UNUSED(value);
    // TODO: Implement uniform setting
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniform(
    const FString& name, const ITexture* texture, UInt32 slot
)
{
    TKD_UNUSED(name);
    TKD_UNUSED(texture);
    TKD_UNUSED(slot);
    // TODO: Implement texture uniform setting
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniformArray(
    const FString& name, const float* values, UInt32 count
)
{
    TKD_UNUSED(name);
    TKD_UNUSED(values);
    TKD_UNUSED(count);
    // TODO: Implement uniform array setting
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniformArray(
    const FString& name, const Int32* values, UInt32 count
)
{
    TKD_UNUSED(name);
    TKD_UNUSED(values);
    TKD_UNUSED(count);
    // TODO: Implement uniform array setting
}

///////////////////////////////////////////////////////////////////////////////
void* Shader::GetNativeHandle(void) const
{
    // TODO: Implement native handle retrieval
    return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
sf::Shader* Shader::GetSFMLShader(void) const { return m_shader.get(); }

}   // namespace tkd::SFML
