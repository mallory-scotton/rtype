# 🎨 Sprite

## Overview

The `USprite` class is a fundamental component for 2D rendering in the TKD Engine. It represents a textured quad that can be positioned, scaled, rotated, and colored in 2D space. Sprites are essential for displaying images, animations, and UI elements in games and applications.

As a drawable object that inherits from `FTransformable2D`, sprites support transformations such as translation, rotation, and scaling. They can be rendered with custom colors and texture coordinates, making them versatile for various visual effects.

## Key Features

- **Texture Support**: Display portions of textures with customizable rectangles
- **Color Modulation**: Apply tinting and transparency effects
- **Transformable**: Full 2D transformation support (position, rotation, scale)
- **Bounds Calculation**: Automatic local and global bounding box computation
- **Efficient Rendering**: Optimized vertex-based rendering with texture mapping

## Constructors

### Default Constructor
```cpp
USprite::USprite(void);
```
Creates an empty sprite with no texture assigned. The sprite will not render until a texture is set.

### Texture Constructor
```cpp
USprite::USprite(const ITexture& texture);
```
Creates a sprite using the entire provided texture.

**Parameters:**
- `texture`: The texture to be used for the sprite

### Texture with Rectangle Constructor
```cpp
USprite::USprite(const ITexture& texture, const FRectanglei& rect);
```
Creates a sprite using a specific rectangular portion of the provided texture.

**Parameters:**
- `texture`: The texture to be used for the sprite
- `rect`: The rectangle defining the texture area to use

## Methods

### SetTexture
```cpp
void SetTexture(const ITexture& texture, bool resetRect = false);
```
Assigns a texture to the sprite. Optionally resets the texture rectangle to cover the entire texture.

**Parameters:**
- `texture`: The texture to be used for the sprite
- `resetRect`: If true, the texture rectangle will be reset to the entire texture size

### SetTextureRect
```cpp
void SetTextureRect(const FRectanglei& rect);
```
Defines the rectangular area of the texture to be displayed by the sprite.

**Parameters:**
- `rect`: The rectangle defining the texture area (in pixels)

### SetColor (FColor)
```cpp
void SetColor(const FColor& color);
```
Sets the color modulation for the entire sprite. This color is multiplied with the texture colors.

**Parameters:**
- `color`: The color to apply to the sprite

### SetColor (FLinearColor)
```cpp
void SetColor(const FLinearColor& color);
```
Sets the color modulation using linear color values.

**Parameters:**
- `color`: The linear color to apply to the sprite

### GetTexture
```cpp
const ITexture* GetTexture(void) const;
```
Retrieves the current texture assigned to the sprite.

**Returns:** Pointer to the texture, or nullptr if no texture is assigned

### GetTextureRect
```cpp
const FRectanglei GetTextureRect(void) const;
```
Retrieves the current texture rectangle.

**Returns:** The rectangle defining the texture area being displayed

### GetColor
```cpp
const FColor& GetColor(void) const;
```
Retrieves the current color modulation.

**Returns:** The color currently applied to the sprite

### GetLocalBounds
```cpp
FRectangle GetLocalBounds(void) const;
```
Calculates the local bounding rectangle of the sprite in its own coordinate system.

**Returns:** The local bounds as a rectangle (position is always 0,0, size matches texture rect)

### GetGlobalBounds
```cpp
FRectangle GetGlobalBounds(void) const;
```
Calculates the global bounding rectangle of the sprite after applying all transformations.

**Returns:** The transformed bounds in world coordinates

### Draw
```cpp
virtual void Draw(IRenderer& target, FRenderStates states = FRenderStates()) const;
```
Renders the sprite to the specified renderer using the provided render states.

**Parameters:**
- `target`: The renderer to draw to
- `states`: Additional render states to apply (default: identity states)

## Usage Examples

### Basic Sprite Creation and Rendering
```cpp
#include <Engine/Renderer/USprite.hpp>
#include <Engine/Renderer/ITexture.hpp>

// Assuming you have a texture loaded
ITexture* myTexture = LoadTexture("player.png");

// Create a sprite using the entire texture
USprite playerSprite(*myTexture);

// Position the sprite
playerSprite.SetPosition(FVector2(100.0f, 200.0f));

// Set a semi-transparent color
playerSprite.SetColor(FColor(255, 255, 255, 128));

// Render the sprite
renderer.Draw(playerSprite);
```

### Sprite with Texture Sub-Region
```cpp
// Create a sprite using only a portion of the texture
FRectanglei spriteRect(0, 0, 32, 32); // 32x32 pixels from top-left
USprite animatedSprite(*textureAtlas, spriteRect);

// Later, change to a different frame
FRectanglei nextFrame(32, 0, 32, 32);
animatedSprite.SetTextureRect(nextFrame);
```

### Transformed Sprite
```cpp
USprite sprite(*myTexture);

// Scale the sprite
sprite.SetScale(FVector2(2.0f, 2.0f));

// Rotate 45 degrees
sprite.SetRotation(45.0f);

// Move to position
sprite.SetPosition(FVector2(400.0f, 300.0f));

// The global bounds will reflect all transformations
FRectangle bounds = sprite.GetGlobalBounds();
```

### Color Effects
```cpp
USprite sprite(*myTexture);

// Make the sprite red
sprite.SetColor(FColor(255, 0, 0));

// Or using linear color for more precision
sprite.SetColor(FLinearColor(1.0f, 0.5f, 0.0f, 1.0f)); // Orange
```

## Performance Considerations

- Sprites are lightweight objects that reference textures rather than copying them
- Texture changes are expensive; reuse sprites with different texture rectangles for animations
- Color modulation is performed in the vertex shader for efficient rendering
- Use `GetGlobalBounds()` for collision detection rather than recalculating manually

## See Also

- `FTransformable2D`: Base class providing transformation functionality
- `IDrawable`: Interface for renderable objects
- `ITexture`: Interface for texture resources
- `IRenderer`: Interface for rendering targets
