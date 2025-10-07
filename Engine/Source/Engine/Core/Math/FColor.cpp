///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Core/Math/FColor.hpp>
#include <Engine/Core/Math/FLinearColor.hpp>
#include <Engine/Core/Math/Utils.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
const FColor FColor::White(1.f, 1.f, 1.f, 1.f);
const FColor FColor::Black(0.f, 0.f, 0.f, 1.f);
const FColor FColor::Red(1.f, 0.f, 0.f, 1.f);
const FColor FColor::Green(0.f, 1.f, 0.f, 1.f);
const FColor FColor::Blue(0.f, 0.f, 1.f, 1.f);
const FColor FColor::Yellow(1.f, 1.f, 0.f, 1.f);
const FColor FColor::Cyan(0.f, 1.f, 1.f, 1.f);
const FColor FColor::Magenta(1.f, 0.f, 1.f, 1.f);
const FColor FColor::Transparent(0.f, 0.f, 0.f, 0.f);

///////////////////////////////////////////////////////////////////////////////
FColor::FColor(void)
    : r(0.f)
    , g(0.f)
    , b(0.f)
    , a(1.f)
{}

///////////////////////////////////////////////////////////////////////////////
FColor::FColor(Float32 r, Float32 g, Float32 b, Float32 a)
    : r(Math<Float32>::Clamp(r, 0.f, 1.f))
    , g(Math<Float32>::Clamp(g, 0.f, 1.f))
    , b(Math<Float32>::Clamp(b, 0.f, 1.f))
    , a(Math<Float32>::Clamp(a, 0.f, 1.f))
{}

///////////////////////////////////////////////////////////////////////////////
FColor::FColor(const FLinearColor& other)
    : r(Math<Float32>::Clamp(static_cast<Float32>(other.r) / 255.0f, 0.f, 1.f))
    , g(Math<Float32>::Clamp(static_cast<Float32>(other.g) / 255.0f, 0.f, 1.f))
    , b(Math<Float32>::Clamp(static_cast<Float32>(other.b) / 255.0f, 0.f, 1.f))
    , a(Math<Float32>::Clamp(static_cast<Float32>(other.a) / 255.0f, 0.f, 1.f))
{}

///////////////////////////////////////////////////////////////////////////////
FColor::operator FLinearColor(void) const
{
    return FLinearColor(
        static_cast<UInt8>(r * 255.f),
        static_cast<UInt8>(g * 255.f),
        static_cast<UInt8>(b * 255.f),
        static_cast<UInt8>(a * 255.f)
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
    return (lhs.r == static_cast<Float32>(rhs.r) / 255.f) &&
           (lhs.g == static_cast<Float32>(rhs.g) / 255.f) &&
           (lhs.b == static_cast<Float32>(rhs.b) / 255.f) &&
           (lhs.a == static_cast<Float32>(rhs.a) / 255.f);
}

///////////////////////////////////////////////////////////////////////////////
bool operator!=(const FColor& lhs, const FLinearColor& rhs)
{
    return !(lhs == rhs);
}

}   // namespace tkd
