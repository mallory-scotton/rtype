# TMatrix2x2

The `TMatrix2x2` class represents a 2x2 matrix template used for 2D linear algebra operations, transformations, rotations, scaling, and other mathematical computations in the TKD Engine. It provides a complete set of arithmetic operations, matrix manipulations, and utility functions essential for game development and computer graphics.

## Table of Contents

- [Overview](#overview)
- [Template Parameters](#template-parameters)
- [Static Members](#static-members)
- [Data Members](#data-members)
- [Constructors](#constructors)
- [Operators](#operators)
- [Methods](#methods)
- [Mathematical Background](#mathematical-background)
- [Usage Examples](#usage-examples)
- [Performance Considerations](#performance-considerations)

## Overview

The `TMatrix2x2` class is a template class that encapsulates a 2x2 matrix with elements of type `T`. It supports all fundamental matrix operations required for 2D graphics, physics simulations, and general linear algebra computations. The class ensures type safety through compile-time assertions and provides both const-correct and mutable access to matrix elements.

### Key Features

- **Template-based**: Supports any arithmetic type (int, float, double, etc.)
- **Arithmetic Operations**: Addition, subtraction, multiplication (matrix-matrix and matrix-scalar)
- **Matrix Properties**: Determinant, transpose, inverse, trace
- **Utility Functions**: Identity and zero matrix initialization
- **Comparison Operators**: Equality and inequality checks
- **Element Access**: Direct access via row-column indexing

### Namespace

```cpp
namespace tkd
{
    template <typename T>
    class TMatrix2x2;
}
```

## Template Parameters

| Parameter | Description | Requirements |
|-----------|-------------|--------------|
| `T` | The arithmetic type of matrix elements | Must satisfy `std::is_arithmetic<T>::value` |

### Supported Types

- `int`, `unsigned int`
- `float`, `double`
- `long`, `long long`
- Any other arithmetic type supported by the compiler

## Static Members

### Identity Matrix

```cpp
static const TMatrix2x2<T> Identity;
```

Represents the identity matrix:

$$
\begin{pmatrix}
1 & 0 \\
0 & 1
\end{pmatrix}
$$

The identity matrix is the multiplicative identity for matrix multiplication.

### Zero Matrix

```cpp
static const TMatrix2x2<T> Zero;
```

Represents the zero matrix:

$$
\begin{pmatrix}
0 & 0 \\
0 & 0
\end{pmatrix}
$$

All elements are initialized to zero.

## Data Members

### Matrix Data

```cpp
T data[2][2];
```

A 2D array storing the matrix elements in row-major order.

- `data[0][0]`: Top-left element (m₀₀)
- `data[0][1]`: Top-right element (m₀₁)
- `data[1][0]`: Bottom-left element (m₁₀)
- `data[1][1]`: Bottom-right element (m₁₁)

## Constructors

### Default Constructor

```cpp
TMatrix2x2(void);
```

Initializes the matrix to the identity matrix.

**Example:**
```cpp
tkd::TMatrix2x2<float> matrix;  // Creates identity matrix
```

### Parameterized Constructor

```cpp
TMatrix2x2(T m00, T m01, T m10, T m11);
```

Initializes the matrix with specified elements.

**Parameters:**
- `m00`: Element at row 0, column 0
- `m01`: Element at row 0, column 1
- `m10`: Element at row 1, column 0
- `m11`: Element at row 1, column 1

**Example:**
```cpp
tkd::TMatrix2x2<float> rotation(
    std::cos(angle), -std::sin(angle),
    std::sin(angle),  std::cos(angle)
);
```

## Operators

### Element Access Operators

#### Mutable Access

```cpp
T& operator()(SizeT row, SizeT col);
```

Provides mutable access to matrix elements.

**Parameters:**
- `row`: Row index (0 or 1)
- `col`: Column index (0 or 1)

**Return Value:** Reference to the element at the specified position.

**Example:**
```cpp
tkd::TMatrix2x2<float> matrix;
matrix(0, 1) = 2.0f;  // Set top-right element
```

#### Const Access

```cpp
const T& operator()(SizeT row, SizeT col) const;
```

Provides read-only access to matrix elements.

**Parameters:**
- `row`: Row index (0 or 1)
- `col`: Column index (0 or 1)

**Return Value:** Const reference to the element at the specified position.

**Example:**
```cpp
float value = matrix(1, 0);  // Get bottom-left element
```

### Arithmetic Operators

#### Matrix Addition

```cpp
TMatrix2x2<T> operator+(const TMatrix2x2<T>& other) const;
```

Performs element-wise addition of two matrices.

**Mathematical Operation:**
$$
\begin{pmatrix}
a & b \\
c & d
\end{pmatrix} +
\begin{pmatrix}
e & f \\
g & h
\end{pmatrix} =
\begin{pmatrix}
a+e & b+f \\
c+g & d+h
\end{pmatrix}
$$

**Example:**
```cpp
tkd::TMatrix2x2<float> result = matrixA + matrixB;
```

#### Matrix Subtraction

```cpp
TMatrix2x2<T> operator-(const TMatrix2x2<T>& other) const;
```

Performs element-wise subtraction of two matrices.

**Mathematical Operation:**
$$
\begin{pmatrix}
a & b \\
c & d
\end{pmatrix} -
\begin{pmatrix}
e & f \\
g & h
\end{pmatrix} =
\begin{pmatrix}
a-e & b-f \\
c-g & d-h
\end{pmatrix}
$$

**Example:**
```cpp
tkd::TMatrix2x2<float> result = matrixA - matrixB;
```

#### Matrix Multiplication

```cpp
TMatrix2x2<T> operator*(const TMatrix2x2<T>& other) const;
```

Performs matrix multiplication.

**Mathematical Operation:**
$$
\begin{pmatrix}
a & b \\
c & d
\end{pmatrix} \times
\begin{pmatrix}
e & f \\
g & h
\end{pmatrix} =
\begin{pmatrix}
ae+bg & af+bh \\
ce+dg & cf+dh
\end{pmatrix}
$$

**Example:**
```cpp
tkd::TMatrix2x2<float> result = matrixA * matrixB;
```

#### Scalar Multiplication

```cpp
TMatrix2x2<T> operator*(T scalar) const;
```

Multiplies each element of the matrix by a scalar value.

**Mathematical Operation:**
$$
k \times \begin{pmatrix}
a & b \\
c & d
\end{pmatrix} =
\begin{pmatrix}
ka & kb \\
kc & kd
\end{pmatrix}
$$

**Example:**
```cpp
tkd::TMatrix2x2<float> scaled = matrix * 2.0f;
```

### Comparison Operators

#### Equality

```cpp
bool operator==(const TMatrix2x2<T>& other) const;
```

Checks if two matrices are equal by comparing all elements.

**Return Value:** `true` if all corresponding elements are equal, `false` otherwise.

**Example:**
```cpp
if (matrixA == matrixB) {
    // Matrices are identical
}
```

#### Inequality

```cpp
bool operator!=(const TMatrix2x2<T>& other) const;
```

Checks if two matrices are not equal.

**Return Value:** `true` if any corresponding elements differ, `false` otherwise.

**Example:**
```cpp
if (matrixA != matrixB) {
    // Matrices differ
}
```

## Methods

### SetIdentity

```cpp
void SetIdentity(void);
```

Sets all elements to create an identity matrix.

**Mathematical Result:**
$$
\begin{pmatrix}
1 & 0 \\
0 & 1
\end{pmatrix}
$$

**Example:**
```cpp
tkd::TMatrix2x2<float> matrix;
matrix.SetIdentity();
```

### SetZero

```cpp
void SetZero(void);
```

Sets all elements to zero.

**Mathematical Result:**
$$
\begin{pmatrix}
0 & 0 \\
0 & 0
\end{pmatrix}
$$

**Example:**
```cpp
tkd::TMatrix2x2<float> matrix;
matrix.SetZero();
```

### Determinant

```cpp
T Determinant(void) const;
```

Calculates the determinant of the matrix.

**Mathematical Formula:**
$$
\det\begin{pmatrix}
a & b \\
c & d
\end{pmatrix} = ad - bc
$$

**Return Value:** The determinant value.

**Example:**
```cpp
float det = matrix.Determinant();
if (det != 0.0f) {
    // Matrix is invertible
}
```

### Transpose

```cpp
TMatrix2x2<T> Transpose(void) const;
```

Returns the transpose of the matrix.

**Mathematical Operation:**
$$
\begin{pmatrix}
a & b \\
c & d
\end{pmatrix}^T =
\begin{pmatrix}
a & c \\
b & d
\end{pmatrix}
$$

**Return Value:** A new transposed matrix.

**Example:**
```cpp
tkd::TMatrix2x2<float> transposed = matrix.Transpose();
```

### Inverse

```cpp
TMatrix2x2<T> Inverse(void) const;
```

Calculates the inverse of the matrix.

**Mathematical Formula:**
$$
\begin{pmatrix}
a & b \\
c & d
\end{pmatrix}^{-1} =
\frac{1}{\det}
\begin{pmatrix}
d & -b \\
-c & a
\end{pmatrix}
$$

**Return Value:** A new inverse matrix.

**Throws:** `std::runtime_error` if the matrix is not invertible (determinant is zero).

**Example:**
```cpp
try {
    tkd::TMatrix2x2<float> inverse = matrix.Inverse();
} catch (const std::runtime_error& e) {
    // Matrix is singular
}
```

### Trace

```cpp
T Trace(void) const;
```

Calculates the trace of the matrix (sum of diagonal elements).

**Mathematical Formula:**
$$
\tr\begin{pmatrix}
a & b \\
c & d
\end{pmatrix} = a + d
$$

**Return Value:** The trace value.

**Example:**
```cpp
float trace = matrix.Trace();
```

## Mathematical Background

### Matrix Representation

A 2x2 matrix can be represented as:

$$
M = \begin{pmatrix}
m_{00} & m_{01} \\
m_{10} & m_{11}
\end{pmatrix}
$$

Where:
- $m_{ij}$ represents the element at row $i$, column $j$
- Rows and columns are indexed from 0

### Common 2x2 Matrices

#### Rotation Matrix

```cpp
tkd::TMatrix2x2<float> CreateRotationMatrix(float angle) {
    float cosA = std::cos(angle);
    float sinA = std::sin(angle);
    return tkd::TMatrix2x2<float>(
        cosA, -sinA,
        sinA,  cosA
    );
}
```

**Formula:**
$$
R(\theta) = \begin{pmatrix}
\cos\theta & -\sin\theta \\
\sin\theta & \cos\theta
\end{pmatrix}
$$

#### Scaling Matrix

```cpp
tkd::TMatrix2x2<float> CreateScalingMatrix(float sx, float sy) {
    return tkd::TMatrix2x2<float>(
        sx,  0.0f,
        0.0f, sy
    );
}
```

**Formula:**
$$
S(s_x, s_y) = \begin{pmatrix}
s_x & 0 \\
0 & s_y
\end{pmatrix}
$$

#### Shear Matrix

```cpp
tkd::TMatrix2x2<float> CreateShearMatrix(float kx, float ky) {
    return tkd::TMatrix2x2<float>(
        1.0f, kx,
        ky,   1.0f
    );
}
```

**Formula:**
$$
H(k_x, k_y) = \begin{pmatrix}
1 & k_x \\
k_y & 1
\end{pmatrix}
$$

## Usage Examples

### Basic Matrix Operations

```cpp
#include <Engine/Core/Math/TMatrix2x2.hpp>

// Create matrices
tkd::TMatrix2x2<float> A(1.0f, 2.0f, 3.0f, 4.0f);
tkd::TMatrix2x2<float> B(5.0f, 6.0f, 7.0f, 8.0f);

// Addition
tkd::TMatrix2x2<float> sum = A + B;

// Multiplication
tkd::TMatrix2x2<float> product = A * B;

// Scalar multiplication
tkd::TMatrix2x2<float> scaled = A * 2.0f;

// Transpose
tkd::TMatrix2x2<float> transposed = A.Transpose();

// Determinant
float det = A.Determinant();

// Inverse (if determinant != 0)
if (det != 0.0f) {
    tkd::TMatrix2x2<float> inverse = A.Inverse();
}
```

### 2D Transformation Pipeline

```cpp
// Create transformation matrices
float angle = 45.0f * 3.14159f / 180.0f;  // 45 degrees in radians
tkd::TMatrix2x2<float> rotation = CreateRotationMatrix(angle);
tkd::TMatrix2x2<float> scaling = CreateScalingMatrix(2.0f, 1.5f);

// Combine transformations (note: matrix multiplication order)
tkd::TMatrix2x2<float> transform = scaling * rotation;

// Apply to a 2D vector (conceptual - would need vector class)
float x = 1.0f, y = 0.0f;
float newX = transform(0,0) * x + transform(0,1) * y;
float newY = transform(1,0) * x + transform(1,1) * y;
```

### Matrix Properties Check

```cpp
tkd::TMatrix2x2<float> matrix(1.0f, 2.0f, 3.0f, 4.0f);

// Check if matrix is invertible
bool isInvertible = (matrix.Determinant() != 0.0f);

// Check if matrix is symmetric
bool isSymmetric = (matrix == matrix.Transpose());

// Check if matrix is orthogonal (rotation matrix property)
tkd::TMatrix2x2<float> product = matrix * matrix.Transpose();
bool isOrthogonal = (product == tkd::TMatrix2x2<float>::Identity);
```

### Advanced: Solving Linear Systems

```cpp
// Solve 2x2 system: Ax = b
// Where A is the matrix, x is unknown vector, b is result vector
tkd::TMatrix2x2<float> A(2.0f, 1.0f, 1.0f, 3.0f);
float b1 = 5.0f, b2 = 8.0f;

// x = A^(-1) * b
tkd::TMatrix2x2<float> A_inv = A.Inverse();
float x1 = A_inv(0,0) * b1 + A_inv(0,1) * b2;
float x2 = A_inv(1,0) * b1 + A_inv(1,1) * b2;
```

## Performance Considerations

### Memory Layout

- The matrix uses a fixed-size 2D array for optimal memory access patterns
- Row-major storage ensures cache-friendly access in typical matrix operations

### Computational Complexity

| Operation | Complexity | Notes |
|-----------|------------|-------|
| Element access | O(1) | Direct array indexing |
| Addition/Subtraction | O(1) | 4 arithmetic operations |
| Matrix multiplication | O(1) | 8 multiplications + 4 additions |
| Scalar multiplication | O(1) | 4 multiplications |
| Determinant | O(1) | 2 multiplications + 1 subtraction |
| Transpose | O(1) | 2 element swaps |
| Inverse | O(1) | Determinant + 4 multiplications + 1 division |
| Trace | O(1) | 1 addition |

### Type Considerations

- Use `float` for graphics applications requiring speed over precision
- Use `double` for physics simulations requiring high numerical accuracy
- Integer types can be used for discrete transformations (rotation by 90°, etc.)

### Optimization Notes

- All operations create new matrix instances; consider in-place operations for performance-critical code
- Matrix multiplication is not commutative: `A * B ≠ B * A`
- Always check determinant before attempting inversion to avoid exceptions
- Static identity and zero matrices can be reused across operations
