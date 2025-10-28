# UPolygonShape - Regular Polygon Rendering Shape

## Overview

The `UPolygonShape` class represents a regular polygon shape in the TKD Game Engine. A regular polygon is a polygon where all sides have the same length and all angles are equal. This class allows you to create any regular polygon by specifying the number of sides and radius.

Regular polygons are essential for:
- **UI Elements**: Badges, buttons, and decorative elements
- **Game Objects**: Gems, coins, and collectibles
- **Geometric Design**: Patterns, tiles, and backgrounds
- **Symbols**: Stop signs, warning icons, and indicators
- **Procedural Generation**: Symmetric game elements

## Architecture

### Class Hierarchy
```
UShape (base class)
├── FTransformable2D
├── IDrawable
└── UPolygonShape
    └── Configurable side count and radius
```

### Dependencies
- `UShape` - Base shape class providing core functionality
- `FVector2f` - 2D vector mathematics
- `FColor` - Color representation

### Memory Layout
The `UPolygonShape` contains:
- **Inherited Members**: Fill color, outline color, texture, transform
- **m_radius**: Distance from center to vertices (float)
- **m_sideCount**: Number of sides (SizeT)

## Core Components

### Regular Polygon Properties
A regular polygon is characterized by:
- **Equal Sides**: All sides have the same length
- **Equal Angles**: All interior angles are equal
- **Symmetry**: Rotational symmetry around the center
- **Inscribed Circle**: All vertices lie on a circle of radius `m_radius`

**Common Regular Polygons:**
- 3 sides: Equilateral Triangle
- 4 sides: Square
- 5 sides: Pentagon
- 6 sides: Hexagon
- 8 sides: Octagon

## API Reference

### Constructors

#### Default Constructor
```cpp
UPolygonShape::UPolygonShape(void);
```
Creates a regular hexagon (6 sides) with radius 50.0.

**Example:**
```cpp
UPolygonShape polygon; // Creates a hexagon
```

#### Parameterized Constructor
```cpp
UPolygonShape::UPolygonShape(float radius, SizeT sideCount);
```
Creates a regular polygon with specified radius and number of sides.

**Parameters:**
- `radius`: Distance from center to vertices
- `sideCount`: Number of sides (minimum: 3)

**Example:**
```cpp
UPolygonShape pentagon(50.0f, 5); // Create a pentagon
UPolygonShape octagon(100.0f, 8); // Create an octagon
```

### Property Management

#### SetRadius
```cpp
void SetRadius(float radius);
```
Sets the radius of the polygon.

**Parameters:**
- `radius`: New radius value

**Example:**
```cpp
polygon.SetRadius(75.0f);
```

#### GetRadius
```cpp
TKD_NODISCARD float GetRadius(void) const;
```
Returns the current radius of the polygon.

**Returns:** Current radius value.

**Example:**
```cpp
float radius = polygon.GetRadius();
```

#### SetSideCount
```cpp
void SetSideCount(SizeT count);
```
Sets the number of sides of the polygon.

**Parameters:**
- `count`: New side count (minimum: 3)

**Example:**
```cpp
polygon.SetSideCount(7); // Change to heptagon
```

#### GetSideCount
```cpp
TKD_NODISCARD SizeT GetSideCount(void) const;
```
Returns the current number of sides.

**Returns:** Current side count.

**Example:**
```cpp
SizeT sides = polygon.GetSideCount();
```

### Shape Interface Implementation

#### GetPointCount
```cpp
TKD_NODISCARD virtual SizeT GetPointCount(void) const override;
```
Returns the number of points defining the polygon.

**Returns:** Number of sides (equal to point count).

**Example:**
```cpp
SizeT pointCount = polygon.GetPointCount();
```

#### GetPoint
```cpp
TKD_NODISCARD virtual FVector2f GetPoint(SizeT index) const override;
```
Returns the position of a specific vertex.

