# UWidgetImageComponent

## Overview

`UWidgetImageComponent` displays textured images in the UI. It supports automatic aspect-ratio preservation with two scaling modes: **Fit** (no cropping) and **Cover** (fills area, may crop). Full-screen images automatically use cover mode to eliminate letterboxing.

## Inheritance

```
UActorComponent
    └── UWidgetComponent
        └── UWidgetImageComponent
```

## Features

- **Texture Rendering**: Display any loaded texture asset
- **Aspect-Ratio Preservation**: Images maintain their proportions when scaled
- **Dual Scaling Modes**: 
  - **Fit Mode**: Scales to fit within bounds (no cropping, may letterbox)
  - **Cover Mode**: Scales to fill bounds (crops to fill, no letterbox)
- **Auto Full-Screen Detection**: Images ≥1920x1080 automatically use cover mode
- **Auto-Sizing**: If size is not set, uses texture dimensions

## Properties

### Inherited from UWidgetComponent
- `FVector2 m_position` - Image position (1920x1080 reference)
- `FVector2 m_size` - Image display dimensions (1920x1080 reference)
- `EWidgetAlignment m_alignment` - Anchor point
- `int m_z` - Rendering order
- `bool m_visible` - Visibility state

### Image-Specific
- `FilePath m_texturePath` - Path to the texture file
- `FColor m_color` - Tint color applied to the sprite

## Scaling Modes

```cpp
enum class EScaleMode
{
    Fit,    // Scale to fit (min scale) - entire image visible, may letterbox
    Cover   // Scale to fill (max scale) - fills area, may crop edges
};
```

### Fit Mode
- Uses **minimum** of X and Y scale factors
- Entire image is always visible
- May show letterboxing (empty space) if aspect ratios don't match
- Best for: UI icons, buttons, logos, portraits

### Cover Mode
- Uses **maximum** of X and Y scale factors
- Fills the entire target area
- May crop edges if aspect ratios don't match
- Best for: Backgrounds, full-screen images, hero images

### Automatic Mode Selection

The component automatically chooses the scaling mode based on the widget size you specify:
- **Cover Mode**: If image size ≥ 1920x1080 (detected as full-screen images)
- **Fit Mode**: If image size < 1920x1080 (detected as UI elements)

**Why This Distinction?**

This automatic detection solves a common UI problem. Consider a main menu background image that's 1920x1080:
- On a 16:9 monitor (1920x1080): Perfect fit, no issues
- On a 21:9 ultrawide (2560x1080): Using "fit" mode would add black bars on sides - looks bad!
- Solution: Cover mode scales up to 2560x1080, cropping top/bottom slightly - looks professional

Meanwhile, a small icon or logo (say 128x128) should use fit mode:
- Fit mode: Icon stays fully visible, may have padding
- Cover mode: Icon might get cropped at edges - would lose important details

**The Technical Implementation:**

For full-screen images, the target size is the **actual window dimensions** (not the scaled reference size), ensuring perfect coverage without letterboxing:

```cpp
// Pseudo-code from the engine
if (widgetSize.x >= 1920.0f && widgetSize.y >= 1080.0f) {
    // Full-screen mode: Use actual window as target
    FVector2 targetSize = Window::GetDimensions();
    float scaleX = targetSize.x / textureSize.x;
    float scaleY = targetSize.y / textureSize.y;
    float finalScale = std::max(scaleX, scaleY); // Cover mode
} else {
    // UI element mode: Use uniform scaled widget size
    FVector2 targetSize = GetUniformScale() * widgetSize;
    float scaleX = targetSize.x / textureSize.x;
    float scaleY = targetSize.y / textureSize.y;
    float finalScale = std::min(scaleX, scaleY); // Fit mode
}
```

This dual approach means you never have to manually check window size or aspect ratio - just set your widget size appropriately and the engine handles the rest.

## Methods

### Constructor

```cpp
UWidgetImageComponent();
```
Creates a new image widget with no texture set.

### Setters

```cpp
void SetTexturePath(const FilePath& path);
```
Sets the path to the texture file to display.

```cpp
void SetColor(const FColor& color);
```
Sets the tint color applied to the sprite (0-255 range).

```cpp
void SetColor(const FLinearColor& color);
```
Sets the tint color applied to the sprite (0.0-1.0 range).

### Rendering

```cpp
void Render(IRenderer& renderer) const override;
```
Renders the textured sprite with aspect-ratio preservation. Called automatically by the engine.

