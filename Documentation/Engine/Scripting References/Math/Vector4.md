# Vector4

The `TVector4` class is a template-based 4D vector implementation for arithmetic types. It provides comprehensive vector operations commonly used in homogeneous coordinate systems, quaternion mathematics, RGBA color representation, and high-dimensional mathematical computations.

## Features

* Template-based design supporting all arithmetic types (int, float, double, etc.)
* Static predefined vectors (Zero, One, UnitX, UnitY, UnitZ, UnitW)
* Component-wise arithmetic operations (addition, subtraction, multiplication, division)
* Scalar arithmetic operations
* Comparison operations with component-wise logic
* Array-style element access via operator[]
* Stream output support
* Comprehensive 4D mathematical functions (normalization, distance, length)
* Component-wise utility functions (min, max, floor, ceil, etc.)

## Usage

### Basic Construction and Access

```cpp
// Default constructor (0, 0, 0, 0)
tkd::TVector4<float> vec1;

// Component constructor
tkd::TVector4<float> vec2(1.0f, 2.0f, 3.0f, 4.0f);

// Copy constructor
tkd::TVector4<float> vec3(vec2);

// Type conversion constructor
tkd::TVector4<float> vec4(5, 7, 9, 11); // int to float conversion

// Access components
float x = vec2.x;
float y = vec2.y;
float z = vec2.z;
float w = vec2.w;

// Array-style access
float firstComponent = vec2[0];  // x component
float secondComponent = vec2[1]; // y component
float thirdComponent = vec2[2];  // z component
float fourthComponent = vec2[3]; // w component

// Using predefined vectors
tkd::TVector4<float> zero = tkd::TVector4<float>::Zero;   // (0, 0, 0, 0)
tkd::TVector4<float> unitX = tkd::TVector4<float>::UnitX; // (1, 0, 0, 0)
tkd::TVector4<float> unitY = tkd::TVector4<float>::UnitY; // (0, 1, 0, 0)
tkd::TVector4<float> unitZ = tkd::TVector4<float>::UnitZ; // (0, 0, 1, 0)
tkd::TVector4<float> unitW = tkd::TVector4<float>::UnitW; // (0, 0, 0, 1)
```

### Arithmetic Operations

```cpp
tkd::TVector4<float> a(2.0f, 3.0f, 4.0f, 5.0f);
tkd::TVector4<float> b(1.0f, 4.0f, 2.0f, 3.0f);

// Vector operations
tkd::TVector4<float> sum = a + b;           // (3, 7, 6, 8)
tkd::TVector4<float> diff = a - b;          // (1, -1, 2, 2)
tkd::TVector4<float> product = a * b;       // Component-wise: (2, 12, 8, 15)
tkd::TVector4<float> quotient = a / b;      // Component-wise: (2, 0.75, 2, 1.67)

// Scalar operations
tkd::TVector4<float> scaled = a * 2.0f;     // (4, 6, 8, 10)
tkd::TVector4<float> offset = a + 1.0f;     // (3, 4, 5, 6)

// Compound assignment
a += b;    // a becomes (3, 7, 6, 8)
a *= 2.0f; // a becomes (6, 14, 12, 16)

// Unary operations
tkd::TVector4<float> negated = -a;          // (-6, -14, -12, -16)
++a;       // Pre-increment: a becomes (7, 15, 13, 17)
a++;       // Post-increment: a becomes (8, 16, 14, 18)
```

### 4D Mathematical Functions

```cpp
tkd::TVector4<float> vec1(1.0f, 2.0f, 3.0f, 4.0f);
tkd::TVector4<float> vec2(5.0f, 6.0f, 7.0f, 8.0f);

// Length and distance
float length = tkd::Length(vec1);                    // sqrt(30) ≈ 5.48
float distance = tkd::Distance(vec1, vec2);          // Distance between vectors

// Normalization
tkd::TVector4<float> normalized = tkd::Normalize(vec1); // Unit vector

// Component-wise operations
tkd::TVector4<float> absolute = tkd::Absolute(tkd::TVector4<float>(-1, -2, -3, -4)); // (1, 2, 3, 4)
tkd::TVector4<float> minimum = tkd::Minimum(vec1, vec2);
tkd::TVector4<float> maximum = tkd::Maximum(vec1, vec2);

// Multiply-add operation (useful for matrix operations)
tkd::TVector4<float> c(0.1f, 0.2f, 0.3f, 0.4f);
tkd::TVector4<float> result = tkd::MultiplyAdd(vec1, vec2, c); // vec1 * vec2 + c
```

### Utility Functions

```cpp
tkd::TVector4<float> vec(3.7f, -2.3f, 1.9f, -4.1f);

// Rounding operations
tkd::TVector4<float> floored = tkd::Floor(vec);      // (3, -3, 1, -5)
tkd::TVector4<float> ceiled = tkd::Ceil(vec);        // (4, -2, 2, -4)
tkd::TVector4<float> fractional = tkd::Fraction(vec); // (0.7, 0.7, 0.9, 0.9)

// Modulo and wrapping
tkd::TVector4<float> modded = tkd::Modulo(vec, 2.0f);
tkd::TVector4<float> wrapped = tkd::Wrap(vec, -1.0f, 1.0f);
tkd::TVector4<float> snapped = tkd::Snap(vec, 0.5f);   // Snap to 0.5 multiples

// Trigonometric functions
tkd::TVector4<float> angles(0.5f, 1.0f, 1.57f, 3.14f);
tkd::TVector4<float> sines = tkd::Sine(angles);
tkd::TVector4<float> cosines = tkd::Cosine(angles);
tkd::TVector4<float> tangents = tkd::Tangent(angles);
```

