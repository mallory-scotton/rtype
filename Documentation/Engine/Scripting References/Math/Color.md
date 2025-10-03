# FColor

## Overview

The `FColor` class represents a color with floating-point components in the range [0.0, 1.0]. It provides a convenient way to work with colors in normalized floating-point space, with support for conversion to and from `FLinearColor` (byte-based colors).

## Public Members

### Data Members

The class uses a union to allow access to color components in two ways:

- **r, g, b, a**: Individual floating-point components (Float32)
  - `r`: Red component (0.0 to 1.0)
  - `g`: Green component (0.0 to 1.0)
  - `b`: Blue component (0.0 to 1.0)
  - `a`: Alpha component (0.0 to 1.0)

- **data[4]**: Array access to the color components
  - `data[0]`: Red component
  - `data[1]`: Green component
  - `data[2]`: Blue component
  - `data[3]`: Alpha component

## Static Members

The class provides predefined color constants:

- `FColor::White` - White color (1.0, 1.0, 1.0, 1.0)
- `FColor::Black` - Black color (0.0, 0.0, 0.0, 1.0)
- `FColor::Red` - Red color (1.0, 0.0, 0.0, 1.0)
- `FColor::Green` - Green color (0.0, 1.0, 0.0, 1.0)
- `FColor::Blue` - Blue color (0.0, 0.0, 1.0, 1.0)
- `FColor::Yellow` - Yellow color (1.0, 1.0, 0.0, 1.0)
- `FColor::Cyan` - Cyan color (0.0, 1.0, 1.0, 1.0)
- `FColor::Magenta` - Magenta color (1.0, 0.0, 1.0, 1.0)
- `FColor::Transparent` - Transparent color (0.0, 0.0, 0.0, 0.0)

## Constructors

### Default Constructor

```cpp
FColor::FColor(void)
```

Initializes the color to black with full opacity (0.0, 0.0, 0.0, 1.0).

### Parameterized Constructor

```cpp
FColor::FColor(Float32 r, Float32 g, Float32 b, Float32 a = 1.0f)
```

Creates a color with the specified components. All values are clamped to the range [0.0, 1.0].

**Parameters:**
- `r`: Red component
- `g`: Green component
- `b`: Blue component
- `a`: Alpha component (defaults to 1.0 for full opacity)

### Conversion Constructor

```cpp
FColor::FColor(const FLinearColor& other)
```

Creates an `FColor` from an `FLinearColor` by converting byte values (0-255) to normalized floats (0.0-1.0).

**Parameters:**
- `other`: The `FLinearColor` to convert from

## Operators

### Conversion Operator

```cpp
FColor::operator FLinearColor(void) const
```

Converts the `FColor` to an `FLinearColor` by scaling float values (0.0-1.0) to byte values (0-255).

**Return Value:** An `FLinearColor` equivalent of this color.

## Methods

### ToLinearColor

```cpp
FLinearColor FColor::ToLinearColor(void) const
```

Converts this `FColor` to an `FLinearColor`.

**Return Value:** An `FLinearColor` equivalent of this color.

### FromLinearColor (Static)

```cpp
static FColor FColor::FromLinearColor(const FLinearColor& linearColor)
```

Creates an `FColor` from an `FLinearColor`.

**Parameters:**
- `linearColor`: The `FLinearColor` to convert from

**Return Value:** An `FColor` equivalent of the input `FLinearColor`.

## Global Operators

### Equality Operators

```cpp
bool operator==(const FColor& lhs, const FColor& rhs)
bool operator!=(const FColor& lhs, const FColor& rhs)
```

Compares two `FColor` objects for equality/inequality.

**Parameters:**
- `lhs`: Left-hand side `FColor`
- `rhs`: Right-hand side `FColor`

**Return Value:** `true` if colors are equal/unequal, `false` otherwise.

### Mixed Equality Operators

```cpp
bool operator==(const FColor& lhs, const FLinearColor& rhs)
bool operator!=(const FColor& lhs, const FLinearColor& rhs)
```

Compares an `FColor` with an `FLinearColor` for equality/inequality.

**Parameters:**
- `lhs`: The `FColor` to compare
- `rhs`: The `FLinearColor` to compare

**Return Value:** `true` if colors are equal/unequal, `false` otherwise.

## Examples

### Creating Colors

```cpp
// Default constructor - creates black color
FColor blackColor;

// Parameterized constructor
FColor customColor(0.5f, 0.7f, 0.9f, 1.0f);

// Using predefined colors
FColor red = FColor::Red;
FColor green = FColor::Green;
FColor blue = FColor::Blue;
```

### Color Conversion

```cpp
// Convert from FLinearColor to FColor
FLinearColor linearColor(128, 64, 255, 255);
FColor floatColor = FColor::FromLinearColor(linearColor);

// Convert from FColor to FLinearColor
FColor floatColor(0.5f, 0.25f, 1.0f, 1.0f);
FLinearColor linearColor = floatColor.ToLinearColor();
```

### Accessing Components

```cpp
FColor color(0.8f, 0.6f, 0.4f, 1.0f);

// Access via struct members
float red = color.r;
float green = color.g;
float blue = color.b;
float alpha = color.a;

// Access via array
float red2 = color.data[0];
float green2 = color.data[1];
float blue2 = color.data[2];
float alpha2 = color.data[3];
```

### Color Comparison

```cpp
FColor color1(1.0f, 0.0f, 0.0f, 1.0f);
FColor color2(1.0f, 0.0f, 0.0f, 1.0f);
FLinearColor linearColor(255, 0, 0, 255);

bool areEqual = (color1 == color2);  // true
bool areEqualMixed = (color1 == linearColor);  // true
bool areNotEqual = (color1 != color2);  // false
```
