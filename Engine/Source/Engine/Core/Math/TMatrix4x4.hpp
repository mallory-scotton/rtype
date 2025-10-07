///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <cmath>
#include <Engine/Config.hpp>
#include <stdexcept>
#include <type_traits>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief 4x4 Matrix class for mathematical operations
///
/// \tparam T The numeric type for matrix elements
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
    ///////////////////////////////////////////////////////////////////////////
    // Class Member
    ///////////////////////////////////////////////////////////////////////////
    T data[4][4];   //<! Matrix data stored in row-major order

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor, initializes to identity matrix
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMatrix4x4(void)
        : data{
            { 1.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 1.0f },
    }
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor with individual elements
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMatrix4x4(
        T m00,
        T m01,
        T m02,
        T m03,
        T m10,
        T m11,
        T m12,
        T m13,
        T m20,
        T m21,
        T m22,
        T m23,
        T m30,
        T m31,
        T m32,
        T m33
    )
        : data{
            { m00, m01, m02, m03 },
            { m10, m11, m12, m13 },
            { m20, m21, m22, m23 },
            { m30, m31, m32, m33 }
    }
    {}

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Access element at row and column (mutable)
    ///
    ///////////////////////////////////////////////////////////////////////////
    T& operator()(SizeT row, SizeT col) { return data[row][col]; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Access element at row and column (const)
    ///
    ///////////////////////////////////////////////////////////////////////////
    const T& operator()(SizeT row, SizeT col) const { return data[row][col]; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Matrix addition
    ///
    /// \param other The matrix to add
    ///
    /// \return Result of addition
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMatrix4x4<T> operator+(const TMatrix4x4<T>& other) const
    {
        return TMatrix4x4<T>(
            data[0][0] + other.data[0][0],
            data[0][1] + other.data[0][1],
            data[0][2] + other.data[0][2],
            data[0][3] + other.data[0][3],
            data[1][0] + other.data[1][0],
            data[1][1] + other.data[1][1],
            data[1][2] + other.data[1][2],
            data[1][3] + other.data[1][3],
            data[2][0] + other.data[2][0],
            data[2][1] + other.data[2][1],
            data[2][2] + other.data[2][2],
            data[2][3] + other.data[2][3],
            data[3][0] + other.data[3][0],
            data[3][1] + other.data[3][1],
            data[3][2] + other.data[3][2],
            data[3][3] + other.data[3][3]
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Matrix subtraction
    ///
    /// \param other The matrix to subtract
    ///
    /// \return Result of subtraction
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMatrix4x4<T> operator-(const TMatrix4x4<T>& other) const
    {
        return TMatrix4x4<T>(
            data[0][0] - other.data[0][0],
            data[0][1] - other.data[0][1],
            data[0][2] - other.data[0][2],
            data[0][3] - other.data[0][3],
            data[1][0] - other.data[1][0],
            data[1][1] - other.data[1][1],
            data[1][2] - other.data[1][2],
            data[1][3] - other.data[1][3],
            data[2][0] - other.data[2][0],
            data[2][1] - other.data[2][1],
            data[2][2] - other.data[2][2],
            data[2][3] - other.data[2][3],
            data[3][0] - other.data[3][0],
            data[3][1] - other.data[3][1],
            data[3][2] - other.data[3][2],
            data[3][3] - other.data[3][3]
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Matrix multiplication
    ///
    /// \param other The matrix to multiply with
    ///
    /// \return Result of multiplication
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMatrix4x4<T> operator*(const TMatrix4x4<T>& other) const
    {
        TMatrix4x4<T> result;

        for (SizeT i = 0; i < 4; i++)
        {
            for (SizeT j = 0; j < 4; j++)
            {
                result.data[i][j] = data[i][0] * other.data[0][j] +
                                    data[i][1] * other.data[1][j] +
                                    data[i][2] * other.data[2][j] +
                                    data[i][3] * other.data[3][j];
            }
        }

        return result;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Scalar multiplication
    ///
    /// \param scalar The scalar to multiply with
    ///
    /// \return Result of multiplication
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMatrix4x4<T> operator*(T scalar) const
    {
        return TMatrix4x4<T>(
            data[0][0] * scalar,
            data[0][1] * scalar,
            data[0][2] * scalar,
            data[0][3] * scalar,
            data[1][0] * scalar,
            data[1][1] * scalar,
            data[1][2] * scalar,
            data[1][3] * scalar,
            data[2][0] * scalar,
            data[2][1] * scalar,
            data[2][2] * scalar,
            data[2][3] * scalar,
            data[3][0] * scalar,
            data[3][1] * scalar,
            data[3][2] * scalar,
            data[3][3] * scalar
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Equality comparison
    ///
    /// \param other The matrix to compare with
    ///
    /// \return True if matrices are equal, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool operator==(const TMatrix4x4<T>& other) const
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                if (data[i][j] != other.data[i][j]) { return false; }
            }
        }
        return true;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Inequality comparison
    ///
    /// \param other The matrix to compare with
    ///
    /// \return True if matrices are not equal, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool operator!=(const TMatrix4x4<T>& other) const
    {
        return !(*this == other);
    }

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set this matrix to identity
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetIdentity(void)
    {
        data[0][0] = T(1);
        data[0][1] = T(0);
        data[0][2] = T(0);
        data[0][3] = T(0);
        data[1][0] = T(0);
        data[1][1] = T(1);
        data[1][2] = T(0);
        data[1][3] = T(0);
        data[2][0] = T(0);
        data[2][1] = T(0);
        data[2][2] = T(1);
        data[2][3] = T(0);
        data[3][0] = T(0);
        data[3][1] = T(0);
        data[3][2] = T(0);
        data[3][3] = T(1);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set all elements to zero
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetZero(void)
    {
        for (SizeT i = 0; i < 4; i++)
        {
            for (SizeT j = 0; j < 4; j++) { data[i][j] = T(0); }
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Calculate determinant of the matrix
    ///
    /// \return Determinant value
    ///
    ///////////////////////////////////////////////////////////////////////////
    T Determinant(void) const
    {
        T det = T(0);

        // Using cofactor expansion along the first row
        det +=
            data[0][0] *
            (data[1][1] * (data[2][2] * data[3][3] - data[2][3] * data[3][2]) -
             data[1][2] * (data[2][1] * data[3][3] - data[2][3] * data[3][1]) +
             data[1][3] * (data[2][1] * data[3][2] - data[2][2] * data[3][1]));

        det -=
            data[0][1] *
            (data[1][0] * (data[2][2] * data[3][3] - data[2][3] * data[3][2]) -
             data[1][2] * (data[2][0] * data[3][3] - data[2][3] * data[3][0]) +
             data[1][3] * (data[2][0] * data[3][2] - data[2][2] * data[3][0]));

        det +=
            data[0][2] *
            (data[1][0] * (data[2][1] * data[3][3] - data[2][3] * data[3][1]) -
             data[1][1] * (data[2][0] * data[3][3] - data[2][3] * data[3][0]) +
             data[1][3] * (data[2][0] * data[3][1] - data[2][1] * data[3][0]));

        det -=
            data[0][3] *
            (data[1][0] * (data[2][1] * data[3][2] - data[2][2] * data[3][1]) -
             data[1][1] * (data[2][0] * data[3][2] - data[2][2] * data[3][0]) +
             data[1][2] * (data[2][0] * data[3][1] - data[2][1] * data[3][0]));

        return det;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get transpose of this matrix
    ///
    /// \return Transposed matrix
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMatrix4x4<T> Transpose(void) const
    {
        return TMatrix4x4<T>(
            data[0][0],
            data[1][0],
            data[2][0],
            data[3][0],
            data[0][1],
            data[1][1],
            data[2][1],
            data[3][1],
            data[0][2],
            data[1][2],
            data[2][2],
            data[3][2],
            data[0][3],
            data[1][3],
            data[2][3],
            data[3][3]
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get inverse of this matrix
    ///
    /// \return Inverted matrix
    ///
    /// \throws std::runtime_error if the matrix is not invertible
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMatrix4x4<T> Inverse(void) const
    {
        T det = Determinant();
        if (det == T(0))
        {
            throw std::runtime_error("Matrix is not invertible");
        }

        T invDet = T(1) / det;
        TMatrix4x4<T> result;

        // Calculate cofactors and adjugate matrix
        for (SizeT i = 0; i < 4; i++)
        {
            for (SizeT j = 0; j < 4; j++)
            {
                // Get minor by excluding row i and column j
                T minor = T(0);
                int sign = ((i + j) % 2 == 0) ? 1 : -1;

                // 3x3 determinant for each minor
                SizeT r[3], c[3];
                for (SizeT k = 0, kr = 0; k < 4; k++)
                {
                    if (k != i) { r[kr++] = k; }
                }
                for (SizeT k = 0, kc = 0; k < 4; k++)
                {
                    if (k != j) { c[kc++] = k; }
                }

                minor =
                    data[r[0]][c[0]] * (data[r[1]][c[1]] * data[r[2]][c[2]] -
                                        data[r[1]][c[2]] * data[r[2]][c[1]]) -
                    data[r[0]][c[1]] * (data[r[1]][c[0]] * data[r[2]][c[2]] -
                                        data[r[1]][c[2]] * data[r[2]][c[0]]) +
                    data[r[0]][c[2]] * (data[r[1]][c[0]] * data[r[2]][c[1]] -
                                        data[r[1]][c[1]] * data[r[2]][c[0]]);

                // Adjugate has transposed indices
                result.data[j][i] = sign * minor * invDet;
            }
        }

        return result;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Calculate the trace (sum of diagonal elements)
    ///
    /// \return Trace value
    ///
    ///////////////////////////////////////////////////////////////////////////
    T Trace(void) const
    {
        return data[0][0] + data[1][1] + data[2][2] + data[3][3];
    }
};

}   // namespace tkd