**Parameters:**
- `index`: Index of the vertex (0 to sideCount-1)

**Returns:** Position of the vertex at the given index.

**Formula:**
```cpp
angle = index * 2π / sideCount - π/2
x = radius + radius * cos(angle)
y = radius + radius * sin(angle)
```

**Example:**
```cpp
for (SizeT i = 0; i < polygon.GetPointCount(); ++i)
{
    FVector2f vertex = polygon.GetPoint(i);
}
```

#### GetGeometricCenter
```cpp
TKD_NODISCARD virtual FVector2f GetGeometricCenter(void) const override;
```
Returns the center of the polygon.

**Returns:** Center point at (radius, radius).

**Example:**
```cpp
FVector2f center = polygon.GetGeometricCenter();
polygon.SetOrigin(center); // Center the origin
```

## Usage Examples

### Basic Shapes

#### Triangle
```cpp
UPolygonShape triangle(50.0f, 3);
triangle.SetFillColor(FColor::Red);
triangle.SetPosition(FVector2f(100, 100));
```

#### Square
```cpp
UPolygonShape square(50.0f, 4);
square.SetFillColor(FColor::Blue);
square.SetRotation(45.0f); // Diamond orientation
```

#### Pentagon
```cpp
UPolygonShape pentagon(60.0f, 5);
pentagon.SetFillColor(FColor::Green);
pentagon.SetOutlineColor(FColor::White);
pentagon.SetOutlineThickness(2.0f);
```

#### Hexagon
```cpp
UPolygonShape hexagon; // Default is hexagon
hexagon.SetFillColor(FColor::Yellow);
hexagon.SetPosition(FVector2f(200, 200));
```

#### Octagon (Stop Sign)
```cpp
UPolygonShape stopSign(80.0f, 8);
stopSign.SetFillColor(FColor::Red);
stopSign.SetOutlineColor(FColor::White);
stopSign.SetOutlineThickness(3.0f);
```

### Game Objects

#### Collectible Gem
```cpp
UPolygonShape gem(30.0f, 6);
gem.SetFillColor(FColor::Cyan);
gem.SetOrigin(gem.GetGeometricCenter());
gem.SetPosition(spawnPosition);
```

#### Badge/Icon
```cpp
UPolygonShape badge(40.0f, 8);
badge.SetFillColor(FColor(255, 215, 0)); // Gold
badge.SetOutlineColor(FColor(139, 69, 19)); // Brown
badge.SetOutlineThickness(2.0f);
```

#### Rotating Star Point
```cpp
UPolygonShape starPoint(50.0f, 5);
starPoint.SetFillColor(FColor::Yellow);
starPoint.SetOrigin(starPoint.GetGeometricCenter());

// Animate rotation
void Update(float deltaTime)
{
    float currentRotation = starPoint.GetRotation();
    starPoint.SetRotation(currentRotation + 45.0f * deltaTime);
}
```

### UI Elements

#### Loading Spinner
```cpp
UPolygonShape spinner(20.0f, 6);
spinner.SetFillColor(FColor::White);
spinner.SetOrigin(spinner.GetGeometricCenter());

void AnimateSpinner(float time)
{
    spinner.SetRotation(time * 180.0f); // Rotate over time
}
```

#### Progress Indicator Segments
```cpp
std::vector<UPolygonShape> progressSegments;
const int segmentCount = 8;
const float radius = 100.0f;

for (int i = 0; i < segmentCount; ++i)
{
    UPolygonShape segment(15.0f, 6);
    float angle = i * 360.0f / segmentCount;
    float x = radius * std::cos(angle * M_PI / 180.0f);
    float y = radius * std::sin(angle * M_PI / 180.0f);
    
    segment.SetPosition(FVector2f(x, y));
    segment.SetFillColor(i < currentProgress ? FColor::Green : FColor::Gray);
    progressSegments.push_back(segment);
}
```

