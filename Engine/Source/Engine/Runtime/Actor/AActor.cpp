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
    , self(*this)
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
const FTransform& AActor::GetTransform(void) const
{
    return m_transform.GetValue();
}

///////////////////////////////////////////////////////////////////////////////
void AActor::SetTransform(const FTransform& transform)
{
    m_transform = transform;
}

///////////////////////////////////////////////////////////////////////////////
Bool AActor::IsActive(void) const { return m_isActive; }

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
IMPLEMENT_CLASS(AActor);

}   // namespace tkd
