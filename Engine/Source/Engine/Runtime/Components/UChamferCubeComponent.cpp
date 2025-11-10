///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Runtime/Components/UChamferCubeComponent.hpp>
#include <Engine/Runtime/Actor/AActor.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
UChamferCubeComponent::UChamferCubeComponent(
    const FString& name, Float32 chamferSize
)
    : UActorComponent(name)
    , m_chamferCube(FColor::White, chamferSize)
    , m_localTransform(FTransform::Identity)
{}

///////////////////////////////////////////////////////////////////////////////
UChamferCubePrimitive& UChamferCubeComponent::GetChamferCube(void)
{
    return m_chamferCube;
}

///////////////////////////////////////////////////////////////////////////////
const UChamferCubePrimitive& UChamferCubeComponent::GetChamferCube(void) const
{
    return m_chamferCube;
}

///////////////////////////////////////////////////////////////////////////////
const FTransform& UChamferCubeComponent::GetLocalTransform(void) const
{
    return m_localTransform;
}

///////////////////////////////////////////////////////////////////////////////
void UChamferCubeComponent::SetLocalTransform(const FTransform& transform)
{
    m_localTransform = transform;
}

///////////////////////////////////////////////////////////////////////////////
void UChamferCubeComponent::SetTexture(ITexture* texture)
{
    m_chamferCube.SetTexture(texture);
}

///////////////////////////////////////////////////////////////////////////////
ITexture* UChamferCubeComponent::GetTexture(void) const
{
    return m_chamferCube.GetTexture();
}

///////////////////////////////////////////////////////////////////////////////
void UChamferCubeComponent::SetChamferSize(Float32 size)
{
    m_chamferCube.SetChamferSize(size);
}

///////////////////////////////////////////////////////////////////////////////
Float32 UChamferCubeComponent::GetChamferSize(void) const
{
    return m_chamferCube.GetChamferSize();
}

///////////////////////////////////////////////////////////////////////////////
void UChamferCubeComponent::Render(IRenderer& renderer) const
{
    if (IsHiddenInGame()) { return; }

    // Update the chamfer cube transform with owner + local transform
    FTransform ownerTransform = GetOwner()->GetTransform();
    m_chamferCube.SetTransform(ownerTransform * m_localTransform);

    FRenderStates states;
    m_chamferCube.Draw(renderer, states);
}

}   // namespace tkd
