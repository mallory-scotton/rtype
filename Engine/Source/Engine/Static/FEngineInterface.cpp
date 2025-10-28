///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Static/FEngineInterface.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
TUniquePtr<__internal::Engine> FEngineInterface::s_instance = nullptr;
std::mutex FEngineInterface::s_mutex;

///////////////////////////////////////////////////////////////////////////////
bool FEngineInterface::Initialize(int argc, char* argv[])
{
    std::lock_guard lock(s_mutex);

    if (s_instance) { return false; }

    s_instance = std::make_unique<__internal::Engine>();
    return s_instance->Initialize(argc, argv);
}

///////////////////////////////////////////////////////////////////////////////
void FEngineInterface::Run(void)
{
    __internal::Engine* instance = nullptr;
    {
        std::lock_guard lock(s_mutex);
        instance = s_instance.get();
    }

    if (instance) { instance->Run(); }
}

///////////////////////////////////////////////////////////////////////////////
bool FEngineInterface::Shutdown(void)
{
    std::lock_guard lock(s_mutex);

    if (!s_instance) { return false; }

    s_instance->Shutdown();
    s_instance.reset();
    return true;
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD __internal::Engine& FEngineInterface::GetInstance(void)
{
    std::lock_guard lock(s_mutex);

    if (!s_instance)
    {
        throw std::runtime_error("Engine instance is not initialized.");
    }

    return *s_instance;
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD bool FEngineInterface::IsRunning(void)
{
    std::lock_guard lock(s_mutex);
    return s_instance && s_instance->IsRunning();
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD bool FEngineInterface::IsInitialized(void)
{
    std::lock_guard lock(s_mutex);
    return s_instance && s_instance->IsInitialized();
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD bool FEngineInterface::IsValid(void)
{
    std::lock_guard lock(s_mutex);
    return s_instance != nullptr;
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD int FEngineInterface::GetExitCode(void)
{
    std::lock_guard lock(s_mutex);
    return s_instance ? s_instance->GetExitCode() : 0;
}

///////////////////////////////////////////////////////////////////////////////
void FEngineInterface::PrintExitMessage(void)
{
    std::lock_guard lock(s_mutex);
    if (s_instance) { s_instance->PrintExitMessage(); }
}

///////////////////////////////////////////////////////////////////////////////
void FEngineInterface::RequestShutdown(void)
{
    std::lock_guard lock(s_mutex);
    if (s_instance) { s_instance->RequestShutdown(); }
}

}   // namespace tkd
