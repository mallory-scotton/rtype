///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Renderer/VR/FOpenVRBackend.hpp>
#include <Engine/Core/Utils/FLogger.hpp>
#include <Engine/Renderer/VR/FVREvent.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::VR
///////////////////////////////////////////////////////////////////////////////
namespace tkd::VR
{

#if TKD_ENGINE_CLIENT

///////////////////////////////////////////////////////////////////////////////
FOpenVRBackend::FOpenVRBackend(void)
    : m_system(nullptr)
    , m_hapticTimers()
{
    m_controllerIndices[0] = -1;
    m_controllerIndices[1] = -1;
    m_hapticTimers[EHand::Left] = 0.0f;
    m_hapticTimers[EHand::Right] = 0.0f;
}

///////////////////////////////////////////////////////////////////////////////
FOpenVRBackend::~FOpenVRBackend(void) { Shutdown(); }

///////////////////////////////////////////////////////////////////////////////
bool FOpenVRBackend::Initialize(void)
{
    vr::EVRInitError initError = vr::VRInitError_None;
    m_system = vr::VR_Init(&initError, vr::VRApplication_Scene);

    if (initError != vr::VRInitError_None || !m_system)
    {
        m_system = nullptr;
        return false;
    }

    if (!vr::VRCompositor())
    {
        vr::VR_Shutdown();
        m_system = nullptr;
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
void FOpenVRBackend::Shutdown(void)
{
    if (m_system)
    {
        vr::VR_Shutdown();
        m_system = nullptr;
    }
}

///////////////////////////////////////////////////////////////////////////////
bool FOpenVRBackend::IsHMDPresent(void) const
{
    return m_system && vr::VR_IsHmdPresent();
}

///////////////////////////////////////////////////////////////////////////////
FVRSpecs FOpenVRBackend::GetSpecs(void) const
{
    FVRSpecs specs;
    if (!m_system) { return specs; }

    m_system->GetRecommendedRenderTargetSize(
        &specs.recommendedWidth, &specs.recommendedHeight
    );
    specs.refreshRate = m_system->GetFloatTrackedDeviceProperty(
        vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_DisplayFrequency_Float
    );

    char buffer[256];
    m_system->GetStringTrackedDeviceProperty(
        vr::k_unTrackedDeviceIndex_Hmd,
        vr::Prop_ModelNumber_String,
        buffer,
        sizeof(buffer)
    );
    specs.deviceName = buffer;

    return specs;
}

///////////////////////////////////////////////////////////////////////////////
FMatrix4x4 FOpenVRBackend::GetProjectionMatrix(
    EEye eye, Float32 nearClip, Float32 farClip
) const
{
    if (!m_system) { return FMatrix4x4(); }

    vr::Hmd_Eye vrEye = (eye == EEye::Left) ? vr::Eye_Left : vr::Eye_Right;

    vr::HmdMatrix44_t proj =
        m_system->GetProjectionMatrix(vrEye, nearClip, farClip);

    return FMatrix4x4(
        proj.m[0][0],
        proj.m[1][0],
        proj.m[2][0],
        proj.m[3][0],
        proj.m[0][1],
        proj.m[1][1],
        proj.m[2][1],
        proj.m[3][1],
        proj.m[0][2],
        proj.m[1][2],
        proj.m[2][2],
        proj.m[3][2],
        proj.m[0][3],
        proj.m[1][3],
        proj.m[2][3],
        proj.m[3][3]
    );
}

///////////////////////////////////////////////////////////////////////////////
FMatrix4x4 FOpenVRBackend::GetEyeToHeadTransform(EEye eye) const
{
    if (!m_system) { return FMatrix4x4(); }

    vr::Hmd_Eye vrEye = (eye == EEye::Left) ? vr::Eye_Left : vr::Eye_Right;

    vr::HmdMatrix34_t transform = m_system->GetEyeToHeadTransform(vrEye);

    return FMatrix4x4(
        transform.m[0][0],
        transform.m[0][1],
        transform.m[0][2],
        transform.m[0][3],
        transform.m[1][0],
        transform.m[1][1],
        transform.m[1][2],
        transform.m[1][3],
        transform.m[2][0],
        transform.m[2][1],
        transform.m[2][2],
        transform.m[2][3],
        0.0f,
        0.0f,
        0.0f,
        1.0f
    );
}

///////////////////////////////////////////////////////////////////////////////
void FOpenVRBackend::WaitForSync(void)
{
    if (!m_system) { return; }

    vr::VRCompositor()->WaitGetPoses(
        m_poses, vr::k_unMaxTrackedDeviceCount, nullptr, 0
    );

    // Update controller indices
    m_controllerIndices[0] = -1;
    m_controllerIndices[1] = -1;

    for (uint32_t i = 0; i < vr::k_unMaxTrackedDeviceCount; ++i)
    {
        if (!m_poses[i].bPoseIsValid) { continue; }

        if (m_system->GetTrackedDeviceClass(i) ==
            vr::TrackedDeviceClass_Controller)
        {
            auto role = m_system->GetControllerRoleForTrackedDeviceIndex(i);
            if (role == vr::TrackedControllerRole_LeftHand)
            {
                m_controllerIndices[0] = i;
            }
            else if (role == vr::TrackedControllerRole_RightHand)
            {
                m_controllerIndices[1] = i;
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void FOpenVRBackend::UpdatePoses(void)
{
    // Poses are updated in WaitForSync
    WaitForSync();

    static const float maxHapticDuration = 0.04f;   // seconds
    static const UInt16 microseconds =
        static_cast<UInt16>(maxHapticDuration * 1000000.0f);

    // Update haptic timers
    if (m_hapticTimers[EHand::Left] > 0.0f)
    {
        int idx = m_controllerIndices[static_cast<SizeT>(EHand::Left)];
        if (idx != -1)
        {
            m_system->TriggerHapticPulse(idx, 0, microseconds);
            m_hapticTimers[EHand::Left] -= maxHapticDuration;
        }
    }

    if (m_hapticTimers[EHand::Right] > 0.0f)
    {
        int idx = m_controllerIndices[static_cast<SizeT>(EHand::Right)];
        if (idx != -1)
        {
            m_system->TriggerHapticPulse(idx, 0, microseconds);
            m_hapticTimers[EHand::Right] -= maxHapticDuration;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
std::vector<FVREvent> FOpenVRBackend::ProcessEvents(void)
{
    std::vector<FVREvent> events;
    if (!m_system) { return events; }

    vr::VREvent_t event;
    while (m_system->PollNextEvent(&event, sizeof(event)))
    {
        EVREventType eventType = EVREventType::None;

        // Map OpenVR events to our event types
        switch (event.eventType)
        {
        case vr::VREvent_TrackedDeviceActivated:
            eventType = EVREventType::TrackedDeviceActivated;
            break;
        case vr::VREvent_TrackedDeviceDeactivated:
            eventType = EVREventType::TrackedDeviceDeactivated;
            break;
        case vr::VREvent_TrackedDeviceUpdated:
            eventType = EVREventType::TrackedDeviceUpdated;
            break;
        case vr::VREvent_TrackedDeviceUserInteractionStarted:
            eventType = EVREventType::TrackedDeviceUserInteractionStarted;
            break;
        case vr::VREvent_TrackedDeviceUserInteractionEnded:
            eventType = EVREventType::TrackedDeviceUserInteractionEnded;
            break;
        case vr::VREvent_ButtonPress:
            eventType = EVREventType::ButtonPress;
            break;
        case vr::VREvent_ButtonUnpress:
            eventType = EVREventType::ButtonUnpress;
            break;
        case vr::VREvent_ButtonTouch:
            eventType = EVREventType::ButtonTouch;
            break;
        case vr::VREvent_ButtonUntouch:
            eventType = EVREventType::ButtonUntouch;
            break;
        case vr::VREvent_ChaperoneDataHasChanged:
            eventType = EVREventType::ChaperoneDataHasChanged;
            break;
        case vr::VREvent_ChaperoneUniverseHasChanged:
            eventType = EVREventType::ChaperoneUniverseHasChanged;
            break;
        case vr::VREvent_ChaperoneSettingsHaveChanged:
            eventType = EVREventType::ChaperoneSettingsHaveChanged;
            break;
        case vr::VREvent_Quit: eventType = EVREventType::Quit; break;
        case vr::VREvent_ProcessQuit:
            eventType = EVREventType::ProcessQuit;
            break;
        case vr::VREvent_QuitAcknowledged:
            eventType = EVREventType::QuitAcknowledged;
            break;
        case vr::VREvent_DriverRequestedQuit:
            eventType = EVREventType::DriverRequestedQuit;
            break;
        case vr::VREvent_RestartRequested:
            eventType = EVREventType::RestartRequested;
            break;
        default: eventType = EVREventType::None; break;
        }

        if (eventType != EVREventType::None)
        {
            events.push_back(FVREvent(
                eventType, event.trackedDeviceIndex, event.eventAgeSeconds
            ));
        }
    }

    return events;
}

///////////////////////////////////////////////////////////////////////////////
FPose FOpenVRBackend::ConvertPose(const vr::TrackedDevicePose_t& pose) const
{
    FPose result;
    const vr::HmdMatrix34_t& mat = pose.mDeviceToAbsoluteTracking;

    result.position.x = mat.m[0][3];
    result.position.y = mat.m[1][3];
    result.position.z = mat.m[2][3];

    result.rotation.FromMatrix4x4(FMatrix4x4(
        mat.m[0][0],
        mat.m[0][1],
        mat.m[0][2],
        0.0f,
        mat.m[1][0],
        mat.m[1][1],
        mat.m[1][2],
        0.0f,
        mat.m[2][0],
        mat.m[2][1],
        mat.m[2][2],
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        1.0f
    ));

    result.velocity.x = pose.vVelocity.v[0];
    result.velocity.y = pose.vVelocity.v[1];
    result.velocity.z = pose.vVelocity.v[2];

    result.angularVelocity.x = pose.vAngularVelocity.v[0];
    result.angularVelocity.y = pose.vAngularVelocity.v[1];
    result.angularVelocity.z = pose.vAngularVelocity.v[2];

    result.isValid = true;
    return result;
}

///////////////////////////////////////////////////////////////////////////////
FPose FOpenVRBackend::GetHMDPose(void) const
{
    if (!m_system || !m_poses[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid)
    {
        return FPose();
    }
    return ConvertPose(m_poses[vr::k_unTrackedDeviceIndex_Hmd]);
}

///////////////////////////////////////////////////////////////////////////////
FControllerState FOpenVRBackend::GetControllerState(EHand hand) const
{
    FControllerState state;

    int idx = m_controllerIndices[static_cast<SizeT>(hand)];
    if (idx == -1 || !m_poses[idx].bPoseIsValid) { return state; }

    state.pose = ConvertPose(m_poses[idx]);
    state.isConnected = true;

    vr::VRControllerState_t vrState;
    if (m_system->GetControllerState(idx, &vrState, sizeof(vrState)))
    {
        // Trigger
        if (vrState.ulButtonPressed &
            vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Trigger))
        {
            state.trigger = vrState.rAxis[1].x;
        }

        // Grip
        if (vrState.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_Grip))
        {
            state.grip = 1.0f;
        }

        // Touchpad/Joystick
        state.touchpad = FVector2(vrState.rAxis[0].x, vrState.rAxis[0].y);
        state.joystick = state.touchpad;
    }

    return state;
}

///////////////////////////////////////////////////////////////////////////////
bool FOpenVRBackend::IsButtonPressed(EHand hand, EButton button) const
{
    int idx = m_controllerIndices[static_cast<SizeT>(hand)];
    if (idx == -1) { return false; }

    vr::VRControllerState_t state;
    if (!m_system->GetControllerState(idx, &state, sizeof(state)))
    {
        return false;
    }

    vr::EVRButtonId vrButton;
    switch (button)
    {
    case EButton::Trigger : vrButton = vr::k_EButton_SteamVR_Trigger; break;
    case EButton::Grip    : vrButton = vr::k_EButton_Grip; break;
    case EButton::Menu    : vrButton = vr::k_EButton_ApplicationMenu; break;
    case EButton::Touchpad: vrButton = vr::k_EButton_SteamVR_Touchpad; break;
    case EButton::Joystick:
        vrButton = vr::k_EButton_SteamVR_Touchpad;
        break;   // Many controllers use touchpad as joystick
    case EButton::ButtonA: vrButton = vr::k_EButton_A; break;
    case EButton::ButtonB:
        vrButton = vr::k_EButton_ApplicationMenu;
        break;   // Fallback to menu
    default: return false;
    }

    return (state.ulButtonPressed & vr::ButtonMaskFromId(vrButton)) != 0;
}

///////////////////////////////////////////////////////////////////////////////
void FOpenVRBackend::TriggerHapticPulse(EHand hand, Float32 duration)
{
    int idx = m_controllerIndices[static_cast<SizeT>(hand)];
    if (idx == -1 || !m_system) { return; }

    if (m_hapticTimers[hand] <= duration) { m_hapticTimers[hand] = duration; }
}

///////////////////////////////////////////////////////////////////////////////
void FOpenVRBackend::SubmitFrame(EEye eye, const FRenderTarget& target)
{
    if (!m_system) { return; }

    // Validate texture
    if (target.textureID == 0)
    {
        FLogger::SetNamespace("VR");
        FLogger::Error(
            "Attempting to submit invalid texture ID for {} eye",
            (eye == EEye::Left ? "Left" : "Right")
        );
        return;
    }

    vr::Texture_t vrTexture = {
        reinterpret_cast<void*>(static_cast<SizeT>(target.textureID)),
        vr::TextureType_OpenGL,
        vr::ColorSpace_Gamma
    };

    // OpenGL textures are bottom-left origin
    vr::VRTextureBounds_t bounds;
    bounds.uMin = 0.0f;
    bounds.uMax = 1.0f;
    bounds.vMin = 0.0f;   // Bottom
    bounds.vMax = 1.0f;   // Top

    vr::EVREye vrEye = (eye == EEye::Left) ? vr::Eye_Left : vr::Eye_Right;

    vr::EVRCompositorError err =
        vr::VRCompositor()->Submit(vrEye, &vrTexture, &bounds);

    if (err != vr::VRCompositorError_None)
    {
        FLogger::SetNamespace("VR");
        FLogger::Error(
            "Compositor submit failed for {} eye: Error code {}",
            (eye == EEye::Left ? "Left" : "Right"),
            static_cast<int>(err)
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
FVector3 FOpenVRBackend::GetPlayAreaSize() const
{
    if (!m_system) { return FVector3::Zero; }

    Float32 sizeX = 0.0f, sizeZ = 0.0f;
    if (vr::VRChaperone()->GetPlayAreaSize(&sizeX, &sizeZ))
    {
        return FVector3(sizeX, 0.0f, sizeZ);
    }
    return FVector3::Zero;
}

///////////////////////////////////////////////////////////////////////////////
void FOpenVRBackend::RecenterSeatedPosition(void)
{
    if (!m_system) { return; }

    vr::VRChaperone()->ResetZeroPose(vr::TrackingUniverseSeated);
}

///////////////////////////////////////////////////////////////////////////////
void FOpenVRBackend::SetTrackingUniverse(ETrackingUniverse universe)
{
    if (!m_system) { return; }

    vr::ETrackingUniverseOrigin origin;
    switch (universe)
    {
    case ETrackingUniverse::Seated: origin = vr::TrackingUniverseSeated; break;
    case ETrackingUniverse::Standing:
        origin = vr::TrackingUniverseStanding;
        break;
    case ETrackingUniverse::RawAndUncalibrated:
        origin = vr::TrackingUniverseRawAndUncalibrated;
        break;
    default: origin = vr::TrackingUniverseStanding; break;
    }

    vr::VRCompositor()->SetTrackingSpace(origin);
}

///////////////////////////////////////////////////////////////////////////////
ETrackingUniverse FOpenVRBackend::GetTrackingUniverse(void) const
{
    if (!m_system) { return ETrackingUniverse::Standing; }

    vr::ETrackingUniverseOrigin origin =
        vr::VRCompositor()->GetTrackingSpace();

    switch (origin)
    {
    case vr::TrackingUniverseSeated  : return ETrackingUniverse::Seated;
    case vr::TrackingUniverseStanding: return ETrackingUniverse::Standing;
    case vr::TrackingUniverseRawAndUncalibrated:
        return ETrackingUniverse::RawAndUncalibrated;
    default: return ETrackingUniverse::Standing;
    }
}

///////////////////////////////////////////////////////////////////////////////
Float32 FOpenVRBackend::GetControllerBattery(EHand hand) const
{
    if (!m_system) { return -1.0f; }

    int idx = m_controllerIndices[static_cast<SizeT>(hand)];
    if (idx == -1) { return -1.0f; }

    vr::ETrackedPropertyError error;
    Float32 battery = m_system->GetFloatTrackedDeviceProperty(
        idx, vr::Prop_DeviceBatteryPercentage_Float, &error
    );

    if (error != vr::TrackedProp_Success) { return -1.0f; }

    return battery;
}

///////////////////////////////////////////////////////////////////////////////
std::vector<FVector3> FOpenVRBackend::GetPlayAreaBounds(void) const
{
    std::vector<FVector3> bounds;
    if (!m_system) { return bounds; }

    vr::HmdQuad_t rect;
    if (vr::VRChaperone()->GetPlayAreaRect(&rect))
    {
        // Add all 4 corners
        for (int i = 0; i < 4; ++i)
        {
            bounds.push_back(FVector3(
                rect.vCorners[i].v[0],
                rect.vCorners[i].v[1],
                rect.vCorners[i].v[2]
            ));
        }
    }

    return bounds;
}

#endif

}   // namespace tkd::VR
