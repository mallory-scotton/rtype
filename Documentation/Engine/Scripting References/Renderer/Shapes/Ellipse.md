# UEllipseShape - Ellipse Rendering Shape

## Overview

The `UEllipseShape` class represents an ellipse (oval) shape in the TKD Game Engine. An ellipse is defined by two radii - horizontal and vertical - allowing for stretched circular shapes. When both radii are equal, it behaves as a perfect circle.

Ellipses are essential for:
- **UI Elements**: Buttons, indicators, and selection highlights
- **Game Objects**: Planets, celestial bodies, and organic shapes
- **Effects**: Auras, shields, and energy fields
- **Sports Games**: Balls, pucks, and sporting equipment
- **Biology**: Cells, organisms, and natural structures

## Architecture

### Class Hierarchy
```
UShape (base class)
├── FTransformable2D
├── IDrawable
└── UEllipseShape
    └── Horizontal and vertical radii with point count
```

### Dependencies
- `UShape` - Base shape class providing core functionality
- `FVector2f` - 2D vector mathematics
- `FColor` - Color representation

### Memory Layout
The `UEllipseShape` contains:
- **Inherited Members**: Fill color, outline color, texture, transform
- **m_radius**: Horizontal and vertical radii (FVector2f)
- **m_pointCount**: Number of points for smoothness (SizeT)

## Core Components

### Ellipse Properties
An ellipse is characterized by:
- **Horizontal Radius (a)**: Distance along the X-axis
- **Vertical Radius (b)**: Distance along the Y-axis
- **Point Count**: Number of vertices defining the ellipse curve
- **Parametric Form**: `x = a*cos(t)`, `y = b*sin(t)`

**Special Cases:**
- When a == b: Perfect circle
- When a > b: Horizontally stretched
- When a < b: Vertically stretched

## API Reference

### Constructors

#### Default Constructor
```cpp
UEllipseShape::UEllipseShape(void);
```
Creates an ellipse with default radii (50.0, 30.0) and 30 points.

**Example:**
```cpp
UEllipseShape ellipse; // Creates a horizontal oval
```

#### Parameterized Constructor (Separate Radii)
```cpp
UEllipseShape::UEllipseShape(float radiusX, float radiusY, SizeT pointCount = 30);
```
Creates an ellipse with specified horizontal and vertical radii.

**Parameters:**
- `radiusX`: Horizontal radius
- `radiusY`: Vertical radius
- `pointCount`: Number of points (default: 30, minimum: 3)

**Example:**
```cpp
UEllipseShape ellipse(80.0f, 50.0f, 40); // Horizontal oval
UEllipseShape circle(60.0f, 60.0f);      // Perfect circle
```

#### Parameterized Constructor (Vector)
```cpp
UEllipseShape::UEllipseShape(const FVector2f& radius, SizeT pointCount = 30);
```
Creates an ellipse with radius vector.

**Parameters:**
- `radius`: Radius vector (x = horizontal, y = vertical)
- `pointCount`: Number of points (default: 30, minimum: 3)

**Example:**
```cpp
UEllipseShape ellipse(FVector2f(100.0f, 60.0f), 50);
```

### Radius Management

#### SetRadius (Separate Values)
```cpp
void SetRadius(float radiusX, float radiusY);
```
Sets the horizontal and vertical radii.

**Parameters:**
- `radiusX`: New horizontal radius
- `radiusY`: New vertical radius

**Example:**
```cpp
ellipse.SetRadius(120.0f, 80.0f);
```

#### SetRadius (Vector)
```cpp
void SetRadius(const FVector2f& radius);
```
Sets both radii using a vector.

**Parameters:**
- `radius`: New radius vector

**Example:**
```cpp
ellipse.SetRadius(FVector2f(100.0f, 70.0f));
```

#### GetRadius
```cpp
TKD_NODISCARD const FVector2f& GetRadius(void) const;
```
Returns the current radius vector.

**Returns:** Reference to the radius vector (x = horizontal, y = vertical).

**Example:**
```cpp
const FVector2f& radius = ellipse.GetRadius();
float horizontalRadius = radius.x;
float verticalRadius = radius.y;
```

### Point Count Management

#### SetPointCount
```cpp
void SetPointCount(SizeT count);
```
Sets the number of points defining the ellipse.

