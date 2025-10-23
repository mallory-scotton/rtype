# Camera Documentation

## Overview

The `ACamera` class is a specialized actor that represents a camera in the TKD Game Engine's 3D rendering system. It extends the base `AActor` functionality with camera-specific properties and behaviors, providing the foundation for viewpoint management, perspective projection, and scene visualization.

### Key Features

- **Actor Integration**: Inherits from `AActor` for full actor system integration
- **View Transform**: Separate view transform for camera positioning and orientation
- **Projection Parameters**: Configurable field of view, aspect ratio, and clipping planes
- **Rendering Integration**: Works seamlessly with the engine's rendering pipeline
- **Property Replication**: Camera properties can be synchronized across network
- **Component Support**: Can have camera-related components attached

### Architecture

```cpp
class ACamera : public AActor
{
public:
    // Camera-specific properties
    UProperty<FTransform> ViewTransform;   // Camera's view matrix
    UProperty<Float32> FieldOfView;        // Vertical field of view (degrees)
    UProperty<Float32> AspectRatio;        // Width/height ratio
    UProperty<Float32> NearClipPlane;      // Near clipping distance
    UProperty<Float32> FarClipPlane;       // Far clipping distance

public:
    ACamera(const FString& name = "ACamera");
};
```

The `ACamera` builds upon the `AActor` foundation, adding camera-specific properties while maintaining all actor capabilities like component attachment, networking, and lifecycle management.

## Core Components

### View Transform

The `ViewTransform` property defines the camera's position and orientation in world space:

- **Position**: Camera's location in 3D space
- **Rotation**: Camera's viewing direction and up vector
- **Scale**: Typically (1,1,1) for cameras (no scaling applied)

### Projection Parameters

#### Field of View (FOV)
- **Range**: Typically 60-120 degrees
- **Default**: 90 degrees
- **Effect**: Controls the camera's viewing angle
- **Narrow FOV**: Telephoto effect, less distortion
- **Wide FOV**: Wide-angle effect, more distortion

#### Aspect Ratio
- **Calculation**: Width ÷ Height
- **Default**: 16:9 (1.777...)
- **Effect**: Determines viewport shape
- **Common Ratios**: 16:9, 16:10, 4:3, 21:9

#### Clipping Planes
- **Near Clip**: Minimum rendering distance (prevents z-fighting)
- **Far Clip**: Maximum rendering distance (performance optimization)
- **Default Range**: 0.1 to 1000 units

## API Reference

### Constructor & Destructor

#### `ACamera(const FString& name = "ACamera")`

Creates a new camera actor with default camera properties.

**Parameters:**
- `name`: Optional camera name (defaults to "ACamera")

**Behavior:**
- Initializes all camera properties with sensible defaults
- Disables transform replication (cameras typically controlled locally)
- Registers camera in the reflection system

### Camera Properties

#### `UProperty<FTransform> ViewTransform`

The camera's view transform defining its position and orientation.

**Type:** `FTransform`
**Default:** `FTransform::Identity`
**Replication:** Inherits from actor's replication settings

**Usage:**
```cpp
// Set camera position
camera->ViewTransform = FTransform(
    FVector3(0, 0, 10),    // Position
    FRotator(0, 0, 0),     // Rotation
    FVector3(1, 1, 1)      // Scale (usually 1,1,1 for cameras)
);

// Get camera forward vector
FVector3 forward = camera->ViewTransform.GetRotation().GetForwardVector();
```

#### `UProperty<Float32> FieldOfView`

The camera's vertical field of view in degrees.

**Type:** `Float32`
**Default:** `90.0f`
**Range:** `1.0f - 179.0f` (practical range: `30.0f - 120.0f`)
**Replication:** Inherits from actor's replication settings

**Usage:**
```cpp
// Set wide-angle view
camera->FieldOfView = 120.0f;

// Set telephoto view
camera->FieldOfView = 30.0f;

// Get horizontal FOV from vertical FOV
Float32 horizontalFOV = 2.0f * atan(tan(FieldOfView * DEG_TO_RAD / 2.0f) * AspectRatio) * RAD_TO_DEG;
```

#### `UProperty<Float32> AspectRatio`

The camera's aspect ratio (width/height).

**Type:** `Float32`
**Default:** `16.0f / 9.0f` (≈1.777)
**Range:** `> 0.0f`
**Replication:** Inherits from actor's replication settings

