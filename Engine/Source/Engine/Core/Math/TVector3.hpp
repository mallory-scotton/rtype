///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <cassert>
#include <Engine/Config.hpp>
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
/// \brief A 3D vector template class for arithmetic types.
///
/// \tparam T The arithmetic type for the vector components.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
class TVector3
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
            T x;   //!< The x-component of the vector.
            T y;   //!< The y-component of the vector.
            T z;   //!< The z-component of the vector.
        };
#ifdef __GNUC__
    #pragma GCC diagnostic pop
#elif defined(_MSC_VER)
    #pragma warning(pop)
#endif
        T data[3];   //!< Array access to the components.
    };

public:
    ///////////////////////////////////////////////////////////////////////////
    // Static Member
    ///////////////////////////////////////////////////////////////////////////
    static const TVector3 Zero;   //!< A vector with all components set to 0.
    static const TVector3 One;    //!< A vector with all components set to 1.
    static const TVector3
        UnitX;   //!< A unit vector along the x-axis (1, 0, 0).
    static const TVector3
        UnitY;   //!< A unit vector along the y-axis (0, 1, 0).
    static const TVector3
        UnitZ;   //!< A unit vector along the z-axis (0, 0, 1).

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor. Initializes the vector to (0, 0, 0).
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector3(void)
        : x(0)
        , y(0)
        , z(0)
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructs a vector with the specified x, y, and z components.
    ///
    /// \param x The x-component.
    /// \param y The y-component.
    /// \param z The z-component.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector3(const T& x, const T& y, const T& z)
        : x(x)
        , y(y)
        , z(z)
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Copy constructor. Creates a copy of another vector.
    ///
    /// \param other The vector to copy from.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector3(const TVector3& other)
        : x(other.x)
        , y(other.y)
        , z(other.z)
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Move constructor. Moves the contents of another vector.
    ///
    /// \param other The vector to move from.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector3(TVector3&& other) noexcept
        : x(std::move(other.x))
        , y(std::move(other.y))
        , z(std::move(other.z))
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructs a vector from components of a different type,
    /// casting to T.
    ///
    /// \tparam U The type of the components.
    ///
    /// \param x The x-component.
    /// \param y The y-component.
    /// \param y The z-component.
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename U>
    TVector3(const U& x, const U& y, const U& z)
        : x(static_cast<T>(x))
        , y(static_cast<T>(y))
        , z(static_cast<T>(z))
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructs a vector from another TVector3 of a different type,
    /// casting to T.
    ///
    /// \tparam U The type of the other vector's components.
    ///
    /// \param other The vector to copy from.
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename U>
    TVector3(const TVector3<U>& other)
        : x(static_cast<T>(other.x))
        , y(static_cast<T>(other.y))
        , z(static_cast<T>(other.z))
    {}

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Copy assignment operator.
    ///
    /// \param other The vector to assign from.
    /// \return Reference to this vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector3& operator=(const TVector3& other)
    {
        if (this != &other)
        {
            x = other.x;
            y = other.y;
            z = other.z;
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
    TVector3& operator=(TVector3&& other) noexcept
    {
        if (this != &other)
        {
            x = std::move(other.x);
            y = std::move(other.y);
            z = std::move(other.z);
        }
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Accesses the component at the specified index.
    ///
    /// \param index The index (0 for x, 1 for y, 2 for z).
    /// \return Reference to the component.
    ///
    ///////////////////////////////////////////////////////////////////////////
    T& operator[](SizeT index)
    {
        assert(index < 3);
        return data[index];
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Accesses the component at the specified index (const version).
    ///
    /// \param index The index (0 for x, 1 for y, 2 for z).
    /// \return Const reference to the component.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const T& operator[](SizeT index) const
    {
        assert(index < 3);
        return data[index];
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Pre-increment operator. Increments all components by 1.
    ///
    /// \return Reference to this vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector3& operator++(void)
    {
        ++x;
        ++y;
        ++z;
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Post-increment operator. Increments all components by 1.
    ///
    /// \return A copy of the vector before incrementing.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector3 operator++(int)
    {
        TVector3 temp(*this);
        ++(*this);
        return temp;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Pre-decrement operator. Decrements all components by 1.
    ///
    /// \return Reference to this vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector3& operator--(void)
    {
        --x;
        --y;
        --z;
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Post-decrement operator. Decrements all components by 1.
    ///
    /// \return A copy of the vector before decrementing.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector3 operator--(int)
    {
        TVector3 temp(*this);
        --(*this);
        return temp;
    }
};

///////////////////////////////////////////////////////////////////////////////
// Static Member Initialization
///////////////////////////////////////////////////////////////////////////////
template <typename T>
const TVector3<T> TVector3<T>::Zero(0, 0, 0);
template <typename T>
const TVector3<T> TVector3<T>::One(1, 1, 1);
template <typename T>
const TVector3<T> TVector3<T>::UnitX(1, 0, 0);
template <typename T>
const TVector3<T> TVector3<T>::UnitY(0, 1, 0);
template <typename T>
const TVector3<T> TVector3<T>::UnitZ(0, 0, 1);

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
TVector3<T> operator+(const TVector3<T>& lhs)
{
    return TVector3<T>(+lhs.x, +lhs.y, +lhs.z);
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
TVector3<T> operator+(const TVector3<T>& lhs, const TVector3<T>& rhs)
{
    return TVector3<T>(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
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
TVector3<T> operator+(const TVector3<T>& lhs, const T& rhs)
{
    return TVector3<T>(lhs.x + rhs, lhs.y + rhs, lhs.z + rhs);
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
TVector3<T> operator+(const T& lhs, const TVector3<T>& rhs)
{
    return TVector3<T>(lhs + rhs.x, lhs + rhs.y, lhs + rhs.z);
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
TVector3<T> operator+=(TVector3<T>& lhs, const TVector3<T>& rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    lhs.z += rhs.z;
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
TVector3<T> operator+=(TVector3<T>& lhs, const T& rhs)
{
    lhs.x += rhs;
    lhs.y += rhs;
    lhs.z += rhs;
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
TVector3<T> operator-(const TVector3<T>& lhs)
{
    return TVector3<T>(-lhs.x, -lhs.y, -lhs.z);
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
TVector3<T> operator-(const TVector3<T>& lhs, const TVector3<T>& rhs)
{
    return TVector3<T>(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
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
TVector3<T> operator-(const TVector3<T>& lhs, const T& rhs)
{
    return TVector3<T>(lhs.x - rhs, lhs.y - rhs, lhs.z - rhs);
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
TVector3<T> operator-(const T& lhs, const TVector3<T>& rhs)
{
    return TVector3<T>(lhs - rhs.x, lhs - rhs.y, lhs - rhs.z);
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
TVector3<T> operator-=(TVector3<T>& lhs, const TVector3<T>& rhs)
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    lhs.z -= rhs.z;
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
TVector3<T> operator-=(TVector3<T>& lhs, const T& rhs)
{
    lhs.x -= rhs;
    lhs.y -= rhs;
    lhs.z -= rhs;
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
TVector3<T> operator*(const TVector3<T>& lhs, const TVector3<T>& rhs)
{
    return TVector3<T>(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
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
TVector3<T> operator*(const TVector3<T>& lhs, const T& rhs)
{
    return TVector3<T>(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
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
TVector3<T> operator*(const T& lhs, const TVector3<T>& rhs)
{
    return TVector3<T>(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z);
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
TVector3<T> operator*=(TVector3<T>& lhs, const TVector3<T>& rhs)
{
    lhs.x *= rhs.x;
    lhs.y *= rhs.y;
    lhs.z *= rhs.z;
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
TVector3<T> operator*=(TVector3<T>& lhs, const T& rhs)
{
    lhs.x *= rhs;
    lhs.y *= rhs;
    lhs.z *= rhs;
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
TVector3<T> operator/(const TVector3<T>& lhs, const TVector3<T>& rhs)
{
    assert(rhs.x != T(0) && rhs.y != T(0) && rhs.z != T(0));
    return TVector3<T>(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z);
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
TVector3<T> operator/(const TVector3<T>& lhs, const T& rhs)
{
    assert(rhs != T(0));
    return TVector3<T>(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs);
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
TVector3<T> operator/(const T& lhs, const TVector3<T>& rhs)
{
    assert(rhs.x != T(0) && rhs.y != T(0) && rhs.z != T(0));
    return TVector3<T>(lhs / rhs.x, lhs / rhs.y, lhs / rhs.z);
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
TVector3<T> operator/=(TVector3<T>& lhs, const TVector3<T>& rhs)
{
    assert(rhs.x != T(0) && rhs.y != T(0) && rhs.z != T(0));
    lhs.x /= rhs.x;
    lhs.y /= rhs.y;
    lhs.z /= rhs.z;
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
TVector3<T> operator/=(TVector3<T>& lhs, const T& rhs)
{
    assert(rhs != T(0));
    lhs.x /= rhs;
    lhs.y /= rhs;
    lhs.z /= rhs;
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
/// \return True if all components are equal.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
bool operator==(const TVector3<T>& lhs, const TVector3<T>& rhs)
{
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Checks if a vector is equal to a scalar in all components.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The vector.
/// \param rhs The scalar.
///
/// \return True if all components equal the scalar.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
bool operator==(const TVector3<T>& lhs, const T& rhs)
{
    return lhs.x == rhs && lhs.y == rhs && lhs.z == rhs;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Checks if a scalar is equal to all components of a vector.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The scalar.
/// \param rhs The vector.
///
/// \return True if the scalar equals all components.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
bool operator==(const T& lhs, const TVector3<T>& rhs)
{
    return lhs == rhs.x && lhs == rhs.y && lhs == rhs.z;
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
bool operator!=(const TVector3<T>& lhs, const TVector3<T>& rhs)
{
    return !(lhs == rhs);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Checks if a vector is not equal to a scalar in all components.
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
bool operator!=(const TVector3<T>& lhs, const T& rhs)
{
    return !(lhs == rhs);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Checks if a scalar is not equal to all components of a vector.
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
bool operator!=(const T& lhs, const TVector3<T>& rhs)
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
/// \return True if all components of lhs are >= corresponding in rhs.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
bool operator>=(const TVector3<T>& lhs, const TVector3<T>& rhs)
{
    return lhs.x >= rhs.x && lhs.y >= rhs.y && lhs.z >= rhs.z;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Checks if the vector's components are >= a scalar.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The vector.
/// \param rhs The scalar.
///
/// \return True if all components >= scalar.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
bool operator>=(const TVector3<T>& lhs, const T& rhs)
{
    return lhs.x >= rhs && lhs.y >= rhs && lhs.z >= rhs;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Checks if a scalar is >= all components of the vector.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The scalar.
/// \param rhs The vector.
///
/// \return True if scalar >= all components.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
bool operator>=(const T& lhs, const TVector3<T>& rhs)
{
    return lhs >= rhs.x && lhs >= rhs.y && lhs >= rhs.z;
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
/// \return True if all components of lhs > corresponding in rhs.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
bool operator>(const TVector3<T>& lhs, const TVector3<T>& rhs)
{
    return lhs.x > rhs.x && lhs.y > rhs.y && lhs.z > rhs.z;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Checks if the vector's components are > a scalar.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The vector.
/// \param rhs The scalar.
///
/// \return True if all components > scalar.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
bool operator>(const TVector3<T>& lhs, const T& rhs)
{
    return lhs.x > rhs && lhs.y > rhs && lhs.z > rhs;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Checks if a scalar is > all components of the vector.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The scalar.
/// \param rhs The vector.
///
/// \return True if scalar > all components.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
bool operator>(const T& lhs, const TVector3<T>& rhs)
{
    return lhs > rhs.x && lhs > rhs.y && lhs > rhs.z;
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
/// \return True if all components of lhs <= corresponding in rhs.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
bool operator<=(const TVector3<T>& lhs, const TVector3<T>& rhs)
{
    return lhs.x <= rhs.x && lhs.y <= rhs.y && lhs.z <= rhs.z;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Checks if the vector's components are <= a scalar.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The vector.
/// \param rhs The scalar.
///
/// \return True if all components <= scalar.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
bool operator<=(const TVector3<T>& lhs, const T& rhs)
{
    return lhs.x <= rhs && lhs.y <= rhs && lhs.z <= rhs;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Checks if a scalar is <= all components of the vector.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The scalar.
/// \param rhs The vector.
///
/// \return True if scalar <= all components.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
bool operator<=(const T& lhs, const TVector3<T>& rhs)
{
    return lhs <= rhs.x && lhs <= rhs.y && lhs <= rhs.z;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Checks if the first vector is less than the second component-wise.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The left-hand side vector.
/// \param rhs The right-hand side vector.
///
/// \return True if all components of lhs < corresponding in rhs.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
bool operator<(const TVector3<T>& lhs, const TVector3<T>& rhs)
{
    return lhs.x < rhs.x && lhs.y < rhs.y && lhs.z < rhs.z;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Checks if the vector's components are < a scalar.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The vector.
/// \param rhs The scalar.
///
/// \return True if all components < scalar.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
bool operator<(const TVector3<T>& lhs, const T& rhs)
{
    return lhs.x < rhs && lhs.y < rhs && lhs.z < rhs;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Checks if a scalar is < all components of the vector.
///
/// \tparam T The type of the vector components.
///
/// \param lhs The scalar.
/// \param rhs The vector.
///
/// \return True if scalar < all components.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
bool operator<(const T& lhs, const TVector3<T>& rhs)
{
    return lhs < rhs.x && lhs < rhs.y && lhs < rhs.z;
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
std::ostream& operator<<(std::ostream& os, const TVector3<T>& vec)
{
    os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    return os;
}

}   // namespace tkd
