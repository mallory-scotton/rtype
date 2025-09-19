///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Core/Concurrency/FThread.hpp>
#include <stdexcept>
#include <utility>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
FThread::FThread(std::function<void()> function)
    : m_function(std::move(function))
    , m_started(false)
    , running(false)
{}

///////////////////////////////////////////////////////////////////////////////
FThread::~FThread()
{
    if (m_thread.joinable())
    {
        try
        {
            m_thread.join();
        }
        catch (...)
        {}
    }
}

///////////////////////////////////////////////////////////////////////////////
void FThread::Start(void)
{
    if (m_started) { throw std::runtime_error("Thread already started."); }
    if (!m_function)
    {
        throw std::runtime_error("Cannot start thread with no function.");
    }
    try
    {
        running = true;
        m_thread = std::thread(m_function);
        m_started = true;
    }
    catch (const std::exception& e)
    {
        m_started = false;
        throw std::runtime_error(
            "Failed to start thread: " + std::string(e.what())
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
void FThread::Join(void)
{
    if (!m_started) { return; }

    if (m_thread.joinable())
    {
        try
        {
            m_thread.join();
        }
        catch (const std::exception& e)
        {
            throw std::runtime_error(
                "Failed to join thread: " + std::string(e.what())
            );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void FThread::Detach(void)
{
    if (!m_started) { return; }

    if (m_thread.joinable())
    {
        try
        {
            m_thread.detach();
        }
        catch (const std::exception& e)
        {
            throw std::runtime_error(
                "Failed to detach thread: " + std::string(e.what())
            );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
bool FThread::Joinable(void) const
{
    if (!m_started) { return false; }
    return m_thread.joinable();
}

}   // namespace tkd
