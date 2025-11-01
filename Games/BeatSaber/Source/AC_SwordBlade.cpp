///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <AC_SwordBlade.hpp>
#if TKD_ENGINE_CLIENT
    #include <GL/glew.h>
    #include <GL/glu.h>
#endif

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
AC_SwordBlade::AC_SwordBlade(const FString& name, VR::EHand hand)
    : UActorComponent(name)
    , m_hand(hand)
    , m_trailMaxPoints(30)
    , m_trailMinVelocity(0.1f)
{}

///////////////////////////////////////////////////////////////////////////////
void AC_SwordBlade::BeginPlay(void) {}

///////////////////////////////////////////////////////////////////////////////
void AC_SwordBlade::Tick(float deltaTime)
{
#if TKD_ENGINE_CLIENT
    // Update the controller state
    VR::FVRSystem& vrSystem = Window::GetVRSystem();

    if (vrSystem.IsInitialized())
    {
        m_state = vrSystem.GetControllerState(m_hand);
    }

    // Update trail points
    UpdateTrail(deltaTime);
#else
    TKD_UNUSED(deltaTime);
#endif
}

///////////////////////////////////////////////////////////////////////////////
void AC_SwordBlade::UpdateTrail(float deltaTime)
{
#if TKD_ENGINE_CLIENT
    FTransform transform = GetOwner()->GetTransform();

    // Calculate blade tip position (end of the blade cylinder)
    FVector3 bladeTip = transform.TransformPoint(FVector3(0, 0, -1.0f));
    FVector3 bladeBase = transform.TransformPoint(FVector3(0, 0, 0));

    // Calculate velocity
    if (!m_trailPoints.empty())
    {
        FVector3 lastTip = m_trailPoints.back().tipPosition;
        float velocity = (bladeTip - lastTip).Length() / deltaTime;

        // Only add points when moving fast enough
        if (velocity > m_trailMinVelocity)
        {
            TrailPoint point;
            point.tipPosition = bladeTip;
            point.basePosition = bladeBase;
            point.lifetime = 0.0f;
            m_trailPoints.push_back(point);

            // Limit trail length
            if (m_trailPoints.size() > m_trailMaxPoints)
            {
                m_trailPoints.erase(m_trailPoints.begin());
            }
        }
    }
    else
    {
        // Initialize first point
        TrailPoint point;
        point.tipPosition = bladeTip;
        point.basePosition = bladeBase;
        point.lifetime = 0.0f;
        m_trailPoints.push_back(point);
    }

    // Update lifetimes and fade
    for (SizeT i = 0; i < m_trailPoints.size(); i++)
    {
        m_trailPoints[i].lifetime += deltaTime;
    }

    // Remove old points (fade duration)
    const float maxLifetime = 0.3f;   // Trail duration in seconds
    while (!m_trailPoints.empty() &&
           m_trailPoints.front().lifetime > maxLifetime)
    {
        m_trailPoints.erase(m_trailPoints.begin());
    }
#else
    TKD_UNUSED(deltaTime);
#endif
}

