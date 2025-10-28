#include <cmath>
#include <GL/glew.h>
#include <GL/glu.h>
#include <iostream>
#include <openvr/openvr.h>
#include <SFML/OpenGL.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <string>

// Matrix helper functions
struct Matrix4
{
    float m[4][4];

    Matrix4()
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++) { m[i][j] = (i == j) ? 1.0f : 0.0f; }
        }
    }

    void toOpenGL(float* arr) const
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++) { arr[i * 4 + j] = m[j][i]; }
        }
    }

    Matrix4 invert() const
    {
        Matrix4 inv;
        float det;

        inv.m[0][0] =
            m[1][1] * m[2][2] * m[3][3] - m[1][1] * m[2][3] * m[3][2] -
            m[2][1] * m[1][2] * m[3][3] + m[2][1] * m[1][3] * m[3][2] +
            m[3][1] * m[1][2] * m[2][3] - m[3][1] * m[1][3] * m[2][2];

        inv.m[1][0] =
            -m[1][0] * m[2][2] * m[3][3] + m[1][0] * m[2][3] * m[3][2] +
            m[2][0] * m[1][2] * m[3][3] - m[2][0] * m[1][3] * m[3][2] -
            m[3][0] * m[1][2] * m[2][3] + m[3][0] * m[1][3] * m[2][2];

        det = m[0][0] * inv.m[0][0] + m[0][1] * inv.m[1][0];

        if (det != 0.0f)
        {
            det = 1.0f / det;
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++) { inv.m[i][j] *= det; }
            }
        }

        return inv;
    }
};

Matrix4 convertSteamVRMatrix(const vr::HmdMatrix34_t& matPose)
{
    Matrix4 mat;
    mat.m[0][0] = matPose.m[0][0];
    mat.m[1][0] = matPose.m[1][0];
    mat.m[2][0] = matPose.m[2][0];
    mat.m[3][0] = 0.0f;

    mat.m[0][1] = matPose.m[0][1];
    mat.m[1][1] = matPose.m[1][1];
    mat.m[2][1] = matPose.m[2][1];
    mat.m[3][1] = 0.0f;

    mat.m[0][2] = matPose.m[0][2];
    mat.m[1][2] = matPose.m[1][2];
    mat.m[2][2] = matPose.m[2][2];
    mat.m[3][2] = 0.0f;

    mat.m[0][3] = matPose.m[0][3];
    mat.m[1][3] = matPose.m[1][3];
    mat.m[2][3] = matPose.m[2][3];
    mat.m[3][3] = 1.0f;

    return mat;
}

Matrix4 convertSteamVRMatrix44(const vr::HmdMatrix44_t& mat)
{
    Matrix4 result;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++) { result.m[i][j] = mat.m[i][j]; }
    }
    return result;
}

// Framebuffer Eye structure
struct FramebufferDesc
{
    GLuint depthBufferId;
    GLuint renderTextureId;
    GLuint renderFramebufferId;
    GLuint resolveTextureId;
    GLuint resolveFramebufferId;
};