**Usage:**
```cpp
// Set aspect ratio for different displays
camera->AspectRatio = 16.0f / 9.0f;   // 16:9 widescreen
camera->AspectRatio = 4.0f / 3.0f;    // 4:3 standard
camera->AspectRatio = 21.0f / 9.0f;   // 21:9 ultrawide

// Calculate from viewport dimensions
Float32 viewportWidth = 1920.0f;
Float32 viewportHeight = 1080.0f;
camera->AspectRatio = viewportWidth / viewportHeight;
```

#### `UProperty<Float32> NearClipPlane`

The distance to the near clipping plane.

**Type:** `Float32`
**Default:** `0.1f`
**Range:** `> 0.0f` (must be less than FarClipPlane)
**Replication:** Inherits from actor's replication settings

**Usage:**
```cpp
// Close near plane for detailed work
camera->NearClipPlane = 0.01f;

// Default near plane
camera->NearClipPlane = 0.1f;

// Far near plane to hide nearby objects
camera->NearClipPlane = 1.0f;
```

#### `UProperty<Float32> FarClipPlane`

The distance to the far clipping plane.

**Type:** `Float32`
**Default:** `1000.0f`
**Range:** `> NearClipPlane`
**Replication:** Inherits from actor's replication settings

**Usage:**
```cpp
// Short range for indoor scenes
camera->FarClipPlane = 50.0f;

// Long range for outdoor scenes
camera->FarClipPlane = 10000.0f;

// Very long range for space scenes
camera->FarClipPlane = 1000000.0f;
```

### Inherited Methods

Since `ACamera` inherits from `AActor`, it has access to all actor functionality:

- **Transform Management**: `GetTransform()`, `SetTransform()`, `Translate()`, `Rotate()`, `Scale()`
- **Component System**: `AddComponent<T>()`, `GetComponent<T>()`, `RemoveComponent()`
- **Lifecycle**: `BeginPlay()`, `Tick()`, `EndPlay()`
- **Networking**: Movement prediction, replication, RPCs
- **State Management**: `IsActive()`, `SetActive()`, `MarkForDeletion()`

## Usage Examples

### Basic Camera Setup

```cpp
class AMyGameMode : public AActor
{
public:
    virtual void BeginPlay() override
    {
        AActor::BeginPlay();

        // Create main camera
        mainCamera = GetWorld()->SpawnActor<ACamera>("MainCamera");

        // Position camera
        mainCamera->ViewTransform = FTransform(
            FVector3(0, -10, 5),    // Position behind and above origin
            FRotator(-15, 0, 0),    // Look down slightly
            FVector3(1, 1, 1)
        );

        // Configure projection
        mainCamera->FieldOfView = 75.0f;
        mainCamera->AspectRatio = 16.0f / 9.0f;
        mainCamera->NearClipPlane = 0.1f;
        mainCamera->FarClipPlane = 1000.0f;
    }

private:
    ACamera* mainCamera;
};
```

### Third-Person Camera System

```cpp
class AThirdPersonCamera : public ACamera
{
public:
    AThirdPersonCamera(AActor* targetActor)
        : ACamera("ThirdPersonCamera")
        , target(targetActor)
        , distance(5.0f)
        , height(2.0f)
        , damping(5.0f)
    {
        // Set initial position
        UpdateCameraPosition();
    }

    virtual void Tick(Float32 deltaTime) override
    {
        ACamera::Tick(deltaTime);

        // Smooth camera follow
        UpdateCameraPosition();
    }

    void SetTarget(AActor* newTarget)
    {
        target = newTarget;
    }

    void SetDistance(Float32 newDistance)
    {
        distance = newDistance;
        UpdateCameraPosition();
    }

    void SetHeight(Float32 newHeight)
    {
        height = newHeight;
        UpdateCameraPosition();
    }

private:
    AActor* target;
    Float32 distance;
    Float32 height;
    Float32 damping;

    void UpdateCameraPosition()
    {
        if (!target) return;

        FVector3 targetPos = target->GetTransform().GetPosition();
        FRotator targetRot = target->GetTransform().GetRotation();

        // Calculate desired camera position
        FVector3 offset = targetRot.GetForwardVector() * -distance;
        offset.z += height;

        FVector3 desiredPos = targetPos + offset;

        // Smooth interpolation
        FVector3 currentPos = ViewTransform.GetPosition();
        FVector3 newPos = FMath::Lerp(currentPos, desiredPos, damping * GetWorld()->GetDeltaTime());

        // Look at target
        FVector3 lookDir = (targetPos - newPos).Normalized();
        FRotator lookRot = FRotator::FromDirection(lookDir);

        ViewTransform = FTransform(newPos, lookRot, FVector3(1, 1, 1));
    }
};
```

