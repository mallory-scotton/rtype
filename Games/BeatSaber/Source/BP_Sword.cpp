///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <BP_Sword.hpp>

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
    AddComponent<UCubeComponent>("SwordBlade");
}

///////////////////////////////////////////////////////////////////////////////
void BP_Sword::BeginPlay(void)
{
    // Call the base class BeginPlay
    Super::BeginPlay();

    // Initialization code for BP_Sword can be added here
    auto blade = GetComponent<UCubeComponent>("SwordBlade");
    if (blade)
    {
        FTransform transform = FTransform::Identity;
        transform.SetScale(FVector3(0.05f, 1.0f, 0.05f));
        transform.SetRotation(FRotator(0.0f, 0.0f, 90.0f));
        blade->GetCube().SetOrigin(FVector3(0.0f, 0.5f, 0.0f));
        blade->SetLocalTransform(transform);
        if (m_hand == VR::EHand::Left)
        {
            blade->GetCube().SetColor(FColor::Blue);
        }
        else { blade->GetCube().SetColor(FColor::Red); }
    }
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
            // Get rotation matrix from controller
            FVector3 rotation = ctrl.pose.rotation.ToEulerAngles();

            // Calculate final position (controller position + camera/player
            // position)
            FVector3 finalPosition =
                ctrl.pose.position + Window::GetCamera().position;

            // Create transform from rotation and position separately
            FTransform newTransform;
            newTransform.SetRotation(
                FRotator(-rotation.x, -rotation.y, rotation.z)
            );
            newTransform.SetPosition(finalPosition);

            // Update the sword's transform
            SetTransform(newTransform);
        }
    }
#endif
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_Sword, AActor)

}   // namespace tkd
