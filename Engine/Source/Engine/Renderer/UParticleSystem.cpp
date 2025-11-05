///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Renderer/UParticleSystem.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
bool FParticle::IsAlive(void) const { return lifetime > 0.0f; }

///////////////////////////////////////////////////////////////////////////////
UParticleSystem::UParticleSystem(SizeT maxParticles, ITexture* texture)
    : m_maxParticles(maxParticles)
    , m_texture(texture)
    , m_emitterPosition(0.0f, 0.0f, 0.0f)
    , m_emitterType(EParticleEmitterType::Point)
    , m_emitterRadius(0.5f)
    , m_lifetimeMin(1.0f)
    , m_lifetimeMax(3.0f)
    , m_sizeMin(0.1f)
    , m_sizeMax(0.3f)
    , m_speedMin(1.0f)
    , m_speedMax(3.0f)
    , m_colorStart(1.0f, 1.0f, 1.0f, 1.0f)
    , m_colorEnd(1.0f, 1.0f, 1.0f, 0.0f)
    , m_gravity(0.0f, -2.0f, 0.0f)
    , m_additiveBlending(false)
    , m_rng(std::random_device{}())
    , m_dist(0.0f, 1.0f)
{
    m_particles.reserve(maxParticles);
}

///////////////////////////////////////////////////////////////////////////////
void UParticleSystem::Update(float deltaTime)
{
    // Update existing particles
    for (auto& particle: m_particles)
    {
        if (!particle.IsAlive()) { continue; }

        // Update lifetime
        particle.lifetime -= deltaTime;

        // Update physics
        particle.velocity += m_gravity * deltaTime;
        particle.position += particle.velocity * deltaTime;
        particle.rotation += particle.rotationSpeed * deltaTime;

        // Update color (fade over lifetime)
        float t = 1.0f - (particle.lifetime / particle.maxLifetime);
        particle.color.r =
            m_colorStart.r + (m_colorEnd.r - m_colorStart.r) * t;
        particle.color.g =
            m_colorStart.g + (m_colorEnd.g - m_colorStart.g) * t;
        particle.color.b =
            m_colorStart.b + (m_colorEnd.b - m_colorStart.b) * t;
        particle.color.a =
            m_colorStart.a + (m_colorEnd.a - m_colorStart.a) * t;
    }

    // Remove dead particles
    m_particles.erase(
        std::remove_if(
            m_particles.begin(),
            m_particles.end(),
            [](const FParticle& p) { return !p.IsAlive(); }
        ),
        m_particles.end()
    );
}

///////////////////////////////////////////////////////////////////////////////
void UParticleSystem::Draw(IRenderer& renderer)
{
    if (m_particles.empty() || !m_texture) { return; }

    // Setup blending
    renderer.SetBlend(true);
    if (m_additiveBlending)
    {
        renderer.SetBlendFunc(EBlendFactor::SrcAlpha, EBlendFactor::One);
    }
    else
    {
        renderer.SetBlendFunc(
            EBlendFactor::SrcAlpha, EBlendFactor::OneMinusSrcAlpha
        );
    }

    // Disable depth writing but keep depth testing
    renderer.SetDepthMask(false);
    renderer.SetDepthTest(true);

    // Get camera vectors for billboarding
    FCamera camera = renderer.GetCamera();
    FVector3 camRight = camera.right;
    FVector3 camUp = camera.up;

    // Bind texture
    m_texture->Bind();
    renderer.SetTexture2D(true);

    // Draw each particle as a billboarded quad
    for (const auto& particle: m_particles)
    {
        if (!particle.IsAlive()) { continue; }

        float halfSize = particle.size * 0.5f;

        // Calculate billboard corners in world space
        FVector3 right = camRight * halfSize;
        FVector3 up = camUp * halfSize;

        // Optional: Apply rotation
        if (particle.rotation != 0.0f)
        {
            float cosR = std::cos(particle.rotation);
            float sinR = std::sin(particle.rotation);
            FVector3 newRight = right * cosR - up * sinR;
            FVector3 newUp = right * sinR + up * cosR;
            right = newRight;
            up = newUp;
        }

        // Create quad vertices
        std::vector<FVertex> vertices;
        vertices.push_back(FVertex(
            particle.position - right - up,
            particle.color,
            FVector2(0.0f, 1.0f)
        ));
        vertices.push_back(FVertex(
            particle.position + right - up,
            particle.color,
            FVector2(1.0f, 1.0f)
        ));
        vertices.push_back(FVertex(
            particle.position + right + up,
            particle.color,
            FVector2(1.0f, 0.0f)
        ));
        vertices.push_back(FVertex(
            particle.position - right + up,
            particle.color,
            FVector2(0.0f, 0.0f)
        ));

        // Draw quad
        renderer.Draw(
            vertices.data(),
            vertices.size(),
            EPrimitiveType::Quads,
            FTransform()
        );
    }

    m_texture->Unbind();
    renderer.SetTexture2D(false);

    // Restore rendering state
    renderer.SetDepthMask(true);
    renderer.SetBlend(false);
}