### Camera Controller with Input

```cpp
class ACameraController : public AActor
{
public:
    ACameraController(ACamera* cameraToControl)
        : AActor("CameraController")
        , controlledCamera(cameraToControl)
        , moveSpeed(10.0f)
        , rotateSpeed(90.0f)
    {
        // Disable controller's own transform replication
        SetTransformReplicated(false);
    }

    virtual void Tick(Float32 deltaTime) override
    {
        AActor::Tick(deltaTime);

        HandleMovement(deltaTime);
        HandleRotation(deltaTime);
        HandleZoom(deltaTime);
    }

    void HandleMovement(Float32 deltaTime)
    {
        FVector3 movement = FVector3::Zero;

        // WASD movement
        if (GetInputManager()->IsKeyPressed(EKey::W)) movement += FVector3(0, 1, 0);
        if (GetInputManager()->IsKeyPressed(EKey::S)) movement += FVector3(0, -1, 0);
        if (GetInputManager()->IsKeyPressed(EKey::A)) movement += FVector3(-1, 0, 0);
        if (GetInputManager()->IsKeyPressed(EKey::D)) movement += FVector3(1, 0, 0);

        if (movement.Length() > 0.0f)
        {
            movement = movement.Normalized() * moveSpeed * deltaTime;
            controlledCamera->ViewTransform.Translate(movement);
        }
    }

    void HandleRotation(Float32 deltaTime)
    {
        FVector2 mouseDelta = GetInputManager()->GetMouseDelta();

        if (mouseDelta.Length() > 0.0f)
        {
            // Rotate camera based on mouse movement
            FRotator currentRot = controlledCamera->ViewTransform.GetRotation();
            FRotator deltaRot(mouseDelta.y * rotateSpeed * deltaTime,
                            mouseDelta.x * rotateSpeed * deltaTime, 0);

            controlledCamera->ViewTransform.SetRotation(currentRot + deltaRot);
        }
    }

    void HandleZoom(Float32 deltaTime)
    {
        Float32 scrollDelta = GetInputManager()->GetMouseWheelDelta();

        if (abs(scrollDelta) > 0.0f)
        {
            // Zoom by adjusting field of view
            Float32 newFOV = controlledCamera->FieldOfView - scrollDelta * 10.0f;
            newFOV = FMath::Clamp(newFOV, 30.0f, 120.0f);
            controlledCamera->FieldOfView = newFOV;
        }
    }

private:
    ACamera* controlledCamera;
    Float32 moveSpeed;
    Float32 rotateSpeed;
};
```

### Multi-Camera System

```cpp
class AMultiCameraSystem : public AActor
{
public:
    virtual void BeginPlay() override
    {
        AActor::BeginPlay();

        // Create multiple cameras for different views
        mainCamera = CreateCamera("MainCamera", FVector3(0, -5, 3), FRotator(-10, 0, 0));
        overheadCamera = CreateCamera("OverheadCamera", FVector3(0, 0, 20), FRotator(-90, 0, 0));
        sideCamera = CreateCamera("SideCamera", FVector3(10, 0, 0), FRotator(0, -90, 0));

        // Set different properties for each camera
        overheadCamera->FieldOfView = 45.0f;  // Wider view for overhead
        sideCamera->NearClipPlane = 0.5f;     // Closer near plane for side view

        // Start with main camera active
        SetActiveCamera(mainCamera);
    }

    void SwitchToNextCamera()
    {
        if (activeCamera == mainCamera) SetActiveCamera(overheadCamera);
        else if (activeCamera == overheadCamera) SetActiveCamera(sideCamera);
        else SetActiveCamera(mainCamera);
    }

    void SetActiveCamera(ACamera* camera)
    {
        activeCamera = camera;
        GetRenderer()->SetActiveCamera(camera);
    }

    ACamera* GetActiveCamera() const { return activeCamera; }

private:
    ACamera* mainCamera;
    ACamera* overheadCamera;
    ACamera* sideCamera;
    ACamera* activeCamera;

    ACamera* CreateCamera(const FString& name, const FVector3& position, const FRotator& rotation)
    {
        ACamera* camera = GetWorld()->SpawnActor<ACamera>(name);
        camera->ViewTransform = FTransform(position, rotation, FVector3(1, 1, 1));
        return camera;
    }
};
```

