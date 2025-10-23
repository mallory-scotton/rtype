///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Renderer/VR/FPose.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::VR
///////////////////////////////////////////////////////////////////////////////
namespace tkd::VR
{

///////////////////////////////////////////////////////////////////////////////
FPose::FPose()
    : position(FVector3::Zero)
    , rotation(FQuaternion::Identity)
    , velocity(FVector3::Zero)
    , angularVelocity(FVector3::Zero)
    , isValid(false)
{}

///////////////////////////////////////////////////////////////////////////////
FPose::FPose(
    const FVector3& pos,
    const FQuaternion& rot,
    const FVector3& vel,
    const FVector3& angVel,
    bool valid
)
    : position(pos)
    , rotation(rot)
    , velocity(vel)
    , angularVelocity(angVel)
    , isValid(valid)
{}

///////////////////////////////////////////////////////////////////////////////
FMatrix4x4 FPose::GetMatrix(void) const
{
    // Convert quaternion to rotation matrix
    float x = rotation.GetX();
    float y = rotation.GetY();
    float z = rotation.GetZ();
    float w = rotation.GetW();

    float x2 = x * x;
    float y2 = y * y;
    float z2 = z * z;
    float xy = x * y;
    float xz = x * z;
    float yz = y * z;
    float wx = w * x;
    float wy = w * y;
    float wz = w * z;

    // Create rotation matrix from quaternion
    FMatrix4x4 mat(
        1.0f - 2.0f * (y2 + z2),
        2.0f * (xy - wz),
        2.0f * (xz + wy),
        0.0f,
        2.0f * (xy + wz),
        1.0f - 2.0f * (x2 + z2),
        2.0f * (yz - wx),
        0.0f,
        2.0f * (xz - wy),
        2.0f * (yz + wx),
        1.0f - 2.0f * (x2 + y2),
        0.0f,
        position.x,
        position.y,
        position.z,
        1.0f
    );

    return mat;
}

///////////////////////////////////////////////////////////////////////////////
FMatrix4x4 FPose::GetViewMatrix(void) const
{
    // View matrix is the inverse of the transformation matrix
    return GetMatrix().Inverse();
}

}   // namespace tkd::VR
