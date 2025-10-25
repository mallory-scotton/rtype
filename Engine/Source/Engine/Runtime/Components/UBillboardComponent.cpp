///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Runtime/Components/UBillboardComponent.hpp>
#include <Engine/Assets/URessource.hpp>
#include <Engine/Runtime/Actor/AActor.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
UBillboardComponent::UBillboardComponent(const FString& name)
    : UActorComponent(name)
    , m_billboard()
    , m_localTransform(FTransform::Identity)
{}

///////////////////////////////////////////////////////////////////////////////
UBillboardPrimitive& UBillboardComponent::GetBillboard(void)
{
    return m_billboard;
}

///////////////////////////////////////////////////////////////////////////////
const UBillboardPrimitive& UBillboardComponent::GetBillboard(void) const
{
    return m_billboard;
}

///////////////////////////////////////////////////////////////////////////////
const FTransform& UBillboardComponent::GetLocalTransform(void) const
{
    return m_localTransform;
}

///////////////////////////////////////////////////////////////////////////////
void UBillboardComponent::SetLocalTransform(const FTransform& transform)
{
    m_localTransform = transform;
}

///////////////////////////////////////////////////////////////////////////////
const FilePath& UBillboardComponent::GetTexturePath(void) const
{
    return m_texturePath;
}

///////////////////////////////////////////////////////////////////////////////
void UBillboardComponent::SetTexturePath(const FilePath& texturePath)
{
    // Update the texture path
    m_texturePath = texturePath;

    // Try to get cached texture first
    m_textureHandle = URessource::GetTextureHandle(m_texturePath);

    // Set the texture on the billboard if valid
    if (m_textureHandle.IsValid())
    {
        ITexture* texture = m_textureHandle.Get();
        m_billboard.SetTexture(texture);

        // Set the texture rectangle to cover the entire texture
        FVector2u textureSize = texture->GetSize();
        m_billboard.SetTextureRect(
            FRectanglei(0, 0, textureSize.x, textureSize.y)
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
void UBillboardComponent::Render(IRenderer& renderer) const
{
    if (IsHiddenInGame()) { return; }

    // Update the billboard transform with owner + local transform
    FTransform ownerTransform = GetOwner()->GetTransform();
    m_billboard.SetTransform(ownerTransform * m_localTransform);

    FRenderStates states;
    m_billboard.Draw(renderer, states);
}

}   // namespace tkd
