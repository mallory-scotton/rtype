# UTriangleShape - Triangle Rendering Shape

## Overview

The `UTriangleShape` class represents a triangle shape in the TKD Game Engine. A triangle is a fundamental 2D geometric shape consisting of three vertices connected by three edges, commonly used for arrows, indicators, geometric designs, and UI elements.

Triangles are essential for:
- **UI Elements**: Arrow indicators, direction markers, and warning symbols
- **Game Objects**: Simple geometric shapes and patterns
- **Debugging**: Visual markers and pointers
- **Geometric Design**: Basic 2D modeling and tessellation
- **Navigation**: Direction indicators and waypoint markers

## Architecture

### Class Hierarchy
```
UShape (base class)
├── FTransformable2D
├── IDrawable
└── UTriangleShape
    └── 3 vertex positions
```

### Dependencies
- `UShape` - Base shape class providing core functionality
- `FVector2f` - 2D vector mathematics
- `FColor` - Color representation
- `FVertex2DArray` - 2D vertex array for rendering

### Memory Layout
The `UTriangleShape` contains:
- **Inherited Members**: Fill color, outline color, texture, transform
- **m_point1**: First vertex position (FVector2f)
- **m_point2**: Second vertex position (FVector2f)
- **m_point3**: Third vertex position (FVector2f)

## Core Components

### Vertex Structure
The triangle is defined by three vertices that can be positioned anywhere in 2D space:
- **Point 1**: First vertex of the triangle
- **Point 2**: Second vertex of the triangle
- **Point 3**: Third vertex of the triangle

The vertices are ordered counter-clockwise for proper rendering.

## API Reference

### Constructors

#### Default Constructor
```cpp
UTriangleShape::UTriangleShape(void);
```
Creates an equilateral triangle with default vertices:
- Point 1: (0.0f, 0.0f)
- Point 2: (50.0f, 0.0f)
- Point 3: (25.0f, 43.3f)

**Example:**
```cpp
UTriangleShape triangle;
```

#### Parameterized Constructor
```cpp
UTriangleShape::UTriangleShape(const FVector2f& p1, const FVector2f& p2, const FVector2f& p3);
```
Creates a triangle with custom vertex positions.

**Parameters:**
- `p1`: Position of the first vertex
- `p2`: Position of the second vertex
- `p3`: Position of the third vertex

**Example:**
```cpp
UTriangleShape triangle(FVector2f(0, 0), FVector2f(100, 0), FVector2f(50, 100));
```

### Point Management

#### SetPoint1
```cpp
void SetPoint1(const FVector2f& point);
```
Sets the position of the first vertex.

**Parameters:**
- `point`: New position for the first vertex

**Example:**
```cpp
triangle.SetPoint1(FVector2f(10, 20));
```

#### SetPoint2
```cpp
void SetPoint2(const FVector2f& point);
```
Sets the position of the second vertex.

**Parameters:**
- `point`: New position for the second vertex

**Example:**
```cpp
triangle.SetPoint2(FVector2f(30, 40));
```

#### SetPoint3
```cpp
void SetPoint3(const FVector2f& point);
```
Sets the position of the third vertex.

**Parameters:**
- `point`: New position for the third vertex

**Example:**
```cpp
triangle.SetPoint3(FVector2f(50, 60));
```

#### GetPoint1
```cpp
TKD_NODISCARD const FVector2f& GetPoint1(void) const;
```
Returns the position of the first vertex.

**Returns:** Reference to the first vertex position.

**Example:**
```cpp
const FVector2f& p1 = triangle.GetPoint1();
```

#### GetPoint2
```cpp
TKD_NODISCARD const FVector2f& GetPoint2(void) const;
```
Returns the position of the second vertex.

**Returns:** Reference to the second vertex position.

**Example:**
```cpp
const FVector2f& p2 = triangle.GetPoint2();
```

#### GetPoint3
```cpp
TKD_NODISCARD const FVector2f& GetPoint3(void) const;
```
Returns the position of the third vertex.

**Returns:** Reference to the third vertex position.

**Example:**
```cpp
const FVector2f& p3 = triangle.GetPoint3();
```

### Shape Interface Implementation

#### GetPointCount
```cpp
TKD_NODISCARD virtual SizeT GetPointCount(void) const override;
```
Returns the number of points defining the triangle.

**Returns:** Always returns 3.

