///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Renderer/FCamera.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
FCamera::FCamera(void)
    : position(0.0f, 2.0f, 10.0f)
    , front(0.0f, 0.0f, -1.0f)
    , up(0.0f, 1.0f, 0.0f)
    , right(1.0f, 0.0f, 0.0f)
    , yaw(-90.0f)
    , pitch(0.0f)
    , moveSpeed(5.0f)
    , mouseSensitivity(0.05f)
    , fov(60.0f)
    , aspectRatio(4.0f / 3.0f)
    , nearPlane(0.1f)
    , farPlane(1000.0f)
{
    UpdateCameraVectors();
}

///////////////////////////////////////////////////////////////////////////////
FCamera::FCamera(Float32 fov, Float32 aspect, Float32 near, Float32 far)
    : position(0.0f, 2.0f, 10.0f)
    , front(0.0f, 0.0f, -1.0f)
    , up(0.0f, 1.0f, 0.0f)
    , right(1.0f, 0.0f, 0.0f)
    , yaw(-90.0f)
    , pitch(0.0f)
    , moveSpeed(5.0f)
    , mouseSensitivity(0.05f)
    , fov(fov)
    , aspectRatio(aspect)
    , nearPlane(near)
    , farPlane(far)
{
    UpdateCameraVectors();
}

///////////////////////////////////////////////////////////////////////////////
void FCamera::UpdateCameraVectors(void)
{
    FVector3 newFront;

    newFront.x =
        std::cos(yaw * M_PI / 180.0f) * std::cos(pitch * M_PI / 180.0f);
    newFront.y = std::sin(pitch * M_PI / 180.0f);
    newFront.z =
        std::sin(yaw * M_PI / 180.0f) * std::cos(pitch * M_PI / 180.0f);

    front = FVector3::Normalize(newFront);
    right = FVector3::Normalize(FVector3::Cross(front, FVector3::Up));
    up = FVector3::Normalize(FVector3::Cross(right, front));
}

}   // namespace tkd
