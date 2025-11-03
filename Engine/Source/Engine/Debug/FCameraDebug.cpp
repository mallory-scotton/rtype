///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Debug/FCameraDebug.hpp>
#include <cmath>
#include <Engine/Static.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::debug
///////////////////////////////////////////////////////////////////////////////
namespace tkd::debug
{

#if TKD_ENGINE_CLIENT

///////////////////////////////////////////////////////////////////////////////
void FCameraDebug::Show(
    const FEngineSettings& /*settings*/, UWorld* /*world*/
)
{
    // Window styling and flags for a compact, readable debug window
    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;
    ImGui::SetNextWindowBgAlpha(0.95f);
    ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
    ImGui::Begin("Camera Debug", nullptr, flags);

    FCamera& camera = Window::GetCamera();

    // --- Position ---
    ImGui::Text("Position");
    ImGui::Separator();
    // Use a compact 3-value input for position
    ImGui::PushID("pos");
    float pos[3] = { camera.position.x, camera.position.y, camera.position.z };
    if (ImGui::InputFloat3("##position", pos))
    {
        camera.position.x = pos[0];
        camera.position.y = pos[1];
        camera.position.z = pos[2];
    }
    ImGui::PopID();

    ImGui::Spacing();

    // --- Rotation ---
    ImGui::Text("Rotation (Euler)");
    ImGui::Separator();
    ImGui::PushID("rot");
    if (ImGui::InputFloat("Pitch", &camera.pitch, 0.5f, 5.0f, "%.3f"))
    {
        // clamp pitch to avoid flipping
        if (camera.pitch > 89.9f) { camera.pitch = 89.9f; }
        if (camera.pitch < -89.9f) { camera.pitch = -89.9f; }
        camera.UpdateCameraVectors();
    }
    ImGui::SameLine();
    if (ImGui::InputFloat("Yaw", &camera.yaw, 0.5f, 5.0f, "%.3f"))
    {
        // normalize yaw to [-180,180]
        while (camera.yaw > 180.0f) { camera.yaw -= 360.0f; }
        while (camera.yaw < -180.0f) { camera.yaw += 360.0f; }
        camera.UpdateCameraVectors();
    }
    ImGui::PopID();

    ImGui::Spacing();

    // --- Projection ---
    ImGui::Text("Projection");
    ImGui::Separator();
    ImGui::PushID("proj");
    ImGui::InputFloat("FOV", &camera.fov, 0.1f, 1.0f, "%.2f");
    ImGui::SameLine();
    ImGui::InputFloat("Aspect", &camera.aspectRatio, 0.01f, 0.1f, "%.3f");
    ImGui::InputFloat("Near", &camera.nearPlane, 0.01f, 0.1f, "%.3f");
    ImGui::InputFloat("Far", &camera.farPlane, 10.0f, 100.0f, "%.1f");
    ImGui::PopID();

    ImGui::Spacing();

    // Quick actions
    if (ImGui::Button("Reset Transform"))
    {
        camera.position.x = 0.0f;
        camera.position.y = 0.0f;
        camera.position.z = 0.0f;
        camera.pitch = 0.0f;
        camera.yaw = 0.0f;
    }
    ImGui::SameLine();
    if (ImGui::Button("Center View"))
    {
        // A small helper to recenter; may be adjusted by the caller
        camera.position.x = 0.0f;
        camera.position.y = 0.0f;
        camera.position.z = 0.0f;
    }

    ImGui::Spacing();

    // --- FreeCam controls (WASD + QE + mouse-look) ---
    static bool freeCamEnabled = false;
    static float freeCamSpeed = 600.0f;        // world units per second
    static float freeCamSensitivity = 0.15f;   // degrees per pixel

    ImGui::Separator();
    ImGui::Text("FreeCam");
    ImGui::Checkbox("Enable FreeCam (focus this window)", &freeCamEnabled);
    ImGui::InputFloat("Speed", &freeCamSpeed, 10.0f, 100.0f, "%.0f");
    ImGui::InputFloat("Sensitivity", &freeCamSensitivity, 0.01f, 0.1f, "%.3f");

    ImGui::TextWrapped(
        "Controls: Hold Right Mouse Button + move to look. WASD to move, Q/E to move down/up."
    );

    // Apply FreeCam movement when enabled and this window is focused
    if (freeCamEnabled && ImGui::IsWindowFocused())
    {
        ImGuiIO& io = ImGui::GetIO();
        const float dt = (io.DeltaTime > 0.0f) ? io.DeltaTime : (1.0f / 60.0f);

        // Mouse-look when holding right mouse button
        if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
        {
            ImVec2 md = io.MouseDelta;
            camera.yaw += md.x * freeCamSensitivity;
            camera.pitch -= md.y * freeCamSensitivity;
            // clamp pitch
            if (camera.pitch > 89.9f) { camera.pitch = 89.9f; }
            if (camera.pitch < -89.9f) { camera.pitch = -89.9f; }

            camera.UpdateCameraVectors();
        }

        // Movement
        // Convert yaw/pitch (degrees) to radians
        const float degToRad = 3.14159265358979323846f / 180.0f;
        float yawRad = camera.yaw * degToRad;
        float pitchRad = camera.pitch * degToRad;

        // Forward vector
        float cx = std::cos(pitchRad) * std::cos(yawRad);
        float cy = std::sin(pitchRad);
        float cz = std::cos(pitchRad) * std::sin(yawRad);

        // Right vector (yaw + 90deg)
        float yawRight = (camera.yaw + 90.0f) * degToRad;
        float rx = std::cos(yawRight);
        float rz = std::sin(yawRight);

        float moveSpeed = freeCamSpeed * dt;

        if (ImGui::IsKeyDown(ImGuiKey_W))
        {
            camera.position.x += cx * moveSpeed;
            camera.position.y += cy * moveSpeed;
            camera.position.z += cz * moveSpeed;
        }
        if (ImGui::IsKeyDown(ImGuiKey_S))
        {
            camera.position.x -= cx * moveSpeed;
            camera.position.y -= cy * moveSpeed;
            camera.position.z -= cz * moveSpeed;
        }
        if (ImGui::IsKeyDown(ImGuiKey_A))
        {
            camera.position.x -= rx * moveSpeed;
            camera.position.z -= rz * moveSpeed;
        }
        if (ImGui::IsKeyDown(ImGuiKey_D))
        {
            camera.position.x += rx * moveSpeed;
            camera.position.z += rz * moveSpeed;
        }
        if (ImGui::IsKeyDown(ImGuiKey_Q) ||
            ImGui::IsKeyDown(ImGuiKey_LeftShift))
        {   // down
            camera.position.y -= moveSpeed;
        }
        if (ImGui::IsKeyDown(ImGuiKey_E) || ImGui::IsKeyDown(ImGuiKey_Space))
        {   // up
            camera.position.y += moveSpeed;
        }
    }

    ImGui::PopStyleColor();
    ImGui::End();
}

#endif

}   // namespace tkd::debug
