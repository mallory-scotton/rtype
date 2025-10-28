# Matrix3x3

## Overview

The `TMatrix3x3` class is a template-based implementation of a 3x3 matrix, designed for mathematical operations in 2D graphics, transformations, and linear algebra computations. It supports various arithmetic operations, matrix manipulations, and provides essential utilities for game engine mathematics.

This class is part of the TKD Engine's core math library and is optimized for performance while maintaining type safety through C++ templates.

## Class Declaration

```cpp
namespace tkd
{
    template <typename T>
    class TMatrix3x3
    {
        // ... class members and methods
    };
}
```

## Template Parameters

- **T**: The numeric type for matrix elements. Must be an arithmetic type (integral or floating-point). Common instantiations include `float`, `double`, and `int`.

## Static Members

### Constants

- **`TMatrix3x3<T> Zero`**: A static constant representing the zero matrix.
  ```
  [ 0  0  0 ]
  [ 0  0  0 ]
  [ 0  0  0 ]
  ```

- **`TMatrix3x3<T> Identity`**: A static constant representing the identity matrix.
  ```
  [ 1  0  0 ]
  [ 0  1  0 ]
  [ 0  0  1 ]
  ```

## Data Members

- **`T data[3][3]`**: A 2D array storing the matrix elements. Elements are stored in row-major order.

Matrix layout:
```
data[0][0]  data[0][1]  data[0][2]
data[1][0]  data[1][1]  data[1][2]
data[2][0]  data[2][1]  data[2][2]
```

## Constructors

### Default Constructor

```cpp
TMatrix3x3(void);
```

Initializes the matrix to the identity matrix.

**Example:**
```cpp
tkd::TMatrix3x3<float> matrix;  // Creates identity matrix
```

### Parameterized Constructor

```cpp
TMatrix3x3(T m00, T m01, T m02, T m10, T m11, T m12, T m20, T m21, T m22);
```

Initializes the matrix with specified elements.

**Parameters:**
- `m00, m01, m02`: Elements of the first row
- `m10, m11, m12`: Elements of the second row
- `m20, m21, m22`: Elements of the third row

**Example:**
```cpp
tkd::TMatrix3x3<float> rotation(
    0.0f, -1.0f, 0.0f,  // First row
    1.0f,  0.0f, 0.0f,  // Second row
    0.0f,  0.0f, 1.0f   // Third row
);
```

## Operators

### Arithmetic Operators

#### Matrix Addition

```cpp
TMatrix3x3<T> operator+(const TMatrix3x3<T>& other) const;
```

Performs element-wise addition of two matrices.

**Example:**
```cpp
tkd::TMatrix3x3<float> a = tkd::TMatrix3x3<float>::Identity;
tkd::TMatrix3x3<float> b(1, 2, 3, 4, 5, 6, 7, 8, 9);
tkd::TMatrix3x3<float> result = a + b;
```

#### Matrix Subtraction

```cpp
TMatrix3x3<T> operator-(const TMatrix3x3<T>& other) const;
```

Performs element-wise subtraction of two matrices.

#### Matrix Multiplication

```cpp
TMatrix3x3<T> operator*(const TMatrix3x3<T>& other) const;
```

Performs matrix multiplication.

**Note:** Matrix multiplication is not commutative (A * B ≠ B * A).

**Example:**
```cpp
tkd::TMatrix3x3<float> transform = rotation * translation;
```

#### Scalar Multiplication

```cpp
TMatrix3x3<T> operator*(T scalar) const;
```

Multiplies each element of the matrix by a scalar value.

**Example:**
```cpp
tkd::TMatrix3x3<float> scaled = matrix * 2.0f;
```

### Comparison Operators

#### Equality

```cpp
bool operator==(const TMatrix3x3<T>& other) const;
```

Compares two matrices for exact equality.

#### Inequality

```cpp
bool operator!=(const TMatrix3x3<T>& other) const;
```

Compares two matrices for inequality.

### Access Operators

#### Element Access (Mutable)

```cpp
T& operator()(SizeT row, SizeT col);
```

Provides mutable access to matrix elements.

**Parameters:**
- `row`: Row index (0-2)
- `col`: Column index (0-2)

**Example:**
```cpp
matrix(1, 2) = 5.0f;
```

#### Element Access (Const)

```cpp
const T& operator()(SizeT row, SizeT col) const;
```

Provides read-only access to matrix elements.

## Methods

### SetIdentity

```cpp
void SetIdentity(void);
```

Sets all elements to create an identity matrix.

**Example:**
```cpp
tkd::TMatrix3x3<float> matrix;
matrix.SetIdentity();
```

### SetZero

```cpp
void SetZero(void);
```

Sets all elements to zero.

**Example:**
```cpp
tkd::TMatrix3x3<float> matrix;
matrix.SetZero();
```

### Determinant

```cpp
T Determinant(void) const;
```

Calculates the determinant of the matrix using the formula:

