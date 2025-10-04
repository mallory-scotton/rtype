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

    auto classes = UClass::GetAllClasses();

    ImGui::Text("Registered Classes: %lu", classes.size());

    for (const auto& _class: classes)
    {
        ImGui::Text("Class: %s", _class->GetName().CStr());
        auto properties = _class->GetProperties();
        ImGui::Text("  Properties: %lu", properties.size());
        for (const auto& property: properties)
        {
            ImGui::Text("    %s", property.CStr());
        }
    }

    ImGui::Separator();
    ImGui::Text("Actors: %lu", actors.size());

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