#### GetPoint
```cpp
TKD_NODISCARD virtual FVector2f GetPoint(SizeT index) const override;
```
Returns the position of a specific point.

**Parameters:**
- `index`: Index of the point (0, 1, or 2)

**Returns:** Position of the point at the given index.

**Example:**
```cpp
for (SizeT i = 0; i < triangle.GetPointCount(); ++i)
{
    FVector2f point = triangle.GetPoint(i);
}
```

#### GetGeometricCenter
```cpp
TKD_NODISCARD virtual FVector2f GetGeometricCenter(void) const override;
```
Returns the centroid (geometric center) of the triangle.

**Returns:** The centroid position calculated as the average of the three vertices.

**Formula:** `centroid = (p1 + p2 + p3) / 3`

**Example:**
```cpp
FVector2f center = triangle.GetGeometricCenter();
```

## Usage Examples

### Basic Triangle Creation
```cpp
// Create a default equilateral triangle
UTriangleShape triangle1;
triangle1.SetFillColor(FColor::Red);
triangle1.SetPosition(FVector2f(100, 100));
```

### Custom Triangle
```cpp
// Create a custom triangle
UTriangleShape triangle2(
    FVector2f(0, 0),
    FVector2f(100, 0),
    FVector2f(50, 87)
);
triangle2.SetFillColor(FColor::Blue);
triangle2.SetOutlineColor(FColor::White);
triangle2.SetOutlineThickness(2.0f);
```

### Right-Angled Triangle
```cpp
// Create a right-angled triangle
UTriangleShape rightTriangle;
rightTriangle.SetPoint1(FVector2f(0, 0));
rightTriangle.SetPoint2(FVector2f(100, 0));
rightTriangle.SetPoint3(FVector2f(0, 100));
rightTriangle.SetFillColor(FColor::Green);
```

### Animated Triangle
```cpp
// Animate triangle vertices
void UpdateTriangle(UTriangleShape& triangle, float time)
{
    float offset = std::sin(time) * 20.0f;
    triangle.SetPoint3(FVector2f(50, 87 + offset));
}
```

### Arrow Indicator
```cpp
// Create an upward-pointing arrow indicator
UTriangleShape arrow;
arrow.SetPoint1(FVector2f(50, 0));    // Top point
arrow.SetPoint2(FVector2f(0, 100));   // Bottom left
arrow.SetPoint3(FVector2f(100, 100)); // Bottom right
arrow.SetFillColor(FColor::Yellow);
arrow.SetPosition(playerPosition + FVector2f(0, -50));
```

### Transformations
```cpp
// Apply transformations
triangle.SetPosition(FVector2f(200, 150));
triangle.SetRotation(45.0f);
triangle.SetScale(FVector2f(2.0f, 2.0f));
triangle.SetOrigin(triangle.GetGeometricCenter());
```

### Textured Triangle
```cpp
// Apply texture to triangle
ITexture* texture = LoadTexture("pattern.png");
triangle.SetTexture(texture);
triangle.SetTextureRect(FRectanglei(0, 0, 100, 100));
```

## Best Practices

### Performance
- **Reuse Objects**: Create triangle objects once and reuse them
- **Batch Rendering**: Group similar triangles together for efficient rendering
- **Minimize Updates**: Only call setter methods when values actually change

### Design Patterns
- **Factory Pattern**: Create specialized triangle types (isosceles, equilateral, etc.)
- **Object Pooling**: Reuse triangle objects for temporary indicators
- **Component Pattern**: Use triangles as components in more complex shapes

### Common Use Cases
1. **Direction Indicators**: Point to objectives or destinations
2. **Warning Symbols**: Visual alerts and notifications
3. **Play Buttons**: UI element for media controls
4. **Geometric Patterns**: Tessellations and decorative elements
5. **Collision Visualization**: Debug display for triangular collision shapes

## Notes

- Triangles are defined in local space; use transformations for positioning
- The centroid is the balance point of the triangle
- All inherited shape features (fill color, outline, texture) are supported
- Changing any point automatically updates the geometry
- Supports both clockwise and counter-clockwise winding orders

## See Also

- [UShape](../Shape.md) - Base shape class
- [URectangleShape](Rectangle.md) - Rectangle shape
- [UCircleShape](Circle.md) - Circle shape
- [UConvexShape](Convex.md) - Convex polygon shape
- [FTransformable2D](../Transformable%202D.md) - 2D transformation component