bool createFrameBuffer(int width, int height, FramebufferDesc& framebuffer)
{
    glGenFramebuffers(1, &framebuffer.renderFramebufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.renderFramebufferId);

    glGenRenderbuffers(1, &framebuffer.depthBufferId);
    glBindRenderbuffer(GL_RENDERBUFFER, framebuffer.depthBufferId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER,
        framebuffer.depthBufferId
    );

    glGenTextures(1, &framebuffer.renderTextureId);
    glBindTexture(GL_TEXTURE_2D, framebuffer.renderTextureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        framebuffer.renderTextureId,
        0
    );

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Framebuffer not complete!" << std::endl;
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

void drawController(bool isRightHand)
{
    glPushMatrix();

    glColor3f(isRightHand ? 0.8f : 0.2f, 0.2f, 0.8f);
    glPushMatrix();
    glScalef(0.03f, 0.1f, 0.05f);

    glBegin(GL_QUADS);
    // Front
    glVertex3f(-1, -1, 1);
    glVertex3f(1, -1, 1);
    glVertex3f(1, 1, 1);
    glVertex3f(-1, 1, 1);
    // Back
    glVertex3f(-1, -1, -1);
    glVertex3f(-1, 1, -1);
    glVertex3f(1, 1, -1);
    glVertex3f(1, -1, -1);
    // Top
    glVertex3f(-1, 1, -1);
    glVertex3f(-1, 1, 1);
    glVertex3f(1, 1, 1);
    glVertex3f(1, 1, -1);
    // Bottom
    glVertex3f(-1, -1, -1);
    glVertex3f(1, -1, -1);
    glVertex3f(1, -1, 1);
    glVertex3f(-1, -1, 1);
    // Right
    glVertex3f(1, -1, -1);
    glVertex3f(1, 1, -1);
    glVertex3f(1, 1, 1);
    glVertex3f(1, -1, 1);
    // Left
    glVertex3f(-1, -1, -1);
    glVertex3f(-1, -1, 1);
    glVertex3f(-1, 1, 1);
    glVertex3f(-1, 1, -1);
    glEnd();
    glPopMatrix();

    glColor3f(1.0f, 1.0f, 0.0f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, -1.0f);
    glEnd();

    glPopMatrix();
}

void drawCube(float x, float y, float z, float size)
{
    glPushMatrix();
    glTranslatef(x, y, z);

    float s = size / 2;
    glBegin(GL_QUADS);

    glColor3f(1, 0, 0);
    glVertex3f(-s, -s, s);
    glVertex3f(s, -s, s);
    glVertex3f(s, s, s);
    glVertex3f(-s, s, s);

    glColor3f(0, 1, 0);
    glVertex3f(-s, -s, -s);
    glVertex3f(-s, s, -s);
    glVertex3f(s, s, -s);
    glVertex3f(s, -s, -s);

    glColor3f(0, 0, 1);
    glVertex3f(-s, s, -s);
    glVertex3f(-s, s, s);
    glVertex3f(s, s, s);
    glVertex3f(s, s, -s);

    glColor3f(1, 1, 0);
    glVertex3f(-s, -s, -s);
    glVertex3f(s, -s, -s);
    glVertex3f(s, -s, s);
    glVertex3f(-s, -s, s);

    glColor3f(1, 0, 1);
    glVertex3f(s, -s, -s);
    glVertex3f(s, s, -s);
    glVertex3f(s, s, s);
    glVertex3f(s, -s, s);

    glColor3f(0, 1, 1);
    glVertex3f(-s, -s, -s);
    glVertex3f(-s, -s, s);
    glVertex3f(-s, s, s);
    glVertex3f(-s, s, -s);

    glEnd();
    glPopMatrix();
}

void drawScene()
{
    // Grid floor
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_LINES);
    for (int i = -10; i <= 10; i++)
    {
        glVertex3f(i * 0.5f, 0, -5);
        glVertex3f(i * 0.5f, 0, 5);
        glVertex3f(-5, 0, i * 0.5f);
        glVertex3f(5, 0, i * 0.5f);
    }
    glEnd();

    // Cubes
    drawCube(0, 1, -2, 0.3f);
    drawCube(-1, 0.5f, -1, 0.2f);
    drawCube(1, 0.5f, -1, 0.2f);
    drawCube(0, 0.5f, 0, 0.2f);
}

