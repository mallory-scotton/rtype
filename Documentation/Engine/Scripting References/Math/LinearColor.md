# FLinearColor

## Overview

The `FLinearColor` class represents a color with byte-based components in the range [0, 255]. It provides a convenient way to work with colors in linear byte space, with support for conversion to and from `FColor` (floating-point colors) and packed UInt32 representation.

## Public Members

### Data Members

The class uses a union to allow access to color components in two ways:

- **r, g, b, a**: Individual byte components (UInt8)
  - `r`: Red component (0 to 255)
  - `g`: Green component (0 to 255)
  - `b`: Blue component (0 to 255)
  - `a`: Alpha component (0 to 255)

- **color**: Packed UInt32 representation of the color
  - The color packed as a single 32-bit unsigned integer

## Static Members

The class provides predefined color constants:

- `FLinearColor::White` - White color (255, 255, 255, 255)
- `FLinearColor::Black` - Black color (0, 0, 0, 255)
- `FLinearColor::Red` - Red color (255, 0, 0, 255)
- `FLinearColor::Green` - Green color (0, 255, 0, 255)
- `FLinearColor::Blue` - Blue color (0, 0, 255, 255)
- `FLinearColor::Yellow` - Yellow color (255, 255, 0, 255)
- `FLinearColor::Cyan` - Cyan color (0, 255, 255, 255)
- `FLinearColor::Magenta` - Magenta color (255, 0, 255, 255)
- `FLinearColor::Transparent` - Transparent color (0, 0, 0, 0)

## Constructors

### Default Constructor

```cpp
FLinearColor::FLinearColor(void)
```

Initializes the color to black with full opacity (0, 0, 0, 255).

### Parameterized Constructor

```cpp
FLinearColor::FLinearColor(UInt8 r, UInt8 g, UInt8 b, UInt8 a = 255)
```

Creates a color with the specified components. All values are clamped to the range [0, 255].

**Parameters:**
- `r`: Red component
- `g`: Green component
- `b`: Blue component
- `a`: Alpha component (defaults to 255 for full opacity)

### UInt32 Constructor

```cpp
FLinearColor::FLinearColor(UInt32 color)
```

Creates a color from a packed UInt32 value.

**Parameters:**
- `color`: Color represented as a single UInt32 value

### Conversion Constructor

```cpp
FLinearColor::FLinearColor(const FColor& other)
```

Creates an `FLinearColor` from an `FColor` by converting normalized floats (0.0-1.0) to byte values (0-255).

**Parameters:**
- `other`: The `FColor` to convert from

## Operators

### UInt32 Conversion Operator

```cpp
FLinearColor::operator UInt32(void) const
```

Converts the `FLinearColor` to a packed UInt32 representation.

**Return Value:** The color as a single UInt32 value.

### FColor Conversion Operator

```cpp
FLinearColor::operator FColor(void) const
```

Converts the `FLinearColor` to an `FColor` by scaling byte values (0-255) to normalized floats (0.0-1.0).

**Return Value:** An `FColor` equivalent of this color.

## Methods

### ToColor

```cpp
FColor FLinearColor::ToColor(void) const
```

Converts this `FLinearColor` to an `FColor`.

**Return Value:** An `FColor` equivalent of this color.

### FromColor (Static)

```cpp
static FLinearColor FLinearColor::FromColor(const FColor& color)
```

Creates an `FLinearColor` from an `FColor`.

**Parameters:**
- `color`: The `FColor` to convert from

**Return Value:** An `FLinearColor` equivalent of the input `FColor`.

## Global Operators

### Equality Operators

```cpp
bool operator==(const FLinearColor& lhs, const FLinearColor& rhs)
bool operator!=(const FLinearColor& lhs, const FLinearColor& rhs)
```

Compares two `FLinearColor` objects for equality/inequality.

**Parameters:**
- `lhs`: Left-hand side `FLinearColor`
- `rhs`: Right-hand side `FLinearColor`

**Return Value:** `true` if colors are equal/unequal, `false` otherwise.

### Mixed Equality Operators

```cpp
bool operator==(const FLinearColor& lhs, const FColor& rhs)
bool operator!=(const FLinearColor& lhs, const FColor& rhs)
```

Compares an `FLinearColor` with an `FColor` for equality/inequality.

**Parameters:**
- `lhs`: The `FLinearColor` to compare
- `rhs`: The `FColor` to compare

**Return Value:** `true` if colors are equal/unequal, `false` otherwise.

## Examples

### Creating Colors

```cpp
// Default constructor - creates black color
FLinearColor blackColor;

// Parameterized constructor
FLinearColor customColor(128, 64, 255, 255);

// Using predefined colors
FLinearColor red = FLinearColor::Red;
FLinearColor green = FLinearColor::Green;
FLinearColor blue = FLinearColor::Blue;
```

### Color Conversion

```cpp
// Convert from FColor to FLinearColor
FColor floatColor(0.5f, 0.25f, 1.0f, 1.0f);
FLinearColor linearColor = FLinearColor::FromColor(floatColor);

// Convert from FLinearColor to FColor
FLinearColor linearColor(128, 64, 255, 255);
FColor floatColor = linearColor.ToColor();
```

### Packed UInt32 Representation

```cpp
// Create color from UInt32
UInt32 packedColor = 0xFF8000FF;  // Red: 255, Green: 128, Blue: 0, Alpha: 255
FLinearColor color(packedColor);

// Get packed representation
UInt32 packed = static_cast<UInt32>(color);
```

### Accessing Components

```cpp
FLinearColor color(128, 64, 255, 255);

// Access via struct members
UInt8 red = color.r;
UInt8 green = color.g;
UInt8 blue = color.b;
UInt8 alpha = color.a;

// Access via packed color
UInt32 packed = color.color;
```

### Color Comparison

```cpp
FLinearColor color1(255, 0, 0, 255);
FLinearColor color2(255, 0, 0, 255);
FColor floatColor(1.0f, 0.0f, 0.0f, 1.0f);

bool areEqual = (color1 == color2);  // true
bool areEqualMixed = (color1 == floatColor);  // true
bool areNotEqual = (color1 != color2);  // false
```
