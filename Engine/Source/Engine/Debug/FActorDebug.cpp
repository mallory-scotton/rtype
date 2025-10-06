///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Debug/FActorDebug.hpp>
#include <algorithm>
#include <Engine/Runtime/Actor/AActor.hpp>
#include <Engine/Static/FEngineInterface.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::debug
///////////////////////////////////////////////////////////////////////////////
namespace tkd::debug
{

#if TKD_ENGINE_CLIENT

///////////////////////////////////////////////////////////////////////////////
void FActorDebug::Show(const FEngineSettings& settings, UWorld* world)
{
    TKD_UNUSED(settings);

    ImGui::Begin("Actor & Class Debug", nullptr);

    // Header with styling
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.8f, 1.0f, 1.0f));
    ImGui::Text("Actor & Class Information");
    ImGui::PopStyleColor();

    auto actors = world->GetActors();
    auto classes = UClass::GetAllClasses();

    ImGui::Text(
        "Total Classes: %lu | Total Actors: %lu", classes.size(), actors.size()
    );
    ImGui::Separator();
    ImGui::Spacing();

    // Create tab bar for different views
    if (ImGui::BeginTabBar("ActorDebugTabs", ImGuiTabBarFlags_None))
    {
        if (ImGui::BeginTabItem("Class Hierarchy"))
        {
            ShowClassHierarchy();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("World View"))
        {
            ShowWorldView(world);
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}

///////////////////////////////////////////////////////////////////////////////
void FActorDebug::ShowClassHierarchy(void)
{
    ImGui::Spacing();

    auto classes = UClass::GetAllClasses();

    if (classes.empty())
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
        ImGui::Text("No classes registered");
        ImGui::PopStyleColor();
        return;
    }

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.2f, 1.0f));
    ImGui::Text("Registered Classes: %lu", classes.size());
    ImGui::PopStyleColor();
    ImGui::Spacing();

    // Find root classes (classes with no superclass)
    std::vector<UClass*> rootClasses;
    for (auto* _class: classes)
    {
        if (_class->GetSuper() == nullptr) { rootClasses.push_back(_class); }
    }

    // Display each root class and its hierarchy
    ImGui::BeginChild(
        "ClassHierarchyScroll",
        ImVec2(0, 400),
        true,
        ImGuiWindowFlags_HorizontalScrollbar
    );

    for (auto* rootClass: rootClasses)
    {
        DisplayClassTree(rootClass, classes);
    }

    ImGui::EndChild();
}

///////////////////////////////////////////////////////////////////////////////
void FActorDebug::ShowWorldView(UWorld* world)
{
    ImGui::Spacing();

    auto actors = world->GetActors();

    if (actors.empty())
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
        ImGui::Text("No actors in world");
        ImGui::PopStyleColor();
        return;
    }

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.2f, 1.0f));
    ImGui::Text("Active Actors: %lu", actors.size());
    ImGui::PopStyleColor();
    ImGui::Spacing();

    ImGui::BeginChild(
        "WorldViewScroll",
        ImVec2(0, 400),
        true,
        ImGuiWindowFlags_HorizontalScrollbar
    );

    for (auto& actor: actors)
    {
        DisplayActorInfo(actor.get());
        ImGui::Spacing();
    }

    ImGui::EndChild();
}

///////////////////////////////////////////////////////////////////////////////
void FActorDebug::DisplayClassTree(
    UClass* _class, const std::vector<UClass*>& allClasses
)
{
    if (!_class) { return; }

    // Find child classes
    std::vector<UClass*> children;
    for (auto* potentialChild: allClasses)
    {
        if (potentialChild->GetSuper() == _class)
        {
            children.push_back(potentialChild);
        }
    }

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow |
                               ImGuiTreeNodeFlags_OpenOnDoubleClick |
                               ImGuiTreeNodeFlags_SpanAvailWidth;

    if (children.empty())
    {
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }

    // Color the class name
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 1.0f, 0.8f, 1.0f));

    bool nodeOpen = ImGui::TreeNodeEx(
        (void*)(intptr_t)_class, flags, "%s", _class->GetName().CStr()
    );

    ImGui::PopStyleColor();

    // Show properties on same line
    ImGui::SameLine();
    auto properties = _class->GetProperties();
    auto functions = _class->GetFunctions();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
    ImGui::Text(
        "(%lu properties, %lu functions)", properties.size(), functions.size()
    );
    ImGui::PopStyleColor();

    if (nodeOpen && !children.empty())
    {
        // Display properties
        if (!properties.empty())
        {
            ImGui::Indent();
            DisplayClassProperties(_class);
            ImGui::Unindent();
        }

        // Display functions
        if (!functions.empty())
        {
            ImGui::Indent();
            DisplayClassFunctions(_class);
            ImGui::Unindent();
        }

        if (!properties.empty() || !functions.empty()) { ImGui::Spacing(); }

        // Display children
        for (auto* child: children) { DisplayClassTree(child, allClasses); }

        ImGui::TreePop();
    }
    else if (nodeOpen && children.empty())
    {
        // For leaf nodes, show properties and functions in tooltip
        if (ImGui::IsItemHovered() &&
            (!properties.empty() || !functions.empty()))
        {
            ImGui::BeginTooltip();

            if (!properties.empty())
            {
                ImGui::PushStyleColor(
                    ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.5f, 1.0f)
                );
                ImGui::Text("Properties:");
                ImGui::PopStyleColor();
                for (const auto& property: properties)
                {
                    ImGui::BulletText("%s", property.CStr());
                }
            }

            if (!functions.empty())
            {
                if (!properties.empty()) { ImGui::Spacing(); }
                ImGui::PushStyleColor(
                    ImGuiCol_Text, ImVec4(0.5f, 0.9f, 1.0f, 1.0f)
                );
                ImGui::Text("Functions:");
                ImGui::PopStyleColor();
                for (const auto& function: functions)
                {
                    ImGui::BulletText("%s", function.CStr());
                }
            }

            ImGui::EndTooltip();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void FActorDebug::DisplayClassProperties(UClass* _class)
{
    if (!_class) { return; }

    auto properties = _class->GetProperties();

    if (properties.empty()) { return; }

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.8f, 1.0f, 1.0f));
    ImGui::Text("Properties:");
    ImGui::PopStyleColor();

    ImGui::Indent();
    for (const auto& property: properties)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.7f, 1.0f));
        ImGui::BulletText("%s", property.CStr());
        ImGui::PopStyleColor();
    }
    ImGui::Unindent();
}

