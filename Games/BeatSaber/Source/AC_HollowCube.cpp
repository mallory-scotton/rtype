///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <AC_HollowCube.hpp>
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
AC_HollowCube::AC_HollowCube(const FString& name)
    : UActorComponent(name)
    , m_outerScale(16.0f)
    , m_innerScale(14.f)
    , m_thickness(1.f)
    , m_color(FColor::White)
{}

///////////////////////////////////////////////////////////////////////////////
void AC_HollowCube::SetOuterScale(float scale) { m_outerScale = scale; }

///////////////////////////////////////////////////////////////////////////////
void AC_HollowCube::SetInnerScale(float scale) { m_innerScale = scale; }

///////////////////////////////////////////////////////////////////////////////
void AC_HollowCube::SetThickness(float thickness) { m_thickness = thickness; }

///////////////////////////////////////////////////////////////////////////////
void AC_HollowCube::SetColor(const FColor& color) { m_color = color; }

///////////////////////////////////////////////////////////////////////////////
void AC_HollowCube::SetLocalTransform(const FTransform& transform)
{
    m_localTransform = transform;
}

///////////////////////////////////////////////////////////////////////////////
const FTransform& AC_HollowCube::GetLocalTransform(void) const
{
    return m_localTransform;
}

///////////////////////////////////////////////////////////////////////////////
void AC_HollowCube::Render(IRenderer&) const
{
#if TKD_ENGINE_CLIENT
    // Get world transform
    FTransform worldTransform = GetOwner()->GetTransform() * m_localTransform;

    // Extract position, rotation, and scale
    FVector3 position = worldTransform.GetPosition();
    FRotator rotation = worldTransform.GetRotation();
    FVector3 scale = worldTransform.GetScale();

    // Save current matrix state
    glPushMatrix();

    // Apply transformations
    glTranslatef(position.x, position.y, position.z);

    // Apply rotations (order: yaw, pitch, roll)
    glRotatef(rotation.GetYaw(), 0.0f, 1.0f, 0.0f);
    glRotatef(rotation.GetPitch(), 1.0f, 0.0f, 0.0f);
    glRotatef(rotation.GetRoll(), 0.0f, 0.0f, 1.0f);

    // Apply scale
    glScalef(scale.x, scale.y, scale.z);

    // Set color (convert from 0-255 to 0.0-1.0)
    glColor4f(m_color.r, m_color.g, m_color.b, m_color.a);

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Calculate dimensions
    // outerScale and innerScale define the X and Y dimensions
    // thickness defines the Z depth
    float outerHalfX = m_outerScale * 0.5f;
    float outerHalfY = m_outerScale * 0.5f;
    float innerHalfX = m_innerScale * 0.5f;
    float innerHalfY = m_innerScale * 0.5f;
    float halfThickness = m_thickness * 0.5f;

    // Calculate frame width (difference between outer and inner)
    float frameWidth = (m_outerScale - m_innerScale) * 0.5f;

    // Render a hollow square frame with depth (4 rectangular bars forming a
    // square)

    // TOP BAR (Y+)
    glBegin(GL_QUADS);
    // Front face
    glVertex3f(-outerHalfX, outerHalfY, halfThickness);
    glVertex3f(outerHalfX, outerHalfY, halfThickness);
    glVertex3f(outerHalfX, innerHalfY, halfThickness);
    glVertex3f(-outerHalfX, innerHalfY, halfThickness);

    // Back face
    glVertex3f(-outerHalfX, innerHalfY, -halfThickness);
    glVertex3f(outerHalfX, innerHalfY, -halfThickness);
    glVertex3f(outerHalfX, outerHalfY, -halfThickness);
    glVertex3f(-outerHalfX, outerHalfY, -halfThickness);

    // Top face
    glVertex3f(-outerHalfX, outerHalfY, -halfThickness);
    glVertex3f(outerHalfX, outerHalfY, -halfThickness);
    glVertex3f(outerHalfX, outerHalfY, halfThickness);
    glVertex3f(-outerHalfX, outerHalfY, halfThickness);

    // Bottom face
    glVertex3f(-outerHalfX, innerHalfY, halfThickness);
    glVertex3f(outerHalfX, innerHalfY, halfThickness);
    glVertex3f(outerHalfX, innerHalfY, -halfThickness);
    glVertex3f(-outerHalfX, innerHalfY, -halfThickness);

    // Left face
    glVertex3f(-outerHalfX, innerHalfY, halfThickness);
    glVertex3f(-outerHalfX, innerHalfY, -halfThickness);
    glVertex3f(-outerHalfX, outerHalfY, -halfThickness);
    glVertex3f(-outerHalfX, outerHalfY, halfThickness);

    // Right face
    glVertex3f(outerHalfX, innerHalfY, halfThickness);
    glVertex3f(outerHalfX, outerHalfY, halfThickness);
    glVertex3f(outerHalfX, outerHalfY, -halfThickness);
    glVertex3f(outerHalfX, innerHalfY, -halfThickness);
    glEnd();

    // BOTTOM BAR (Y-)
    glBegin(GL_QUADS);
    // Front face
    glVertex3f(-outerHalfX, -innerHalfY, halfThickness);
    glVertex3f(outerHalfX, -innerHalfY, halfThickness);
    glVertex3f(outerHalfX, -outerHalfY, halfThickness);
    glVertex3f(-outerHalfX, -outerHalfY, halfThickness);

    // Back face
    glVertex3f(-outerHalfX, -outerHalfY, -halfThickness);
    glVertex3f(outerHalfX, -outerHalfY, -halfThickness);
    glVertex3f(outerHalfX, -innerHalfY, -halfThickness);
    glVertex3f(-outerHalfX, -innerHalfY, -halfThickness);

    // Top face
    glVertex3f(-outerHalfX, -innerHalfY, halfThickness);
    glVertex3f(outerHalfX, -innerHalfY, halfThickness);
    glVertex3f(outerHalfX, -innerHalfY, -halfThickness);
    glVertex3f(-outerHalfX, -innerHalfY, -halfThickness);

    // Bottom face
    glVertex3f(-outerHalfX, -outerHalfY, -halfThickness);
    glVertex3f(outerHalfX, -outerHalfY, -halfThickness);
    glVertex3f(outerHalfX, -outerHalfY, halfThickness);
    glVertex3f(-outerHalfX, -outerHalfY, halfThickness);

    // Left face
    glVertex3f(-outerHalfX, -outerHalfY, halfThickness);
    glVertex3f(-outerHalfX, -outerHalfY, -halfThickness);
    glVertex3f(-outerHalfX, -innerHalfY, -halfThickness);
    glVertex3f(-outerHalfX, -innerHalfY, halfThickness);

    // Right face
    glVertex3f(outerHalfX, -outerHalfY, halfThickness);
    glVertex3f(outerHalfX, -innerHalfY, halfThickness);
    glVertex3f(outerHalfX, -innerHalfY, -halfThickness);
    glVertex3f(outerHalfX, -outerHalfY, -halfThickness);
    glEnd();

    // LEFT BAR (X-)
    glBegin(GL_QUADS);
    // Front face
    glVertex3f(-outerHalfX, -innerHalfY, halfThickness);
    glVertex3f(-innerHalfX, -innerHalfY, halfThickness);
    glVertex3f(-innerHalfX, innerHalfY, halfThickness);
    glVertex3f(-outerHalfX, innerHalfY, halfThickness);

    // Back face
    glVertex3f(-outerHalfX, innerHalfY, -halfThickness);
    glVertex3f(-innerHalfX, innerHalfY, -halfThickness);
    glVertex3f(-innerHalfX, -innerHalfY, -halfThickness);
    glVertex3f(-outerHalfX, -innerHalfY, -halfThickness);

    // Top face
    glVertex3f(-outerHalfX, innerHalfY, -halfThickness);
    glVertex3f(-innerHalfX, innerHalfY, -halfThickness);
    glVertex3f(-innerHalfX, innerHalfY, halfThickness);
    glVertex3f(-outerHalfX, innerHalfY, halfThickness);

    // Bottom face
    glVertex3f(-outerHalfX, -innerHalfY, halfThickness);
    glVertex3f(-innerHalfX, -innerHalfY, halfThickness);
    glVertex3f(-innerHalfX, -innerHalfY, -halfThickness);
    glVertex3f(-outerHalfX, -innerHalfY, -halfThickness);

    // Left face
    glVertex3f(-outerHalfX, -innerHalfY, halfThickness);
    glVertex3f(-outerHalfX, -innerHalfY, -halfThickness);
    glVertex3f(-outerHalfX, innerHalfY, -halfThickness);
    glVertex3f(-outerHalfX, innerHalfY, halfThickness);

    // Right face (inner edge)
    glVertex3f(-innerHalfX, -innerHalfY, halfThickness);
    glVertex3f(-innerHalfX, innerHalfY, halfThickness);
    glVertex3f(-innerHalfX, innerHalfY, -halfThickness);
    glVertex3f(-innerHalfX, -innerHalfY, -halfThickness);
    glEnd();

    // RIGHT BAR (X+)
    glBegin(GL_QUADS);
    // Front face
    glVertex3f(innerHalfX, -innerHalfY, halfThickness);
    glVertex3f(outerHalfX, -innerHalfY, halfThickness);
    glVertex3f(outerHalfX, innerHalfY, halfThickness);
    glVertex3f(innerHalfX, innerHalfY, halfThickness);

    // Back face
    glVertex3f(innerHalfX, innerHalfY, -halfThickness);
    glVertex3f(outerHalfX, innerHalfY, -halfThickness);
    glVertex3f(outerHalfX, -innerHalfY, -halfThickness);
    glVertex3f(innerHalfX, -innerHalfY, -halfThickness);

    // Top face
    glVertex3f(innerHalfX, innerHalfY, -halfThickness);
    glVertex3f(outerHalfX, innerHalfY, -halfThickness);
    glVertex3f(outerHalfX, innerHalfY, halfThickness);
    glVertex3f(innerHalfX, innerHalfY, halfThickness);

    // Bottom face
    glVertex3f(innerHalfX, -innerHalfY, halfThickness);
    glVertex3f(outerHalfX, -innerHalfY, halfThickness);
    glVertex3f(outerHalfX, -innerHalfY, -halfThickness);
    glVertex3f(innerHalfX, -innerHalfY, -halfThickness);

    // Left face (inner edge)
    glVertex3f(innerHalfX, -innerHalfY, halfThickness);
    glVertex3f(innerHalfX, -innerHalfY, -halfThickness);
    glVertex3f(innerHalfX, innerHalfY, -halfThickness);
    glVertex3f(innerHalfX, innerHalfY, halfThickness);

    // Right face
    glVertex3f(outerHalfX, -innerHalfY, halfThickness);
    glVertex3f(outerHalfX, innerHalfY, halfThickness);
    glVertex3f(outerHalfX, innerHalfY, -halfThickness);
    glVertex3f(outerHalfX, -innerHalfY, -halfThickness);
    glEnd();

    // Restore matrix state
    glPopMatrix();

    glDisable(GL_BLEND);
#endif
}

}   // namespace tkd