## Usage Examples

### Full-Screen Background

```cpp
// Full-screen images automatically use cover mode
auto* background = CreateComponent<UWidgetImageComponent>();
background->SetPosition(FVector2(960.0f, 540.0f));
background->SetSize(FVector2(1920.0f, 1080.0f)); // Full reference resolution
background->SetAlignment(EAlignment::Center);
background->SetTexturePath("Assets/Textures/background.png");
background->SetZIndex(0); // Render behind everything

// At any window size:
// - 1920x1080: Displays perfectly
// - 2560x1440: Scales up, crops top/bottom or sides to fill
// - 1280x720: Scales down, crops to fill
// - No letterboxing at any resolution!
```

### UI Icon (Fit Mode)

```cpp
// Small images automatically use fit mode
auto* icon = CreateComponent<UWidgetImageComponent>();
icon->SetPosition(FVector2(100.0f, 100.0f));
icon->SetSize(FVector2(64.0f, 64.0f)); // Small UI element
icon->SetAlignment(EAlignment::TopLeft);
icon->SetTexturePath("Assets/Icons/player_icon.png");
icon->SetZIndex(10);

// Maintains aspect ratio, fits within 64x64 bounds
// If texture is 128x96, it scales down uniformly
```

### Button Image

```cpp
// Create a button image component
auto* button = CreateComponent<UWidgetImageComponent>();

// Button with image
button->SetPosition(FVector2(960.0f, 600.0f));
button->SetSize(FVector2(200.0f, 80.0f));
button->SetAlignment(EAlignment::Center);
button->SetTexturePath("Assets/UI/button_normal.png");
button->SetZIndex(20);

// Handle button states by changing texture
void OnButtonHover()
{
    button->SetTexturePath("Assets/UI/button_hover.png");
}

void OnButtonClick()
{
    button->SetTexturePath("Assets/UI/button_pressed.png");
}
```

### Auto-Sized Image

```cpp
// Create a logo image component
auto* logo = CreateComponent<UWidgetImageComponent>();

// Let the image use its natural texture size
logo->SetPosition(FVector2(960.0f, 200.0f));
// Don't set size - will use texture dimensions
logo->SetAlignment(EAlignment::TopCenter);
logo->SetTexturePath("Assets/UI/logo.png");
logo->SetZIndex(15);

// If texture is 512x256, widget will be sized 512x256 (at 1920x1080)
// Will still scale uniformly with window size
```

### Setting Texture Path

```cpp
// Create an image component
auto* image = CreateComponent<UWidgetImageComponent>();

// Set texture and position
image->SetTexturePath("Assets/Textures/MyImage.png");
image->SetPosition(FVector2(960.0f, 540.0f));
image->SetAlignment(EAlignment::Center);
```

## Scaling Behavior

### UI Elements (< 1920x1080)

For UI elements, the component uses **uniform scaling with fit mode**:

```cpp
// Calculate target size
FVector2 targetSize = GetUniformScale() * widgetSize;

// Use minimum scale (fit mode)
float scaleX = targetSize.x / textureSize.x;
float scaleY = targetSize.y / textureSize.y;
float finalScale = std::min(scaleX, scaleY); // Fit mode

// Result: Image fits within bounds, maintains aspect ratio
```

**Example at 1280x720 window:**
- Widget: 200x200 (reference)
- Uniform scale: ~0.67
- Target: 134x134
- 256x128 texture → scales to 134x67 (fits in bounds)

### Full-Screen Images (≥ 1920x1080)

For full-screen images, the component uses **actual window dimensions with cover mode**:

```cpp
// Use actual window size as target
FVector2 targetSize = Window::GetDimensions();

// Use maximum scale (cover mode)
float scaleX = targetSize.x / textureSize.x;
float scaleY = targetSize.y / textureSize.y;
float finalScale = std::max(scaleX, scaleY); // Cover mode

// Result: Image fills entire window, may crop edges
```

**Example at 2560x1440 window:**
- Widget: 1920x1080 (reference)
- Target: 2560x1440 (actual window!)
- 1920x1080 texture → scales to 2560x1440 (fills window)
- No letterboxing, crops to fit

## Rendering Details

1. **Texture Check**: Returns if no texture is set
2. **Auto-Size**: Uses texture dimensions if widget size is zero
3. **Scale Mode Detection**: Checks if widget size ≥ 1920x1080
4. **Target Calculation**: 
   - Full-screen: Uses actual window dimensions
   - UI: Uses uniform scaled widget size
