# FVertex2DArray

## Overview

The `FVertex2DArray` class represents a collection of 2D vertices used for rendering graphical primitives in the TKD Engine. It extends the `TVector<FVertex2D>` class, providing additional functionality for managing vertex data and specifying the type of primitives to render.

This class is essential for defining custom shapes, sprites, or any 2D graphical elements that require vertex-based rendering. It encapsulates the vertex data along with the primitive type, allowing for efficient rendering operations.

## Constructor

### FVertex2DArray(EPrimitiveType type)

Creates a new instance of `FVertex2DArray` with the specified primitive type.

**Parameters:**
- `type` (EPrimitiveType): The type of primitives to render (e.g., points, lines, triangles).

**Example:**
```cpp
FVertex2DArray vertexArray(EPrimitiveType::Triangles);
```

## Methods

### GetBounds() -> FRectanglef

Calculates and returns the bounding rectangle that encompasses all vertices in the array.

**Returns:**
- `FRectanglef`: A rectangle representing the bounds of the vertex array. If the array is empty, returns `FRectanglef::Zero`.

**Description:**
This method iterates through all vertices to find the minimum and maximum x and y coordinates, then constructs a rectangle from these bounds. It's useful for collision detection, culling, or determining the spatial extent of the rendered geometry.

**Example:**
```cpp
FRectanglef bounds = vertexArray.GetBounds();
if (bounds.width > 0 && bounds.height > 0) {
    // The vertex array has valid bounds
}
```

### GetPrimitiveType() -> EPrimitiveType

Retrieves the primitive type associated with this vertex array.

**Returns:**
- `EPrimitiveType`: The current primitive type of the vertex array.

**Description:**
Returns the primitive type that was set during construction. This determines how the vertices are interpreted during rendering (e.g., as individual points, line segments, or triangles).

**Example:**
```cpp
EPrimitiveType type = vertexArray.GetPrimitiveType();
switch (type) {
    case EPrimitiveType::Points:
        // Handle point rendering
        break;
    case EPrimitiveType::Lines:
        // Handle line rendering
        break;
    case EPrimitiveType::Triangles:
        // Handle triangle rendering
        break;
}
```

## Usage Example

The following example demonstrates how to create a simple triangle using `FVertex2DArray`:

```cpp
// Create a vertex array for triangles
FVertex2DArray triangleArray(EPrimitiveType::Triangles);

// Define the vertices of a triangle
FVertex2D vertex1(FVector2f(0.0f, 0.0f), FColor::Red);
FVertex2D vertex2(FVector2f(100.0f, 0.0f), FColor::Green);
FVertex2D vertex3(FVector2f(50.0f, 100.0f), FColor::Blue);

// Add vertices to the array
triangleArray.PushBack(vertex1);
triangleArray.PushBack(vertex2);
triangleArray.PushBack(vertex3);

// Get the bounds for collision detection
FRectanglef bounds = triangleArray.GetBounds();

// Render the triangle (assuming a render function exists)
renderer.DrawVertexArray(triangleArray);
```

This class provides a flexible way to define and manipulate 2D vertex data for rendering purposes, integrating seamlessly with the TKD Engine's rendering pipeline.
