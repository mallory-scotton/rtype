# Vector3

The `TVector3` class is a template-based 3D vector implementation for arithmetic types. It provides comprehensive vector operations commonly used in 3D graphics programming, physics simulations, game development, and mathematical computations.

## Features

* Template-based design supporting all arithmetic types (int, float, double, etc.)
* Static predefined vectors (Zero, One, UnitX, UnitY, UnitZ)
* Component-wise arithmetic operations (addition, subtraction, multiplication, division)
* Scalar arithmetic operations
* Comparison operations with component-wise logic
* Array-style element access via operator[]
* Stream output support
* Comprehensive 3D mathematical functions (normalization, projection, reflection)
* 3D cross product calculation
* Distance and length calculations
* Component-wise utility functions (min, max, floor, ceil, etc.)

## Usage

### Basic Construction and Access

```cpp
// Default constructor (0, 0, 0)
tkd::TVector3<float> vec1;

// Component constructor
tkd::TVector3<float> vec2(1.0f, 2.0f, 3.0f);

// Copy constructor
tkd::TVector3<float> vec3(vec2);

// Type conversion constructor
tkd::TVector3<float> vec4(5, 7, 9); // int to float conversion

// Access components
float x = vec2.x;
float y = vec2.y;
float z = vec2.z;

// Array-style access
float firstComponent = vec2[0];  // x component
float secondComponent = vec2[1]; // y component
float thirdComponent = vec2[2];  // z component

// Using predefined vectors
tkd::TVector3<float> zero = tkd::TVector3<float>::Zero;   // (0, 0, 0)
tkd::TVector3<float> unitX = tkd::TVector3<float>::UnitX; // (1, 0, 0)
tkd::TVector3<float> unitY = tkd::TVector3<float>::UnitY; // (0, 1, 0)
tkd::TVector3<float> unitZ = tkd::TVector3<float>::UnitZ; // (0, 0, 1)
```

### Arithmetic Operations

```cpp
tkd::TVector3<float> a(2.0f, 3.0f, 4.0f);
tkd::TVector3<float> b(1.0f, 4.0f, 2.0f);

// Vector operations
tkd::TVector3<float> sum = a + b;           // (3, 7, 6)
tkd::TVector3<float> diff = a - b;          // (1, -1, 2)
tkd::TVector3<float> product = a * b;       // Component-wise: (2, 12, 8)
tkd::TVector3<float> quotient = a / b;      // Component-wise: (2, 0.75, 2)

// Scalar operations
tkd::TVector3<float> scaled = a * 2.0f;     // (4, 6, 8)
tkd::TVector3<float> offset = a + 1.0f;     // (3, 4, 5)

// Compound assignment
a += b;    // a becomes (3, 7, 6)
a *= 2.0f; // a becomes (6, 14, 12)

// Unary operations
tkd::TVector3<float> negated = -a;          // (-6, -14, -12)
++a;       // Pre-increment: a becomes (7, 15, 13)
a++;       // Post-increment: a becomes (8, 16, 14)
```

### 3D Mathematical Functions

```cpp
tkd::TVector3<float> vec1(1.0f, 2.0f, 3.0f);
tkd::TVector3<float> vec2(4.0f, 5.0f, 6.0f);

// Length and distance
float length = tkd::Length(vec1);                    // sqrt(14) ≈ 3.74
float distance = tkd::Distance(vec1, vec2);          // Distance between vectors

// Normalization
tkd::TVector3<float> normalized = tkd::Normalize(vec1); // Unit vector

// 3D Cross product (returns a vector)
tkd::TVector3<float> crossProduct = tkd::Cross(vec1, vec2);

// Projection and reflection
tkd::TVector3<float> projected = tkd::Project(vec1, vec2);
tkd::TVector3<float> normal(0.0f, 1.0f, 0.0f);
tkd::TVector3<float> reflected = tkd::Reflect(vec1, normal);

// Component-wise operations
tkd::TVector3<float> absolute = tkd::Absolute(tkd::TVector3<float>(-1, -2, -3)); // (1, 2, 3)
tkd::TVector3<float> minimum = tkd::Minimum(vec1, vec2);
tkd::TVector3<float> maximum = tkd::Maximum(vec1, vec2);
```

### Advanced Vector Operations

