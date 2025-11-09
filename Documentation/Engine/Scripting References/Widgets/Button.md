# UWidgetButtonComponent

## Overview

`UWidgetButtonComponent` is an interactive rectangular button widget that responds to mouse input. It renders as a solid color rectangle and provides callbacks for hover, click, and release events. Perfect for menus, forms, and interactive UI elements.

## Inheritance

```
UActorComponent
    └── UWidgetComponent
        └── UWidgetButtonComponent
```

## Features

- **Input Handling**: Built-in hover, click, and release detection
- **Visual Feedback**: Easy color changes based on button state
- **Lambda Callbacks**: Flexible event handling with lambda functions
- **Uniform Scaling**: Maintains aspect ratio when window is resized
- **State Management**: Automatic tracking of hover, pressed, and released states

## Properties

### Inherited from UWidgetComponent
- `FVector2 m_position` - Button position (1920x1080 reference)
- `FVector2 m_size` - Button dimensions (1920x1080 reference)
- `EWidgetAlignment m_alignment` - Anchor point
- `int m_z` - Rendering order
- `bool m_visible` - Visibility state
- `bool m_enabled` - Whether button responds to input

### Button-Specific
- `FColor m_color` - The button color (default: white)
- `std::function<void()> m_onClickCallback` - Callback when button is clicked
- `std::function<void()> m_onUnclickedCallback` - Callback when button is unclicked
- `std::function<void()> m_onReleasedCallback` - Callback when button is released
- `std::function<void()> m_onHeldCallback` - Callback while button is held
- `std::function<void()> m_onHoverCallback` - Callback when button is hovered

### Input States (Inherited, Read-Only)
- `bool m_isHovered` - Mouse is over the button
- `bool m_isClicked` - Button was just clicked this frame
- `bool m_isHeld` - Mouse button held down on button
- `bool m_isReleased` - Mouse released after pressing button

## Methods

### Constructor

```cpp
UWidgetButtonComponent();
```
Creates a new button with default white color.

### Setters

```cpp
void SetColor(const FColor& color);
```
Sets the button color. Use `FColor` for RGBA values (0 - 255 range).

```cpp
void SetOnClick(std::function<void()> callback);
```
Sets the function to call when the button is clicked.

```cpp
void SetOnUnclicked(std::function<void()> callback);
```
Sets the function to call when the button is unclicked (released after click).

```cpp
void SetOnReleased(std::function<void()> callback);
```
Sets the function to call when the button is released.

```cpp
void SetOnHeld(std::function<void()> callback);
```
Sets the function to call while the button is held down.

```cpp
void SetOnHover(std::function<void()> callback);
```
Sets the function to call when the button is hovered.

### Rendering

```cpp
void Render(IRenderer& renderer) const override;
```
Renders the button as a filled rectangle. Called automatically by the engine.

## Usage Examples

### Basic Button

```cpp
auto* button = CreateComponent<UWidgetButtonComponent>();
button->SetPosition(FVector2(960.0f, 540.0f));
button->SetSize(FVector2(200.0f, 60.0f));
button->SetAlignment(EWidgetAlignment::Center);
button->SetColor(FLinearColor(0.2f, 0.6f, 1.0f, 1.0f)); // Blue
button->SetZIndex(10);

// Add click handler
button->SetOnClick([this]() {
    Debug::Log("Button clicked!");
});
```

### Button with States

```cpp
auto* button = CreateComponent<UWidgetButtonComponent>();
button->SetPosition(FVector2(960.0f, 400.0f));
button->SetSize(FVector2(250.0f, 80.0f));
button->SetAlignment(EAlignment::Center);
button->SetColor(FColor(76, 76, 76, 255)); // Normal: Gray

// Change color on hover
button->SetOnHover([button]() {
    button->SetColor(FColor(128, 128, 128, 255)); // Hover: Light Gray
});

// Change color on click
button->SetOnClick([button]() {
    button->SetColor(FColor(25, 25, 25, 255)); // Pressed: Dark Gray
    Debug::Log("Button pressed!");
});

button->SetOnRelease([button]() {
    button->SetColor(FColor(76, 76, 76, 255)); // Back to normal
    Debug::Log("Button released!");
});
```

### Play Button

```cpp
// Create a play button
auto* playButton = CreateComponent<UWidgetButtonComponent>();

// Set up the button properties
playButton->SetPosition(FVector2(960.0f, 600.0f));
playButton->SetSize(FVector2(300.0f, 80.0f));
playButton->SetAlignment(EAlignment::Center);
playButton->SetColor(FLinearColor(0.0f, 0.8f, 0.0f, 1.0f));       // Green
playButton->SetHoverColor(FLinearColor(0.0f, 1.0f, 0.0f, 1.0f));  // Bright Green
playButton->SetPressedColor(FLinearColor(0.0f, 0.5f, 0.0f, 1.0f)); // Dark Green

// Add click callback
playButton->SetOnClick([this]() {
    StartGame();
});
```

### Disabled Button

