///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Config.hpp>
#include <type_traits>
#include <utility>
#include <ostream>
#include <cassert>
#include <stdexcept>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief A 2D vector template class for arithmetic types.
/// \tparam T The arithmetic type for the vector components.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
class TVector2
{
public:
    static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");

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
    #pragma warning(disable: 4201)
#endif
        struct
        {
            T x;        //!< The x-component of the vector.
            T y;        //!< The y-component of the vector.
        };
#ifdef __GNUC__
    #pragma GCC diagnostic pop
#elif defined(_MSC_VER)
    #pragma warning(pop)
#endif
        T data[2];      //!< Array access to the components.
    };

public:
    ///////////////////////////////////////////////////////////////////////////
    // Static Member
    ///////////////////////////////////////////////////////////////////////////
    static const TVector2 Zero;  //!< A vector with both components set to 0.
    static const TVector2 One;   //!< A vector with both components set to 1.
    static const TVector2 UnitX; //!< A unit vector along the x-axis (1, 0).
    static const TVector2 UnitY; //!< A unit vector along the y-axis (0, 1).

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
    /// \return Reference to this vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector2& operator=(const TVector2& other)
    {
        if (this != &other) {
            x = other.x;
            y = other.y;
        }
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Move assignment operator.
    ///
    /// \param other The vector to move from.
    /// \return Reference to this vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector2& operator=(TVector2&& other) noexcept
    {
        if (this != &other) {
            x = std::move(other.x);
            y = std::move(other.y);
        }
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Accesses the component at the specified index.
    ///
    /// \param index The index (0 for x, 1 for y).
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
};

///////////////////////////////////////////////////////////////////////////////
// Static Member Initialization
///////////////////////////////////////////////////////////////////////////////
template <typename T> const TVector2<T> TVector2<T>::Zero (0, 0);
template <typename T> const TVector2<T> TVector2<T>::One  (1, 1);
template <typename T> const TVector2<T> TVector2<T>::UnitX(1, 0);
template <typename T> const TVector2<T> TVector2<T>::UnitY(0, 1);

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

} // !namespace tkd
