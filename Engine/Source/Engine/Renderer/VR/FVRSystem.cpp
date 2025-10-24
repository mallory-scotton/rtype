///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Renderer/VR/FVRSystem.hpp>
#include <Engine/Renderer/VR/FOpenVRBackend.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::VR
///////////////////////////////////////////////////////////////////////////////
namespace tkd::VR
{

///////////////////////////////////////////////////////////////////////////////
FVRSystem::FVRSystem(void)
    : m_backend(nullptr)
    , m_initialized(false)
    , m_hmdPose()
    , m_buttonCallback(nullptr)
    , m_hapticCallback(nullptr)
{}

///////////////////////////////////////////////////////////////////////////////
FVRSystem::~FVRSystem() { Shutdown(); }

///////////////////////////////////////////////////////////////////////////////
bool FVRSystem::Initialize(void)
{
    if (m_initialized) { return true; }

#if TKD_ENGINE_CLIENT
    m_backend = std::make_unique<FOpenVRBackend>();
#endif
    if (!m_backend || !m_backend->Initialize())
    {
        m_backend.reset();
        return false;
    }

    m_initialized = true;
    return true;
}

///////////////////////////////////////////////////////////////////////////////
void FVRSystem::Shutdown(void)
{
    if (!m_initialized) { return; }

    if (m_backend)
    {
        m_backend->Shutdown();
        m_backend.reset();
    }

    m_initialized = false;
}

///////////////////////////////////////////////////////////////////////////////
bool FVRSystem::IsInitialized(void) const { return m_initialized; }

///////////////////////////////////////////////////////////////////////////////
bool FVRSystem::IsHMDPresent(void) const
{
    return m_backend && m_backend->IsHMDPresent();
}

///////////////////////////////////////////////////////////////////////////////
FVRSpecs FVRSystem::GetSpecs(void) const
{
    return m_backend ? m_backend->GetSpecs() : FVRSpecs();
}

///////////////////////////////////////////////////////////////////////////////
FMatrix4x4 FVRSystem::GetProjectionMatrix(
    EEye eye, Float32 nearClip, Float32 farClip
) const
{
    return m_backend ? m_backend->GetProjectionMatrix(eye, nearClip, farClip)
                     : FMatrix4x4();
}

///////////////////////////////////////////////////////////////////////////////
FMatrix4x4 FVRSystem::GetEyeToHeadTransform(EEye eye) const
{
    return m_backend ? m_backend->GetEyeToHeadTransform(eye) : FMatrix4x4();
}

///////////////////////////////////////////////////////////////////////////////
void FVRSystem::BeginFrame(void)
{
    if (!m_backend) { return; }

    UpdatePoses();
    ProcessEvents();
    UpdateInputStates();
}

///////////////////////////////////////////////////////////////////////////////
void FVRSystem::EndFrame(void)
{
    if (!m_backend) { return; }

    // Store previous button states
    m_prevButtonStates = m_buttonStates;
}

///////////////////////////////////////////////////////////////////////////////
void FVRSystem::WaitForSync(void)
{
    if (m_backend) { m_backend->WaitForSync(); }
}

///////////////////////////////////////////////////////////////////////////////
void FVRSystem::UpdatePoses(void)
{
    if (!m_backend) { return; }

    m_backend->UpdatePoses();
    m_hmdPose = m_backend->GetHMDPose();

    m_controllerStates[0] = m_backend->GetControllerState(EHand::Left);
    m_controllerStates[1] = m_backend->GetControllerState(EHand::Right);
}

///////////////////////////////////////////////////////////////////////////////
void FVRSystem::ProcessEvents(void)
{
    if (m_backend) { m_backend->ProcessEvents(); }
}

///////////////////////////////////////////////////////////////////////////////
FControllerState FVRSystem::GetControllerState(EHand hand) const
{
    return m_controllerStates[static_cast<SizeT>(hand)];
}

///////////////////////////////////////////////////////////////////////////////
bool FVRSystem::IsButtonPressed(EHand hand, EButton button) const
{
    if (!m_backend) { return false; }
    return m_backend->IsButtonPressed(hand, button);
}

///////////////////////////////////////////////////////////////////////////////
bool FVRSystem::IsButtonJustPressed(EHand hand, EButton button) const
{
    SizeT handIdx = static_cast<SizeT>(hand);
    SizeT btnIdx = static_cast<SizeT>(button);
    return m_buttonStates[handIdx][btnIdx] &&
           !m_prevButtonStates[handIdx][btnIdx];
}

///////////////////////////////////////////////////////////////////////////////
bool FVRSystem::IsButtonJustReleased(EHand hand, EButton button) const
{
    SizeT handIdx = static_cast<SizeT>(hand);
    SizeT btnIdx = static_cast<SizeT>(button);
    return !m_buttonStates[handIdx][btnIdx] &&
           m_prevButtonStates[handIdx][btnIdx];
}

///////////////////////////////////////////////////////////////////////////////
FVector2 FVRSystem::GetTouchpadAxis(EHand hand) const
{
    return m_controllerStates[static_cast<SizeT>(hand)].touchpad;
}

///////////////////////////////////////////////////////////////////////////////
FVector2 FVRSystem::GetJoystickAxis(EHand hand) const
{
    return m_controllerStates[static_cast<SizeT>(hand)].joystick;
}

///////////////////////////////////////////////////////////////////////////////
Float32 FVRSystem::GetTriggerValue(EHand hand) const
{
    return m_controllerStates[static_cast<SizeT>(hand)].trigger;
}

///////////////////////////////////////////////////////////////////////////////
Float32 FVRSystem::GetGripValue(EHand hand) const
{
    return m_controllerStates[static_cast<SizeT>(hand)].grip;
}

///////////////////////////////////////////////////////////////////////////////
void FVRSystem::TriggerHapticPulse(
    EHand hand, Float32 intensity, Float32 duration
)
{
    if (m_backend)
    {
        m_backend->TriggerHapticPulse(hand, intensity, duration);
    }
    if (m_hapticCallback) { m_hapticCallback(hand, intensity, duration); }
}

///////////////////////////////////////////////////////////////////////////////
void FVRSystem::SubmitFrame(EEye eye, const FRenderTarget& target)
{
    if (m_backend) { m_backend->SubmitFrame(eye, target); }
}

///////////////////////////////////////////////////////////////////////////////
void FVRSystem::SetButtonCallback(ButtonCallback callback)
{
    m_buttonCallback = callback;
}

///////////////////////////////////////////////////////////////////////////////
void FVRSystem::SetHapticCallback(HapticCallback callback)
{
    m_hapticCallback = callback;
}

///////////////////////////////////////////////////////////////////////////////
FVector3 FVRSystem::GetPlayAreaSize(void) const
{
    return m_backend ? m_backend->GetPlayAreaSize() : FVector3::Zero;
}

///////////////////////////////////////////////////////////////////////////////
void FVRSystem::RecenterSeatedPosition(void)
{
    if (m_backend) { m_backend->RecenterSeatedPosition(); }
}

///////////////////////////////////////////////////////////////////////////////
void FVRSystem::UpdateInputStates(void)
{
    if (!m_backend) { return; }

    for (SizeT handIdx = 0; handIdx < 2; ++handIdx)
    {
        EHand hand = static_cast<EHand>(handIdx);
        for (SizeT btnIdx = 0; btnIdx < static_cast<SizeT>(EButton::COUNT);
             ++btnIdx)
        {
            EButton button = static_cast<EButton>(btnIdx);
            m_buttonStates[handIdx][btnIdx] =
                m_backend->IsButtonPressed(hand, button);

            // Check for button state changes and invoke callback
            if (m_buttonCallback)
            {
                if (IsButtonJustPressed(hand, button))
                {
                    m_buttonCallback(hand, button, true);
                }
                else if (IsButtonJustReleased(hand, button))
                {
                    m_buttonCallback(hand, button, false);
                }
            }
        }
    }
}

}   // namespace tkd::VR