```cpp
tkd::TVector3<float> a(1.0f, 0.0f, 0.0f);
tkd::TVector3<float> b(0.0f, 1.0f, 0.0f);
tkd::TVector3<float> c(2.0f, 3.0f, 4.0f);

// 3D Cross product for calculating perpendicular vectors
tkd::TVector3<float> perpendicular = tkd::Cross(a, b); // Results in (0, 0, 1)

// Multiply-add operation (useful for physics calculations)
tkd::TVector3<float> result = tkd::MultiplyAdd(a, b, c); // a * b + c

// Vector operations for 3D transformations
tkd::TVector3<float> position(10.0f, 20.0f, 30.0f);
tkd::TVector3<float> velocity(1.0f, -2.0f, 0.5f);
tkd::TVector3<float> newPosition = position + velocity; // Simple physics integration
```

### Utility Functions

```cpp
tkd::TVector3<float> vec(3.7f, -2.3f, 1.9f);

// Rounding operations
tkd::TVector3<float> floored = tkd::Floor(vec);      // (3, -3, 1)
tkd::TVector3<float> ceiled = tkd::Ceil(vec);        // (4, -2, 2)
tkd::TVector3<float> fractional = tkd::Fraction(vec); // (0.7, 0.7, 0.9)

// Modulo and wrapping
tkd::TVector3<float> modded = tkd::Modulo(vec, 2.0f);
tkd::TVector3<float> wrapped = tkd::Wrap(vec, -1.0f, 1.0f);
tkd::TVector3<float> snapped = tkd::Snap(vec, 0.5f);   // Snap to 0.5 multiples

// Trigonometric functions (useful for rotation calculations)
tkd::TVector3<float> angles(0.5f, 1.0f, 1.57f);
tkd::TVector3<float> sines = tkd::Sine(angles);
tkd::TVector3<float> cosines = tkd::Cosine(angles);
tkd::TVector3<float> tangents = tkd::Tangent(angles);
```

### Comparison Operations

```cpp
tkd::TVector3<float> a(1.0f, 2.0f, 3.0f);
tkd::TVector3<float> b(4.0f, 5.0f, 6.0f);

// Component-wise comparisons
bool equal = (a == b);        // false
bool notEqual = (a != b);     // true
bool less = (a < b);          // true (all components less)
bool greater = (a > b);       // false
bool lessEqual = (a <= b);    // true
bool greaterEqual = (a >= b); // false

// Scalar comparisons
bool allGreaterThan = (b > 3.0f); // true (all components > 3)
bool allEqual = (a == 1.0f);      // false (only x == 1)
```

### Stream Output

```cpp
tkd::TVector3<float> vec(1.41f, 2.71f, 3.14f);
std::cout << vec << std::endl; // Outputs: (1.41, 2.71, 3.14)
```

### Common Use Cases

```cpp
// 3D Position and direction vectors
tkd::TVector3<float> playerPosition(100.0f, 0.0f, 50.0f);
tkd::TVector3<float> forward = tkd::TVector3<float>::UnitZ;

// RGB color representation
tkd::TVector3<float> redColor(1.0f, 0.0f, 0.0f);
tkd::TVector3<float> mixedColor = redColor * 0.5f; // 50% red

// 3D surface normal calculation
tkd::TVector3<float> edge1(1.0f, 0.0f, 0.0f);
tkd::TVector3<float> edge2(0.0f, 1.0f, 0.0f);
tkd::TVector3<float> surfaceNormal = tkd::Normalize(tkd::Cross(edge1, edge2));

// Physics calculations
tkd::TVector3<float> force(0.0f, -9.8f, 0.0f); // Gravity
tkd::TVector3<float> acceleration = force / mass; // F = ma
```

## Type Aliases

Common type aliases can be defined for convenience:

```cpp
using Vec3f = tkd::TVector3<float>;
using Vec3d = tkd::TVector3<double>;
using Vec3i = tkd::TVector3<int>;

Vec3f position(10.0f, 20.0f, 30.0f);
Vec3i voxelCoords(128, 64, 256);
Vec3d preciseCalculation(1.0, 2.0, 3.0);
```

## Notes

* All division operations include assertion checks for zero denominators
* The class uses a union for component access, allowing both named access (x, y, z) and array access (data[])
* Template instantiation requires arithmetic types only (enforced by static_assert)
* Mathematical functions like Length, Normalize, and trigonometric functions require floating-point types for meaningful results
* The 3D cross product returns a vector perpendicular to both input vectors (unlike 2D cross product which returns a scalar)
* Cross product follows the right-hand rule: Cross(UnitX, UnitY) = UnitZ