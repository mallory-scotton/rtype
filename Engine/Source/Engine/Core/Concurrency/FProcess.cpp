///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Core/Concurrency/FProcess.hpp>
#include <cstring>
#include <iostream>
#include <signal.h>
#include <stdexcept>
#include <sys/wait.h>
#include <unistd.h>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
FProcess::FProcess(std::function<void()> function)
    : m_function(function)
    , m_pid(-1)
    , m_status(EProcessStatus::Stopped)
    , m_returnValue(0)
{}

///////////////////////////////////////////////////////////////////////////////
FProcess::~FProcess()
{
    if (m_pid > 0)
    {
        if (this->IsRunning())
        {
            if (kill(m_pid, SIGTERM) == 0)
            {
                int status;
                pid_t wait_ret = waitpid(m_pid, &status, 0);

                if (wait_ret == m_pid) { UpdateStatusFromWait(status); }
                else
                {
                    kill(m_pid, SIGKILL);
                    wait_ret = waitpid(m_pid, &status, 0);
                    if (wait_ret == m_pid) { UpdateStatusFromWait(status); }
                    else { m_status = EProcessStatus::Error; }
                }
            }
            else
            {
                if (errno == ESRCH)
                {
                    int status;
                    if (waitpid(m_pid, &status, WNOHANG) == m_pid)
                    {
                        UpdateStatusFromWait(status);
                    }
                    else { m_status = EProcessStatus::Finished; }
                }
                else { m_status = EProcessStatus::Error; }
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void FProcess::Start(void)
{
    if (m_status == EProcessStatus::Running)
    {
        throw std::runtime_error("Process already started.");
    }

    m_pid = fork();

    if (m_pid < 0)
    {
        m_status = EProcessStatus::Error;
        throw std::runtime_error(
            "Fork failed: " + std::string(strerror(errno))
        );
    }
    else if (m_pid == 0)
    {
        try
        {
            if (m_function) { m_function(); }
            _exit(0);
        }
        catch (const std::exception& e)
        {
            _exit(1);
        }
        catch (...)
        {
            _exit(2);
        }
    }
    else
    {
        m_status = EProcessStatus::Running;
        m_returnValue = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////
void FProcess::UpdateStatusFromWait(int status)
{
    if (WIFEXITED(status))
    {
        m_returnValue = WEXITSTATUS(status);
        m_status = EProcessStatus::Finished;
    }
    else if (WIFSIGNALED(status))
    {
        m_returnValue = WTERMSIG(status);
        m_status = EProcessStatus::Error;
    }
    else { m_status = EProcessStatus::Error; }
}

///////////////////////////////////////////////////////////////////////////////
void FProcess::Wait(void)
{
    if (m_pid == -1)
    {
        throw std::runtime_error(
            "Process not started or already waited upon without restart."
        );
    }

    if (m_status != EProcessStatus::Running) { return; }

    int status;
    pid_t result = waitpid(m_pid, &status, 0);

    if (result == -1)
    {
        m_status = EProcessStatus::Error;
        throw std::runtime_error(
            "Waitpid failed: " + std::string(strerror(errno))
        );
    }
    else if (result == m_pid) { UpdateStatusFromWait(status); }
}

///////////////////////////////////////////////////////////////////////////////
bool FProcess::IsRunning(void)
{
    if (m_pid == -1 || m_status == EProcessStatus::Finished ||
        m_status == EProcessStatus::Error)
    {
        return false;
    }
    if (m_status == EProcessStatus::Stopped && m_pid == -1) { return false; }

    int status;
    pid_t result = waitpid(m_pid, &status, WNOHANG);

    if (result == 0)
    {
        m_status = EProcessStatus::Running;
        return true;
    }
    else if (result == m_pid)
    {
        UpdateStatusFromWait(status);
        return false;
    }
    else if (result == -1) { m_status = EProcessStatus::Error; }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
int FProcess::GetReturnValue(void) { return m_returnValue; }

///////////////////////////////////////////////////////////////////////////////
EProcessStatus FProcess::GetStatus(void) const { return m_status; }

///////////////////////////////////////////////////////////////////////////////
pid_t FProcess::GetPid(void) const { return m_pid; }

///////////////////////////////////////////////////////////////////////////////
bool FProcess::IsParent(void) const { return m_pid > 0; }

///////////////////////////////////////////////////////////////////////////////
bool FProcess::IsChild(void) const { return m_pid == 0; }

}   // namespace tkd
