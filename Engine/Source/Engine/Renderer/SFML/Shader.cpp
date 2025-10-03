///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Renderer/SFML/Shader.hpp>
#include <Engine/Renderer/SFML/Utils.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::SFML
///////////////////////////////////////////////////////////////////////////////
namespace tkd::SFML
{

#if TKD_ENGINE_CLIENT

///////////////////////////////////////////////////////////////////////////////
Shader::Shader(void)
    : m_shader(nullptr)
{}

///////////////////////////////////////////////////////////////////////////////
bool Shader::LoadFromFile(
    const FilePath& vertexPath,
    const FilePath& fragmentPath,
    const FilePath& geometryPath
)
{
    if (!m_shader) { m_shader = std::make_unique<sf::Shader>(); }
    return geometryPath.empty()
               ? m_shader->loadFromFile(
                     vertexPath.string(), fragmentPath.string()
                 )
               : m_shader->loadFromFile(
                     vertexPath.string(),
                     fragmentPath.string(),
                     geometryPath.string()
                 );
}

///////////////////////////////////////////////////////////////////////////////
bool Shader::LoadFromSource(
    const FString& vertexSource,
    const FString& fragmentSource,
    const FString& geometrySource
)
{
    if (!m_shader) { m_shader = std::make_unique<sf::Shader>(); }
    return geometrySource.IsEmpty()
               ? m_shader->loadFromMemory(
                     vertexSource.CStr(), fragmentSource.CStr()
                 )
               : m_shader->loadFromMemory(
                     vertexSource.CStr(),
                     fragmentSource.CStr(),
                     geometrySource.CStr()
                 );
}

///////////////////////////////////////////////////////////////////////////////
void Shader::Bind(void) const
{
    if (m_shader) { sf::Shader::bind(m_shader.get()); }
}

///////////////////////////////////////////////////////////////////////////////
void Shader::Unbind(void) const
{
    if (m_shader) { sf::Shader::bind(nullptr); }
}

///////////////////////////////////////////////////////////////////////////////
bool Shader::IsValid(void) const { return m_shader != nullptr; }

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniform(const FString& name, Int32 value)
{
    if (m_shader) { m_shader->setUniform(name.CStr(), value); }
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniform(const FString& name, float value)
{
    if (m_shader) { m_shader->setUniform(name.CStr(), value); }
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniform(const FString& name, const FVector2f& value)
{
    if (m_shader) { m_shader->setUniform(name.CStr(), Utils::Convert(value)); }
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniform(const FString& name, const FVector3f& value)
{
    if (m_shader) { m_shader->setUniform(name.CStr(), Utils::Convert(value)); }
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniform(const FString& name, const FVector4f& value)
{
    if (m_shader)
    {
        m_shader->setUniform(
            name.CStr(), sf::Glsl::Vec4(value.x, value.y, value.z, value.w)
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniform(const FString& name, const FColor& value)
{
    if (m_shader)
    {
        m_shader->setUniform(
            name.CStr(), sf::Glsl::Vec4(value.r, value.g, value.b, value.a)
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniform(const FString& name, const FLinearColor& value)
{
    SetUniform(name, FColor(value));
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniform(const FString& name, const FMatrix3x3& value)
{
    if (m_shader) { m_shader->setUniform(name.CStr(), Utils::Convert(value)); }
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniform(const FString& name, const FMatrix4x4& value)
{
    if (m_shader) { m_shader->setUniform(name.CStr(), Utils::Convert(value)); }
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniform(const FString& name, const ITexture* texture)
{
    if (m_shader)
    {
        m_shader->setUniform(
            name.CStr(),
            reinterpret_cast<sf::Texture*>(texture->GetNativeHandle())
        );
    }
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
    return m_shader ? static_cast<void*>(m_shader.get()) : nullptr;
}

///////////////////////////////////////////////////////////////////////////////
sf::Shader* Shader::GetSFMLShader(void) const { return m_shader.get(); }

#endif

}   // namespace tkd::SFML
