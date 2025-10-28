///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Renderer/VR/FVRFrameBuffer.hpp>
#if TKD_ENGINE_CLIENT
    #include <GL/glew.h>
    #include <GL/glu.h>
#endif

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::VR
///////////////////////////////////////////////////////////////////////////////
namespace tkd::VR
{

///////////////////////////////////////////////////////////////////////////////
FVRFrameBuffer::FVRFrameBuffer(void)
    : m_width(0)
    , m_height(0)
    , m_framebuffer(0)
    , m_depthBuffer(0)
    , m_renderTexture(0)
    , m_resolveFramebuffer(0)
    , m_resolveTexture(0)
    , m_msaaSamples(0)
{}

///////////////////////////////////////////////////////////////////////////////
FVRFrameBuffer::FVRFrameBuffer(UInt32 width, UInt32 height, UInt32 msaaSamples)
    : m_width(0)
    , m_height(0)
    , m_framebuffer(0)
    , m_depthBuffer(0)
    , m_renderTexture(0)
    , m_resolveFramebuffer(0)
    , m_resolveTexture(0)
    , m_msaaSamples(0)
{
    Create(width, height, msaaSamples);
}

///////////////////////////////////////////////////////////////////////////////
FVRFrameBuffer::~FVRFrameBuffer() { Destroy(); }

///////////////////////////////////////////////////////////////////////////////
bool FVRFrameBuffer::Create(UInt32 width, UInt32 height, UInt32 msaaSamples)
{
#if TKD_ENGINE_CLIENT
    m_width = width;
    m_height = height;
    m_msaaSamples = msaaSamples;

    // Create multisample framebuffer
    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    // Depth buffer
    glGenRenderbuffers(1, &m_depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthBuffer);

    if (msaaSamples > 1)
    {
        glRenderbufferStorageMultisample(
            GL_RENDERBUFFER, msaaSamples, GL_DEPTH_COMPONENT24, width, height
        );
    }
    else
    {
        glRenderbufferStorage(
            GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height
        );
    }

    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffer
    );

    // Color texture
    glGenTextures(1, &m_renderTexture);
    glBindTexture(
        msaaSamples > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D,
        m_renderTexture
    );

    if (msaaSamples > 1)
    {
        glTexImage2DMultisample(
            GL_TEXTURE_2D_MULTISAMPLE,
            msaaSamples,
            GL_RGBA8,
            width,
            height,
            GL_TRUE
        );
        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D_MULTISAMPLE,
            m_renderTexture,
            0
        );
    }
    else
    {
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA8,
            width,
            height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            nullptr
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D,
            m_renderTexture,
            0
        );
    }

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        Destroy();
        return false;
    }

    // Create resolve framebuffer if using MSAA
    if (msaaSamples > 1)
    {
        glGenFramebuffers(1, &m_resolveFramebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_resolveFramebuffer);

        glGenTextures(1, &m_resolveTexture);
        glBindTexture(GL_TEXTURE_2D, m_resolveTexture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA8,
            width,
            height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            nullptr
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D,
            m_resolveTexture,
            0
        );

        status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            Destroy();
            return false;
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
#else
    TKD_UNUSED(width);
    TKD_UNUSED(height);
    TKD_UNUSED(msaaSamples);
#endif
    return true;
}

///////////////////////////////////////////////////////////////////////////////
void FVRFrameBuffer::Destroy(void)
{
#if TKD_ENGINE_CLIENT
    if (m_framebuffer) { glDeleteFramebuffers(1, &m_framebuffer); }
    if (m_depthBuffer) { glDeleteRenderbuffers(1, &m_depthBuffer); }
    if (m_renderTexture) { glDeleteTextures(1, &m_renderTexture); }
    if (m_resolveFramebuffer)
    {
        glDeleteFramebuffers(1, &m_resolveFramebuffer);
    }
    if (m_resolveTexture) { glDeleteTextures(1, &m_resolveTexture); }

    m_framebuffer = 0;
    m_depthBuffer = 0;
    m_renderTexture = 0;
    m_resolveFramebuffer = 0;
    m_resolveTexture = 0;
#endif
}

///////////////////////////////////////////////////////////////////////////////
void FVRFrameBuffer::Bind(void)
{
#if TKD_ENGINE_CLIENT
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glViewport(0, 0, m_width, m_height);
#endif
}

///////////////////////////////////////////////////////////////////////////////
void FVRFrameBuffer::Unbind(void)
{
#if TKD_ENGINE_CLIENT
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif
}

///////////////////////////////////////////////////////////////////////////////
void FVRFrameBuffer::Resolve(void)
{
#if TKD_ENGINE_CLIENT
    if (m_msaaSamples > 1)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_framebuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_resolveFramebuffer);
        glBlitFramebuffer(
            0,
            0,
            m_width,
            m_height,
            0,
            0,
            m_width,
            m_height,
            GL_COLOR_BUFFER_BIT,
            GL_LINEAR
        );
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
#endif
}

///////////////////////////////////////////////////////////////////////////////
FRenderTarget FVRFrameBuffer::GetRenderTarget(void) const
{
    FRenderTarget target;
    target.width = m_width;
    target.height = m_height;
    target.textureID = m_msaaSamples > 1 ? m_resolveTexture : m_renderTexture;
    return target;
}

}   // namespace tkd::VR
