# Matrix4x4

The `TMatrix4x4` class represents a 4×4 matrix commonly used for 3D transformations, projections, and other linear operations. It provides a comprehensive set of methods for matrix manipulation.

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
tkd::TMatrix4x4<float> matrix;

// Create a custom matrix
tkd::TMatrix4x4<float> custom(
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
);

// Access elements
float element = matrix(0, 1);

// Perform operations
tkd::TMatrix4x4<float> inverse = matrix.Inverse();
float det = matrix.Determinant();
```