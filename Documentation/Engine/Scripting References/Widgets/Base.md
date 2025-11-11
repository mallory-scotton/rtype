# UWidgetComponent

## Overview

`UWidgetComponent` is the base class for all UI widgets in the TKD Engine. It provides core functionality for positioning, scaling, alignment, input handling, and rendering of UI elements. All specific widget types (Panel, Image, Button, Text, Textbox) inherit from this base class.

## Inheritance

```
UActorComponent
    └── UWidgetComponent
        ├── UWidgetPanelComponent
        ├── UWidgetImageComponent
        ├── UWidgetButtonComponent
        ├── UWidgetTextComponent
        └── UWidgetTextboxComponent
```

## Features

- **Automatic Scaling**: Widgets automatically scale based on window size relative to a 1920x1080 reference resolution
- **9-Point Alignment System**: Align widgets to any of 9 anchor points (corners, edges, center)
- **Input Handling**: Built-in hover, click, and focus detection
- **Z-Index Ordering**: Control rendering order with z-index values
- **Visibility & Enabled States**: Control widget display and interaction independently

## Properties

### Position & Size
- `FVector2 m_position` - Widget position in reference resolution coordinates (1920x1080)
- `FVector2 m_size` - Widget size in reference resolution coordinates
- `FVector2 m_scale` - Additional scale multiplier (default: 1.0, 1.0)

### Layout
- `EAlignment m_alignment` - Anchor point for positioning (default: Center)
  - `None` - No alignment, use exact position
  - `TopLeft`, `TopCenter`, `TopRight`
  - `CenterLeft`, `Center`, `CenterRight`
  - `BottomLeft`, `BottomCenter`, `BottomRight`
- `Int32 m_zIndex` - Z-index for rendering order (higher values render on top)

### State
- `bool m_visible` - Whether the widget is rendered (default: true)
- `bool m_enabled` - Whether the widget responds to input (default: true)

### Input State (Read-Only)
- `bool m_isHovered` - Mouse is over the widget
- `bool m_isClicked` - Mouse button pressed on widget this frame
- `bool m_isUnclicked` - Mouse button released on widget this frame
- `bool m_isHeld` - Mouse button held down on widget
- `bool m_isReleased` - Mouse button released (anywhere) while widget was held
- `bool m_isFocused` - Widget has keyboard focus

## Scaling System

The widget system uses a reference resolution of **1920x1080** (Full HD) and automatically scales all widgets to match the actual window size. This means you design your UI once for 1920x1080, and it will work correctly on any screen size - from 720p to 4K and beyond.

### Why Reference Resolution?

When you position a widget at `(960, 540)` with size `(200, 100)`, you're working in a predictable coordinate space. The engine then calculates scale factors based on the actual window dimensions:
- On a 1920x1080 window: Your widget appears exactly as specified
- On a 1280x720 window: Everything scales down by ~67% (720/1080)
- On a 3840x2160 window: Everything scales up by ~200% (2160/1080)

This automatic scaling ensures your UI remains usable and proportional regardless of the player's screen resolution.

### Scaling Methods

```cpp
FVector2 GetScaledPosition() const;
```
Returns the widget position scaled to the current window size. X and Y are scaled independently.

```cpp
FVector2 GetScaledSize() const;
```
Returns the widget size scaled to the current window size. X and Y are scaled independently.

```cpp
float GetUniformScale() const;
```
Returns the average of X and Y scale factors. Use this for elements that need to maintain aspect ratio (text, uniform shapes, etc.).

**Scale Calculation:**
```
scaleX = windowWidth / 1920.0f
scaleY = windowHeight / 1080.0f
uniformScale = (scaleX + scaleY) * 0.5f
```

**When to use which scaling:**
- **GetScaledPosition() & GetScaledSize()**: Use for layout containers, panels, and rectangular backgrounds. These scale X and Y independently, which means a 16:9 widget at 1920x1080 will maintain its screen coverage on a 21:9 ultrawide monitor, but the aspect ratio of the widget itself may change. This is ideal for backgrounds and containers that should stretch to fill space.

- **GetUniformScale()**: Use for visual elements that need to maintain their aspect ratio - text, icons, circular buttons, or any content where distortion would look wrong. The uniform scale factor ensures a circle stays circular and text remains readable, though it may not fill the exact same screen percentage on different aspect ratios.

**Practical Example:**
```cpp
// A health bar background should stretch
FVector2 barSize = GetScaledSize(); // Uses independent X/Y scaling

// But the heart icon next to it should maintain its shape
float iconScale = GetUniformScale();
FVector2 iconSize = FVector2(32.0f, 32.0f) * iconScale; // Stays square
```

