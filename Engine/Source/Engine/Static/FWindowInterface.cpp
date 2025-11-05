///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Static/FWindowInterface.hpp>
#include <Engine/Static/FEngineInterface.hpp>
#include <Engine/Static/FWindowSubsystem.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

#if TKD_ENGINE_CLIENT

///////////////////////////////////////////////////////////////////////////////
std::mutex FWindowInterface::s_mutex;

///////////////////////////////////////////////////////////////////////////////
__internal::FWindowSubsystem* FWindowInterface::GetWindowSubsystem(void)
{
    std::lock_guard lock(s_mutex);

    try
    {
        auto& engine = FEngineInterface::GetInstance();
        return engine.GetWindow();
    }
    catch (const std::exception&)
    {
        return nullptr;
    }
}

///////////////////////////////////////////////////////////////////////////////
bool FWindowInterface::IsOpen(void)
{
    auto* windowSubsystem = GetWindowSubsystem();
    if (!windowSubsystem) { return false; }

    return windowSubsystem->IsOpen();
}

///////////////////////////////////////////////////////////////////////////////
FVector2u FWindowInterface::GetDimensions(void)
{
    auto* windowSubsystem = GetWindowSubsystem();
    if (!windowSubsystem) { return FVector2u::Zero; }

    return windowSubsystem->GetDimensions();
}

///////////////////////////////////////////////////////////////////////////////
UInt32 FWindowInterface::GetWidth(void)
{
    auto* windowSubsystem = GetWindowSubsystem();
    if (!windowSubsystem) { return 0; }

    return windowSubsystem->GetDimensions().x;
}

///////////////////////////////////////////////////////////////////////////////
UInt32 FWindowInterface::GetHeight(void)
{
    auto* windowSubsystem = GetWindowSubsystem();
    if (!windowSubsystem) { return 0; }

    return windowSubsystem->GetDimensions().y;
}

///////////////////////////////////////////////////////////////////////////////
void FWindowInterface::SetTitle(const FString& title)
{
    auto* windowSubsystem = GetWindowSubsystem();
    if (!windowSubsystem) { return; }

    windowSubsystem->SetTitle(title);
}

///////////////////////////////////////////////////////////////////////////////
float FWindowInterface::GetFPS(void)
{
    auto* windowSubsystem = GetWindowSubsystem();
    if (!windowSubsystem) { return 0.0f; }

    return windowSubsystem->GetFPS();
}

///////////////////////////////////////////////////////////////////////////////
float FWindowInterface::GetAverageFrameTime(void)
{
    auto* windowSubsystem = GetWindowSubsystem();
    if (!windowSubsystem) { return 0.0f; }

    return windowSubsystem->GetAverageFrameTime();
}

///////////////////////////////////////////////////////////////////////////////
FInputManager* FWindowInterface::GetInputManager(void)
{
    auto* windowSubsystem = GetWindowSubsystem();
    if (!windowSubsystem) { return nullptr; }

    return windowSubsystem->GetInputManager();
}

///////////////////////////////////////////////////////////////////////////////
bool FWindowInterface::IsInitialized(void)
{
    std::lock_guard lock(s_mutex);

    try
    {
        auto& engine = FEngineInterface::GetInstance();
        auto* windowSubsystem = engine.GetWindow();
        return windowSubsystem != nullptr;
    }
    catch (const std::exception&)
    {
        return false;
    }
}

///////////////////////////////////////////////////////////////////////////////
FCamera& FWindowInterface::GetCamera(void)
{
    auto* windowSubsystem = GetWindowSubsystem();
    if (!windowSubsystem)
    {
        throw std::runtime_error("Window subsystem is not initialized");
    }

    return windowSubsystem->GetCamera();
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD VR::FVRSystem& FWindowInterface::GetVRSystem(void)
{
    // Return the singleton instance of the VR system
    // From the engine context, this is safe to do
    return VR::FVRSystem::GetInstance();
}

#endif   // TKD_ENGINE_CLIENT

}   // namespace tkd