///////////////////////////////////////////////////////////////////////////////
void UParticleSystem::EmitParticles(SizeT count)
{
    for (SizeT i = 0; i < count && m_particles.size() < m_maxParticles; ++i)
    {
        FParticle particle;
        InitializeParticle(particle);
        m_particles.push_back(particle);
    }
}

///////////////////////////////////////////////////////////////////////////////
void UParticleSystem::InitializeParticle(FParticle& particle)
{
    // Set position based on emitter type
    switch (m_emitterType)
    {
    case EParticleEmitterType::Point:
        particle.position = m_emitterPosition;
        break;

    case EParticleEmitterType::Sphere:
    {
        FVector3 dir = GetRandomDirection();
        particle.position = m_emitterPosition + dir * m_emitterRadius;
        break;
    }

    case EParticleEmitterType::Box:
    {
        particle.position = FVector3(
            m_emitterPosition.x +
                (m_dist(m_rng) - 0.5f) * m_emitterRadius * 2.0f,
            m_emitterPosition.y +
                (m_dist(m_rng) - 0.5f) * m_emitterRadius * 2.0f,
            m_emitterPosition.z +
                (m_dist(m_rng) - 0.5f) * m_emitterRadius * 2.0f
        );
        break;
    }

    case EParticleEmitterType::Cone:
    {
        float angle = m_dist(m_rng) * 2.0f * M_PI;
        float radius = m_dist(m_rng) * m_emitterRadius;
        particle.position = FVector3(
            m_emitterPosition.x + std::cos(angle) * radius,
            m_emitterPosition.y,
            m_emitterPosition.z + std::sin(angle) * radius
        );
        break;
    }
    }

    // Set velocity
    FVector3 direction = GetRandomDirection();
    float speed = RandomFloat(m_speedMin, m_speedMax);
    particle.velocity = direction * speed;

    // Set other properties
    particle.color = m_colorStart;
    particle.size = RandomFloat(m_sizeMin, m_sizeMax);
    particle.maxLifetime = RandomFloat(m_lifetimeMin, m_lifetimeMax);
    particle.lifetime = particle.maxLifetime;
    particle.rotation = RandomFloat(0.0f, 2.0f * M_PI);
    particle.rotationSpeed = RandomFloat(-2.0f, 2.0f);
}

///////////////////////////////////////////////////////////////////////////////
FVector3 UParticleSystem::GetRandomDirection(void)
{
    float theta = m_dist(m_rng) * 2.0f * M_PI;
    float phi = std::acos(2.0f * m_dist(m_rng) - 1.0f);

    return FVector3(
        std::sin(phi) * std::cos(theta),
        std::sin(phi) * std::sin(theta),
        std::cos(phi)
    );
}

///////////////////////////////////////////////////////////////////////////////
float UParticleSystem::RandomFloat(float min, float max)
{
    return min + m_dist(m_rng) * (max - min);
}

///////////////////////////////////////////////////////////////////////////////
void UParticleSystem::SetEmitterPosition(const FVector3& position)
{
    m_emitterPosition = position;
}

///////////////////////////////////////////////////////////////////////////////
void UParticleSystem::SetEmitterType(EParticleEmitterType type)
{
    m_emitterType = type;
}

///////////////////////////////////////////////////////////////////////////////
void UParticleSystem::SetParticleLifetime(float min, float max)
{
    m_lifetimeMin = min;
    m_lifetimeMax = max;
}

///////////////////////////////////////////////////////////////////////////////
void UParticleSystem::SetParticleSize(float min, float max)
{
    m_sizeMin = min;
    m_sizeMax = max;
}

///////////////////////////////////////////////////////////////////////////////
void UParticleSystem::SetParticleSpeed(float min, float max)
{
    m_speedMin = min;
    m_speedMax = max;
}

///////////////////////////////////////////////////////////////////////////////
void UParticleSystem::SetParticleColor(
    const FColor& startColor, const FColor& endColor
)
{
    m_colorStart = startColor;
    m_colorEnd = endColor;
}

///////////////////////////////////////////////////////////////////////////////
void UParticleSystem::SetEmitterRadius(float radius)
{
    m_emitterRadius = radius;
}

///////////////////////////////////////////////////////////////////////////////
void UParticleSystem::SetGravity(const FVector3& gravity)
{
    m_gravity = gravity;
}

///////////////////////////////////////////////////////////////////////////////
void UParticleSystem::SetTexture(ITexture* texture) { m_texture = texture; }

///////////////////////////////////////////////////////////////////////////////
void UParticleSystem::SetAdditiveBlending(bool additive)
{
    m_additiveBlending = additive;
}

///////////////////////////////////////////////////////////////////////////////
void UParticleSystem::Clear(void) { m_particles.clear(); }

}   // namespace tkd
