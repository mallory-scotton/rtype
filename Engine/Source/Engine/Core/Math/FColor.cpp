///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Core/Math/FColor.hpp>
#include <Engine/Core/Math/FLinearColor.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
const FColor FColor::White(1.0f, 1.0f, 1.0f);
const FColor FColor::Black(0.0f, 0.0f, 0.0f);
const FColor FColor::Red(1.0f, 0.0f, 0.0f);
const FColor FColor::Green(0.0f, 1.0f, 0.0f);
const FColor FColor::Blue(0.0f, 0.0f, 1.0f);
const FColor FColor::Yellow(1.0f, 1.0f, 0.0f);
const FColor FColor::Cyan(0.0f, 1.0f, 1.0f);
const FColor FColor::Magenta(1.0f, 0.0f, 1.0f);
const FColor FColor::Transparent(0.0f, 0.0f, 0.0f, 0.0f);

///////////////////////////////////////////////////////////////////////////////
FColor::FColor(void)
    : r(0.0f)
    , g(0.0f)
    , b(0.0f)
    , a(1.0f)
{}

///////////////////////////////////////////////////////////////////////////////
FColor::FColor(Float32 r, Float32 g, Float32 b, Float32 a)
    : r(r)
    , g(g)
    , b(b)
    , a(a)
{}

///////////////////////////////////////////////////////////////////////////////
FColor::FColor(const FLinearColor& other)
    : r(static_cast<Float32>(other.r) / 255.0f)
    , g(static_cast<Float32>(other.g) / 255.0f)
    , b(static_cast<Float32>(other.b) / 255.0f)
    , a(static_cast<Float32>(other.a) / 255.0f)
{}

///////////////////////////////////////////////////////////////////////////////
FColor::operator FLinearColor(void) const
{
    return FLinearColor(
        static_cast<UInt8>(r * 255.0f),
        static_cast<UInt8>(g * 255.0f),
        static_cast<UInt8>(b * 255.0f),
        static_cast<UInt8>(a * 255.0f)
    );
}

///////////////////////////////////////////////////////////////////////////////
FLinearColor FColor::ToLinearColor(void) const
{
    return static_cast<FLinearColor>(*this);
}

///////////////////////////////////////////////////////////////////////////////
FColor FColor::FromLinearColor(const FLinearColor& linearColor)
{
    return FColor(linearColor);
}

///////////////////////////////////////////////////////////////////////////////
bool operator==(const FColor& lhs, const FColor& rhs)
{
    return (lhs.r == rhs.r) && (lhs.g == rhs.g) && (lhs.b == rhs.b) &&
           (lhs.a == rhs.a);
}

///////////////////////////////////////////////////////////////////////////////
bool operator!=(const FColor& lhs, const FColor& rhs) { return !(lhs == rhs); }

///////////////////////////////////////////////////////////////////////////////
bool operator==(const FColor& lhs, const FLinearColor& rhs)
{
    return (lhs.r == static_cast<Float32>(rhs.r) / 255.0f) &&
           (lhs.g == static_cast<Float32>(rhs.g) / 255.0f) &&
           (lhs.b == static_cast<Float32>(rhs.b) / 255.0f) &&
           (lhs.a == static_cast<Float32>(rhs.a) / 255.0f);
}

///////////////////////////////////////////////////////////////////////////////
bool operator!=(const FColor& lhs, const FLinearColor& rhs)
{
    return !(lhs == rhs);
}

}   // namespace tkd
