# UCircleShape

## Overview

`UCircleShape` is a class in the TKD Engine that represents a circular shape. It inherits from `UShape` and provides functionality to define and manipulate circles through radius and point count settings, point retrieval, and geometric calculations. The circle is approximated using a polygon with a configurable number of points.

## Public Methods

### Constructors

#### `UCircleShape(float radius = 0.f, SizeT pointCount = 30)`

Constructor that initializes the circle with the specified radius and point count.

**Parameters:**
- `radius`: The radius of the circle (default is 0.f).
- `pointCount`: The number of points used to approximate the circle (default is 30, minimum is 3).

### `void SetRadius(float radius)`

Sets the radius of the circle and updates its geometry.

**Parameters:**
- `radius`: The new radius of the circle.

### `float GetRadius() const`

Retrieves the current radius of the circle.

**Returns:**
- `float`: The current radius of the circle.

### `void SetPointCount(SizeT count)`

Sets the number of points used to approximate the circle and updates its geometry. The minimum number of points is 3.

**Parameters:**
- `count`: The new point count (minimum is 3).

### `SizeT GetPointCount() const`

Returns the number of points that define the circle shape.

**Returns:**
- `SizeT`: The number of points used to approximate the circle.

### `FVector2f GetPoint(SizeT index) const`

Retrieves a specific point on the circle based on the given index. Points are calculated using trigonometric functions to distribute them evenly around the circle.

**Parameters:**
- `index`: The index of the point to retrieve (valid range: 0 to pointCount-1).

**Returns:**
- `FVector2f`: The point at the specified index, relative to the circle's origin.

**Point Calculation:**
Points are calculated as:
- x = radius * cos(angle)
- y = radius * sin(angle)
- angle = index * 2π / pointCount - π/2

### `FVector2f GetGeometricCenter() const`

Calculates and returns the geometric center of the circle.

**Returns:**
- `FVector2f`: The geometric center point, which is (radius, radius).

## Examples

### Basic Circle Creation and Radius Setting

```cpp
// Create a circle with default constructor (zero radius, 30 points)
UCircleShape circle;

// Set the radius to 50 units
circle.SetRadius(50.0f);

// Retrieve the radius
float currentRadius = circle.GetRadius();
```

### Circle Creation with Initial Radius and Point Count

```cpp
// Create a circle with initial radius of 100 units and 20 points
UCircleShape circle(100.0f, 20);

// The circle is ready to use with the specified radius and point count
```

### Adjusting Point Count for Circle Approximation

```cpp
// Create a circle with default settings
UCircleShape circle(50.0f);

// Set a higher point count for smoother appearance
circle.SetPointCount(60);

// Get the current point count
SizeT points = circle.GetPointCount();
```

### Iterating Through Circle Points

```cpp
// Create a circle
UCircleShape circle(25.0f, 8);

// Get the number of points
SizeT pointCount = circle.GetPointCount();

// Iterate through all points
for (SizeT i = 0; i < pointCount; ++i) {
    FVector2f point = circle.GetPoint(i);
    // Process each point (e.g., for rendering or collision detection)
    // Point coordinates are relative to the circle's origin
}
```

### Calculating Geometric Center

```cpp
// Create a circle
UCircleShape circle(75.0f);

// Get the geometric center (will be at (75.0f, 75.0f))
FVector2f center = circle.GetGeometricCenter();

// The center can be used for positioning, rotation, or other transformations
```
