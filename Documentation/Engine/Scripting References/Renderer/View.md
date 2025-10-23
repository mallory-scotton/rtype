# FView - 2D View System

## Overview

The `FView` class represents a 2D view in the world space within the TKD Game Engine. A view defines how the game world is projected onto the screen, controlling the visible area, position, rotation, and scaling of the rendered scene. This class is fundamental to 2D rendering systems, providing camera-like functionality for 2D games and UI systems.

Views are essential for:
- **Camera Control**: Defining which part of the game world is visible
- **UI Rendering**: Managing different coordinate systems for interface elements
- **Mini-maps**: Creating smaller views of the game world
- **Split-screen**: Supporting multiple viewport configurations
- **Zoom Effects**: Implementing camera zoom and pan functionality

## Architecture

### Class Hierarchy
```
FView
├── Member Variables
│   ├── m_center (FVector2) - View center position
│   ├── m_size (FVector2) - View dimensions
│   ├── m_rotation (float) - Rotation angle in degrees
│   └── m_viewport (FRectangle) - Normalized viewport rectangle
├── Constructors
│   ├── FView() - Default constructor
│   ├── FView(const FRectangle&) - Rectangle-based initialization
│   └── FView(const FVector2&, const FVector2&) - Center/size initialization
├── Setters
├── Getters
├── Transformations
└── Utility Methods
```

### Dependencies
- `FVector2` - 2D vector mathematics
- `FRectangle` - Rectangle geometry
- `FRotator2D` - 2D rotation representation
- `<cmath>` - Mathematical functions for rotation normalization

### Memory Layout
The `FView` class maintains four core properties:
- **Center**: The world position that appears at the center of the view
- **Size**: The width and height of the view in world units
- **Rotation**: Counter-clockwise rotation angle in degrees
- **Viewport**: Normalized rectangle (0-1) defining the screen area

## Core Components

### Member Variables

#### Center (`m_center`)
```cpp
FVector2 m_center;  //<! The center of the view in world coordinates.
```
- **Type**: `FVector2`
- **Purpose**: Defines the world position that appears at the center of the view
- **Units**: World coordinates
- **Default**: `(0, 0)`

#### Size (`m_size`)
```cpp
FVector2 m_size;  //<! The size of the view.
```
- **Type**: `FVector2`
- **Purpose**: Defines the width and height of the view in world units
- **Units**: World coordinates
- **Default**: `(0, 0)` (set by constructors)

#### Rotation (`m_rotation`)
```cpp
float m_rotation;  //<! The rotation of the view in degrees.
```
- **Type**: `float`
- **Purpose**: Counter-clockwise rotation angle of the view
- **Units**: Degrees
- **Range**: `[0, 360)` (automatically normalized)
- **Default**: `0.0f`

#### Viewport (`m_viewport`)
```cpp
FRectangle m_viewport;  //<! The viewport rectangle.
```
- **Type**: `FRectangle`
- **Purpose**: Defines the normalized screen area where the view is rendered
- **Units**: Normalized coordinates (0.0 to 1.0)
- **Default**: `(0, 0, 1, 1)` (full screen)

## API Reference

### Constructors

#### Default Constructor
```cpp
FView::FView(void);
```
Initializes the view with default parameters and resets to a 1000x1000 rectangle.

#### Rectangle Constructor
```cpp
FView::FView(const FRectangle& rectangle);
```
Initializes the view to fit the specified rectangle.

**Parameters:**
- `rectangle`: The rectangle defining the view's initial area

#### Center-Size Constructor
```cpp
FView::FView(const FVector2& center, const FVector2& size);
```
Initializes the view with the specified center position and dimensions.

**Parameters:**
- `center`: The center position of the view
- `size`: The size of the view

### Setter Methods

#### SetCenter (Float Coordinates)
```cpp
void SetCenter(float x, float y);
```
Sets the center of the view using individual coordinates.

**Parameters:**
- `x`: X-coordinate of the center
- `y`: Y-coordinate of the center

#### SetCenter (Vector)
```cpp
void SetCenter(const FVector2& center);
```
Sets the center of the view using a vector.

**Parameters:**
- `center`: The new center position

