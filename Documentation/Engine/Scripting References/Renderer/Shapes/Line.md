# ULineShape - Line Rendering Shape

## Overview

The `ULineShape` class represents a line segment shape in the TKD Game Engine. A line is defined by two points (start and end) with adjustable thickness, making it useful for drawing connections, rays, UI elements, and debug visualizations.

Lines are essential for:
- **Connections**: Visual links between game objects
- **UI Elements**: Dividers, borders, and progress bars
- **Debug Visualization**: Ray casting, paths, and trajectories
- **Effects**: Laser beams, trails, and lightning bolts
- **Graphs**: Network diagrams and flow charts

## Architecture

### Class Hierarchy
```
UShape (base class)
├── FTransformable2D
├── IDrawable
└── ULineShape
    └── Start and end points with thickness
```

### Dependencies
- `UShape` - Base shape class providing core functionality
- `FVector2f` - 2D vector mathematics
- `FColor` - Color representation

### Memory Layout
The `ULineShape` contains:
- **Inherited Members**: Fill color, outline color, texture, transform
- **m_start**: Start point position (FVector2f)
- **m_end**: End point position (FVector2f)
- **m_thickness**: Line thickness (float)

## Core Components

### Line Representation
The line is internally represented as a rectangle (4 points) to give it visible thickness:
- The thickness is applied perpendicular to the line direction
- The line forms a quad that can be filled and textured
- Points are calculated using perpendicular vectors

## API Reference

### Constructors

#### Default Constructor
```cpp
ULineShape::ULineShape(void);
```
Creates a horizontal line with default properties:
- Start: (0, 0)
- End: (100, 0)
- Thickness: 1.0

**Example:**
```cpp
ULineShape line;
```

#### Parameterized Constructor
```cpp
ULineShape::ULineShape(const FVector2f& start, const FVector2f& end, float thickness = 1.0f);
```
Creates a line with specified start point, end point, and thickness.

**Parameters:**
- `start`: Start point of the line
- `end`: End point of the line
- `thickness`: Line thickness (default: 1.0f)

**Example:**
```cpp
ULineShape line(FVector2f(0, 0), FVector2f(100, 100), 5.0f);
```

### Point Management

#### SetStart
```cpp
void SetStart(const FVector2f& point);
```
Sets the start point of the line.

**Parameters:**
- `point`: New start point position

**Example:**
```cpp
line.SetStart(FVector2f(50, 50));
```

#### SetEnd
```cpp
void SetEnd(const FVector2f& point);
```
Sets the end point of the line.

**Parameters:**
- `point`: New end point position

**Example:**
```cpp
line.SetEnd(FVector2f(200, 150));
```

#### GetStart
```cpp
TKD_NODISCARD const FVector2f& GetStart(void) const;
```
Returns the start point of the line.

**Returns:** Reference to the start point position.

**Example:**
```cpp
const FVector2f& start = line.GetStart();
```

#### GetEnd
```cpp
TKD_NODISCARD const FVector2f& GetEnd(void) const;
```
Returns the end point of the line.

**Returns:** Reference to the end point position.

**Example:**
```cpp
const FVector2f& end = line.GetEnd();
```

### Thickness Management

#### SetThickness
```cpp
void SetThickness(float thickness);
```
Sets the thickness of the line.

**Parameters:**
- `thickness`: New line thickness

**Example:**
```cpp
line.SetThickness(3.0f);
```

#### GetThickness
```cpp
TKD_NODISCARD float GetThickness(void) const;
```
Returns the current thickness of the line.

**Returns:** Current line thickness.

**Example:**
```cpp
float thickness = line.GetThickness();
```

### Utility Methods

#### GetLength
```cpp
TKD_NODISCARD float GetLength(void) const;
```
Calculates and returns the length of the line.

**Returns:** The Euclidean distance between start and end points.

**Formula:** `length = sqrt((end.x - start.x)² + (end.y - start.y)²)`

**Example:**
```cpp
float length = line.GetLength();
std::cout << "Line length: " << length << std::endl;
```

### Shape Interface Implementation

#### GetPointCount
```cpp
TKD_NODISCARD virtual SizeT GetPointCount(void) const override;
```
Returns the number of points defining the line shape.

**Returns:** Always returns 4 (line is rendered as a quad).