**Parameters:**
- `count`: New point count (minimum: 3)

**Example:**
```cpp
ellipse.SetPointCount(60); // Smoother ellipse
```

#### GetPointCount
```cpp
TKD_NODISCARD SizeT GetPointCount(void) const override;
```
Returns the current number of points.

**Returns:** Current point count.

**Example:**
```cpp
SizeT points = ellipse.GetPointCount();
```

### Shape Interface Implementation

#### GetPoint
```cpp
TKD_NODISCARD FVector2f GetPoint(SizeT index) const override;
```
Returns the position of a specific point on the ellipse.

**Parameters:**
- `index`: Index of the point (0 to pointCount-1)

**Returns:** Position of the point at the given index.

**Formula:**
```cpp
angle = index * 2π / pointCount - π/2
x = radiusX * cos(angle)
y = radiusY * sin(angle)
```

**Example:**
```cpp
for (SizeT i = 0; i < ellipse.GetPointCount(); ++i)
{
    FVector2f point = ellipse.GetPoint(i);
}
```

#### GetGeometricCenter
```cpp
TKD_NODISCARD FVector2f GetGeometricCenter(void) const override;
```
Returns the center of the ellipse.

**Returns:** Center point at (radiusX, radiusY).

**Example:**
```cpp
FVector2f center = ellipse.GetGeometricCenter();
ellipse.SetOrigin(center); // Center the origin
```

## Usage Examples

### Basic Shapes

#### Horizontal Oval
```cpp
UEllipseShape horizontalOval(100.0f, 60.0f);
horizontalOval.SetFillColor(FColor::Blue);
horizontalOval.SetPosition(FVector2f(200, 150));
```

#### Vertical Oval
```cpp
UEllipseShape verticalOval(50.0f, 90.0f);
verticalOval.SetFillColor(FColor::Green);
verticalOval.SetPosition(FVector2f(300, 150));
```

#### Perfect Circle
```cpp
UEllipseShape circle(70.0f, 70.0f);
circle.SetFillColor(FColor::Red);
// Alternatively, use UCircleShape for perfect circles
```

#### Thin Ellipse
```cpp
UEllipseShape thinOval(150.0f, 20.0f);
thinOval.SetFillColor(FColor::Yellow);
thinOval.SetRotation(30.0f); // Tilted
```

### Game Objects

#### Planet
```cpp
UEllipseShape planet(80.0f, 75.0f, 50);
planet.SetFillColor(FColor(70, 130, 180)); // Steel blue
planet.SetOutlineColor(FColor(100, 149, 237));
planet.SetOutlineThickness(2.0f);
planet.SetOrigin(planet.GetGeometricCenter());
```

#### Energy Shield
```cpp
UEllipseShape shield(100.0f, 100.0f, 60);
shield.SetFillColor(FColor(0, 191, 255, 50)); // Transparent blue
shield.SetOutlineColor(FColor(0, 191, 255));
shield.SetOutlineThickness(3.0f);
shield.SetOrigin(shield.GetGeometricCenter());
shield.SetPosition(playerPosition);
```

#### Selection Highlight
```cpp
UEllipseShape highlight(60.0f, 40.0f);
highlight.SetFillColor(FColor::Transparent);
highlight.SetOutlineColor(FColor::Yellow);
highlight.SetOutlineThickness(2.0f);
highlight.SetOrigin(highlight.GetGeometricCenter());
highlight.SetPosition(selectedObject.GetPosition());
```

### UI Elements

#### Button
```cpp
UEllipseShape button(120.0f, 50.0f);
button.SetFillColor(FColor(70, 130, 180));
button.SetOutlineColor(FColor::White);
button.SetOutlineThickness(2.0f);
button.SetPosition(FVector2f(400, 300));
```

#### Loading Indicator
```cpp
UEllipseShape loadingRing(40.0f, 40.0f);
loadingRing.SetFillColor(FColor::Transparent);
loadingRing.SetOutlineColor(FColor::White);
loadingRing.SetOutlineThickness(4.0f);
loadingRing.SetOrigin(loadingRing.GetGeometricCenter());

void AnimateLoading(float time)
{
    loadingRing.SetRotation(time * 180.0f);
}
```

