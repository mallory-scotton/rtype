#include <cmath>
#include <GL/glu.h>
#include <memory>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <vector>

// Aliases for easier access
struct Vec3
{
    float x, y, z;
};

struct Color
{
    float r, g, b, a;
};

// Camera Constants
const float CAMERA_FOV = 60.0f;
const float CAMERA_ASPECT_RATIO = 4.0f / 3.0f;
const float CAMERA_NEAR_PLANE = 0.1f;
const float CAMERA_FAR_PLANE = 1000.0f;

// Keybinding
const sf::Keyboard::Key KEY_MOVE_FORWARD = sf::Keyboard::W;
const sf::Keyboard::Key KEY_MOVE_BACKWARD = sf::Keyboard::S;
const sf::Keyboard::Key KEY_MOVE_LEFT = sf::Keyboard::A;
const sf::Keyboard::Key KEY_MOVE_RIGHT = sf::Keyboard::D;
const sf::Keyboard::Key KEY_MOVE_UP = sf::Keyboard::Space;
const sf::Keyboard::Key KEY_MOVE_DOWN = sf::Keyboard::LShift;
const sf::Keyboard::Key KEY_TOGGLE_MOUSE = sf::Keyboard::Tab;

// ============================================================================
// Vector Math Utilities
// ============================================================================

Vec3 operator+(const Vec3& a, const Vec3& b)
{
    return { a.x + b.x, a.y + b.y, a.z + b.z };
}

Vec3 operator-(const Vec3& a, const Vec3& b)
{
    return { a.x - b.x, a.y - b.y, a.z - b.z };
}

Vec3 operator*(const Vec3& v, float s)
{
    return { v.x * s, v.y * s, v.z * s };
}

Vec3 normalize(const Vec3& v)
{
    float len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (len < 0.0001f) { return { 0, 0, 1 }; }
    return { v.x / len, v.y / len, v.z / len };
}

Vec3 cross(const Vec3& a, const Vec3& b)
{
    return { a.y * b.z - a.z * b.y,
             a.z * b.x - a.x * b.z,
             a.x * b.y - a.y * b.x };
}

// ============================================================================
// Camera
// ============================================================================

class Camera
{
public:
    Vec3 position{ 0.0f, 2.0f, 10.0f };
    Vec3 front{ 0.0f, 0.0f, -1.0f };
    Vec3 up{ 0.0f, 1.0f, 0.0f };
    Vec3 right{ 1.0f, 0.0f, 0.0f };

    float yaw = -90.0f;
    float pitch = 0.0f;
    float moveSpeed = 5.0f;
    float mouseSensitivity = 0.05f;   // Reduced from 0.1f

    float fov;
    float aspectRatio;
    float nearPlane;
    float farPlane;

    Camera(
        float fov = CAMERA_FOV,
        float aspect = CAMERA_ASPECT_RATIO,
        float near = CAMERA_NEAR_PLANE,
        float far = CAMERA_FAR_PLANE
    )
        : fov(fov)
        , aspectRatio(aspect)
        , nearPlane(near)
        , farPlane(far)
    {
        updateCameraVectors();
    }

    void update(float deltaTime)
    {
        // Keyboard movement
        if (sf::Keyboard::isKeyPressed(KEY_MOVE_FORWARD))
        {
            position = position + front * moveSpeed * deltaTime;
        }
        if (sf::Keyboard::isKeyPressed(KEY_MOVE_BACKWARD))
        {
            position = position + front * (-moveSpeed * deltaTime);
        }
        if (sf::Keyboard::isKeyPressed(KEY_MOVE_LEFT))
        {
            position = position + right * (-moveSpeed * deltaTime);
        }
        if (sf::Keyboard::isKeyPressed(KEY_MOVE_RIGHT))
        {
            position = position + right * moveSpeed * deltaTime;
        }
        if (sf::Keyboard::isKeyPressed(KEY_MOVE_UP))
        {
            position = position + up * moveSpeed * deltaTime;
        }
        if (sf::Keyboard::isKeyPressed(KEY_MOVE_DOWN))
        {
            position = position + up * (-moveSpeed * deltaTime);
        }
    }