#### SetSize (Float Dimensions)
```cpp
void SetSize(float width, float height);
```
Sets the size of the view using individual dimensions.

**Parameters:**
- `width`: The new width of the view
- `height`: The new height of the view

#### SetSize (Vector)
```cpp
void SetSize(const FVector2& size);
```
Sets the size of the view using a vector.

**Parameters:**
- `size`: The new size vector

#### SetRotation (Float)
```cpp
void SetRotation(float angle);
```
Sets the rotation of the view in degrees. The angle is automatically normalized to [0, 360).

**Parameters:**
- `angle`: The rotation angle in degrees

#### SetRotation (Rotator)
```cpp
void SetRotation(const FRotator2D& rotation);
```
Sets the rotation of the view using a rotator object.

**Parameters:**
- `rotation`: The new rotation

#### SetViewport
```cpp
void SetViewport(const FRectangle& viewport);
```
Sets the viewport rectangle for the view.

**Parameters:**
- `viewport`: The normalized viewport rectangle (coordinates 0.0 to 1.0)

### Getter Methods

#### GetCenter
```cpp
const FVector2& GetCenter(void) const;
```
Returns the current center position of the view.

**Returns:** Reference to the center vector

#### GetSize
```cpp
const FVector2& GetSize(void) const;
```
Returns the current size of the view.

**Returns:** Reference to the size vector

#### GetRotation
```cpp
float GetRotation(void) const;
```
Returns the current rotation angle in degrees.

**Returns:** Rotation angle in degrees

#### GetRotator
```cpp
FRotator2D GetRotator(void) const;
```
Returns the current rotation as a rotator object.

**Returns:** `FRotator2D` representing the current rotation

#### GetViewport
```cpp
const FRectangle& GetViewport(void) const;
```
Returns the current viewport rectangle.

**Returns:** Reference to the viewport rectangle

### Transformation Methods

#### Move (Float Offsets)
```cpp
void Move(float offsetX, float offsetY);
```
Moves the view by the specified offset.

**Parameters:**
- `offsetX`: X-axis offset
- `offsetY`: Y-axis offset

#### Move (Vector)
```cpp
void Move(const FVector2& offset);
```
Moves the view by the specified offset vector.

**Parameters:**
- `offset`: The offset vector to move by

#### Rotate (Float)
```cpp
void Rotate(float angle);
```
Rotates the view by the specified angle.

**Parameters:**
- `angle`: The angle to rotate by in degrees

#### Rotate (Rotator)
```cpp
void Rotate(const FRotator2D& rotation);
```
Rotates the view by the specified rotator.

**Parameters:**
- `rotation`: The rotation to apply

#### Zoom
```cpp
void Zoom(float factor);
```
Zooms the view by the specified factor. Values > 1 zoom out, values < 1 zoom in.

**Parameters:**
- `factor`: The zoom factor (must be > 0)

### Utility Methods

#### Reset
```cpp
void Reset(const FRectangle& rectangle);
```
Resets the view to fit the specified rectangle, setting center, size, and rotation to match.

**Parameters:**
- `rectangle`: The rectangle to reset the view to

## Usage Examples

### Basic View Creation
```cpp
// Create a view centered at (400, 300) with size 800x600
FView gameView(FVector2(400.0f, 300.0f), FVector2(800.0f, 600.0f));

// Create a view that fits a specific rectangle
FRectangle gameArea(0, 0, 1920, 1080);
FView screenView(gameArea);

// Default view (1000x1000 centered at 500,500)
FView defaultView;
```

### View Manipulation
```cpp
FView camera(FVector2(0, 0), FVector2(1920, 1080));

// Move the camera
camera.Move(100.0f, 50.0f);  // Move right and down
camera.Move(FVector2(-50.0f, 25.0f));  // Move left and up

// Rotate the camera
camera.Rotate(45.0f);  // Rotate 45 degrees clockwise
camera.SetRotation(90.0f);  // Set absolute rotation

// Zoom operations
camera.Zoom(2.0f);  // Zoom out (see more of the world)
camera.Zoom(0.5f);  // Zoom in (see less of the world)

// Set viewport for split-screen
camera.SetViewport(FRectangle(0.0f, 0.0f, 0.5f, 1.0f));  // Left half
```

