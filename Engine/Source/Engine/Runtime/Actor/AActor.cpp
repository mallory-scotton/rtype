///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Runtime/Actor/AActor.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
AActor::AActor(const FString& name)
    : UObject(name)
    , m_transform(
          *this, "Transform", FTransform::Identity, EPropertyFlags::None
      )
    , m_isActive(*this, "IsActive", true)
    , m_components()
    , m_markedForDeletion(false)
    , m_pendingMoves()
    , m_moveTimestamp(0)
    , m_lastAcknowledgedMove(0)
    , m_serverTransform(FTransform::Identity)
    , m_pendingTransform(FTransform::Identity)
    , m_lastInputVector(FVector3::Zero)
    , m_isMoving(false)
    , m_timeSinceLastInput(0.0f)
    , m_timeSinceLastUpdate(0.0f)
    , m_interpolationStart(FTransform::Identity)
    , m_interpolationTarget(FTransform::Identity)
    , m_interpolationAlpha(1.0f)
    , m_interpolationDuration(INTERPOLATION_TIME)
    , m_extrapolationVelocity(FVector3::Zero)
    , m_positionHistory()
    , m_interpolationDelay(DEFAULT_INTERPOLATION_DELAY)
    , m_localTime(0.0f)
    , m_clientTime(0.0f)
    , m_estimatedRTT(0.1f)
    , m_lastMoveClientTime(0.0f)
    , OnActorBeginOverlap(*this, "OnActorBeginOverlap")
    , OnActorEndOverlap(*this, "OnActorEndOverlap")
    , ServerMoveRPC(
          *this,
          "ServerMove",
          ERPCType::Server,
          std::bind(
              &AActor::RPC_ServerMove,
              this,
              std::placeholders::_1,
              std::placeholders::_2,
              std::placeholders::_3,
              std::placeholders::_4,
              std::placeholders::_5
          ),
          true
      )
    , ClientAckMoveRPC(
          *this,
          "ClientAckMove",
          ERPCType::Client,
          std::bind(
              &AActor::RPC_ClientAckMove,
              this,
              std::placeholders::_1,
              std::placeholders::_2,
              std::placeholders::_3
          ),
          true
      )
    , MulticastMoveRPC(
          *this,
          "MulticastMove",
          ERPCType::Multicast,
          std::bind(
              &AActor::RPC_MulticastMove,
              this,
              std::placeholders::_1,
              std::placeholders::_2,
              std::placeholders::_3
          ),
          true
      )
{}

///////////////////////////////////////////////////////////////////////////////
void AActor::BeginPlay(void)
{
    for (const auto& component: m_components)
    {
        if (component->IsActive()) { component->BeginPlay(); }
    }
}

///////////////////////////////////////////////////////////////////////////////
void AActor::Tick(Float32 deltaTime)
{
    // Update client time
    if (IsLocallyControlled()) { m_clientTime += deltaTime; }

    // Update local time for all actors (used for snapshot timing)
    m_localTime += deltaTime;

    // Server-side continuous movement simulation
    if (IsAuthority() && !IsLocallyControlled())
    {
        // Continue applying the last input vector on the server
        if (m_lastInputVector.Length() > INPUT_THRESHOLD)
        {
            m_timeSinceLastInput += deltaTime;

            // Stop movement after timeout without new input (safety mechanism)
            const Float32 INPUT_TIMEOUT = 0.2f;   // 200ms timeout
            if (m_timeSinceLastInput < INPUT_TIMEOUT)
            {
                FTransform startTransform = m_transform.Get();
                FTransform newTransform = SimulateMovement(
                    m_lastInputVector, deltaTime, startTransform
                );
                m_transform = newTransform;
            }
            else
            {
                // Timeout reached, stop moving
                m_lastInputVector = FVector3::Zero;
            }
        }
    }
    // Use snapshot-based interpolation for simulated proxies
    else if (!IsLocallyControlled() && !IsAuthority())
    {
        InterpolatePosition(deltaTime);
    }

    // Tick all components
    for (const auto& component: m_components)
    {
        if (component->IsActive()) { component->Tick(deltaTime); }
    }
}

///////////////////////////////////////////////////////////////////////////////
void AActor::EndPlay(void)
{
    for (const auto& component: m_components)
    {
        if (component->IsActive()) { component->EndPlay(); }
    }
}

///////////////////////////////////////////////////////////////////////////////
FTransform AActor::GetTransform(void) const { return m_transform.Get(); }