    void processMouseMovement(float xoffset, float yoffset)
    {
        xoffset *= mouseSensitivity;
        yoffset *= mouseSensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (pitch > 89.0f) { pitch = 89.0f; }
        if (pitch < -89.0f) { pitch = -89.0f; }

        updateCameraVectors();
    }

    void applyView() const
    {
        Vec3 center = position + front;
        gluLookAt(
            position.x,
            position.y,
            position.z,
            center.x,
            center.y,
            center.z,
            up.x,
            up.y,
            up.z
        );
    }

private:
    void updateCameraVectors()
    {
        Vec3 newFront;
        newFront.x = std::cos(yaw * 3.14159265f / 180.0f) *
                     std::cos(pitch * 3.14159265f / 180.0f);
        newFront.y = std::sin(pitch * 3.14159265f / 180.0f);
        newFront.z = std::sin(yaw * 3.14159265f / 180.0f) *
                     std::cos(pitch * 3.14159265f / 180.0f);

        front = normalize(newFront);
        right = normalize(cross(front, { 0.0f, 1.0f, 0.0f }));
        up = normalize(cross(right, front));
    }
};

// ============================================================================
// Primitives
// ============================================================================

class IPrimitive
{
public:
    Vec3 position{ 0.0f, 0.0f, 0.0f };
    Vec3 rotation{ 0.0f, 0.0f, 0.0f };
    Vec3 scale{ 1.0f, 1.0f, 1.0f };
    Color color{ 1.0f, 1.0f, 1.0f, 1.0f };

    virtual ~IPrimitive() = default;
    virtual void render() = 0;

    void applyTransform() const
    {
        glTranslatef(position.x, position.y, position.z);
        glRotatef(rotation.x, 1.0f, 0.0f, 0.0f);
        glRotatef(rotation.y, 0.0f, 1.0f, 0.0f);
        glRotatef(rotation.z, 0.0f, 0.0f, 1.0f);
        glScalef(scale.x, scale.y, scale.z);
    }
};

class Cube : public IPrimitive
{
public:
    Cube(const Vec3& pos, const Vec3& scl, const Color& col)
    {
        position = pos;
        scale = scl;
        color = col;
    }

    void render() override
    {
        glPushMatrix();
        applyTransform();
        glColor4f(color.r, color.g, color.b, color.a);

        glBegin(GL_QUADS);

        // Front face
        glNormal3f(0.0f, 0.0f, 1.0f);
        glVertex3f(-0.5f, -0.5f, 0.5f);
        glVertex3f(0.5f, -0.5f, 0.5f);
        glVertex3f(0.5f, 0.5f, 0.5f);
        glVertex3f(-0.5f, 0.5f, 0.5f);

        // Back face
        glNormal3f(0.0f, 0.0f, -1.0f);
        glVertex3f(-0.5f, -0.5f, -0.5f);
        glVertex3f(-0.5f, 0.5f, -0.5f);
        glVertex3f(0.5f, 0.5f, -0.5f);
        glVertex3f(0.5f, -0.5f, -0.5f);

        // Top face
        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(-0.5f, 0.5f, -0.5f);
        glVertex3f(-0.5f, 0.5f, 0.5f);
        glVertex3f(0.5f, 0.5f, 0.5f);
        glVertex3f(0.5f, 0.5f, -0.5f);

        // Bottom face
        glNormal3f(0.0f, -1.0f, 0.0f);
        glVertex3f(-0.5f, -0.5f, -0.5f);
        glVertex3f(0.5f, -0.5f, -0.5f);
        glVertex3f(0.5f, -0.5f, 0.5f);
        glVertex3f(-0.5f, -0.5f, 0.5f);

        // Right face
        glNormal3f(1.0f, 0.0f, 0.0f);
        glVertex3f(0.5f, -0.5f, -0.5f);
        glVertex3f(0.5f, 0.5f, -0.5f);
        glVertex3f(0.5f, 0.5f, 0.5f);
        glVertex3f(0.5f, -0.5f, 0.5f);

        // Left face
        glNormal3f(-1.0f, 0.0f, 0.0f);
        glVertex3f(-0.5f, -0.5f, -0.5f);
        glVertex3f(-0.5f, -0.5f, 0.5f);
        glVertex3f(-0.5f, 0.5f, 0.5f);
        glVertex3f(-0.5f, 0.5f, -0.5f);

        glEnd();
        glPopMatrix();
    }
};