### Camera Following Player
```cpp
class Player {
    FVector2 position;
    // ... other player code
};

class CameraController {
private:
    FView& m_view;
    const Player& m_player;
    float m_smoothSpeed;

public:
    CameraController(FView& view, const Player& player, float smoothSpeed = 0.1f)
        : m_view(view), m_player(player), m_smoothSpeed(smoothSpeed) {}

    void Update() {
        // Smooth camera following
        FVector2 currentCenter = m_view.GetCenter();
        FVector2 targetCenter = m_player.position;
        FVector2 newCenter = FMath::Lerp(currentCenter, targetCenter, m_smoothSpeed);

        m_view.SetCenter(newCenter);
    }
};
```

### Mini-map Implementation
```cpp
class MiniMap {
private:
    FView m_miniView;
    FRectangle m_miniViewport;

public:
    MiniMap(const FView& mainView) {
        // Create mini-map view covering entire world
        m_miniView.SetCenter(FVector2(0, 0));
        m_miniView.SetSize(FVector2(5000, 5000));  // Large world view

        // Position mini-map in bottom-right corner
        m_miniViewport = FRectangle(0.75f, 0.75f, 0.25f, 0.25f);
        m_miniView.SetViewport(m_miniViewport);
    }

    const FView& GetView() const { return m_miniView; }
};
```

### Screen Shake Effect
```cpp
class ScreenShake {
private:
    FView& m_view;
    FVector2 m_originalCenter;
    float m_intensity;
    float m_duration;
    float m_timer;

public:
    ScreenShake(FView& view) : m_view(view), m_intensity(0), m_duration(0), m_timer(0) {
        m_originalCenter = view.GetCenter();
    }

    void Start(float intensity, float duration) {
        m_intensity = intensity;
        m_duration = duration;
        m_timer = duration;
        m_originalCenter = m_view.GetCenter();
    }

    void Update(float deltaTime) {
        if (m_timer > 0) {
            m_timer -= deltaTime;

            // Generate random offset
            float offsetX = (FMath::RandomFloat() - 0.5f) * m_intensity * (m_timer / m_duration);
            float offsetY = (FMath::RandomFloat() - 0.5f) * m_intensity * (m_timer / m_duration);

            m_view.SetCenter(m_originalCenter + FVector2(offsetX, offsetY));

            if (m_timer <= 0) {
                m_view.SetCenter(m_originalCenter);  // Reset to original position
            }
        }
    }
};
```

## Performance Considerations

### Memory Usage
- **Size**: ~32 bytes (4 floats for FVector2 components + 4 floats for FRectangle + 1 float for rotation)
- **Allocation**: No dynamic memory allocation
- **Copy Cost**: Cheap to copy (POD-like structure)

### Computational Complexity
- **Transformations**: O(1) for all operations
- **Getters/Setters**: O(1) constant time
- **Rotation Normalization**: O(1) using fmod

### Best Practices
1. **Minimize Updates**: Only update view properties when necessary
2. **Batch Operations**: Group multiple transformations together
3. **Cache View State**: Store computed matrices if used frequently
4. **Viewport Optimization**: Use smaller viewports for UI elements

### Thread Safety
- **Not Thread-Safe**: FView is not thread-safe by design
- **Usage Context**: Typically used in main rendering thread
- **Synchronization**: External synchronization required for multi-threaded access

## Mathematical Foundation

### View Transformation Matrix
The view transformation combines translation, rotation, and scaling:

```
ViewMatrix = Translation(-center) * Rotation(-angle) * Scale(2/size)
```

Where:
- `Translation(-center)`: Moves world so view center is at origin
- `Rotation(-angle)`: Rotates world opposite to view rotation
- `Scale(2/size)`: Scales world to fit view dimensions

### Coordinate Systems
```
World Space → View Space → Screen Space
     ↓            ↓            ↓
   (x,y)   →  (-1..1)   →  (0..width)
```

### Viewport Mapping
Viewport coordinates are normalized (0.0 to 1.0) and map to screen pixels:
```
ScreenX = ViewportX * ScreenWidth
ScreenY = ViewportY * ScreenHeight
```

## Diagrams

