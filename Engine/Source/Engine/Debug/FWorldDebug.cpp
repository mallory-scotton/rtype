///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Debug/FWorldDebug.hpp>
#include <Engine/Runtime/Core/AGameMode.hpp>
#include <Engine/Runtime/World/ULevel.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::debug
///////////////////////////////////////////////////////////////////////////////
namespace tkd::debug
{

#if TKD_ENGINE_CLIENT

///////////////////////////////////////////////////////////////////////////////
void FWorldDebug::Show(const FEngineSettings& settings, UWorld* world)
{
    TKD_UNUSED(settings);

    if (world == nullptr) { return; }

    ImGui::Begin("World Debug", nullptr);

    // Header with styling
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 1.0f, 0.8f, 1.0f));
    ImGui::Text("World Information Monitor");
    ImGui::PopStyleColor();

    const auto& loadedLevels = world->GetLoadedLevels();
    const auto& actors = world->GetActors();

    ImGui::Text(
        "World Time: %.2f | Loaded Levels: %lu | Active Actors: %lu",
        world->GetWorldTime(),
        loadedLevels.size(),
        actors.size()
    );
    ImGui::Separator();
    ImGui::Spacing();

    // Create tab bar for different views
    if (ImGui::BeginTabBar("WorldDebugTabs", ImGuiTabBarFlags_None))
    {
        if (ImGui::BeginTabItem("Overview"))
        {
            ShowWorldOverview(world);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Current Level"))
        {
            ShowCurrentLevelDetails(world);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("All Levels"))
        {
            ShowLevelsList(world);
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}

///////////////////////////////////////////////////////////////////////////////
void FWorldDebug::ShowWorldOverview(UWorld* world)
{
    ImGui::Spacing();

    // World general information
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.2f, 1.0f));
    ImGui::Text("World Status");
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::Text("World Name: %s", world->GetName().CStr());
    ImGui::Text("World Time: %.2f seconds", world->GetWorldTime());
    ImGui::Text("Active Actors: %lu", world->GetActors().size());

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Current level quick info
    ULevel* currentLevel = world->GetCurrentLevel();
    if (currentLevel != nullptr)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 1.0f, 0.8f, 1.0f));
        ImGui::Text("Current Level");
        ImGui::PopStyleColor();
        ImGui::Separator();

        ImGui::Text("Level Name: %s", currentLevel->GetLevelName().CStr());
        ImGui::Text(
            "Actor Entries: %lu", currentLevel->GetActorEntries().Size()
        );

        const AGameMode& gameMode = currentLevel->GetGameMode();
        ImGui::Text("Game Mode: %s", gameMode.GetName().CStr());
        ImGui::Text("Game Mode Valid: %s", gameMode.IsValid() ? "Yes" : "No");

        if (gameMode.IsValid())
        {
            ImGui::Text(
                "Actor Class: %s", gameMode.GetActorClassName().CStr()
            );
            ImGui::Text("Players: %u", gameMode.GetPlayerCount());
        }
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.4f, 0.4f, 1.0f));
        ImGui::Text("No current level loaded");
        ImGui::PopStyleColor();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Loaded levels summary
    const auto& loadedLevels = world->GetLoadedLevels();

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.2f, 1.0f));
    ImGui::Text("Loaded Levels Summary");
    ImGui::PopStyleColor();
    ImGui::Separator();

    if (loadedLevels.size() == 0)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
        ImGui::Text("No levels loaded");
        ImGui::PopStyleColor();
    }
    else
    {
        ImGui::Text("Total Loaded Levels: %lu", loadedLevels.size());

        ImGui::Spacing();

        // Display a table of loaded levels
        if (ImGui::BeginTable(
                "LoadedLevelsTable",
                4,
                ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                    ImGuiTableFlags_Resizable
            ))
        {
            ImGui::TableSetupColumn("Level Name");
            ImGui::TableSetupColumn("Actor Entries");
            ImGui::TableSetupColumn("Game Mode");
            ImGui::TableSetupColumn("Current");
            ImGui::TableHeadersRow();

            for (size_t i = 0; i < loadedLevels.size(); ++i)
            {
                const ULevel& level =
                    const_cast<std::vector<ULevel>&>(loadedLevels)[i];
                bool isCurrent =
                    (currentLevel != nullptr &&
                     level.GetLevelName() == currentLevel->GetLevelName());

                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", level.GetLevelName().CStr());

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%lu", level.GetActorEntries().Size());

                ImGui::TableSetColumnIndex(2);
                const AGameMode& gameMode = level.GetGameMode();
                ImGui::Text("%s", gameMode.GetName().CStr());

                ImGui::TableSetColumnIndex(3);
                if (isCurrent)
                {
                    ImGui::PushStyleColor(
                        ImGuiCol_Text, ImVec4(0.2f, 1.0f, 0.4f, 1.0f)
                    );
                    ImGui::Text("Yes");
                    ImGui::PopStyleColor();
                }
                else { ImGui::Text("No"); }
            }

            ImGui::EndTable();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void FWorldDebug::ShowCurrentLevelDetails(UWorld* world)
{
    ImGui::Spacing();

    ULevel* currentLevel = world->GetCurrentLevel();
    if (currentLevel == nullptr)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.4f, 0.4f, 1.0f));
        ImGui::Text("No current level loaded");
        ImGui::PopStyleColor();
        return;
    }

    DisplayLevelInfo(*currentLevel, true);
}

