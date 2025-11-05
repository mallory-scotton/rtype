///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Core/Menu/BP_MainMenuController.hpp>
#include <Engine/Static/FWindowInterface.cpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
BP_MainMenuController::BP_MainMenuController(void)
    : AActor()
    , m_inputManager(nullptr)
{}

///////////////////////////////////////////////////////////////////////////////
void BP_MainMenuController::BeginPlay(void)
{
    Super::BeginPlay();

    // Get input manager reference (raw pointer owned by FWindowInterface)
    m_inputManager = Window::GetInputManager();
}

///////////////////////////////////////////////////////////////////////////////
void BP_MainMenuController::Tick(Float32 deltaTime)
{
    Super::Tick(deltaTime);

    if (!m_inputManager) { return; }

    // Check if spacebar is pressed using the Fire action
    auto* fireAction = m_inputManager->GetAction("Fire");
    if (fireAction && fireAction->IsPressed())
    {
        // Change to lobby level
        World::WithWorld([](UWorld& world)
                         { world.ChangeLevel("L_RType_Lobby"); });
    }
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_MainMenuController, AActor)

}   // namespace tkd