///////////////////////////////////////////////////////////////////////////////
void FActorDebug::DisplayClassFunctions(UClass* _class)
{
    if (!_class) { return; }

    auto functions = _class->GetFunctions();

    if (functions.empty()) { return; }

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("Functions:");
    ImGui::PopStyleColor();

    ImGui::Indent();
    for (const auto& functionName: functions)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.9f, 1.0f, 1.0f));
        ImGui::BulletText("%s", functionName.CStr());
        ImGui::PopStyleColor();
    }
    ImGui::Unindent();
}

///////////////////////////////////////////////////////////////////////////////
void FActorDebug::DisplayActorInfo(AActor* actor)
{
    if (!actor) { return; }

    bool isActive = actor->IsActive();
    auto components = actor->GetComponents();

    // Use collapsing header for each actor
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;

    // Gray out if inactive
    if (!isActive)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.3f, 0.3f, 0.3f, 0.8f));
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.9f, 0.4f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.4f, 0.6f, 0.8f, 0.8f));
    }

    bool nodeOpen = ImGui::CollapsingHeader(actor->GetName().CStr(), flags);
    UClass* actorClass = actor->GetClass();
    if (actorClass)
    {
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
        ImGui::Text("(%s)", actorClass->GetName().CStr());
        ImGui::PopStyleColor();
    }

    ImGui::PopStyleColor(2);

    if (nodeOpen)
    {
        ImGui::Indent();

        // Display actor info in a table
        if (ImGui::BeginTable(
                ("ActorInfoTable" + actor->GetObjectID()).c_str(),
                2,
                ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                    ImGuiTableFlags_Resizable
            ))
        {
            ImGui::TableSetupColumn(
                "Property", ImGuiTableColumnFlags_WidthFixed, 150.0f
            );
            ImGui::TableSetupColumn(
                "Value", ImGuiTableColumnFlags_WidthStretch
            );
            ImGui::TableHeadersRow();

            // Object ID
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Object ID");
            ImGui::TableSetColumnIndex(1);
            ImGui::PushStyleColor(
                ImGuiCol_Text, ImVec4(0.7f, 0.9f, 1.0f, 1.0f)
            );
            ImGui::Text("%s", actor->GetObjectID().c_str());
            ImGui::PopStyleColor();

            // Active status
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Active");
            ImGui::TableSetColumnIndex(1);
            if (isActive)
            {
                ImGui::PushStyleColor(
                    ImGuiCol_Text, ImVec4(0.4f, 1.0f, 0.4f, 1.0f)
                );
                ImGui::Text("True");
            }
            else
            {
                ImGui::PushStyleColor(
                    ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f)
                );
                ImGui::Text("False");
            }
            ImGui::PopStyleColor();

            // Transform
            const auto& transform = actor->GetTransform();

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Position");
            ImGui::TableSetColumnIndex(1);
            DisplayVector(transform.GetPosition());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Rotation");
            ImGui::TableSetColumnIndex(1);
            DisplayRotation(transform.GetRotation());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Scale");
            ImGui::TableSetColumnIndex(1);
            DisplayVector(transform.GetScale());

            // Component count
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Components");
            ImGui::TableSetColumnIndex(1);
            ImGui::PushStyleColor(
                ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.5f, 1.0f)
            );
            ImGui::Text("%lu", components.Size());
            ImGui::PopStyleColor();

            ImGui::EndTable();
        }

        // Display components
        if (!components.Empty())
        {
            ImGui::Spacing();
            ImGui::PushStyleColor(
                ImGuiCol_Text, ImVec4(0.6f, 0.9f, 0.6f, 1.0f)
            );
            ImGui::Text("Components:");
            ImGui::PopStyleColor();

            ImGui::Indent();
            for (const auto& component: components)
            {
                DisplayComponentInfo(component.get());
            }
            ImGui::Unindent();
        }

        // Display class properties
        auto properties =
            actorClass ? actorClass->GetProperties() : std::vector<FString>();
        if (!properties.empty())
        {
            ImGui::Spacing();
            ImGui::PushStyleColor(
                ImGuiCol_Text, ImVec4(0.6f, 0.8f, 1.0f, 1.0f)
            );
            ImGui::Text("Properties:");
            ImGui::PopStyleColor();

            ImGui::Indent();
            for (const auto& property: properties)
            {
                ImGui::PushStyleColor(
                    ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.7f, 1.0f)
                );
                ImGui::BulletText(
                    "%s: %s",
                    property.CStr(),
                    actor->GetProperty(property)->ToString().CStr()
                );
                ImGui::PopStyleColor();
            }
            ImGui::Unindent();
        }

        // Display functions
        auto functions =
            actorClass ? actorClass->GetFunctions() : std::vector<FString>();
        if (!functions.empty())
        {
            ImGui::Spacing();
            ImGui::PushStyleColor(
                ImGuiCol_Text, ImVec4(0.5f, 0.9f, 1.0f, 1.0f)
            );
            ImGui::Text("Functions:");
            ImGui::PopStyleColor();

            ImGui::Indent();
            for (const auto& functionName: functions)
            {
                IFunction* function = actor->GetFunction(functionName);
                bool isBound = function ? function->IsBound() : false;

                if (isBound)
                {
                    ImGui::PushStyleColor(
                        ImGuiCol_Text, ImVec4(0.4f, 1.0f, 0.4f, 1.0f)
                    );
                    ImGui::BulletText("%s [BOUND]", functionName.CStr());
                }
                else
                {
                    ImGui::PushStyleColor(
                        ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f)
                    );
                    ImGui::BulletText("%s [UNBOUND]", functionName.CStr());
                }
                ImGui::PopStyleColor();

                // Show tooltip with more info
                if (ImGui::IsItemHovered())
                {
                    ImGui::BeginTooltip();
                    ImGui::PushStyleColor(
                        ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.5f, 1.0f)
                    );
                    ImGui::Text("Function: %s", functionName.CStr());
                    ImGui::PopStyleColor();
                    ImGui::Separator();
                    ImGui::Text("Status: %s", isBound ? "Bound" : "Not Bound");
                    ImGui::EndTooltip();
                }
            }
            ImGui::Unindent();
        }

        ImGui::Unindent();
    }
}

