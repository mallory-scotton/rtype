///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <AC_NoteArrow.hpp>
#include <BP_Note.hpp>
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
AC_NoteArrow::AC_NoteArrow(const FString& name, EType type)
    : UActorComponent(name)
    , m_type(type)
    , m_glowIntensity(1.0f)
{}

///////////////////////////////////////////////////////////////////////////////
void AC_NoteArrow::Render(IRenderer&) const
{
    if (IsHiddenInGame() || !IsActive()) { return; }

#if TKD_ENGINE_CLIENT
    glPushMatrix();

    // Apply the note's transform
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

    // Enable blending for glow effects
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDepthMask(GL_FALSE);
    glDisable(GL_LIGHTING);

    if (m_type == EType::Dot) {}
    else
    {
        // Position arrow on the front face of the cube (top position, facing
        // down)
        glTranslatef(0.0f, 0.05f, 0.30f);   // Slightly in front of cube face
        glColor3f(1.f, 1.f, 1.f);

        float w = 0.3f;
        float h1 = 0.07f;
        float h2 = 0.05f;

        glBegin(GL_TRIANGLE_STRIP);

        glVertex2f(-w / 2, h1 + h2);   // D
        glVertex2f(w / 2, h1 + h2);    // E
        glVertex2f(-w / 2, h1);        // B
        glVertex2f(w / 2, h1);         // C
        glVertex2f(0.0f, 0.0f);        // A

        glEnd();
    }

    // Restore OpenGL state
    glDepthMask(GL_TRUE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);

    glPopMatrix();
#endif
}

///////////////////////////////////////////////////////////////////////////////
AC_NoteArrow::EType AC_NoteArrow::GetType(void) const { return m_type; }

///////////////////////////////////////////////////////////////////////////////
void AC_NoteArrow::SetType(EType type) { m_type = type; }

///////////////////////////////////////////////////////////////////////////////
float AC_NoteArrow::GetGlowIntensity(void) const { return m_glowIntensity; }

///////////////////////////////////////////////////////////////////////////////
void AC_NoteArrow::SetGlowIntensity(float intensity)
{
    m_glowIntensity = Math<float>::Clamp(intensity, 0.0f, 1.0f);
}

}   // namespace tkd
