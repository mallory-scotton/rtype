///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Renderer/SFML/Texture.hpp>
#include <Engine/Renderer/SFML/Utils.hpp>
#include <stdexcept>
#if TKD_ENGINE_CLIENT
    #include <GL/glu.h>
    #include <SFML/OpenGL.hpp>
#endif

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::SFML
///////////////////////////////////////////////////////////////////////////////
namespace tkd::SFML
{

#if TKD_ENGINE_CLIENT

///////////////////////////////////////////////////////////////////////////////
Texture::Texture(void)
    : m_texture(nullptr)
{}

///////////////////////////////////////////////////////////////////////////////
Texture::Texture(const Texture& other)
    : m_texture(
          other.m_texture ? std::make_unique<sf::Texture>(*other.m_texture)
                          : nullptr
      )
{}

///////////////////////////////////////////////////////////////////////////////
Texture::Texture(const FilePath& filepath, const FRectanglei& area)
    : m_texture(std::make_unique<sf::Texture>())
{
    if (!m_texture->loadFromFile(filepath.string(), Utils::Convert(area)))
    {
        throw std::runtime_error(
            "Failed to load texture from file: " + filepath.string()
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
Texture::Texture(const void* data, SizeT size, const FRectanglei& area)
    : m_texture(std::make_unique<sf::Texture>())
{
    if (!m_texture->loadFromMemory(data, size, Utils::Convert(area)))
    {
        throw std::runtime_error("Failed to load texture from memory");
    }
}

///////////////////////////////////////////////////////////////////////////////
Texture::Texture(const TVector<Byte>& bytes, const FRectanglei& area)
    : m_texture(std::make_unique<sf::Texture>())
{
    if (!m_texture->loadFromMemory(
            bytes.Data(), bytes.Size(), Utils::Convert(area)
        ))
    {
        throw std::runtime_error("Failed to load texture from byte array");
    }
}

///////////////////////////////////////////////////////////////////////////////
Texture::Texture(const UAsset& asset, const FRectanglei& area)
    : m_texture(std::make_unique<sf::Texture>())
{
    const TVector<Byte>& data = asset.GetData();
    if (!m_texture->loadFromMemory(
            data.Data(), data.Size(), Utils::Convert(area)
        ))
    {
        throw std::runtime_error("Failed to load texture from asset");
    }
}

///////////////////////////////////////////////////////////////////////////////
bool Texture::LoadFromFile(const FilePath& filepath, const FRectanglei& area)
{
    if (!m_texture) { m_texture = std::make_unique<sf::Texture>(); }
    return m_texture->loadFromFile(filepath.string(), Utils::Convert(area));
}

///////////////////////////////////////////////////////////////////////////////
bool Texture::LoadFromMemory(
    const void* data, SizeT size, const FRectanglei& area
)
{
    if (!m_texture) { m_texture = std::make_unique<sf::Texture>(); }
    return m_texture->loadFromMemory(data, size, Utils::Convert(area));
}

///////////////////////////////////////////////////////////////////////////////
bool Texture::LoadFromBytes(
    const TVector<Byte>& bytes, const FRectanglei& area
)
{
    if (!m_texture) { m_texture = std::make_unique<sf::Texture>(); }
    return m_texture->loadFromMemory(
        bytes.Data(), bytes.Size(), Utils::Convert(area)
    );
}

///////////////////////////////////////////////////////////////////////////////
bool Texture::LoadFromAsset(const UAsset& asset, const FRectanglei& area)
{
    if (!m_texture) { m_texture = std::make_unique<sf::Texture>(); }
    const TVector<Byte>& data = asset.GetData();
    return m_texture->loadFromMemory(
        data.Data(), data.Size(), Utils::Convert(area)
    );
}

///////////////////////////////////////////////////////////////////////////////
bool Texture::Create(UInt32 width, UInt32 height)
{
    if (!m_texture) { m_texture = std::make_unique<sf::Texture>(); }
    return m_texture->create(width, height);
}

///////////////////////////////////////////////////////////////////////////////
void Texture::GenerateMipmaps(void)
{
    if (m_texture) { m_texture->generateMipmap(); }
}

///////////////////////////////////////////////////////////////////////////////
UInt32 Texture::GetWidth(void) const
{
    return m_texture ? m_texture->getSize().x : 0;
}

///////////////////////////////////////////////////////////////////////////////
UInt32 Texture::GetHeight(void) const
{
    return m_texture ? m_texture->getSize().y : 0;
}

///////////////////////////////////////////////////////////////////////////////
FVector2u Texture::GetSize(void) const
{
    return m_texture ? Utils::Convert(m_texture->getSize()) : FVector2u::Zero;
}

///////////////////////////////////////////////////////////////////////////////
bool Texture::IsValid(void) const
{
    return m_texture != nullptr && m_texture->getSize().x > 0 &&
           m_texture->getSize().y > 0;
}

///////////////////////////////////////////////////////////////////////////////
UInt32 Texture::GetNativeHandle(void) const
{
    return m_texture ? m_texture->getNativeHandle() : 0;
}

///////////////////////////////////////////////////////////////////////////////
void* Texture::GetNativePointer(void) const
{
    return reinterpret_cast<void*>(m_texture ? m_texture.get() : nullptr);
}

///////////////////////////////////////////////////////////////////////////////
void Texture::Update(const UInt8* pixels)
{
    if (m_texture) { m_texture->update(pixels); }
}

///////////////////////////////////////////////////////////////////////////////
void Texture::Update(
    const UInt8* pixels, UInt32 width, UInt32 height, UInt32 x, UInt32 y
)
{
    if (m_texture) { m_texture->update(pixels, width, height, x, y); }
}

///////////////////////////////////////////////////////////////////////////////
void Texture::Update(const TVector<UInt8>& pixels)
{
    if (m_texture) { m_texture->update(pixels.Data()); }
}

///////////////////////////////////////////////////////////////////////////////
void Texture::Update(
    const TVector<UInt8>& pixels,
    UInt32 width,
    UInt32 height,
    UInt32 x,
    UInt32 y
)
{
    if (m_texture) { m_texture->update(pixels.Data(), width, height, x, y); }
}

///////////////////////////////////////////////////////////////////////////////
void Texture::Update(const TVector<FColor>& pixels)
{
    if (m_texture)
    {
        m_texture->update(reinterpret_cast<const UInt8*>(pixels.Data()));
    }
}

///////////////////////////////////////////////////////////////////////////////
void Texture::Update(
    const TVector<FColor>& pixels,
    UInt32 width,
    UInt32 height,
    UInt32 x,
    UInt32 y
)
{
    if (m_texture)
    {
        m_texture->update(
            reinterpret_cast<const UInt8*>(pixels.Data()), width, height, x, y
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
void Texture::Update(const TVector<FLinearColor>& pixels)
{
    TVector<FColor> convertedPixels;
    convertedPixels.Reserve(pixels.Size());

    for (const auto& linearColor: pixels)
    {
        convertedPixels.EmplaceBack(FColor(linearColor));
    }

    Update(convertedPixels);
}

///////////////////////////////////////////////////////////////////////////////
void Texture::Update(
    const TVector<FLinearColor>& pixels,
    UInt32 width,
    UInt32 height,
    UInt32 x,
    UInt32 y
)
{
    TVector<FColor> convertedPixels;
    convertedPixels.Reserve(pixels.Size());

    for (const auto& linearColor: pixels)
    {
        convertedPixels.EmplaceBack(FColor(linearColor));
    }

    Update(convertedPixels, width, height, x, y);
}

///////////////////////////////////////////////////////////////////////////////
void Texture::Update(const ITexture& texture)
{
    const Texture* sfmlTexture = dynamic_cast<const Texture*>(&texture);
    if (m_texture && sfmlTexture && sfmlTexture->m_texture)
    {
        m_texture->update(*sfmlTexture->m_texture);
    }
}

///////////////////////////////////////////////////////////////////////////////
void Texture::Update(const ITexture& texture, UInt32 x, UInt32 y)
{
    const Texture* sfmlTexture = dynamic_cast<const Texture*>(&texture);
    if (m_texture && sfmlTexture && sfmlTexture->m_texture)
    {
        m_texture->update(*sfmlTexture->m_texture, x, y);
    }
}

///////////////////////////////////////////////////////////////////////////////
void Texture::SetSmooth(bool smooth)
{
    if (m_texture) { m_texture->setSmooth(smooth); }
}

///////////////////////////////////////////////////////////////////////////////
bool Texture::IsSmooth(void) const
{
    return m_texture ? m_texture->isSmooth() : false;
}

///////////////////////////////////////////////////////////////////////////////
void Texture::SetSRGB(bool sRGB)
{
    if (m_texture) { m_texture->setSrgb(sRGB); }
}

///////////////////////////////////////////////////////////////////////////////
bool Texture::IsSRGB(void) const
{
    return m_texture ? m_texture->isSrgb() : false;
}

///////////////////////////////////////////////////////////////////////////////
bool Texture::IsRepeated(void) const
{
    return m_texture ? m_texture->isRepeated() : false;
}

///////////////////////////////////////////////////////////////////////////////
void Texture::SetRepeated(bool repeated)
{
    if (m_texture) { m_texture->setRepeated(repeated); }
}

///////////////////////////////////////////////////////////////////////////////
void Texture::Swap(ITexture& other)
{
    Texture* sfmlTexture = dynamic_cast<Texture*>(&other);
    if (sfmlTexture) { std::swap(m_texture, sfmlTexture->m_texture); }
}

///////////////////////////////////////////////////////////////////////////////
void Texture::Bind(ETextureCoordinateType type) const
{
    if (m_texture)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, m_texture->getNativeHandle());
    }
}

///////////////////////////////////////////////////////////////////////////////
void Texture::Unbind(void) const
{
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}

#endif

}   // namespace tkd::SFML
