///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Runtime/Physics/FOBB.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
FOBB::FOBB(void)
    : center(FVector3::Zero)
    , extents(FVector3::Zero)
{
    axes[0] = FVector3::UnitX;
    axes[1] = FVector3::UnitY;
    axes[2] = FVector3::UnitZ;
}

///////////////////////////////////////////////////////////////////////////////
void FOBB::UpdateFromTransform(
    const FTransform& transform, const FVector3& halfExtents
)
{
    center = transform.GetPosition();
    extents = halfExtents;

    // Extract rotation axes from transform
    FRotator rotation = transform.GetRotation();

    // Convert rotation to axes (simplified - you'd use your math library)
    Float32 pitch = rotation.GetPitch() * M_PI / 180.0f;
    Float32 yaw = rotation.GetYaw() * M_PI / 180.0f;
    Float32 roll = rotation.GetRoll() * M_PI / 180.0f;

    Float32 cp = std::cos(pitch);
    Float32 sp = std::sin(pitch);
    Float32 cy = std::cos(yaw);
    Float32 sy = std::sin(yaw);
    Float32 cr = std::cos(roll);
    Float32 sr = std::sin(roll);

    // Right axis (X)
    axes[0] = FVector3(cy * cr, cy * sr, -sy);

    // Up axis (Y)
    axes[1] =
        FVector3(sp * sy * cr - cp * sr, sp * sy * sr + cp * cr, sp * cy);

    // Forward axis (Z)
    axes[2] =
        FVector3(cp * sy * cr + sp * sr, cp * sy * sr - sp * cr, cp * cy);
}

///////////////////////////////////////////////////////////////////////////////
void FOBB::GetCorners(FVector3 corners[8]) const
{
    FVector3 offsets[8] = { FVector3(-1, -1, -1), FVector3(1, -1, -1),
                            FVector3(1, 1, -1),   FVector3(-1, 1, -1),
                            FVector3(-1, -1, 1),  FVector3(1, -1, 1),
                            FVector3(1, 1, 1),    FVector3(-1, 1, 1) };

    for (int i = 0; i < 8; ++i)
    {
        corners[i] = center;
        for (int j = 0; j < 3; ++j)
        {
            corners[i] = corners[i] + axes[j] * (extents[j] * offsets[i][j]);
        }
    }
}

}   // namespace tkd