///////////////////////////////////////////////////////////////////////////////
void FWorldDebug::ShowLevelsList(UWorld* world)
{
    ImGui::Spacing();

    const auto& loadedLevels = world->GetLoadedLevels();

    if (loadedLevels.size() == 0)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
        ImGui::Text("No levels loaded");
        ImGui::PopStyleColor();
        return;
    }

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.2f, 1.0f));
    ImGui::Text("Total Loaded Levels: %lu", loadedLevels.size());
    ImGui::PopStyleColor();
    ImGui::Spacing();

    // Create a list of levels
    ImGui::BeginChild(
        "LevelsListPane",
        ImVec2(250, 0),
        true,
        ImGuiWindowFlags_HorizontalScrollbar
    );

    ULevel* currentLevel = world->GetCurrentLevel();

    for (size_t i = 0; i < loadedLevels.size(); ++i)
    {
        const ULevel& level =
            const_cast<std::vector<ULevel>&>(loadedLevels)[i];
        bool isCurrent =
            (currentLevel != nullptr &&
             level.GetLevelName() == currentLevel->GetLevelName());

        // Highlight current level
        if (isCurrent)
        {
            ImGui::PushStyleColor(
                ImGuiCol_Text, ImVec4(0.2f, 1.0f, 0.4f, 1.0f)
            );
        }

        FString label = level.GetLevelName() + (isCurrent ? " (Current)" : "");
        if (ImGui::Selectable(label.CStr(), m_selectedLevelIndex == (int)i))
        {
            m_selectedLevelIndex = (int)i;
        }

        if (isCurrent) { ImGui::PopStyleColor(); }
    }

    ImGui::EndChild();

    // Display details of selected level
    ImGui::SameLine();

    ImGui::BeginChild("LevelDetailsPane");

    if (m_selectedLevelIndex >= 0 &&
        m_selectedLevelIndex < (int)loadedLevels.size())
    {
        ULevel& level = const_cast<std::vector<ULevel>&>(loadedLevels
        )[m_selectedLevelIndex];
        bool isCurrent =
            (currentLevel != nullptr &&
             level.GetLevelName() == currentLevel->GetLevelName());

        DisplayLevelInfo(level, isCurrent);
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
        ImGui::Text("Select a level to view details");
        ImGui::PopStyleColor();
    }

    ImGui::EndChild();
}

///////////////////////////////////////////////////////////////////////////////
void FWorldDebug::DisplayLevelInfo(ULevel& level, bool isCurrentLevel)
{
    // Level header
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 1.0f, 0.8f, 1.0f));
    ImGui::Text("Level: %s", level.GetLevelName().CStr());
    ImGui::PopStyleColor();

    if (isCurrentLevel)
    {
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 1.0f, 0.4f, 1.0f));
        ImGui::Text("(Current)");
        ImGui::PopStyleColor();
    }

    ImGui::Separator();
    ImGui::Spacing();

    // Level basic info
    ImGui::Text("Object Name: %s", level.GetName().CStr());
    ImGui::Text("Level Name: %s", level.GetLevelName().CStr());
    ImGui::Text("Actor Entries: %lu", level.GetActorEntries().Size());

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Game mode information
    const AGameMode& gameMode = level.GetGameMode();
    DisplayGameModeInfo(gameMode);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Actor entries
    DisplayActorEntries(level);
}

///////////////////////////////////////////////////////////////////////////////
void FWorldDebug::DisplayGameModeInfo(const AGameMode& gameMode)
{
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.6f, 1.0f, 1.0f));
    ImGui::Text("Game Mode Information");
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::Text("Name: %s", gameMode.GetName().CStr());
    ImGui::Text("Valid: %s", gameMode.IsValid() ? "Yes" : "No");

    if (!gameMode.IsValid())
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.6f, 0.2f, 1.0f));
        ImGui::Text("Game mode is not properly initialized");
        ImGui::PopStyleColor();
        return;
    }

    ImGui::Spacing();

    // Game mode settings
    if (ImGui::TreeNode("Settings"))
    {
        ImGui::Text("Actor Class: %s", gameMode.GetActorClassName().CStr());
        ImGui::Text(
            "Player Controller: %s",
            gameMode.GetPlayerControllerClassName().CStr()
        );
        ImGui::Text("Game State: %s", gameMode.GetGameStateClassName().CStr());
        ImGui::Text("HUD Class: %s", gameMode.GetHUDClassName().CStr());
        ImGui::Text(
            "Spectator Class: %s", gameMode.GetSpectatorClassName().CStr()
        );
        ImGui::Text(
            "Default Player Name: %s", gameMode.GetDefaultPlayerName().CStr()
        );

        ImGui::Spacing();
        ImGui::Separator();

        ImGui::Text("Number of Players: %u", gameMode.GetPlayerCount());
        ImGui::Text(
            "Min Respawn Delay: %.2f", gameMode.GetMinimumRespawnDelay()
        );
        ImGui::Text(
            "Max Respawn Delay: %.2f", gameMode.GetMaximumRespawnDelay()
        );
        ImGui::Text("Gravity Z: %.2f", gameMode.GetGravityZ());
        ImGui::Text("World to Meters: %.2f", gameMode.GetWorldToMeters());

        ImGui::TreePop();
    }
}