```cpp
// Create a button (initially disabled)
auto* button = CreateComponent<UWidgetButtonComponent>();

// Set up disabled appearance
button->SetPosition(FVector2(960.0f, 700.0f));
button->SetSize(FVector2(200.0f, 60.0f));
button->SetAlignment(EAlignment::Center);
button->SetColor(FLinearColor(0.5f, 0.5f, 0.5f, 0.5f)); // Gray, semi-transparent
button->SetEnabled(false); // Button won't respond to input

// Enable later when conditions are met
void OnConditionMet()
{
    button->SetEnabled(true);
    button->SetColor(FLinearColor(0.0f, 0.8f, 0.0f, 1.0f)); // Green
}
```

### Button with Label

```cpp
// Create button background
auto* button = CreateComponent<UWidgetButtonComponent>();
button->SetPosition(FVector2(960.0f, 500.0f));
button->SetSize(FVector2(300.0f, 80.0f));
button->SetAlignment(EAlignment::Center);
button->SetColor(FLinearColor(0.2f, 0.4f, 0.8f, 1.0f));
button->SetZIndex(10);

// Create text label on top
auto* label = CreateComponent<UWidgetTextComponent>();
label->SetPosition(FVector2(960.0f, 500.0f));
label->SetAlignment(EAlignment::Center);
label->SetText("START GAME");
label->SetColor(FColor(255, 255, 255, 255)); // White text
label->SetZIndex(11); // Above button

// Both will scale together
button->SetOnClick([this]() {
    LoadLevel("MainLevel");
});
```

### Menu Buttons

```cpp
class MainMenu
{
private:
    void CreateButton(const String& text, const FVector2& position, 
                     std::function<void()> onClick)
    {
        // Create button component
        auto* button = CreateComponent<UWidgetButtonComponent>();
        button->SetPosition(position);
        button->SetSize(FVector2(400.0f, 70.0f));
        button->SetAlignment(EAlignment::Center);
        button->SetColor(FLinearColor(0.15f, 0.15f, 0.2f, 0.95f));
        button->SetHoverColor(FLinearColor(0.25f, 0.25f, 0.35f, 1.0f));
        button->SetPressedColor(FLinearColor(0.1f, 0.1f, 0.15f, 1.0f));
        button->SetOnClick(onClick);
        button->SetZIndex(10);
        
        // Create label component
        auto* label = CreateComponent<UWidgetTextComponent>();
        label->SetPosition(position);
        label->SetAlignment(EAlignment::Center);
        label->SetText(text);
        label->SetZIndex(11);
    }
    
public:
    void BeginPlay() override
    {
        CreateButton("New Game", FVector2(960.0f, 400.0f), [this]() {
            StartNewGame();
        });
        
        CreateButton("Load Game", FVector2(960.0f, 500.0f), [this]() {
            ShowLoadMenu();
        });
        
        CreateButton("Settings", FVector2(960.0f, 600.0f), [this]() {
            ShowSettings();
        });
        
        CreateButton("Quit", FVector2(960.0f, 700.0f), [this]() {
            QuitGame();
        });
    }
};
```

### Toggle Button

```cpp
class ToggleButton
{
private:
    UWidgetButtonComponent* button;
    bool isToggled = false;
    
public:
    void Initialize()
    {
        button = CreateComponent<UWidgetButtonComponent>();
        button->SetPosition(FVector2(200.0f, 200.0f));
        button->SetSize(FVector2(150.0f, 50.0f));
        button->SetAlignment(EWidgetAlignment::TopLeft);
        UpdateVisuals();
        
        button->SetOnClick([this]() {
            Toggle();
        });
    }
    
    void Toggle()
    {
        isToggled = !isToggled;
        UpdateVisuals();
        OnToggled(isToggled);
    }
    
    void UpdateVisuals()
    {
        if (isToggled)
        {
            button->SetColor(FLinearColor(0.0f, 0.8f, 0.0f, 1.0f)); // Green when on
        }
        else
        {
            button->SetColor(FLinearColor(0.6f, 0.6f, 0.6f, 1.0f)); // Gray when off
        }
    }
    
    virtual void OnToggled(bool state) { }
};
```

## Input State Examples

```cpp
void Tick(float deltaTime) override
{
    // Check if mouse is hovering
    if (button->IsHovered())
    {
        // Show tooltip, change cursor, etc.
        // Note: IsHovered() is true every frame the mouse is over the button
    }
    
    // Check if button was just clicked this frame
    if (button->IsClicked())
    {
        // Play click sound
        // This triggers ONCE when the mouse button goes down
        PlaySound("button_click.wav");
    }
    
    // Check if button is being held down
    if (button->IsHeld())
    {
        // Show pressed visual effect
        // This is true every frame while the mouse button stays down
        // Useful for hold-to-charge mechanics or visual feedback
    }
    
    // Check if button was released this frame
    if (button->IsReleased())
    {
        // Trigger action, play release sound
        // This triggers ONCE when the mouse button goes up
        // Different from IsUnclicked - this works even if mouse moved off button
    }
}
```

