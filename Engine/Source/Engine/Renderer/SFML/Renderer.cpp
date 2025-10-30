///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Renderer/SFML/Renderer.hpp>
#include <Engine/Config.hpp>
#include <Engine/Renderer/SFML/Utils.hpp>
#include <Engine/Renderer/SFML/Window.hpp>
#if TKD_ENGINE_CLIENT
    #include <GL/glu.h>
    #include <SFML/OpenGL.hpp>
#endif

//?TEMP
#include <Engine/Assets/URessource.hpp>
#include <Engine/Renderer/Primitives.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::SFML
///////////////////////////////////////////////////////////////////////////////
namespace tkd::SFML
{

#if TKD_ENGINE_CLIENT

///////////////////////////////////////////////////////////////////////////////
Renderer::Renderer(IWindow* window)
    : m_window(reinterpret_cast<sf::RenderWindow*>(window->GetNativeHandle()))
    , m_currentTarget(m_window)
    , m_currentView(/*FView::GetDefaultView()*/)
    , m_camera(60.0f, 4.0f / 3.0f, 0.1f, 1000.0f)
{
    // Set the camera to set the initial OpenGL states
    SetCamera(m_camera);
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::Clear(const FColor& color)
{
    m_currentTarget->clear(Utils::Convert(color));
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::SetView(const FView& view)
{
    m_currentView = view;
    m_currentTarget->setView(Utils::Convert(view));
}

///////////////////////////////////////////////////////////////////////////////
const FView& Renderer::GetView(void) const { return m_currentView; }

///////////////////////////////////////////////////////////////////////////////
FView Renderer::GetDefaultView(void) const
{
    sf::View sfmlView = m_currentTarget->getDefaultView();
    return FView(FRectangle(
        sfmlView.getCenter().x - sfmlView.getSize().x / 2.0f,
        sfmlView.getCenter().y - sfmlView.getSize().y / 2.0f,
        sfmlView.getSize().x,
        sfmlView.getSize().y
    ));
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::ApplyCameraView(void)
{
    // Apply camera view
    FVector3 center = m_camera.position + m_camera.front;
    gluLookAt(
        m_camera.position.x,
        m_camera.position.y,
        m_camera.position.z,
        center.x,
        center.y,
        center.z,
        m_camera.up.x,
        m_camera.up.y,
        m_camera.up.z
    );
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::Draw(
    const FVertex2D* vertices,
    UInt32 vertexCount,
    EPrimitiveType type,
    const FRenderStates& states
)
{
    if (vertices == nullptr || vertexCount == 0) { return; }

    // Save the current OpenGL state
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushMatrix();

    // Get the current window size and create a proper 2D view
    sf::Vector2u size = m_currentTarget->getSize();
    sf::View view2D(sf::FloatRect(
        0.f, 0.f, static_cast<float>(size.x), static_cast<float>(size.y)
    ));
    m_currentTarget->setView(view2D);

    // Reset to 2D mode for SFML rendering
    m_window->resetGLStates();
    m_window->pushGLStates();

    std::vector<sf::Vertex> sfmlVertices;
    sfmlVertices.reserve(vertexCount);
    for (UInt32 i = 0; i < vertexCount; ++i)
    {
        sfmlVertices.push_back(Utils::Convert(vertices[i]));
    }

    m_currentTarget->draw(
        sfmlVertices.data(),
        vertexCount,
        Utils::Convert(type),
        Utils::Convert(states)
    );

    // Restore OpenGL state for 3D rendering
    m_window->popGLStates();
    glPopMatrix();
    glPopAttrib();

    // Reapply 3D camera after 2D rendering
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Update aspect ratio based on current size
    m_camera.aspectRatio =
        static_cast<float>(size.x) / static_cast<float>(size.y);

    gluPerspective(
        m_camera.fov,
        m_camera.aspectRatio,
        m_camera.nearPlane,
        m_camera.farPlane
    );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    ApplyCameraView();
}

///////////////////////////////////////////////////////////////////////////////
inline void CalculateAndSetNormal(
    const FVertex& v0, const FVertex& v1, const FVertex& v2
)
{
    // Calculate edge vectors
    float edge1X = v1.position.x - v0.position.x;
    float edge1Y = v1.position.y - v0.position.y;
    float edge1Z = v1.position.z - v0.position.z;

    float edge2X = v2.position.x - v0.position.x;
    float edge2Y = v2.position.y - v0.position.y;
    float edge2Z = v2.position.z - v0.position.z;

    // Cross product
    float normalX = edge1Y * edge2Z - edge1Z * edge2Y;
    float normalY = edge1Z * edge2X - edge1X * edge2Z;
    float normalZ = edge1X * edge2Y - edge1Y * edge2X;

    // Normalize
    float length =
        sqrtf(normalX * normalX + normalY * normalY + normalZ * normalZ);
    if (length > 0.0001f)
    {
        normalX /= length;
        normalY /= length;
        normalZ /= length;
    }

    glNormal3f(normalX, normalY, normalZ);
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::Draw(
    const FVertex* vertices,
    SizeT count,
    EPrimitiveType type,
    FTransform transform
)
{
    glPushMatrix();

    FVector3 position = transform.GetPosition();
    FRotator rotation = transform.GetRotation();
    FVector3 scale = transform.GetScale();

    glTranslatef(position.x, position.y, position.z);
    glRotatef(rotation.GetRoll(), 1.0f, 0.0f, 0.0f);
    glRotatef(rotation.GetPitch(), 0.0f, 1.0f, 0.0f);
    glRotatef(rotation.GetYaw(), 0.0f, 0.0f, 1.0f);
    glScalef(scale.x, scale.y, scale.z);

    // Set material color once for the entire object
    if (count > 0)
    {
        glColor4f(
            vertices[0].color.r,
            vertices[0].color.g,
            vertices[0].color.b,
            vertices[0].color.a
        );

        if (vertices[0].color.a < 1.0f)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
    }

    switch (type)
    {
    case EPrimitiveType::Points       : glBegin(GL_POINTS); break;
    case EPrimitiveType::Lines        : glBegin(GL_LINES); break;
    case EPrimitiveType::LineStrip    : glBegin(GL_LINE_STRIP); break;
    case EPrimitiveType::Triangles    : glBegin(GL_TRIANGLES); break;
    case EPrimitiveType::TriangleStrip: glBegin(GL_TRIANGLE_STRIP); break;
    case EPrimitiveType::TriangleFan  : glBegin(GL_TRIANGLE_FAN); break;
    case EPrimitiveType::Quads        : glBegin(GL_QUADS); break;
    case EPrimitiveType::QuadStrip    : glBegin(GL_QUAD_STRIP); break;
    }

    switch (type)
    {
    case EPrimitiveType::Triangles:
        // Each set of 3 vertices forms a triangle
        for (SizeT i = 0; i + 2 < count; i += 3)
        {
            const FVertex& v0 = vertices[i];
            const FVertex& v1 = vertices[i + 1];
            const FVertex& v2 = vertices[i + 2];

            // Calculate and set normal for the triangle
            CalculateAndSetNormal(v0, v1, v2);

            // Render all three vertices with the same normal
            glTexCoord2f(v0.uv.x, v0.uv.y);
            glVertex3f(v0.position.x, v0.position.y, v0.position.z);

            glTexCoord2f(v1.uv.x, v1.uv.y);
            glVertex3f(v1.position.x, v1.position.y, v1.position.z);

            glTexCoord2f(v2.uv.x, v2.uv.y);
            glVertex3f(v2.position.x, v2.position.y, v2.position.z);
        }
        break;

    case EPrimitiveType::TriangleStrip:
        // Triangle strip: each new vertex forms a triangle with previous two
        for (SizeT i = 0; i + 2 < count; i++)
        {
            const FVertex& v0 = vertices[i];
            const FVertex& v1 = vertices[i + 1];
            const FVertex& v2 = vertices[i + 2];

            // Calculate normal (flip for odd triangles to maintain consistent
            // winding)
            if (i % 2 == 0) { CalculateAndSetNormal(v0, v1, v2); }
            else { CalculateAndSetNormal(v0, v2, v1); }

            // Only render the current vertex (strip already has previous
            // vertices)
            const FVertex& vertex = vertices[i];
            glTexCoord2f(vertex.uv.x, vertex.uv.y);
            glVertex3f(
                vertex.position.x, vertex.position.y, vertex.position.z
            );
        }
        // Render last two vertices
        for (SizeT i = (count >= 2 ? count - 2 : 0); i < count; i++)
        {
            const FVertex& vertex = vertices[i];
            glTexCoord2f(vertex.uv.x, vertex.uv.y);
            glVertex3f(
                vertex.position.x, vertex.position.y, vertex.position.z
            );
        }
        break;

    case EPrimitiveType::TriangleFan:
        // Triangle fan: all triangles share the first vertex
        if (count >= 3)
        {
            const FVertex& center = vertices[0];

            for (SizeT i = 1; i + 1 < count; i++)
            {
                const FVertex& v1 = vertices[i];
                const FVertex& v2 = vertices[i + 1];

                // Calculate normal
                CalculateAndSetNormal(center, v1, v2);

                // Render center vertex for each triangle
                glTexCoord2f(center.uv.x, center.uv.y);
                glVertex3f(
                    center.position.x, center.position.y, center.position.z
                );

                glTexCoord2f(v1.uv.x, v1.uv.y);
                glVertex3f(v1.position.x, v1.position.y, v1.position.z);

                glTexCoord2f(v2.uv.x, v2.uv.y);
                glVertex3f(v2.position.x, v2.position.y, v2.position.z);
            }
        }
        break;

    case EPrimitiveType::Quads:
        // Each set of 4 vertices forms a quad
        for (SizeT i = 0; i + 4 <= count; i += 4)
        {
            const FVertex& v0 = vertices[i];
            const FVertex& v1 = vertices[i + 1];
            const FVertex& v2 = vertices[i + 2];
            const FVertex& v3 = vertices[i + 3];

            // Calculate normal from first three vertices
            CalculateAndSetNormal(v0, v1, v2);

            // Render all four vertices with the same normal
            glTexCoord2f(v0.uv.x, v0.uv.y);
            glVertex3f(v0.position.x, v0.position.y, v0.position.z);

            glTexCoord2f(v1.uv.x, v1.uv.y);
            glVertex3f(v1.position.x, v1.position.y, v1.position.z);

            glTexCoord2f(v2.uv.x, v2.uv.y);
            glVertex3f(v2.position.x, v2.position.y, v2.position.z);

            glTexCoord2f(v3.uv.x, v3.uv.y);
            glVertex3f(v3.position.x, v3.position.y, v3.position.z);
        }
        break;

    case EPrimitiveType::QuadStrip:
        // Quad strip: each pair of new vertices forms a quad with previous
        // pair
        for (SizeT i = 0; i + 3 < count; i += 2)
        {
            const FVertex& v0 = vertices[i];
            const FVertex& v1 = vertices[i + 1];
            const FVertex& v2 = vertices[i + 2];

            // Calculate normal
            CalculateAndSetNormal(v0, v2, v1);

            // Render current pair
            glTexCoord2f(v0.uv.x, v0.uv.y);
            glVertex3f(v0.position.x, v0.position.y, v0.position.z);

            glTexCoord2f(v1.uv.x, v1.uv.y);
            glVertex3f(v1.position.x, v1.position.y, v1.position.z);
        }
        // Render last pair if exists
        for (SizeT i = (count >= 2 ? count - 2 : 0); i < count; i++)
        {
            const FVertex& vertex = vertices[i];
            glTexCoord2f(vertex.uv.x, vertex.uv.y);
            glVertex3f(
                vertex.position.x, vertex.position.y, vertex.position.z
            );
        }
        break;

    default:
        // For Points, Lines, LineStrip: no normals needed, render as-is
        for (SizeT i = 0; i < count; i++)
        {
            const FVertex& vertex = vertices[i];

            glTexCoord2f(vertex.uv.x, vertex.uv.y);
            glVertex3f(
                vertex.position.x, vertex.position.y, vertex.position.z
            );
        }
        break;
    }

    glEnd();
    glPopMatrix();

    if (count > 0 && vertices[0].color.a < 1.0f) { glDisable(GL_BLEND); }
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::Draw(
    const std::vector<FVertex>& vertices,
    EPrimitiveType type,
    FTransform transform
)
{
    this->Draw(vertices.data(), vertices.size(), type, transform);
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::SetRenderTarget(IRenderTarget* target)
{
    if (target == nullptr) { m_currentTarget = m_window; }
    else
    {
        m_currentTarget =
            reinterpret_cast<sf::RenderTarget*>(target->GetNativeHandle());
    }
    // Reset view to default when changing target
    m_currentView = FView();
    m_currentTarget->setView(m_currentTarget->getDefaultView());
}

///////////////////////////////////////////////////////////////////////////////
FVector2u Renderer::GetRenderTargetSize(void) const
{
    sf::Vector2u size = m_currentTarget->getSize();
    return FVector2u(size.x, size.y);
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::PushScissorTest(const FRectangle& rect)
{
    TKD_UNUSED(rect);
    // TODO: Implement scissor test using SFML views or OpenGL directly
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::PopScissorTest(void)
{
    // TODO: Implement scissor test pop
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::BeginFrame(void)
{
    // Get current window size and update aspect ratio
    sf::Vector2u size = m_currentTarget->getSize();
    m_camera.aspectRatio =
        static_cast<float>(size.x) / static_cast<float>(size.y);

    // Apply current view
    m_currentTarget->setView(Utils::Convert(m_currentView));

    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set up 3D projection with updated aspect ratio
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Clamp near and far planes to valid ranges
    if (m_camera.nearPlane < 0.1f)
    {
        m_camera.nearPlane = 0.1f;
    }
    else if (m_camera.nearPlane >= m_camera.farPlane)
    {
        m_camera.nearPlane = m_camera.farPlane - 0.1f;
    }
    if (m_camera.farPlane <= m_camera.nearPlane)
    {
        m_camera.farPlane = m_camera.nearPlane + 1000.0f;
    }

    gluPerspective(
        m_camera.fov,
        m_camera.aspectRatio,
        m_camera.nearPlane,
        m_camera.farPlane
    );

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Apply camera view
    ApplyCameraView();
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::EndFrame(void)
{
    if (m_currentTarget == m_window) { m_window->display(); }
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::SetCamera(const FCamera& camera)
{
    m_camera = camera;

    // Apply OpenGL GLU perspective
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    sf::Vector2u size = m_currentTarget->getSize();
    m_camera.aspectRatio =
        static_cast<float>(size.x) / static_cast<float>(size.y);

    gluPerspective(
        camera.fov, m_camera.aspectRatio, camera.nearPlane, camera.farPlane
    );

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

///////////////////////////////////////////////////////////////////////////////
const FCamera& Renderer::GetCamera(void) const { return m_camera; }

///////////////////////////////////////////////////////////////////////////////
FCamera& Renderer::GetCamera(void) { return m_camera; }

///////////////////////////////////////////////////////////////////////////////
void Renderer::PushAttributes(UInt32 flags) { glPushAttrib(flags); }

///////////////////////////////////////////////////////////////////////////////
void Renderer::PopAttributes(void) { glPopAttrib(); }

///////////////////////////////////////////////////////////////////////////////
void Renderer::PushMatrix(void) { glPushMatrix(); }

///////////////////////////////////////////////////////////////////////////////
void Renderer::PopMatrix(void) { glPopMatrix(); }

///////////////////////////////////////////////////////////////////////////////
void Renderer::LoadIdentity(void) { glLoadIdentity(); }

///////////////////////////////////////////////////////////////////////////////
void Renderer::Translate(Float32 x, Float32 y, Float32 z)
{
    glTranslatef(x, y, z);
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::Rotate(Float32 angle, Float32 x, Float32 y, Float32 z)
{
    glRotatef(angle, x, y, z);
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::Scale(Float32 x, Float32 y, Float32 z) { glScalef(x, y, z); }

///////////////////////////////////////////////////////////////////////////////
void Renderer::LookAt(
    const FVector3& eye, const FVector3& center, const FVector3& up
)
{
    gluLookAt(
        eye.x, eye.y, eye.z, center.x, center.y, center.z, up.x, up.y, up.z
    );
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::SetDepthMask(bool enable)
{
    glDepthMask(enable ? GL_TRUE : GL_FALSE);
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::SetDepthTest(bool enable)
{
    if (enable) { glEnable(GL_DEPTH_TEST); }
    else { glDisable(GL_DEPTH_TEST); }
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::SetBlend(bool enable)
{
    if (enable) { glEnable(GL_BLEND); }
    else { glDisable(GL_BLEND); }
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::SetBlendFunc(EBlendFactor sfactor, EBlendFactor dfactor)
{
    glBlendFunc(static_cast<GLenum>(sfactor), static_cast<GLenum>(dfactor));
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::SetCullFace(bool enable)
{
    if (enable) { glEnable(GL_CULL_FACE); }
    else { glDisable(GL_CULL_FACE); }
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::SetCullFaceMode(ECullFaceMode mode)
{
    glCullFace(static_cast<GLenum>(mode));
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::SetFrontFace(EFrontFace mode)
{
    glFrontFace(static_cast<GLenum>(mode));
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::SetLighting(bool enable)
{
    if (enable) { glEnable(GL_LIGHTING); }
    else { glDisable(GL_LIGHTING); }
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::SetTexture2D(bool enable)
{
    if (enable) { glEnable(GL_TEXTURE_2D); }
    else { glDisable(GL_TEXTURE_2D); }
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::SetPolygonMode(ECullFaceMode face, EPolygonMode mode)
{
    glPolygonMode(static_cast<GLenum>(face), static_cast<GLenum>(mode));
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::SetLineWidth(Float32 width) { glLineWidth(width); }

///////////////////////////////////////////////////////////////////////////////
void Renderer::SetPointSize(Float32 size) { glPointSize(size); }

///////////////////////////////////////////////////////////////////////////////
void Renderer::SetViewport(Int32 x, Int32 y, Int32 width, Int32 height)
{
    glViewport(x, y, width, height);
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::SetMatrixMode(EMatrixMode mode)
{
    glMatrixMode(static_cast<GLenum>(mode));
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::SetClearColor(const FColor& color)
{
    glClearColor(color.r, color.g, color.b, color.a);
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::SetDepthFunc(EDepthFunc func)
{
    glDepthFunc(static_cast<GLenum>(func));
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::SetColorMaterial(bool enable)
{
    if (enable) { glEnable(GL_COLOR_MATERIAL); }
    else { glDisable(GL_COLOR_MATERIAL); }
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::SetShadeModel(EShadeModel mode)
{
    glShadeModel(static_cast<GLenum>(mode));
}

#endif

}   // namespace tkd::SFML