///////////////////////////////////////////////////////////////////////////////
void AActor::SetTransform(const FTransform& transform)
{
    m_transform = transform;
}

///////////////////////////////////////////////////////////////////////////////
Bool AActor::IsActive(void) const { return m_isActive.Get(); }

///////////////////////////////////////////////////////////////////////////////
void AActor::SetActive(Bool isActive) { m_isActive = isActive; }

///////////////////////////////////////////////////////////////////////////////
void AActor::RemoveComponent(const FString& name)
{
    m_components.Erase(
        std::remove_if(
            m_components.Begin(),
            m_components.End(),
            [&name](const Component& component)
            { return component->GetName() == name; }
        ),
        m_components.End()
    );
}

///////////////////////////////////////////////////////////////////////////////
void AActor::RemoveComponent(UActorComponent* component)
{
    m_components.Erase(
        std::remove_if(
            m_components.Begin(),
            m_components.End(),
            [component](const Component& comp)
            { return comp.get() == component; }
        ),
        m_components.End()
    );
}

///////////////////////////////////////////////////////////////////////////////
void AActor::MarkForDeletion(void) { m_markedForDeletion = true; }

///////////////////////////////////////////////////////////////////////////////
bool AActor::IsMarkedForDeletion(void) const { return m_markedForDeletion; }

///////////////////////////////////////////////////////////////////////////////
bool AActor::IsTransformReplicated(void) const
{
    return m_transform.HasFlag(EPropertyFlags::Replicated);
}

///////////////////////////////////////////////////////////////////////////////
void AActor::SetTransformReplicated(Bool replicated)
{
    if (replicated)
    {
        if (!m_hasSetUpdateFrequency) { m_netUpdateFrequency = 20.f; }
    }
    else
    {
        if (!m_hasSetUpdateFrequency) { m_netUpdateFrequency = 10.f; }
    }

    m_isTransformReplicated = replicated;
}

///////////////////////////////////////////////////////////////////////////////
void AActor::Translate(const FVector3& translation)
{
    if (m_isTransformReplicated) { m_pendingTransform.Translate(translation); }
    else { m_transform->Translate(translation); }
}

///////////////////////////////////////////////////////////////////////////////
void AActor::Translate(Float32 x, Float32 y, Float32 z)
{
    if (m_isTransformReplicated)
    {
        m_pendingTransform.Translate(FVector3(x, y, z));
    }
    else { m_transform->Translate(FVector3(x, y, z)); }
}

///////////////////////////////////////////////////////////////////////////////
void AActor::Rotate(const FVector3& rotation)
{
    FRotator rotator = FRotator(rotation.x, rotation.y, rotation.z);
    if (m_isTransformReplicated) { m_pendingTransform.Rotate(rotator); }
    else { m_transform->Rotate(rotator); }
}

///////////////////////////////////////////////////////////////////////////////
void AActor::Rotate(const FRotator& rotation)
{
    if (m_isTransformReplicated) { m_pendingTransform.Rotate(rotation); }
    else { m_transform->Rotate(rotation); }
}

///////////////////////////////////////////////////////////////////////////////
void AActor::Rotate(Float32 pitch, Float32 yaw, Float32 roll)
{
    FRotator rotator = FRotator(pitch, yaw, roll);
    if (m_isTransformReplicated) { m_pendingTransform.Rotate(rotator); }
    else { m_transform->Rotate(rotator); }
}

///////////////////////////////////////////////////////////////////////////////
void AActor::Scale(const FVector3& scale)
{
    if (m_isTransformReplicated) { m_pendingTransform.Scale(scale); }
    else { m_transform->Scale(scale); }
}

///////////////////////////////////////////////////////////////////////////////
void AActor::Scale(Float32 x, Float32 y, Float32 z)
{
    if (m_isTransformReplicated)
    {
        m_pendingTransform.Scale(FVector3(x, y, z));
    }
    else { m_transform->Scale(FVector3(x, y, z)); }
}