class Sphere : public IPrimitive
{
public:
    Sphere(const Vec3& pos, float radius, const Color& col, int segments = 32)
        : segmentCount(segments)
    {
        position = pos;
        scale = { radius, radius, radius };
        color = col;
    }

    void render() override
    {
        glPushMatrix();
        applyTransform();
        glColor4f(color.r, color.g, color.b, color.a);

        const float pi = 3.14159265f;

        for (int lat = 0; lat < segmentCount; ++lat)
        {
            glBegin(GL_QUAD_STRIP);
            for (int lon = 0; lon <= segmentCount; ++lon)
            {
                for (int k = 0; k < 2; ++k)
                {
                    float theta = (lat + k) * pi / segmentCount;
                    float phi = lon * 2.0f * pi / segmentCount;

                    float x = std::cos(phi) * std::sin(theta);
                    float y = std::cos(theta);
                    float z = std::sin(phi) * std::sin(theta);

                    glNormal3f(x, y, z);
                    glVertex3f(x, y, z);
                }
            }
            glEnd();
        }

        glPopMatrix();
    }

private:
    int segmentCount;
};

class Plane : public IPrimitive
{
public:
    Plane(const Vec3& pos, const Vec3& scl, const Color& col)
    {
        position = pos;
        scale = scl;
        color = col;
    }

    void render() override
    {
        glPushMatrix();
        applyTransform();
        glColor4f(color.r, color.g, color.b, color.a);

        glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(-0.5f, 0.0f, -0.5f);
        glVertex3f(0.5f, 0.0f, -0.5f);
        glVertex3f(0.5f, 0.0f, 0.5f);
        glVertex3f(-0.5f, 0.0f, 0.5f);
        glEnd();

        glPopMatrix();
    }
};

// ============================================================================
// World
// ============================================================================

class World
{
public:
    Camera camera;
    std::vector<std::unique_ptr<IPrimitive>> primitives;

    World()
        : camera(
              CAMERA_FOV,
              CAMERA_ASPECT_RATIO,
              CAMERA_NEAR_PLANE,
              CAMERA_FAR_PLANE
          )
    {}

    void addPrimitive(std::unique_ptr<IPrimitive> primitive)
    {
        primitives.push_back(std::move(primitive));
    }

    void update(float deltaTime) { camera.update(deltaTime); }

    void render()
    {
        camera.applyView();

        for (const auto& primitive: primitives) { primitive->render(); }
    }
};

// ============================================================================
// OpenGL Setup
// ============================================================================

void initOpenGL(int width, int height)
{
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Enable back-face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // Enable lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    // Set up light
    GLfloat lightPos[] = { 10.0f, 10.0f, 10.0f, 1.0f };
    GLfloat lightAmbient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);

    // Set up viewport and projection
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(
        CAMERA_FOV, CAMERA_ASPECT_RATIO, CAMERA_NEAR_PLANE, CAMERA_FAR_PLANE
    );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Clear color (background)
    glClearColor(0.2f, 0.2f, 0.3f, 1.0f);

    // Enable smooth shading
    glShadeModel(GL_SMOOTH);
}

// ============================================================================
// Main
// ============================================================================