void renderScene(
    vr::IVRSystem* hmd,
    vr::Hmd_Eye eye,
    const vr::TrackedDevicePose_t* trackedPoses,
    int controllerIndices[2]
)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.15f, 0.15f, 0.18f, 1.0f);

    if (!trackedPoses[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid) { return; }

    Matrix4 headPose = convertSteamVRMatrix(
        trackedPoses[vr::k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking
    );
    Matrix4 eyeToHead = convertSteamVRMatrix(hmd->GetEyeToHeadTransform(eye));
    Matrix4 projection =
        convertSteamVRMatrix44(hmd->GetProjectionMatrix(eye, 0.1f, 100.0f));

    glMatrixMode(GL_PROJECTION);
    float projArr[16];
    projection.toOpenGL(projArr);
    glLoadMatrixf(projArr);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Calculate view matrix: inverse of (headPose * eyeToHead)
    Matrix4 viewMatrix;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            viewMatrix.m[i][j] = 0;
            for (int k = 0; k < 4; k++)
            {
                viewMatrix.m[i][j] += headPose.m[i][k] * eyeToHead.m[k][j];
            }
        }
    }

    // Simple inverse for viewing (transpose rotation, negate translation)
    Matrix4 view;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++) { view.m[i][j] = viewMatrix.m[j][i]; }
    }
    view.m[0][3] =
        -(viewMatrix.m[0][3] * view.m[0][0] +
          viewMatrix.m[1][3] * view.m[0][1] +
          viewMatrix.m[2][3] * view.m[0][2]);
    view.m[1][3] =
        -(viewMatrix.m[0][3] * view.m[1][0] +
          viewMatrix.m[1][3] * view.m[1][1] +
          viewMatrix.m[2][3] * view.m[1][2]);
    view.m[2][3] =
        -(viewMatrix.m[0][3] * view.m[2][0] +
          viewMatrix.m[1][3] * view.m[2][1] +
          viewMatrix.m[2][3] * view.m[2][2]);
    view.m[3][3] = 1.0f;

    float viewArr[16];
    view.toOpenGL(viewArr);
    glLoadMatrixf(viewArr);

    drawScene();

    // Draw controllers
    for (int i = 0; i < 2; i++)
    {
        if (controllerIndices[i] != -1 &&
            trackedPoses[controllerIndices[i]].bPoseIsValid)
        {
            Matrix4 controllerPose = convertSteamVRMatrix(
                trackedPoses[controllerIndices[i]].mDeviceToAbsoluteTracking
            );

            glPushMatrix();
            float ctrlArr[16];
            controllerPose.toOpenGL(ctrlArr);
            glMultMatrixf(ctrlArr);

            drawController(i == 1);
            glPopMatrix();
        }
    }
}

