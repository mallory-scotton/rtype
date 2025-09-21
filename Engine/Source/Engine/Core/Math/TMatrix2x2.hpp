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
class TMatrix2x2
{
public:
    static_assert(
        std::is_arithmetic<T>::value, "T must be an arithmetic type"
    );

public:
    ///////////////////////////////////////////////////////////////////////////
    // Class Member
    ///////////////////////////////////////////////////////////////////////////
    T m[2][2];  //!< 2x2 matrix elements

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor, initializes to identity matrix.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMatrix2x2(void)
        : m{
            {1.0f, 0.0f},
            {0.0f, 1.0f}
        }
    {};

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Parameterized constructor to initialize matrix elements.
    ///
    /// \param m00 Element at row 0, column 0.
    /// \param m01 Element at row 0, column 1.
    /// \param m10 Element at row 1, column 0.
    /// \param m11 Element at row 1, column 1.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMatrix2x2(
        T m00, T m01,
        T m10, T m11
    )
        : m{
            {m00, m01},
            {m10, m11}
        }
    {};

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Element access operator.
    ///
    /// \param row Row index (0-1).
    /// \param col Column index (0-1).
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
    /// \param row Row index (0-1).
    /// \param col Column index (0-1).
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
        m[0][0] = 1.0f; m[0][1] = 0.0f;
        m[1][0] = 0.0f; m[1][1] = 1.0f;
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Sets the matrix to zero.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetZero()
    {
        m[0][0] = 0.0f; m[0][1] = 0.0f;
        m[1][0] = 0.0f; m[1][1] = 0.0f;
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Calculates the determinant of the matrix.
    ///
    /// \return The determinant value.
    ///
    ///////////////////////////////////////////////////////////////////////////
    T Determinant() const
    {
        return m[0][0] * m[1][1] - m[0][1] * m[1][0];
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Calculates the transpose of the matrix.
    ///
    /// \return Transposed matrix.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMatrix2x2<T> Transpose() const
    {
        return TMatrix2x2<float>(
            m[0][0], m[1][0],
            m[0][1], m[1][1]
        );
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Calculates the inverse of the matrix.
    ///
    /// \return Inverse matrix.
    /// \throws std::runtime_error if matrix is not invertible.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMatrix2x2<T> Inverse() const
    {
        float det = Determinant();
        if (det == 0)
        {
            throw std::runtime_error("Matrix is not invertible");
        }

        float invDet = 1.0f / det;
        return TMatrix2x2<float>(
            m[1][1] * invDet, -m[0][1] * invDet,
            -m[1][0] * invDet, m[0][0] * invDet
        );
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Addition operator.
    ///
    /// \param other The matrix to add.
    /// \return Result of addition.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMatrix2x2<T> operator+(const TMatrix2x2<T>& other) const
    {
        return TMatrix2x2<float>(
            m[0][0] + other.m[0][0], m[0][1] + other.m[0][1],
            m[1][0] + other.m[1][0], m[1][1] + other.m[1][1]
        );
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Subtraction operator.
    ///
    /// \param other The matrix to subtract.
    /// \return Result of subtraction.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMatrix2x2<T> operator-(const TMatrix2x2<T>& other) const
    {
        return TMatrix2x2<float>(
            m[0][0] - other.m[0][0], m[0][1] - other.m[0][1],
            m[1][0] - other.m[1][0], m[1][1] - other.m[1][1]
        );
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Matrix multiplication operator.
    ///
    /// \param other The matrix to multiply with.
    /// \return Result of multiplication.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMatrix2x2<T> operator*(const TMatrix2x2<T>& other) const
    {
        return TMatrix2x2<float>(
            m[0][0] * other.m[0][0] + m[0][1] * other.m[1][0],
            m[0][0] * other.m[0][1] + m[0][1] * other.m[1][1],
            m[1][0] * other.m[0][0] + m[1][1] * other.m[1][0],
            m[1][0] * other.m[0][1] + m[1][1] * other.m[1][1]
        );
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Scalar multiplication operator.
    ///
    /// \param scalar The scalar to multiply with.
    /// \return Result of multiplication.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMatrix2x2<T> operator*(T scalar) const
    {
        return TMatrix2x2<float>(
            m[0][0] * scalar, m[0][1] * scalar,
            m[1][0] * scalar, m[1][1] * scalar
        );
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Equality comparison operator.
    ///
    /// \param other The matrix to compare with.
    /// \return True if matrices are equal, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool operator==(const TMatrix2x2<T>& other) const
    {
        return m[0][0] == other.m[0][0] && m[0][1] == other.m[0][1] &&
               m[1][0] == other.m[1][0] && m[1][1] == other.m[1][1];
    };

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
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Calculates the trace of the matrix (sum of diagonal elements).
    ///
    /// \return The trace value.
    ///
    ///////////////////////////////////////////////////////////////////////////
    T Trace(void) const
    {
        return m[0][0] + m[1][1];
    };

};

}   // namespace tkd
