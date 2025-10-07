# URectangleShape

## Overview

`URectangleShape` is a class in the TKD Engine that represents a rectangular shape. It inherits from `UShape` and provides functionality to define and manipulate rectangles through size settings, point retrieval, and geometric calculations.

## Public Methods

### Constructors

#### `URectangleShape()`

Default constructor that initializes the rectangle with a zero size.

#### `URectangleShape(const FVector2f& size)`

Constructor that initializes the rectangle with the specified size.

**Parameters:**
- `size`: The size of the rectangle as a `FVector2f` object.

### `void SetSize(const FVector2f& size)`

Sets the size of the rectangle and updates its geometry.

**Parameters:**
- `size`: The new size of the rectangle as a `FVector2f` object.

### `const FVector2f& GetSize() const`

Retrieves the current size of the rectangle.

**Returns:**
- `const FVector2f&`: A reference to the rectangle's size.

### `SizeT GetPointCount() const`

Returns the number of points that define the rectangle shape.

**Returns:**
- `SizeT`: The number of points (always 4 for a rectangle).

### `FVector2f GetPoint(SizeT index) const`

Retrieves a specific point of the rectangle based on the given index.

**Parameters:**
- `index`: The index of the point to retrieve (valid range: 0-3).

**Returns:**
- `FVector2f`: The point at the specified index.

**Point Mapping:**
- Index 0: (0.0f, 0.0f) - Bottom-left corner
- Index 1: (size.x, 0.0f) - Bottom-right corner
- Index 2: (size.x, size.y) - Top-right corner
- Index 3: (0.0f, size.y) - Top-left corner

### `FVector2f GetGeometricCenter() const`

Calculates and returns the geometric center of the rectangle.

**Returns:**
- `FVector2f`: The geometric center point, calculated as `size * 0.5f`.

## Examples

### Basic Rectangle Creation and Size Setting

```cpp
// Create a rectangle with default constructor (zero size)
URectangleShape rectangle;

// Set the size to 100x50 units
rectangle.SetSize(FVector2f(100.0f, 50.0f));

// Retrieve the size
FVector2f currentSize = rectangle.GetSize();
```

### Rectangle Creation with Initial Size

```cpp
// Create a rectangle with initial size of 200x100 units
URectangleShape rectangle(FVector2f(200.0f, 100.0f));

// The rectangle is ready to use with the specified size
```

### Iterating Through Rectangle Points

```cpp
// Create a rectangle
URectangleShape rectangle(FVector2f(10.0f, 20.0f));

// Get the number of points (will be 4)
SizeT pointCount = rectangle.GetPointCount();

// Iterate through all points
for (SizeT i = 0; i < pointCount; ++i) {
    FVector2f point = rectangle.GetPoint(i);
    // Process each point (e.g., for rendering or collision detection)
    // Point coordinates are relative to the rectangle's origin
}
```

### Calculating Geometric Center

```cpp
// Create a square rectangle
URectangleShape square(FVector2f(100.0f, 100.0f));

// Get the geometric center (will be at (50.0f, 50.0f))
FVector2f center = square.GetGeometricCenter();

// The center can be used for positioning, rotation, or other transformations
```
