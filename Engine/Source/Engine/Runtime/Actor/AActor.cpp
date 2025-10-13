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
    , m_transformTimestamp(0)
    , OnActorBeginOverlap(*this, "OnActorBeginOverlap")
    , OnActorEndOverlap(*this, "OnActorEndOverlap")
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

    if (m_transform.HasFlag(EPropertyFlags::Replicated))
    {
        if (IsLocallyControlled() && IsAuthority())
        {
            // TODO: Apply local input prediction here
        }
        else if (IsLocallyControlled())
        {
            // TODO: Apply client-side prediction here
        }
        else if (IsAuthority())
        {
            // TODO: Send transform updates to clients here
        }
        else
        {
            // TODO: Apply server simulation here
        }
    }

    // Reset pending transform after applying it
    m_pendingTransform = FTransform::Identity;
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
        m_transform.AddFlag(EPropertyFlags::Replicated);
        if (!m_hasSetUpdateFrequency) { m_netUpdateFrequency = 20.f; }
    }
    else
    {
        m_transform.RemoveFlag(EPropertyFlags::Replicated);
        if (!m_hasSetUpdateFrequency) { m_netUpdateFrequency = 10.f; }
    }
}

///////////////////////////////////////////////////////////////////////////////
void AActor::Translate(const FVector3& translation)
{
    if (m_transform.HasFlag(EPropertyFlags::Replicated))
    {
        m_pendingTransform.Translate(translation);
    }
    else { m_transform->Translate(translation); }
}

///////////////////////////////////////////////////////////////////////////////
void AActor::Translate(Float32 x, Float32 y, Float32 z)
{
    if (m_transform.HasFlag(EPropertyFlags::Replicated))
    {
        m_pendingTransform.Translate(FVector3(x, y, z));
    }
    else { m_transform->Translate(FVector3(x, y, z)); }
}

///////////////////////////////////////////////////////////////////////////////
void AActor::Rotate(const FVector3& rotation)
{
    FRotator rotator = FRotator(rotation.x, rotation.y, rotation.z);
    if (m_transform.HasFlag(EPropertyFlags::Replicated))
    {
        m_pendingTransform.Rotate(rotator);
    }
    else { m_transform->Rotate(rotator); }
}

///////////////////////////////////////////////////////////////////////////////
void AActor::Rotate(const FRotator& rotation)
{
    if (m_transform.HasFlag(EPropertyFlags::Replicated))
    {
        m_pendingTransform.Rotate(rotation);
    }
    else { m_transform->Rotate(rotation); }
}

///////////////////////////////////////////////////////////////////////////////
void AActor::Rotate(Float32 pitch, Float32 yaw, Float32 roll)
{
    FRotator rotator = FRotator(pitch, yaw, roll);
    if (m_transform.HasFlag(EPropertyFlags::Replicated))
    {
        m_pendingTransform.Rotate(rotator);
    }
    else { m_transform->Rotate(rotator); }
}

///////////////////////////////////////////////////////////////////////////////
void AActor::Scale(const FVector3& scale)
{
    if (m_transform.HasFlag(EPropertyFlags::Replicated))
    {
        m_pendingTransform.Scale(scale);
    }
    else { m_transform->Scale(scale); }
}

///////////////////////////////////////////////////////////////////////////////
void AActor::Scale(Float32 x, Float32 y, Float32 z)
{
    if (m_transform.HasFlag(EPropertyFlags::Replicated))
    {
        m_pendingTransform.Scale(FVector3(x, y, z));
    }
    else { m_transform->Scale(FVector3(x, y, z)); }
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS(AActor);

}   // namespace tkd
