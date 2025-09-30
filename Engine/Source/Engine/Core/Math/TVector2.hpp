///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <cassert>
#include <cmath>
#include <Engine/Config.hpp>
#include <math.h>
#include <ostream>
#include <stdexcept>
#include <type_traits>
#include <utility>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief A 2D vector template class for arithmetic types.
///
/// \tparam T The arithmetic type for the vector components.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
class TVector2
{
public:
    static_assert(
        std::is_arithmetic<T>::value, "T must be an arithmetic type"
    );

public:
    ///////////////////////////////////////////////////////////////////////////
    // Class Member
    ///////////////////////////////////////////////////////////////////////////
    union
    {
#ifdef __GNUC__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpedantic"
#elif defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable : 4201)
#endif
        struct
        {
            T x;   //<! The x-component of the vector.
            T y;   //<! The y-component of the vector.
        };
#ifdef __GNUC__
    #pragma GCC diagnostic pop
#elif defined(_MSC_VER)
    #pragma warning(pop)
#endif
        T data[2];   //<! Array access to the components.
    };

public:
    ///////////////////////////////////////////////////////////////////////////
    // Static Member
    ///////////////////////////////////////////////////////////////////////////
    static const TVector2 Zero;    //<! A vector with both components set to 0.
    static const TVector2 One;     //<! A vector with both components set to 1.
    static const TVector2 UnitX;   //<! A unit vector along the x-axis (1, 0).
    static const TVector2 UnitY;   //<! A unit vector along the y-axis (0, 1).

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor. Initializes the vector to (0, 0).
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector2(void)
        : x(0)
        , y(0)
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructs a vector with all components set to the same value.
    ///
    /// \param all All the components.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector2(const T& all)
        : x(all)
        , y(all)
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructs a vector with the specified x and y components.
    ///
    /// \param x The x-component.
    /// \param y The y-component.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector2(const T& x, const T& y)
        : x(x)
        , y(y)
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Copy constructor. Creates a copy of another vector.
    ///
    /// \param other The vector to copy from.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector2(const TVector2& other)
        : x(other.x)
        , y(other.y)
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Move constructor. Moves the contents of another vector.
    ///
    /// \param other The vector to move from.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector2(TVector2&& other) noexcept
        : x(std::move(other.x))
        , y(std::move(other.y))
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructs a vector from components of a different type,
    /// casting to T.
    ///
    /// \tparam U The type of the components.
    ///
    /// \param x The x-component.
    /// \param y The y-component.
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename U>
    TVector2(const U& x, const U& y)
        : x(static_cast<T>(x))
        , y(static_cast<T>(y))
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructs a vector from another TVector2 of a different type,
    /// casting to T.
    ///
    /// \tparam U The type of the other vector's components.
    ///
    /// \param other The vector to copy from.
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename U>
    TVector2(const TVector2<U>& other)
        : x(static_cast<T>(other.x))
        , y(static_cast<T>(other.y))
    {}

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Copy assignment operator.
    ///
    /// \param other The vector to assign from.
    ///
    /// \return Reference to this vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector2& operator=(const TVector2& other)
    {
        if (this != &other)
        {
            x = other.x;
            y = other.y;
        }
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Move assignment operator.
    ///
    /// \param other The vector to move from.
    ///
    /// \return Reference to this vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector2& operator=(TVector2&& other) noexcept
    {
        if (this != &other)
        {
            x = std::move(other.x);
            y = std::move(other.y);
        }
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Accesses the component at the specified index.
    ///
    /// \param index The index (0 for x, 1 for y).
    ///
    /// \return Reference to the component.
    ///
    ///////////////////////////////////////////////////////////////////////////
    T& operator[](SizeT index)
    {
        assert(index < 2);
        return data[index];
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Accesses the component at the specified index (const version).
    ///
    /// \param index The index (0 for x, 1 for y).
    ///
    /// \return Const reference to the component.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const T& operator[](SizeT index) const
    {
        assert(index < 2);
        return data[index];
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Pre-increment operator. Increments both components by 1.
    ///
    /// \return Reference to this vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector2& operator++(void)
    {
        ++x;
        ++y;
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Post-increment operator. Increments both components by 1.
    ///
    /// \return A copy of the vector before incrementing.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector2 operator++(int)
    {
        TVector2 temp(*this);
        ++(*this);
        return temp;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Pre-decrement operator. Decrements both components by 1.
    ///
    /// \return Reference to this vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector2& operator--(void)
    {
        --x;
        --y;
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Post-decrement operator. Decrements both components by 1.
    ///
    /// \return A copy of the vector before decrementing.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector2 operator--(int)
    {
        TVector2 temp(*this);
        --(*this);
        return temp;
    }

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Calculates the dot product with another vector.
    ///
    /// \param other The other vector.
    ///
    /// \return The dot product.
    ///
    ///////////////////////////////////////////////////////////////////////////
    T Dot(const TVector2& other) const { return x * other.x + y * other.y; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Calculates the cross product with another vector.
    ///
    /// \param other The other vector.
    ///
    /// \return The scalar cross product result.
    ///
    ///////////////////////////////////////////////////////////////////////////
    T Cross(const TVector2& other) const { return x * other.y - y * other.x; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Calculates the length (magnitude) of the vector.
    ///
    /// \return The length.
    ///
    ///////////////////////////////////////////////////////////////////////////
    T Length(void) const { return static_cast<T>(std::sqrt(x * x + y * y)); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Calculates the squared length of the vector.
    ///
    /// \return The squared length.
    ///
    ///////////////////////////////////////////////////////////////////////////
    T LengthSquared(void) const { return x * x + y * y; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Computes the perpendicular vector (rotated 90 degrees)
    ///
    /// \return The perpendicular vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector2<T> Perpendicular(void) const { return TVector2<T>(-y, x); }
};

///////////////////////////////////////////////////////////////////////////////
// Static Member Initialization
///////////////////////////////////////////////////////////////////////////////
template <typename T>
const TVector2<T> TVector2<T>::Zero(0, 0);
template <typename T>
const TVector2<T> TVector2<T>::One(1, 1);
template <typename T>
const TVector2<T> TVector2<T>::UnitX(1, 0);
template <typename T>
const TVector2<T> TVector2<T>::UnitY(0, 1);

///////////////////////////////////////////////////////////////////////////////
/// \brief Unary plus operator. Returns the vector unchanged.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The vector.
///
/// \return The vector.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector2<T> operator+(const TVector2<T>& lhs)
{
    return TVector2<T>(+lhs.x, +lhs.y);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Adds two vectors component-wise.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The left-hand side vector.
/// \param rhs The right-hand side vector.
///
/// \return The resulting vector.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector2<T> operator+(const TVector2<T>& lhs, const TVector2<T>& rhs)
{
    return TVector2<T>(lhs.x + rhs.x, lhs.y + rhs.y);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Adds a scalar to each component of the vector.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The vector.
/// \param rhs The scalar.
///
/// \return The resulting vector.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector2<T> operator+(const TVector2<T>& lhs, const T& rhs)
{
    return TVector2<T>(lhs.x + rhs, lhs.y + rhs);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Adds a scalar to each component of the vector (commutative).
///
/// \tparam T The type of the vector components.
///
/// \param lhs The scalar.
/// \param rhs The vector.
///
/// \return The resulting vector.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector2<T> operator+(const T& lhs, const TVector2<T>& rhs)
{
    return TVector2<T>(lhs + rhs.x, lhs + rhs.y);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Compound addition assignment for vectors.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The vector to modify.
/// \param rhs The vector to add.
///
/// \return Reference to the modified vector.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector2<T> operator+=(TVector2<T>& lhs, const TVector2<T>& rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    return lhs;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Compound addition assignment for scalar.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The vector to modify.
/// \param rhs The scalar to add.
///
/// \return Reference to the modified vector.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector2<T> operator+=(TVector2<T>& lhs, const T& rhs)
{
    lhs.x += rhs;
    lhs.y += rhs;
    return lhs;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Unary minus operator. Negates the vector.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The vector.
///
/// \return The negated vector.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector2<T> operator-(const TVector2<T>& lhs)
{
    return TVector2<T>(-lhs.x, -lhs.y);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Subtracts two vectors component-wise.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The left-hand side vector.
/// \param rhs The right-hand side vector.
///
/// \return The resulting vector.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector2<T> operator-(const TVector2<T>& lhs, const TVector2<T>& rhs)
{
    return TVector2<T>(lhs.x - rhs.x, lhs.y - rhs.y);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Subtracts a scalar from each component of the vector.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The vector.
/// \param rhs The scalar.
///
/// \return The resulting vector.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector2<T> operator-(const TVector2<T>& lhs, const T& rhs)
{
    return TVector2<T>(lhs.x - rhs, lhs.y - rhs);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Subtracts the vector from a scalar component-wise.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The scalar.
/// \param rhs The vector.
///
/// \return The resulting vector.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector2<T> operator-(const T& lhs, const TVector2<T>& rhs)
{
    return TVector2<T>(lhs - rhs.x, lhs - rhs.y);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Compound subtraction assignment for vectors.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The vector to modify.
/// \param rhs The vector to subtract.
///
/// \return Reference to the modified vector.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector2<T> operator-=(TVector2<T>& lhs, const TVector2<T>& rhs)
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    return lhs;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Compound subtraction assignment for scalar.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The vector to modify.
/// \param rhs The scalar to subtract.
///
/// \return Reference to the modified vector.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector2<T> operator-=(TVector2<T>& lhs, const T& rhs)
{
    lhs.x -= rhs;
    lhs.y -= rhs;
    return lhs;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Multiplies two vectors component-wise.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The left-hand side vector.
/// \param rhs The right-hand side vector.
///
/// \return The resulting vector.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector2<T> operator*(const TVector2<T>& lhs, const TVector2<T>& rhs)
{
    return TVector2<T>(lhs.x * rhs.x, lhs.y * rhs.y);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Multiplies the vector by a scalar.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The vector.
/// \param rhs The scalar.
///
/// \return The resulting vector.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector2<T> operator*(const TVector2<T>& lhs, const T& rhs)
{
    return TVector2<T>(lhs.x * rhs, lhs.y * rhs);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Multiplies a scalar by the vector (commutative).
///
/// \tparam T The type of the vector components.
///
/// \param lhs The scalar.
/// \param rhs The vector.
///
/// \return The resulting vector.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector2<T> operator*(const T& lhs, const TVector2<T>& rhs)
{
    return TVector2<T>(lhs * rhs.x, lhs * rhs.y);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Compound multiplication assignment for vectors.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The vector to modify.
/// \param rhs The vector to multiply by.
///
/// \return Reference to the modified vector.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector2<T> operator*=(TVector2<T>& lhs, const TVector2<T>& rhs)
{
    lhs.x *= rhs.x;
    lhs.y *= rhs.y;
    return lhs;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Compound multiplication assignment for scalar.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The vector to modify.
/// \param rhs The scalar to multiply by.
///
/// \return Reference to the modified vector.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector2<T> operator*=(TVector2<T>& lhs, const T& rhs)
{
    lhs.x *= rhs;
    lhs.y *= rhs;
    return lhs;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Divides two vectors component-wise.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The left-hand side vector.
/// \param rhs The right-hand side vector.
///
/// \return The resulting vector.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector2<T> operator/(const TVector2<T>& lhs, const TVector2<T>& rhs)
{
    assert(rhs.x != T(0) && rhs.y != T(0));
    return TVector2<T>(lhs.x / rhs.x, lhs.y / rhs.y);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Divides the vector by a scalar.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The vector.
/// \param rhs The scalar.
///
/// \return The resulting vector.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector2<T> operator/(const TVector2<T>& lhs, const T& rhs)
{
    assert(rhs != T(0));
    return TVector2<T>(lhs.x / rhs, lhs.y / rhs);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Divides a scalar by the vector component-wise.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The scalar.
/// \param rhs The vector.
///
/// \return The resulting vector.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector2<T> operator/(const T& lhs, const TVector2<T>& rhs)
{
    assert(rhs.x != T(0) && rhs.y != T(0));
    return TVector2<T>(lhs / rhs.x, lhs / rhs.y);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Compound division assignment for vectors.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The vector to modify.
/// \param rhs The vector to divide by.
///
/// \return Reference to the modified vector.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector2<T> operator/=(TVector2<T>& lhs, const TVector2<T>& rhs)
{
    assert(rhs.x != T(0) && rhs.y != T(0));
    lhs.x /= rhs.x;
    lhs.y /= rhs.y;
    return lhs;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Compound division assignment for scalar.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The vector to modify.
/// \param rhs The scalar to divide by.
///
/// \return Reference to the modified vector.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector2<T> operator/=(TVector2<T>& lhs, const T& rhs)
{
    assert(rhs != T(0));
    lhs.x /= rhs;
    lhs.y /= rhs;
    return lhs;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Checks if two vectors are equal.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The left-hand side vector.
/// \param rhs The right-hand side vector.
///
/// \return True if both components are equal.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
bool operator==(const TVector2<T>& lhs, const TVector2<T>& rhs)
{
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Checks if a vector is equal to a scalar in both components.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The vector.
/// \param rhs The scalar.
///
/// \return True if both components equal the scalar.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
bool operator==(const TVector2<T>& lhs, const T& rhs)
{
    return lhs.x == rhs && lhs.y == rhs;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Checks if a scalar is equal to both components of a vector.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The scalar.
/// \param rhs The vector.
///
/// \return True if the scalar equals both components.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
bool operator==(const T& lhs, const TVector2<T>& rhs)
{
    return lhs == rhs.x && lhs == rhs.y;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Checks if two vectors are not equal.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The left-hand side vector.
/// \param rhs The right-hand side vector.
///
/// \return True if any component differs.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
bool operator!=(const TVector2<T>& lhs, const TVector2<T>& rhs)
{
    return !(lhs == rhs);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Checks if a vector is not equal to a scalar in both components.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The vector.
/// \param rhs The scalar.
///
/// \return True if any component differs from the scalar.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
bool operator!=(const TVector2<T>& lhs, const T& rhs)
{
    return !(lhs == rhs);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Checks if a scalar is not equal to both components of a vector.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The scalar.
/// \param rhs The vector.
///
/// \return True if the scalar differs from any component.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
bool operator!=(const T& lhs, const TVector2<T>& rhs)
{
    return !(lhs == rhs);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Checks if the first vector is greater than or equal to the second
/// component-wise.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The left-hand side vector.
/// \param rhs The right-hand side vector.
///
/// \return True if both components of lhs are >= corresponding in rhs.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
bool operator>=(const TVector2<T>& lhs, const TVector2<T>& rhs)
{
    return lhs.x >= rhs.x && lhs.y >= rhs.y;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Checks if the vector's components are >= a scalar.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The vector.
/// \param rhs The scalar.
///
/// \return True if both components >= scalar.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
bool operator>=(const TVector2<T>& lhs, const T& rhs)
{
    return lhs.x >= rhs && lhs.y >= rhs;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Checks if a scalar is >= both components of the vector.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The scalar.
/// \param rhs The vector.
///
/// \return True if scalar >= both components.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
bool operator>=(const T& lhs, const TVector2<T>& rhs)
{
    return lhs >= rhs.x && lhs >= rhs.y;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Checks if the first vector is greater than the second
/// component-wise.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The left-hand side vector.
/// \param rhs The right-hand side vector.
///
/// \return True if both components of lhs > corresponding in rhs.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
bool operator>(const TVector2<T>& lhs, const TVector2<T>& rhs)
{
    return lhs.x > rhs.x && lhs.y > rhs.y;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Checks if the vector's components are > a scalar.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The vector.
/// \param rhs The scalar.
///
/// \return True if both components > scalar.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
bool operator>(const TVector2<T>& lhs, const T& rhs)
{
    return lhs.x > rhs && lhs.y > rhs;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Checks if a scalar is > both components of the vector.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The scalar.
/// \param rhs The vector.
///
/// \return True if scalar > both components.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
bool operator>(const T& lhs, const TVector2<T>& rhs)
{
    return lhs > rhs.x && lhs > rhs.y;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Checks if the first vector is less than or equal to the second
/// component-wise.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The left-hand side vector.
/// \param rhs The right-hand side vector.
///
/// \return True if both components of lhs <= corresponding in rhs.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
bool operator<=(const TVector2<T>& lhs, const TVector2<T>& rhs)
{
    return lhs.x <= rhs.x && lhs.y <= rhs.y;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Checks if the vector's components are <= a scalar.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The vector.
/// \param rhs The scalar.
///
/// \return True if both components <= scalar.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
bool operator<=(const TVector2<T>& lhs, const T& rhs)
{
    return lhs.x <= rhs && lhs.y <= rhs;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Checks if a scalar is <= both components of the vector.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The scalar.
/// \param rhs The vector.
///
/// \return True if scalar <= both components.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
bool operator<=(const T& lhs, const TVector2<T>& rhs)
{
    return lhs <= rhs.x && lhs <= rhs.y;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Checks if the first vector is less than the second component-wise.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The left-hand side vector.
/// \param rhs The right-hand side vector.
///
/// \return True if both components of lhs < corresponding in rhs.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
bool operator<(const TVector2<T>& lhs, const TVector2<T>& rhs)
{
    return lhs.x < rhs.x && lhs.y < rhs.y;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Checks if the vector's components are < a scalar.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The vector.
/// \param rhs The scalar.
///
/// \return True if both components < scalar.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
bool operator<(const TVector2<T>& lhs, const T& rhs)
{
    return lhs.x < rhs && lhs.y < rhs;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Checks if a scalar is < both components of the vector.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The scalar.
/// \param rhs The vector.
///
/// \return True if scalar < both components.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
bool operator<(const T& lhs, const TVector2<T>& rhs)
{
    return lhs < rhs.x && lhs < rhs.y;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Outputs the vector to an output stream.
///
/// \tparam T The type of the vector components.
///
/// \param os The output stream.
/// \param vec The vector to output.
///
/// \return The output stream.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
std::ostream& operator<<(std::ostream& os, const TVector2<T>& vec)
{
    os << "(" << vec.x << ", " << vec.y << ")";
    return os;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Adds two vectors component-wise.
///
/// \tparam T The type of the vector components.
/// \param a The first vector.
/// \param b The second vector.
/// \return The resulting vector after addition.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector2<T> Add(const TVector2<T>& a, const TVector2<T>& b)
{
    return TVector2<T>(a.x + b.x, a.y + b.y);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Subtracts one vector from another component-wise.
///
/// \tparam T The type of the vector components.
/// \param a The first vector.
/// \param b The second vector.
/// \return The resulting vector after subtraction.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector2<T> Subtract(const TVector2<T>& a, const TVector2<T>& b)
{
    return TVector2<T>(a.x - b.x, a.y - b.y);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Multiplies two vectors component-wise.
///
/// \tparam T The type of the vector components.
/// \param a The first vector.
/// \param b The second vector.
/// \return The resulting vector after multiplication.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector2<T> Multiply(const TVector2<T>& a, const TVector2<T>& b)
{
    return TVector2<T>(a.x * b.x, a.y * b.y);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Divides one vector by another component-wise.
///
/// \tparam T The type of the vector components.
/// \param a The numerator vector.
/// \param b The denominator vector (must not contain zeros).
/// \return The resulting vector after division.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector2<T> Divide(const TVector2<T>& a, const TVector2<T>& b)
{
    assert(b.x != T(0) && b.y != T(0));
    return TVector2<T>(a.x / b.x, a.y / b.y);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Performs a multiply-add operation on vectors: a * b + c.
///
/// \tparam T The type of the vector components.
/// \param a The first vector (multiplicand).
/// \param b The second vector (multiplier).
/// \param c The vector to be added.
/// \return The resulting vector after the multiply-add operation.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector2<T> MultiplyAdd(
    const TVector2<T>& a, const TVector2<T>& b, const TVector2<T>& c
)
{
    return TVector2<T>(a.x * b.x + c.x, a.y * b.y + c.y);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Computes the 2D cross product (determinant) of two vectors.
///
/// \tparam T The type of the vector components.
/// \param a The first vector.
/// \param b The second vector.
/// \return The scalar result of the cross product (a.x * b.y - a.y * b.x).
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
T Cross(const TVector2<T>& a, const TVector2<T>& b)
{
    return a.x * b.y - a.y * b.x;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Projects vector a onto vector b.
///
/// \tparam T The type of the vector components.
/// \param a The vector to project.
/// \param b The vector onto which a is projected (must not be the zero
/// vector).
/// \return The projected vector.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector2<T> Project(const TVector2<T>& a, const TVector2<T>& b)
{
    T dot = a.x * b.x + a.y * b.y;
    T lenSq = b.x * b.x + b.y * b.y;
    assert(lenSq != T(0));
    T scale = dot / lenSq;
    return TVector2<T>(b.x * scale, b.y * scale);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Reflects vector a around a normal vector n.
///
/// \tparam T The type of the vector components.
/// \param a The incident vector.
/// \param n The normal vector (assumed to be normalized).
/// \return The reflected vector.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector2<T> Reflect(const TVector2<T>& a, const TVector2<T>& n)
{
    T dot = a.x * n.x + a.y * n.y;
    return TVector2<T>(a.x - 2 * dot * n.x, a.y - 2 * dot * n.y);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Refracts vector a through a surface with normal n and refraction
/// index eta.
///
/// \tparam T The type of the vector components.
/// \param a The incident vector.
/// \param n The surface normal (assumed to be normalized).
/// \param eta The ratio of indices of refraction (n1 / n2).
/// \return The refracted vector, or (0,0) if total internal reflection occurs.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector2<T> Refract(const TVector2<T>& a, const TVector2<T>& n, T eta)
{
    T dot = a.x * n.x + a.y * n.y;
    T k = 1 - eta * eta * (1 - dot * dot);
    if (k < 0) { return TVector2<T>(0, 0); }
    else
    {
        return TVector2<T>(
            eta * a.x - (eta * dot + std::sqrt(k)) * n.x,
            eta * a.y - (eta * dot + std::sqrt(k)) * n.y
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Computes the Euclidean distance between two vectors.
///
/// \tparam T The type of the vector components.
/// \param a The first vector.
/// \param b The second vector.
/// \return The distance between the two vectors.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
T Distance(const TVector2<T>& a, const TVector2<T>& b)
{
    T dx = a.x - b.x;
    T dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Computes the length (magnitude) of a vector.
///
/// \tparam T The type of the vector components.
/// \param v The input vector.
/// \return The length of the vector.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
T Length(const TVector2<T>& v)
{
    return std::sqrt(v.x * v.x + v.y * v.y);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Returns the component-wise absolute value of a vector.
///
/// \tparam T The type of the vector components.
/// \param v The input vector.
/// \return The vector with absolute values of each component.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector2<T> Absolute(const TVector2<T>& v)
{
    return TVector2<T>(std::abs(v.x), std::abs(v.y));
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Normalizes a vector to unit length.
///
/// \tparam T The type of the vector components.
/// \param v The input vector (must not be the zero vector).
/// \return The normalized vector.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector2<T> Normalize(const TVector2<T>& v)
{
    T len = Length(v);
    assert(len != T(0));
    return TVector2<T>(v.x / len, v.y / len);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Computes the component-wise minimum of two vectors.
///
/// \tparam T The type of the vector components.
/// \param a The first vector.
/// \param b The second vector.
/// \return A vector containing the minimum of each component.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector2<T> Minimum(const TVector2<T>& a, const TVector2<T>& b)
{
    return TVector2<T>(std::min(a.x, b.x), std::min(a.y, b.y));
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Computes the component-wise maximum of two vectors.
///
/// \tparam T The type of the vector components.
/// \param a The first vector.
/// \param b The second vector.
/// \return A vector containing the maximum of each component.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector2<T> Maximum(const TVector2<T>& a, const TVector2<T>& b)
{
    return TVector2<T>(std::max(a.x, b.x), std::max(a.y, b.y));
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Applies floor operation component-wise to a vector.
///
/// \tparam T The type of the vector components.
/// \param v The input vector.
/// \return The floored vector.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector2<T> Floor(const TVector2<T>& v)
{
    return TVector2<T>(std::floor(v.x), std::floor(v.y));
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Applies ceil operation component-wise to a vector.
///
/// \tparam T The type of the vector components.
/// \param v The input vector.
/// \return The ceiled vector.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector2<T> Ceil(const TVector2<T>& v)
{
    return TVector2<T>(std::ceil(v.x), std::ceil(v.y));
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Computes the fractional part of each component of a vector.
///
/// \tparam T The type of the vector components.
/// \param v The input vector.
/// \return The fractional part of the vector components.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector2<T> Fraction(const TVector2<T>& v)
{
    return TVector2<T>(v.x - std::floor(v.x), v.y - std::floor(v.y));
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Applies the modulo operation to each component of the vector.
///
/// \tparam T The type of the vector components.
///
/// \param v The input vector.
/// \param m The modulo value.
///
/// \return A vector where each component is v mod m.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector2<T> Modulo(const TVector2<T>& v, const T& m)
{
    return TVector2<T>(std::fmod(v.x, m), std::fmod(v.y, m));
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Wraps each component of the vector within a specified range [min,
/// max).
///
/// \tparam T The type of the vector components.
///
/// \param v The input vector.
/// \param min The minimum value of the range.
/// \param max The maximum value of the range.
///
/// \return A vector with components wrapped into the [min, max) interval.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector2<T> Wrap(const TVector2<T>& v, const T& min, const T& max)
{
    auto wrap = [](T val, T min, T max)
    {
        T range = max - min;
        return min + std::fmod(std::fmod(val - min, range) + range, range);
    };
    return TVector2<T>(wrap(v.x, min, max), wrap(v.y, min, max));
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Snaps each component of the vector to a multiple of the given step.
///
/// \tparam T The type of the vector components.
///
/// \param v The input vector.
/// \param step The snapping step.
///
/// \return A vector with each component rounded to the nearest multiple of
/// step.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector2<T> Snap(const TVector2<T>& v, const T& step)
{
    auto snap = [](T val, T step) { return std::round(val / step) * step; };
    return TVector2<T>(snap(v.x, step), snap(v.y, step));
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Applies the sine function to each component of the vector.
///
/// \tparam T The type of the vector components.
///
/// \param v The input vector.
///
/// \return A vector where each component is the sine of the original
/// component.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector2<T> Sine(const TVector2<T>& v)
{
    return TVector2<T>(std::sin(v.x), std::sin(v.y));
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Applies the cosine function to each component of the vector.
///
/// \tparam T The type of the vector components.
///
/// \param v The input vector.
///
/// \return A vector where each component is the cosine of the original
/// component.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector2<T> Cosine(const TVector2<T>& v)
{
    return TVector2<T>(std::cos(v.x), std::cos(v.y));
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Applies the tangent function to each component of the vector.
///
/// \tparam T The type of the vector components.
///
/// \param v The input vector.
///
/// \return A vector where each component is the tangent of the original
/// component.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector2<T> Tangent(const TVector2<T>& v)
{
    return TVector2<T>(std::tan(v.x), std::tan(v.y));
}

}   // namespace tkd