5. **Aspect-Ratio Scale**: Calculates scaleX and scaleY from texture/target ratio
6. **Final Scale**: 
   - Fit mode: `min(scaleX, scaleY)`
   - Cover mode: `max(scaleX, scaleY)`
7. **Render**: Draws sprite with calculated scale and origin

## Best Practices

1. **Full-Screen Backgrounds**: Set size to 1920x1080 for automatic cover mode
2. **UI Elements**: Keep size < 1920x1080 for fit mode
3. **Texture Dimensions**: Use power-of-two textures when possible (256, 512, 1024, etc.)
4. **Asset Loading**: Load textures through the asset system, not raw file paths
5. **Z-Index**: Background images at z=0, UI elements at higher values
6. **Alignment**: Use Center for full-screen images
7. **Performance**: Reuse texture handles, avoid loading same texture multiple times

## Common Patterns

### Image Gallery

```cpp
class ImageGallery
{
private:
    std::vector<UWidgetImageComponent*> images;
    int currentIndex = 0;
    
public:
    void AddImage(const String& texturePath)
    {
        // Create a new image component for the gallery
        auto* img = CreateComponent<UWidgetImageComponent>();
        img->SetPosition(FVector2(960.0f, 540.0f));
        img->SetSize(FVector2(800.0f, 600.0f));
        img->SetAlignment(EAlignment::Center);
        img->SetTexturePath(texturePath);
        img->SetVisible(false);
        images.push_back(img);
    }
    
    void ShowNext()
    {
        images[currentIndex]->SetVisible(false);
        currentIndex = (currentIndex + 1) % images.size();
        images[currentIndex]->SetVisible(true);
    }
};
```

### Animated Sprite Swap

```cpp
class AnimatedIcon
{
private:
    UWidgetImageComponent* icon;
    std::vector<String> frames;
    int currentFrame = 0;
    float frameTime = 0.0f;
    
public:
    void Update(float deltaTime)
    {
        frameTime += deltaTime;
        if (frameTime >= 0.1f) // 10 FPS
        {
            frameTime = 0.0f;
            currentFrame = (currentFrame + 1) % frames.size();
            icon->SetTexturePath(frames[currentFrame]);
        }
    }
};
```

## Troubleshooting

**Problem**: Image appears stretched or squashed
- **Solution**: Check texture aspect ratio vs widget size aspect ratio. The component preserves aspect ratio, so mismatched ratios will show letterboxing (fit mode) or cropping (cover mode).
- **Example**: A 16:9 image (1920x1080) displayed in a square widget (512x512) will either show black bars or crop to fill the square.

**Problem**: Full-screen image has gaps/letterboxing
- **Solution**: Ensure widget size is set to exactly 1920x1080 (or larger) to trigger cover mode. Even setting it to 1919x1080 will use fit mode!
- **Debug Tip**: Add a debug log to print the widget size - you might be accidentally using scaled values instead of reference resolution values.

**Problem**: Image not displaying
- **Solution**: 
  1. Check that texture path is valid relative to your Assets folder
  2. Verify the texture file format is supported (PNG, JPG, BMP typically work)
  3. Check z-index - the image might be behind other widgets
  4. Verify `SetVisible(true)` is called (it's true by default, but check if something set it to false)
  
**Problem**: Image appears at wrong size
- **Solution**: Remember the 1920x1080 reference resolution. A 200x200 widget at 1280x720 will display at ~133x133 actual pixels. This is correct behavior - don't try to "fix" it by manually scaling your sizes!

**Problem**: Image is too dark or wrong color
- **Solution**: Check if `SetColor()` was called with a tint. The default is white (255,255,255), which shows the image normally. Any other color will tint the image. Use `GetColor()` to check the current tint value.

**Problem**: Performance issues with many images
- **Solution**:
  1. Ensure you're reusing texture handles - loading the same texture multiple times wastes memory
  2. Consider using a texture atlas for UI elements (combine multiple small images into one large texture)
  3. Use `SetVisible(false)` for off-screen images instead of destroying/recreating them
  4. Avoid very large texture files (>4096x4096) unless necessary for full-screen backgrounds

## See Also

- [Widget Component](Widget%20Component.md) - Base widget class documentation
- [Widget Panel Component](Widget%20Panel%20Component.md) - For solid color backgrounds
- [Widget Button Component](Widget%20Button%20Component.md) - For interactive buttons with images
- [URessource](../Assets/URessource.md) - Asset loading system
