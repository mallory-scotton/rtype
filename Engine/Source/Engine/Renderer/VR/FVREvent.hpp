///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Config.hpp>
#include <Engine/Core/Containers.hpp>
#include <Engine/Renderer/VR/Enumerations.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::VR
///////////////////////////////////////////////////////////////////////////////
namespace tkd::VR
{

///////////////////////////////////////////////////////////////////////////////
/// \brief Enum for VR event types
///
///////////////////////////////////////////////////////////////////////////////
enum class EVREventType
{
    None,
    TrackedDeviceActivated,
    TrackedDeviceDeactivated,
    TrackedDeviceUpdated,
    TrackedDeviceUserInteractionStarted,
    TrackedDeviceUserInteractionEnded,
    ButtonPress,
    ButtonUnpress,
    ButtonTouch,
    ButtonUntouch,
    ChaperoneDataHasChanged,
    ChaperoneUniverseHasChanged,
    ChaperoneSettingsHaveChanged,
    StatusUpdate,
    MCImageUpdated,
    FirmwareUpdateStarted,
    FirmwareUpdateFinished,
    KeyboardClosed,
    KeyboardCharInput,
    KeyboardDone,
    ApplicationTransitionStarted,
    ApplicationTransitionAborted,
    ApplicationTransitionNewAppStarted,
    Compositor_ChaperoneBoundsShown,
    Compositor_ChaperoneBoundsHidden,
    TrackedCamera_StartVideoStream,
    TrackedCamera_StopVideoStream,
    TrackedCamera_PauseVideoStream,
    TrackedCamera_ResumeVideoStream,
    PerformanceTest_EnableCapture,
    PerformanceTest_DisableCapture,
    PerformanceTest_FidelityLevel,
    MessageOverlay_Closed,
    Quit,
    ProcessQuit,
    QuitAcknowledged,
    DriverRequestedQuit,
    RestartRequested
};

///////////////////////////////////////////////////////////////////////////////
/// \brief Structure representing a VR system event
///
///////////////////////////////////////////////////////////////////////////////
class FVREvent
{
public:
    ///////////////////////////////////////////////////////////////////////////
    // Class Member
    ///////////////////////////////////////////////////////////////////////////
    EVREventType eventType;   //<! Type of the event
    UInt32
        trackedDeviceIndex;   //<! Index of the tracked device (if applicable)
    Float32 eventAge;         //<! Age of the event in seconds

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor
    ///
    ///////////////////////////////////////////////////////////////////////////
    FVREvent(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor with parameters
    ///
    /// \param type Event type
    /// \param deviceIndex Tracked device index
    /// \param age Event age
    ///
    ///////////////////////////////////////////////////////////////////////////
    FVREvent(EVREventType type, UInt32 deviceIndex, Float32 age);
};

}   // namespace tkd::VR
