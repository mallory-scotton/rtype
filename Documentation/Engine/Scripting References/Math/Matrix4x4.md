# Matrix4x4

The `TMatrix4x4` class represents a 4×4 matrix commonly used for 3D transformations, projections, and other linear operations in computer graphics and game development. It provides a comprehensive set of methods for matrix manipulation, arithmetic operations, and advanced mathematical computations.

## Overview

The `TMatrix4x4` is a template class that supports various numeric types (e.g., `float`, `double`, `int`) for matrix elements. It stores data in row-major order and provides efficient implementations for common matrix operations used in 3D graphics, such as transformations, projections, and linear algebra computations.

### Key Features

- **Template-based**: Supports any arithmetic type (`float`, `double`, `int`, etc.)
- **Row-major storage**: Data stored in row-major order for compatibility with OpenGL and other graphics APIs
- **Comprehensive operations**: Addition, subtraction, multiplication, transposition, inversion
- **Mathematical utilities**: Determinant calculation, trace computation
- **Type safety**: Compile-time checks ensure only arithmetic types are used
- **Exception handling**: Proper error handling for non-invertible matrices

## Template Parameters

| Parameter | Description |
|-----------|-------------|
| `T` | The numeric type for matrix elements. Must be an arithmetic type (integral or floating-point). |

## Class Definition

```cpp
template <typename T>
class TMatrix4x4
{
public:
    T data[4][4];  // Matrix data stored in row-major order

    // Constructors and methods...
};
```

## Members

### Data Member

- **`T data[4][4]`**: A 2D array storing the matrix elements in row-major order. Element `data[i][j]` represents the element at row `i` and column `j`.

### Static Assertions

The class includes a static assertion to ensure the template parameter `T` is an arithmetic type:

```cpp
static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");
```

## Constructors

### Default Constructor

```cpp
TMatrix4x4(void);
```

Initializes the matrix as an identity matrix:

\[
\begin{pmatrix}
1 & 0 & 0 & 0 \\
0 & 1 & 0 & 0 \\
0 & 0 & 1 & 0 \\
0 & 0 & 0 & 1
\end{pmatrix}
\]

**Example:**
```cpp
tkd::TMatrix4x4<float> identity;  // Creates identity matrix
```

### Element-wise Constructor

```cpp
TMatrix4x4(
    T m00, T m01, T m02, T m03,
    T m10, T m11, T m12, T m13,
    T m20, T m21, T m22, T m23,
    T m30, T m31, T m32, T m33
);
```

Constructs a matrix with specified elements. Parameters are ordered as `m[row][column]`.

**Example:**
```cpp
tkd::TMatrix4x4<float> translation(
    1.0f, 0.0f, 0.0f, 3.0f,  // Translation by (3, 0, 0)
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
);
```

## Operators

### Element Access Operators

#### Mutable Access

```cpp
T& operator()(SizeT row, SizeT col);
```

Returns a reference to the element at the specified row and column.

#### Const Access

```cpp
const T& operator()(SizeT row, SizeT col) const;
```

Returns a const reference to the element at the specified row and column.

**Example:**
```cpp
tkd::TMatrix4x4<float> matrix;
matrix(0, 1) = 2.0f;  // Set element at row 0, column 1
float value = matrix(2, 3);  // Get element at row 2, column 3
```

### Arithmetic Operators

#### Matrix Addition

```cpp
TMatrix4x4<T> operator+(const TMatrix4x4<T>& other) const;
```

Performs element-wise addition of two matrices.

**Mathematical Operation:**
\[
\mathbf{C} = \mathbf{A} + \mathbf{B} \quad \text{where} \quad c_{ij} = a_{ij} + b_{ij}
\]

**Example:**
```cpp
tkd::TMatrix4x4<float> A = /* ... */;
tkd::TMatrix4x4<float> B = /* ... */;
tkd::TMatrix4x4<float> C = A + B;
```

#### Matrix Subtraction

```cpp
TMatrix4x4<T> operator-(const TMatrix4x4<T>& other) const;
```