///////////////////////////////////////////////////////////////////////////////
void AActor::ApplyMovement(const FVector3& inputVector, Float32 deltaTime)
{
    if (!m_isTransformReplicated)
    {
        // No replication, just apply movement directly
        FTransform newTransform =
            SimulateMovement(inputVector, deltaTime, m_transform.Get());
        m_transform = newTransform;
        return;
    }

    if (IsLocallyControlled())
    {
        // Always simulate movement locally for instant response
        FTransform startTransform = m_transform.Get();
        FTransform newTransform =
            SimulateMovement(inputVector, deltaTime, startTransform);
        m_transform = newTransform;

        // Check if input has changed significantly
        FVector3 inputDelta = inputVector - m_lastInputVector;
        Float32 inputChange = inputDelta.Length();
        bool inputChanged = inputChange > INPUT_THRESHOLD;

        // Determine if we're moving
        bool currentlyMoving = inputVector.Length() > INPUT_THRESHOLD;
        bool movementStateChanged = currentlyMoving != m_isMoving;

        // We send an RPC when:
        // 1. Movement state changes (started or stopped moving) - IMMEDIATE
        // 2. Input direction/magnitude changed significantly - IMMEDIATE
        // 3. Keep-alive while holding same input (to prevent server timeout) -
        // PERIODIC
        bool shouldSendRPC = false;

        if (movementStateChanged)
        {
            // State change: started or stopped moving
            shouldSendRPC = true;
            m_isMoving = currentlyMoving;
            m_timeSinceLastUpdate = 0.0f;   // Reset timer
        }
        else if (inputChanged)
        {
            // Input changed (direction or magnitude)
            shouldSendRPC = true;
            m_timeSinceLastUpdate = 0.0f;   // Reset timer
        }
        else if (currentlyMoving)
        {
            // Keep-alive update: we're moving with same input, send periodic
            // updates
            m_timeSinceLastUpdate += deltaTime;
            if (m_timeSinceLastUpdate >= MOVEMENT_UPDATE_RATE)
            {
                shouldSendRPC = true;
                m_timeSinceLastUpdate = 0.0f;
            }
        }

        if (shouldSendRPC)
        {
            UInt32 timestamp = ++m_moveTimestamp;
            Float32 clientTime = m_clientTime;

            // Store the move for later reconciliation
            m_pendingMoves.push_back(FMoveData(
                timestamp,
                deltaTime,
                clientTime,
                inputVector,
                startTransform,
                newTransform
            ));

            // Limit the pending moves buffer
            if (m_pendingMoves.size() > MAX_PENDING_MOVES)
            {
                m_pendingMoves.erase(
                    m_pendingMoves.begin(),
                    m_pendingMoves.begin() +
                        (m_pendingMoves.size() - MAX_PENDING_MOVES)
                );
            }

            // Send move to server with client timestamp
            ServerMoveRPC(
                timestamp, deltaTime, clientTime, inputVector, newTransform
            );

            // Update last input and move time
            m_lastInputVector = inputVector;
            m_lastMoveClientTime = clientTime;
        }
    }
    else if (IsAuthority())
    {
        // Server authoritative - movement comes from RPC
        // Don't apply movement here, wait for client input
    }
    // Simulated proxies just interpolate based on replicated transform
}

///////////////////////////////////////////////////////////////////////////////
void AActor::RPC_ServerMove(
    UInt32 timestamp,
    Float32 deltaTime,
    Float32 clientTime,
    const FVector3& inputVector,
    const FTransform& clientTransform
)
{
    if (!IsAuthority()) { return; }

    // Validate input to prevent crashes
    if (deltaTime <= 0.0f || deltaTime > 1.0f) { return; }   // Sanity check
    if (inputVector.Length() > 10000.0f)
    {
        return;
    }   // Prevent extreme values

    // Store the input vector for continuous simulation
    m_lastInputVector = inputVector;
    m_timeSinceLastInput = 0.0f;   // Reset timeout

    // Get current server transform
    FTransform startTransform = m_transform.Get();

    // Adjust deltaTime based on network latency
    // The client sent this move at clientTime, but we're receiving it now
    // We need to simulate the movement that happened during the network delay
    // For simplicity, we'll use the original deltaTime plus a small buffer
    // This compensates for the one-way latency (Client -> Server)
    Float32 adjustedDeltaTime = deltaTime;

    // Optional: If we track when the packet was sent vs received,
    // we could add that network delay here: adjustedDeltaTime += networkDelay

    // Simulate the same movement on server
    FTransform serverTransform =
        SimulateMovement(inputVector, adjustedDeltaTime, startTransform);

    // Apply the server's authoritative transform
    m_transform = serverTransform;

    // Get current server time for the response
    Float32 serverTime = m_clientTime;   // Or use a global time source

    // Send acknowledgment back to owning client with corrected position
    ClientAckMoveRPC(timestamp, serverTime, serverTransform);

    // Broadcast movement to all other clients (simulated proxies)
    MulticastMoveRPC(serverTransform, inputVector, serverTime);

    // Calculate difference for debugging
    Float32 positionError =
        (serverTransform.GetPosition() - clientTransform.GetPosition())
            .Length();

    // Optional: Log significant deviations
    if (positionError > 1.0f)
    {
        // Client prediction diverged significantly from server
        // The correction will be applied automatically
    }
}

