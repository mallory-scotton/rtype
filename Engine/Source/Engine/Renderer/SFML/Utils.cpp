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
    sf::RenderStates sfStates = sf::RenderStates::Default;

    sfStates.blendMode = Convert(states.blendMode);
    sfStates.transform = Convert(states.transform);
    if (states.shader)
    {
        sfStates.shader =
            reinterpret_cast<sf::Shader*>(states.shader->GetNativePointer());
    }
    if (states.texture)
    {
        sfStates.texture =
            reinterpret_cast<sf::Texture*>(states.texture->GetNativePointer());
    }

    return sfStates;
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
    case EPrimitiveType::QuadStrip  : return sf::PrimitiveType::Quads;
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

///////////////////////////////////////////////////////////////////////////////
sf::Keyboard::Key Utils::Convert(EKeyboardKeys key)
{
    switch (key)
    {
    case EKeyboardKeys::A        : return sf::Keyboard::A;
    case EKeyboardKeys::B        : return sf::Keyboard::B;
    case EKeyboardKeys::C        : return sf::Keyboard::C;
    case EKeyboardKeys::D        : return sf::Keyboard::D;
    case EKeyboardKeys::E        : return sf::Keyboard::E;
    case EKeyboardKeys::F        : return sf::Keyboard::F;
    case EKeyboardKeys::G        : return sf::Keyboard::G;
    case EKeyboardKeys::H        : return sf::Keyboard::H;
    case EKeyboardKeys::I        : return sf::Keyboard::I;
    case EKeyboardKeys::J        : return sf::Keyboard::J;
    case EKeyboardKeys::K        : return sf::Keyboard::K;
    case EKeyboardKeys::L        : return sf::Keyboard::L;
    case EKeyboardKeys::M        : return sf::Keyboard::M;
    case EKeyboardKeys::N        : return sf::Keyboard::N;
    case EKeyboardKeys::O        : return sf::Keyboard::O;
    case EKeyboardKeys::P        : return sf::Keyboard::P;
    case EKeyboardKeys::Q        : return sf::Keyboard::Q;
    case EKeyboardKeys::R        : return sf::Keyboard::R;
    case EKeyboardKeys::S        : return sf::Keyboard::S;
    case EKeyboardKeys::T        : return sf::Keyboard::T;
    case EKeyboardKeys::U        : return sf::Keyboard::U;
    case EKeyboardKeys::V        : return sf::Keyboard::V;
    case EKeyboardKeys::W        : return sf::Keyboard::W;
    case EKeyboardKeys::X        : return sf::Keyboard::X;
    case EKeyboardKeys::Y        : return sf::Keyboard::Y;
    case EKeyboardKeys::Z        : return sf::Keyboard::Z;
    case EKeyboardKeys::Num0     : return sf::Keyboard::Num0;
    case EKeyboardKeys::Num1     : return sf::Keyboard::Num1;
    case EKeyboardKeys::Num2     : return sf::Keyboard::Num2;
    case EKeyboardKeys::Num3     : return sf::Keyboard::Num3;
    case EKeyboardKeys::Num4     : return sf::Keyboard::Num4;
    case EKeyboardKeys::Num5     : return sf::Keyboard::Num5;
    case EKeyboardKeys::Num6     : return sf::Keyboard::Num6;
    case EKeyboardKeys::Num7     : return sf::Keyboard::Num7;
    case EKeyboardKeys::Num8     : return sf::Keyboard::Num8;
    case EKeyboardKeys::Num9     : return sf::Keyboard::Num9;
    case EKeyboardKeys::Escape   : return sf::Keyboard::Escape;
    case EKeyboardKeys::LControl : return sf::Keyboard::LControl;
    case EKeyboardKeys::LShift   : return sf::Keyboard::LShift;
    case EKeyboardKeys::LAlt     : return sf::Keyboard::LAlt;
    case EKeyboardKeys::LSystem  : return sf::Keyboard::LSystem;
    case EKeyboardKeys::RControl : return sf::Keyboard::RControl;
    case EKeyboardKeys::RShift   : return sf::Keyboard::RShift;
    case EKeyboardKeys::RAlt     : return sf::Keyboard::RAlt;
    case EKeyboardKeys::RSystem  : return sf::Keyboard::RSystem;
    case EKeyboardKeys::Menu     : return sf::Keyboard::Menu;
    case EKeyboardKeys::LBracket : return sf::Keyboard::LBracket;
    case EKeyboardKeys::RBracket : return sf::Keyboard::RBracket;
    case EKeyboardKeys::Semicolon: return sf::Keyboard::Semicolon;
    case EKeyboardKeys::Comma    : return sf::Keyboard::Comma;
    case EKeyboardKeys::Period   : return sf::Keyboard::Period;
    case EKeyboardKeys::Quote    : return sf::Keyboard::Quote;
    case EKeyboardKeys::Slash    : return sf::Keyboard::Slash;
    case EKeyboardKeys::Backslash: return sf::Keyboard::Backslash;
    case EKeyboardKeys::Tilde    : return sf::Keyboard::Tilde;
    case EKeyboardKeys::Equal    : return sf::Keyboard::Equal;
    case EKeyboardKeys::Hyphen   : return sf::Keyboard::Hyphen;
    case EKeyboardKeys::Space    : return sf::Keyboard::Space;
    case EKeyboardKeys::Enter    : return sf::Keyboard::Enter;
    case EKeyboardKeys::Backspace: return sf::Keyboard::Backspace;
    case EKeyboardKeys::Tab      : return sf::Keyboard::Tab;
    case EKeyboardKeys::PageUp   : return sf::Keyboard::PageUp;
    case EKeyboardKeys::PageDown : return sf::Keyboard::PageDown;
    case EKeyboardKeys::End      : return sf::Keyboard::End;
    case EKeyboardKeys::Home     : return sf::Keyboard::Home;
    case EKeyboardKeys::Insert   : return sf::Keyboard::Insert;
    case EKeyboardKeys::Delete   : return sf::Keyboard::Delete;
    case EKeyboardKeys::Add      : return sf::Keyboard::Add;
    case EKeyboardKeys::Subtract : return sf::Keyboard::Subtract;
    case EKeyboardKeys::Multiply : return sf::Keyboard::Multiply;
    case EKeyboardKeys::Divide   : return sf::Keyboard::Divide;
    case EKeyboardKeys::Left     : return sf::Keyboard::Left;
    case EKeyboardKeys::Right    : return sf::Keyboard::Right;
    case EKeyboardKeys::Up       : return sf::Keyboard::Up;
    case EKeyboardKeys::Down     : return sf::Keyboard::Down;
    case EKeyboardKeys::Numpad0  : return sf::Keyboard::Numpad0;
    case EKeyboardKeys::Numpad1  : return sf::Keyboard::Numpad1;
    case EKeyboardKeys::Numpad2  : return sf::Keyboard::Numpad2;
    case EKeyboardKeys::Numpad3  : return sf::Keyboard::Numpad3;
    case EKeyboardKeys::Numpad4  : return sf::Keyboard::Numpad4;
    case EKeyboardKeys::Numpad5  : return sf::Keyboard::Numpad5;
    case EKeyboardKeys::Numpad6  : return sf::Keyboard::Numpad6;
    case EKeyboardKeys::Numpad7  : return sf::Keyboard::Numpad7;
    case EKeyboardKeys::Numpad8  : return sf::Keyboard::Numpad8;
    case EKeyboardKeys::Numpad9  : return sf::Keyboard::Numpad9;
    case EKeyboardKeys::F1       : return sf::Keyboard::F1;
    case EKeyboardKeys::F2       : return sf::Keyboard::F2;
    case EKeyboardKeys::F3       : return sf::Keyboard::F3;
    case EKeyboardKeys::F4       : return sf::Keyboard::F4;
    case EKeyboardKeys::F5       : return sf::Keyboard::F5;
    case EKeyboardKeys::F6       : return sf::Keyboard::F6;
    case EKeyboardKeys::F7       : return sf::Keyboard::F7;
    case EKeyboardKeys::F8       : return sf::Keyboard::F8;
    case EKeyboardKeys::F9       : return sf::Keyboard::F9;
    case EKeyboardKeys::F10      : return sf::Keyboard::F10;
    case EKeyboardKeys::F11      : return sf::Keyboard::F11;
    case EKeyboardKeys::F12      : return sf::Keyboard::F12;
    case EKeyboardKeys::F13      : return sf::Keyboard::F13;
    case EKeyboardKeys::F14      : return sf::Keyboard::F14;
    case EKeyboardKeys::F15      : return sf::Keyboard::F15;
    case EKeyboardKeys::Pause    : return sf::Keyboard::Pause;
    default                      : return sf::Keyboard::Unknown;
    }
}