Performs element-wise subtraction of two matrices.

**Mathematical Operation:**
\[
\mathbf{C} = \mathbf{A} - \mathbf{B} \quad \text{where} \quad c_{ij} = a_{ij} - b_{ij}
\]

#### Matrix Multiplication

```cpp
TMatrix4x4<T> operator*(const TMatrix4x4<T>& other) const;
```

Performs matrix multiplication.

**Mathematical Operation:**
\[
\mathbf{C} = \mathbf{A} \mathbf{B} \quad \text{where} \quad c_{ij} = \sum_{k=0}^{3} a_{ik} b_{kj}
\]

**Example:**
```cpp
tkd::TMatrix4x4<float> transform = rotation * translation;
```

#### Scalar Multiplication

```cpp
TMatrix4x4<T> operator*(T scalar) const;
```

Multiplies each element of the matrix by a scalar value.

**Mathematical Operation:**
\[
\mathbf{B} = s \mathbf{A} \quad \text{where} \quad b_{ij} = s \cdot a_{ij}
\]

**Example:**
```cpp
tkd::TMatrix4x4<float> scaled = matrix * 2.0f;
```

### Comparison Operators

#### Equality

```cpp
bool operator==(const TMatrix4x4<T>& other) const;
```

Returns `true` if all corresponding elements are equal.

#### Inequality

```cpp
bool operator!=(const TMatrix4x4<T>& other) const;
```

Returns `true` if any corresponding elements are not equal.

## Methods

### SetIdentity

```cpp
void SetIdentity(void);
```

Sets the matrix to the identity matrix.

**Example:**
```cpp
tkd::TMatrix4x4<float> matrix;
matrix.SetIdentity();
```

### SetZero

```cpp
void SetZero(void);
```

Sets all elements of the matrix to zero.

**Example:**
```cpp
tkd::TMatrix4x4<float> matrix;
matrix.SetZero();
```

### Determinant

```cpp
T Determinant(void) const;
```

Calculates the determinant of the matrix using cofactor expansion along the first row.

**Mathematical Operation:**
\[
\det(\mathbf{A}) = \sum_{j=0}^{3} (-1)^j a_{0j} M_{0j}
\]

Where \(M_{0j}\) is the determinant of the 3×3 minor matrix obtained by removing row 0 and column j.

**Example:**
```cpp
tkd::TMatrix4x4<float> matrix = /* ... */;
float det = matrix.Determinant();
if (det != 0.0f) {
    // Matrix is invertible
}
```

### Transpose

```cpp
TMatrix4x4<T> Transpose(void) const;
```

Returns the transpose of the matrix.

**Mathematical Operation:**
\[
(\mathbf{A}^T)_{ij} = a_{ji}
\]

**Example:**
```cpp
tkd::TMatrix4x4<float> matrix = /* ... */;
tkd::TMatrix4x4<float> transposed = matrix.Transpose();
```

### Inverse

```cpp
TMatrix4x4<T> Inverse(void) const;
```

Calculates the inverse of the matrix using the adjugate matrix method.

**Mathematical Operation:**
\[
\mathbf{A}^{-1} = \frac{1}{\det(\mathbf{A})} \text{adj}(\mathbf{A})
\]

**Throws:** `std::runtime_error` if the matrix is not invertible (determinant is zero).

**Example:**
```cpp
tkd::TMatrix4x4<float> matrix = /* ... */;
try {
    tkd::TMatrix4x4<float> inverse = matrix.Inverse();
} catch (const std::runtime_error& e) {
    // Handle non-invertible matrix
}
```

### Trace

```cpp
T Trace(void) const;
```

Calculates the trace of the matrix (sum of diagonal elements).

**Mathematical Operation:**
\[
\text{tr}(\mathbf{A}) = \sum_{i=0}^{3} a_{ii}
\]

**Example:**
```cpp
tkd::TMatrix4x4<float> matrix = /* ... */;
float trace = matrix.Trace();
```

## Usage Examples

### Basic Matrix Operations