#### GetPoint
```cpp
TKD_NODISCARD virtual FVector2f GetPoint(SizeT index) const override;
```
Returns the position of a specific corner point of the line quad.

**Parameters:**
- `index`: Index of the point (0-3)

**Returns:** Position of the point at the given index.

**Point Layout:**
- 0: Start point - perpendicular offset
- 1: End point - perpendicular offset
- 2: End point + perpendicular offset
- 3: Start point + perpendicular offset

#### GetGeometricCenter
```cpp
TKD_NODISCARD virtual FVector2f GetGeometricCenter(void) const override;
```
Returns the midpoint of the line.

**Returns:** The center point between start and end.

**Formula:** `center = (start + end) / 2`

**Example:**
```cpp
FVector2f center = line.GetGeometricCenter();
```

## Usage Examples

### Basic Line
```cpp
// Create a simple diagonal line
ULineShape line1(FVector2f(0, 0), FVector2f(100, 100), 2.0f);
line1.SetFillColor(FColor::White);
```

### Horizontal Divider
```cpp
// Create a horizontal divider
ULineShape divider;
divider.SetStart(FVector2f(50, 300));
divider.SetEnd(FVector2f(750, 300));
divider.SetThickness(3.0f);
divider.SetFillColor(FColor(200, 200, 200));
```

### Connection Line
```cpp
// Draw a line connecting two objects
ULineShape connection;
connection.SetStart(object1.GetPosition());
connection.SetEnd(object2.GetPosition());
connection.SetThickness(2.0f);
connection.SetFillColor(FColor::Blue);
```

### Ray Visualization
```cpp
// Visualize a raycast
ULineShape ray(origin, origin + direction * 1000.0f, 1.0f);
ray.SetFillColor(FColor::Red);
```

### Progress Bar Border
```cpp
// Create progress bar border lines
ULineShape topBorder(FVector2f(100, 50), FVector2f(300, 50), 2.0f);
ULineShape bottomBorder(FVector2f(100, 70), FVector2f(300, 70), 2.0f);
ULineShape leftBorder(FVector2f(100, 50), FVector2f(100, 70), 2.0f);
ULineShape rightBorder(FVector2f(300, 50), FVector2f(300, 70), 2.0f);
```

### Animated Line
```cpp
// Animate line endpoints
void UpdateLine(ULineShape& line, float time)
{
    float angle = time * 2.0f;
    float radius = 100.0f;
    FVector2f center(400, 300);
    
    line.SetStart(center);
    line.SetEnd(center + FVector2f(
        radius * std::cos(angle),
        radius * std::sin(angle)
    ));
}
```

### Laser Beam Effect
```cpp
// Create a glowing laser beam
ULineShape laser(gunPosition, targetPosition, 4.0f);
laser.SetFillColor(FColor::Red);
laser.SetOutlineColor(FColor(255, 100, 100));
laser.SetOutlineThickness(2.0f);
```

### Path Visualization
```cpp
// Draw a path between waypoints
std::vector<FVector2f> waypoints = GetPathWaypoints();
std::vector<ULineShape> pathLines;

for (size_t i = 0; i < waypoints.size() - 1; ++i)
{
    ULineShape segment(waypoints[i], waypoints[i + 1], 3.0f);
    segment.SetFillColor(FColor::Green);
    pathLines.push_back(segment);
}
```

### Grid Lines
```cpp
// Create a grid using lines
std::vector<ULineShape> gridLines;
const float spacing = 50.0f;
const float size = 800.0f;

// Horizontal lines
for (float y = 0; y <= size; y += spacing)
{
    ULineShape hLine(FVector2f(0, y), FVector2f(size, y), 1.0f);
    hLine.SetFillColor(FColor(50, 50, 50, 128));
    gridLines.push_back(hLine);
}

// Vertical lines
for (float x = 0; x <= size; x += spacing)
{
    ULineShape vLine(FVector2f(x, 0), FVector2f(x, size), 1.0f);
    vLine.SetFillColor(FColor(50, 50, 50, 128));
    gridLines.push_back(vLine);
}
```

