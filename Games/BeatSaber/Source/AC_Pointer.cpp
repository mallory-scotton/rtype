///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <AC_Pointer.hpp>
#include <ST_State.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
AC_Pointer::AC_Pointer(const FString& name, VR::EHand hand)
    : UCollisionComponent(name)
    , m_hand(hand)
    , m_length(10.0f)
    , m_thickness(0.01f)
{}

///////////////////////////////////////////////////////////////////////////////
FOBB AC_Pointer::GetLocalBoundingBox(void) const
{
    FOBB bound;

    // Centered along the blade length
    bound.center = FVector3(0.0f, 0.0f, -m_length / 2.0f);

    // Half-extents: covers the full blade length
    float halfThickness = m_thickness / 2.0f;
    bound.extents = FVector3(halfThickness, halfThickness, m_length / 2.0f);

    // Local axes (identity - no rotation in local space)
    bound.axes[0] = FVector3(1.0f, 0.0f, 0.0f);   // X-axis
    bound.axes[1] = FVector3(0.0f, 1.0f, 0.0f);   // Y-axis
    bound.axes[2] = FVector3(0.0f, 0.0f, 1.0f);   // Z-axis

    return bound;
}

///////////////////////////////////////////////////////////////////////////////
FOBB AC_Pointer::GetWorldBoundingBox(void) const
{
    FOBB bound;

    // Get the owner's transform
    FTransform transform = GetOwner()->GetTransform();

    // Get local bounding box
    FOBB localBound = GetLocalBoundingBox();

    // Transform the center to world space
    bound.center = transform.TransformPoint(localBound.center);

    // Transform the axes to world space (rotate but don't translate)
    bound.axes[0] = transform.TransformDirection(localBound.axes[0]);
    bound.axes[1] = transform.TransformDirection(localBound.axes[1]);
    bound.axes[2] = transform.TransformDirection(localBound.axes[2]);

    // Normalize axes (in case transform has scaling)
    bound.axes[0] = bound.axes[0].Normalized();
    bound.axes[1] = bound.axes[1].Normalized();
    bound.axes[2] = bound.axes[2].Normalized();

    // Extents remain the same (assuming uniform scaling or no scaling)
    // If you need to handle non-uniform scaling, you'd need to scale extents
    // along each axis based on the transform's scale
    bound.extents = localBound.extents;

    return bound;
}

///////////////////////////////////////////////////////////////////////////////
FAABB AC_Pointer::GetWorldAABB(void) const
{
    // Get the world-space OBB
    FOBB worldOBB = GetWorldBoundingBox();

    // Get all 8 corners of the OBB
    FVector3 corners[8];
    worldOBB.GetCorners(corners);

    // Initialize min/max with the first corner
    FVector3 min = corners[0];
    FVector3 max = corners[0];

    // Find the min and max bounds across all corners
    for (int i = 1; i < 8; i++)
    {
        min.x = std::min(min.x, corners[i].x);
        min.y = std::min(min.y, corners[i].y);
        min.z = std::min(min.z, corners[i].z);

        max.x = std::max(max.x, corners[i].x);
        max.y = std::max(max.y, corners[i].y);
        max.z = std::max(max.z, corners[i].z);
    }

    return FAABB(min, max);
}

///////////////////////////////////////////////////////////////////////////////
void AC_Pointer::BeginPlay(void)
{
    // Call parent BeginPlay
    Super::BeginPlay();
}

///////////////////////////////////////////////////////////////////////////////
void AC_Pointer::Tick(float deltaTime)
{
    // Call parent Tick
    Super::Tick(deltaTime);

#if TKD_ENGINE_CLIENT
    // Update the controller state
    VR::FVRSystem& vrSystem = Window::GetVRSystem();

    if (vrSystem.IsInitialized())
    {
        m_state = vrSystem.GetControllerState(m_hand);
    }

    // Get state
    auto& stateManager = ST_State::GetInstance();
    if (stateManager.lastMenuHand == m_hand)
    {
        SetHiddenInGame(false);
        SetShowDebug(true);
    }
    else
    {
        SetHiddenInGame(true);
        SetShowDebug(false);
    }
#else
    TKD_UNUSED(deltaTime);
#endif
}

///////////////////////////////////////////////////////////////////////////////
void AC_Pointer::Render(IRenderer& renderer) const
{
    // Call parent Render
    Super::Render(renderer);
}

}   // namespace tkd