```cpp
#include <Engine/Core/Math/TMatrix4x4.hpp>

// Create matrices
tkd::TMatrix4x4<float> A(
    1.0f, 2.0f, 3.0f, 4.0f,
    5.0f, 6.0f, 7.0f, 8.0f,
    9.0f, 10.0f, 11.0f, 12.0f,
    13.0f, 14.0f, 15.0f, 16.0f
);

tkd::TMatrix4x4<float> B;
B.SetIdentity();

// Matrix operations
tkd::TMatrix4x4<float> C = A + B;
tkd::TMatrix4x4<float> D = A * B;
tkd::TMatrix4x4<float> E = A.Transpose();

// Element access
float element = A(1, 2);  // Row 1, Column 2
A(0, 0) = 99.0f;         // Modify element
```

### 3D Transformation Matrices

```cpp
// Translation matrix
tkd::TMatrix4x4<float> CreateTranslation(float x, float y, float z) {
    tkd::TMatrix4x4<float> matrix;
    matrix.SetIdentity();
    matrix(0, 3) = x;
    matrix(1, 3) = y;
    matrix(2, 3) = z;
    return matrix;
}

// Scale matrix
tkd::TMatrix4x4<float> CreateScale(float sx, float sy, float sz) {
    tkd::TMatrix4x4<float> matrix;
    matrix.SetIdentity();
    matrix(0, 0) = sx;
    matrix(1, 1) = sy;
    matrix(2, 2) = sz;
    return matrix;
}

// Combine transformations
tkd::TMatrix4x4<float> translation = CreateTranslation(1.0f, 2.0f, 3.0f);
tkd::TMatrix4x4<float> scale = CreateScale(2.0f, 2.0f, 2.0f);
tkd::TMatrix4x4<float> combined = translation * scale;
```

### Projection Matrix Example

```cpp
// Simple orthographic projection matrix
tkd::TMatrix4x4<float> CreateOrthographic(float left, float right, float bottom, float top, float near, float far) {
    tkd::TMatrix4x4<float> matrix;
    matrix.SetZero();

    matrix(0, 0) = 2.0f / (right - left);
    matrix(1, 1) = 2.0f / (top - bottom);
    matrix(2, 2) = -2.0f / (far - near);
    matrix(3, 3) = 1.0f;

    matrix(0, 3) = -(right + left) / (right - left);
    matrix(1, 3) = -(top + bottom) / (top - bottom);
    matrix(2, 3) = -(far + near) / (far - near);

    return matrix;
}
```

## Mathematical Background

### Matrix Representation

A 4×4 matrix in row-major order:

\[
\mathbf{M} = \begin{pmatrix}
m_{00} & m_{01} & m_{02} & m_{03} \\
m_{10} & m_{11} & m_{12} & m_{13} \\
m_{20} & m_{21} & m_{22} & m_{23} \\
m_{30} & m_{31} & m_{32} & m_{33}
\end{pmatrix}
\]

### Common Uses in Game Development

1. **Model Matrix**: Transforms objects from local space to world space
2. **View Matrix**: Transforms from world space to camera space
3. **Projection Matrix**: Transforms from camera space to clip space
4. **MVP Matrix**: Model-View-Projection matrix for vertex transformation

### Performance Considerations

- Matrix operations are computationally intensive
- Inversion is particularly expensive (O(n³) for n×n matrices)
- Cache-friendly row-major storage for graphics APIs
- Consider using SIMD instructions for optimized implementations

## Dependencies

- `<cmath>`: For mathematical functions
- `<Engine/Config.hpp>`: Engine configuration
- `<stdexcept>`: For exception handling
- `<type_traits>`: For type checking

## Error Handling

- **Non-invertible matrices**: `Inverse()` throws `std::runtime_error` when determinant is zero
- **Type safety**: Static assertion ensures arithmetic types only

## See Also

- [Vector3D](Vector3D.md) - 3D vector operations
- [Quaternion](Quaternion.md) - Quaternion rotations
- [Transform](Transform.md) - Combined transformation utilities
