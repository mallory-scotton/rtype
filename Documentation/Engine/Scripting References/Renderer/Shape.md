# UShape Class

## Overview

The `UShape` class is an abstract base class for drawable shapes in the TKD Engine. It provides a common interface for rendering 2D shapes with support for fill colors, outline colors, textures, transformations, and geometric operations. This class cannot be instantiated directly and must be subclassed to define specific shapes by implementing the pure virtual methods `GetPointCount()` and `GetPoint()`.

## Public Methods

### Constructor

#### `UShape(void)`

Constructs a new shape with default properties.

- **Fill Color**: White (`FColor::White`)
- **Outline Color**: Black (`FColor::Black`)
- **Outline Thickness**: 0.0f (no outline)
- **Texture**: nullptr (no texture)
- **Origin**: (0, 0)
- **Miter Limit**: 10.0f

### Destructor

#### `virtual ~UShape() = default`

Destroys the shape. Virtual to allow proper cleanup in derived classes.

### Miter Limit

#### `void SetMiterLimit(float limit)`

Sets the miter limit for outline joins. The miter limit controls how sharp the corners of the outline can be before they are beveled instead of mitered.

**Parameters:**
- `limit`: The miter limit value (default is 10.0f)

**Example:**
```cpp
shape.SetMiterLimit(5.0f);
```

#### `TKD_NODISCARD float GetMiterLimit(void) const`

Returns the current miter limit for outline joins.

**Returns:** The current miter limit value.

**Example:**
```cpp
float limit = shape.GetMiterLimit();
```

### Fill Color

#### `void SetFillColor(const FColor& color)`

Sets the fill color of the shape. This color is applied to all vertices of the shape's fill geometry.

**Parameters:**
- `color`: The fill color to set.

**Example:**
```cpp
shape.SetFillColor(FColor::Red);
```

#### `TKD_NODISCARD const FColor& GetFillColor(void) const`

Returns the current fill color of the shape.

**Returns:** A reference to the current fill color.

**Example:**
```cpp
const FColor& fillColor = shape.GetFillColor();
```

### Outline Color

#### `void SetOutlineColor(const FColor& color)`

Sets the outline color of the shape. This color is applied to all vertices of the shape's outline geometry.

**Parameters:**
- `color`: The outline color to set.

**Example:**
```cpp
shape.SetOutlineColor(FColor::Blue);
```

#### `TKD_NODISCARD const FColor& GetOutlineColor(void) const`

Returns the current outline color of the shape.

**Returns:** A reference to the current outline color.

**Example:**
```cpp
const FColor& outlineColor = shape.GetOutlineColor();
```

### Outline Thickness

#### `void SetOutlineThickness(float thickness)`

Sets the thickness of the shape's outline in pixels. A thickness of 0.0f disables the outline.

**Parameters:**
- `thickness`: The outline thickness in pixels.

**Example:**
```cpp
shape.SetOutlineThickness(2.0f);
```

#### `TKD_NODISCARD float GetOutlineThickness(void) const`

Returns the current outline thickness of the shape.

**Returns:** The current outline thickness in pixels.

**Example:**
```cpp
float thickness = shape.GetOutlineThickness();
```

### Texture

#### `void SetTexture(const ITexture* texture, bool resetRect = true)`

Sets the texture to apply to the shape. If `resetRect` is true and no texture rectangle is set, the texture rectangle is reset to cover the entire texture.

**Parameters:**
- `texture`: Pointer to the texture to set (nullptr for no texture).
- `resetRect`: Whether to reset the texture rectangle to the full texture size.

**Example:**
```cpp
ITexture* myTexture = ...; // Assume texture is loaded
shape.SetTexture(myTexture);
```

#### `TKD_NODISCARD const ITexture* GetTexture(void) const`

Returns the current texture applied to the shape.

**Returns:** Pointer to the current texture (nullptr if no texture).

**Example:**
```cpp
const ITexture* texture = shape.GetTexture();
```

### Position

#### `void SetPosition(const FVector2f& position)`

Sets the position of the shape in world space.

**Parameters:**
- `position`: The position to set.

**Example:**
```cpp
shape.SetPosition(FVector2f(100.0f, 200.0f));
```

#### `TKD_NODISCARD FVector2f GetPosition(void) const`

Returns the current position of the shape.

**Returns:** The current position.

**Example:**
```cpp
FVector2f position = shape.GetPosition();
```

### Rotation

#### `void SetRotation(float rotation)`

Sets the rotation of the shape in radians.

**Parameters:**
- `rotation`: The rotation angle in radians.