### Cinematic Camera with Animation

```cpp
class ACinematicCamera : public ACamera
{
public:
    ACinematicCamera()
        : ACamera("CinematicCamera")
        , animationTime(0.0f)
        , animationDuration(5.0f)
        , isPlaying(false)
    {
        // Set cinematic camera properties
        FieldOfView = 50.0f;  // Slightly narrower for cinematic feel
        NearClipPlane = 0.1f;
        FarClipPlane = 2000.0f;
    }

    void StartCinematic(const FTransform& startTransform, const FTransform& endTransform)
    {
        startPosition = startTransform;
        endPosition = endTransform;
        animationTime = 0.0f;
        isPlaying = true;

        ViewTransform = startTransform;
    }

    void StopCinematic()
    {
        isPlaying = false;
        animationTime = 0.0f;
    }

    virtual void Tick(Float32 deltaTime) override
    {
        ACamera::Tick(deltaTime);

        if (isPlaying)
        {
            animationTime += deltaTime;
            Float32 t = FMath::Clamp(animationTime / animationDuration, 0.0f, 1.0f);

            // Smooth interpolation with easing
            Float32 easedT = EaseInOutCubic(t);

            // Interpolate position
            FVector3 pos = FMath::Lerp(startPosition.GetPosition(),
                                     endPosition.GetPosition(), easedT);

            // Interpolate rotation (spherical interpolation for smooth rotation)
            FRotator rot = FRotator::Slerp(startPosition.GetRotation(),
                                         endPosition.GetRotation(), easedT);

            ViewTransform = FTransform(pos, rot, FVector3(1, 1, 1));

            // Dynamic FOV change during cinematic
            Float32 fovT = sin(t * PI) * 0.5f + 0.5f;  // Sine wave
            FieldOfView = FMath::Lerp(40.0f, 80.0f, fovT);

            if (t >= 1.0f)
            {
                isPlaying = false;
                OnCinematicFinished();
            }
        }
    }

    UFunction<> OnCinematicFinished;

private:
    FTransform startPosition;
    FTransform endPosition;
    Float32 animationTime;
    Float32 animationDuration;
    bool isPlaying;

    Float32 EaseInOutCubic(Float32 t) const
    {
        return t < 0.5f ? 4 * t * t * t : 1 - pow(-2 * t + 2, 3) / 2;
    }
};
```

## Integration with Rendering System

### Camera Registration

Cameras need to be registered with the rendering system to be used:

```cpp
class URenderer
{
public:
    void SetActiveCamera(ACamera* camera);
    void RegisterCamera(ACamera* camera);
    void UnregisterCamera(ACamera* camera);

    // Get view and projection matrices from camera
    FMatrix4x4 GetViewMatrix(const ACamera* camera) const;
    FMatrix4x4 GetProjectionMatrix(const ACamera* camera) const;
    FMatrix4x4 GetViewProjectionMatrix(const ACamera* camera) const;
};
```

### Matrix Calculations

The rendering system uses camera properties to compute view and projection matrices:

```cpp
FMatrix4x4 URenderer::GetViewMatrix(const ACamera* camera) const
{
    // View matrix = inverse of camera transform
    return camera->ViewTransform.GetInverseMatrix();
}

FMatrix4x4 URenderer::GetProjectionMatrix(const ACamera* camera) const
{
    // Perspective projection matrix
    Float32 fovRad = camera->FieldOfView * DEG_TO_RAD;
    Float32 aspect = camera->AspectRatio;
    Float32 nearZ = camera->NearClipPlane;
    Float32 farZ = camera->FarClipPlane;

    return FMatrix4x4::CreatePerspective(fovRad, aspect, nearZ, farZ);
}
```

### Viewport Management

```cpp
class FViewport
{
public:
    void SetCamera(ACamera* camera);
    void SetViewportRect(const FRect& rect);
    void SetScissorRect(const FRect& rect);

    // Multiple viewports for split-screen
    void AddViewport(ACamera* camera, const FRect& rect);
    void RemoveViewport(ACamera* camera);
};
```

## Performance Considerations

### Memory Usage