## Methods

### Virtual Methods (Override in Derived Classes)

```cpp
virtual void BeginPlay() override;
```
Called when the widget is first created. Initialize widget-specific state here.

```cpp
virtual void Tick(float deltaTime) override;
```
Called every frame. Update widget state and prepare for rendering here.

```cpp
virtual void Render() const;
```
Called during the render pass. Draw the widget using the Renderer interface.

### Input Detection

```cpp
bool ContainsPoint(const FVector2& point) const;
```
Returns true if the given point (in screen coordinates) is inside the widget's bounds. Automatically handles scaled coordinates.

## Usage Example

```cpp
// Create a custom widget
class UMyWidget : public UWidgetComponent
{
    UCLASS(UMyWidget, UWidgetComponent)
    
public:
    void BeginPlay() override
    {
        // Set position to center of screen
        SetPosition(FVector2(960.0f, 540.0f));
        SetAlignment(EAlignment::Center);
        
        // Set size
        SetSize(FVector2(400.0f, 300.0f));
        
        // Set rendering order
        SetZIndex(10);
    }
    
    void Tick(float deltaTime) override
    {
        // Check input states
        if (IsHovered())
        {
            // Mouse is over widget
        }
        
        if (IsClicked())
        {
            // Widget was just clicked
        }
    }
    
    void Render(IRenderer& renderer) const override
    {
        if (!IsVisible()) return;
        
        // Use GetScaledPosition() and GetScaledSize() for rendering
        FVector2 pos = GetScaledPosition();
        FVector2 size = GetScaledSize();
        
        // Draw your widget...
    }
};
```

## Alignment Examples

```cpp
// Create a widget (could be any widget type)
auto* widget = CreateComponent<UWidgetPanelComponent>();
widget->SetSize(FVector2(200.0f, 100.0f));

// Top-left corner
widget->SetPosition(FVector2(100.0f, 100.0f));
widget->SetAlignment(EAlignment::TopLeft);

// Center of screen
widget->SetPosition(FVector2(960.0f, 540.0f));
widget->SetAlignment(EAlignment::Center);

// Bottom-right corner
widget->SetPosition(FVector2(1820.0f, 980.0f));
widget->SetAlignment(EAlignment::BottomRight);
```

## Best Practices

1. **Use Reference Resolution**: Design your UI for 1920x1080, it will automatically scale. Don't try to query the window size and adjust positions manually - the scaling system handles this for you.

2. **Uniform Scaling for Aspect Ratio**: Use `GetUniformScale()` for text and elements that need consistent proportions. A common mistake is using `GetScaledSize()` for character sizes, which causes text to appear stretched on ultrawide monitors.

3. **Independent Scaling for Layouts**: Use `GetScaledPosition()` and `GetScaledSize()` for containers and layout elements. These should adapt to fill available space.

4. **Z-Index Organization**: Reserve ranges for different UI layers:
   - 0-99: Background images and decorative elements
   - 100-199: HUD elements (health bars, score displays)
   - 200-299: Menus and dialogs
   - 300+: Tooltips and critical overlays
   
   This makes it easy to ensure proper layering without conflicts.

5. **Client-Only**: Widgets are client-side only. Always wrap window/input code with `#if TKD_ENGINE_CLIENT` preprocessor guards to prevent compilation errors in server builds.

6. **Alignment Strategy**: Use alignment for responsive positioning:
   ```cpp
   // Instead of hardcoding positions relative to edges:
   // BAD: position = FVector2(windowWidth - 100, 50);
   
   // Use alignment to anchor to corners/edges:
   // GOOD:
   widget->SetPosition(FVector2(100.0f, 50.0f));
   widget->SetAlignment(EAlignment::TopRight);
   // Widget stays 100 pixels from right edge at any resolution
   ```

7. **Performance**: Widget input checking happens every frame. For complex UIs with many widgets, consider:
   - Disabling widgets that aren't visible (`SetVisible(false)`)
   - Disabling input on background widgets (`SetEnabled(false)`)
   - Grouping related widgets to reduce individual component overhead

## See Also

- [Widget Panel Component](Widget%20Panel%20Component.md) - Simple colored rectangle widget
- [Widget Image Component](Widget%20Image%20Component.md) - Textured image widget
- [Widget Button Component](Widget%20Button%20Component.md) - Interactive button widget
- [Widget Text Component](Widget%20Text%20Component.md) - Text display widget
- [Widget Textbox Component](Widget%20Textbox%20Component.md) - Text input widget
