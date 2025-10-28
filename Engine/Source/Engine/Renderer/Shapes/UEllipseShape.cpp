///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Renderer/Shapes/UEllipseShape.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
UEllipseShape::UEllipseShape(void)
    : UShape()
    , m_radius(50.f, 30.f)
    , m_pointCount(30)
{
    UpdateGeometry();
}

///////////////////////////////////////////////////////////////////////////////
UEllipseShape::UEllipseShape(float radiusX, float radiusY, SizeT pointCount)
    : UShape()
    , m_radius(radiusX, radiusY)
    , m_pointCount((pointCount < 3) ? 3 : pointCount)
{
    UpdateGeometry();
}

///////////////////////////////////////////////////////////////////////////////
UEllipseShape::UEllipseShape(const FVector2f& radius, SizeT pointCount)
    : UShape()
    , m_radius(radius)
    , m_pointCount((pointCount < 3) ? 3 : pointCount)
{
    UpdateGeometry();
}

///////////////////////////////////////////////////////////////////////////////
void UEllipseShape::SetRadius(float radiusX, float radiusY)
{
    m_radius = FVector2f(radiusX, radiusY);
    UpdateGeometry();
}

///////////////////////////////////////////////////////////////////////////////
void UEllipseShape::SetRadius(const FVector2f& radius)
{
    m_radius = radius;
    UpdateGeometry();
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD const FVector2f& UEllipseShape::GetRadius(void) const
{
    return m_radius;
}

///////////////////////////////////////////////////////////////////////////////
void UEllipseShape::SetPointCount(SizeT count)
{
    m_pointCount = (count < 3) ? 3 : count;
    UpdateGeometry();
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD SizeT UEllipseShape::GetPointCount(void) const
{
    return m_pointCount;
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD FVector2f UEllipseShape::GetPoint(SizeT index) const
{
    const float angle =
        index * 2.f * M_PI / static_cast<float>(m_pointCount) - M_PI / 2.f;
    const float x = m_radius.x * std::cos(angle);
    const float y = m_radius.y * std::sin(angle);
    return FVector2f(x, y);
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD FVector2f UEllipseShape::GetGeometricCenter(void) const
{
    return m_radius;
}

}   // namespace tkd