///////////////////////////////////////////////////////////////////////////////
void AC_SwordBlade::Render(IRenderer& renderer) const
{
#if TKD_ENGINE_CLIENT
    glPushMatrix();

    // Apply full transform matrix (rotation + position)
    FTransform transform = GetOwner()->GetTransform();
    float matrixData[16];
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            matrixData[i * 4 + j] = transform(j, i);
        }
    }
    glMultMatrixf(matrixData);

    // Position for blade rendering
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -1.0f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);   // rotate +Y → –Z

    // Define vibrant colors based on hand
    float coreR, coreG, coreB;
    float glowR, glowG, glowB;

    if (m_hand == VR::EHand::Right)
    {
        // Intense red for right hand
        coreR = 1.0f;
        coreG = 0.1f;
        coreB = 0.2f;
        glowR = 0.9f;
        glowG = 0.0f;
        glowB = 0.1f;
    }
    else
    {
        // Vibrant blue for left hand
        coreR = 0.1f;
        coreG = 0.3f;
        coreB = 1.0f;
        glowR = 0.0f;
        glowG = 0.2f;
        glowB = 1.0f;
    }

    // Enable blending for glow effects
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDepthMask(GL_FALSE);

    // Layer 3: Outer glow (largest, most transparent)
    GLfloat outerGlow[] = { glowR * 0.3f, glowG * 0.3f, glowB * 0.3f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, outerGlow);
    glColor4f(glowR, glowG, glowB, 0.15f);
    DrawCylinder(0.045f, 1.0f, 32);

    // Layer 2: Middle glow (medium size, semi-transparent)
    GLfloat middleGlow[] = { glowR * 0.6f, glowG * 0.6f, glowB * 0.6f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, middleGlow);
    glColor4f(glowR, glowG, glowB, 0.35f);
    DrawCylinder(0.03f, 1.0f, 32);

    // Layer 1: Inner bright glow
    GLfloat innerGlow[] = { coreR * 0.9f, coreG * 0.9f, coreB * 0.9f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, innerGlow);
    glColor4f(coreR, coreG, coreB, 0.7f);
    DrawCylinder(0.022f, 1.0f, 32);

    // Core: Bright solid core with maximum emission
    glDepthMask(GL_TRUE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GLfloat coreEmission[] = {
        coreR * 1.2f, coreG * 1.2f, coreB * 1.2f, 1.0f
    };
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, coreEmission);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    DrawCylinder(0.015f, 1.0f, 32);

    // Add glowing tip effect
    glDepthMask(GL_FALSE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    // Tip glow sphere
    glPushMatrix();
    glTranslatef(0.0f, 1.0f, 0.0f);
    glColor4f(coreR, coreG, coreB, 0.8f);
    DrawSphere(0.04f, 16, 16);
    glPopMatrix();

    // Tip bright core
    glPushMatrix();
    glTranslatef(0.0f, 1.0f, 0.0f);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    DrawSphere(0.02f, 16, 16);
    glPopMatrix();

    // Restore OpenGL state
    GLfloat noEmission[] = { 0, 0, 0, 1 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmission);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    glPopMatrix();
    glPopMatrix();

    // Render trail (after popping the transform matrix so we use world space)
    DrawTrail();
#else
    TKD_UNUSED(renderer);
#endif
}

///////////////////////////////////////////////////////////////////////////////
void AC_SwordBlade::DrawCylinder(float radius, float height, int segments)
    const
{
#if TKD_ENGINE_CLIENT
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= segments; i++)
    {
        float angle = (i / (float)segments) * 2.0f * (float)M_PI;
        float x = radius * std::cos(angle);
        float z = radius * std::sin(angle);
        glVertex3f(x, 0, z);
        glVertex3f(x, height, z);
    }
    glEnd();

    // Top cap
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, height, 0);
    for (int i = 0; i <= segments; i++)
    {
        float angle = (i / (float)segments) * 2.0f * (float)M_PI;
        glVertex3f(radius * std::cos(angle), height, radius * std::sin(angle));
    }
    glEnd();

    // Bottom cap
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, 0, 0);
    for (int i = segments; i >= 0; i--)
    {
        float angle = (i / (float)segments) * 2.0f * (float)M_PI;
        glVertex3f(radius * std::cos(angle), 0, radius * std::sin(angle));
    }
    glEnd();
#else
    TKD_UNUSED(radius);
    TKD_UNUSED(height);
    TKD_UNUSED(segments);
#endif
}

///////////////////////////////////////////////////////////////////////////////
void AC_SwordBlade::DrawSphere(float radius, int slices, int stacks) const
{
#if TKD_ENGINE_CLIENT
    for (int i = 0; i < stacks; i++)
    {
        float lat0 = (float)M_PI * (-0.5f + (float)i / stacks);
        float z0 = radius * std::sin(lat0);
        float zr0 = radius * std::cos(lat0);

        float lat1 = (float)M_PI * (-0.5f + (float)(i + 1) / stacks);
        float z1 = radius * std::sin(lat1);
        float zr1 = radius * std::cos(lat1);

        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j <= slices; j++)
        {
            float lng = 2.0f * (float)M_PI * (float)j / slices;
            float x = std::cos(lng);
            float y = std::sin(lng);

            glNormal3f(x * zr0, y * zr0, z0);
            glVertex3f(x * zr0, y * zr0, z0);
            glNormal3f(x * zr1, y * zr1, z1);
            glVertex3f(x * zr1, y * zr1, z1);
        }
        glEnd();
    }
#else
    TKD_UNUSED(radius);
    TKD_UNUSED(slices);
    TKD_UNUSED(stacks);
#endif
}

