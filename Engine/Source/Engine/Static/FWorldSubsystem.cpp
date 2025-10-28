///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Static/FWorldSubsystem.hpp>
#include <Engine/Core/Math.hpp>
#include <Engine/Static/FEngineInterface.hpp>
#include <Engine/Static/FNetworkInterface.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::__internal
///////////////////////////////////////////////////////////////////////////////
namespace tkd::__internal
{

///////////////////////////////////////////////////////////////////////////////
FWorldSubsystem::FWorldSubsystem(float targetTickRate)
    : m_targetTickRate(targetTickRate)
    , m_fixedDeltaTime(0.0f)
{
    if (m_targetTickRate > 0.0f)
    {
        m_fixedDeltaTime = 1.0f / m_targetTickRate;
    }
}

///////////////////////////////////////////////////////////////////////////////
bool FWorldSubsystem::Initialize(void)
{
    if (m_initialized.load(std::memory_order_acquire)) { return false; }

    try
    {
        m_world = std::make_unique<UWorld>();
        m_initialized.store(true, std::memory_order_release);
        return true;
    }
    catch (const std::exception& e)
    {
        return false;
    }
}

///////////////////////////////////////////////////////////////////////////////
UWorld* FWorldSubsystem::GetWorld(void) const noexcept
{
    std::shared_lock lock(m_worldMutex);
    return m_world.get();
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD double FWorldSubsystem::GetSimulationTime(void) const noexcept
{
    return m_simulationTime.load(std::memory_order_acquire);
}

///////////////////////////////////////////////////////////////////////////////
TKD_NODISCARD float FWorldSubsystem::GetAverageTickTime(void) const noexcept
{
    return m_averageTickTime.load(std::memory_order_acquire);
}

///////////////////////////////////////////////////////////////////////////////
void FWorldSubsystem::SetTargetTickRate(float tickRate) noexcept
{
    std::unique_lock lock(m_worldMutex);
    m_targetTickRate = tickRate;
    m_fixedDeltaTime =
        m_targetTickRate > 0.0f ? 1.0f / m_targetTickRate : 0.0f;
}

///////////////////////////////////////////////////////////////////////////////
void FWorldSubsystem::ThreadLoop(void)
{
    {   // BEGIN PLAY
        std::unique_lock lock(m_worldMutex);

        //? BEGIN TEMPORARY
        // TODO: Load default level if specified
        //? END TEMPORARY

        m_world->BeginPlay();
    }

    TimePoint lastTime = SteadyClock::now();
    float accumulator = 0.0f;

    while (m_running.load(std::memory_order_acquire))
    {
        TimePoint currentTime = SteadyClock::now();
        float frameTime = TDuration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        // Clamp frame time to prevent spiral of death
        frameTime = Math<float>::Min(frameTime, 0.25f);
        accumulator += frameTime;

        if (m_fixedDeltaTime > 0.0f)
        {
            // Fixed timestep updates
            while (accumulator >= m_fixedDeltaTime)
            {
                TimePoint tickStart = SteadyClock::now();

                // Process deferred RPCs BEFORE locking the world mutex
                // This allows RPCs to call World::SpawnActor and other
                // functions that need to acquire the world mutex
                Network::ProcessDeferredRPCs(*m_world);

                // Process deferred property replications BEFORE locking the
                // world mutex This follows the same pattern as RPCs for thread
                // safety
                Network::ProcessDeferredPropertyReplications(*m_world);

                {
                    std::unique_lock lock(m_worldMutex);
                    m_world->Tick(m_fixedDeltaTime);
                }

                m_simulationTime.fetch_add(
                    m_fixedDeltaTime, std::memory_order_release
                );
                accumulator -= m_fixedDeltaTime;

                // Track tick performance
                float tickTime =
                    TDuration<float>(SteadyClock::now() - tickStart).count() *
                    1000.0f;
                m_averageTickTime.store(
                    m_averageTickTime.load(std::memory_order_acquire) * 0.95f +
                        tickTime * 0.05f,
                    std::memory_order_release
                );
            }

            // Sleep to maintain target tick rate
            auto targetFrameDuration =
                std::chrono::duration_cast<Milliseconds>(
                    TDuration<float>(m_fixedDeltaTime)
                );
            WaitFor(targetFrameDuration);
        }
        else
        {
            // Variable timestep update
            TimePoint tickStart = SteadyClock::now();

            // Process deferred RPCs BEFORE locking the world mutex
            // This allows RPCs to call World::SpawnActor and other functions
            // that need to acquire the world mutex
            Network::ProcessDeferredRPCs(*m_world);

            // Process deferred property replications BEFORE locking the world
            // mutex This follows the same pattern as RPCs for thread safety
            Network::ProcessDeferredPropertyReplications(*m_world);

            {
                std::unique_lock lock(m_worldMutex);
                m_world->Tick(frameTime);
            }

            m_simulationTime.fetch_add(frameTime, std::memory_order_release);
            accumulator = 0.0f;

            // Track tick performance
            float tickTime =
                TDuration<float>(SteadyClock::now() - tickStart).count() *
                1000.0f;
            m_averageTickTime.store(
                m_averageTickTime.load(std::memory_order_acquire) * 0.95f +
                    tickTime * 0.05f,
                std::memory_order_release
            );

            // Little sleep to prevent 100% CPU usage
            WaitFor(Milliseconds(1));
        }
    }

    {   // END PLAY
        std::unique_lock lock(m_worldMutex);
        m_world->EndPlay();
    }
}

///////////////////////////////////////////////////////////////////////////////
const AGameMode& FWorldSubsystem::GetGameMode(void) const
{
    std::shared_lock lock(m_worldMutex);
    return m_world->GetGameMode();
}

///////////////////////////////////////////////////////////////////////////////
const std::vector<ULevel>& FWorldSubsystem::GetLoadedLevels(void) const
{
    std::shared_lock lock(m_worldMutex);
    return m_world->GetLoadedLevels();
}

///////////////////////////////////////////////////////////////////////////////
ULevel* FWorldSubsystem::GetCurrentLevel(void) const
{
    std::shared_lock lock(m_worldMutex);
    return m_world->GetCurrentLevel();
}

}   // namespace tkd::__internal