- **Base Overhead**: Minimal additional memory beyond `AActor`
- **Property Storage**: 5 float properties + transform (negligible)
- **No Components**: Cameras typically don't need many components

### CPU Performance

- **Matrix Calculations**: View/projection matrices computed per frame
- **Minimal Updates**: Usually static or smoothly interpolated
- **Frustum Culling**: Can be used for culling optimization

### Optimization Strategies

1. **Matrix Caching**: Cache view/projection matrices when camera doesn't move
2. **Frustum Planes**: Pre-calculate frustum planes for culling
3. **LOD Updates**: Update camera properties based on distance/importance

### Best Practices

- **Single Active Camera**: Most games use one active camera at a time
- **Smooth Interpolation**: Use interpolation for camera movements to avoid jarring transitions
- **Aspect Ratio Handling**: Handle different aspect ratios for various displays
- **Clipping Optimization**: Adjust near/far planes based on scene requirements

## Architecture Diagrams

### Camera Class Hierarchy

```
UObject (Base Object)
├── UUID: Unique identifier
├── Name: Object name
├── Properties: Reflection system
├── Events: Object lifecycle
└── Serialization: Data persistence

ITickable (Update Interface)
└── Tick(deltaTime): Frame update method

AActor (Game Actor)
├── Transform: Position/Rotation/Scale
├── Active State: Update enable/disable
├── Components: Modular functionality
├── Network State: Prediction/Reconciliation
├── Interpolation: Smooth synchronization
├── Movement RPCs: Network movement sync
└── Lifecycle: BeginPlay/Tick/EndPlay

ACamera (Camera Actor)
├── ViewTransform: Camera view matrix
├── FieldOfView: Vertical FOV (degrees)
├── AspectRatio: Width/height ratio
├── NearClipPlane: Near clipping distance
├── FarClipPlane: Far clipping distance
└── Rendering Integration: View/projection matrices
```

### Camera Coordinate System

```
World Space ──► View Space ──► Clip Space ──► Screen Space
     │              │              │              │
     ▼              ▼              ▼              ▼
   Objects     Camera-centric   Normalized     Pixel coordinates
   positioned  coordinates      device coords  (0,0) to (width,height)
   in world
```

### Multi-Camera System Architecture

```
Game World
├── Main Camera (Third-person view)
├── Overhead Camera (Top-down view)
├── Side Camera (Profile view)
└── Cinematic Camera (Cutscene view)

Renderer
├── Active Camera: Current rendering camera
├── Viewport System: Multiple viewports support
├── Matrix Cache: Cached view/projection matrices
└── Frustum Culling: Visibility optimization
```

## Troubleshooting

### Common Issues

#### Camera Not Rendering

**Symptoms:** Black screen or no scene visible

**Possible Causes:**
- Camera not set as active in renderer
- Camera positioned incorrectly
- Clipping planes too restrictive
- Field of view too narrow

**Solutions:**
```cpp
// Check if camera is active
void DebugCameraRendering(ACamera* camera, URenderer* renderer) {
    if (renderer->GetActiveCamera() != camera) {
        FLogger::Log(ELogLevel::Error, "Camera is not set as active");
        renderer->SetActiveCamera(camera);
    }

    // Check camera position
    FVector3 pos = camera->ViewTransform.GetPosition();
    FLogger::Log(ELogLevel::Debug,
        FString::Format("Camera position: ({}, {}, {})", pos.x, pos.y, pos.z));

    // Check clipping planes
    if (camera->NearClipPlane >= camera->FarClipPlane) {
        FLogger::Log(ELogLevel::Error, "Invalid clipping planes");
        camera->NearClipPlane = 0.1f;
        camera->FarClipPlane = 1000.0f;
    }

    // Check field of view
    if (camera->FieldOfView <= 0.0f || camera->FieldOfView >= 180.0f) {
        FLogger::Log(ELogLevel::Error, "Invalid field of view");
        camera->FieldOfView = 90.0f;
    }
}
```

#### Incorrect Aspect Ratio

**Symptoms:** Stretched or squished rendering

**Possible Causes:**
- Aspect ratio doesn't match viewport
- Incorrect calculation
- Display resolution changes not handled