///////////////////////////////////////////////////////////////////////////////
void AC_SwordBlade::DrawTrail(void) const
{
#if TKD_ENGINE_CLIENT
    if (m_trailPoints.size() < 2) { return; }

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);   // Additive blending for glow effect
    glDepthMask(GL_FALSE);   // Don't write to depth buffer for trails

    // Disable lighting for emissive trail effect
    glDisable(GL_LIGHTING);

    const float maxLifetime = 0.3f;

    // Choose vibrant trail color based on hand
    float r = m_hand == VR::EHand::Right ? 1.0f : 0.1f;
    float g = m_hand == VR::EHand::Right ? 0.1f : 0.3f;
    float b = m_hand == VR::EHand::Right ? 0.2f : 1.0f;

    // Layer 1: Outer glow trail (widest, most transparent)
    glBegin(GL_QUAD_STRIP);
    for (SizeT i = 0; i < m_trailPoints.size(); i++)
    {
        const TrailPoint& point = m_trailPoints[i];

        float alpha = 1.0f - (point.lifetime / maxLifetime);
        alpha = alpha * alpha;

        float positionFade = (float)i / (float)m_trailPoints.size();
        alpha *= positionFade;

        glColor4f(r * 0.7f, g * 0.7f, b * 0.7f, alpha * 0.3f);

        // Wider trail for outer glow
        FVector3 dir = point.tipPosition - point.basePosition;
        FVector3 wider1 = point.basePosition - dir * 0.1f;
        FVector3 wider2 = point.tipPosition + dir * 0.1f;

        glVertex3f(wider1.x, wider1.y, wider1.z);
        glVertex3f(wider2.x, wider2.y, wider2.z);
    }
    glEnd();

    // Layer 2: Main colored trail
    glBegin(GL_QUAD_STRIP);
    for (SizeT i = 0; i < m_trailPoints.size(); i++)
    {
        const TrailPoint& point = m_trailPoints[i];

        float alpha = 1.0f - (point.lifetime / maxLifetime);
        alpha = alpha * alpha;

        float positionFade = (float)i / (float)m_trailPoints.size();
        alpha *= positionFade;

        glColor4f(r, g, b, alpha * 0.6f);

        glVertex3f(
            point.basePosition.x, point.basePosition.y, point.basePosition.z
        );
        glVertex3f(
            point.tipPosition.x, point.tipPosition.y, point.tipPosition.z
        );
    }
    glEnd();

    // Layer 3: Bright core trail (whiter, sharper fade)
    glBegin(GL_QUAD_STRIP);
    for (SizeT i = 0; i < m_trailPoints.size(); i++)
    {
        const TrailPoint& point = m_trailPoints[i];

        float alpha = 1.0f - (point.lifetime / maxLifetime);
        alpha = alpha * alpha * alpha;   // Sharper fade for core

        float positionFade = (float)i / (float)m_trailPoints.size();
        alpha *= positionFade;

        // Bright white-ish core with color tint
        glColor4f(
            0.9f + r * 0.1f, 0.9f + g * 0.1f, 0.9f + b * 0.1f, alpha * 0.8f
        );

        // Narrower core (interpolate between base and tip)
        FVector3 mid1 = point.basePosition +
                        (point.tipPosition - point.basePosition) * 0.35f;
        FVector3 mid2 = point.basePosition +
                        (point.tipPosition - point.basePosition) * 0.65f;

        glVertex3f(mid1.x, mid1.y, mid1.z);
        glVertex3f(mid2.x, mid2.y, mid2.z);
    }
    glEnd();

    // Layer 4: Ultra-bright center line
    glLineWidth(3.0f);
    glBegin(GL_LINE_STRIP);
    for (SizeT i = 0; i < m_trailPoints.size(); i++)
    {
        const TrailPoint& point = m_trailPoints[i];

        float alpha = 1.0f - (point.lifetime / maxLifetime);
        alpha = alpha * alpha * alpha * alpha;

        float positionFade = (float)i / (float)m_trailPoints.size();
        alpha *= positionFade;

        glColor4f(1.0f, 1.0f, 1.0f, alpha);

        // Center line
        FVector3 center = point.basePosition +
                          (point.tipPosition - point.basePosition) * 0.5f;
        glVertex3f(center.x, center.y, center.z);
    }
    glEnd();
    glLineWidth(1.0f);

    // Restore OpenGL state
    glEnable(GL_LIGHTING);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
#endif
}

}   // namespace tkd
