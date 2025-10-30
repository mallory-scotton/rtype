///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Static/FThreadedSubsystem.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::__internal
///////////////////////////////////////////////////////////////////////////////
namespace tkd::__internal
{

///////////////////////////////////////////////////////////////////////////////
FThreadedSubsystem::~FThreadedSubsystem(void)
{
    if (m_running.load(std::memory_order_acquire))
    {
        RequestShutdown();
        Shutdown();
    }
}

///////////////////////////////////////////////////////////////////////////////
bool FThreadedSubsystem::Initialize(void)
{
    m_initialized.store(true, std::memory_order_release);
    return true;
}

///////////////////////////////////////////////////////////////////////////////
void FThreadedSubsystem::Start(void)
{
    if (m_running.load(std::memory_order_acquire) ||
        !m_initialized.load(std::memory_order_acquire))
    {
        return;
    }

    m_running.store(true, std::memory_order_release);
#ifndef TKD_SYSTEM_WINDOWS
    m_thread = std::thread([this] {
        ThreadSetup();
        while (m_running.load(std::memory_order_acquire))
        {
            ThreadLoop();
        }
        ThreadTeardown();
    });
#endif
}

///////////////////////////////////////////////////////////////////////////////
void FThreadedSubsystem::RequestShutdown(void)
{
    m_running.store(false, std::memory_order_release);
    m_cv.notify_all();
}

///////////////////////////////////////////////////////////////////////////////
void FThreadedSubsystem::Shutdown(void)
{
    RequestShutdown();

#ifndef TKD_SYSTEM_WINDOWS
    if (m_thread.joinable()) { m_thread.join(); }
#endif

    m_initialized.store(false, std::memory_order_release);
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD bool FThreadedSubsystem::IsRunning(void) const noexcept
{
    return m_running.load(std::memory_order_acquire);
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD bool FThreadedSubsystem::IsInitialized(void) const noexcept
{
    return m_initialized.load(std::memory_order_acquire);
}

}   // namespace tkd::__internal
