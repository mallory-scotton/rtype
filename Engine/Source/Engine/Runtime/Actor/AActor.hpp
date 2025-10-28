///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Config.hpp>
#include <Engine/Core/Containers.hpp>
#include <Engine/Core/Math.hpp>
#include <Engine/Core/Object.hpp>
#include <Engine/Network/Enumerations.hpp>
#include <Engine/Runtime/Components/UActorComponent.hpp>
#include <Engine/Runtime/Time/ITickable.hpp>
#include <memory>
#include <type_traits>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief A base class for all actors in the world
///
///////////////////////////////////////////////////////////////////////////////
class AActor
    : public UObject
    , public ITickable
{
public:
    ///////////////////////////////////////////////////////////////////////////
    // Class Aliases
    ///////////////////////////////////////////////////////////////////////////
    using Component = std::shared_ptr<UActorComponent>;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Struct to hold move data for replication
    ///
    ///////////////////////////////////////////////////////////////////////////
    struct FMoveData
    {
        UInt32 timestamp;            //<! Time of the move
        Float32 deltaTime;           //<! Time since the last move
        Float32 clientTime;          //<! Client time when move was generated
        FVector3 inputVector;        //<! Input vector for this move
        FTransform startTransform;   //<! Transform before the move
        FTransform endTransform;     //<! Transform after the move

        ///////////////////////////////////////////////////////////////////////
        /// \brief Default constructor
        ///
        ///////////////////////////////////////////////////////////////////////
        FMoveData(void)
            : timestamp(0)
            , deltaTime(0.0f)
            , clientTime(0.0f)
            , inputVector(FVector3::Zero)
            , startTransform(FTransform::Identity)
            , endTransform(FTransform::Identity)
        {}

        ///////////////////////////////////////////////////////////////////////
        /// \brief Parameterized constructor
        ///
        /// \param ts The timestamp of the move
        /// \param dt The delta time since the last move
        /// \param ct The client time when move was generated
        /// \param input The input vector for this move
        /// \param start The starting transform before the move
        /// \param end The ending transform after the move
        ///
        ///////////////////////////////////////////////////////////////////////
        FMoveData(
            UInt32 ts,
            Float32 dt,
            Float32 ct,
            const FVector3& input,
            const FTransform& start,
            const FTransform& end
        )
            : timestamp(ts)
            , deltaTime(dt)
            , clientTime(ct)
            , inputVector(input)
            , startTransform(start)
            , endTransform(end)
        {}
    };

private:
    ///////////////////////////////////////////////////////////////////////////
    // Class Member
    ///////////////////////////////////////////////////////////////////////////
    UProperty<FTransform> m_transform;   //<! The actor's transform
    UProperty<Bool> m_isActive;          //<! Whether the actor is active
    TVector<Component> m_components;     //<! The actor's components
    bool m_markedForDeletion;            //<! Marked for deletion

    // Client-side prediction
    std::vector<FMoveData> m_pendingMoves;   //<! Unacknowledged moves (client)
    UInt32 m_moveTimestamp;                  //<! Current move timestamp

    // Server reconciliation
    UInt32 m_lastAcknowledgedMove;   //<! Last move processed by server
    FTransform m_serverTransform;    //<! Last known server transform

    // Movement state
    FTransform m_pendingTransform;   //<! Pending transform for replication
    FVector3 m_lastInputVector;      //<! Last input vector (client/server)
    bool m_isMoving;                 //<! Whether we're currently moving
    Float32 m_timeSinceLastInput;    //<! Time since last input (server)
    Float32
        m_timeSinceLastUpdate;   //<! Time since last network update (client)

    // Interpolation for simulated proxies
    FTransform
        m_interpolationStart;   //<! Starting transform for interpolation
    FTransform m_interpolationTarget;   //<! Target transform for interpolation
    Float32 m_interpolationAlpha;       //<! Current interpolation alpha [0-1]
    Float32 m_interpolationDuration;    //<! Duration of interpolation
    FVector3 m_extrapolationVelocity;   //<! Velocity for extrapolation

    // Network timing
    Float32 m_clientTime;           //<! Client-side time accumulator
    Float32 m_estimatedRTT;         //<! Estimated round-trip time
    Float32 m_lastMoveClientTime;   //<! Client time of last move sent

    // Network configuration
    static constexpr UInt32 MAX_PENDING_MOVES = 32;   //<! Max buffered moves
    static constexpr Float32 INPUT_THRESHOLD =
        0.01f;   //<! Threshold for input change
    static constexpr Float32 MOVEMENT_UPDATE_RATE =
        0.1f;    //<! Keep-alive update rate (100ms)
    static constexpr Float32 INTERPOLATION_TIME =
        0.1f;    //<! Interpolation duration
    static constexpr Float32 EXTRAPOLATION_LIMIT =
        0.25f;   //<! Max time to extrapolate (250ms)

public:
    ///////////////////////////////////////////////////////////////////////////
    // Class Member
    ///////////////////////////////////////////////////////////////////////////
    UFunction<AActor*> OnActorBeginOverlap;
    UFunction<AActor*> OnActorEndOverlap;

    // Movement RPCs
    UFunction<UInt32, Float32, Float32, FVector3, FTransform> ServerMoveRPC;
    UFunction<UInt32, Float32, FTransform> ClientAckMoveRPC;
    UFunction<FTransform, FVector3, Float32>
        MulticastMoveRPC;   //<! Replicate movement to all clients

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor
    ///
    ///////////////////////////////////////////////////////////////////////////
    AActor(const FString& name = "AActor");

protected:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Simulate movement locally (used by both client and server)
    ///
    /// \param inputVector The input vector for this move
    /// \param deltaTime The time elapsed for this move
    ///
    /// \return The new transform after applying movement
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual FTransform SimulateMovement(
        const FVector3& inputVector,
        Float32 deltaTime,
        const FTransform& startTransform
    );

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Server RPC to process movement
    ///
    /// \param timestamp The timestamp of the move
    /// \param deltaTime The time elapsed for this move
    /// \param clientTime The client time when move was generated
    /// \param inputVector The input vector for this move
    /// \param clientTransform The client's predicted transform
    ///
    ///////////////////////////////////////////////////////////////////////////
    void RPC_ServerMove(
        UInt32 timestamp,
        Float32 deltaTime,
        Float32 clientTime,
        const FVector3& inputVector,
        const FTransform& clientTransform
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Client RPC to acknowledge movement and correct position
    ///
    /// \param timestamp The timestamp of the acknowledged move
    /// \param serverTime The server time when processing this move
    /// \param serverTransform The authoritative server transform
    ///
    ///////////////////////////////////////////////////////////////////////////
    void RPC_ClientAckMove(
        UInt32 timestamp, Float32 serverTime, const FTransform& serverTransform
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Reconcile client prediction with server state
    ///
    /// \param serverTimestamp The timestamp acknowledged by server
    /// \param serverTransform The authoritative server transform
    ///
    ///////////////////////////////////////////////////////////////////////////
    void ReconcileMovement(
        UInt32 serverTimestamp, const FTransform& serverTransform
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Multicast RPC to replicate movement to all clients
    ///
    /// \param newTransform The new transform to replicate
    /// \param velocity The current velocity/input vector
    /// \param serverTime The server time when this update was sent
    ///
    ///////////////////////////////////////////////////////////////////////////
    void RPC_MulticastMove(
        const FTransform& newTransform,
        const FVector3& velocity,
        Float32 serverTime
    );

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Called when the object is first created
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void BeginPlay(void) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Ticks the object
    ///
    /// \param deltaTime The time elapsed since the last tick
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void Tick(Float32 deltaTime) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Called when the object is being destroyed
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void EndPlay(void) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the actor's transform
    ///
    /// \return The actor's transform
    ///
    ///////////////////////////////////////////////////////////////////////////
    FTransform GetTransform(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the actor's transform
    ///
    /// \param transform The new transform
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetTransform(const FTransform& transform);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Check if the actor is active
    ///
    /// \return True if the actor is active, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    Bool IsActive(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set whether the actor is active
    ///
    /// \param isActive True to set the actor as active, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetActive(Bool isActive);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Add a component to the actor
    ///
    /// \tparam T The type of the component to add
    /// \tparam Args The types of the arguments to forward to the component's
    /// constructor
    ///
    /// \param args The arguments to forward to the component's constructor
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T, typename... Args>
    T* AddComponent(Args&&... args)
    {
        static_assert(
            std::is_base_of<UActorComponent, T>::value,
            "T must be derived from UActorComponent"
        );

        auto newComponent = m_components.EmplaceBack(
            std::make_shared<T>(std::forward<Args>(args)...)
        );

        newComponent->SetOwner(this);

        return reinterpret_cast<T*>(newComponent.get());
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Add a component to the actor
    ///
    /// \param component The component to add
    ///
    ///////////////////////////////////////////////////////////////////////////
    UActorComponent* AddComponent(Component component)
    {
        auto newComponent = m_components.EmplaceBack(std::move(component));
        newComponent->SetOwner(this);
        return newComponent.get();
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Add a component to the actor
    ///
    /// \param component Pointer to the component to add
    ///
    ///////////////////////////////////////////////////////////////////////////
    UActorComponent* AddComponent(UActorComponent* component)
    {
        auto newComponent = m_components.EmplaceBack(component);
        newComponent->SetOwner(this);
        return newComponent.get();
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the actor's components
    ///
    /// \return A constant reference to the vector of component pointers
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T = UActorComponent>
    TVector<std::shared_ptr<T>> GetComponents(void) const
    {
        static_assert(
            std::is_base_of<UActorComponent, T>::value,
            "T must be derived from UActorComponent"
        );

        if constexpr (std::is_same<T, UActorComponent>::value)
        {
            return m_components;
        }

        TVector<std::shared_ptr<T>> filteredComponents;
        for (const auto& component: m_components)
        {
            if (auto casted = dynamic_cast<T*>(component.get()))
            {
                filteredComponents.EmplaceBack(std::make_shared<T>(*casted));
            }
        }
        return filteredComponents;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get a component of the actor by name
    ///
    /// \param name The name of the component to get
    ///
    /// \return A pointer to the component if found, nullptr otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T = UActorComponent>
    T* GetComponent(const FString& name) const
    {
        static_assert(
            std::is_base_of<UActorComponent, T>::value,
            "T must be derived from UActorComponent"
        );

        for (const auto& component: m_components)
        {
            if (component->GetName() == name)
            {
                return dynamic_cast<T*>(component.get());
            }
        }
        return nullptr;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Remove all components from a type from the actor
    ///
    /// \tparam T The type of the components to remove
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    void RemoveComponents(void)
    {
        static_assert(
            std::is_base_of<UActorComponent, T>::value,
            "T must be derived from UActorComponent"
        );

        m_components.Erase(
            std::remove_if(
                m_components.Begin(),
                m_components.End(),
                [](const Component& component)
                { return dynamic_cast<T*>(component.get()) != nullptr; }
            ),
            m_components.End()
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Remove a component by name from the actor
    ///
    /// \param name The name of the component to remove
    ///
    ///////////////////////////////////////////////////////////////////////////
    void RemoveComponent(const FString& name);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Remove a specific component from the actor
    ///
    /// \param component Pointer to the component to remove
    ///
    ///////////////////////////////////////////////////////////////////////////
    void RemoveComponent(UActorComponent* component);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Mark the actor for deletion
    ///
    ///////////////////////////////////////////////////////////////////////////
    void MarkForDeletion(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Check if the actor is marked for deletion
    ///
    /// \return True if the actor is marked for deletion, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool IsMarkedForDeletion(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Check wether the transform property is replicated
    ///
    /// \return True if the transform property is replicated, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool IsTransformReplicated(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set whether the transform property is replicated
    ///
    /// \param replicated True to replicate the transform property, false
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetTransformReplicated(Bool replicated);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Translate the actor by a given vector
    ///
    /// \param translation The vector to translate the actor by
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Translate(const FVector3& translation);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Translate the actor by a given vector
    ///
    /// \param x The x component of the translation vector
    /// \param y The y component of the translation vector
    /// \param z The z component of the translation vector
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Translate(Float32 x, Float32 y, Float32 z);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Rotate the actor by a given vector
    ///
    /// \param rotation The vector to rotate the actor by (in degrees)
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Rotate(const FVector3& rotation);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Rotate the actor by a given rotator
    ///
    /// \param rotation The rotator to rotate the actor by (in degrees)
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Rotate(const FRotator& rotation);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Rotate the actor by a given vector
    ///
    /// \param pitch The pitch component of the rotation vector
    /// \param yaw The yaw component of the rotation vector
    /// \param roll The roll component of the rotation vector
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Rotate(Float32 pitch, Float32 yaw, Float32 roll);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Scale the actor by a given vector
    ///
    /// \param factor The vector to scale the actor by
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Scale(const FVector3& factor);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Scale the actor by a given vector
    ///
    /// \param x The x component of the factor vector
    /// \param y The y component of the factor vector
    /// \param z The z component of the factor vector
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Scale(Float32 x, Float32 y, Float32 z);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Apply movement with client-side prediction and server
    /// reconciliation
    ///
    /// \param inputVector The input vector for this frame
    /// \param deltaTime The time elapsed since the last frame
    ///
    ///////////////////////////////////////////////////////////////////////////
    void ApplyMovement(const FVector3& inputVector, Float32 deltaTime);

public:
    ///////////////////////////////////////////////////////////////////////////
    // Class metadata
    ///////////////////////////////////////////////////////////////////////////
    DECLARE_CLASS_WITH_SUPER(AActor, UObject)
};

}   // namespace tkd
