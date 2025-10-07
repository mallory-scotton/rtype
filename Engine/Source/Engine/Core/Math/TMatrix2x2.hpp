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
/// \brief Template class for a 2x2 matrix.
///
/// \tparam T The type of the matrix elements (e.g., float, double).
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
class TMatrix2x2
{
public:
    static_assert(
        std::is_arithmetic<T>::value, "T must be an arithmetic type"
    );

public:
    ///////////////////////////////////////////////////////////////////////////
    // Static Member
    ///////////////////////////////////////////////////////////////////////////
    static const TMatrix2x2<T> Identity;   //<! Identity matrix
    static const TMatrix2x2<T> Zero;       //<! Zero matrix

public:
    ///////////////////////////////////////////////////////////////////////////
    // Class Member
    ///////////////////////////////////////////////////////////////////////////
    T data[2][2];   //<! 2x2 matrix elements

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor, initializes to identity matrix.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMatrix2x2(void)
        : data{
            { 1.0f, 0.0f },
            { 0.0f, 1.0f }
    }
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Parameterized constructor to initialize matrix elements.
    ///
    /// \param m00 Element at row 0, column 0.
    /// \param m01 Element at row 0, column 1.
    /// \param m10 Element at row 1, column 0.
    /// \param m11 Element at row 1, column 1.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMatrix2x2(T m00, T m01, T m10, T m11)
        : data{
            { m00, m01 },
            { m10, m11 }
    }
    {}

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Element access operator.
    ///
    /// \param row Row index (0-1).
    /// \param col Column index (0-1).
    ///
    /// \return Reference to the element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    T& operator()(SizeT row, SizeT col) { return data[row][col]; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Const element access operator.
    ///
    /// \param row Row index (0-1).
    /// \param col Column index (0-1).
    ///
    /// \return Const reference to the element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const T& operator()(SizeT row, SizeT col) const { return data[row][col]; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Addition operator.
    ///
    /// \param other The matrix to add.
    ///
    /// \return Result of addition.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMatrix2x2<T> operator+(const TMatrix2x2<T>& other) const
    {
        return TMatrix2x2<T>(
            data[0][0] + other.data[0][0],
            data[0][1] + other.data[0][1],
            data[1][0] + other.data[1][0],
            data[1][1] + other.data[1][1]
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
    TMatrix2x2<T> operator-(const TMatrix2x2<T>& other) const
    {
        return TMatrix2x2<T>(
            data[0][0] - other.data[0][0],
            data[0][1] - other.data[0][1],
            data[1][0] - other.data[1][0],
            data[1][1] - other.data[1][1]
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
    TMatrix2x2<T> operator*(const TMatrix2x2<T>& other) const
    {
        return TMatrix2x2<T>(
            data[0][0] * other.data[0][0] + data[0][1] * other.data[1][0],
            data[0][0] * other.data[0][1] + data[0][1] * other.data[1][1],
            data[1][0] * other.data[0][0] + data[1][1] * other.data[1][0],
            data[1][0] * other.data[0][1] + data[1][1] * other.data[1][1]
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
    TMatrix2x2<T> operator*(T scalar) const
    {
        return TMatrix2x2<T>(
            data[0][0] * scalar,
            data[0][1] * scalar,
            data[1][0] * scalar,
            data[1][1] * scalar
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Equality comparison operator.
    ///
    /// \param other The matrix to compare with.
    /// \return True if matrices are equal, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool operator==(const TMatrix2x2<T>& other) const
    {
        return data[0][0] == other.data[0][0] &&
               data[0][1] == other.data[0][1] &&
               data[1][0] == other.data[1][0] &&
               data[1][1] == other.data[1][1];
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Inequality comparison operator.
    ///
    /// \param other The matrix to compare with.
    /// \return True if matrices are not equal, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool operator!=(const TMatrix2x2<T>& other) const
    {
        return !(*this == other);
    }

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Sets the matrix to identity.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetIdentity(void)
    {
        data[0][0] = T(1);
        data[0][1] = T(0);
        data[1][0] = T(0);
        data[1][1] = T(1);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Sets the matrix to zero.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetZero(void)
    {
        data[0][0] = T(0);
        data[0][1] = T(0);
        data[1][0] = T(0);
        data[1][1] = T(0);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Calculates the determinant of the matrix.
    ///
    /// \return The determinant value.
    ///
    ///////////////////////////////////////////////////////////////////////////
    T Determinant(void) const
    {
        return data[0][0] * data[1][1] - data[0][1] * data[1][0];
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Calculates the transpose of the matrix.
    ///
    /// \return Transposed matrix.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMatrix2x2<T> Transpose(void) const
    {
        return TMatrix2x2<T>(data[0][0], data[1][0], data[0][1], data[1][1]);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Calculates the inverse of the matrix.
    ///
    /// \return Inverse matrix.
    ///
    /// \throws std::runtime_error if matrix is not invertible.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMatrix2x2<T> Inverse(void) const
    {
        T det = Determinant();
        if (det == T(0))
        {
            throw std::runtime_error("Matrix is not invertible");
        }

        T invDet = T(1) / det;
        return TMatrix2x2<T>(
            data[1][1] * invDet,
            -data[0][1] * invDet,
            -data[1][0] * invDet,
            data[0][0] * invDet
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Calculates the trace of the matrix (sum of diagonal elements).
    ///
    /// \return The trace value.
    ///
    ///////////////////////////////////////////////////////////////////////////
    T Trace(void) const { return data[0][0] + data[1][1]; }
};

///////////////////////////////////////////////////////////////////////////////
template <typename T>
const TMatrix2x2<T> TMatrix2x2<T>::Identity =
    TMatrix2x2<T>(T(1), T(0), T(0), T(1));

///////////////////////////////////////////////////////////////////////////////
template <typename T>
const TMatrix2x2<T> TMatrix2x2<T>::Zero =
    TMatrix2x2<T>(T(0), T(0), T(0), T(0));

}   // namespace tkd
