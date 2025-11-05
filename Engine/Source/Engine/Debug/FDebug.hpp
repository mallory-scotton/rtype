///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Config.hpp>
#include <Engine/Core/Utils/Singleton.hpp>
#include <Engine/Debug/FActorDebug.hpp>
#include <Engine/Debug/FCameraDebug.hpp>
#include <Engine/Debug/FEngineSettingsDebug.hpp>
#include <Engine/Debug/FInputsDebug.hpp>
#include <Engine/Debug/FNetworkDebug.hpp>
#include <Engine/Debug/FPerformanceMonitorDebug.hpp>
#include <Engine/Debug/FWorldDebug.hpp>
#include <Engine/Debug/IDebugInterface.hpp>

#if TKD_ENGINE_CLIENT
    #include <imgui.h>
#endif

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::debug
///////////////////////////////////////////////////////////////////////////////
namespace tkd::debug
{

#if TKD_ENGINE_CLIENT

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
class FDebug
    : public TSingleton<FDebug>
    , public IDebugInterface
{
private:
    ///////////////////////////////////////////////////////////////////////////
    // Debug Window Flags
    ///////////////////////////////////////////////////////////////////////////
    bool m_showEngineSettings = false;       //<! Show engine settings flag
    bool m_showPerformanceMonitor = false;   //<! Show performance monitor flag
    bool m_showMemoryProfiler = false;       //<! Show memory profiler flag
    bool m_showRenderDebug = false;          //<! Show render debug flag
    bool m_showCameraDebug = false;          //<! Show camera debug flag
    bool m_showNetworkMonitor = false;       //<! Show network monitor flag
    bool m_showPacketInspector = false;      //<! Show packet inspector flag
    bool m_showInputDebug = false;           //<! Show input debug flag
    bool m_showActorDebug = false;           //<! Show actor debug flag
    bool m_showWorldDebug = false;           //<! Show world debug flag

    ///////////////////////////////////////////////////////////////////////////
    // Debug Interface Instances
    ///////////////////////////////////////////////////////////////////////////
    FEngineSettingsDebug m_engineSettings;   //<! Engine settings instance
    FInputsDebug m_inputsDebug;              //<! Inputs debug instance
    FActorDebug m_actorDebug;                //<! Actor debug instance
    FNetworkDebug m_networkDebug;            //<! Network debug instance
    FPerformanceMonitorDebug
        m_performanceMonitor;                //<! Performance monitor instance
    FWorldDebug m_worldDebug;                //<! World debug instance
    FCameraDebug m_cameraDebug;              //<! Camera debug instance

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Apply debug menu styling
    ///
    ///////////////////////////////////////////////////////////////////////////
    void ApplyDebugMenuStyling(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Pop debug menu styling
    ///
    ///////////////////////////////////////////////////////////////////////////
    void PopDebugMenuStyling(void);

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Show the debug interface
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void Show(const FEngineSettings& settings, UWorld* world) override;
};

#endif

}   // namespace tkd::debug