### Procedural Patterns

#### Honeycomb Pattern
```cpp
void CreateHoneycomb(std::vector<UPolygonShape>& cells, 
                    int rows, int cols, float size)
{
    const float hexWidth = size * std::sqrt(3.0f);
    const float hexHeight = size * 2.0f;
    
    for (int row = 0; row < rows; ++row)
    {
        for (int col = 0; col < cols; ++col)
        {
            UPolygonShape hexagon(size, 6);
            
            float x = col * hexWidth + (row % 2) * (hexWidth / 2.0f);
            float y = row * hexHeight * 0.75f;
            
            hexagon.SetPosition(FVector2f(x, y));
            hexagon.SetFillColor(FColor::White);
            hexagon.SetOutlineColor(FColor::Black);
            hexagon.SetOutlineThickness(1.0f);
            
            cells.push_back(hexagon);
        }
    }
}
```

#### Circular Pattern
```cpp
void CreateCircularPattern(std::vector<UPolygonShape>& polygons,
                          int count, float radius, int sides)
{
    for (int i = 0; i < count; ++i)
    {
        UPolygonShape poly(20.0f, sides);
        float angle = i * 2.0f * M_PI / count;
        
        float x = radius * std::cos(angle);
        float y = radius * std::sin(angle);
        
        poly.SetPosition(FVector2f(x, y));
        poly.SetOrigin(poly.GetGeometricCenter());
        poly.SetRotation(angle * 180.0f / M_PI);
        
        polygons.push_back(poly);
    }
}
```

### Animation Effects

#### Morphing Polygon
```cpp
void MorphPolygon(UPolygonShape& polygon, float time)
{
    // Morph between different shapes
    int minSides = 3;
    int maxSides = 8;
    float t = (std::sin(time) + 1.0f) / 2.0f; // Normalize to [0, 1]
    int sides = minSides + static_cast<int>(t * (maxSides - minSides));
    
    polygon.SetSideCount(sides);
}
```

#### Pulsating Polygon
```cpp
void PulsatePolygon(UPolygonShape& polygon, float time, 
                   float baseRadius, float amplitude)
{
    float radius = baseRadius + amplitude * std::sin(time * 3.0f);
    polygon.SetRadius(radius);
}
```

#### Color Cycling
```cpp
void CyclePolygonColor(UPolygonShape& polygon, float time)
{
    float hue = std::fmod(time * 60.0f, 360.0f);
    FColor color = HSVtoRGB(hue, 1.0f, 1.0f);
    polygon.SetFillColor(color);
}
```

### Helper Functions

#### Create Regular Polygon by Side Length
```cpp
UPolygonShape* CreatePolygonBySideLength(SizeT sides, float sideLength)
{
    // Calculate radius from side length
    float angle = M_PI / sides;
    float radius = sideLength / (2.0f * std::sin(angle));
    
    return new UPolygonShape(radius, sides);
}
```

#### Calculate Polygon Area
```cpp
float CalculatePolygonArea(const UPolygonShape& polygon)
{
    float radius = polygon.GetRadius();
    SizeT sides = polygon.GetSideCount();
    float angle = 2.0f * M_PI / sides;
    
    return 0.5f * sides * radius * radius * std::sin(angle);
}
```

#### Calculate Polygon Perimeter
```cpp
float CalculatePolygonPerimeter(const UPolygonShape& polygon)
{
    float radius = polygon.GetRadius();
    SizeT sides = polygon.GetSideCount();
    float angle = 2.0f * M_PI / sides;
    float sideLength = 2.0f * radius * std::sin(angle / 2.0f);
    
    return sideLength * sides;
}
```

## Best Practices

### Performance
- **Cache Polygon Objects**: Create polygons once and reuse them
- **Batch Similar Polygons**: Group polygons with the same properties
- **Use Appropriate Side Count**: More sides = smoother but slower
- **Optimize Side Count**: Use fewer sides when possible (6-8 is usually sufficient)