///////////////////////////////////////////////////////////////////////////////
void FActorDebug::DisplayComponentInfo(UActorComponent* component)
{
    if (!component) { return; }

    bool isActive = component->IsActive();

    // Component bullet with color based on active status
    if (!isActive)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 1.0f, 0.9f, 1.0f));
    }

    ImGui::BulletText(
        "%s: %s %s",
        component->GetName().CStr(),
        component->GetObjectID().c_str(),
        isActive ? "" : "(Inactive)"
    );

    ImGui::PopStyleColor();

    // Show tooltip with more info
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.5f, 1.0f));
        ImGui::Text("Component Details");
        ImGui::PopStyleColor();
        ImGui::Separator();
        ImGui::Text("Name: %s", component->GetName().CStr());
        ImGui::Text("ID: %s", component->GetObjectID().c_str());
        ImGui::Text("Active: %s", isActive ? "True" : "False");
        ImGui::EndTooltip();
    }
}

///////////////////////////////////////////////////////////////////////////////
void FActorDebug::DisplayVector(const FVector3& vec)
{
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.6f, 0.6f, 1.0f));
    ImGui::Text("X: %.2f", vec.x);
    ImGui::PopStyleColor();

    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.0f));
    ImGui::Text("Y: %.2f", vec.y);
    ImGui::PopStyleColor();

    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 1.0f, 1.0f));
    ImGui::Text("Z: %.2f", vec.z);
    ImGui::PopStyleColor();
}

///////////////////////////////////////////////////////////////////////////////
void FActorDebug::DisplayRotation(const FRotator& rotator)
{
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.6f, 0.6f, 1.0f));
    ImGui::Text("Pitch: %.2f°", rotator.GetPitch());
    ImGui::PopStyleColor();

    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.0f));
    ImGui::Text("Yaw: %.2f°", rotator.GetYaw());
    ImGui::PopStyleColor();

    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 1.0f, 1.0f));
    ImGui::Text("Roll: %.2f°", rotator.GetRoll());
    ImGui::PopStyleColor();
}

#endif

}   // namespace tkd::debug