**Example:**
```cpp
shape.SetRotation(3.14159f / 2.0f); // 90 degrees
```

#### `TKD_NODISCARD float GetRotation(void) const`

Returns the current rotation of the shape in radians.

**Returns:** The current rotation angle in radians.

**Example:**
```cpp
float rotation = shape.GetRotation();
```

### Scale

#### `void SetScale(const FVector2f& scale)`

Sets the scale of the shape.

**Parameters:**
- `scale`: The scale factors for x and y axes.

**Example:**
```cpp
shape.SetScale(FVector2f(2.0f, 1.5f));
```

#### `TKD_NODISCARD FVector2f GetScale(void) const`

Returns the current scale of the shape.

**Returns:** The current scale factors.

**Example:**
```cpp
FVector2f scale = shape.GetScale();
```

### Origin

#### `void SetOrigin(const FVector2f& origin)`

Sets the origin point of the shape in local coordinates. The origin affects how transformations are applied.

**Parameters:**
- `origin`: The origin point in local coordinates.

**Example:**
```cpp
shape.SetOrigin(FVector2f(50.0f, 50.0f));
```

#### `TKD_NODISCARD const FVector2f& GetOrigin(void) const`

Returns the current origin point of the shape.

**Returns:** A reference to the current origin point.

**Example:**
```cpp
const FVector2f& origin = shape.GetOrigin();
```

### Transform

#### `void SetTransform(const FTransform2D& transform)`

Sets the transform of the shape, replacing the current position, rotation, and scale.

**Parameters:**
- `transform`: The transform to set.

**Example:**
```cpp
FTransform2D newTransform;
newTransform.SetPosition(FVector2f(100.0f, 100.0f));
newTransform.SetRotation(1.57f);
shape.SetTransform(newTransform);
```

#### `TKD_NODISCARD const FTransform2D& GetTransform(void) const`

Returns the current transform of the shape.

**Returns:** A reference to the current transform.

**Example:**
```cpp
const FTransform2D& transform = shape.GetTransform();
```

#### `TKD_NODISCARD FTransform2D GetOriginTransform(void) const`

Returns the transform with the origin applied. This is the transform used for rendering.

**Returns:** The origin-adjusted transform.

**Example:**
```cpp
FTransform2D originTransform = shape.GetOriginTransform();
```

### Bounds

#### `TKD_NODISCARD FRectangle GetLocalBounds(void) const`

Returns the local bounding rectangle of the shape in local coordinates.

**Returns:** The local bounds rectangle.

**Example:**
```cpp
FRectangle localBounds = shape.GetLocalBounds();
```

#### `TKD_NODISCARD FRectangle GetGlobalBounds(void) const`

Returns the global bounding rectangle of the shape in world coordinates, taking into account the shape's transform.

**Returns:** The global bounds rectangle.

**Example:**
```cpp
FRectangle globalBounds = shape.GetGlobalBounds();
```

### Points

#### `TKD_NODISCARD virtual SizeT GetPointCount(void) const = 0`

Returns the number of points that define the shape. This is a pure virtual method that must be implemented by derived classes.

**Returns:** The number of points.

#### `TKD_NODISCARD virtual FVector2f GetPoint(SizeT index) const = 0`

Returns the position of a specific point in the shape. This is a pure virtual method that must be implemented by derived classes.

**Parameters:**
- `index`: The index of the point to retrieve.

**Returns:** The position of the point at the given index.

### Geometric Center

#### `TKD_NODISCARD virtual FVector2f GetGeometricCenter(void) const`

Returns the geometric center of the shape. The default implementation calculates the centroid for polygons or the average position for simpler shapes.

**Returns:** The geometric center point.

**Example:**
```cpp
FVector2f center = shape.GetGeometricCenter();
```

### Drawing

#### `void Draw(IRenderer& renderer, FRenderStates states = FRenderStates()) const`

Draws the shape using the provided renderer. This method applies the shape's transform, texture, and renders both the fill and outline (if thickness > 0).

**Parameters:**
- `renderer`: The renderer to use for drawing.
- `states`: Additional render states to apply.

**Example:**
```cpp
IRenderer& renderer = ...; // Assume renderer is available
shape.Draw(renderer);
```

## Notes

- The `UShape` class is designed to be subclassed. Common derived classes might include `URectangleShape`, `UCircleShape`, etc.
- Transformations (position, rotation, scale) are applied in the order: scale, then rotation, then translation.
- The origin affects the pivot point for transformations.
- Outline geometry is automatically updated when outline properties change.
- Texture coordinates are automatically calculated based on the texture rectangle and shape bounds.