///////////////////////////////////////////////////////////////////////////////
sf::Mouse::Button Utils::Convert(EMouseButtons button)
{
    switch (button)
    {
    case EMouseButtons::Left    : return sf::Mouse::Left;
    case EMouseButtons::Right   : return sf::Mouse::Right;
    case EMouseButtons::Middle  : return sf::Mouse::Middle;
    case EMouseButtons::XButton1: return sf::Mouse::XButton1;
    case EMouseButtons::XButton2: return sf::Mouse::XButton2;
    default                     : return sf::Mouse::Left;
    }
}

///////////////////////////////////////////////////////////////////////////////
UInt32 Utils::Convert(EGamepadButtons button)
{
    switch (button)
    {
    case EGamepadButtons::A            : return 0;
    case EGamepadButtons::B            : return 1;
    case EGamepadButtons::X            : return 2;
    case EGamepadButtons::Y            : return 3;
    case EGamepadButtons::Back         : return 6;
    case EGamepadButtons::Guide        : return 999;
    case EGamepadButtons::Start        : return 7;
    case EGamepadButtons::LeftStick    : return 8;
    case EGamepadButtons::RightStick   : return 9;
    case EGamepadButtons::LeftShoulder : return 4;
    case EGamepadButtons::RightShoulder: return 5;
    case EGamepadButtons::DPadUp       : return 999;
    case EGamepadButtons::DPadDown     : return 999;
    case EGamepadButtons::DPadLeft     : return 999;
    case EGamepadButtons::DPadRight    : return 999;
    default                            : return 0;
    }
}

///////////////////////////////////////////////////////////////////////////////
sf::Joystick::Axis Utils::Convert(EGamepadAxes axis)
{
    switch (axis)
    {
    case EGamepadAxes::LeftX       : return sf::Joystick::X;
    case EGamepadAxes::LeftY       : return sf::Joystick::Y;
    case EGamepadAxes::RightX      : return sf::Joystick::Z;
    case EGamepadAxes::RightY      : return sf::Joystick::R;
    case EGamepadAxes::LeftTrigger : return sf::Joystick::U;
    case EGamepadAxes::RightTrigger: return sf::Joystick::V;
    default                        : return sf::Joystick::X;
    }
}

#endif

}   // namespace tkd::SFML
