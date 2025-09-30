///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Renderer/SFML/Texture.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::SFML
///////////////////////////////////////////////////////////////////////////////
namespace tkd::SFML
{

#if TKD_ENGINE_CLIENT

///////////////////////////////////////////////////////////////////////////////
Texture::Texture(void)
    : m_texture(nullptr)
    , m_filter(ETextureFilter::Linear)
    , m_wrap(ETextureWrap::Repeat)
    , m_format(ETextureFormat::RGBA8)
{}

///////////////////////////////////////////////////////////////////////////////
bool Texture::LoadFromFile(const FilePath& filepath)
{
    TKD_UNUSED(filepath);
    // TODO: Implement texture loading from file
    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool Texture::CreateFromMemory(
    const void* data, UInt32 width, UInt32 height, ETextureFormat format
)
{
    TKD_UNUSED(data);
    TKD_UNUSED(width);
    TKD_UNUSED(height);
    TKD_UNUSED(format);
    // TODO: Implement texture creation from memory
    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool Texture::Create(UInt32 width, UInt32 height, ETextureFormat format)
{
    TKD_UNUSED(width);
    TKD_UNUSED(height);
    TKD_UNUSED(format);
    // TODO: Implement texture creation
    return true;
}

///////////////////////////////////////////////////////////////////////////////
void Texture::Update(
    const void* data, UInt32 x, UInt32 y, UInt32 width, UInt32 height
)
{
    TKD_UNUSED(data);
    TKD_UNUSED(x);
    TKD_UNUSED(y);
    TKD_UNUSED(width);
    TKD_UNUSED(height);
    // TODO: Implement texture update
}

///////////////////////////////////////////////////////////////////////////////
void Texture::SetFilter(ETextureFilter filter)
{
    TKD_UNUSED(filter);
    // TODO: Implement filter setting
}

///////////////////////////////////////////////////////////////////////////////
ETextureFilter Texture::GetFilter(void) const
{
    // TODO: Implement filter getting
    return m_filter;
}

///////////////////////////////////////////////////////////////////////////////
void Texture::SetWrap(ETextureWrap wrap)
{
    TKD_UNUSED(wrap);
    // TODO: Implement wrap setting
}

///////////////////////////////////////////////////////////////////////////////
ETextureWrap Texture::GetWrap(void) const
{
    // TODO: Implement wrap getting
    return m_wrap;
}

///////////////////////////////////////////////////////////////////////////////
void Texture::GenerateMipmaps(void)
{
    // TODO: Implement mipmap generation
}

///////////////////////////////////////////////////////////////////////////////
UInt32 Texture::GetWidth(void) const
{
    // TODO: Implement width getting
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
UInt32 Texture::GetHeight(void) const
{
    // TODO: Implement height getting
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
FVector2u Texture::GetSize(void) const
{
    // TODO: Implement size getting
    return FVector2u(0, 0);
}

///////////////////////////////////////////////////////////////////////////////
ETextureFormat Texture::GetFormat(void) const
{
    // TODO: Implement format getting
    return m_format;
}

///////////////////////////////////////////////////////////////////////////////
bool Texture::IsValid(void) const
{
    // TODO: Implement validity check
    return false;
}

///////////////////////////////////////////////////////////////////////////////
void* Texture::GetNativeHandle(void) const
{
    // TODO: Implement native handle retrieval
    return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
sf::Texture* Texture::GetSFMLTexture(void) const { return m_texture.get(); }

///////////////////////////////////////////////////////////////////////////////
void Texture::ApplySettings(void)
{
    // TODO: Implement settings application
}

#endif

}   // namespace tkd::SFML
