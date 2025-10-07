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
/// \brief 3x3 Matrix class for mathematical operations
///
/// \tparam T The numeric type for matrix elements
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
class TMatrix3x3
{
public:
    static_assert(
        std::is_arithmetic<T>::value, "T must be an arithmetic type"
    );

public:
    ///////////////////////////////////////////////////////////////////////////
    // Class Constants
    ///////////////////////////////////////////////////////////////////////////
    static TMatrix3x3<T> Zero;       //<! Zero matrix
    static TMatrix3x3<T> Identity;   //<! Identity matrix

public:
    ///////////////////////////////////////////////////////////////////////////
    // Class Member
    ///////////////////////////////////////////////////////////////////////////
    T data[3][3];   //<! 3x3 matrix elements

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor, initializes to identity matrix.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMatrix3x3(void)
        : data{
            { 1.0f, 0.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f }
    }
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Parameterized constructor to initialize matrix elements.
    ///
    /// \param m00 Element at row 0, column 0.
    /// \param m01 Element at row 0, column 1.
    /// \param m02 Element at row 0, column 2.
    /// \param m10 Element at row 1, column 0.
    /// \param m11 Element at row 1, column 1.
    /// \param m12 Element at row 1, column 2.
    /// \param m20 Element at row 2, column 0.
    /// \param m21 Element at row 2, column 1.
    /// \param m22 Element at row 2, column 2.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMatrix3x3(T m00, T m01, T m02, T m10, T m11, T m12, T m20, T m21, T m22)
        : data{
            { m00, m01, m02 },
            { m10, m11, m12 },
            { m20, m21, m22 }
    }
    {}

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Addition operator.
    ///
    /// \param other The matrix to add.
    ///
    /// \return Result of addition.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMatrix3x3<T> operator+(const TMatrix3x3<T>& other) const
    {
        return TMatrix3x3<T>(
            data[0][0] + other.data[0][0],
            data[0][1] + other.data[0][1],
            data[0][2] + other.data[0][2],
            data[1][0] + other.data[1][0],
            data[1][1] + other.data[1][1],
            data[1][2] + other.data[1][2],
            data[2][0] + other.data[2][0],
            data[2][1] + other.data[2][1],
            data[2][2] + other.data[2][2]
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Subtraction operator.
    ///
    /// \param other The matrix to subtract.
    ///
    /// \return Result of subtraction.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMatrix3x3<T> operator-(const TMatrix3x3<T>& other) const
    {
        return TMatrix3x3<T>(
            data[0][0] - other.data[0][0],
            data[0][1] - other.data[0][1],
            data[0][2] - other.data[0][2],
            data[1][0] - other.data[1][0],
            data[1][1] - other.data[1][1],
            data[1][2] - other.data[1][2],
            data[2][0] - other.data[2][0],
            data[2][1] - other.data[2][1],
            data[2][2] - other.data[2][2]
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Matrix multiplication operator.
    ///
    /// \param other The matrix to multiply with.
    ///
    /// \return Result of multiplication.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMatrix3x3<T> operator*(const TMatrix3x3<T>& other) const
    {
        return TMatrix3x3<T>(
            data[0][0] * other.data[0][0] + data[0][1] * other.data[1][0] +
                data[0][2] * other.data[2][0],
            data[0][0] * other.data[0][1] + data[0][1] * other.data[1][1] +
                data[0][2] * other.data[2][1],
            data[0][0] * other.data[0][2] + data[0][1] * other.data[1][2] +
                data[0][2] * other.data[2][2],

            data[1][0] * other.data[0][0] + data[1][1] * other.data[1][0] +
                data[1][2] * other.data[2][0],
            data[1][0] * other.data[0][1] + data[1][1] * other.data[1][1] +
                data[1][2] * other.data[2][1],
            data[1][0] * other.data[0][2] + data[1][1] * other.data[1][2] +
                data[1][2] * other.data[2][2],

            data[2][0] * other.data[0][0] + data[2][1] * other.data[1][0] +
                data[2][2] * other.data[2][0],
            data[2][0] * other.data[0][1] + data[2][1] * other.data[1][1] +
                data[2][2] * other.data[2][1],
            data[2][0] * other.data[0][2] + data[2][1] * other.data[1][2] +
                data[2][2] * other.data[2][2]
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Scalar multiplication operator.
    ///
    /// \param scalar The scalar to multiply with.
    ///
    /// \return Result of multiplication.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMatrix3x3<T> operator*(T scalar) const
    {
        return TMatrix3x3<T>(
            data[0][0] * scalar,
            data[0][1] * scalar,
            data[0][2] * scalar,
            data[1][0] * scalar,
            data[1][1] * scalar,
            data[1][2] * scalar,
            data[2][0] * scalar,
            data[2][1] * scalar,
            data[2][2] * scalar
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Equality comparison operator.
    ///
    /// \param other The matrix to compare with.
    ///
    /// \return True if matrices are equal, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool operator==(const TMatrix3x3<T>& other) const
    {
        return data[0][0] == other.data[0][0] &&
               data[0][1] == other.data[0][1] &&
               data[0][2] == other.data[0][2] &&
               data[1][0] == other.data[1][0] &&
               data[1][1] == other.data[1][1] &&
               data[1][2] == other.data[1][2] &&
               data[2][0] == other.data[2][0] &&
               data[2][1] == other.data[2][1] &&
               data[2][2] == other.data[2][2];
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Inequality comparison operator.
    ///
    /// \param other The matrix to compare with.
    ///
    /// \return True if matrices are not equal, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool operator!=(const TMatrix3x3<T>& other) const
    {
        return !(*this == other);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Element access operator.
    ///
    /// \param row Row index (0-2).
    /// \param col Column index (0-2).
    ///
    /// \return Reference to the element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    T& operator()(SizeT row, SizeT col) { return data[row][col]; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Const element access operator.
    ///
    /// \param row Row index (0-2).
    /// \param col Column index (0-2).
    ///
    /// \return Const reference to the element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const T& operator()(SizeT row, SizeT col) const { return data[row][col]; }

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Sets the matrix to identity.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetIdentity(void)
    {
        data[0][0] = T(1);
        data[0][1] = T(0);
        data[0][2] = T(0);
        data[1][0] = T(0);
        data[1][1] = T(1);
        data[1][2] = T(0);
        data[2][0] = T(0);
        data[2][1] = T(0);
        data[2][2] = T(1);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Sets the matrix to zero.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetZero(void)
    {
        data[0][0] = T(0);
        data[0][1] = T(0);
        data[0][2] = T(0);
        data[1][0] = T(0);
        data[1][1] = T(0);
        data[1][2] = T(0);
        data[2][0] = T(0);
        data[2][1] = T(0);
        data[2][2] = T(0);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Calculates the determinant of the matrix.
    ///
    /// \return The determinant value.
    ///
    ///////////////////////////////////////////////////////////////////////////
    T Determinant(void) const
    {
        return data[0][0] *
                   (data[1][1] * data[2][2] - data[1][2] * data[2][1]) -
               data[0][1] *
                   (data[1][0] * data[2][2] - data[1][2] * data[2][0]) +
               data[0][2] *
                   (data[1][0] * data[2][1] - data[1][1] * data[2][0]);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Calculates the transpose of the matrix.
    ///
    /// \return Transposed matrix.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMatrix3x3<T> Transpose(void) const
    {
        return TMatrix3x3<T>(
            data[0][0],
            data[1][0],
            data[2][0],
            data[0][1],
            data[1][1],
            data[2][1],
            data[0][2],
            data[1][2],
            data[2][2]
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Calculates the inverse of the matrix.
    ///
    /// \return Inverse matrix.
    ///
    /// \throws std::runtime_error if matrix is not invertible.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMatrix3x3<T> Inverse(void) const
    {
        T det = Determinant();
        if (det == 0) { throw std::runtime_error("Matrix is not invertible"); }

        T invDet = T(1) / det;

        return TMatrix3x3<T>(
            (data[1][1] * data[2][2] - data[1][2] * data[2][1]) * invDet,
            (data[0][2] * data[2][1] - data[0][1] * data[2][2]) * invDet,
            (data[0][1] * data[1][2] - data[0][2] * data[1][1]) * invDet,

            (data[1][2] * data[2][0] - data[1][0] * data[2][2]) * invDet,
            (data[0][0] * data[2][2] - data[0][2] * data[2][0]) * invDet,
            (data[0][2] * data[1][0] - data[0][0] * data[1][2]) * invDet,

            (data[1][0] * data[2][1] - data[1][1] * data[2][0]) * invDet,
            (data[0][1] * data[2][0] - data[0][0] * data[2][1]) * invDet,
            (data[0][0] * data[1][1] - data[0][1] * data[1][0]) * invDet
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Calculates the trace of the matrix (sum of diagonal elements).
    ///
    /// \return The trace value.
    ///
    ///////////////////////////////////////////////////////////////////////////
    T Trace(void) const { return data[0][0] + data[1][1] + data[2][2]; }
};

///////////////////////////////////////////////////////////////////////////////
/// \brief Global scalar multiplication operator.
///
/// \param scalar The scalar value.
/// \param mat The matrix to multiply.
///
/// \return Result of multiplication.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TMatrix3x3<T> operator*(T scalar, const TMatrix3x3<T>& mat)
{
    return mat * scalar;
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
TMatrix3x3<T> TMatrix3x3<T>::Zero =
    TMatrix3x3<T>(T(0), T(0), T(0), T(0), T(0), T(0), T(0), T(0), T(0));

///////////////////////////////////////////////////////////////////////////////
template <typename T>
TMatrix3x3<T> TMatrix3x3<T>::Identity =
    TMatrix3x3<T>(T(1), T(0), T(0), T(0), T(1), T(0), T(0), T(0), T(1));

}   // namespace tkd
