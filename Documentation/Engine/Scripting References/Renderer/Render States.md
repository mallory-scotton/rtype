# FRenderStates

## Overview

The `FRenderStates` class encapsulates the various states that can affect how an object is rendered. It includes settings for blending, transformations, textures, and shaders. This class is used to customize the rendering process for drawable objects, allowing for effects like transparency, scaling, rotation, texturing, and custom shading. By passing an `FRenderStates` object to the `Draw` method of drawable objects, you can control how they appear on the render target.

## Constructor

### `FRenderStates(EBlendMode blend = EBlendMode::Alpha)`

Constructs a new `FRenderStates` object with the specified blend mode. All other states are initialized to their default values.

**Parameters:**
- `blend`: The blend mode to use for rendering (default: `EBlendMode::Alpha`)

**Example:**
```cpp
// Create render states with default alpha blending
FRenderStates states;

// Create render states with additive blending
FRenderStates additiveStates(EBlendMode::Add);
```

## Members

### `EBlendMode blendMode`

The blend mode determines how the colors of the object being drawn are combined with the colors already on the render target. Common blend modes include alpha blending for transparency and additive blending for glowing effects.

**Type:** `EBlendMode`

**Default:** `EBlendMode::Alpha`

**Example:**
```cpp
states.blendMode = EBlendMode::Multiply;
```

### `FTransform2D transform`

The transformation to apply to the object before rendering. This includes position, rotation, and scale, allowing you to move, rotate, or resize the object during drawing.

**Type:** `FTransform2D`

**Default:** Identity transform (no transformation)

**Example:**
```cpp
states.transform.SetPosition(FVector2f(100.0f, 200.0f));
states.transform.SetRotation(45.0f); // Rotate 45 degrees
states.transform.SetScale(FVector2f(2.0f, 2.0f)); // Scale up
```

### `const ITexture* texture`

A pointer to the texture to apply to the object. If set to `nullptr`, no texture is applied, and the object will use its inherent colors or shaders.

**Type:** `const ITexture*`

**Default:** `nullptr`

**Example:**
```cpp
states.texture = myTexture;
```

### `const IShader* shader`

A pointer to the shader program to use for rendering. Shaders can perform custom vertex and fragment processing, enabling advanced visual effects.

**Type:** `const IShader*`

**Default:** `nullptr`

**Example:**
```cpp
states.shader = myShader;
```

## Usage

`FRenderStates` is typically used when drawing objects to customize their appearance. Here's an example of how to use it with a drawable object:

```cpp
// Assume we have a drawable object and a render target
IDrawable* drawable = GetMyDrawable();
IRenderer* renderer = GetRenderer();

// Create custom render states
FRenderStates states;
states.blendMode = EBlendMode::Add;
states.transform.SetPosition(FVector2f(50.0f, 50.0f));
states.texture = myTexture;
states.shader = myShader;

// Draw the object with the custom states
drawable->Draw(*renderer, states);
```

You can also modify individual states as needed:

```cpp
FRenderStates states;

// Start with defaults
drawable->Draw(*renderer, states);

// Change only the blend mode for the next draw
states.blendMode = EBlendMode::Multiply;
drawable->Draw(*renderer, states);
```

For more advanced usage, combine multiple states:

```cpp
FRenderStates states(EBlendMode::Alpha);
states.transform.SetRotation(90.0f);
states.transform.SetScale(FVector2f(0.5f, 0.5f));
states.texture = nullptr; // No texture, use solid color
states.shader = glowShader; // Apply a glow effect

drawable->Draw(*renderer, states);
```