int main()
{
    std::cout << "Initializing OpenVR..." << std::endl;

    // Initialize OpenVR
    vr::EVRInitError error = vr::VRInitError_None;
    vr::IVRSystem* hmd = vr::VR_Init(&error, vr::VRApplication_Scene);

    if (error != vr::VRInitError_None)
    {
        std::cerr << "Failed to initialize OpenVR: "
                  << vr::VR_GetVRInitErrorAsEnglishDescription(error)
                  << std::endl;
        std::cerr << "Make sure SteamVR is running!" << std::endl;
        system("pause");
        return 1;
    }

    if (!hmd)
    {
        std::cerr << "HMD is NULL!" << std::endl;
        system("pause");
        return 1;
    }

    std::cout << "OpenVR initialized successfully!" << std::endl;
    std::cout << "Waiting for compositor..." << std::endl;

    // Wait a bit for compositor to initialize
    sf::sleep(sf::milliseconds(500));

    if (!vr::VRCompositor())
    {
        std::cerr << "Compositor initialization failed!" << std::endl;
        std::cerr << "Make sure SteamVR is fully loaded!" << std::endl;
        vr::VR_Shutdown();
        system("pause");
        return 1;
    }

    std::cout << "Compositor ready!" << std::endl;

    uint32_t width, height;
    hmd->GetRecommendedRenderTargetSize(&width, &height);
    std::cout << "Render target size: " << width << "x" << height << std::endl;

    // Create window
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 3;
    settings.minorVersion = 3;

    sf::Window window(
        sf::VideoMode(1280, 720),
        "OpenVR Game - Check your VR Headset!",
        sf::Style::Default,
        settings
    );

    window.setVerticalSyncEnabled(false);
    window.setActive(true);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK)
    {
        std::cerr << "Error initializing GLEW: "
                  << glewGetErrorString(glewError) << std::endl;
        return 1;
    }

    std::cout << "GLEW initialized successfully!" << std::endl;
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

    // OpenGL setup
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_MULTISAMPLE);

    // Create framebuffers for each eye
    FramebufferDesc leftEyeDesc, rightEyeDesc;
    if (!createFrameBuffer(width, height, leftEyeDesc))
    {
        std::cerr << "Failed to create left eye framebuffer" << std::endl;
        return 1;
    }
    if (!createFrameBuffer(width, height, rightEyeDesc))
    {
        std::cerr << "Failed to create right eye framebuffer" << std::endl;
        return 1;
    }

    std::cout << "Framebuffers created successfully!" << std::endl;
    std::cout << "Look in your VR headset now!" << std::endl;

    int controllerIndices[2] = { -1, -1 };
    vr::TrackedDevicePose_t trackedPoses[vr::k_unMaxTrackedDeviceCount];

    bool running = true;
    while (running)
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) { running = false; }
            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::Escape)
            {
                running = false;
            }
        }

        vr::VREvent_t vrEvent;
        while (hmd->PollNextEvent(&vrEvent, sizeof(vrEvent)))
        {
            // Handle VR events
        }

        // Update poses
        vr::EVRCompositorError compositorError =
            vr::VRCompositor()->WaitGetPoses(
                trackedPoses, vr::k_unMaxTrackedDeviceCount, nullptr, 0
            );
        if (compositorError != vr::VRCompositorError_None)
        {
            std::cerr << "WaitGetPoses error: " << compositorError
                      << std::endl;
            continue;
        }

        // Find controllers
        for (int i = 0; i < 2; i++) { controllerIndices[i] = -1; }
        for (uint32_t i = 0; i < vr::k_unMaxTrackedDeviceCount; i++)
        {
            if (!trackedPoses[i].bPoseIsValid) { continue; }

            vr::ETrackedDeviceClass deviceClass =
                hmd->GetTrackedDeviceClass(i);
            if (deviceClass == vr::TrackedDeviceClass_Controller)
            {
                vr::ETrackedControllerRole role =
                    hmd->GetControllerRoleForTrackedDeviceIndex(i);
                if (role == vr::TrackedControllerRole_LeftHand)
                {
                    controllerIndices[0] = i;
                }
                else if (role == vr::TrackedControllerRole_RightHand)
                {
                    controllerIndices[1] = i;
                }
            }
        }

        // Render left eye
        glBindFramebuffer(GL_FRAMEBUFFER, leftEyeDesc.renderFramebufferId);
        glViewport(0, 0, width, height);
        renderScene(hmd, vr::Eye_Left, trackedPoses, controllerIndices);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Render right eye
        glBindFramebuffer(GL_FRAMEBUFFER, rightEyeDesc.renderFramebufferId);
        glViewport(0, 0, width, height);
        renderScene(hmd, vr::Eye_Right, trackedPoses, controllerIndices);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Submit to VR compositor
        vr::Texture_t leftEyeTexture = { (void*)(uintptr_t
                                         )leftEyeDesc.renderTextureId,
                                         vr::TextureType_OpenGL,
                                         vr::ColorSpace_Gamma };
        vr::EVRCompositorError submitError =
            vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);
        if (submitError != vr::VRCompositorError_None)
        {
            std::cerr << "Left eye submit error: " << submitError << std::endl;
        }

        vr::Texture_t rightEyeTexture = { (void*)(uintptr_t
                                          )rightEyeDesc.renderTextureId,
                                          vr::TextureType_OpenGL,
                                          vr::ColorSpace_Gamma };
        submitError =
            vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture);
        if (submitError != vr::VRCompositorError_None)
        {
            std::cerr << "Right eye submit error: " << submitError
                      << std::endl;
        }

        // Display preview in window (show what left eye sees)
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, 1280, 720);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Setup orthographic projection for displaying texture
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-1, 1, -1, 1, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Disable depth test for 2D rendering
        glDisable(GL_DEPTH_TEST);

        // Bind and display the left eye texture
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, leftEyeDesc.renderTextureId);

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

        glDisable(GL_TEXTURE_2D);
        glEnable(GL_DEPTH_TEST);

        window.display();
    }

    vr::VR_Shutdown();
    std::cout << "VR shutdown complete" << std::endl;

    return 0;
}
