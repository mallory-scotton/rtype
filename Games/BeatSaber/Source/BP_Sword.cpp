///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <BP_Sword.hpp>
#include <AC_SwordBlade.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
BP_Sword::BP_Sword(const FString& name, VR::EHand hand)
    : AActor(name)
    , m_hand(hand)
{
    // Add Components to act as the sword's visual representation and collision
    AddComponent<AC_SwordBlade>("SwordBladeCollider", m_hand);
}

///////////////////////////////////////////////////////////////////////////////
void BP_Sword::BeginPlay(void)
{
    // Call the base class BeginPlay
    Super::BeginPlay();
}

///////////////////////////////////////////////////////////////////////////////
void BP_Sword::Tick(Float32 deltaTime)
{
    // Call the base class Tick
    Super::Tick(deltaTime);

    // Per-frame update code for BP_Sword can be added here
#if TKD_ENGINE_CLIENT
    VR::FVRSystem& vrSystem = Window::GetVRSystem();

    if (vrSystem.IsInitialized())
    {
        VR::FControllerState ctrl = vrSystem.GetControllerState(m_hand);

        if (vrSystem.IsButtonPressed(m_hand, VR::EButton::ButtonA))
        {
            // Haptic Pulse on trigger press
            vrSystem.TriggerHapticPulse(m_hand, 1.0f, 0.04f);
        }

        if (ctrl.pose.isValid)
        {
            // Create player rotation matrix for controllers (rotation only, no
            // translation)
            float yawRad = 0.0f * (3.14159265359f / 180.0f);
            float cosYaw = std::cos(yawRad);
            float sinYaw = std::sin(yawRad);

            FMatrix4x4 playerRotation;
            playerRotation(0, 0) = cosYaw;
            playerRotation(0, 1) = 0.0f;
            playerRotation(0, 2) = sinYaw;
            playerRotation(0, 3) = 0.0f;
            playerRotation(1, 0) = 0.0f;
            playerRotation(1, 1) = 1.0f;
            playerRotation(1, 2) = 0.0f;
            playerRotation(1, 3) = 0.0f;
            playerRotation(2, 0) = -sinYaw;
            playerRotation(2, 1) = 0.0f;
            playerRotation(2, 2) = cosYaw;
            playerRotation(2, 3) = 0.0f;
            playerRotation(3, 0) = 0.0f;
            playerRotation(3, 1) = 0.0f;
            playerRotation(3, 2) = 0.0f;
            playerRotation(3, 3) = 1.0f;

            // Build controller local transform matrix with rotation
            FMatrix4x4 localTransform = ctrl.pose.rotation.ToMatrix4x4();
            localTransform(0, 3) = ctrl.pose.position.x;
            localTransform(1, 3) = ctrl.pose.position.y;
            localTransform(2, 3) = ctrl.pose.position.z;

            // Apply player rotation: playerRotation * localTransform
            FMatrix4x4 worldTransform;
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    worldTransform(i, j) = 0;
                    for (int k = 0; k < 4; k++)
                    {
                        worldTransform(i, j) +=
                            playerRotation(i, k) * localTransform(k, j);
                    }
                }
            }

            // Add player position translation
            worldTransform(0, 3) += Window::GetCamera().position.x;
            worldTransform(1, 3) += Window::GetCamera().position.y;
            worldTransform(2, 3) += Window::GetCamera().position.z;

            // Update the sword's transform
            FTransform newTransform(worldTransform);
            SetTransform(newTransform);
        }
    }
#endif
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_Sword, AActor)

}   // namespace tkd
