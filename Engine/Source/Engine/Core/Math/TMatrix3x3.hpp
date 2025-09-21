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
    // Class Member
    ///////////////////////////////////////////////////////////////////////////
    T m[3][3];  //!< 3x3 matrix elements

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor, initializes to identity matrix.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMatrix3x3(void)
        : m{
            {1.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 1.0f}
        }
    {};

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
    TMatrix3x3(
        T m00, T m01, T m02,
        T m10, T m11, T m12,
        T m20, T m21, T m22
    )
        : m{
            {m00, m01, m02},
            {m10, m11, m12},
            {m20, m21, m22}
        }
    {};

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Element access operator.
    ///
    /// \param row Row index (0-2).
    /// \param col Column index (0-2).
    /// \return Reference to the element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    T& operator()(std::size_t row, std::size_t col)
    {
        return m[row][col];
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Const element access operator.
    ///
    /// \param row Row index (0-2).
    /// \param col Column index (0-2).
    /// \return Const reference to the element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const T& operator()(std::size_t row, std::size_t col) const
    {
        return m[row][col];
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Sets the matrix to identity.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetIdentity()
    {
        m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f;
        m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f;
        m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f;
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Sets the matrix to zero.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetZero()
    {
        m[0][0] = 0.0f; m[0][1] = 0.0f; m[0][2] = 0.0f;
        m[1][0] = 0.0f; m[1][1] = 0.0f; m[1][2] = 0.0f;
        m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 0.0f;
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Calculates the determinant of the matrix.
    ///
    /// \return The determinant value.
    ///
    ///////////////////////////////////////////////////////////////////////////
    T Determinant() const
    {
        return
            m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
            m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
            m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Calculates the transpose of the matrix.
    ///
    /// \return Transposed matrix.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMatrix3x3<T> Transpose() const
    {
        return TMatrix3x3<float>(
            m[0][0], m[1][0], m[2][0],
            m[0][1], m[1][1], m[2][1],
            m[0][2], m[1][2], m[2][2]
        );
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Calculates the inverse of the matrix.
    ///
    /// \return Inverse matrix.
    /// \throws std::runtime_error if matrix is not invertible.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMatrix3x3<T> Inverse() const
    {
        float det = Determinant();
        if (det == 0)
        {
            throw std::runtime_error("Matrix is not invertible");
        }

        float invDet = 1.0f / det;

        return TMatrix3x3<float>(
            (m[1][1] * m[2][2] - m[1][2] * m[2][1]) * invDet,
            (m[0][2] * m[2][1] - m[0][1] * m[2][2]) * invDet,
            (m[0][1] * m[1][2] - m[0][2] * m[1][1]) * invDet,

            (m[1][2] * m[2][0] - m[1][0] * m[2][2]) * invDet,
            (m[0][0] * m[2][2] - m[0][2] * m[2][0]) * invDet,
            (m[0][2] * m[1][0] - m[0][0] * m[1][2]) * invDet,

            (m[1][0] * m[2][1] - m[1][1] * m[2][0]) * invDet,
            (m[0][1] * m[2][0] - m[0][0] * m[2][1]) * invDet,
            (m[0][0] * m[1][1] - m[0][1] * m[1][0]) * invDet
        );
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Addition operator.
    ///
    /// \param other The matrix to add.
    /// \return Result of addition.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMatrix3x3<T> operator+(const TMatrix3x3<T>& other) const
    {
        return TMatrix3x3<float>(
            m[0][0] + other.m[0][0], m[0][1] + other.m[0][1], m[0][2] + other.m[0][2],
            m[1][0] + other.m[1][0], m[1][1] + other.m[1][1], m[1][2] + other.m[1][2],
            m[2][0] + other.m[2][0], m[2][1] + other.m[2][1], m[2][2] + other.m[2][2]
        );
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Subtraction operator.
    ///
    /// \param other The matrix to subtract.
    /// \return Result of subtraction.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMatrix3x3<T> operator-(const TMatrix3x3<T>& other) const
    {
        return TMatrix3x3<float>(
            m[0][0] - other.m[0][0], m[0][1] - other.m[0][1], m[0][2] - other.m[0][2],
            m[1][0] - other.m[1][0], m[1][1] - other.m[1][1], m[1][2] - other.m[1][2],
            m[2][0] - other.m[2][0], m[2][1] - other.m[2][1], m[2][2] - other.m[2][2]
        );
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Matrix multiplication operator.
    ///
    /// \param other The matrix to multiply with.
    /// \return Result of multiplication.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMatrix3x3<T> operator*(const TMatrix3x3<T>& other) const
    {
        return TMatrix3x3<float>(
            m[0][0] * other.m[0][0] + m[0][1] * other.m[1][0] + m[0][2] * other.m[2][0],
            m[0][0] * other.m[0][1] + m[0][1] * other.m[1][1] + m[0][2] * other.m[2][1],
            m[0][0] * other.m[0][2] + m[0][1] * other.m[1][2] + m[0][2] * other.m[2][2],

            m[1][0] * other.m[0][0] + m[1][1] * other.m[1][0] + m[1][2] * other.m[2][0],
            m[1][0] * other.m[0][1] + m[1][1] * other.m[1][1] + m[1][2] * other.m[2][1],
            m[1][0] * other.m[0][2] + m[1][1] * other.m[1][2] + m[1][2] * other.m[2][2],

            m[2][0] * other.m[0][0] + m[2][1] * other.m[1][0] + m[2][2] * other.m[2][0],
            m[2][0] * other.m[0][1] + m[2][1] * other.m[1][1] + m[2][2] * other.m[2][1],
            m[2][0] * other.m[0][2] + m[2][1] * other.m[1][2] + m[2][2] * other.m[2][2]
        );
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Scalar multiplication operator.
    ///
    /// \param scalar The scalar to multiply with.
    /// \return Result of multiplication.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMatrix3x3<T> operator*(T scalar) const
    {
        return TMatrix3x3<float>(
            m[0][0] * scalar, m[0][1] * scalar, m[0][2] * scalar,
            m[1][0] * scalar, m[1][1] * scalar, m[1][2] * scalar,
            m[2][0] * scalar, m[2][1] * scalar, m[2][2] * scalar
        );
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Equality comparison operator.
    ///
    /// \param other The matrix to compare with.
    /// \return True if matrices are equal, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool operator==(const TMatrix3x3<T>& other) const
    {
        return m[0][0] == other.m[0][0] && m[0][1] == other.m[0][1] && m[0][2] == other.m[0][2] &&
               m[1][0] == other.m[1][0] && m[1][1] == other.m[1][1] && m[1][2] == other.m[1][2] &&
               m[2][0] == other.m[2][0] && m[2][1] == other.m[2][1] && m[2][2] == other.m[2][2];
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Inequality comparison operator.
    ///
    /// \param other The matrix to compare with.
    /// \return True if matrices are not equal, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool operator!=(const TMatrix3x3<T>& other) const
    {
        return !(*this == other);
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Calculates the trace of the matrix (sum of diagonal elements).
    ///
    /// \return The trace value.
    ///
    ///////////////////////////////////////////////////////////////////////////
    T Trace() const
    {
        return m[0][0] + m[1][1] + m[2][2];
    };
};

///////////////////////////////////////////////////////////////////////////
/// \brief Global scalar multiplication operator.
///
/// \param scalar The scalar value.
/// \param mat The matrix to multiply.
/// \return Result of multiplication.
///
///////////////////////////////////////////////////////////////////////////
template <typename T>
TMatrix3x3<T> operator*(T scalar, const TMatrix3x3<T>& mat)
{
    return mat * scalar;
}

}   // namespace tkd