### View Coordinate System
```
World Space:
(-∞, +∞)     View Center (0,0 in view space)
    ↑              ↓
    │         ┌─────────────┐
    │         │             │
    │         │    View     │
    │         │   Area      │
    │         │             │
    │         └─────────────┘
    │              ↑
    └──────────────┴───────────────
           View Size/2
```

### Viewport Layout
```
Screen Space (Normalized):
(0,0) ┌─────────────────────┐ (1,0)
      │                     │
      │    Viewport Area     │
      │  (e.g., 0.75,0.75,   │
      │        0.25,0.25)    │
      │                     │
(0,1) └─────────────────────┘ (1,1)
```

### Rotation Effect
```
Before Rotation:
┌─────┐
│World│
│Objects│
└─────┘

After 45° Rotation:
   ┌─────┐
  ╱      ╲
 │ World  │
 │Objects │
  ╲      ╱
   └─────┘
```

### Zoom Effect
```
Zoom Out (factor > 1):
┌─────────────────┐
│                 │
│   More World    │
│    Visible      │
│                 │
└─────────────────┘

Zoom In (factor < 1):
     ┌─────┐
     │Less │
     │World│
     │Visible│
     └─────┘
```

## Integration with Rendering Pipeline

### Renderer Integration
```cpp
class FRenderer {
private:
    FView m_currentView;

public:
    void SetView(const FView& view) {
        m_currentView = view;
        UpdateProjectionMatrix();
    }

    void UpdateProjectionMatrix() {
        // Convert view parameters to projection matrix
        FMatrix4x4 projection = CalculateProjectionMatrix(m_currentView);
        SetUniform("u_projection", projection);
    }
};
```

### Multiple Viewports
```cpp
class MultiViewportRenderer {
private:
    std::vector<FView> m_views;

public:
    void AddViewport(const FView& view) {
        m_views.push_back(view);
    }

    void Render() {
        for (const auto& view : m_views) {
            SetScissor(view.GetViewport());
            SetView(view);
            RenderScene();
        }
    }
};
```

## Troubleshooting

### Common Issues

#### View Not Visible
**Problem**: View appears as black screen or wrong area
**Solutions**:
- Check viewport coordinates are within [0,1] range
- Verify view size is not zero
- Ensure view center is within expected world bounds

#### Incorrect Aspect Ratio
**Problem**: View appears stretched or distorted
**Solutions**:
- Match view size aspect ratio to viewport aspect ratio
- Use `Reset()` with proper rectangle dimensions
- Check for incorrect size calculations

#### Rotation Issues
**Problem**: Objects rotate in unexpected directions
**Solutions**:
- Remember rotation is counter-clockwise in degrees
- Use `GetRotator()` for complex rotation operations
- Check rotation normalization (should be [0,360))

#### Zoom Problems
**Problem**: Zoom factor causes unexpected behavior
**Solutions**:
- Ensure zoom factor is always positive
- Values > 1 zoom out, < 1 zoom in
- Check for division by zero in zoom calculations

### Debug Visualization
```cpp
void DebugDrawView(const FView& view, FRenderer& renderer) {
    // Draw view bounds
    FRectangle bounds = CalculateViewBounds(view);
    renderer.DrawRectangle(bounds, FColor::Red, 2.0f);

    // Draw view center
    renderer.DrawCircle(view.GetCenter(), 5.0f, FColor::Blue);

    // Draw rotation indicator
    FVector2 direction = FVector2::Up.Rotate(view.GetRotation());
    renderer.DrawLine(view.GetCenter(),
                    view.GetCenter() + direction * 50.0f,
                    FColor::Green, 2.0f);
}
```

## Future Enhancements

### Potential Features
- **View Interpolation**: Smooth transitions between view states
- **View Constraints**: Boundary limits and aspect ratio locking
- **View Effects**: Post-processing effects per view
- **View Stacking**: Layered view system for complex UIs
- **3D View Extension**: Support for 3D camera views

### API Extensions
```cpp
// Potential future methods
void FView::LerpTo(const FView& target, float t);
void FView::ConstrainToBounds(const FRectangle& bounds);
void FView::SetAspectRatio(float ratio, bool maintainHeight = true);
FMatrix4x4 FView::GetProjectionMatrix() const;
```