### Velocity Indicator
```cpp
// Show object velocity
ULineShape velocityArrow;
velocityArrow.SetStart(object.GetPosition());
velocityArrow.SetEnd(object.GetPosition() + object.GetVelocity());
velocityArrow.SetThickness(2.0f);
velocityArrow.SetFillColor(FColor::Yellow);
```

### Distance Measurement
```cpp
// Measure and display distance between two points
ULineShape measurement(pointA, pointB, 2.0f);
measurement.SetFillColor(FColor::White);

float distance = measurement.GetLength();
// Display distance as text at the midpoint
FVector2f textPos = measurement.GetGeometricCenter();
```

## Best Practices

### Performance
- **Reuse Line Objects**: Create lines once and update their endpoints
- **Batch Similar Lines**: Group lines with similar properties for efficient rendering
- **Avoid Tiny Lines**: Very short lines may not render well; check length before drawing
- **Use Appropriate Thickness**: Very thin lines (< 1.0) may be hard to see

### Design Patterns
- **Object Pooling**: Reuse line objects for temporary visualizations
- **Factory Pattern**: Create line factories for common line types (grid, border, etc.)
- **Component Pattern**: Use lines as components in complex UI elements

### Visual Quality
1. **Thickness Scaling**: Scale thickness with zoom level for consistent appearance
2. **Anti-Aliasing**: Use outline for smoother appearance
3. **Color Gradients**: Use textures for gradient effects along the line
4. **End Caps**: Consider adding circle shapes at endpoints for rounded appearance

### Common Use Cases
1. **Debug Drawing**: Visualize vectors, rays, and trajectories
2. **UI Elements**: Borders, dividers, and connection lines
3. **Graph Visualization**: Network diagrams and flowcharts
4. **Trails**: Motion trails and particle effects
5. **Laser/Beam Effects**: Weapon effects and energy beams

## Advanced Techniques

### Dashed Lines
```cpp
// Create a dashed line effect using multiple segments
void CreateDashedLine(const FVector2f& start, const FVector2f& end, 
                      float dashLength, float gapLength,
                      std::vector<ULineShape>& outLines)
{
    FVector2f direction = end - start;
    float totalLength = std::sqrt(direction.x * direction.x + 
                                  direction.y * direction.y);
    direction /= totalLength;
    
    float currentLength = 0.0f;
    bool drawing = true;
    
    while (currentLength < totalLength)
    {
        if (drawing)
        {
            float segmentLength = std::min(dashLength, 
                                          totalLength - currentLength);
            FVector2f segStart = start + direction * currentLength;
            FVector2f segEnd = segStart + direction * segmentLength;
            
            ULineShape dash(segStart, segEnd, 2.0f);
            dash.SetFillColor(FColor::White);
            outLines.push_back(dash);
            
            currentLength += segmentLength;
        }
        else
        {
            currentLength += gapLength;
        }
        drawing = !drawing;
    }
}
```

### Arrow Line
```cpp
// Add arrowhead to a line
void CreateArrowLine(const FVector2f& start, const FVector2f& end,
                    ULineShape& line, UTriangleShape& arrowhead)
{
    line.SetStart(start);
    line.SetEnd(end);
    line.SetThickness(3.0f);
    
    FVector2f direction = end - start;
    float angle = std::atan2(direction.y, direction.x);
    
    arrowhead.SetPosition(end);
    arrowhead.SetRotation(angle * 180.0f / M_PI);
    arrowhead.SetOrigin(FVector2f(5, 0));
}
```

### Thickness Animation
```cpp
// Animate line thickness
void AnimateLineThickness(ULineShape& line, float time)
{
    float thickness = 2.0f + std::sin(time * 5.0f) * 1.0f;
    line.SetThickness(thickness);
}
```

## Notes

- Lines are rendered as rectangles (quads) internally
- The fill color controls the line's visible color
- Outline adds a border around the line thickness
- Very thick lines may appear as rectangles
- Transformations apply to both endpoints
- Zero-length lines will not be visible

## See Also

- [UShape](../Shape.md) - Base shape class
- [URectangleShape](Rectangle.md) - Rectangle shape
- [UTriangleShape](Triangle.md) - Triangle shape
- [UConvexShape](Convex.md) - Convex polygon shape
- [FTransformable2D](../Transformable%202D.md) - 2D transformation component
- [FVector2f](../../Math/Vector2.md) - 2D vector mathematics
