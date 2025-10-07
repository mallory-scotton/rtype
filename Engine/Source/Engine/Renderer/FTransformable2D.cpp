///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Renderer/FTransformable2D.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
FTransformable2D::FTransformable2D(void)
    : m_origin(FVector2::Zero)
    , m_position(FVector2::Zero)
    , m_rotation(0.0f)
    , m_scale(FVector2::One)
    , m_transform(FTransform2D::Identity)
    , m_transformNeedUpdate(true)
    , m_inverseTransform(FTransform2D::Identity)
    , m_inverseTransformNeedUpdate(true)
{}

///////////////////////////////////////////////////////////////////////////////
void FTransformable2D::SetPosition(float x, float y)
{
    m_position.x = x;
    m_position.y = y;
    m_transformNeedUpdate = true;
    m_inverseTransformNeedUpdate = true;
}

///////////////////////////////////////////////////////////////////////////////
void FTransformable2D::SetPosition(const FVector2& position)
{
    SetPosition(position.x, position.y);
}

///////////////////////////////////////////////////////////////////////////////
void FTransformable2D::SetRotation(float angle)
{
    m_rotation = Math<float>::Mod(angle, 360.f);
    while (m_rotation < 0.f) { m_rotation += 360.f; }
    m_transformNeedUpdate = true;
    m_inverseTransformNeedUpdate = true;
}

///////////////////////////////////////////////////////////////////////////////
void FTransformable2D::SetRotation(const FRotator2D& rotation)
{
    SetRotation(rotation.GetAngle());
}

///////////////////////////////////////////////////////////////////////////////
void FTransformable2D::SetScale(float x, float y)
{
    m_scale.x = x;
    m_scale.y = y;
    m_transformNeedUpdate = true;
    m_inverseTransformNeedUpdate = true;
}

///////////////////////////////////////////////////////////////////////////////
void FTransformable2D::SetScale(const FVector2& scale)
{
    SetScale(scale.x, scale.y);
}

///////////////////////////////////////////////////////////////////////////////
void FTransformable2D::SetOrigin(float x, float y)
{
    m_origin.x = x;
    m_origin.y = y;
    m_transformNeedUpdate = true;
    m_inverseTransformNeedUpdate = true;
}

///////////////////////////////////////////////////////////////////////////////
void FTransformable2D::SetOrigin(const FVector2& origin)
{
    SetOrigin(origin.x, origin.y);
}

///////////////////////////////////////////////////////////////////////////////
const FVector2& FTransformable2D::GetPosition(void) const
{
    return m_position;
}

///////////////////////////////////////////////////////////////////////////////
float FTransformable2D::GetRotation(void) const { return m_rotation; }

///////////////////////////////////////////////////////////////////////////////
const FVector2& FTransformable2D::GetScale(void) const { return m_scale; }

///////////////////////////////////////////////////////////////////////////////
const FVector2& FTransformable2D::GetOrigin(void) const { return m_origin; }

///////////////////////////////////////////////////////////////////////////////
void FTransformable2D::Move(float offsetX, float offsetY)
{
    SetPosition(m_position.x + offsetX, m_position.y + offsetY);
}

///////////////////////////////////////////////////////////////////////////////
void FTransformable2D::Move(const FVector2& offset)
{
    Move(offset.x, offset.y);
}

///////////////////////////////////////////////////////////////////////////////
void FTransformable2D::Rotate(float angle) { SetRotation(m_rotation + angle); }

///////////////////////////////////////////////////////////////////////////////
void FTransformable2D::Rotate(const FRotator2D& rotation)
{
    Rotate(rotation.GetAngle());
}

///////////////////////////////////////////////////////////////////////////////
void FTransformable2D::Scale(float x, float y)
{
    SetScale(m_scale.x * x, m_scale.y * y);
}

///////////////////////////////////////////////////////////////////////////////
void FTransformable2D::Scale(const FVector2& scale)
{
    Scale(scale.x, scale.y);
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD const FTransform2D& FTransformable2D::GetTransform(void) const
{
    if (m_transformNeedUpdate)
    {
        float angle = -m_rotation * M_PI / 180.f;
        float cosine = std::cos(angle);
        float sine = std::sin(angle);
        float sxc = m_scale.x * cosine;
        float syc = m_scale.y * cosine;
        float sxs = m_scale.x * sine;
        float sys = m_scale.y * sine;
        float tx = -m_origin.x * sxc - m_origin.y * sys + m_position.x;
        float ty = m_origin.x * sxs - m_origin.y * syc + m_position.y;

        m_transform = FTransform2D(sxc, sys, tx, -sxs, syc, ty, 0.f, 0.f, 1.f);
        m_transformNeedUpdate = false;
    }
    return m_transform;
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD const FTransform2D& FTransformable2D::GetInverseTransform(void
) const
{
    if (m_inverseTransformNeedUpdate)
    {
        m_inverseTransform = GetTransform().Inverse();
        m_inverseTransformNeedUpdate = false;
    }
    return m_inverseTransform;
}

}   // namespace tkd