### Comparison Operations

```cpp
tkd::TVector4<float> a(1.0f, 2.0f, 3.0f, 4.0f);
tkd::TVector4<float> b(5.0f, 6.0f, 7.0f, 8.0f);

// Component-wise comparisons
bool equal = (a == b);        // false
bool notEqual = (a != b);     // true
bool less = (a < b);          // true (all components less)
bool greater = (a > b);       // false
bool lessEqual = (a <= b);    // true
bool greaterEqual = (a >= b); // false

// Scalar comparisons
bool allGreaterThan = (b > 4.0f); // true (all components > 4)
bool allEqual = (a == 1.0f);      // false (only x == 1)
```

### Stream Output

```cpp
tkd::TVector4<float> vec(1.0f, 2.0f, 3.0f, 4.0f);
std::cout << vec << std::endl; // Outputs: (1, 2, 3, 4)
```

### Common Use Cases

```cpp
// RGBA Color representation
tkd::TVector4<float> redColor(1.0f, 0.0f, 0.0f, 1.0f); // Red with full alpha
tkd::TVector4<float> transparentBlue(0.0f, 0.0f, 1.0f, 0.5f); // Semi-transparent blue

// Homogeneous coordinates for 3D graphics
tkd::TVector4<float> point3D(10.0f, 20.0f, 30.0f, 1.0f);     // 3D point
tkd::TVector4<float> direction3D(1.0f, 0.0f, 0.0f, 0.0f);    // 3D direction vector

// Quaternion representation (x, y, z, w)
tkd::TVector4<float> quaternion(0.0f, 0.0f, 0.0f, 1.0f);     // Identity quaternion

// Multi-dimensional data processing
tkd::TVector4<float> dataPoint(temperature, humidity, pressure, altitude);

// Shader uniform parameters
tkd::TVector4<float> materialProperties(metallic, roughness, emission, occlusion);
```

### Graphics Programming Applications

```cpp
// Color blending
tkd::TVector4<float> color1(1.0f, 0.0f, 0.0f, 0.8f); // Red
tkd::TVector4<float> color2(0.0f, 1.0f, 0.0f, 0.6f); // Green
tkd::TVector4<float> blended = (color1 + color2) * 0.5f;

// Matrix-vector multiplication preparation
tkd::TVector4<float> vertex(x, y, z, 1.0f); // Homogeneous coordinate
// This vector can be multiplied by a 4x4 transformation matrix

// Texture coordinates with layers
tkd::TVector4<float> texCoord(u, v, layer, mipLevel);

// Lighting calculations
tkd::TVector4<float> lightColor(r, g, b, intensity);
tkd::TVector4<float> materialColor(r, g, b, shininess);
```

### Advanced Mathematical Operations

```cpp
// Statistical calculations
tkd::TVector4<float> dataset1(1.0f, 2.0f, 3.0f, 4.0f);
tkd::TVector4<float> dataset2(5.0f, 6.0f, 7.0f, 8.0f);

// Component-wise statistics
tkd::TVector4<float> means = (dataset1 + dataset2) * 0.5f;
tkd::TVector4<float> ranges = tkd::Maximum(dataset1, dataset2) - tkd::Minimum(dataset1, dataset2);

// Physics simulations (position, velocity as 4D)
tkd::TVector4<float> state(x, y, z, time);
tkd::TVector4<float> derivative(vx, vy, vz, dt);
tkd::TVector4<float> newState = state + derivative;
```

## Type Aliases

Common type aliases can be defined for convenience:

```cpp
using Vec4f = tkd::TVector4<float>;
using Vec4d = tkd::TVector4<double>;
using Vec4i = tkd::TVector4<int>;
using Color4f = tkd::TVector4<float>;  // For RGBA colors
using Quaternionf = tkd::TVector4<float>; // For quaternions

Vec4f position(10.0f, 20.0f, 30.0f, 1.0f);
Color4f redColor(1.0f, 0.0f, 0.0f, 1.0f);
Quaternionf rotation(0.0f, 0.0f, 0.0f, 1.0f);
```

## Notes

* All division operations include assertion checks for zero denominators
* The class uses a union for component access, allowing both named access (x, y, z, w) and array access (data[])
* Template instantiation requires arithmetic types only (enforced by static_assert)
* Mathematical functions like Length, Normalize, and trigonometric functions require floating-point types for meaningful results
* Unlike TVector3, TVector4 does not have a cross product function (cross product is specific to 3D vectors)
* The fourth component (w) is commonly used for:
  - Alpha channel in RGBA colors
  - Homogeneous coordinate (1.0 for points, 0.0 for directions) in 3D graphics
  - Scalar component in quaternions
  - Time dimension in 4D spacetime calculations