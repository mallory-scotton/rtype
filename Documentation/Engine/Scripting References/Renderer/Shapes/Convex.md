# UConvexShape - Convex Polygon Rendering Shape

## Overview

The `UConvexShape` class represents a convex polygon shape in the TKD Game Engine. A convex polygon is a polygon where all interior angles are less than 180 degrees, and any line segment between two points on the boundary lies entirely inside or on the polygon. This class allows you to create custom convex polygons by specifying the positions of the vertices.

Convex shapes are essential for:
- **Custom Game Objects**: Unique geometric shapes and game elements
- **Collision Shapes**: Efficient collision detection with convex hulls
- **UI Elements**: Custom buttons, panels, and interface elements
- **Geometric Design**: Complex 2D modeling and level design
- **Physics Simulation**: Convex collision boundaries

## Architecture

### Class Hierarchy
```
UShape (base class)
├── FTransformable2D
├── IDrawable
└── UConvexShape
    └── Variable number of vertices
```

### Dependencies
- `UShape` - Base shape class providing core functionality
- `FVector2f` - 2D vector mathematics
- `FColor` - Color representation
- `std::vector<FVector2f>` - Dynamic vertex storage

### Memory Layout
The `UConvexShape` contains:
- **Inherited Members**: Fill color, outline color, texture, transform
- **m_points**: std::vector<FVector2f> storing all vertex positions

## Core Components

### Convex Polygon Properties
A convex polygon must satisfy:
1. All interior angles are less than 180 degrees
2. All vertices point "outward"
3. Any line segment between two points on the boundary stays inside the polygon
4. The polygon does not intersect itself

**Important:** This class does not validate convexity. It is the user's responsibility to ensure vertices form a convex shape for proper rendering.

## API Reference

### Constructor

#### Default Constructor
```cpp
explicit UConvexShape(SizeT pointCount = 0);
```
Creates a convex shape with the specified number of points initialized to (0, 0).

**Parameters:**
- `pointCount`: Initial number of points (default: 0)

**Example:**
```cpp
// Create an empty convex shape
UConvexShape shape1;

// Create a convex shape with 5 points
UConvexShape shape2(5);
```

### Point Management

#### SetPointCount
```cpp
void SetPointCount(SizeT count);
```
Sets the number of points in the convex shape. Existing points are preserved if the new count is larger; extra points are initialized to (0, 0).

**Parameters:**
- `count`: New number of points

**Example:**
```cpp
shape.SetPointCount(6); // Create a hexagon
```

#### SetPoint
```cpp
void SetPoint(SizeT index, const FVector2f& point);
```
Sets the position of a specific point.

**Parameters:**
- `index`: Index of the point to set (must be less than point count)
- `point`: New position for the point

**Example:**
```cpp
shape.SetPoint(0, FVector2f(0, 0));
shape.SetPoint(1, FVector2f(100, 0));
shape.SetPoint(2, FVector2f(100, 100));
```

### Shape Interface Implementation

#### GetPointCount
```cpp
TKD_NODISCARD virtual SizeT GetPointCount(void) const override;
```
Returns the number of points defining the convex shape.

**Returns:** Number of points in the shape.

**Example:**
```cpp
SizeT count = shape.GetPointCount();
```

#### GetPoint
```cpp
TKD_NODISCARD virtual FVector2f GetPoint(SizeT index) const override;
```
Returns the position of a specific point.

**Parameters:**
- `index`: Index of the point

**Returns:** Position of the point at the given index, or (0, 0) if index is invalid.

**Example:**
```cpp
for (SizeT i = 0; i < shape.GetPointCount(); ++i)
{
    FVector2f point = shape.GetPoint(i);
}
```

#### GetGeometricCenter
```cpp
TKD_NODISCARD virtual FVector2f GetGeometricCenter(void) const override;
```
Returns the geometric center (centroid) of the convex shape.

**Returns:** The centroid position calculated as the average of all vertices.

**Formula:** `centroid = sum(points) / pointCount`

**Example:**
```cpp
FVector2f center = shape.GetGeometricCenter();
shape.SetOrigin(center); // Center the origin
```

## Usage Examples

### Creating a Pentagon
```cpp
// Create a regular pentagon
UConvexShape pentagon(5);
float radius = 50.0f;
for (SizeT i = 0; i < 5; ++i)
{
    float angle = i * 2.0f * M_PI / 5.0f - M_PI / 2.0f;
    float x = radius * std::cos(angle);
    float y = radius * std::sin(angle);
    pentagon.SetPoint(i, FVector2f(x, y));
}
pentagon.SetFillColor(FColor::Cyan);
pentagon.SetPosition(FVector2f(200, 200));
```

### Creating a Diamond
```cpp
// Create a diamond shape
UConvexShape diamond(4);
diamond.SetPoint(0, FVector2f(50, 0));    // Top
diamond.SetPoint(1, FVector2f(100, 50));  // Right
diamond.SetPoint(2, FVector2f(50, 100));  // Bottom
diamond.SetPoint(3, FVector2f(0, 50));    // Left
diamond.SetFillColor(FColor::Magenta);
diamond.SetOutlineColor(FColor::White);
diamond.SetOutlineThickness(2.0f);
```

### Creating a Star Point (Convex Part)
```cpp
// Create a convex part of a star
UConvexShape starPoint(3);
starPoint.SetPoint(0, FVector2f(50, 0));
starPoint.SetPoint(1, FVector2f(60, 40));
starPoint.SetPoint(2, FVector2f(40, 40));
starPoint.SetFillColor(FColor::Yellow);
```

