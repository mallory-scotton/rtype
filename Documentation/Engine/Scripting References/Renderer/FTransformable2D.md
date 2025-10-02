# FTransformable2D

## Overview

The `FTransformable2D` class provides a base implementation for objects that can be transformed in 2D space. It encapsulates common transformation properties such as position, rotation, scale, and origin, and provides methods to manipulate these properties. This class is designed to be inherited by drawable or renderable objects that require 2D transformations.

The class maintains a combined transformation matrix that represents the cumulative effect of all transformations applied in the correct order: translation to the origin, scaling, rotation, and translation to the final position. It also provides an inverse transformation matrix for operations that require transforming points from world space back to local space.

## Constructor

### FTransformable2D()

Default constructor that initializes the transformable object with default values:
- Origin: (0, 0)
- Position: (0, 0)
- Rotation: 0 degrees
- Scale: (1, 1)

## Setters

### void SetPosition(float x, float y)

Sets the position of the object to the specified coordinates.

**Parameters:**
- `x`: The x-coordinate of the new position
- `y`: The y-coordinate of the new position

### void SetPosition(const FVector2& position)

Sets the position of the object using a vector.

**Parameters:**
- `position`: The new position as an FVector2

### void SetRotation(float angle)

Sets the rotation angle of the object.

**Parameters:**
- `angle`: The new rotation angle in degrees (normalized to 0-360 range)

### void SetRotation(const FRotator2D& rotation)

Sets the rotation using a rotator object.

**Parameters:**
- `rotation`: The new rotation as an FRotator2D

### void SetScale(float x, float y)

Sets the scale factors for the object.

**Parameters:**
- `x`: The x-axis scale factor
- `y`: The y-axis scale factor

### void SetScale(const FVector2& scale)

Sets the scale using a vector.

**Parameters:**
- `scale`: The new scale factors as an FVector2

### void SetOrigin(float x, float y)

Sets the origin point for transformations.

**Parameters:**
- `x`: The x-coordinate of the origin
- `y`: The y-coordinate of the origin

### void SetOrigin(const FVector2& origin)

Sets the origin using a vector.

**Parameters:**
- `origin`: The new origin as an FVector2

## Getters

### const FVector2& GetPosition() const

Returns the current position of the object.

**Returns:** The position as an FVector2

### float GetRotation() const

Returns the current rotation angle.

**Returns:** The rotation angle in degrees

### const FVector2& GetScale() const

Returns the current scale factors.

**Returns:** The scale as an FVector2

### const FVector2& GetOrigin() const

Returns the current origin point.

**Returns:** The origin as an FVector2

## Relative Transformations

### void Move(float offsetX, float offsetY)

Moves the object by the specified offset relative to its current position.

**Parameters:**
- `offsetX`: The x-offset to move by
- `offsetY`: The y-offset to move by

### void Move(const FVector2& offset)

Moves the object by the specified offset vector.

**Parameters:**
- `offset`: The offset vector to move by

### void Rotate(float angle)

Rotates the object by the specified angle relative to its current rotation.

**Parameters:**
- `angle`: The angle to rotate by in degrees

### void Rotate(const FRotator2D& rotation)

Rotates the object using a rotator object.

**Parameters:**
- `rotation`: The rotation to apply

### void Scale(float x, float y)

Scales the object by the specified factors relative to its current scale.

**Parameters:**
- `x`: The x-axis scaling factor
- `y`: The y-axis scaling factor

### void Scale(const FVector2& scale)

Scales the object using a scale vector.

**Parameters:**
- `scale`: The scaling factors to apply

## Transform Access

### const FTransform2D& GetTransform() const

Returns the combined transformation matrix that represents all applied transformations. The matrix is computed on-demand and cached until any transformation property changes.

**Returns:** The combined transformation matrix

### const FTransform2D& GetInverseTransform() const

Returns the inverse of the combined transformation matrix. This is useful for transforming points from world space back to the object's local space.

**Returns:** The inverse transformation matrix
