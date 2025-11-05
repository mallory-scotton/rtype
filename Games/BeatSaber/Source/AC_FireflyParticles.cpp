///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <AC_FireflyParticles.hpp>
#include <Engine/Assets/URessource.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
AC_FireflyParticles::AC_FireflyParticles(const FString& name)
    : UActorComponent(name)
    , m_particles(500)
{}

///////////////////////////////////////////////////////////////////////////////
void AC_FireflyParticles::BeginPlay(void)
{
    // Load the firefly texture
    m_handle =
        URessource::GetTextureHandle("Assets/Textures/CircleGradient.png");

    // Configure the particle system
    m_particles.SetTexture(m_handle.Get());
    m_particles.SetEmitterType(EParticleEmitterType::Box);
    m_particles.SetEmitterRadius(5.0f);
    m_particles.SetParticleLifetime(3.0f, 5.0f);
    m_particles.SetParticleSize(0.005f, 0.02f);
    m_particles.SetParticleSpeed(0.0f, 0.1f);
    m_particles.SetGravity(FVector3(0.0f, 0.0f, 0.0f));
    m_particles.SetParticleColor(
        FColor(0.8f, 0.9f, 1.0f, 0.8f),   // Start: bright blue-white
        FColor(0.6f, 0.7f, 0.9f, 0.0f)    // End: dimmer blue, transparent
    );
    m_particles.SetAdditiveBlending(true);
}

///////////////////////////////////////////////////////////////////////////////
void AC_FireflyParticles::Render(IRenderer& renderer) const
{
    if (IsHiddenInGame()) { return; }

    // Render the particle system
    m_particles.Draw(renderer);
}

///////////////////////////////////////////////////////////////////////////////
void AC_FireflyParticles::Tick(float deltaTime)
{
    // Update the particle system
    m_particles.Update(deltaTime);

    // Update the particle system transform with owner transform
    FTransform ownerTransform = GetOwner()->GetTransform();
    m_particles.SetEmitterPosition(ownerTransform.GetPosition());

    // Emit new particles
    m_particles.EmitParticles(1);
}

}   // namespace tkd
