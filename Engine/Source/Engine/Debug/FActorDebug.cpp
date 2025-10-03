///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Debug/FActorDebug.hpp>
#include <Engine/Runtime/Actor.hpp>
#include <Engine/Static/Engine.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::debug
///////////////////////////////////////////////////////////////////////////////
namespace tkd::debug
{

#if TKD_ENGINE_CLIENT

///////////////////////////////////////////////////////////////////////////////
void FActorDebug::Show(void)
{
    ImGui::Begin("Actor Debug");

    auto actors = Engine::World.GetActors();

    for (const auto& actor: actors)
    {
        auto components = actor->GetComponents();

        ImGui::Text(
            "%s: %s (%lu)",
            actor->GetName().CStr(),
            actor->GetObjectID().c_str(),
            components.Size()
        );

        for (const auto& component: components)
        {
            ImGui::BulletText(
                "%s: %s (Active: %s)",
                component->GetName().CStr(),
                component->GetObjectID().c_str(),
                component->IsActive() ? "True" : "False"
            );
        }
    }

    ImGui::End();
}

#endif

}   // namespace tkd::debug
