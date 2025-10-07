# Matrix3x3

The `TMatrix3x3` class represents a 3x3 matrix commonly used for 2D transformations, rotations, and other linear operations. It provides a comprehensive set of methods for matrix manipulation.

## Features

- Identity and zero matrix initialization
- Matrix arithmetic (addition, subtraction, multiplication)
- Determinant calculation
- Matrix transposition and inversion
- Comparison operations
- Trace calculation (sum of diagonal elements)

## Usage

```cpp
// Create an identity matrix
tkd::TMatrix3x3<float> matrix;

// Create a custom matrix
tkd::TMatrix3x3<float> custom(
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f
);

// Access elements
float element = matrix(0, 1);

// Perform operations
tkd::TMatrix3x3<float> inverse = matrix.Inverse();
float det = matrix.Determinant();
```