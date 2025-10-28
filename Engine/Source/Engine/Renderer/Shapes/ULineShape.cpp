///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Renderer/Shapes/ULineShape.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
ULineShape::ULineShape(void)
    : UShape()
    , m_start(0.f, 0.f)
    , m_end(100.f, 0.f)
    , m_thickness(1.f)
{
    UpdateGeometry();
}

///////////////////////////////////////////////////////////////////////////////
ULineShape::ULineShape(
    const FVector2f& start, const FVector2f& end, float thickness
)
    : UShape()
    , m_start(start)
    , m_end(end)
    , m_thickness(thickness)
{
    UpdateGeometry();
}

///////////////////////////////////////////////////////////////////////////////
void ULineShape::SetStart(const FVector2f& point)
{
    m_start = point;
    UpdateGeometry();
}

///////////////////////////////////////////////////////////////////////////////
void ULineShape::SetEnd(const FVector2f& point)
{
    m_end = point;
    UpdateGeometry();
}

///////////////////////////////////////////////////////////////////////////////
void ULineShape::SetThickness(float thickness)
{
    m_thickness = thickness;
    UpdateGeometry();
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD const FVector2f& ULineShape::GetStart(void) const
{
    return m_start;
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD const FVector2f& ULineShape::GetEnd(void) const { return m_end; }

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD float ULineShape::GetThickness(void) const
{
    return m_thickness;
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD float ULineShape::GetLength(void) const
{
    FVector2f diff = m_end - m_start;
    return std::sqrt(diff.x * diff.x + diff.y * diff.y);
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD SizeT ULineShape::GetPointCount(void) const { return 4; }

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD FVector2f ULineShape::GetPoint(SizeT index) const
{
    FVector2f direction = m_end - m_start;
    float length = GetLength();

    if (length != 0.f)
    {
        // Normalize direction
        direction /= length;
    }

    // Perpendicular vector for thickness
    FVector2f perpendicular(-direction.y, direction.x);
    perpendicular *= m_thickness * 0.5f;

    switch (index)
    {
    case 0 : return m_start - perpendicular;
    case 1 : return m_end - perpendicular;
    case 2 : return m_end + perpendicular;
    case 3 : return m_start + perpendicular;
    default: return m_start;   // Should never happen
    }
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD FVector2f ULineShape::GetGeometricCenter(void) const
{
    return (m_start + m_end) * 0.5f;
}

}   // namespace tkd
