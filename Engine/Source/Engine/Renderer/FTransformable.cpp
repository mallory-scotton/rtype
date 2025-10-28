///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Renderer/FTransformable.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
FTransformable::FTransformable(void)
    : m_origin(FVector3::Zero)
    , m_position(FVector3::Zero)
    , m_rotation(FRotator::Identity)
    , m_scale(FVector3::One)
    , m_transform(FTransform::Identity)
    , m_transformNeedUpdate(true)
    , m_inverseTransform(FTransform::Identity)
    , m_inverseTransformNeedUpdate(true)
{}

///////////////////////////////////////////////////////////////////////////////
void FTransformable::SetPosition(float x, float y, float z)
{
    m_position.x = x;
    m_position.y = y;
    m_position.z = z;
    m_transformNeedUpdate = true;
    m_inverseTransformNeedUpdate = true;
}

///////////////////////////////////////////////////////////////////////////////
void FTransformable::SetPosition(const FVector3& position)
{
    SetPosition(position.x, position.y, position.z);
}

///////////////////////////////////////////////////////////////////////////////
void FTransformable::SetRotation(float roll, float pitch, float yaw)
{
    m_rotation = FRotator(pitch, yaw, roll);
    m_transformNeedUpdate = true;
    m_inverseTransformNeedUpdate = true;
}

///////////////////////////////////////////////////////////////////////////////
void FTransformable::SetRotation(const FRotator& rotation)
{
    m_rotation = rotation;
    m_transformNeedUpdate = true;
    m_inverseTransformNeedUpdate = true;
}

///////////////////////////////////////////////////////////////////////////////
void FTransformable::SetScale(float x, float y, float z)
{
    m_scale.x = x;
    m_scale.y = y;
    m_scale.z = z;
    m_transformNeedUpdate = true;
    m_inverseTransformNeedUpdate = true;
}

///////////////////////////////////////////////////////////////////////////////
void FTransformable::SetScale(const FVector3& scale)
{
    SetScale(scale.x, scale.y, scale.z);
}

///////////////////////////////////////////////////////////////////////////////
void FTransformable::SetOrigin(float x, float y, float z)
{
    m_origin.x = x;
    m_origin.y = y;
    m_origin.z = z;
    m_transformNeedUpdate = true;
    m_inverseTransformNeedUpdate = true;
}

///////////////////////////////////////////////////////////////////////////////
void FTransformable::SetOrigin(const FVector3& origin)
{
    SetOrigin(origin.x, origin.y, origin.z);
}

///////////////////////////////////////////////////////////////////////////////
void FTransformable::Move(float x, float y, float z)
{
    SetPosition(m_position.x + x, m_position.y + y, m_position.z + z);
}

///////////////////////////////////////////////////////////////////////////////
void FTransformable::Move(const FVector3& offset)
{
    SetPosition(
        m_position.x + offset.x,
        m_position.y + offset.y,
        m_position.z + offset.z
    );
}

///////////////////////////////////////////////////////////////////////////////
void FTransformable::Rotate(float roll, float pitch, float yaw)
{
    SetRotation(
        m_rotation.GetPitch() + pitch,
        m_rotation.GetYaw() + yaw,
        m_rotation.GetRoll() + roll
    );
}

///////////////////////////////////////////////////////////////////////////////
void FTransformable::Rotate(const FRotator& rotation)
{
    SetRotation(
        m_rotation.GetPitch() + rotation.GetPitch(),
        m_rotation.GetYaw() + rotation.GetYaw(),
        m_rotation.GetRoll() + rotation.GetRoll()
    );
}

///////////////////////////////////////////////////////////////////////////////
void FTransformable::Scale(float x, float y, float z)
{
    SetScale(m_scale.x * x, m_scale.y * y, m_scale.z * z);
}

///////////////////////////////////////////////////////////////////////////////
void FTransformable::Scale(const FVector3& scale)
{
    Scale(scale.x, scale.y, scale.z);
}

///////////////////////////////////////////////////////////////////////////////
const FTransform& FTransformable::GetTransform(void) const
{
    if (m_transformNeedUpdate)
    {
        // Convert rotation from degrees to radians
        float pitchRad = m_rotation.GetPitch() * M_PI / 180.f;
        float yawRad = m_rotation.GetYaw() * M_PI / 180.f;
        float rollRad = m_rotation.GetRoll() * M_PI / 180.f;

        // Calculate trigonometric values
        float cp = std::cos(pitchRad);
        float sp = std::sin(pitchRad);
        float cy = std::cos(yawRad);
        float sy = std::sin(yawRad);
        float cr = std::cos(rollRad);
        float sr = std::sin(rollRad);

        // Build rotation matrix (ZYX Euler angles)
        // Combined rotation: Rz(yaw) * Ry(pitch) * Rx(roll)
        // Apply scale to rotation matrix
        float m00 = (cy * cp) * m_scale.x;
        float m01 = (cy * sp * sr - sy * cr) * m_scale.y;
        float m02 = (cy * sp * cr + sy * sr) * m_scale.z;

        float m10 = (sy * cp) * m_scale.x;
        float m11 = (sy * sp * sr + cy * cr) * m_scale.y;
        float m12 = (sy * sp * cr - cy * sr) * m_scale.z;

        float m20 = (-sp) * m_scale.x;
        float m21 = (cp * sr) * m_scale.y;
        float m22 = (cp * cr) * m_scale.z;

        // Apply origin offset and position
        float tx = -m_origin.x * m00 - m_origin.y * m01 - m_origin.z * m02 +
                   m_position.x;
        float ty = -m_origin.x * m10 - m_origin.y * m11 - m_origin.z * m12 +
                   m_position.y;
        float tz = -m_origin.x * m20 - m_origin.y * m21 - m_origin.z * m22 +
                   m_position.z;

        // Construct the transform from components
        m_transform = FTransform(FVector3(tx, ty, tz), m_rotation, m_scale);

        m_transformNeedUpdate = false;
    }
    return m_transform;
}

///////////////////////////////////////////////////////////////////////////////
const FTransform& FTransformable::GetInverseTransform(void) const
{
    if (m_inverseTransformNeedUpdate)
    {
        m_inverseTransform = GetTransform().Inverse();
        m_inverseTransformNeedUpdate = false;
    }
    return m_inverseTransform;
}

///////////////////////////////////////////////////////////////////////////////
const FVector3& FTransformable::GetPosition(void) const { return m_position; }

///////////////////////////////////////////////////////////////////////////////
const FRotator& FTransformable::GetRotation(void) const { return m_rotation; }

///////////////////////////////////////////////////////////////////////////////
const FVector3& FTransformable::GetScale(void) const { return m_scale; }

///////////////////////////////////////////////////////////////////////////////
const FVector3& FTransformable::GetOrigin(void) const { return m_origin; }

///////////////////////////////////////////////////////////////////////////////
void FTransformable::SetTransform(const FTransform& transform)
{
    m_position = transform.GetPosition();
    m_rotation = transform.GetRotation();
    m_scale = transform.GetScale();
    m_transformNeedUpdate = true;
    m_inverseTransformNeedUpdate = true;
}

}   // namespace tkd
