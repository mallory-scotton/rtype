# IDrawable

## Overview

The `IDrawable` class is an abstract base class that defines the interface for objects that can be drawn to a render target. It provides a pure virtual `Draw` method that must be implemented by derived classes to specify how the object should be rendered. This class serves as the foundation for all drawable entities in the engine, ensuring a consistent drawing interface across different types of renderable objects.

## Destructor

### virtual ~IDrawable() = default

Default virtual destructor for the `IDrawable` class. It allows proper cleanup of derived classes through polymorphism.

## Methods

### virtual void Draw(IRenderer& target, FRenderStates states) const = 0

Draws the object on the specified render target using the provided render states. This is a pure virtual method that must be overridden by any concrete drawable class to define the specific drawing behavior.

**Parameters:**
- `target`: The render target to draw the object on
- `states`: The current render states to apply during drawing

**Note:** This method is declared as `const` to indicate that drawing does not modify the drawable object's state.