int main()
{
    // Create SFML window with OpenGL context
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 2;
    settings.minorVersion = 1;

    sf::Window window(
        sf::VideoMode(800, 600),
        "SFML + OpenGL 3D Scene",
        sf::Style::Default,
        settings
    );
    window.setFramerateLimit(60);

    bool mouseCaptured = true;
    window.setMouseCursorVisible(!mouseCaptured);
    window.setMouseCursorGrabbed(mouseCaptured);

    // Initialize OpenGL
    initOpenGL(800, 600);

    // Create world and add primitives
    World world;

    // Add various colored cubes
    world.addPrimitive(std::make_unique<Cube>(
        Vec3{ 0.0f, 0.0f, 0.0f },
        Vec3{ 1.0f, 1.0f, 1.0f },
        Color{ 1.0f, 0.0f, 0.0f, 1.0f }
    ));
    world.addPrimitive(std::make_unique<Cube>(
        Vec3{ 3.0f, 0.0f, 0.0f },
        Vec3{ 1.5f, 1.5f, 1.5f },
        Color{ 0.0f, 1.0f, 0.0f, 1.0f }
    ));
    world.addPrimitive(std::make_unique<Cube>(
        Vec3{ -3.0f, 0.0f, 0.0f },
        Vec3{ 1.0f, 2.0f, 1.0f },
        Color{ 0.0f, 0.0f, 1.0f, 1.0f }
    ));

    // Add spheres
    world.addPrimitive(std::make_unique<Sphere>(
        Vec3{ 0.0f, 3.0f, 0.0f }, 1.0f, Color{ 1.0f, 1.0f, 0.0f, 1.0f }, 32
    ));
    world.addPrimitive(std::make_unique<Sphere>(
        Vec3{ 3.0f, 3.0f, 0.0f }, 0.7f, Color{ 1.0f, 0.0f, 1.0f, 1.0f }, 24
    ));

    // Add ground plane
    world.addPrimitive(std::make_unique<Plane>(
        Vec3{ 0.0f, -2.0f, 0.0f },
        Vec3{ 15.0f, 1.0f, 15.0f },
        Color{ 0.4f, 0.4f, 0.4f, 1.0f }
    ));

    // Additional cubes in the distance
    world.addPrimitive(std::make_unique<Cube>(
        Vec3{ 0.0f, 0.0f, -5.0f },
        Vec3{ 1.0f, 1.0f, 1.0f },
        Color{ 0.0f, 1.0f, 1.0f, 1.0f }
    ));
    world.addPrimitive(std::make_unique<Cube>(
        Vec3{ 5.0f, 0.0f, -5.0f },
        Vec3{ 1.0f, 1.0f, 1.0f },
        Color{ 1.0f, 0.5f, 0.0f, 1.0f }
    ));

    sf::Clock clock;
    sf::Vector2i centerPos(400, 300);
    bool firstMouse = true;

    bool running = true;
    while (running)
    {
        float deltaTime = clock.restart().asSeconds();

        // Handle events
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) { running = false; }
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Escape)
                {
                    running = false;
                }
                if (event.key.code == KEY_TOGGLE_MOUSE)
                {
                    mouseCaptured = !mouseCaptured;
                    window.setMouseCursorVisible(!mouseCaptured);
                    window.setMouseCursorGrabbed(mouseCaptured);
                    firstMouse = true;   // Reset mouse tracking
                }
            }
            if (event.type == sf::Event::Resized)
            {
                glViewport(0, 0, event.size.width, event.size.height);
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                gluPerspective(
                    CAMERA_FOV,
                    (float)event.size.width / event.size.height,
                    CAMERA_NEAR_PLANE,
                    CAMERA_FAR_PLANE
                );
                glMatrixMode(GL_MODELVIEW);
                centerPos.x = event.size.width / 2;
                centerPos.y = event.size.height / 2;
            }
        }

        // Mouse movement for camera (only when captured)
        if (mouseCaptured)
        {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);

            if (firstMouse)
            {
                sf::Mouse::setPosition(centerPos, window);
                firstMouse = false;
            }
            else
            {
                float xoffset = static_cast<float>(mousePos.x - centerPos.x);
                float yoffset = static_cast<float>(centerPos.y - mousePos.y);

                // Only process if there's actual movement
                if (std::abs(xoffset) > 0.1f || std::abs(yoffset) > 0.1f)
                {
                    world.camera.processMouseMovement(xoffset, yoffset);
                    sf::Mouse::setPosition(centerPos, window);
                }
            }
        }

        // Update world
        world.update(deltaTime);

        // Render
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        world.render();

        // Swap buffers
        window.display();
    }

    return 0;
}