///////////////////////////////////////////////////////////////////////////////
void FWorldDebug::DisplayActorEntries(const ULevel& level)
{
    const auto& actorEntries = level.GetActorEntries();

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.2f, 1.0f));
    ImGui::Text("Actor Entries");
    ImGui::PopStyleColor();
    ImGui::Separator();

    if (actorEntries.Size() == 0)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
        ImGui::Text("No actor entries in this level");
        ImGui::PopStyleColor();
        return;
    }

    ImGui::Text("Total Entries: %lu", actorEntries.Size());
    ImGui::Spacing();

    // Display actors in a scrollable region
    ImGui::BeginChild(
        "ActorEntriesScroll",
        ImVec2(0, 0),
        false,
        ImGuiWindowFlags_HorizontalScrollbar
    );

    for (size_t i = 0; i < actorEntries.Size(); ++i)
    {
        DisplayActorEntry(actorEntries[i], i);

        if (i < actorEntries.Size() - 1)
        {
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
        }
    }

    ImGui::EndChild();
}

///////////////////////////////////////////////////////////////////////////////
void FWorldDebug::DisplayActorEntry(
    const ULevel::ActorEntry& entry, SizeT index
)
{
    FString headerLabel = FString("Actor #") + FString::ToString(index) +
                          ": " + entry.name + " (" + entry.class_name + ")";

    if (ImGui::TreeNode(headerLabel.CStr()))
    {
        // Status
        if (entry.isActive)
        {
            ImGui::PushStyleColor(
                ImGuiCol_Text, ImVec4(0.2f, 1.0f, 0.4f, 1.0f)
            );
            ImGui::Text("Status: Active");
            ImGui::PopStyleColor();
        }
        else
        {
            ImGui::PushStyleColor(
                ImGuiCol_Text, ImVec4(0.8f, 0.4f, 0.4f, 1.0f)
            );
            ImGui::Text("Status: Inactive");
            ImGui::PopStyleColor();
        }

        // Basic info
        ImGui::Text("Class: %s", entry.class_name.CStr());
        ImGui::Text("Name: %s", entry.name.CStr());

        ImGui::Spacing();

        // Transform information
        if (ImGui::TreeNode("Transform"))
        {
            ImGui::Text(
                "Position: (%.2f, %.2f, %.2f)",
                entry.position.x,
                entry.position.y,
                entry.position.z
            );
            ImGui::Text(
                "Rotation: (%.2f, %.2f, %.2f)",
                entry.rotation.GetPitch(),
                entry.rotation.GetYaw(),
                entry.rotation.GetRoll()
            );
            ImGui::Text(
                "Scale: (%.2f, %.2f, %.2f)",
                entry.scale.x,
                entry.scale.y,
                entry.scale.z
            );

            ImGui::TreePop();
        }

        // Properties
        if (entry.properties.Size() > 0)
        {
            DisplayActorProperties(entry.properties);
        }

        ImGui::TreePop();
    }
}

///////////////////////////////////////////////////////////////////////////////
void FWorldDebug::DisplayActorProperties(
    const TVector<ULevel::PropertyEntry>& properties
)
{
    FString propertiesLabel =
        FString("Properties (") + FString::ToString(properties.Size()) + ")";

    if (ImGui::TreeNode(propertiesLabel.CStr()))
    {
        if (ImGui::BeginTable(
                "PropertiesTable",
                3,
                ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                    ImGuiTableFlags_Resizable
            ))
        {
            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("Size (bytes)");
            ImGui::TableSetupColumn("Value (hex preview)");
            ImGui::TableHeadersRow();

            for (const auto& prop: properties)
            {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", prop.name.CStr());

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%lu", prop.size);

                ImGui::TableSetColumnIndex(2);
                // Display first few bytes as hex
                FString hexPreview = "";
                size_t previewBytes = std::min(prop.value.size(), (size_t)8);
                for (size_t i = 0; i < previewBytes; ++i)
                {
                    char hexBuf[4];
                    snprintf(
                        hexBuf,
                        sizeof(hexBuf),
                        "%02X ",
                        (unsigned char)prop.value[i]
                    );
                    hexPreview += hexBuf;
                }
                if (prop.value.size() > previewBytes) { hexPreview += "..."; }
                ImGui::Text("%s", hexPreview.CStr());
            }

            ImGui::EndTable();
        }

        ImGui::TreePop();
    }
}

#endif

}   // namespace tkd::debug