### Creating an Octagon
```cpp
// Create a regular octagon
UConvexShape octagon(8);
float radius = 60.0f;
for (SizeT i = 0; i < 8; ++i)
{
    float angle = i * 2.0f * M_PI / 8.0f;
    octagon.SetPoint(i, FVector2f(
        radius * std::cos(angle),
        radius * std::sin(angle)
    ));
}
octagon.SetFillColor(FColor::Red);
```

### Custom Game Object
```cpp
// Create a spaceship shape
UConvexShape spaceship(5);
spaceship.SetPoint(0, FVector2f(0, -30));   // Nose
spaceship.SetPoint(1, FVector2f(20, 10));   // Right front
spaceship.SetPoint(2, FVector2f(15, 30));   // Right back
spaceship.SetPoint(3, FVector2f(-15, 30));  // Left back
spaceship.SetPoint(4, FVector2f(-20, 10));  // Left front
spaceship.SetFillColor(FColor(100, 150, 255));
spaceship.SetOrigin(spaceship.GetGeometricCenter());
```

### Dynamic Shape Modification
```cpp
// Animate a convex shape
void UpdateShape(UConvexShape& shape, float time)
{
    for (SizeT i = 0; i < shape.GetPointCount(); ++i)
    {
        float angle = i * 2.0f * M_PI / shape.GetPointCount();
        float radius = 50.0f + 10.0f * std::sin(time + angle);
        shape.SetPoint(i, FVector2f(
            radius * std::cos(angle),
            radius * std::sin(angle)
        ));
    }
}
```

### Collision Shape
```cpp
// Create a collision boundary
UConvexShape collisionBounds(4);
collisionBounds.SetPoint(0, FVector2f(0, 0));
collisionBounds.SetPoint(1, FVector2f(100, 20));
collisionBounds.SetPoint(2, FVector2f(90, 80));
collisionBounds.SetPoint(3, FVector2f(10, 70));
// Use for collision detection
```

### Helper Function for Regular Polygons
```cpp
// Utility function to create regular polygons
UConvexShape* CreateRegularPolygon(SizeT sides, float radius)
{
    UConvexShape* polygon = new UConvexShape(sides);
    for (SizeT i = 0; i < sides; ++i)
    {
        float angle = i * 2.0f * M_PI / sides - M_PI / 2.0f;
        polygon->SetPoint(i, FVector2f(
            radius * std::cos(angle),
            radius * std::sin(angle)
        ));
    }
    return polygon;
}

// Usage
UConvexShape* hexagon = CreateRegularPolygon(6, 50.0f);
```

## Best Practices

### Ensuring Convexity
1. **Define Points in Order**: Always define points in counter-clockwise or clockwise order
2. **Check Angles**: Ensure all interior angles are less than 180 degrees
3. **No Self-Intersection**: Points should not cause the shape to cross itself
4. **Use Regular Polygons**: When possible, use symmetric, regular shapes

### Performance
- **Minimize Point Count**: Use fewer points when possible for better performance
- **Avoid Frequent Changes**: Changing points triggers geometry updates
- **Reuse Objects**: Create shapes once and transform them rather than recreating
- **Batch Similar Shapes**: Group convex shapes with similar properties

### Design Patterns
- **Factory Functions**: Create helper functions for common convex shapes
- **Shape Library**: Build a collection of pre-made convex shapes
- **Component-Based**: Use convex shapes as components in larger systems
- **Object Pooling**: Reuse convex shape objects for temporary effects

### Common Use Cases
1. **Game Characters**: Simplified character collision boundaries
2. **UI Panels**: Custom-shaped interface elements
3. **Level Geometry**: Platforms, ramps, and obstacles
4. **Particle Effects**: Custom particle shapes
5. **Debug Visualization**: Display collision hulls and boundaries

## Limitations

### Convexity Requirement
- The class does not validate that points form a convex shape
- Non-convex shapes will render incorrectly
- For concave shapes, use multiple convex shapes or a different approach

### Point Order
- Points must be defined in consistent winding order (clockwise or counter-clockwise)
- Incorrect ordering can cause rendering artifacts

## Tips and Tricks

### Converting Concave to Convex
```cpp
// For concave shapes, decompose into multiple convex shapes
// Example: L-shape as two rectangles
UConvexShape part1(4), part2(4);
// Define part1 and part2 as convex rectangles
```

### Calculating Convex Hull
```cpp
// Use a convex hull algorithm to generate convex shape from points
std::vector<FVector2f> points = GetPointCloud();
std::vector<FVector2f> hull = ComputeConvexHull(points);

UConvexShape shape(hull.size());
for (SizeT i = 0; i < hull.size(); ++i)
{
    shape.SetPoint(i, hull[i]);
}
```

### Testing Convexity
```cpp
// Simple test for convexity (cross product sign consistency)
bool IsConvex(const UConvexShape& shape)
{
    int sign = 0;
    for (SizeT i = 0; i < shape.GetPointCount(); ++i)
    {
        FVector2f p1 = shape.GetPoint(i);
        FVector2f p2 = shape.GetPoint((i + 1) % shape.GetPointCount());
        FVector2f p3 = shape.GetPoint((i + 2) % shape.GetPointCount());
        
        float cross = (p2.x - p1.x) * (p3.y - p2.y) - 
                     (p2.y - p1.y) * (p3.x - p2.x);
        
        int currentSign = (cross > 0) ? 1 : -1;
        if (sign == 0) sign = currentSign;
        else if (sign != currentSign) return false;
    }
    return true;
}
```

## See Also

- [UShape](../Shape.md) - Base shape class
- [UTriangleShape](Triangle.md) - Triangle shape
- [URectangleShape](Rectangle.md) - Rectangle shape
- [UPolygonShape](Polygon.md) - Regular polygon shape
- [FTransformable2D](../Transformable%202D.md) - 2D transformation component
