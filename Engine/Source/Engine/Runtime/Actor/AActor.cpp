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
    , m_transform(*this, "Transform", FTransform::Identity)
    , m_isActive(*this, "IsActive", true)
    , m_components()
    , m_markedForDeletion(false)
    , m_netRole(ENetRole::None)
    , m_networkID(0)
    , m_owningClientID(0)
    , m_netUpdateFrequency(10.0f)
    , m_timeSinceLastUpdate(0.0f)
    , OnActorBeginOverlap("OnActorBeginOverlap", *this)
    , OnActorEndOverlap("OnActorEndOverlap", *this)
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
Bool AActor::IsLocallyControlled(void) const
{
    // TODO: Add proper locally controller check
    return true;
}

///////////////////////////////////////////////////////////////////////////////
Bool AActor::IsAuthority(void) const
{
    return m_netRole == ENetRole::Authority;
}

///////////////////////////////////////////////////////////////////////////////
Bool AActor::IsSimulated(void) const
{
    return m_netRole == ENetRole::SimulatedProxy;
}

///////////////////////////////////////////////////////////////////////////////
ENetRole AActor::GetNetRole(void) const { return m_netRole; }

///////////////////////////////////////////////////////////////////////////////
void AActor::SetNetRole(ENetRole role) { m_netRole = role; }

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
    if (replicated) { m_transform.AddFlag(EPropertyFlags::Replicated); }
    else { m_transform.RemoveFlag(EPropertyFlags::Replicated); }
}

///////////////////////////////////////////////////////////////////////////////
void AActor::Translate(const FVector3& translation)
{
    m_transform->Translate(translation);
}

///////////////////////////////////////////////////////////////////////////////
void AActor::Translate(Float32 x, Float32 y, Float32 z)
{
    m_transform->Translate(FVector3(x, y, z));
}

///////////////////////////////////////////////////////////////////////////////
void AActor::Rotate(const FVector3& rotation)
{
    FRotator rotator = FRotator(rotation.x, rotation.y, rotation.z);
    m_transform->Rotate(rotator);
}

///////////////////////////////////////////////////////////////////////////////
void AActor::Rotate(const FRotator& rotation)
{
    m_transform->Rotate(rotation);
}

///////////////////////////////////////////////////////////////////////////////
void AActor::Rotate(Float32 pitch, Float32 yaw, Float32 roll)
{
    FRotator rotator = FRotator(pitch, yaw, roll);
    m_transform->Rotate(rotator);
}

///////////////////////////////////////////////////////////////////////////////
void AActor::Scale(const FVector3& scale) { m_transform->Scale(scale); }

///////////////////////////////////////////////////////////////////////////////
void AActor::Scale(Float32 x, Float32 y, Float32 z)
{
    m_transform->Scale(FVector3(x, y, z));
}

///////////////////////////////////////////////////////////////////////////////
void AActor::SetNetUpdateFrequency(Float32 frequency)
{
    m_netUpdateFrequency = frequency;
}

///////////////////////////////////////////////////////////////////////////////
Float32 AActor::GetNetUpdateFrequency(void) const
{
    return m_netUpdateFrequency;
}

///////////////////////////////////////////////////////////////////////////////
void AActor::SetOwningClientID(UInt32 id) { m_owningClientID = id; }

///////////////////////////////////////////////////////////////////////////////
UInt32 AActor::GetOwningClientID(void) const { return m_owningClientID; }

///////////////////////////////////////////////////////////////////////////////
UInt32 AActor::GetNetworkID(void) const { return m_networkID; }

///////////////////////////////////////////////////////////////////////////////
void AActor::SetNetworkID(UInt32 id) { m_networkID = id; }

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS(AActor);

}   // namespace tkd