#### Progress Indicator
```cpp
// Create an oval progress bar
UEllipseShape progressBg(150.0f, 30.0f);
progressBg.SetFillColor(FColor::Gray);

UEllipseShape progressFill(150.0f * progress, 30.0f);
progressFill.SetFillColor(FColor::Green);
```

### Animation Effects

#### Pulsating Aura
```cpp
void UpdateAura(UEllipseShape& aura, float time)
{
    float baseRadius = 100.0f;
    float pulse = std::sin(time * 3.0f) * 10.0f;
    aura.SetRadius(baseRadius + pulse, baseRadius + pulse);
    
    // Fade alpha with pulse
    int alpha = 100 + static_cast<int>(50.0f * std::sin(time * 3.0f));
    aura.SetFillColor(FColor(255, 100, 100, alpha));
}
```

#### Rotating Ellipse
```cpp
void RotateEllipse(UEllipseShape& ellipse, float deltaTime)
{
    float rotation = ellipse.GetRotation();
    ellipse.SetRotation(rotation + 45.0f * deltaTime);
}
```

#### Morphing Ellipse
```cpp
void MorphEllipse(UEllipseShape& ellipse, float time)
{
    float radiusX = 80.0f + 20.0f * std::sin(time);
    float radiusY = 60.0f + 20.0f * std::cos(time);
    ellipse.SetRadius(radiusX, radiusY);
}
```

### Orbital System
```cpp
class OrbitingBody
{
public:
    UEllipseShape body;
    float orbitRadius;
    float orbitSpeed;
    float angle;
    
    void Update(float deltaTime, const FVector2f& center)
    {
        angle += orbitSpeed * deltaTime;
        
        float x = center.x + orbitRadius * std::cos(angle);
        float y = center.y + orbitRadius * std::sin(angle);
        
        body.SetPosition(FVector2f(x, y));
    }
};

// Usage
OrbitingBody moon;
moon.body.SetRadius(20.0f, 20.0f);
moon.body.SetFillColor(FColor::Gray);
moon.orbitRadius = 150.0f;
moon.orbitSpeed = 1.0f;
```

### Sports Game Elements

#### Rugby Ball
```cpp
UEllipseShape rugbyBall(30.0f, 45.0f);
rugbyBall.SetFillColor(FColor(139, 69, 19)); // Brown
rugbyBall.SetOutlineColor(FColor::White);
rugbyBall.SetOutlineThickness(1.0f);
rugbyBall.SetOrigin(rugbyBall.GetGeometricCenter());
```

#### Racetrack Oval
```cpp
UEllipseShape track(400.0f, 250.0f, 100);
track.SetFillColor(FColor::Transparent);
track.SetOutlineColor(FColor::White);
track.SetOutlineThickness(5.0f);
track.SetOrigin(track.GetGeometricCenter());
```

### Advanced Techniques

#### Textured Ellipse
```cpp
ITexture* texture = LoadTexture("planet_texture.png");
ellipse.SetTexture(texture);
ellipse.SetTextureRect(FRectanglei(0, 0, 
    static_cast<int>(ellipse.GetRadius().x * 2),
    static_cast<int>(ellipse.GetRadius().y * 2)));
```

#### Gradient Ellipse (Multiple Layers)
```cpp
std::vector<UEllipseShape> gradientLayers;
for (int i = 0; i < 5; ++i)
{
    float scale = 1.0f - (i * 0.15f);
    UEllipseShape layer(radiusX * scale, radiusY * scale);
    layer.SetPosition(center);
    layer.SetOrigin(layer.GetGeometricCenter());
    
    int alpha = 255 - (i * 40);
    layer.SetFillColor(FColor(color.r, color.g, color.b, alpha));
    
    gradientLayers.push_back(layer);
}
```

#### Collision Ellipse
```cpp
bool PointInEllipse(const FVector2f& point, const UEllipseShape& ellipse)
{
    FVector2f center = ellipse.GetPosition() + ellipse.GetGeometricCenter();
    FVector2f radius = ellipse.GetRadius();
    FVector2f delta = point - center;
    
    float normalized = (delta.x * delta.x) / (radius.x * radius.x) +
                       (delta.y * delta.y) / (radius.y * radius.y);
    
    return normalized <= 1.0f;
}
```