### Visual Design
- **Side Count Guidelines**:
  - 3-4 sides: Sharp, angular appearance
  - 5-8 sides: Balanced geometry
  - 9+ sides: Approaches circular appearance
- **Radius Scaling**: Scale radius proportionally with zoom level
- **Color Schemes**: Use consistent colors for similar polygon types
- **Outlines**: Add outlines for better visibility and definition

### Common Use Cases
1. **UI Indicators**: Status icons, badges, and buttons
2. **Game Tokens**: Collectibles, power-ups, and items
3. **Tile Maps**: Hexagonal or octagonal tile grids
4. **Particle Effects**: Geometric particle shapes
5. **Symbols**: Warning signs, status indicators

## Mathematical Properties

### Interior Angle
```cpp
float GetInteriorAngle(SizeT sides)
{
    return ((sides - 2) * 180.0f) / sides;
}
```

### Exterior Angle
```cpp
float GetExteriorAngle(SizeT sides)
{
    return 360.0f / sides;
}
```

### Side Length
```cpp
float CalculateSideLength(float radius, SizeT sides)
{
    float angle = 2.0f * M_PI / sides;
    return 2.0f * radius * std::sin(angle / 2.0f);
}
```

### Apothem (distance from center to side)
```cpp
float CalculateApothem(float radius, SizeT sides)
{
    float angle = M_PI / sides;
    return radius * std::cos(angle);
}
```

## Advanced Techniques

### Textured Polygon
```cpp
// Apply texture to polygon
ITexture* texture = LoadTexture("pattern.png");
polygon.SetTexture(texture);
polygon.SetTextureRect(FRectanglei(0, 0, 
    static_cast<int>(polygon.GetRadius() * 2),
    static_cast<int>(polygon.GetRadius() * 2)));
```

### Gradient Fill (Using Multiple Polygons)
```cpp
// Create gradient effect with nested polygons
std::vector<UPolygonShape> gradientLayers;
for (int i = 0; i < 5; ++i)
{
    float scale = 1.0f - (i * 0.15f);
    UPolygonShape layer(baseRadius * scale, sides);
    layer.SetPosition(center);
    layer.SetOrigin(layer.GetGeometricCenter());
    
    int alpha = 255 - (i * 40);
    layer.SetFillColor(FColor(baseColor.r, baseColor.g, baseColor.b, alpha));
    
    gradientLayers.push_back(layer);
}
```

### Star Pattern Using Polygons
```cpp
// Create a star pattern with triangular points
void CreateStar(std::vector<UPolygonShape>& star, int points, float radius)
{
    for (int i = 0; i < points; ++i)
    {
        UPolygonShape point(radius * 0.3f, 3);
        float angle = i * 2.0f * M_PI / points;
        
        float x = radius * std::cos(angle);
        float y = radius * std::sin(angle);
        
        point.SetPosition(FVector2f(x, y));
        point.SetOrigin(point.GetGeometricCenter());
        point.SetRotation(angle * 180.0f / M_PI);
        
        star.push_back(point);
    }
}
```

## Tips and Tricks

1. **Smooth Circles**: Use 30-60 sides for smooth circular appearance
2. **Performance**: Use 6-8 sides for most game objects
3. **Rotation**: Polygons with odd sides may need rotation adjustment
4. **Scaling**: Scale by adjusting radius rather than using SetScale()
5. **Centering**: Always set origin to geometric center for proper rotation

## See Also

- [UShape](../Shape.md) - Base shape class
- [UCircleShape](Circle.md) - Circle shape (smooth polygon)
- [UConvexShape](Convex.md) - Custom convex shapes
- [UTriangleShape](Triangle.md) - Three-sided polygon
- [URectangleShape](Rectangle.md) - Four-sided polygon
- [FTransformable2D](../Transformable%202D.md) - 2D transformation component
