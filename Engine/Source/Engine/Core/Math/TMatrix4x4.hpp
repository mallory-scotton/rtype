///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <cmath>
#include <stdexcept>
#include <type_traits>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
/// \tparam T
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
class TMatrix4x4
{
public:
    static_assert(
        std::is_arithmetic<T>::value, "T must be an arithmetic type"
    );

public:
    T m[4][4]; // Matrix data stored in row-major order

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor, initializes to identity matrix
    ///////////////////////////////////////////////////////////////////////////
    TMatrix4x4(void)
        : m{
            {1.0f, 0.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 0.0f, 1.0f},
        }
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor with individual elements
    ///////////////////////////////////////////////////////////////////////////
    TMatrix4x4(
        T m00, T m01, T m02, T m03,
        T m10, T m11, T m12, T m13,
        T m20, T m21, T m22, T m23,
        T m30, T m31, T m32, T m33
    )
        : m{
        {m00, m01, m02, m03},
        {m10, m11, m12, m13},
        {m20, m21, m22, m23},
        {m30, m31, m32, m33}
    }
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Access element at row and column (mutable)
    ///////////////////////////////////////////////////////////////////////////
    T& operator()(std::size_t row, std::size_t col)
    {
        return m[row][col];
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Access element at row and column (const)
    ///////////////////////////////////////////////////////////////////////////
    const T& operator()(std::size_t row, std::size_t col) const
    {
        return m[row][col];
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set this matrix to identity
    ///////////////////////////////////////////////////////////////////////////
    void SetIdentity()
    {
        m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
        m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = 0.0f;
        m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = 0.0f;
        m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set all elements to zero
    ///////////////////////////////////////////////////////////////////////////
    void SetZero()
    {
        m[0][0] = 0.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
        m[1][0] = 0.0f; m[1][1] = 0.0f; m[1][2] = 0.0f; m[1][3] = 0.0f;
        m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 0.0f; m[2][3] = 0.0f;
        m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 0.0f;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Calculate determinant of the matrix
    ///////////////////////////////////////////////////////////////////////////
    T Determinant() const
    {
        float det = 0.0f;

        // Using cofactor expansion along the first row
        det += m[0][0] * (
            m[1][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) -
            m[1][2] * (m[2][1] * m[3][3] - m[2][3] * m[3][1]) +
            m[1][3] * (m[2][1] * m[3][2] - m[2][2] * m[3][1])
        );

        det -= m[0][1] * (
            m[1][0] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) -
            m[1][2] * (m[2][0] * m[3][3] - m[2][3] * m[3][0]) +
            m[1][3] * (m[2][0] * m[3][2] - m[2][2] * m[3][0])
        );

        det += m[0][2] * (
            m[1][0] * (m[2][1] * m[3][3] - m[2][3] * m[3][1]) -
            m[1][1] * (m[2][0] * m[3][3] - m[2][3] * m[3][0]) +
            m[1][3] * (m[2][0] * m[3][1] - m[2][1] * m[3][0])
        );

        det -= m[0][3] * (
            m[1][0] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]) -
            m[1][1] * (m[2][0] * m[3][2] - m[2][2] * m[3][0]) +
            m[1][2] * (m[2][0] * m[3][1] - m[2][1] * m[3][0])
        );

        return det;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get transpose of this matrix
    ///////////////////////////////////////////////////////////////////////////
    TMatrix4x4<T> Transpose() const
    {
        return TMatrix4x4<float>(
            m[0][0], m[1][0], m[2][0], m[3][0],
            m[0][1], m[1][1], m[2][1], m[3][1],
            m[0][2], m[1][2], m[2][2], m[3][2],
            m[0][3], m[1][3], m[2][3], m[3][3]
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get inverse of this matrix
    ///////////////////////////////////////////////////////////////////////////
    TMatrix4x4<T> Inverse() const
    {
        float det = Determinant();
        if (det == 0)
        {
            throw std::runtime_error("Matrix is not invertible");
        }

        float invDet = 1.0f / det;
        TMatrix4x4<float> result;

        // Calculate cofactors and adjugate matrix
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                // Get minor by excluding row i and column j
                float minor = 0.0f;
                int sign = ((i + j) % 2 == 0) ? 1 : -1;

                // 3x3 determinant for each minor
                int r[3], c[3];
                for (int k = 0, kr = 0; k < 4; k++) {
                    if (k != i) r[kr++] = k;
                }
                for (int k = 0, kc = 0; k < 4; k++) {
                    if (k != j) c[kc++] = k;
                }

                minor =
                    m[r[0]][c[0]] * (m[r[1]][c[1]] * m[r[2]][c[2]] - m[r[1]][c[2]] * m[r[2]][c[1]]) -
                    m[r[0]][c[1]] * (m[r[1]][c[0]] * m[r[2]][c[2]] - m[r[1]][c[2]] * m[r[2]][c[0]]) +
                    m[r[0]][c[2]] * (m[r[1]][c[0]] * m[r[2]][c[1]] - m[r[1]][c[1]] * m[r[2]][c[0]]);

                // Adjugate has transposed indices
                result.m[j][i] = sign * minor * invDet;
            }
        }

        return result;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Matrix addition
    ///////////////////////////////////////////////////////////////////////////
    TMatrix4x4<T> operator+(const TMatrix4x4<T>& other) const
    {
        return TMatrix4x4<float>(
            m[0][0] + other.m[0][0], m[0][1] + other.m[0][1], m[0][2] + other.m[0][2], m[0][3] + other.m[0][3],
            m[1][0] + other.m[1][0], m[1][1] + other.m[1][1], m[1][2] + other.m[1][2], m[1][3] + other.m[1][3],
            m[2][0] + other.m[2][0], m[2][1] + other.m[2][1], m[2][2] + other.m[2][2], m[2][3] + other.m[2][3],
            m[3][0] + other.m[3][0], m[3][1] + other.m[3][1], m[3][2] + other.m[3][2], m[3][3] + other.m[3][3]
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Matrix subtraction
    ///////////////////////////////////////////////////////////////////////////
    TMatrix4x4<T> operator-(const TMatrix4x4<T>& other) const
    {
        return TMatrix4x4<float>(
            m[0][0] - other.m[0][0], m[0][1] - other.m[0][1], m[0][2] - other.m[0][2], m[0][3] - other.m[0][3],
            m[1][0] - other.m[1][0], m[1][1] - other.m[1][1], m[1][2] - other.m[1][2], m[1][3] - other.m[1][3],
            m[2][0] - other.m[2][0], m[2][1] - other.m[2][1], m[2][2] - other.m[2][2], m[2][3] - other.m[2][3],
            m[3][0] - other.m[3][0], m[3][1] - other.m[3][1], m[3][2] - other.m[3][2], m[3][3] - other.m[3][3]
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Matrix multiplication
    ///////////////////////////////////////////////////////////////////////////
    TMatrix4x4<T> operator*(const TMatrix4x4<T>& other) const
    {
        TMatrix4x4<float> result;

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                result.m[i][j] =
                    m[i][0] * other.m[0][j] +
                    m[i][1] * other.m[1][j] +
                    m[i][2] * other.m[2][j] +
                    m[i][3] * other.m[3][j];
            }
        }

        return result;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Scalar multiplication
    ///////////////////////////////////////////////////////////////////////////
    TMatrix4x4<T> operator*(T scalar) const
    {
        return TMatrix4x4<float>(
            m[0][0] * scalar, m[0][1] * scalar, m[0][2] * scalar, m[0][3] * scalar,
            m[1][0] * scalar, m[1][1] * scalar, m[1][2] * scalar, m[1][3] * scalar,
            m[2][0] * scalar, m[2][1] * scalar, m[2][2] * scalar, m[2][3] * scalar,
            m[3][0] * scalar, m[3][1] * scalar, m[3][2] * scalar, m[3][3] * scalar
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Equality comparison
    ///////////////////////////////////////////////////////////////////////////
    bool operator==(const TMatrix4x4<T>& other) const
    {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (m[i][j] != other.m[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Inequality comparison
    ///////////////////////////////////////////////////////////////////////////
    bool operator!=(const TMatrix4x4<T>& other) const
    {
        return !(*this == other);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Calculate the trace (sum of diagonal elements)
    ///////////////////////////////////////////////////////////////////////////
    T Trace(void) const
    {
        return m[0][0] + m[1][1] + m[2][2] + m[3][3];
    }

};

}   // namespace tkd
