# Vector2

The `TVector2` class is a template-based 2D vector implementation for arithmetic types. It provides comprehensive vector operations commonly used in graphics programming, physics simulations, and mathematical computations.

## Features

* Template-based design supporting all arithmetic types (int, float, double, etc.)
* Static predefined vectors (Zero, One, UnitX, UnitY)
* Component-wise arithmetic operations (addition, subtraction, multiplication, division)
* Scalar arithmetic operations
* Comparison operations with component-wise logic
* Array-style element access via operator[]
* Stream output support
* Comprehensive mathematical functions (normalization, projection, reflection, etc.)
* Cross product calculation for 2D vectors
* Distance and length calculations
* Component-wise utility functions (min, max, floor, ceil, etc.)

## Usage

### Basic Construction and Access

```cpp
// Default constructor (0, 0)
tkd::TVector2<float> vec1;

// Component constructor
tkd::TVector2<float> vec2(3.0f, 4.0f);

// Copy constructor
tkd::TVector2<float> vec3(vec2);

// Type conversion constructor
tkd::TVector2<float> vec4(5, 7); // int to float conversion

// Access components
float x = vec2.x;
float y = vec2.y;

// Array-style access
float firstComponent = vec2[0]; // x component
float secondComponent = vec2[1]; // y component

// Using predefined vectors
tkd::TVector2<float> zero = tkd::TVector2<float>::Zero;
tkd::TVector2<float> unitX = tkd::TVector2<float>::UnitX;
```

### Arithmetic Operations

```cpp
tkd::TVector2<float> a(2.0f, 3.0f);
tkd::TVector2<float> b(1.0f, 4.0f);

// Vector operations
tkd::TVector2<float> sum = a + b;           // (3, 7)
tkd::TVector2<float> diff = a - b;          // (1, -1)
tkd::TVector2<float> product = a * b;       // Component-wise: (2, 12)
tkd::TVector2<float> quotient = a / b;      // Component-wise: (2, 0.75)

// Scalar operations
tkd::TVector2<float> scaled = a * 2.0f;     // (4, 6)
tkd::TVector2<float> offset = a + 1.0f;     // (3, 4)

// Compound assignment
a += b;    // a becomes (3, 7)
a *= 2.0f; // a becomes (6, 14)

// Unary operations
tkd::TVector2<float> negated = -a;          // (-6, -14)
++a;       // Pre-increment: a becomes (7, 15)
a++;       // Post-increment: a becomes (8, 16)
```

### Mathematical Functions

```cpp
tkd::TVector2<float> vec1(3.0f, 4.0f);
tkd::TVector2<float> vec2(1.0f, 0.0f);

// Length and distance
float length = tkd::Length(vec1);                    // 5.0
float distance = tkd::Distance(vec1, vec2);          // Distance between vectors

// Normalization
tkd::TVector2<float> normalized = tkd::Normalize(vec1); // (0.6, 0.8)

// Dot product and cross product
float crossProduct = tkd::Cross(vec1, vec2);         // 2D cross product (scalar)

// Projection and reflection
tkd::TVector2<float> projected = tkd::Project(vec1, vec2);
tkd::TVector2<float> reflected = tkd::Reflect(vec1, tkd::TVector2<float>(0, 1));

// Component-wise operations
tkd::TVector2<float> absolute = tkd::Absolute(tkd::TVector2<float>(-1, -2)); // (1, 2)
tkd::TVector2<float> minimum = tkd::Minimum(vec1, vec2);
tkd::TVector2<float> maximum = tkd::Maximum(vec1, vec2);
```

### Utility Functions

```cpp
tkd::TVector2<float> vec(3.7f, -2.3f);

// Rounding operations
tkd::TVector2<float> floored = tkd::Floor(vec);      // (3, -3)
tkd::TVector2<float> ceiled = tkd::Ceil(vec);        // (4, -2)
tkd::TVector2<float> fractional = tkd::Fraction(vec); // (0.7, 0.7)

// Modulo and wrapping
tkd::TVector2<float> modded = tkd::Modulo(vec, 2.0f);
tkd::TVector2<float> wrapped = tkd::Wrap(vec, -1.0f, 1.0f);
tkd::TVector2<float> snapped = tkd::Snap(vec, 0.5f);   // Snap to 0.5 multiples

// Trigonometric functions
tkd::TVector2<float> angles(0.5f, 1.0f);
tkd::TVector2<float> sines = tkd::Sine(angles);
tkd::TVector2<float> cosines = tkd::Cosine(angles);
tkd::TVector2<float> tangents = tkd::Tangent(angles);
```

### Comparison Operations

```cpp
tkd::TVector2<float> a(1.0f, 2.0f);
tkd::TVector2<float> b(3.0f, 4.0f);

// Component-wise comparisons
bool equal = (a == b);        // false
bool notEqual = (a != b);     // true
bool less = (a < b);          // true (both components less)
bool greater = (a > b);       // false
bool lessEqual = (a <= b);    // true
bool greaterEqual = (a >= b); // false

// Scalar comparisons
bool allGreaterThan = (b > 2.0f); // true (both components > 2)
bool allEqual = (a == 1.0f);      // false (only x == 1)
```

### Stream Output

```cpp
tkd::TVector2<float> vec(3.14f, 2.71f);
std::cout << vec << std::endl; // Outputs: (3.14, 2.71)
```

## Type Aliases

Common type aliases can be defined for convenience:

```cpp
using Vec2f = tkd::TVector2<float>;
using Vec2d = tkd::TVector2<double>;
using Vec2i = tkd::TVector2<int>;

Vec2f position(100.0f, 200.0f);
Vec2i screenCoords(1920, 1080);
```

## Notes

* All division operations include assertion checks for zero denominators
* The class uses a union for component access, allowing both named access (x, y) and array access (data[])
* Template instantiation requires arithmetic types only (enforced by static_assert)
* Mathematical functions like Length, Normalize, and trigonometric functions require floating-point types for meaningful results