```
det = m00*(m11*m22 - m12*m21) - m01*(m10*m22 - m12*m20) + m02*(m10*m21 - m11*m20)
```

**Returns:** The determinant value.

**Example:**
```cpp
float det = matrix.Determinant();
if (det != 0.0f) {
    // Matrix is invertible
}
```

### Transpose

```cpp
TMatrix3x3<T> Transpose(void) const;
```

Returns the transpose of the matrix (rows become columns).

**Example:**
```cpp
tkd::TMatrix3x3<float> transposed = matrix.Transpose();
```

### Inverse

```cpp
TMatrix3x3<T> Inverse(void) const;
```

Calculates the inverse of the matrix using the adjugate method.

**Returns:** The inverse matrix.

**Throws:** `std::runtime_error` if the matrix is not invertible (determinant is zero).

**Example:**
```cpp
try {
    tkd::TMatrix3x3<float> inverse = matrix.Inverse();
} catch (const std::runtime_error& e) {
    // Handle non-invertible matrix
}
```

### Trace

```cpp
T Trace(void) const;
```

Calculates the trace of the matrix (sum of diagonal elements).

**Returns:** The sum of elements `data[0][0] + data[1][1] + data[2][2]`.

**Example:**
```cpp
float trace = matrix.Trace();
```

## Global Functions

### Scalar-Matrix Multiplication

```cpp
template <typename T>
TMatrix3x3<T> operator*(T scalar, const TMatrix3x3<T>& mat);
```

Allows multiplication of a scalar by a matrix in either order.

**Example:**
```cpp
tkd::TMatrix3x3<float> result = 2.0f * matrix;  // Same as matrix * 2.0f
```

## Usage Examples

### 2D Rotation Matrix

```cpp
#include <Engine/Core/Math/TMatrix3x3.hpp>
#include <cmath>

tkd::TMatrix3x3<float> CreateRotationMatrix(float angleRadians) {
    float cosA = std::cos(angleRadians);
    float sinA = std::sin(angleRadians);

    return tkd::TMatrix3x3<float>(
        cosA, -sinA, 0.0f,
        sinA,  cosA, 0.0f,
        0.0f,  0.0f, 1.0f
    );
}
```

### 2D Translation Matrix

```cpp
tkd::TMatrix3x3<float> CreateTranslationMatrix(float tx, float ty) {
    return tkd::TMatrix3x3<float>(
        1.0f, 0.0f, tx,
        0.0f, 1.0f, ty,
        0.0f, 0.0f, 1.0f
    );
}
```

### 2D Scaling Matrix

```cpp
tkd::TMatrix3x3<float> CreateScalingMatrix(float sx, float sy) {
    return tkd::TMatrix3x3<float>(
        sx,   0.0f, 0.0f,
        0.0f, sy,   0.0f,
        0.0f, 0.0f, 1.0f
    );
}
```

### Combining Transformations

```cpp
// Create individual transformations
auto rotation = CreateRotationMatrix(M_PI / 4.0f);  // 45 degrees
auto translation = CreateTranslationMatrix(10.0f, 20.0f);
auto scaling = CreateScalingMatrix(2.0f, 2.0f);

// Combine transformations (note the order: scale -> rotate -> translate)
tkd::TMatrix3x3<float> transform = translation * rotation * scaling;
```

### Matrix Inversion for Coordinate Transformation

```cpp
// Transform point from world space to local space
tkd::TMatrix3x3<float> worldToLocal = transform.Inverse();

// Apply transformation to a point (homogeneous coordinates)
float point[3] = {5.0f, 3.0f, 1.0f};  // (x, y, 1)
float transformed[3] = {0.0f, 0.0f, 0.0f};

for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
        transformed[i] += worldToLocal(i, j) * point[j];
    }
}
```

## Mathematical Background

### Matrix Representation

A 3x3 matrix can be visualized as:

```
┌             ┐
│ m00  m01  m02 │
│ m10  m11  m12 │
│ m20  m21  m22 │
└             ┘
```

### Common Use Cases

- **2D Graphics Transformations**: Rotation, scaling, translation, shearing
- **Linear Systems**: Solving systems of linear equations
- **Coordinate System Conversions**: World-to-screen transformations
- **Affine Transformations**: Preserving parallelism and ratios

### Properties

- **Associativity**: (A * B) * C = A * (B * C)
- **Identity Element**: A * Identity = Identity * A = A
- **Zero Element**: A * Zero = Zero * A = Zero
- **Non-commutativity**: A * B ≠ B * A (in general)

## Notes

- All operations are performed using the template type T, ensuring type consistency.
- Floating-point precision may affect comparison operations due to rounding errors.
- Matrix inversion requires the determinant to be non-zero.
- The class uses row-major storage, which is consistent with many mathematical libraries.
- Operations are designed to be efficient for real-time applications.

## See Also

- `TVector2<T>`: 2D vector class
- `TVector3<T>`: 3D vector class
- `TMatrix4x4<T>`: 4x4 matrix class for 3D transformations
