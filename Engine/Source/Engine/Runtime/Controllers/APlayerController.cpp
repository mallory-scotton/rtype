///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Runtime/Controllers/APlayerController.hpp>
#include <Engine/Runtime/Controllers/AController.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
APlayerController::APlayerController(const FString& name)
    : AController(name)
    , m_inputManager(nullptr)
{}

///////////////////////////////////////////////////////////////////////////////
APlayerController::~APlayerController() { ClearInputBindings(); }

///////////////////////////////////////////////////////////////////////////////
void APlayerController::SetInputManager(FInputManager* inputManager)
{
    if (m_inputManager == inputManager) { return; }

    // Clear old bindings if changing input manager
    if (m_inputManager != nullptr) { ClearInputBindings(); }

    m_inputManager = inputManager;

    // Setup input bindings for this controller
    if (m_inputManager != nullptr) { SetupInputBindings(); }
}

///////////////////////////////////////////////////////////////////////////////
FInputManager* APlayerController::GetInputManager(void) const
{
    return m_inputManager;
}

///////////////////////////////////////////////////////////////////////////////
bool APlayerController::BindActionPressed(
    const FString& actionName, ActionCallback callback
)
{
    if (m_inputManager == nullptr) { return false; }

    const UInputAction* action = m_inputManager->GetAction(actionName);
    if (action == nullptr) { return false; }

    // Store the callback
    m_actionPressedCallbacks[actionName].PushBack(callback);

    // Subscribe to the action's Pressed event
    ListenerHandle listenerId =
        const_cast<UInputAction*>(action)->On<UInputAction::Events::Pressed>(
            [callback](const UInputAction::Events::Pressed& event)
            { callback(event.input); }
        );

    m_actionListenerIds[actionName].PushBack(listenerId);

    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool APlayerController::BindActionReleased(
    const FString& actionName, ActionCallback callback
)
{
    if (m_inputManager == nullptr) { return false; }

    const UInputAction* action = m_inputManager->GetAction(actionName);
    if (action == nullptr) { return false; }

    // Store the callback
    m_actionReleasedCallbacks[actionName].PushBack(callback);

    // Subscribe to the action's Released event
    ListenerHandle listenerId =
        const_cast<UInputAction*>(action)->On<UInputAction::Events::Released>(
            [callback](const UInputAction::Events::Released& event)
            { callback(event.input); }
        );

    m_actionListenerIds[actionName].PushBack(listenerId);

    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool APlayerController::BindActionHeld(
    const FString& actionName, ActionCallback callback
)
{
    if (m_inputManager == nullptr) { return false; }

    const UInputAction* action = m_inputManager->GetAction(actionName);
    if (action == nullptr) { return false; }

    // Store the callback
    m_actionHeldCallbacks[actionName].PushBack(callback);

    // Subscribe to the action's Held event
    ListenerHandle listenerId =
        const_cast<UInputAction*>(action)->On<UInputAction::Events::Held>(
            [callback](const UInputAction::Events::Held& event)
            { callback(event.input); }
        );

    m_actionListenerIds[actionName].PushBack(listenerId);

    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool APlayerController::BindAxis(
    const FString& axisName, AxisCallback callback
)
{
    if (m_inputManager == nullptr) { return false; }

    const UInputAxis* axis = m_inputManager->GetAxis(axisName);
    if (axis == nullptr) { return false; }

    // Store the callback
    m_axisCallbacks[axisName].PushBack(callback);

    // Subscribe to the axis's Changed event
    ListenerHandle listenerId =
        const_cast<UInputAxis*>(axis)->On<UInputAxis::Events::Changed>(
            [callback](const UInputAxis::Events::Changed& event)
            { callback(event.value); }
        );

    m_axisListenerIds[axisName].PushBack(listenerId);

    return true;
}

///////////////////////////////////////////////////////////////////////////////
void APlayerController::SetupInputBindings(void)
{
    // Base implementation does nothing
    // Override this in derived classes to set up your input bindings
}

///////////////////////////////////////////////////////////////////////////////
void APlayerController::ClearInputBindings(void)
{
    if (m_inputManager == nullptr) { return; }

    // Unsubscribe from all action events
    for (const auto& [actionName, listenerIds]: m_actionListenerIds)
    {
        const UInputAction* action = m_inputManager->GetAction(actionName);
        if (action != nullptr)
        {
            for (size_t listenerId: listenerIds)
            {
                const_cast<UInputAction*>(action)->Off(listenerId);
            }
        }
    }

    // Unsubscribe from all axis events
    for (const auto& [axisName, listenerIds]: m_axisListenerIds)
    {
        const UInputAxis* axis = m_inputManager->GetAxis(axisName);
        if (axis != nullptr)
        {
            for (size_t listenerId: listenerIds)
            {
                const_cast<UInputAxis*>(axis)->Off(listenerId);
            }
        }
    }

    // Clear all stored data
    m_actionPressedCallbacks.clear();
    m_actionReleasedCallbacks.clear();
    m_actionHeldCallbacks.clear();
    m_axisCallbacks.clear();
    m_actionListenerIds.clear();
    m_axisListenerIds.clear();
}

}   // namespace tkd