///////////////////////////////////////////////////////////////////////////////
void AActor::RPC_ClientAckMove(
    UInt32 timestamp, Float32 serverTime, const FTransform& serverTransform
)
{
    if (!IsLocallyControlled()) { return; }

    // Validate timestamp
    if (timestamp < m_lastAcknowledgedMove) { return; }   // Old acknowledgment

    // Calculate RTT (Round Trip Time)
    // Find the move that was just acknowledged
    for (const auto& move: m_pendingMoves)
    {
        if (move.timestamp == timestamp)
        {
            // RTT = current client time - time when we sent the move
            Float32 measuredRTT = m_clientTime - move.clientTime;

            // Smooth RTT estimation using exponential moving average
            const Float32 RTT_SMOOTHING = 0.9f;   // Weight for old RTT
            m_estimatedRTT = (RTT_SMOOTHING * m_estimatedRTT) +
                             ((1.0f - RTT_SMOOTHING) * measuredRTT);

            // Clamp RTT to reasonable values
            m_estimatedRTT = std::max(0.01f, std::min(m_estimatedRTT, 1.0f));

            break;
        }
    }

    // Update last acknowledged move
    m_lastAcknowledgedMove = timestamp;
    m_serverTransform = serverTransform;

    // Remove acknowledged moves from pending list
    m_pendingMoves.erase(
        std::remove_if(
            m_pendingMoves.begin(),
            m_pendingMoves.end(),
            [timestamp](const FMoveData& move)
            { return move.timestamp <= timestamp; }
        ),
        m_pendingMoves.end()
    );

    // Reconcile: Check if we need to correct our position
    ReconcileMovement(timestamp, serverTransform);
}

