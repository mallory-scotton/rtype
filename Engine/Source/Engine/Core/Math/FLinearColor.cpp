///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Core/Math/FLinearColor.hpp>
#include <Engine/Core/Math/FColor.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
const FLinearColor FLinearColor::White(255, 255, 255);
const FLinearColor FLinearColor::Black(0, 0, 0);
const FLinearColor FLinearColor::Red(255, 0, 0);
const FLinearColor FLinearColor::Green(0, 255, 0);
const FLinearColor FLinearColor::Blue(0, 0, 255);
const FLinearColor FLinearColor::Yellow(255, 255, 0);
const FLinearColor FLinearColor::Cyan(0, 255, 255);
const FLinearColor FLinearColor::Magenta(255, 0, 255);
const FLinearColor FLinearColor::Transparent(0, 0, 0, 0);

///////////////////////////////////////////////////////////////////////////////
FLinearColor::FLinearColor(void)
    : r(0)
    , g(0)
    , b(0)
    , a(255)
{}

///////////////////////////////////////////////////////////////////////////////
FLinearColor::FLinearColor(UInt8 r, UInt8 g, UInt8 b, UInt8 a)
    : r(r)
    , g(g)
    , b(b)
    , a(a)
{}

///////////////////////////////////////////////////////////////////////////////
FLinearColor::FLinearColor(UInt32 color)
    : color(color)
{}

///////////////////////////////////////////////////////////////////////////////
FLinearColor::FLinearColor(const FColor& other)
    : r(static_cast<UInt8>(other.r * 255.0f))
    , g(static_cast<UInt8>(other.g * 255.0f))
    , b(static_cast<UInt8>(other.b * 255.0f))
    , a(static_cast<UInt8>(other.a * 255.0f))
{}

///////////////////////////////////////////////////////////////////////////////
FLinearColor::operator UInt32(void) const { return color; }

///////////////////////////////////////////////////////////////////////////////
FLinearColor::operator FColor(void) const
{
    return FColor(
        static_cast<Float32>(r) / 255.0f,
        static_cast<Float32>(g) / 255.0f,
        static_cast<Float32>(b) / 255.0f,
        static_cast<Float32>(a) / 255.0f
    );
}

///////////////////////////////////////////////////////////////////////////////
FColor FLinearColor::ToColor(void) const { return static_cast<FColor>(*this); }

///////////////////////////////////////////////////////////////////////////////
FLinearColor FLinearColor::FromColor(const FColor& color)
{
    return FLinearColor(color);
}

///////////////////////////////////////////////////////////////////////////////
bool operator==(const FLinearColor& lhs, const FLinearColor& rhs)
{
    return (lhs.r == rhs.r) && (lhs.g == rhs.g) && (lhs.b == rhs.b) &&
           (lhs.a == rhs.a);
}

///////////////////////////////////////////////////////////////////////////////
bool operator!=(const FLinearColor& lhs, const FLinearColor& rhs)
{
    return !(lhs == rhs);
}

///////////////////////////////////////////////////////////////////////////////
bool operator==(const FLinearColor& lhs, const FColor& rhs)
{
    return (lhs.r == static_cast<UInt8>(rhs.r * 255.0f)) &&
           (lhs.g == static_cast<UInt8>(rhs.g * 255.0f)) &&
           (lhs.b == static_cast<UInt8>(rhs.b * 255.0f)) &&
           (lhs.a == static_cast<UInt8>(rhs.a * 255.0f));
}

///////////////////////////////////////////////////////////////////////////////
bool operator!=(const FLinearColor& lhs, const FColor& rhs)
{
    return !(lhs == rhs);
}

}   // namespace tkd