**Solutions:**
```cpp
// Update aspect ratio when viewport changes
void UpdateCameraAspectRatio(ACamera* camera, UInt32 width, UInt32 height) {
    if (height > 0) {
        camera->AspectRatio = static_cast<Float32>(width) / static_cast<Float32>(height);
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Updated aspect ratio to: {}", camera->AspectRatio));
    }
}

// Handle window resize
class WindowResizeHandler {
public:
    void OnWindowResized(UInt32 newWidth, UInt32 newHeight) {
        for (ACamera* camera : cameras) {
            UpdateCameraAspectRatio(camera, newWidth, newHeight);
        }
    }

private:
    TVector<ACamera*> cameras;
};
```

#### Camera Jittering or Stuttering

**Symptoms:** Camera movement appears jerky

**Possible Causes:**
- No interpolation between positions
- Frame rate dependent movement
- Floating point precision issues

**Solutions:**
```cpp
class SmoothCameraController : public AActor {
public:
    void MoveCameraTo(const FVector3& targetPosition, Float32 duration) {
        startPosition = controlledCamera->ViewTransform.GetPosition();
        endPosition = targetPosition;
        moveDuration = duration;
        moveTime = 0.0f;
        isMoving = true;
    }

    virtual void Tick(Float32 deltaTime) override {
        AActor::Tick(deltaTime);

        if (isMoving) {
            moveTime += deltaTime;
            Float32 t = FMath::Clamp(moveTime / moveDuration, 0.0f, 1.0f);

            // Smooth interpolation
            Float32 easedT = EaseOutCubic(t);
            FVector3 newPos = FMath::Lerp(startPosition, endPosition, easedT);

            controlledCamera->ViewTransform.SetPosition(newPos);

            if (t >= 1.0f) {
                isMoving = false;
                OnMovementComplete();
            }
        }
    }

private:
    ACamera* controlledCamera;
    FVector3 startPosition;
    FVector3 endPosition;
    Float32 moveTime;
    Float32 moveDuration;
    bool isMoving;

    Float32 EaseOutCubic(Float32 t) const {
        return 1 - pow(1 - t, 3);
    }

    UFunction<> OnMovementComplete;
};
```

#### Objects Disappearing or Clipping

**Symptoms:** Near or far objects not rendering

**Possible Causes:**
- Near clip plane too far
- Far clip plane too close
- Objects outside frustum

**Solutions:**
```cpp
// Debug clipping issues
void DebugClipping(ACamera* camera, const TVector<AActor*>& actors) {
    FLogger::Log(ELogLevel::Debug,
        FString::Format("Near: {}, Far: {}", camera->NearClipPlane, camera->FarClipPlane));

    for (AActor* actor : actors) {
        FVector3 pos = actor->GetTransform().GetPosition();
        FVector3 cameraPos = camera->ViewTransform.GetPosition();
        Float32 distance = (pos - cameraPos).Length();

        FLogger::Log(ELogLevel::Debug,
            FString::Format("Actor {} distance: {}", actor->GetName(), distance));

        if (distance < camera->NearClipPlane) {
            FLogger::Log(ELogLevel::Warning,
                FString::Format("Actor {} is too close to camera", actor->GetName()));
        } else if (distance > camera->FarClipPlane) {
            FLogger::Log(ELogLevel::Warning,
                FString::Format("Actor {} is too far from camera", actor->GetName()));
        }
    }
}

// Adjust clipping planes based on scene
void AutoAdjustClippingPlanes(ACamera* camera, const TVector<AActor*>& actors) {
    if (actors.empty()) return;

    Float32 minDistance = FLT_MAX;
    Float32 maxDistance = 0.0f;
    FVector3 cameraPos = camera->ViewTransform.GetPosition();

    for (AActor* actor : actors) {
        Float32 distance = (actor->GetTransform().GetPosition() - cameraPos).Length();
        minDistance = FMath::Min(minDistance, distance);
        maxDistance = FMath::Max(maxDistance, distance);
    }

    // Set near plane to 10% of minimum distance (with minimum)
    camera->NearClipPlane = FMath::Max(minDistance * 0.1f, 0.01f);

    // Set far plane to 150% of maximum distance
    camera->FarClipPlane = maxDistance * 1.5f;

    FLogger::Log(ELogLevel::Debug,
        FString::Format("Auto-adjusted clipping: near={}, far={}",
            camera->NearClipPlane, camera->FarClipPlane));
}
```

### Debug Tools

Enable comprehensive camera debugging:

