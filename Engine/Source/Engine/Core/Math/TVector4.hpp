///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

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
class TVector4
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
            T w;   //!< The w-component of the vector.
        };
#ifdef __GNUC__
    #pragma GCC diagnostic pop
#elif defined(_MSC_VER)
    #pragma warning(pop)
#endif
        T data[4];   //!< Array access to the components.
    };

public:
    ///////////////////////////////////////////////////////////////////////////
    // Static Member
    ///////////////////////////////////////////////////////////////////////////
    static const TVector4 Zero;   //!< A vector with all components set to 0.
    static const TVector4 One;    //!< A vector with all components set to 1.
    static const TVector4
        UnitX;   //!< A unit vector along the x-axis (1, 0, 0, 0).
    static const TVector4
        UnitY;   //!< A unit vector along the y-axis (0, 1, 0, 0).
    static const TVector4
        UnitZ;   //!< A unit vector along the z-axis (0, 0, 1, 0).
    static const TVector4
        UnitW;   //!< A unit vector along the w-axis (0, 0, 0, 1).

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor. Initializes the vector to (0, 0, 0, 0).
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector4(void)
        : x(0)
        , y(0)
        , z(0)
        , w(0)
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructs a vector with the specified x, y, z, and w
    /// components.
    ///
    /// \param x The x-component.
    /// \param y The y-component.
    /// \param z The z-component.
    /// \param w The w-component.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector4(const T& x, const T& y, const T& z, const T& w)
        : x(x)
        , y(y)
        , z(z)
        , w(w)
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Copy constructor. Creates a copy of another vector.
    ///
    /// \param other The vector to copy from.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector4(const TVector4& other)
        : x(other.x)
        , y(other.y)
        , z(other.z)
        , w(other.w)
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Move constructor. Moves the contents of another vector.
    ///
    /// \param other The vector to move from.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector4(TVector4&& other) noexcept
        : x(std::move(other.x))
        , y(std::move(other.y))
        , z(std::move(other.z))
        , w(std::move(other.w))
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructs a vector from components of a different type,
    /// casting to T.
    ///
    /// \tparam U The type of the components.
    ///
    /// \param x The x-component.
    /// \param y The y-component.
    /// \param z The z-component.
    /// \param w The w-component.
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename U>
    TVector4(const U& x, const U& y, const U& z, const U& w)
        : x(static_cast<T>(x))
        , y(static_cast<T>(y))
        , z(static_cast<T>(z))
        , w(static_cast<T>(w))
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructs a vector from another TVector4 of a different type,
    /// casting to T.
    ///
    /// \tparam U The type of the other vector's components.
    ///
    /// \param other The vector to copy from.
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename U>
    TVector4(const TVector4<U>& other)
        : x(static_cast<T>(other.x))
        , y(static_cast<T>(other.y))
        , z(static_cast<T>(other.z))
        , w(static_cast<T>(other.w))
    {}

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Copy assignment operator.
    ///
    /// \param other The vector to assign from.
    /// \return Reference to this vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector4& operator=(const TVector4& other)
    {
        if (this != &other)
        {
            x = other.x;
            y = other.y;
            z = other.z;
            w = other.w;
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
    TVector4& operator=(TVector4&& other) noexcept
    {
        if (this != &other)
        {
            x = std::move(other.x);
            y = std::move(other.y);
            z = std::move(other.z);
            w = std::move(other.w);
        }
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Accesses the component at the specified index.
    ///
    /// \param index The index (0 for x, 1 for y, 2 for z, 3 for w).
    /// \return Reference to the component.
    ///
    ///////////////////////////////////////////////////////////////////////////
    T& operator[](SizeT index)
    {
        assert(index < 4);
        return data[index];
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Accesses the component at the specified index (const version).
    ///
    /// \param index The index (0 for x, 1 for y, 2 for z, 3 for w).
    /// \return Const reference to the component.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const T& operator[](SizeT index) const
    {
        assert(index < 4);
        return data[index];
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Pre-increment operator. Increments all components by 1.
    ///
    /// \return Reference to this vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector4& operator++(void)
    {
        ++x;
        ++y;
        ++z;
        ++w;
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Post-increment operator. Increments all components by 1.
    ///
    /// \return A copy of the vector before incrementing.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector4 operator++(int)
    {
        TVector4 temp(*this);
        ++(*this);
        return temp;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Pre-decrement operator. Decrements all components by 1.
    ///
    /// \return Reference to this vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector4& operator--(void)
    {
        --x;
        --y;
        --z;
        --w;
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Post-decrement operator. Decrements all components by 1.
    ///
    /// \return A copy of the vector before decrementing.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector4 operator--(int)
    {
        TVector4 temp(*this);
        --(*this);
        return temp;
    }
};

///////////////////////////////////////////////////////////////////////////////
// Static Member Initialization
///////////////////////////////////////////////////////////////////////////////
template <typename T>
const TVector4<T> TVector4<T>::Zero(0, 0, 0, 0);
template <typename T>
const TVector4<T> TVector4<T>::One(1, 1, 1, 1);
template <typename T>
const TVector4<T> TVector4<T>::UnitX(1, 0, 0, 0);
template <typename T>
const TVector4<T> TVector4<T>::UnitY(0, 1, 0, 0);
template <typename T>
const TVector4<T> TVector4<T>::UnitZ(0, 0, 1, 0);
template <typename T>
const TVector4<T> TVector4<T>::UnitW(0, 0, 0, 1);

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
TVector4<T> operator+(const TVector4<T>& lhs)
{
    return TVector4<T>(+lhs.x, +lhs.y, +lhs.z, +lhs.w);
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
TVector4<T> operator+(const TVector4<T>& lhs, const TVector4<T>& rhs)
{
    return TVector4<T>(
        lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w
    );
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
TVector4<T> operator+(const TVector4<T>& lhs, const T& rhs)
{
    return TVector4<T>(lhs.x + rhs, lhs.y + rhs, lhs.z + rhs, lhs.w + rhs);
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
TVector4<T> operator+(const T& lhs, const TVector4<T>& rhs)
{
    return TVector4<T>(lhs + rhs.x, lhs + rhs.y, lhs + rhs.z, lhs + rhs.w);
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
TVector4<T> operator+=(TVector4<T>& lhs, const TVector4<T>& rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    lhs.z += rhs.z;
    lhs.w += rhs.w;
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
TVector4<T> operator+=(TVector4<T>& lhs, const T& rhs)
{
    lhs.x += rhs;
    lhs.y += rhs;
    lhs.z += rhs;
    lhs.w += rhs;
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
TVector4<T> operator-(const TVector4<T>& lhs)
{
    return TVector4<T>(-lhs.x, -lhs.y, -lhs.z, -lhs.w);
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
TVector4<T> operator-(const TVector4<T>& lhs, const TVector4<T>& rhs)
{
    return TVector4<T>(
        lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w
    );
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
TVector4<T> operator-(const TVector4<T>& lhs, const T& rhs)
{
    return TVector4<T>(lhs.x - rhs, lhs.y - rhs, lhs.z - rhs, lhs.w - rhs);
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
TVector4<T> operator-(const T& lhs, const TVector4<T>& rhs)
{
    return TVector4<T>(lhs - rhs.x, lhs - rhs.y, lhs - rhs.z, lhs - rhs.w);
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
TVector4<T> operator-=(TVector4<T>& lhs, const TVector4<T>& rhs)
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    lhs.z -= rhs.z;
    lhs.w -= rhs.w;
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
TVector4<T> operator-=(TVector4<T>& lhs, const T& rhs)
{
    lhs.x -= rhs;
    lhs.y -= rhs;
    lhs.z -= rhs;
    lhs.w -= rhs;
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
TVector4<T> operator*(const TVector4<T>& lhs, const TVector4<T>& rhs)
{
    return TVector4<T>(
        lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w
    );
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
TVector4<T> operator*(const TVector4<T>& lhs, const T& rhs)
{
    return TVector4<T>(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs);
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
TVector4<T> operator*(const T& lhs, const TVector4<T>& rhs)
{
    return TVector4<T>(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z, lhs * rhs.w);
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
TVector4<T> operator*=(TVector4<T>& lhs, const TVector4<T>& rhs)
{
    lhs.x *= rhs.x;
    lhs.y *= rhs.y;
    lhs.z *= rhs.z;
    lhs.w *= rhs.w;
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
TVector4<T> operator*=(TVector4<T>& lhs, const T& rhs)
{
    lhs.x *= rhs;
    lhs.y *= rhs;
    lhs.z *= rhs;
    lhs.w *= rhs;
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
TVector4<T> operator/(const TVector4<T>& lhs, const TVector4<T>& rhs)
{
    assert(rhs.x != T(0) && rhs.y != T(0) && rhs.z != T(0) && rhs.w != T(0));
    return TVector4<T>(
        lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w
    );
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
TVector4<T> operator/(const TVector4<T>& lhs, const T& rhs)
{
    assert(rhs != T(0));
    return TVector4<T>(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs, lhs.w / rhs);
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
TVector4<T> operator/(const T& lhs, const TVector4<T>& rhs)
{
    assert(rhs.x != T(0) && rhs.y != T(0) && rhs.z != T(0) && rhs.w != T(0));
    return TVector4<T>(lhs / rhs.x, lhs / rhs.y, lhs / rhs.z, lhs / rhs.w);
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
TVector4<T> operator/=(TVector4<T>& lhs, const TVector4<T>& rhs)
{
    assert(rhs.x != T(0) && rhs.y != T(0) && rhs.z != T(0) && rhs.w != T(0));
    lhs.x /= rhs.x;
    lhs.y /= rhs.y;
    lhs.z /= rhs.z;
    lhs.w /= rhs.w;
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
TVector4<T> operator/=(TVector4<T>& lhs, const T& rhs)
{
    assert(rhs != T(0));
    lhs.x /= rhs;
    lhs.y /= rhs;
    lhs.z /= rhs;
    lhs.w /= rhs;
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
bool operator==(const TVector4<T>& lhs, const TVector4<T>& rhs)
{
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z &&
           lhs.w == rhs.w;
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
bool operator==(const TVector4<T>& lhs, const T& rhs)
{
    return lhs.x == rhs && lhs.y == rhs && lhs.z == rhs && lhs.w == rhs;
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
bool operator==(const T& lhs, const TVector4<T>& rhs)
{
    return lhs == rhs.x && lhs == rhs.y && lhs == rhs.z && lhs == rhs.w;
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
bool operator!=(const TVector4<T>& lhs, const TVector4<T>& rhs)
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
bool operator!=(const TVector4<T>& lhs, const T& rhs)
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
bool operator!=(const T& lhs, const TVector4<T>& rhs)
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
bool operator>=(const TVector4<T>& lhs, const TVector4<T>& rhs)
{
    return lhs.x >= rhs.x && lhs.y >= rhs.y && lhs.z >= rhs.z &&
           lhs.w >= rhs.w;
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
bool operator>=(const TVector4<T>& lhs, const T& rhs)
{
    return lhs.x >= rhs && lhs.y >= rhs && lhs.z >= rhs && lhs.w >= rhs;
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
bool operator>=(const T& lhs, const TVector4<T>& rhs)
{
    return lhs >= rhs.x && lhs >= rhs.y && lhs >= rhs.z && lhs >= rhs.w;
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
bool operator>(const TVector4<T>& lhs, const TVector4<T>& rhs)
{
    return lhs.x > rhs.x && lhs.y > rhs.y && lhs.z > rhs.z && lhs.w > rhs.w;
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
bool operator>(const TVector4<T>& lhs, const T& rhs)
{
    return lhs.x > rhs && lhs.y > rhs && lhs.z > rhs && lhs.w > rhs;
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
bool operator>(const T& lhs, const TVector4<T>& rhs)
{
    return lhs > rhs.x && lhs > rhs.y && lhs > rhs.z && lhs > rhs.w;
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
bool operator<=(const TVector4<T>& lhs, const TVector4<T>& rhs)
{
    return lhs.x <= rhs.x && lhs.y <= rhs.y && lhs.z <= rhs.z &&
           lhs.w <= rhs.w;
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
bool operator<=(const TVector4<T>& lhs, const T& rhs)
{
    return lhs.x <= rhs && lhs.y <= rhs && lhs.z <= rhs && lhs.w <= rhs;
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
bool operator<=(const T& lhs, const TVector4<T>& rhs)
{
    return lhs <= rhs.x && lhs <= rhs.y && lhs <= rhs.z && lhs <= rhs.w;
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
bool operator<(const TVector4<T>& lhs, const TVector4<T>& rhs)
{
    return lhs.x < rhs.x && lhs.y < rhs.y && lhs.z < rhs.z && lhs.w < rhs.w;
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
bool operator<(const TVector4<T>& lhs, const T& rhs)
{
    return lhs.x < rhs && lhs.y < rhs && lhs.z < rhs && lhs.w < rhs;
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
bool operator<(const T& lhs, const TVector4<T>& rhs)
{
    return lhs < rhs.x && lhs < rhs.y && lhs < rhs.z && lhs < rhs.w;
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
std::ostream& operator<<(std::ostream& os, const TVector4<T>& vec)
{
    os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w
       << ")";
    return os;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Adds two 4D vectors component-wise.
///
/// \tparam T The type of the vector components.
/// \param a The first vector.
/// \param b The second vector.
/// \return The resulting vector after addition.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector4<T> Add(const TVector4<T>& a, const TVector4<T>& b)
{
    return TVector4<T>(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Subtracts one 4D vector from another component-wise.
///
/// \tparam T The type of the vector components.
/// \param a The first vector.
/// \param b The second vector.
/// \return The resulting vector after subtraction.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector4<T> Subtract(const TVector4<T>& a, const TVector4<T>& b)
{
    return TVector4<T>(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Multiplies two 4D vectors component-wise.
///
/// \tparam T The type of the vector components.
/// \param a The first vector.
/// \param b The second vector.
/// \return The resulting vector after multiplication.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector4<T> Multiply(const TVector4<T>& a, const TVector4<T>& b)
{
    return TVector4<T>(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Divides one 4D vector by another component-wise.
///
/// \tparam T The type of the vector components.
/// \param a The numerator vector.
/// \param b The denominator vector (must not contain zeros).
/// \return The resulting vector after division.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector4<T> Divide(const TVector4<T>& a, const TVector4<T>& b)
{
    assert(b.x != T(0) && b.y != T(0) && b.z != T(0) && b.w != T(0));
    return TVector4<T>(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Performs a multiply-add operation on 4D vectors: a * b + c.
///
/// \tparam T The type of the vector components.
/// \param a The first vector (multiplicand).
/// \param b The second vector (multiplier).
/// \param c The vector to be added.
/// \return The resulting vector after the multiply-add operation.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector4<T> MultiplyAdd(
    const TVector4<T>& a, const TVector4<T>& b, const TVector4<T>& c
)
{
    return TVector4<T>(
        a.x * b.x + c.x, a.y * b.y + c.y, a.z * b.z + c.z, a.w * b.w + c.w
    );
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Computes the Euclidean distance between two 4D vectors.
///
/// \tparam T The type of the vector components.
/// \param a The first vector.
/// \param b The second vector.
/// \return The distance between the two vectors.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
T Distance(const TVector4<T>& a, const TVector4<T>& b)
{
    T dx = a.x - b.x;
    T dy = a.y - b.y;
    T dz = a.z - b.z;
    T dw = a.w - b.w;
    return std::sqrt(dx * dx + dy * dy + dz * dz + dw * dw);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Computes the length (magnitude) of a 4D vector.
///
/// \tparam T The type of the vector components.
/// \param v The input vector.
/// \return The length of the vector.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
T Length(const TVector4<T>& v)
{
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Returns the component-wise absolute value of a 4D vector.
///
/// \tparam T The type of the vector components.
/// \param v The input vector.
/// \return The vector with absolute values of each component.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector4<T> Absolute(const TVector4<T>& v)
{
    return TVector4<T>(
        std::abs(v.x), std::abs(v.y), std::abs(v.z), std::abs(v.w)
    );
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Normalizes a 4D vector to unit length.
///
/// \tparam T The type of the vector components.
/// \param v The input vector (must not be the zero vector).
/// \return The normalized vector.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector4<T> Normalize(const TVector4<T>& v)
{
    T len = Length(v);
    assert(len != T(0));
    return TVector4<T>(v.x / len, v.y / len, v.z / len, v.w / len);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Computes the component-wise minimum of two 4D vectors.
///
/// \tparam T The type of the vector components.
/// \param a The first vector.
/// \param b The second vector.
/// \return A vector containing the minimum of each component.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector4<T> Minimum(const TVector4<T>& a, const TVector4<T>& b)
{
    return TVector4<T>(
        std::min(a.x, b.x),
        std::min(a.y, b.y),
        std::min(a.z, b.z),
        std::min(a.w, b.w)
    );
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Computes the component-wise maximum of two 4D vectors.
///
/// \tparam T The type of the vector components.
/// \param a The first vector.
/// \param b The second vector.
/// \return A vector containing the maximum of each component.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector4<T> Maximum(const TVector4<T>& a, const TVector4<T>& b)
{
    return TVector4<T>(
        std::max(a.x, b.x),
        std::max(a.y, b.y),
        std::max(a.z, b.z),
        std::max(a.w, b.w)
    );
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Applies floor operation component-wise to a 4D vector.
///
/// \tparam T The type of the vector components.
/// \param v The input vector.
/// \return The floored vector.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector4<T> Floor(const TVector4<T>& v)
{
    return TVector4<T>(
        std::floor(v.x), std::floor(v.y), std::floor(v.z), std::floor(v.w)
    );
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Applies ceil operation component-wise to a 4D vector.
///
/// \tparam T The type of the vector components.
/// \param v The input vector.
/// \return The ceiled vector.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector4<T> Ceil(const TVector4<T>& v)
{
    return TVector4<T>(
        std::ceil(v.x), std::ceil(v.y), std::ceil(v.z), std::ceil(v.w)
    );
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Computes the fractional part of each component of a 4D vector.
///
/// \tparam T The type of the vector components.
/// \param v The input vector.
/// \return The fractional part of the vector components.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector4<T> Fraction(const TVector4<T>& v)
{
    return TVector4<T>(
        v.x - std::floor(v.x),
        v.y - std::floor(v.y),
        v.z - std::floor(v.z),
        v.w - std::floor(v.w)
    );
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Applies the modulo operation to each component of a 4D vector.
///
/// \tparam T The type of the vector components.
/// \param v The input vector.
/// \param m The modulo value.
/// \return A vector where each component is v mod m.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector4<T> Modulo(const TVector4<T>& v, const T& m)
{
    return TVector4<T>(
        std::fmod(v.x, m),
        std::fmod(v.y, m),
        std::fmod(v.z, m),
        std::fmod(v.w, m)
    );
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Wraps each component of the 4D vector within a specified range [min,
/// max).
///
/// \tparam T The type of the vector components.
/// \param v The input vector.
/// \param min The minimum value of the range.
/// \param max The maximum value of the range.
/// \return A vector with components wrapped into the [min, max) interval.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector4<T> Wrap(const TVector4<T>& v, const T& min, const T& max)
{
    auto wrap = [](T val, T min, T max)
    {
        T range = max - min;
        return min + std::fmod(std::fmod(val - min, range) + range, range);
    };
    return TVector4<T>(
        wrap(v.x, min, max),
        wrap(v.y, min, max),
        wrap(v.z, min, max),
        wrap(v.w, min, max)
    );
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Snaps each component of the 4D vector to a multiple of the given
/// step.
///
/// \tparam T The type of the vector components.
/// \param v The input vector.
/// \param step The snapping step.
/// \return A vector with each component rounded to the nearest multiple of
/// step.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector4<T> Snap(const TVector4<T>& v, const T& step)
{
    auto snap = [](T val, T step) { return std::round(val / step) * step; };
    return TVector4<T>(
        snap(v.x, step), snap(v.y, step), snap(v.z, step), snap(v.w, step)
    );
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Applies the sine function to each component of the 4D vector.
///
/// \tparam T The type of the vector components.
/// \param v The input vector.
/// \return A vector where each component is the sine of the original
/// component.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector4<T> Sine(const TVector4<T>& v)
{
    return TVector4<T>(
        std::sin(v.x), std::sin(v.y), std::sin(v.z), std::sin(v.w)
    );
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Applies the cosine function to each component of the 4D vector.
///
/// \tparam T The type of the vector components.
/// \param v The input vector.
/// \return A vector where each component is the cosine of the original
/// component.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector4<T> Cosine(const TVector4<T>& v)
{
    return TVector4<T>(
        std::cos(v.x), std::cos(v.y), std::cos(v.z), std::cos(v.w)
    );
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Applies the tangent function to each component of the 4D vector.
///
/// \tparam T The type of the vector components.
/// \param v The input vector.
/// \return A vector where each component is the tangent of the original
/// component.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector4<T> Tangent(const TVector4<T>& v)
{
    return TVector4<T>(
        std::tan(v.x), std::tan(v.y), std::tan(v.z), std::tan(v.w)
    );
}

}   // namespace tkd
