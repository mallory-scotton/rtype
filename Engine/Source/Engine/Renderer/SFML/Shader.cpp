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
    , m_isLoaded(false)
{}

///////////////////////////////////////////////////////////////////////////////
Shader::Shader(const FilePath& filePath, EShaderType type)
    : m_shader(nullptr)
    , m_isLoaded(false)
{
    if (!this->LoadFromFile(filePath, type))
    {
        throw std::runtime_error(
            "Failed to load shader from file: " + filePath.string()
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
Shader::Shader(const FilePath& vertexPath, const FilePath& fragmentPath)
    : m_shader(nullptr)
    , m_isLoaded(false)
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
Shader::Shader(
    const FilePath& vertexPath,
    const FilePath& geometryPath,
    const FilePath& fragmentPath
)
    : m_shader(nullptr)
    , m_isLoaded(false)
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
Shader::Shader(const FString& shader, EShaderType type)
    : m_shader(nullptr)
    , m_isLoaded(false)
{
    if (!this->LoadFromMemory(shader, type))
    {
        throw std::runtime_error("Failed to load shader from memory");
    }
}

///////////////////////////////////////////////////////////////////////////////
Shader::Shader(const FString& vertex, const FString& fragment)
    : m_shader(nullptr)
    , m_isLoaded(false)
{
    if (!this->LoadFromMemory(vertex, fragment))
    {
        throw std::runtime_error("Failed to load shader from memory");
    }
}

///////////////////////////////////////////////////////////////////////////////
Shader::Shader(
    const FString& vertex, const FString& geometry, const FString& fragment
)
    : m_shader(nullptr)
    , m_isLoaded(false)
{
    if (!this->LoadFromMemory(vertex, geometry, fragment))
    {
        throw std::runtime_error("Failed to load shader from memory");
    }
}

///////////////////////////////////////////////////////////////////////////////
Shader::Shader(const TVector<Byte>& shader, EShaderType type)
    : m_shader(nullptr)
    , m_isLoaded(false)
{
    if (!this->LoadFromBytes(shader, type))
    {
        throw std::runtime_error("Failed to load shader from bytes");
    }
}

///////////////////////////////////////////////////////////////////////////////
Shader::Shader(const TVector<Byte>& vertex, const TVector<Byte>& fragment)
    : m_shader(nullptr)
    , m_isLoaded(false)
{
    if (!this->LoadFromBytes(vertex, fragment))
    {
        throw std::runtime_error("Failed to load shader from bytes");
    }
}

///////////////////////////////////////////////////////////////////////////////
Shader::Shader(
    const TVector<Byte>& vertex,
    const TVector<Byte>& geometry,
    const TVector<Byte>& fragment
)
    : m_shader(nullptr)
    , m_isLoaded(false)
{
    if (!this->LoadFromBytes(vertex, geometry, fragment))
    {
        throw std::runtime_error("Failed to load shader from bytes");
    }
}

///////////////////////////////////////////////////////////////////////////////
Shader::Shader(const UAsset& asset, EShaderType type)
    : m_shader(nullptr)
    , m_isLoaded(false)
{
    if (!this->LoadFromAsset(asset, type))
    {
        throw std::runtime_error("Failed to load shader from asset");
    }
}

///////////////////////////////////////////////////////////////////////////////
Shader::Shader(const UAsset& vertex, const UAsset& fragment)
    : m_shader(nullptr)
    , m_isLoaded(false)
{
    if (!this->LoadFromAsset(vertex, fragment))
    {
        throw std::runtime_error("Failed to load shader from assets");
    }
}

///////////////////////////////////////////////////////////////////////////////
Shader::Shader(
    const UAsset& vertex, const UAsset& geometry, const UAsset& fragment
)
    : m_shader(nullptr)
    , m_isLoaded(false)
{
    if (!this->LoadFromAsset(vertex, geometry, fragment))
    {
        throw std::runtime_error("Failed to load shader from assets");
    }
}

///////////////////////////////////////////////////////////////////////////////
bool Shader::LoadFromFile(const FilePath& filepath, EShaderType type)
{
    if (!m_shader) { m_shader = std::make_unique<sf::Shader>(); }
    m_isLoaded =
        m_shader->loadFromFile(filepath.string(), Utils::Convert(type));
    return m_isLoaded;
}

///////////////////////////////////////////////////////////////////////////////
bool Shader::LoadFromFile(
    const FilePath& vertexPath, const FilePath& fragmentPath
)
{
    if (!m_shader) { m_shader = std::make_unique<sf::Shader>(); }
    m_isLoaded =
        m_shader->loadFromFile(vertexPath.string(), fragmentPath.string());
    return m_isLoaded;
}

///////////////////////////////////////////////////////////////////////////////
bool Shader::LoadFromFile(
    const FilePath& vertexPath,
    const FilePath& geometryPath,
    const FilePath& fragmentPath
)
{
    if (!m_shader) { m_shader = std::make_unique<sf::Shader>(); }
    m_isLoaded = m_shader->loadFromFile(
        vertexPath.string(), geometryPath.string(), fragmentPath.string()
    );
    return m_isLoaded;
}

///////////////////////////////////////////////////////////////////////////////
bool Shader::LoadFromMemory(const FString& shader, EShaderType type)
{
    if (!m_shader) { m_shader = std::make_unique<sf::Shader>(); }
    m_isLoaded = m_shader->loadFromMemory(shader, Utils::Convert(type));
    return m_isLoaded;
}

///////////////////////////////////////////////////////////////////////////////
bool Shader::LoadFromMemory(const FString& vertex, const FString& fragment)
{
    if (!m_shader) { m_shader = std::make_unique<sf::Shader>(); }
    m_isLoaded = m_shader->loadFromMemory(vertex, fragment);
    return m_isLoaded;
}

///////////////////////////////////////////////////////////////////////////////
bool Shader::LoadFromMemory(
    const FString& vertex, const FString& geometry, const FString& fragment
)
{
    if (!m_shader) { m_shader = std::make_unique<sf::Shader>(); }
    m_isLoaded = m_shader->loadFromMemory(vertex, geometry, fragment);
    return m_isLoaded;
}

///////////////////////////////////////////////////////////////////////////////
bool Shader::LoadFromBytes(const TVector<Byte>& shader, EShaderType type)
{
    if (!m_shader) { m_shader = std::make_unique<sf::Shader>(); }
    m_isLoaded = m_shader->loadFromMemory(
        std::string(
            reinterpret_cast<const char*>(shader.Data()), shader.Size()
        ),
        Utils::Convert(type)
    );
    return m_isLoaded;
}

///////////////////////////////////////////////////////////////////////////////
bool Shader::LoadFromBytes(
    const TVector<Byte>& vertex, const TVector<Byte>& fragment
)
{
    if (!m_shader) { m_shader = std::make_unique<sf::Shader>(); }
    m_isLoaded = m_shader->loadFromMemory(
        std::string(
            reinterpret_cast<const char*>(vertex.Data()), vertex.Size()
        ),
        std::string(
            reinterpret_cast<const char*>(fragment.Data()), fragment.Size()
        )
    );
    return m_isLoaded;
}

///////////////////////////////////////////////////////////////////////////////
bool Shader::LoadFromBytes(
    const TVector<Byte>& vertex,
    const TVector<Byte>& geometry,
    const TVector<Byte>& fragment
)
{
    if (!m_shader) { m_shader = std::make_unique<sf::Shader>(); }
    m_isLoaded = m_shader->loadFromMemory(
        std::string(
            reinterpret_cast<const char*>(vertex.Data()), vertex.Size()
        ),
        std::string(
            reinterpret_cast<const char*>(geometry.Data()), geometry.Size()
        ),
        std::string(
            reinterpret_cast<const char*>(fragment.Data()), fragment.Size()
        )
    );
    return m_isLoaded;
}

///////////////////////////////////////////////////////////////////////////////
bool Shader::LoadFromAsset(const UAsset& asset, EShaderType type)
{
    if (!m_shader) { m_shader = std::make_unique<sf::Shader>(); }
    const TVector<Byte>& assetData = asset.GetData();
    m_isLoaded = m_shader->loadFromMemory(
        std::string(
            reinterpret_cast<const char*>(assetData.Data()), assetData.Size()
        ),
        Utils::Convert(type)
    );
    return m_isLoaded;
}

///////////////////////////////////////////////////////////////////////////////
bool Shader::LoadFromAsset(const UAsset& vertex, const UAsset& fragment)
{
    if (!m_shader) { m_shader = std::make_unique<sf::Shader>(); }
    const TVector<Byte>& vertexData = vertex.GetData();
    const TVector<Byte>& fragmentData = fragment.GetData();
    m_isLoaded = m_shader->loadFromMemory(
        std::string(
            reinterpret_cast<const char*>(vertexData.Data()), vertexData.Size()
        ),
        std::string(
            reinterpret_cast<const char*>(fragmentData.Data()),
            fragmentData.Size()
        )
    );
    return m_isLoaded;
}

///////////////////////////////////////////////////////////////////////////////
bool Shader::LoadFromAsset(
    const UAsset& vertex, const UAsset& geometry, const UAsset& fragment
)
{
    if (!m_shader) { m_shader = std::make_unique<sf::Shader>(); }
    const TVector<Byte>& vertexData = vertex.GetData();
    const TVector<Byte>& geometryData = geometry.GetData();
    const TVector<Byte>& fragmentData = fragment.GetData();
    m_isLoaded = m_shader->loadFromMemory(
        std::string(
            reinterpret_cast<const char*>(vertexData.Data()), vertexData.Size()
        ),
        std::string(
            reinterpret_cast<const char*>(geometryData.Data()),
            geometryData.Size()
        ),
        std::string(
            reinterpret_cast<const char*>(fragmentData.Data()),
            fragmentData.Size()
        )
    );
    return m_isLoaded;
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniform(const FString& name, float value)
{
    if (m_shader) { m_shader->setUniform(name.CStr(), value); }
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniform(const FString& name, int value)
{
    if (m_shader) { m_shader->setUniform(name.CStr(), value); }
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniform(const FString& name, bool value)
{
    if (m_shader) { m_shader->setUniform(name.CStr(), value); }
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniform(const FString& name, const FVector2f& value)
{
    if (m_shader) { m_shader->setUniform(name.CStr(), Utils::Convert(value)); }
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniform(const FString& name, const FVector2i& value)
{
    if (m_shader) { m_shader->setUniform(name.CStr(), Utils::Convert(value)); }
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniform(const FString& name, const FVector2b& value)
{
    if (m_shader) { m_shader->setUniform(name.CStr(), Utils::Convert(value)); }
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniform(const FString& name, const FVector3f& value)
{
    if (m_shader) { m_shader->setUniform(name.CStr(), Utils::Convert(value)); }
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniform(const FString& name, const FVector3i& value)
{
    if (m_shader) { m_shader->setUniform(name.CStr(), Utils::Convert(value)); }
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniform(const FString& name, const FVector3b& value)
{
    if (m_shader) { m_shader->setUniform(name.CStr(), Utils::Convert(value)); }
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniform(const FString& name, const FVector4f& value)
{
    if (m_shader) { m_shader->setUniform(name.CStr(), Utils::Convert(value)); }
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniform(const FString& name, const FVector4i& value)
{
    if (m_shader) { m_shader->setUniform(name.CStr(), Utils::Convert(value)); }
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniform(const FString& name, const FVector4b& value)
{
    if (m_shader) { m_shader->setUniform(name.CStr(), Utils::Convert(value)); }
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniform(const FString& name, const FMatrix3x3f& value)
{
    if (m_shader) { m_shader->setUniform(name.CStr(), Utils::Convert(value)); }
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniform(const FString& name, const FMatrix4x4f& value)
{
    if (m_shader) { m_shader->setUniform(name.CStr(), Utils::Convert(value)); }
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniform(const FString& name, const FLinearColor& value)
{
    SetUniform(name, FColor(value));
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
void Shader::SetUniform(const FString& name, const ITexture& value)
{
    if (m_shader)
    {
        m_shader->setUniform(
            name.CStr(),
            reinterpret_cast<sf::Texture*>(value.GetNativePointer())
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniformArray(
    const FString& name, const float* values, SizeT length
)
{
    if (m_shader) { m_shader->setUniformArray(name.CStr(), values, length); }
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniformArray(const FString& name, const TVector<float>& values)
{
    SetUniformArray(name, values.Data(), values.Size());
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniformArray(
    const FString& name, const FVector2f* values, SizeT length
)
{
    TVector<sf::Glsl::Vec2> sfValues;
    sfValues.Reserve(length);

    for (SizeT i = 0; i < length; ++i)
    {
        sfValues.PushBack(Utils::Convert(values[i]));
    }

    if (m_shader)
    {
        m_shader->setUniformArray(name.CStr(), sfValues.Data(), length);
    }
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniformArray(
    const FString& name, const TVector<FVector2f>& values
)
{
    SetUniformArray(name, values.Data(), values.Size());
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniformArray(
    const FString& name, const FVector3f* values, SizeT length
)
{
    TVector<sf::Glsl::Vec3> sfValues;
    sfValues.Reserve(length);

    for (SizeT i = 0; i < length; ++i)
    {
        sfValues.PushBack(Utils::Convert(values[i]));
    }

    if (m_shader)
    {
        m_shader->setUniformArray(name.CStr(), sfValues.Data(), length);
    }
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniformArray(
    const FString& name, const TVector<FVector3f>& values
)
{
    SetUniformArray(name, values.Data(), values.Size());
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniformArray(
    const FString& name, const FVector4f* values, SizeT length
)
{
    TVector<sf::Glsl::Vec4> sfValues;
    sfValues.Reserve(length);

    for (SizeT i = 0; i < length; ++i)
    {
        sfValues.PushBack(Utils::Convert(values[i]));
    }

    if (m_shader)
    {
        m_shader->setUniformArray(name.CStr(), sfValues.Data(), length);
    }
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniformArray(
    const FString& name, const TVector<FVector4f>& values
)
{
    SetUniformArray(name, values.Data(), values.Size());
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniformArray(
    const FString& name, const FMatrix3x3f* values, SizeT length
)
{
    TVector<sf::Glsl::Mat3> sfValues;
    sfValues.Reserve(length);

    for (SizeT i = 0; i < length; ++i)
    {
        sfValues.PushBack(Utils::Convert(values[i]));
    }

    if (m_shader)
    {
        m_shader->setUniformArray(name.CStr(), sfValues.Data(), length);
    }
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniformArray(
    const FString& name, const TVector<FMatrix3x3f>& values
)
{
    SetUniformArray(name, values.Data(), values.Size());
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniformArray(
    const FString& name, const FMatrix4x4f* values, SizeT length
)
{
    TVector<sf::Glsl::Mat4> sfValues;
    sfValues.Reserve(length);

    for (SizeT i = 0; i < length; ++i)
    {
        sfValues.PushBack(Utils::Convert(values[i]));
    }

    if (m_shader)
    {
        m_shader->setUniformArray(name.CStr(), sfValues.Data(), length);
    }
}

///////////////////////////////////////////////////////////////////////////////
void Shader::SetUniformArray(
    const FString& name, const TVector<FMatrix4x4f>& values
)
{
    SetUniformArray(name, values.Data(), values.Size());
}

///////////////////////////////////////////////////////////////////////////////
UInt32 Shader::GetNativeHandle(void) const
{
    return m_shader ? m_shader->getNativeHandle() : 0;
}

///////////////////////////////////////////////////////////////////////////////
void* Shader::GetNativePointer(void) const
{
    return reinterpret_cast<void*>(m_shader.get());
}

///////////////////////////////////////////////////////////////////////////////
void Shader::Bind(void) const
{
    if (m_shader) { sf::Shader::bind(m_shader.get()); }
}

///////////////////////////////////////////////////////////////////////////////
void Shader::Unbind(void) const { sf::Shader::bind(nullptr); }

///////////////////////////////////////////////////////////////////////////////
bool Shader::IsValid(void) const { return m_shader != nullptr && m_isLoaded; }

#endif

}   // namespace tkd::SFML
