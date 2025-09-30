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

#endif

}   // namespace tkd::SFML
