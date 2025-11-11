///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Renderer/SFML/Renderer.hpp>
#include <Engine/Config.hpp>
#include <Engine/Renderer/SFML/Utils.hpp>
#include <Engine/Renderer/SFML/Window.hpp>
#if TKD_ENGINE_CLIENT
    #include <GL/glew.h>
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
    // Check if VR is initialized
    if (VR::FVRSystem::GetInstance().IsInitialized())
    {
        m_vr.enabled = true;
        m_vr.specs = VR::FVRSystem::GetInstance().GetSpecs();

        FLogger::SetNamespace("OpenGL");
        FLogger::Info("VR initialized with device: {}", m_vr.specs.deviceName);
        FLogger::Info(
            "VR Recommended Render Target Size: {}x{}",
            m_vr.specs.recommendedWidth,
            m_vr.specs.recommendedHeight
        );

        // Create framebuffers for each eye
        if (!m_vr.leftEye.Create(
                m_vr.specs.recommendedWidth, m_vr.specs.recommendedHeight, 1
            ) ||
            !m_vr.rightEye.Create(
                m_vr.specs.recommendedWidth, m_vr.specs.recommendedHeight, 1
            ))
        {
            m_vr.enabled = false;
            // Set the camera to set the initial OpenGL states
            SetCamera(m_camera);
            FLogger::SetNamespace("OpenGL");
            FLogger::Warn(
                "Failed to create VR framebuffers - disabling VR support"
            );
        }
    }
    else
    {
        m_vr.enabled = false;
        // Set the camera to set the initial OpenGL states
        SetCamera(m_camera);
    }
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
    if (!m_vr.enabled)
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
    sf::View view2D(
        sf::FloatRect(
            0.f, 0.f, static_cast<float>(size.x), static_cast<float>(size.y)
        )
    );
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

    if (!m_vr.enabled)
    {
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

    // **Save current blend state**
    GLboolean wasBlendEnabled = glIsEnabled(GL_BLEND);

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

    if (wasBlendEnabled) { glEnable(GL_BLEND); }
    else { glDisable(GL_BLEND); }
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
void Renderer::UpdateCameraFromHMD(const VR::FPose& hmdPose)
{
    if (!hmdPose.isValid) { return; }

    // Convert quaternion rotation to camera direction vectors
    FMatrix4x4 rotMatrix = hmdPose.rotation.ToMatrix4x4();

    // Extract direction vectors from rotation matrix
    // Forward vector (negative Z in OpenGL convention)
    m_camera.front =
        FVector3(-rotMatrix(0, 2), -rotMatrix(1, 2), -rotMatrix(2, 2));

    // Right vector (positive X)
    m_camera.right =
        FVector3(rotMatrix(0, 0), rotMatrix(1, 0), rotMatrix(2, 0));

    // Up vector (positive Y)
    m_camera.up = FVector3(rotMatrix(0, 1), rotMatrix(1, 1), rotMatrix(2, 1));

    // Normalize vectors to be safe
    float frontLen = std::sqrt(
        m_camera.front.x * m_camera.front.x +
        m_camera.front.y * m_camera.front.y +
        m_camera.front.z * m_camera.front.z
    );
    if (frontLen > 0.0001f)
    {
        m_camera.front.x /= frontLen;
        m_camera.front.y /= frontLen;
        m_camera.front.z /= frontLen;
    }

    float upLen = std::sqrt(
        m_camera.up.x * m_camera.up.x + m_camera.up.y * m_camera.up.y +
        m_camera.up.z * m_camera.up.z
    );
    if (upLen > 0.0001f)
    {
        m_camera.up.x /= upLen;
        m_camera.up.y /= upLen;
        m_camera.up.z /= upLen;
    }

    float rightLen = std::sqrt(
        m_camera.right.x * m_camera.right.x +
        m_camera.right.y * m_camera.right.y +
        m_camera.right.z * m_camera.right.z
    );
    if (rightLen > 0.0001f)
    {
        m_camera.right.x /= rightLen;
        m_camera.right.y /= rightLen;
        m_camera.right.z /= rightLen;
    }
}

///////////////////////////////////////////////////////////////////////////////
bool Renderer::IsUsingVirtualReality(void) const { return m_vr.enabled; }

///////////////////////////////////////////////////////////////////////////////
void Renderer::SetupVirtualRealityRightEye(void)
{
    VR::FVRSystem& vrSystem = VR::FVRSystem::GetInstance();
    VR::FPose hmdPose = vrSystem.GetHMDPose();

    // Bind right eye framebuffer
    m_vr.rightEye.Bind();
    glViewport(
        0, 0, m_vr.specs.recommendedWidth, m_vr.specs.recommendedHeight
    );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.15f, 0.15f, 0.18f, 1.0f);

    // Ensure OpenGL states are set correctly
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_TEXTURE_2D);

    if (hmdPose.isValid)
    {
        // Build HMD pose matrix from rotation and position
        FMatrix4x4 headPose = hmdPose.rotation.ToMatrix4x4();
        headPose(0, 3) = hmdPose.position.x;
        headPose(1, 3) = hmdPose.position.y;
        headPose(2, 3) = hmdPose.position.z;

        // Create player rotation matrix (rotation around Y-axis only, no
        // translation yet)
        float yawRad = /*playerYaw*/ 0.f * (M_PI / 180.0f);
        float cosYaw = std::cos(yawRad);
        float sinYaw = std::sin(yawRad);

        FMatrix4x4 playerRotation;
        playerRotation(0, 0) = cosYaw;
        playerRotation(0, 1) = 0.0f;
        playerRotation(0, 2) = sinYaw;
        playerRotation(0, 3) = 0.0f;
        playerRotation(1, 0) = 0.0f;
        playerRotation(1, 1) = 1.0f;
        playerRotation(1, 2) = 0.0f;
        playerRotation(1, 3) = 0.0f;
        playerRotation(2, 0) = -sinYaw;
        playerRotation(2, 1) = 0.0f;
        playerRotation(2, 2) = cosYaw;
        playerRotation(2, 3) = 0.0f;
        playerRotation(3, 0) = 0.0f;
        playerRotation(3, 1) = 0.0f;
        playerRotation(3, 2) = 0.0f;
        playerRotation(3, 3) = 1.0f;

        // Apply player rotation to head pose: playerRotation * headPose
        FMatrix4x4 rotatedHeadPose;
        for (SizeT i = 0; i < 4; i++)
        {
            for (SizeT j = 0; j < 4; j++)
            {
                rotatedHeadPose(i, j) = 0;
                for (SizeT k = 0; k < 4; k++)
                {
                    rotatedHeadPose(i, j) +=
                        playerRotation(i, k) * headPose(k, j);
                }
            }
        }

        // Now add player position translation
        rotatedHeadPose(0, 3) += m_camera.position.x;
        rotatedHeadPose(1, 3) += m_camera.position.y;
        rotatedHeadPose(2, 3) += m_camera.position.z;

        // Setup projection matrix - DON'T transpose, already done in
        // GetProjectionMatrix
        FMatrix4x4 projectionRight =
            vrSystem.GetProjectionMatrix(VR::EEye::Right, 0.1f, 100.0f);
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(&projectionRight.data[0][0]);

        // Setup view matrix
        FMatrix4x4 eyeToHeadRight =
            vrSystem.GetEyeToHeadTransform(VR::EEye::Right);

        // Matrix multiply: rotatedHeadPose * eyeToHead
        FMatrix4x4 viewMatrix;
        for (SizeT i = 0; i < 4; i++)
        {
            for (SizeT j = 0; j < 4; j++)
            {
                viewMatrix(i, j) = 0;
                for (SizeT k = 0; k < 4; k++)
                {
                    viewMatrix(i, j) +=
                        rotatedHeadPose(i, k) * eyeToHeadRight(k, j);
                }
            }
        }

        // Simple inverse for viewing (transpose rotation, negate translation)
        FMatrix4x4 view;
        for (SizeT i = 0; i < 3; i++)
        {
            for (SizeT j = 0; j < 3; j++)
            {
                view(i, j) = viewMatrix(j, i);   // Transpose rotation part
            }
        }
        view(0, 3) =
            -(viewMatrix(0, 3) * view(0, 0) + viewMatrix(1, 3) * view(0, 1) +
              viewMatrix(2, 3) * view(0, 2));
        view(1, 3) =
            -(viewMatrix(0, 3) * view(1, 0) + viewMatrix(1, 3) * view(1, 1) +
              viewMatrix(2, 3) * view(1, 2));
        view(2, 3) =
            -(viewMatrix(0, 3) * view(2, 0) + viewMatrix(1, 3) * view(2, 1) +
              viewMatrix(2, 3) * view(2, 2));
        view(3, 0) = 0.0f;
        view(3, 1) = 0.0f;
        view(3, 2) = 0.0f;
        view(3, 3) = 1.0f;

        glMatrixMode(GL_MODELVIEW);
        FMatrix4x4 viewTransposed = view.Transpose();
        glLoadMatrixf(&viewTransposed.data[0][0]);

        //? TEMP: Draw simple grid at HMD position for debugging
        glPushMatrix();
        // Draw a simple test cube right in front of camera at origin
        glTranslatef(0, 0, -2);   // 2 meters in front

        // Draw colored axes to verify rendering
        glBegin(GL_LINES);
        // X axis - Red
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(0, 0, 0);
        glVertex3f(1, 0, 0);
        // Y axis - Green
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 1, 0);
        // Z axis - Blue
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, 1);
        glEnd();

        glPopMatrix();
        //? ENDTEMP
    }
    else
    {
        FLogger::SetNamespace("OpenGL");
        FLogger::Warn("HMD pose is not valid - cannot render VR view");
    }
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::ResolveVirtualRealityRightEye(void)
{
    // Unbind framebuffer
    m_vr.rightEye.Unbind();
    // Resolve MSAA if needed
    m_vr.rightEye.Resolve();
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::SetupVirtualRealityLeftEye(void)
{
    VR::FVRSystem& vrSystem = VR::FVRSystem::GetInstance();
    VR::FPose hmdPose = vrSystem.GetHMDPose();

    m_vr.leftEye.Bind();
    glViewport(
        0, 0, m_vr.specs.recommendedWidth, m_vr.specs.recommendedHeight
    );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.15f, 0.15f, 0.18f, 1.0f);

    // Ensure OpenGL states are set correctly
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_TEXTURE_2D);

    if (hmdPose.isValid)
    {
        FMatrix4x4 headPose = hmdPose.rotation.ToMatrix4x4();
        headPose(0, 3) = hmdPose.position.x;
        headPose(1, 3) = hmdPose.position.y;
        headPose(2, 3) = hmdPose.position.z;

        // Create player rotation matrix (rotation around Y-axis only, no
        // translation yet)
        float yawRad = /*playerYaw*/ 0.f * (M_PI / 180.0f);
        float cosYaw = std::cos(yawRad);
        float sinYaw = std::sin(yawRad);

        FMatrix4x4 playerRotation;
        playerRotation(0, 0) = cosYaw;
        playerRotation(0, 1) = 0.0f;
        playerRotation(0, 2) = sinYaw;
        playerRotation(0, 3) = 0.0f;
        playerRotation(1, 0) = 0.0f;
        playerRotation(1, 1) = 1.0f;
        playerRotation(1, 2) = 0.0f;
        playerRotation(1, 3) = 0.0f;
        playerRotation(2, 0) = -sinYaw;
        playerRotation(2, 1) = 0.0f;
        playerRotation(2, 2) = cosYaw;
        playerRotation(2, 3) = 0.0f;
        playerRotation(3, 0) = 0.0f;
        playerRotation(3, 1) = 0.0f;
        playerRotation(3, 2) = 0.0f;
        playerRotation(3, 3) = 1.0f;

        // Apply player rotation to head pose: playerRotation * headPose
        FMatrix4x4 rotatedHeadPose;
        for (SizeT i = 0; i < 4; i++)
        {
            for (SizeT j = 0; j < 4; j++)
            {
                rotatedHeadPose(i, j) = 0;
                for (SizeT k = 0; k < 4; k++)
                {
                    rotatedHeadPose(i, j) +=
                        playerRotation(i, k) * headPose(k, j);
                }
            }
        }

        // Now add player position translation
        rotatedHeadPose(0, 3) += m_camera.position.x;
        rotatedHeadPose(1, 3) += m_camera.position.y;
        rotatedHeadPose(2, 3) += m_camera.position.z;

        // Setup projection matrix - DON'T transpose, already done in
        // GetProjectionMatrix
        FMatrix4x4 projectionLeft =
            vrSystem.GetProjectionMatrix(VR::EEye::Left, 0.1f, 100.0f);
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(&projectionLeft.data[0][0]);

        // Setup view matrix
        FMatrix4x4 eyeToHeadLeft =
            vrSystem.GetEyeToHeadTransform(VR::EEye::Left);

        // Matrix multiply: rotatedHeadPose * eyeToHead
        FMatrix4x4 viewMatrix;
        for (SizeT i = 0; i < 4; i++)
        {
            for (SizeT j = 0; j < 4; j++)
            {
                viewMatrix(i, j) = 0;
                for (SizeT k = 0; k < 4; k++)
                {
                    viewMatrix(i, j) +=
                        rotatedHeadPose(i, k) * eyeToHeadLeft(k, j);
                }
            }
        }

        // Simple inverse for viewing (transpose rotation, negate translation)
        FMatrix4x4 view;
        for (SizeT i = 0; i < 3; i++)
        {
            for (SizeT j = 0; j < 3; j++)
            {
                view(i, j) = viewMatrix(j, i);   // Transpose rotation part
            }
        }
        view(0, 3) =
            -(viewMatrix(0, 3) * view(0, 0) + viewMatrix(1, 3) * view(0, 1) +
              viewMatrix(2, 3) * view(0, 2));
        view(1, 3) =
            -(viewMatrix(0, 3) * view(1, 0) + viewMatrix(1, 3) * view(1, 1) +
              viewMatrix(2, 3) * view(1, 2));
        view(2, 3) =
            -(viewMatrix(0, 3) * view(2, 0) + viewMatrix(1, 3) * view(2, 1) +
              viewMatrix(2, 3) * view(2, 2));
        view(3, 0) = 0.0f;
        view(3, 1) = 0.0f;
        view(3, 2) = 0.0f;
        view(3, 3) = 1.0f;

        glMatrixMode(GL_MODELVIEW);
        FMatrix4x4 viewTransposed = view.Transpose();
        glLoadMatrixf(&viewTransposed.data[0][0]);

        //? TEMP: Draw simple grid at HMD position for debugging
        glPushMatrix();
        // Draw a simple test cube right in front of camera at origin
        glTranslatef(0, 0, -2);   // 2 meters in front

        // Draw colored axes to verify rendering
        glBegin(GL_LINES);
        // X axis - Red
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(0, 0, 0);
        glVertex3f(1, 0, 0);
        // Y axis - Green
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 1, 0);
        // Z axis - Blue
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, 1);
        glEnd();

        glPopMatrix();
        //? ENDTEMP
    }
    else
    {
        FLogger::SetNamespace("OpenGL");
        FLogger::Warn("HMD pose is not valid - cannot render VR view");
    }
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::ResolveVirtualRealityLeftEye(void)
{
    // Unbind framebuffer
    m_vr.leftEye.Unbind();
    // Resolve MSAA if needed
    m_vr.leftEye.Resolve();
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::BeginFrame(void)
{
    // Call the VR BeginFrame if VR is enabled
    if (m_vr.enabled)
    {
        VR::FVRSystem::GetInstance().BeginFrame();

        // Update camera rotation from HMD pose
        VR::FPose hmdPose = VR::FVRSystem::GetInstance().GetHMDPose();
        if (hmdPose.isValid) { UpdateCameraFromHMD(hmdPose); }
    }
    else
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
        if (m_camera.nearPlane < 0.1f) { m_camera.nearPlane = 0.1f; }
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
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::EndFrame(void)
{
    // Special handling for VR rendering
    if (m_vr.enabled)
    {
        // Get VR system instance
        auto& vrSystem = VR::FVRSystem::GetInstance();

        // Ensure all rendering is complete before compositor reads textures
        glFinish();

        // Submit VR frames
        vrSystem.SubmitFrame(VR::EEye::Left, m_vr.leftEye.GetRenderTarget());
        vrSystem.SubmitFrame(VR::EEye::Right, m_vr.rightEye.GetRenderTarget());

        // Get the size of the window
        sf::Vector2u windowSize = m_window->getSize();

        // Save all OpenGL state
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);

        // Display preview in window (show what left eye sees)
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, windowSize.x, windowSize.y);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // **RESET TO KNOWN CLEAN STATE FOR 2D TEXTURE RENDERING**
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glDisable(GL_BLEND);
        glDisable(GL_CULL_FACE);
        glDisable(GL_ALPHA_TEST);
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_FOG);
        glDisable(GL_SCISSOR_TEST);
        glDepthMask(GL_FALSE);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        // Setup orthographic projection for displaying texture
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-1, 1, -1, 1, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Bind and display the left eye texture
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, m_vr.leftEye.GetRenderTarget().textureID);

        // Ensure proper texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Draw a full-screen quad with the texture
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0);
        glVertex2f(-1, -1);
        glTexCoord2f(1, 0);
        glVertex2f(1, -1);
        glTexCoord2f(1, 1);
        glVertex2f(1, 1);
        glTexCoord2f(0, 1);
        glVertex2f(-1, 1);
        glEnd();

        // Cleanup - unbind the texture
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);

        // **RESTORE ALL OPENGL STATE**
        glPopClientAttrib();
        glPopAttrib();

        // Display the window contents
        if (m_currentTarget == m_window) { m_window->display(); }

        //  End VR Frame
        vrSystem.EndFrame();
    }
    else
    {
        if (m_currentTarget == m_window) { m_window->display(); }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::SetCamera(const FCamera& camera)
{
    m_camera = camera;

    if (m_vr.enabled) { return; }

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