**Understanding the Input State Lifecycle:**

Here's what happens when a user clicks a button:

1. **Frame 1**: User moves mouse over button
   - `IsHovered()` = true
   - All others = false

2. **Frame 2**: User presses mouse button down
   - `IsHovered()` = true
   - `IsClicked()` = true (ONE FRAME ONLY)
   - `IsHeld()` = true

3. **Frames 3-10**: User keeps holding
   - `IsHovered()` = true (assuming mouse didn't move)
   - `IsHeld()` = true
   - `IsClicked()` = false (only true on the first frame)

4. **Frame 11**: User releases mouse button
   - `IsReleased()` = true (ONE FRAME ONLY)
   - `IsUnclicked()` = true if mouse still over button
   - All states return to false after this frame

**Practical Applications:**

- **IsClicked()**: Use for immediate feedback (play sound, start animation)
- **IsHeld()**: Use for charging mechanics or continuous effects
- **IsReleased()**: Use for confirming actions (only execute if held long enough)
- **IsUnclicked()**: Use for traditional button behavior (click and release on same button)

## Scaling Behavior

Buttons use **uniform scaling** to maintain aspect ratio:

```cpp
FVector2 scaledSize = GetUniformScale() * GetSize();
```

**Example at different resolutions:**
- Button: 300x80 (reference at 1920x1080)
- At 1920x1080: Displays as 300x80
- At 1280x720: Displays as ~200x53 (scaled uniformly by ~0.67)
- At 2560x1440: Displays as ~400x107 (scaled uniformly by ~1.33)

## Best Practices

1. **Clear Visual Feedback**: Always set different colors for normal, hover, and pressed states
2. **Appropriate Sizing**: Make buttons large enough to click easily (minimum ~150x50 reference)
3. **Sound Feedback**: Play sounds in onClick callbacks for better user experience
4. **Disabled State**: Use SetEnabled(false) and gray/transparent colors for disabled buttons
5. **Labels**: Combine buttons with UWidgetTextComponent for text labels
6. **Z-Index**: Labels should have higher z-index than the button background
7. **Touch Friendly**: Consider mobile/touch input - make buttons larger if needed
8. **Callback Safety**: Be careful with lambda captures, ensure referenced objects are valid

## Common Patterns

### Confirmation Dialog

```cpp
void ShowConfirmDialog(const String& message, 
                      std::function<void()> onConfirm,
                      std::function<void()> onCancel)
{
    // Semi-transparent overlay
    auto* overlay = CreateComponent<UWidgetPanelComponent>();
    overlay->SetSize(FVector2(1920.0f, 1080.0f));
    overlay->SetColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.7f));
    overlay->SetZIndex(100);
    
    // Dialog panel
    auto* panel = CreateComponent<UWidgetPanelComponent>();
    panel->SetPosition(FVector2(960.0f, 540.0f));
    panel->SetSize(FVector2(600.0f, 300.0f));
    panel->SetAlignment(EWidgetAlignment::Center);
    panel->SetColor(FLinearColor(0.2f, 0.2f, 0.2f, 1.0f));
    panel->SetZIndex(101);
    
    // Message text
    auto* text = CreateComponent<UWidgetTextComponent>();
    text->SetPosition(FVector2(960.0f, 480.0f));
    text->SetAlignment(EWidgetAlignment::Center);
    text->SetText(message);
    text->SetZIndex(102);
    
    // Yes button
    auto* yesButton = CreateComponent<UWidgetButtonComponent>();
    yesButton->SetPosition(FVector2(860.0f, 600.0f));
    yesButton->SetSize(FVector2(150.0f, 60.0f));
    yesButton->SetAlignment(EWidgetAlignment::Center);
    yesButton->SetColor(FLinearColor(0.0f, 0.8f, 0.0f, 1.0f));
    yesButton->SetZIndex(102);
    yesButton->SetOnClick([onConfirm, overlay, panel, text, yesButton]() {
        onConfirm();
        // Clean up dialog
        overlay->Destroy();
        panel->Destroy();
        text->Destroy();
        // ... destroy other elements
    });
    
    // No button
    auto* noButton = CreateComponent<UWidgetButtonComponent>();
    noButton->SetPosition(FVector2(1060.0f, 600.0f));
    noButton->SetSize(FVector2(150.0f, 60.0f));
    noButton->SetAlignment(EWidgetAlignment::Center);
    noButton->SetColor(FLinearColor(0.8f, 0.0f, 0.0f, 1.0f));
    noButton->SetZIndex(102);
    noButton->SetOnClick([onCancel, overlay, panel, text, noButton]() {
        onCancel();
        // Clean up dialog
    });
}
```

## See Also

- [Widget Component](Widget%20Component.md) - Base widget class with input state documentation
- [Widget Panel Component](Widget%20Panel%20Component.md) - For button backgrounds
- [Widget Text Component](Widget%20Text%20Component.md) - For button labels
- [Widget Image Component](Widget%20Image%20Component.md) - For textured buttons
