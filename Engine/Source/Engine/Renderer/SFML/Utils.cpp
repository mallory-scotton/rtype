///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Renderer/SFML/Utils.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::SFML
///////////////////////////////////////////////////////////////////////////////
namespace tkd::SFML
{

#if TKD_ENGINE_CLIENT

///////////////////////////////////////////////////////////////////////////////
sf::Color Utils::Convert(const FColor& color)
{
    return sf::Color(
        static_cast<sf::Uint8>(color.r * 255.f),
        static_cast<sf::Uint8>(color.g * 255.f),
        static_cast<sf::Uint8>(color.b * 255.f),
        static_cast<sf::Uint8>(color.a * 255.f)
    );
}

///////////////////////////////////////////////////////////////////////////////
sf::Color Utils::Convert(const FLinearColor& color)
{
    return sf::Color(
        static_cast<sf::Uint8>(color.r),
        static_cast<sf::Uint8>(color.g),
        static_cast<sf::Uint8>(color.b),
        static_cast<sf::Uint8>(color.a)
    );
}

///////////////////////////////////////////////////////////////////////////////
sf::View Utils::Convert(const FView& view)
{
    sf::View sfView;
    sfView.setCenter(Convert(view.GetCenter()));
    sfView.setSize(Convert(view.GetSize()));
    sfView.setRotation(view.GetRotation());
    sfView.setViewport(Convert(view.GetViewport()));
    return sfView;
}

///////////////////////////////////////////////////////////////////////////////
sf::BlendMode Utils::Convert(EBlendMode blendMode)
{
    switch (blendMode)
    {
    case EBlendMode::Alpha   : return sf::BlendAlpha;
    case EBlendMode::Add     : return sf::BlendAdd;
    case EBlendMode::Multiply: return sf::BlendMultiply;
    case EBlendMode::None    : return sf::BlendNone;
    case EBlendMode::Screen:
        return sf::BlendMode(
            sf::BlendMode::One,
            sf::BlendMode::OneMinusSrcColor,
            sf::BlendMode::Add
        );
    case EBlendMode::Subtract:
        return sf::BlendMode(
            sf::BlendMode::One,
            sf::BlendMode::One,
            sf::BlendMode::ReverseSubtract
        );
    case EBlendMode::Replace:
        return sf::BlendMode(
            sf::BlendMode::One, sf::BlendMode::Zero, sf::BlendMode::Add
        );
    default: return sf::BlendAlpha;
    }
}

///////////////////////////////////////////////////////////////////////////////
sf::Shader::Type Utils::Convert(EShaderType shaderType)
{
    switch (shaderType)
    {
    case EShaderType::Vertex  : return sf::Shader::Type::Vertex;
    case EShaderType::Fragment: return sf::Shader::Type::Fragment;
    case EShaderType::Geometry: return sf::Shader::Type::Geometry;
    default                   : return sf::Shader::Type::Vertex;
    }
}

///////////////////////////////////////////////////////////////////////////////
sf::RenderStates Utils::Convert(const FRenderStates& states)
{
    return sf::RenderStates(
        Convert(states.blendMode),
        Convert(states.transform),
        states.texture
            ? reinterpret_cast<sf::Texture*>(states.texture->GetNativeHandle())
            : nullptr,
        states.shader
            ? reinterpret_cast<sf::Shader*>(states.shader->GetNativeHandle())
            : nullptr
    );
}

///////////////////////////////////////////////////////////////////////////////
sf::PrimitiveType Utils::Convert(EPrimitiveType type)
{
    switch (type)
    {
    case EPrimitiveType::Points   : return sf::PrimitiveType::Points;
    case EPrimitiveType::Lines    : return sf::PrimitiveType::Lines;
    case EPrimitiveType::LineStrip: return sf::PrimitiveType::LineStrip;
    case EPrimitiveType::Triangles: return sf::PrimitiveType::Triangles;
    case EPrimitiveType::TriangleStrip:
        return sf::PrimitiveType::TriangleStrip;
    case EPrimitiveType::TriangleFan: return sf::PrimitiveType::TriangleFan;
    case EPrimitiveType::Quads      : return sf::PrimitiveType::Quads;
    default                         : return sf::PrimitiveType::Triangles;
    }
}

///////////////////////////////////////////////////////////////////////////////
sf::Texture::CoordinateType
    Utils::Convert(ETextureCoordinateType textureCoordinateType)
{
    switch (textureCoordinateType)
    {
    case ETextureCoordinateType::Normalized: return sf::Texture::Normalized;
    case ETextureCoordinateType::Pixels    : return sf::Texture::Pixels;
    default                                : return sf::Texture::Pixels;
    }
}

///////////////////////////////////////////////////////////////////////////////
sf::Transform Utils::Convert(const FTransform2D& transform)
{
    sf::Transform sfTransform = sf::Transform::Identity;

    sfTransform.translate(Convert(transform.GetPosition()));
    sfTransform.rotate(transform.GetRotation().GetAngle());
    sfTransform.scale(Convert(transform.GetScale()));

    return sfTransform;
}

///////////////////////////////////////////////////////////////////////////////
sf::Vertex Utils::Convert(const FVertex2D& vertex)
{
    return sf::Vertex(
        Convert(vertex.position), Convert(vertex.color), Convert(vertex.uv)
    );
}

///////////////////////////////////////////////////////////////////////////////
sf::Glsl::Mat3 Utils::Convert(const FMatrix3x3f& mat)
{
    return sf::Glsl::Mat3(&mat.data[0][0]);
}

///////////////////////////////////////////////////////////////////////////////
sf::Glsl::Mat4 Utils::Convert(const FMatrix4x4f& mat)
{
    return sf::Glsl::Mat4(&mat.data[0][0]);
}

///////////////////////////////////////////////////////////////////////////////
sf::Glsl::Vec2 Utils::Convert(const FVector2f& vec)
{
    return sf::Glsl::Vec2(vec.x, vec.y);
}

///////////////////////////////////////////////////////////////////////////////
sf::Glsl::Bvec2 Utils::Convert(const FVector2b& vec)
{
    return sf::Glsl::Bvec2(vec.x, vec.y);
}

///////////////////////////////////////////////////////////////////////////////
sf::Glsl::Ivec2 Utils::Convert(const FVector2i& vec)
{
    return sf::Glsl::Ivec2(vec.x, vec.y);
}

///////////////////////////////////////////////////////////////////////////////
sf::Glsl::Vec3 Utils::Convert(const FVector3f& vec)
{
    return sf::Glsl::Vec3(vec.x, vec.y, vec.z);
}

///////////////////////////////////////////////////////////////////////////////
sf::Glsl::Bvec3 Utils::Convert(const FVector3b& vec)
{
    return sf::Glsl::Bvec3(vec.x, vec.y, vec.z);
}

///////////////////////////////////////////////////////////////////////////////
sf::Glsl::Ivec3 Utils::Convert(const FVector3i& vec)
{
    return sf::Glsl::Ivec3(vec.x, vec.y, vec.z);
}

///////////////////////////////////////////////////////////////////////////////
sf::Glsl::Vec4 Utils::Convert(const FVector4f& vec)
{
    return sf::Glsl::Vec4(vec.x, vec.y, vec.z, vec.w);
}

///////////////////////////////////////////////////////////////////////////////
sf::Glsl::Bvec4 Utils::Convert(const FVector4b& vec)
{
    return sf::Glsl::Bvec4(vec.x, vec.y, vec.z, vec.w);
}

///////////////////////////////////////////////////////////////////////////////
sf::Glsl::Ivec4 Utils::Convert(const FVector4i& vec)
{
    return sf::Glsl::Ivec4(vec.x, vec.y, vec.z, vec.w);
}

#endif

}   // namespace tkd::SFML