///////////////////////////////////////////////////////////////////////////////
FTransform AActor::SimulateMovement(
    const FVector3& inputVector,
    Float32 deltaTime,
    const FTransform& startTransform
)
{
    // This is a basic movement simulation
    // Override this in derived classes for custom movement logic
    FTransform result = startTransform;

    if (inputVector.Length() > 0.0f)
    {
        // Normalize input to prevent faster diagonal movement
        FVector3 normalizedInput = inputVector.Normalized();

        // Apply movement (you can add speed properties in derived classes)
        FVector3 movement = normalizedInput * deltaTime;
        result.Translate(movement);
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
void AActor::ReconcileMovement(
    UInt32 serverTimestamp, const FTransform& serverTransform
)
{
    // Safety check: don't reconcile if we have too many pending moves
    if (m_pendingMoves.size() > MAX_PENDING_MOVES)
    {
        // Something is wrong, clear pending moves and snap to server position
        m_pendingMoves.clear();
        m_transform = serverTransform;
        return;
    }

    // Calculate the error between client and server
    FVector3 clientPosition = m_transform->GetPosition();
    FVector3 serverPosition = serverTransform.GetPosition();
    Float32 positionError = (clientPosition - serverPosition).Length();

    // Define error threshold (tune this based on your game)
    const Float32 ERROR_THRESHOLD = 0.5f;

    if (positionError > ERROR_THRESHOLD)
    {
        // Significant error detected, need to reconcile

        // Rewind to server state
        m_transform = serverTransform;

        // Replay all pending moves that happened after the acknowledged move
        for (const auto& move: m_pendingMoves)
        {
            if (move.timestamp > serverTimestamp)
            {
                FTransform newTransform = SimulateMovement(
                    move.inputVector, move.deltaTime, m_transform.Get()
                );
                m_transform = newTransform;
            }
        }
    }
    // If error is small enough, trust client prediction (no correction needed)
}

///////////////////////////////////////////////////////////////////////////////
void AActor::RPC_MulticastMove(
    const FTransform& newTransform,
    const FVector3& velocity,
    Float32 serverTime
)
{
    // Skip if we're the owning client or the server
    if (IsLocallyControlled() || IsAuthority()) { return; }

    // Create a new snapshot and add it to the buffer
    FPositionSnapshot snapshot(
        serverTime,
        newTransform,
        velocity,
        m_localTime   // Current local time when we received this
    );

    // Add to history buffer
    m_positionHistory.push_back(snapshot);

    // Maintain buffer size - remove oldest snapshots if we exceed the limit
    while (m_positionHistory.size() > SNAPSHOT_BUFFER_SIZE)
    {
        m_positionHistory.pop_front();
    }

    // Clean up snapshots that are too old (older than 500ms)
    const Float32 MAX_SNAPSHOT_AGE = 0.5f;
    while (!m_positionHistory.empty())
    {
        Float32 age = m_localTime - m_positionHistory.front().receivedTime;
        if (age > MAX_SNAPSHOT_AGE) { m_positionHistory.pop_front(); }
        else
        {
            break;   // Snapshots are ordered, so we can stop here
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void AActor::InterpolatePosition(Float32 deltaTime)
{
    // Need at least 2 snapshots to interpolate
    if (m_positionHistory.size() < 2)
    {
        // Not enough data yet, try extrapolation if we have at least one
        if (!m_positionHistory.empty())
        {
            const FPositionSnapshot& latest = m_positionHistory.back();
            ExtrapolateOrClamp(latest.serverTime);
        }
        return;
    }

    // Calculate render time = current server time - interpolation delay
    Float32 currentServerTime = EstimateCurrentServerTime();
    Float32 renderTime = currentServerTime - m_interpolationDelay;

    // Find two snapshots that bracket the render time
    FPositionSnapshot* from = nullptr;
    FPositionSnapshot* to = nullptr;

    for (size_t i = 0; i < m_positionHistory.size() - 1; i++)
    {
        if (m_positionHistory[i].serverTime <= renderTime &&
            m_positionHistory[i + 1].serverTime >= renderTime)
        {
            from = &m_positionHistory[i];
            to = &m_positionHistory[i + 1];
            break;
        }
    }

    if (from && to)
    {
        // Calculate interpolation factor
        Float32 timeDiff = to->serverTime - from->serverTime;

        // Avoid division by zero
        if (timeDiff < 0.001f)
        {
            m_transform = from->transform;
            return;
        }

        Float32 t = (renderTime - from->serverTime) / timeDiff;
        t = std::max(0.0f, std::min(t, 1.0f));   // Clamp to [0,1]

        // Lerp position
        FVector3 position = FVector3::Lerp(
            from->transform.GetPosition(), to->transform.GetPosition(), t
        );

        // Lerp rotation (simple linear interpolation for now)
        // TODO: Consider using Slerp for smoother rotation
        FRotator rotation = FRotator::Lerp(
            from->transform.GetRotation(), to->transform.GetRotation(), t
        );

        // Apply interpolated transform
        FTransform interpolated = m_transform.Get();
        interpolated.SetPosition(position);
        interpolated.SetRotation(rotation);
        m_transform = interpolated;
    }
    else
    {
        // Handle edge cases: extrapolate or clamp to newest
        ExtrapolateOrClamp(renderTime);
    }
}

///////////////////////////////////////////////////////////////////////////////
Float32 AActor::EstimateCurrentServerTime(void) const
{
    if (m_positionHistory.empty()) { return 0.0f; }

    // Use the last known server time + time elapsed since we received it
    const FPositionSnapshot& latest = m_positionHistory.back();
    Float32 timeSinceReceived = m_localTime - latest.receivedTime;

    return latest.serverTime + timeSinceReceived;
}

///////////////////////////////////////////////////////////////////////////////
void AActor::ExtrapolateOrClamp(Float32 renderTime)
{
    if (m_positionHistory.empty()) { return; }

    const FPositionSnapshot& latest = m_positionHistory.back();

    // Check how far beyond our data we are
    Float32 extrapolationTime = renderTime - latest.serverTime;

    if (extrapolationTime > 0.0f && extrapolationTime < EXTRAPOLATION_LIMIT)
    {
        // Extrapolate using last known velocity
        if (latest.velocity.Length() > INPUT_THRESHOLD)
        {
            FVector3 extrapolatedPosition =
                latest.transform.GetPosition() +
                latest.velocity * extrapolationTime;

            FTransform newTransform = latest.transform;
            newTransform.SetPosition(extrapolatedPosition);
            m_transform = newTransform;
        }
        else
        {
            // Not moving, just use latest position
            m_transform = latest.transform;
        }
    }
    else
    {
        // Too far out or in the past, just clamp to latest position
        m_transform = latest.transform;
    }
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(AActor, UObject);

}   // namespace tkd