### Helper Functions

#### Calculate Ellipse Area
```cpp
float CalculateEllipseArea(const UEllipseShape& ellipse)
{
    FVector2f radius = ellipse.GetRadius();
    return M_PI * radius.x * radius.y;
}
```

#### Calculate Ellipse Perimeter (Approximation)
```cpp
float ApproximateEllipsePerimeter(const UEllipseShape& ellipse)
{
    FVector2f radius = ellipse.GetRadius();
    float a = radius.x;
    float b = radius.y;
    
    // Ramanujan's approximation
    float h = ((a - b) * (a - b)) / ((a + b) * (a + b));
    return M_PI * (a + b) * (1.0f + (3.0f * h) / (10.0f + std::sqrt(4.0f - 3.0f * h)));
}
```

#### Create Ellipse from Bounding Box
```cpp
UEllipseShape* CreateEllipseFromBounds(const FRectanglef& bounds)
{
    float radiusX = bounds.width / 2.0f;
    float radiusY = bounds.height / 2.0f;
    
    UEllipseShape* ellipse = new UEllipseShape(radiusX, radiusY);
    ellipse->SetPosition(FVector2f(bounds.left, bounds.top));
    
    return ellipse;
}
```

## Best Practices

### Performance
- **Point Count Guidelines**:
  - Small ellipses (< 50 pixels): 20-30 points
  - Medium ellipses (50-150 pixels): 30-40 points
  - Large ellipses (> 150 pixels): 40-60 points
- **Batch Rendering**: Group similar ellipses together
- **Avoid Frequent Updates**: Only update radii when necessary
- **Reuse Objects**: Create ellipses once and transform them

### Visual Quality
- **Smooth Appearance**: Use 30+ points for smooth curves
- **Aspect Ratio**: Consider visual balance when setting radii
- **Outline Enhancement**: Add outlines for better definition
- **Alpha Blending**: Use transparency for overlay effects

### Design Patterns
- **Object Pooling**: Reuse ellipse objects for particles
- **Factory Pattern**: Create specialized ellipse types
- **Component Pattern**: Use ellipses as visual components
- **State Pattern**: Change appearance based on state

### Common Use Cases
1. **Celestial Bodies**: Planets, moons, and stars
2. **UI Elements**: Buttons, indicators, and highlights
3. **Collision Shapes**: Simplified collision boundaries
4. **Effects**: Auras, shields, and energy fields
5. **Sports Elements**: Balls, tracks, and playing fields

## Mathematical Properties

### Eccentricity
```cpp
float CalculateEccentricity(const UEllipseShape& ellipse)
{
    FVector2f radius = ellipse.GetRadius();
    float a = std::max(radius.x, radius.y); // Semi-major axis
    float b = std::min(radius.x, radius.y); // Semi-minor axis
    
    return std::sqrt(1.0f - (b * b) / (a * a));
}
```

### Focal Distance
```cpp
float CalculateFocalDistance(const UEllipseShape& ellipse)
{
    FVector2f radius = ellipse.GetRadius();
    float a = std::max(radius.x, radius.y);
    float b = std::min(radius.x, radius.y);
    
    return std::sqrt(a * a - b * b);
}
```

## Tips and Tricks

1. **Circle Alternative**: For perfect circles, consider using `UCircleShape` instead
2. **Rotation**: Rotate to create tilted ellipses (ovals at angles)
3. **Scaling**: Use radius adjustment instead of `SetScale()` for better control
4. **Centering**: Set origin to geometric center for proper rotation
5. **Performance**: Lower point count for distant or small ellipses

## Notes

- Ellipses are approximated using line segments between points
- Higher point count = smoother appearance but lower performance
- When radii are equal, consider using `UCircleShape` for optimization
- All inherited shape features (fill, outline, texture) are fully supported
- Ellipse is defined in local space; use transformations for positioning

## See Also

- [UShape](../Shape.md) - Base shape class
- [UCircleShape](Circle.md) - Perfect circle shape
- [UPolygonShape](Polygon.md) - Regular polygon shape
- [URectangleShape](Rectangle.md) - Rectangle shape
- [FTransformable2D](../Transformable%202D.md) - 2D transformation component
- [FVector2f](../../Math/Vector2.md) - 2D vector mathematics