```cpp
class CameraDebugger {
public:
    static void LogCameraState(ACamera* camera) {
        FLogger::Log(ELogLevel::Debug, "=== Camera Debug Info ===");
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Name: {}", camera->GetName()));
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Active: {}", camera->IsActive()));

        FTransform viewTransform = camera->ViewTransform.Get();
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Position: ({}, {}, {})",
                viewTransform.GetPosition().x,
                viewTransform.GetPosition().y,
                viewTransform.GetPosition().z));

        FLogger::Log(ELogLevel::Debug,
            FString::Format("FOV: {}°", camera->FieldOfView.Get()));
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Aspect: {}", camera->AspectRatio.Get()));
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Near: {}, Far: {}",
                camera->NearClipPlane.Get(), camera->FarClipPlane.Get()));
    }

    static void VisualizeCameraFrustum(ACamera* camera, URenderer* renderer) {
        // Draw camera frustum for debugging
        FMatrix4x4 viewProj = renderer->GetViewProjectionMatrix(camera);

        // Calculate frustum corners in world space
        // (Implementation would draw debug lines/planes)
        DrawFrustumWireframe(camera);
    }

    static void TestCameraMatrices(ACamera* camera, URenderer* renderer) {
        FMatrix4x4 view = renderer->GetViewMatrix(camera);
        FMatrix4x4 proj = renderer->GetProjectionMatrix(camera);
        FMatrix4x4 viewProj = view * proj;

        // Test matrix validity
        if (!view.IsValid() || !proj.IsValid() || !viewProj.IsValid()) {
            FLogger::Log(ELogLevel::Error, "Invalid camera matrices");
        }

        // Test point projection
        FVector3 testPoint(0, 0, -10);
        FVector4 clipSpace = viewProj * FVector4(testPoint, 1.0f);
        if (clipSpace.w != 0) {
            FVector3 ndc = FVector3(clipSpace.x, clipSpace.y, clipSpace.z) / clipSpace.w;
            FLogger::Log(ELogLevel::Debug,
                FString::Format("Test point NDC: ({}, {}, {})", ndc.x, ndc.y, ndc.z));
        }
    }
};
```

## Future Enhancements

### Planned Features

1. **Advanced Projections**: Orthographic, fisheye, and custom projection matrices
2. **Camera Effects**: Depth of field, motion blur, screen space effects
3. **Multiple Viewports**: Split-screen and picture-in-picture support
4. **Camera Paths**: Spline-based camera movement and animation
5. **Dynamic FOV**: Automatic field of view adjustment based on speed/content
6. **Camera Shakes**: Procedural camera shake effects
7. **Cinematic Tools**: Camera rails, dolly tracks, and crane movements

### Extension Points

The camera system allows for easy extension:

```cpp
class AdvancedCamera : public ACamera {
public:
    // Add post-processing effects
    virtual void ApplyPostProcessing(FRenderTarget* target) {
        // Custom post-processing
    }

    // Add custom projection
    virtual FMatrix4x4 GetCustomProjectionMatrix() const {
        // Custom projection logic
    }

    // Add camera collision
    virtual void UpdateCameraCollision() {
        // Prevent camera from clipping through geometry
    }

    // Add smooth following
    virtual void FollowTargetSmooth(AActor* target, Float32 damping) {
        // Smooth target following with damping
    }
};

// Camera component system
class UCameraComponent : public UActorComponent {
public:
    virtual void UpdateCamera(ACamera* camera) = 0;
};

class UFollowCameraComponent : public UCameraComponent {
public:
    virtual void UpdateCamera(ACamera* camera) override {
        if (target) {
            // Update camera to follow target
            FVector3 targetPos = target->GetTransform().GetPosition();
            camera->ViewTransform.SetPosition(targetPos + offset);
        }
    }

private:
    AActor* target;
    FVector3 offset;
};
```

### Advanced Features

Future versions may include:
- **VR Support**: Stereoscopic rendering and head tracking
- **Ray Tracing**: Hardware-accelerated ray tracing integration
- **Volumetric Effects**: Fog, volumetric lighting, and atmospheric effects
- **Camera Networks**: Distributed camera systems for multiplayer
- **AI Cameras**: Procedural camera placement and movement
- **Performance Profiling**: Detailed rendering performance metrics
- **Mobile Optimization**: Power-efficient camera systems for mobile devices

---

*This documentation covers the complete ACamera implementation as of TKD Engine v1.0.0. For the latest updates and additional examples, refer to the engine's source code and test suites.*
