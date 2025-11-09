# UWidgetPanelComponent

## Overview

`UWidgetPanelComponent` is a simple rectangular widget that renders a solid color. It's perfect for backgrounds, overlays, containers, and basic UI elements that need a colored rectangle.

## Inheritance

```
UActorComponent
    └── UWidgetComponent
        └── UWidgetPanelComponent
```

## Features

- **Solid Color Rendering**: Displays a filled rectangle with a specified color
- **Uniform Scaling**: Maintains aspect ratio when window is resized
- **Automatic Alignment**: Uses the base widget alignment system

## Properties

### Inherited from UWidgetComponent
- `FVector2 m_position` - Panel position (1920x1080 reference)
- `FVector2 m_size` - Panel dimensions (1920x1080 reference)
- `EWidgetAlignment m_alignment` - Anchor point
- `int m_z` - Rendering order
- `bool m_visible` - Visibility state
- `bool m_enabled` - Input enabled state

### Panel-Specific
- `FColor m_color` - The color of the panel (default: white)

## Methods

### Constructor

```cpp
UWidgetPanelComponent();
```
Creates a new panel with default white color.

### Setters

```cpp
void SetColor(const FColor& color);
```
Sets the panel color. Use `FColor` for RGBA values (0 - 255 range).

```cpp
void SetColor(const FLinearColor& color);
```
Sets the panel color. Use `FLinearColor` for RGBA values (0.0 - 1.0 range).

### Rendering

```cpp
void Render(IRenderer& renderer) const override;
```
Renders the panel as a filled rectangle. Called automatically by the engine.

## Usage Example

### Basic Panel

```cpp
// Create a background panel
auto* background = CreateComponent<UWidgetPanelComponent>();
background->SetPosition(FVector2(960.0f, 540.0f));
background->SetSize(FVector2(800.0f, 600.0f));
background->SetAlignment(EWidgetAlignment::Center);
background->SetColor(FLinearColor(0.1f, 0.1f, 0.1f, 0.9f)); // Dark semi-transparent
background->SetZIndex(0);
```

### Panel as Container

```cpp
// Create a container for other widgets
auto* container = CreateComponent<UWidgetPanelComponent>();
container->SetPosition(FVector2(100.0f, 100.0f));
container->SetSize(FVector2(400.0f, 300.0f));
container->SetAlignment(EWidgetAlignment::TopLeft);
container->SetColor(FLinearColor(0.2f, 0.2f, 0.3f, 1.0f));
container->SetZIndex(5);

// Add other widgets with higher z-index to render on top
auto* text = CreateComponent<UWidgetTextComponent>();
text->SetPosition(FVector2(300.0f, 250.0f)); // Position relative to screen
text->SetZIndex(6); // Higher than container
```

### Overlay

```cpp
// Create a full-screen overlay
auto* overlay = CreateComponent<UWidgetPanelComponent>();
overlay->SetPosition(FVector2(960.0f, 540.0f));
overlay->SetSize(FVector2(1920.0f, 1080.0f));
overlay->SetAlignment(EWidgetAlignment::Center);
overlay->SetColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.5f)); // Black 50% transparent
overlay->SetZIndex(100); // High z-index to cover everything
overlay->SetVisible(false); // Hidden by default

// Show overlay when needed
void ShowPauseMenu()
{
    overlay->SetVisible(true);
    // Show other pause menu widgets...
}
```

### Color Examples

```cpp
// Create a panel first
auto* panel = CreateComponent<UWidgetPanelComponent>();
panel->SetPosition(FVector2(960.0f, 540.0f));
panel->SetSize(FVector2(400.0f, 300.0f));
panel->SetAlignment(EAlignment::Center);

// Using FColor (0 - 255 range) - Primary method
panel->SetColor(FColor(255, 0, 0));     // Red
panel->SetColor(FColor(0, 255, 0));     // Green
panel->SetColor(FColor(0, 0, 255));     // Blue
panel->SetColor(FColor(255, 255, 255)); // White

// Using FLinearColor (0.0 - 1.0 range) - Alternative method
panel->SetColor(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f)); // Red
panel->SetColor(FLinearColor(0.0f, 1.0f, 0.0f, 1.0f)); // Green
panel->SetColor(FLinearColor(0.0f, 0.0f, 1.0f, 1.0f)); // Blue
panel->SetColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.5f)); // White 50% transparent
```

## Scaling Behavior

The panel uses **uniform scaling** to maintain its aspect ratio:

```cpp
// In Tick()
FVector2 scaledSize = GetUniformScale() * GetSize();
```

This means a square panel stays square, and a 2:1 rectangle stays 2:1. Here's what this means in practice:

**Resolution Scaling Examples:**
- **At 1920x1080** (reference resolution): A panel sized 400x300 displays at exactly 400x300 pixels
- **At 1280x720** (0.667x scale): The same panel displays at ~267x200 pixels - still 4:3 ratio
- **At 2560x1440** (1.333x scale): The panel displays at ~533x400 pixels - still 4:3 ratio
- **At 3840x2160** (2.0x scale): The panel displays at 800x600 pixels - perfect 2x, still 4:3

**Why Uniform Scaling for Panels?**

While panels could use independent X/Y scaling like the base widget system supports, uniform scaling is chosen for visual consistency. A dialog box or button should look similar (just larger or smaller) across different screens. If panels stretched to fill different aspect ratios, a square button on 16:9 would become a rectangle on 21:9 ultrawide, which looks unprofessional.

**When This Matters:**

If you need a panel to stretch horizontally but not vertically (like a top bar), you have two options:
1. Use multiple thin panels side-by-side
2. Override the scaling in your derived class by using `GetScaledSize()` instead of uniform scaling

```cpp
// Custom stretching panel (advanced)
class UStretchingPanelComponent : public UWidgetPanelComponent
{
    void Tick(Float32 deltaTime) override
    {
        FVector2 scaledSize = GetScaledSize(); // Independent X/Y scaling
        // Rest of rendering code...
    }
};
```

## Rendering Details

The panel is rendered using the engine's shape renderer:
1. Calculates scaled position and size based on current window dimensions
2. Computes origin point based on alignment
3. Draws a filled rectangle at the final position

## Best Practices

1. **Use as Backgrounds**: Great for creating containers and backgrounds for other widgets
2. **Z-Index Organization**: Set lower z-index for background panels, higher for overlays
3. **Semi-Transparent Overlays**: Use alpha < 1.0 for modal dialogs and pause menus
4. **Reference Resolution**: Design panels for 1920x1080, they'll scale automatically
5. **Solid Colors Only**: For textured backgrounds, use `UWidgetImageComponent` instead

## Common Use Cases

- **HUD Backgrounds**: Container panels behind score displays, health bars, etc.
- **Modal Dialogs**: Semi-transparent overlay + foreground panel
- **Menu Containers**: Background panels for menus and forms
- **Progress Bars**: Combine two panels (background + foreground) to create progress indicators
- **Dividers**: Thin panels to separate UI sections

## Example: Progress Bar

```cpp
## Example: Progress Bar

```cpp
// Background (full bar)
auto* progressBg = CreateComponent<UWidgetPanelComponent>();
progressBg->SetPosition(FVector2(960.0f, 100.0f));
progressBg->SetSize(FVector2(400.0f, 30.0f));
progressBg->SetAlignment(EAlignment::TopCenter);
progressBg->SetColor(FLinearColor(0.2f, 0.2f, 0.2f, 1.0f)); // Dark gray
progressBg->SetZIndex(10);

// Foreground (filled portion)
auto* progressFg = CreateComponent<UWidgetPanelComponent>();
progressFg->SetPosition(FVector2(760.0f, 100.0f)); // Left-aligned
progressFg->SetSize(FVector2(0.0f, 30.0f)); // Start at 0 width
progressFg->SetAlignment(EAlignment::TopLeft);
progressFg->SetColor(FLinearColor(0.0f, 1.0f, 0.0f, 1.0f)); // Green
progressFg->SetZIndex(11);

// Update progress (0.0 to 1.0)
void SetProgress(float progress)
{
    float width = 400.0f * progress;
    progressFg->SetSize(FVector2(width, 30.0f));
    
    // Optional: Change color based on progress
    if (progress < 0.3f)
        progressFg->SetColor(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f)); // Red (low)
    else if (progress < 0.6f)
        progressFg->SetColor(FLinearColor(1.0f, 1.0f, 0.0f, 1.0f)); // Yellow (medium)
    else
        progressFg->SetColor(FLinearColor(0.0f, 1.0f, 0.0f, 1.0f)); // Green (high)
}
```

**Important Note About Progress Bars:**

The progress bar foreground should use `TopLeft` alignment, not `TopCenter`. This ensures that as you change the width, the bar fills from left to right. If you used `TopCenter`, the bar would grow from the center outward, which looks wrong for a progress indicator.

Also, keep the position calculation in sync:
```cpp
// Background centered at 960 with width 400
// Left edge: 960 - (400/2) = 760
// This is why progressFg starts at x=760
```
```

## See Also

- [Widget Component](Widget%20Component.md) - Base widget class documentation
- [Widget Image Component](Widget%20Image%20Component.md) - For textured backgrounds
- [Widget Button Component](Widget%20Button%20Component.md) - For interactive elements
