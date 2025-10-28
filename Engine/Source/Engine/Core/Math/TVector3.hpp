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
            T x;   //<! The x-component of the vector.
            T y;   //<! The y-component of the vector.
            T z;   //<! The z-component of the vector.
        };
#ifdef __GNUC__
    #pragma GCC diagnostic pop
#elif defined(_MSC_VER)
    #pragma warning(pop)
#endif
        T data[3];   //<! Array access to the components.
    };

public:
    ///////////////////////////////////////////////////////////////////////////
    // Static Member
    ///////////////////////////////////////////////////////////////////////////
    static const TVector3 Zero;    //<! A vector with all components set to 0.
    static const TVector3 One;     //<! A vector with all components set to 1.
    static const TVector3 UnitX;   //<! A unit vector along the x-axis
    static const TVector3 UnitY;   //<! A unit vector along the y-axis
    static const TVector3 UnitZ;   //<! A unit vector along the z-axis
    static const TVector3 Up;      //<! A unit vector pointing up
    static const TVector3 Down;    //<! A unit vector pointing down
    static const TVector3 Right;   //<! A unit vector pointing right
    static const TVector3 Left;    //<! A unit vector pointing left
    static const TVector3 Forward;   //<! A unit vector pointing forward
    static const TVector3 Back;      //<! A unit vector pointing back

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
    /// \brief Constructs a vector with all components set to the same value.
    ///
    /// \param all The value to set all components to.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector3(const T& all)
        : x(all)
        , y(all)
        , z(all)
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

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Computes the length (magnitude) of the vector.
    ///
    /// \return The length of the vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    T Length(void) const
    {
        return static_cast<T>(std::sqrt(x * x + y * y + z * z));
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    T LengthSquared(void) const { return x * x + y * y + z * z; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a normalized copy of the vector. If the vector is zero,
    ///
    /// \return The normalized vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector3<T> Normalized(void) const { return Normalize(*this); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Computes the dot product with another vector.
    ///
    /// \param other The other vector.
    ///
    /// \return The dot product.
    ///
    ///////////////////////////////////////////////////////////////////////////
    T Dot(const TVector3<T>& other) const
    {
        return x * other.x + y * other.y + z * other.z;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Computes the cross product with another vector.
    ///
    /// \param other The other vector.
    ///
    /// \return The cross product vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector3<T> Cross(const TVector3<T>& other) const
    {
        return TVector3<T>(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Computes the distance to another vector.
    ///
    /// \param other The other vector.
    ///
    /// \return The distance.
    ///
    ///////////////////////////////////////////////////////////////////////////
    T Distance(const TVector3<T>& other) const
    {
        T dx = x - other.x;
        T dy = y - other.y;
        T dz = z - other.z;
        return static_cast<T>(std::sqrt(dx * dx + dy * dy + dz * dz));
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Computes the squared distance to another vector (faster).
    ///
    /// \param other The other vector.
    ///
    /// \return The squared distance.
    ///
    ///////////////////////////////////////////////////////////////////////////
    T DistanceSquared(const TVector3<T>& other) const
    {
        T dx = x - other.x;
        T dy = y - other.y;
        T dz = z - other.z;
        return dx * dx + dy * dy + dz * dz;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Projects this vector onto another vector.
    ///
    /// \param other The vector to project onto.
    ///
    /// \return The projected vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector3<T> Project(const TVector3<T>& other) const
    {
        T dot = x * other.x + y * other.y + z * other.z;
        T lenSq = other.x * other.x + other.y * other.y + other.z * other.z;

        constexpr T epsilon = std::numeric_limits<T>::epsilon();
        if (std::abs(lenSq) < epsilon) { return TVector3<T>::Zero; }

        T scale = dot / lenSq;
        return other * scale;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Reflects this vector around a normal vector.
    ///
    /// \param normal The normal vector (should be normalized).
    ///
    /// \return The reflected vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector3<T> Reflect(const TVector3<T>& normal) const
    {
        T dot = x * normal.x + y * normal.y + z * normal.z;
        return TVector3<T>(
            x - 2 * dot * normal.x,
            y - 2 * dot * normal.y,
            z - 2 * dot * normal.z
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Computes the angle between this vector and another (in radians).
    ///
    /// \param other The other vector.
    ///
    /// \return The angle in radians.
    ///
    ///////////////////////////////////////////////////////////////////////////
    T Angle(const TVector3<T>& other) const
    {
        T dot = x * other.x + y * other.y + z * other.z;
        T len1 = Length();
        T len2 = other.Length();

        constexpr T epsilon = std::numeric_limits<T>::epsilon();
        if (len1 < epsilon || len2 < epsilon) { return static_cast<T>(0); }

        T cosAngle = dot / (len1 * len2);
        cosAngle = std::max(
            static_cast<T>(-1), std::min(static_cast<T>(1), cosAngle)
        );
        return static_cast<T>(std::acos(cosAngle));
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Checks if this vector is parallel to another.
    ///
    /// \param other The other vector.
    /// \param tolerance The tolerance for comparison.
    ///
    /// \return True if vectors are parallel.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool IsParallel(
        const TVector3<T>& other, T tolerance = static_cast<T>(0.001)
    ) const
    {
        TVector3<T> cross = Cross(other);
        return cross.LengthSquared() < tolerance * tolerance;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Checks if this vector is perpendicular to another.
    ///
    /// \param other The other vector.
    /// \param tolerance The tolerance for comparison.
    ///
    /// \return True if vectors are perpendicular.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool IsPerpendicular(
        const TVector3<T>& other, T tolerance = static_cast<T>(0.001)
    ) const
    {
        T dot = Dot(other);
        return std::abs(dot) < tolerance;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Checks if this vector is normalized (unit length).
    ///
    /// \param tolerance The tolerance for comparison.
    ///
    /// \return True if the vector is normalized.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool IsNormalized(T tolerance = static_cast<T>(0.001)) const
    {
        T lenSq = LengthSquared();
        return std::abs(lenSq - static_cast<T>(1)) < tolerance;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Checks if this vector is zero (or near zero).
    ///
    /// \param tolerance The tolerance for comparison.
    ///
    /// \return True if the vector is zero.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool IsZero(T tolerance = static_cast<T>(0.001)) const
    {
        return std::abs(x) < tolerance && std::abs(y) < tolerance &&
               std::abs(z) < tolerance;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns the component-wise absolute value.
    ///
    /// \return The absolute value vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector3<T> Abs(void) const
    {
        return TVector3<T>(std::abs(x), std::abs(y), std::abs(z));
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns the component-wise minimum with another vector.
    ///
    /// \param other The other vector.
    ///
    /// \return The component-wise minimum.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector3<T> Min(const TVector3<T>& other) const
    {
        return TVector3<T>(
            std::min(x, other.x), std::min(y, other.y), std::min(z, other.z)
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns the component-wise maximum with another vector.
    ///
    /// \param other The other vector.
    ///
    /// \return The component-wise maximum.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector3<T> Max(const TVector3<T>& other) const
    {
        return TVector3<T>(
            std::max(x, other.x), std::max(y, other.y), std::max(z, other.z)
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Clamps each component between min and max values.
    ///
    /// \param min The minimum value.
    /// \param max The maximum value.
    ///
    /// \return The clamped vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector3<T> Clamp(const T& min, const T& max) const
    {
        return TVector3<T>(
            std::max(min, std::min(max, x)),
            std::max(min, std::min(max, y)),
            std::max(min, std::min(max, z))
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Clamps each component between corresponding components of min
    /// and max vectors.
    ///
    /// \param min The minimum vector.
    /// \param max The maximum vector.
    ///
    /// \return The clamped vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector3<T> Clamp(const TVector3<T>& min, const TVector3<T>& max) const
    {
        return TVector3<T>(
            std::max(min.x, std::min(max.x, x)),
            std::max(min.y, std::min(max.y, y)),
            std::max(min.z, std::min(max.z, z))
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Linearly interpolates between this vector and another.
    ///
    /// \param other The target vector.
    /// \param t The interpolation factor (0 to 1).
    ///
    /// \return The interpolated vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector3<T> Lerp(const TVector3<T>& other, T t) const
    {
        return TVector3<T>(
            x + (other.x - x) * t, y + (other.y - y) * t, z + (other.z - z) * t
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Spherically interpolates between this vector and another.
    ///
    /// \param other The target vector.
    /// \param t The interpolation factor (0 to 1).
    ///
    /// \return The interpolated vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector3<T> Slerp(const TVector3<T>& other, T t) const
    {
        T dot = Dot(other);
        T len1 = Length();
        T len2 = other.Length();

        constexpr T epsilon = std::numeric_limits<T>::epsilon();
        if (len1 < epsilon || len2 < epsilon) { return Lerp(other, t); }

        dot = dot / (len1 * len2);
        dot = std::max(static_cast<T>(-1), std::min(static_cast<T>(1), dot));

        T theta = static_cast<T>(std::acos(dot)) * t;
        TVector3<T> relative = (other - *this * dot).Normalized();

        return (*this * static_cast<T>(std::cos(theta))) +
               (relative * static_cast<T>(std::sin(theta)));
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns the largest component value.
    ///
    /// \return The maximum component.
    ///
    ///////////////////////////////////////////////////////////////////////////
    T MaxComponent(void) const { return std::max(x, std::max(y, z)); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns the smallest component value.
    ///
    /// \return The minimum component.
    ///
    ///////////////////////////////////////////////////////////////////////////
    T MinComponent(void) const { return std::min(x, std::min(y, z)); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns the index of the largest component (0=x, 1=y, 2=z).
    ///
    /// \return The index of the maximum component.
    ///
    ///////////////////////////////////////////////////////////////////////////
    SizeT MaxComponentIndex(void) const
    {
        if (x >= y && x >= z) { return 0; }
        if (y >= z) { return 1; }
        return 2;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns the index of the smallest component (0=x, 1=y, 2=z).
    ///
    /// \return The index of the minimum component.
    ///
    ///////////////////////////////////////////////////////////////////////////
    SizeT MinComponentIndex(void) const
    {
        if (x <= y && x <= z) { return 0; }
        if (y <= z) { return 1; }
        return 2;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a vector with each component rounded to nearest integer.
    ///
    /// \return The rounded vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector3<T> Round(void) const
    {
        return TVector3<T>(
            static_cast<T>(std::round(x)),
            static_cast<T>(std::round(y)),
            static_cast<T>(std::round(z))
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a vector with each component rounded down.
    ///
    /// \return The floored vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector3<T> Floor(void) const
    {
        return TVector3<T>(
            static_cast<T>(std::floor(x)),
            static_cast<T>(std::floor(y)),
            static_cast<T>(std::floor(z))
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a vector with each component rounded up.
    ///
    /// \return The ceiled vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector3<T> Ceil(void) const
    {
        return TVector3<T>(
            static_cast<T>(std::ceil(x)),
            static_cast<T>(std::ceil(y)),
            static_cast<T>(std::ceil(z))
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns the sign of each component (-1, 0, or 1).
    ///
    /// \return The sign vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector3<T> Sign(void) const
    {
        return TVector3<T>(
            x > 0 ? static_cast<T>(1)
                  : (x < 0 ? static_cast<T>(-1) : static_cast<T>(0)),
            y > 0 ? static_cast<T>(1)
                  : (y < 0 ? static_cast<T>(-1) : static_cast<T>(0)),
            z > 0 ? static_cast<T>(1)
                  : (z < 0 ? static_cast<T>(-1) : static_cast<T>(0))
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Scales this vector to a target length.
    ///
    /// \param targetLength The desired length.
    ///
    /// \return The scaled vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector3<T> SetLength(T targetLength) const
    {
        T len = Length();
        constexpr T epsilon = std::numeric_limits<T>::epsilon();

        if (len < epsilon) { return TVector3<T>::Zero; }

        return (*this) * (targetLength / len);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Limits the length of this vector to a maximum value.
    ///
    /// \param maxLength The maximum length.
    ///
    /// \return The limited vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector3<T> LimitLength(T maxLength) const
    {
        T lenSq = LengthSquared();
        T maxLenSq = maxLength * maxLength;

        if (lenSq <= maxLenSq) { return *this; }

        return (*this) * (maxLength / std::sqrt(lenSq));
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a vector perpendicular to this one.
    ///
    /// \return A perpendicular vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector3<T> Perpendicular(void) const
    {
        T ax = std::abs(x);
        T ay = std::abs(y);
        T az = std::abs(z);

        if (ax < ay && ax < az) { return TVector3<T>(0, -z, y).Normalized(); }
        else if (ay < az) { return TVector3<T>(-z, 0, x).Normalized(); }
        else { return TVector3<T>(-y, x, 0).Normalized(); }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Rotates this vector around an axis by an angle (in radians).
    ///
    /// \param axis The rotation axis (should be normalized).
    /// \param angle The rotation angle in radians.
    ///
    /// \return The rotated vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector3<T> RotateAroundAxis(const TVector3<T>& axis, T angle) const
    {
        T cosAngle = static_cast<T>(std::cos(angle));
        T sinAngle = static_cast<T>(std::sin(angle));

        TVector3<T> parallel = axis * Dot(axis);
        TVector3<T> perpendicular = *this - parallel;
        TVector3<T> w = axis.Cross(*this);

        return parallel + (perpendicular * cosAngle) + (w * sinAngle);
    }

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Normalizes the vector in place. If the vector is zero, it
    /// remains unchanged.
    ///
    /// \param vec The vector to normalize.
    ///
    /// \return Reference to the normalized vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    static TVector3<T> Normalize(const TVector3<T>& vec)
    {
        T length = vec.Length();

        constexpr T epsilon = std::numeric_limits<T>::epsilon();

        if (std::abs(length) < epsilon)
        {
            return TVector3<T>(static_cast<T>(0));
        }

        T invLength = static_cast<T>(1) / length;
        return TVector3<T>(
            vec.x * invLength, vec.y * invLength, vec.z * invLength
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Dot product of two vectors.
    ///
    /// \param lhs The left-hand side vector.
    /// \param rhs The right-hand side vector.
    ///
    /// \return The dot product.
    ///
    ///////////////////////////////////////////////////////////////////////////
    static T Dot(const TVector3<T>& lhs, const TVector3<T>& rhs)
    {
        return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Cross product of two vectors.
    ///
    /// \param lhs The left-hand side vector.
    /// \param rhs The right-hand side vector.
    ///
    /// \return The cross product vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    static TVector3<T> Cross(const TVector3<T>& lhs, const TVector3<T>& rhs)
    {
        return TVector3<T>(
            lhs.y * rhs.z - lhs.z * rhs.y,
            lhs.z * rhs.x - lhs.x * rhs.z,
            lhs.x * rhs.y - lhs.y * rhs.x
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Computes the distance between two vectors.
    ///
    /// \param lhs The first vector.
    /// \param rhs The second vector.
    ///
    /// \return The distance.
    ///
    ///////////////////////////////////////////////////////////////////////////
    static T Distance(const TVector3<T>& lhs, const TVector3<T>& rhs)
    {
        return lhs.Distance(rhs);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Computes the squared distance between two vectors.
    ///
    /// \param lhs The first vector.
    /// \param rhs The second vector.
    ///
    /// \return The squared distance.
    ///
    ///////////////////////////////////////////////////////////////////////////
    static T DistanceSquared(const TVector3<T>& lhs, const TVector3<T>& rhs)
    {
        return lhs.DistanceSquared(rhs);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Projects one vector onto another.
    ///
    /// \param vec The vector to project.
    /// \param onto The vector to project onto.
    ///
    /// \return The projected vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    static TVector3<T> Project(const TVector3<T>& vec, const TVector3<T>& onto)
    {
        return vec.Project(onto);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Reflects a vector around a normal.
    ///
    /// \param vec The vector to reflect.
    /// \param normal The normal vector.
    ///
    /// \return The reflected vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    static TVector3<T>
        Reflect(const TVector3<T>& vec, const TVector3<T>& normal)
    {
        return vec.Reflect(normal);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Refracts a vector through a surface with a given index of
    /// refraction.
    ///
    /// \param vec The incident vector (should be normalized).
    /// \param normal The surface normal (should be normalized).
    /// \param eta The ratio of indices of refraction.
    ///
    /// \return The refracted vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    static TVector3<T>
        Refract(const TVector3<T>& vec, const TVector3<T>& normal, T eta)
    {
        T dot = Dot(vec, normal);
        T k = static_cast<T>(1) - eta * eta * (static_cast<T>(1) - dot * dot);

        if (k < static_cast<T>(0)) { return TVector3<T>::Zero; }

        return vec * eta - normal * (eta * dot + std::sqrt(k));
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Computes the angle between two vectors (in radians).
    ///
    /// \param lhs The first vector.
    /// \param rhs The second vector.
    ///
    /// \return The angle in radians.
    ///
    ///////////////////////////////////////////////////////////////////////////
    static T Angle(const TVector3<T>& lhs, const TVector3<T>& rhs)
    {
        return lhs.Angle(rhs);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Linearly interpolates between two vectors.
    ///
    /// \param start The start vector.
    /// \param end The end vector.
    /// \param t The interpolation factor (0 to 1).
    ///
    /// \return The interpolated vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    static TVector3<T>
        Lerp(const TVector3<T>& start, const TVector3<T>& end, T t)
    {
        return start.Lerp(end, t);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Linearly interpolates between two vectors without clamping t.
    ///
    /// \param start The start vector.
    /// \param end The end vector.
    /// \param t The interpolation factor (can be outside 0-1 range).
    ///
    /// \return The interpolated vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    static TVector3<T>
        LerpUnclamped(const TVector3<T>& start, const TVector3<T>& end, T t)
    {
        return start.Lerp(end, t);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Spherically interpolates between two vectors.
    ///
    /// \param start The start vector.
    /// \param end The end vector.
    /// \param t The interpolation factor (0 to 1).
    ///
    /// \return The interpolated vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    static TVector3<T>
        Slerp(const TVector3<T>& start, const TVector3<T>& end, T t)
    {
        return start.Slerp(end, t);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Smoothly interpolates between two vectors using cubic Hermite
    /// interpolation.
    ///
    /// \param start The start vector.
    /// \param end The end vector.
    /// \param t The interpolation factor (0 to 1).
    ///
    /// \return The smoothly interpolated vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    static TVector3<T>
        SmoothStep(const TVector3<T>& start, const TVector3<T>& end, T t)
    {
        t = std::max(static_cast<T>(0), std::min(static_cast<T>(1), t));
        t = t * t * (static_cast<T>(3) - static_cast<T>(2) * t);
        return start.Lerp(end, t);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns the component-wise minimum of two vectors.
    ///
    /// \param lhs The first vector.
    /// \param rhs The second vector.
    ///
    /// \return The minimum vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    static TVector3<T> Min(const TVector3<T>& lhs, const TVector3<T>& rhs)
    {
        return lhs.Min(rhs);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns the component-wise maximum of two vectors.
    ///
    /// \param lhs The first vector.
    /// \param rhs The second vector.
    ///
    /// \return The maximum vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    static TVector3<T> Max(const TVector3<T>& lhs, const TVector3<T>& rhs)
    {
        return lhs.Max(rhs);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Clamps a vector between min and max values.
    ///
    /// \param vec The vector to clamp.
    /// \param min The minimum value.
    /// \param max The maximum value.
    ///
    /// \return The clamped vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    static TVector3<T>
        Clamp(const TVector3<T>& vec, const T& min, const T& max)
    {
        return vec.Clamp(min, max);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Clamps a vector between min and max vectors.
    ///
    /// \param vec The vector to clamp.
    /// \param min The minimum vector.
    /// \param max The maximum vector.
    ///
    /// \return The clamped vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    static TVector3<T> Clamp(
        const TVector3<T>& vec, const TVector3<T>& min, const TVector3<T>& max
    )
    {
        return vec.Clamp(min, max);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Moves a point towards a target at a specified maximum distance.
    ///
    /// \param current The current position.
    /// \param target The target position.
    /// \param maxDistanceDelta The maximum distance to move.
    ///
    /// \return The new position.
    ///
    ///////////////////////////////////////////////////////////////////////////
    static TVector3<T> MoveTowards(
        const TVector3<T>& current,
        const TVector3<T>& target,
        T maxDistanceDelta
    )
    {
        TVector3<T> delta = target - current;
        T distSq = delta.LengthSquared();

        if (distSq <= maxDistanceDelta * maxDistanceDelta ||
            distSq == static_cast<T>(0))
        {
            return target;
        }

        T dist = std::sqrt(distSq);
        return current + delta * (maxDistanceDelta / dist);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Scales a vector towards a target at a specified rate.
    ///
    /// \param current The current vector.
    /// \param target The target vector.
    /// \param maxRadiansDelta The maximum rotation in radians.
    /// \param maxMagnitudeDelta The maximum change in magnitude.
    ///
    /// \return The rotated vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    static TVector3<T> RotateTowards(
        const TVector3<T>& current,
        const TVector3<T>& target,
        T maxRadiansDelta,
        T maxMagnitudeDelta
    )
    {
        T currentLen = current.Length();
        T targetLen = target.Length();

        constexpr T epsilon = std::numeric_limits<T>::epsilon();
        if (currentLen < epsilon || targetLen < epsilon)
        {
            return MoveTowards(current, target, maxMagnitudeDelta);
        }

        TVector3<T> currentNorm = current / currentLen;
        TVector3<T> targetNorm = target / targetLen;

        T angle = Angle(currentNorm, targetNorm);

        if (angle < epsilon)
        {
            return MoveTowards(current, target, maxMagnitudeDelta);
        }

        if (angle > maxRadiansDelta) { angle = maxRadiansDelta; }

        TVector3<T> axis = Cross(currentNorm, targetNorm).Normalized();
        TVector3<T> rotated = currentNorm.RotateAroundAxis(axis, angle);

        T newLen = currentLen +
                   std::max(
                       -maxMagnitudeDelta,
                       std::min(maxMagnitudeDelta, targetLen - currentLen)
                   );

        return rotated * newLen;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gradually changes a vector towards a target with velocity
    /// smoothing.
    ///
    /// \param current The current position.
    /// \param target The target position.
    /// \param currentVelocity The current velocity (modified by reference).
    /// \param smoothTime The approximate time to reach the target.
    /// \param maxSpeed The maximum speed (optional).
    /// \param deltaTime The time since last call.
    ///
    /// \return The smoothed position.
    ///
    ///////////////////////////////////////////////////////////////////////////
    static TVector3<T> SmoothDamp(
        const TVector3<T>& current,
        const TVector3<T>& target,
        TVector3<T>& currentVelocity,
        T smoothTime,
        T maxSpeed,
        T deltaTime
    )
    {
        smoothTime = std::max(static_cast<T>(0.0001), smoothTime);
        T omega = static_cast<T>(2) / smoothTime;
        T x = omega * deltaTime;
        T exp = static_cast<T>(1) /
                (static_cast<T>(1) + x + static_cast<T>(0.48) * x * x +
                 static_cast<T>(0.235) * x * x * x);

        TVector3<T> change = current - target;
        TVector3<T> originalTo = target;

        T maxChange = maxSpeed * smoothTime;
        change = change.LimitLength(maxChange);
        target = current - change;

        TVector3<T> temp = (currentVelocity + omega * change) * deltaTime;
        currentVelocity = (currentVelocity - omega * temp) * exp;
        TVector3<T> output = target + (change + temp) * exp;

        if (Dot(originalTo - current, output - originalTo) > static_cast<T>(0))
        {
            output = originalTo;
            currentVelocity = (output - originalTo) / deltaTime;
        }

        return output;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Simplified SmoothDamp with default max speed.
    ///
    /// \param current The current position.
    /// \param target The target position.
    /// \param currentVelocity The current velocity (modified by reference).
    /// \param smoothTime The approximate time to reach the target.
    /// \param deltaTime The time since last call.
    ///
    /// \return The smoothed position.
    ///
    ///////////////////////////////////////////////////////////////////////////
    static TVector3<T> SmoothDamp(
        const TVector3<T>& current,
        const TVector3<T>& target,
        TVector3<T>& currentVelocity,
        T smoothTime,
        T deltaTime
    )
    {
        return SmoothDamp(
            current,
            target,
            currentVelocity,
            smoothTime,
            std::numeric_limits<T>::infinity(),
            deltaTime
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Calculates a position between three points using Catmull-Rom
    /// spline.
    ///
    /// \param p0 The first control point.
    /// \param p1 The start point.
    /// \param p2 The end point.
    /// \param p3 The second control point.
    /// \param t The interpolation factor (0 to 1).
    ///
    /// \return The interpolated position.
    ///
    ///////////////////////////////////////////////////////////////////////////
    static TVector3<T> CatmullRom(
        const TVector3<T>& p0,
        const TVector3<T>& p1,
        const TVector3<T>& p2,
        const TVector3<T>& p3,
        T t
    )
    {
        T t2 = t * t;
        T t3 = t2 * t;

        return static_cast<T>(0.5) *
               ((static_cast<T>(2) * p1) + (-p0 + p2) * t +
                (static_cast<T>(2) * p0 - static_cast<T>(5) * p1 +
                 static_cast<T>(4) * p2 - p3) *
                    t2 +
                (-p0 + static_cast<T>(3) * p1 - static_cast<T>(3) * p2 + p3) *
                    t3);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Calculates a position on a cubic Bezier curve.
    ///
    /// \param p0 The start point.
    /// \param p1 The first control point.
    /// \param p2 The second control point.
    /// \param p3 The end point.
    /// \param t The interpolation factor (0 to 1).
    ///
    /// \return The interpolated position.
    ///
    ///////////////////////////////////////////////////////////////////////////
    static TVector3<T> BezierCubic(
        const TVector3<T>& p0,
        const TVector3<T>& p1,
        const TVector3<T>& p2,
        const TVector3<T>& p3,
        T t
    )
    {
        T u = static_cast<T>(1) - t;
        T tt = t * t;
        T uu = u * u;
        T uuu = uu * u;
        T ttt = tt * t;

        return (p0 * uuu) + (p1 * static_cast<T>(3) * uu * t) +
               (p2 * static_cast<T>(3) * u * tt) + (p3 * ttt);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Projects a vector onto a plane defined by a normal.
    ///
    /// \param vec The vector to project.
    /// \param planeNormal The plane normal (should be normalized).
    ///
    /// \return The projected vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    static TVector3<T>
        ProjectOnPlane(const TVector3<T>& vec, const TVector3<T>& planeNormal)
    {
        T dot = Dot(vec, planeNormal);
        return vec - planeNormal * dot;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Orthonormalizes two vectors (Gram-Schmidt).
    ///
    /// \param normal The normal vector (will be normalized).
    /// \param tangent The tangent vector (will be made perpendicular and
    /// normalized).
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void OrthoNormalize(TVector3<T>& normal, TVector3<T>& tangent)
    {
        normal = normal.Normalized();
        tangent = ProjectOnPlane(tangent, normal).Normalized();
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Orthonormalizes three vectors (Gram-Schmidt).
    ///
    /// \param normal The normal vector (will be normalized).
    /// \param tangent The tangent vector (will be made perpendicular and
    /// normalized).
    /// \param binormal The binormal vector (will be made perpendicular and
    /// normalized).
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void OrthoNormalize(
        TVector3<T>& normal, TVector3<T>& tangent, TVector3<T>& binormal
    )
    {
        normal = normal.Normalized();
        tangent = ProjectOnPlane(tangent, normal).Normalized();
        binormal = ProjectOnPlane(ProjectOnPlane(binormal, normal), tangent)
                       .Normalized();
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Computes the triple product (scalar triple product).
    ///
    /// \param a The first vector.
    /// \param b The second vector.
    /// \param c The third vector.
    ///
    /// \return The triple product value.
    ///
    ///////////////////////////////////////////////////////////////////////////
    static T TripleProduct(
        const TVector3<T>& a, const TVector3<T>& b, const TVector3<T>& c
    )
    {
        return Dot(Cross(a, b), c);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Generates a random vector within a unit sphere.
    ///
    /// \return A random vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    static TVector3<T> RandomInUnitSphere(void)
    {
        T theta = static_cast<T>(rand()) / static_cast<T>(RAND_MAX) *
                  static_cast<T>(2) * static_cast<T>(M_PI);
        T phi = static_cast<T>(std::acos(
            static_cast<T>(2) * static_cast<T>(rand()) /
                static_cast<T>(RAND_MAX) -
            static_cast<T>(1)
        ));
        T r = static_cast<T>(std::pow(
            static_cast<T>(rand()) / static_cast<T>(RAND_MAX),
            static_cast<T>(1.0 / 3.0)
        ));

        return TVector3<T>(
            r * static_cast<T>(std::sin(phi)) *
                static_cast<T>(std::cos(theta)),
            r * static_cast<T>(std::sin(phi)) *
                static_cast<T>(std::sin(theta)),
            r * static_cast<T>(std::cos(phi))
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Generates a random unit vector on the surface of a unit sphere.
    ///
    /// \return A random unit vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    static TVector3<T> RandomOnUnitSphere(void)
    {
        return RandomInUnitSphere().Normalized();
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
template <typename T>
const TVector3<T> TVector3<T>::Up(0, 1, 0);
template <typename T>
const TVector3<T> TVector3<T>::Down(0, -1, 0);
template <typename T>
const TVector3<T> TVector3<T>::Right(1, 0, 0);
template <typename T>
const TVector3<T> TVector3<T>::Left(-1, 0, 0);
template <typename T>
const TVector3<T> TVector3<T>::Forward(0, 0, 1);
template <typename T>
const TVector3<T> TVector3<T>::Back(0, 0, -1);

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

///////////////////////////////////////////////////////////////////////////////
/// \brief Adds two vectors component-wise.
///
/// \param T The type of the vector components.
/// \param a The first vector.
/// \param b The second vector.
/// \return The resulting vector after addition.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector3<T> Add(const TVector3<T>& a, const TVector3<T>& b)
{
    return TVector3<T>(a.x + b.x, a.y + b.y, a.z + b.z);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Subtracts one vector from another component-wise.
///
/// \param T The type of the vector components.
/// \param a The first vector.
/// \param b The second vector.
/// \return The resulting vector after subtraction.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector3<T> Subtract(const TVector3<T>& a, const TVector3<T>& b)
{
    return TVector3<T>(a.x - b.x, a.y - b.y, a.z - b.z);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Multiplies two vectors component-wise.
///
/// \param T The type of the vector components.
/// \param a The first vector.
/// \param b The second vector.
/// \return The resulting vector after multiplication.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector3<T> Multiply(const TVector3<T>& a, const TVector3<T>& b)
{
    return TVector3<T>(a.x * b.x, a.y * b.y, a.z * b.z);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Divides one vector by another component-wise.
///
/// \param T The type of the vector components.
/// \param a The numerator vector.
/// \param b The denominator vector (must not contain zeros).
/// \return The resulting vector after division.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector3<T> Divide(const TVector3<T>& a, const TVector3<T>& b)
{
    assert(b.x != T(0) && b.y != T(0) && b.z != T(0));
    return TVector3<T>(a.x / b.x, a.y / b.y, a.z / b.z);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Performs a multiply-add operation on vectors: a * b + c.
///
/// \param T The type of the vector components.
/// \param a The first vector (multiplicand).
/// \param b The second vector (multiplier).
/// \param c The vector to be added.
/// \return The resulting vector after the multiply-add operation.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector3<T> MultiplyAdd(
    const TVector3<T>& a, const TVector3<T>& b, const TVector3<T>& c
)
{
    return TVector3<T>(a.x * b.x + c.x, a.y * b.y + c.y, a.z * b.z + c.z);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Computes the 3D cross product of two vectors.
///
/// \param T The type of the vector components.
/// \param a The first vector.
/// \param b The second vector.
/// \return The resulting vector of the cross product.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector3<T> Cross(const TVector3<T>& a, const TVector3<T>& b)
{
    return TVector3<T>(
        a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x
    );
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Projects vector a onto vector b.
///
/// \param T The type of the vector components.
/// \param a The vector to project.
/// \param b The vector onto which a is projected (must not be the zero
/// vector).
/// \return The projected vector.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector3<T> Project(const TVector3<T>& a, const TVector3<T>& b)
{
    T dot = a.x * b.x + a.y * b.y + a.z * b.z;
    T lenSq = b.x * b.x + b.y * b.y + b.z * b.z;
    assert(lenSq != T(0));
    T scale = dot / lenSq;
    return TVector3<T>(b.x * scale, b.y * scale, b.z * scale);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Reflects vector a around a normal vector n.
///
/// \param T The type of the vector components.
/// \param a The incident vector.
/// \param n The normal vector (assumed to be normalized).
/// \return The reflected vector.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector3<T> Reflect(const TVector3<T>& a, const TVector3<T>& n)
{
    T dot = a.x * n.x + a.y * n.y + a.z * n.z;
    return TVector3<T>(
        a.x - 2 * dot * n.x, a.y - 2 * dot * n.y, a.z - 2 * dot * n.z
    );
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Computes the Euclidean distance between two vectors.
///
/// \param T The type of the vector components.
/// \param a The first vector.
/// \param b The second vector.
/// \return The distance between the two vectors.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
T Distance(const TVector3<T>& a, const TVector3<T>& b)
{
    T dx = a.x - b.x;
    T dy = a.y - b.y;
    T dz = a.z - b.z;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Computes the length (magnitude) of a vector.
///
/// \param T The type of the vector components.
/// \param v The input vector.
/// \return The length of the vector.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
T Length(const TVector3<T>& v)
{
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Returns the component-wise absolute value of a vector.
///
/// \param T The type of the vector components.
/// \param v The input vector.
/// \return The vector with absolute values of each component.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector3<T> Absolute(const TVector3<T>& v)
{
    return TVector3<T>(std::abs(v.x), std::abs(v.y), std::abs(v.z));
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Normalizes a vector to unit length.
///
/// \param T The type of the vector components.
/// \param v The input vector (must not be the zero vector).
/// \return The normalized vector.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector3<T> Normalize(const TVector3<T>& v)
{
    T len = Length(v);
    assert(len != T(0));
    return TVector3<T>(v.x / len, v.y / len, v.z / len);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Computes the component-wise minimum of two vectors.
///
/// \param T The type of the vector components.
/// \param a The first vector.
/// \param b The second vector.
/// \return A vector containing the minimum of each component.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector3<T> Minimum(const TVector3<T>& a, const TVector3<T>& b)
{
    return TVector3<T>(
        std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z)
    );
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Computes the component-wise maximum of two vectors.
///
/// \param T The type of the vector components.
/// \param a The first vector.
/// \param b The second vector.
/// \return A vector containing the maximum of each component.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector3<T> Maximum(const TVector3<T>& a, const TVector3<T>& b)
{
    return TVector3<T>(
        std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z)
    );
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Applies floor operation component-wise to a vector.
///
/// \param T The type of the vector components.
/// \param v The input vector.
/// \return The floored vector.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector3<T> Floor(const TVector3<T>& v)
{
    return TVector3<T>(std::floor(v.x), std::floor(v.y), std::floor(v.z));
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Applies ceil operation component-wise to a vector.
///
/// \param T The type of the vector components.
/// \param v The input vector.
/// \return The ceiled vector.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector3<T> Ceil(const TVector3<T>& v)
{
    return TVector3<T>(std::ceil(v.x), std::ceil(v.y), std::ceil(v.z));
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Computes the fractional part of each component of a vector.
///
/// \param T The type of the vector components.
/// \param v The input vector.
/// \return The fractional part of the vector components.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector3<T> Fraction(const TVector3<T>& v)
{
    return TVector3<T>(
        v.x - std::floor(v.x), v.y - std::floor(v.y), v.z - std::floor(v.z)
    );
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Applies the modulo operation to each component of the vector.
///
/// \param T The type of the vector components.
/// \param v The input vector.
/// \param m The modulo value.
/// \return A vector where each component is v mod m.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector3<T> Modulo(const TVector3<T>& v, const T& m)
{
    return TVector3<T>(
        std::fmod(v.x, m), std::fmod(v.y, m), std::fmod(v.z, m)
    );
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Wraps each component of the vector within a specified range [min,
/// max).
///
/// \param T The type of the vector components.
/// \param v The input vector.
/// \param min The minimum value of the range.
/// \param max The maximum value of the range.
/// \return A vector with components wrapped into the [min, max) interval.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector3<T> Wrap(const TVector3<T>& v, const T& min, const T& max)
{
    auto wrap = [](T val, T min, T max)
    {
        T range = max - min;
        return min + std::fmod(std::fmod(val - min, range) + range, range);
    };
    return TVector3<T>(
        wrap(v.x, min, max), wrap(v.y, min, max), wrap(v.z, min, max)
    );
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Snaps each component of the vector to a multiple of the given step.
///
/// \param T The type of the vector components.
/// \param v The input vector.
/// \param step The snapping step.
/// \return A vector with each component rounded to the nearest multiple of
/// step.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector3<T> Snap(const TVector3<T>& v, const T& step)
{
    auto snap = [](T val, T step) { return std::round(val / step) * step; };
    return TVector3<T>(snap(v.x, step), snap(v.y, step), snap(v.z, step));
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Applies the sine function to each component of the vector.
///
/// \param T The type of the vector components.
/// \param v The input vector.
/// \return A vector where each component is the sine of the original
/// component.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector3<T> Sine(const TVector3<T>& v)
{
    return TVector3<T>(std::sin(v.x), std::sin(v.y), std::sin(v.z));
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Applies the cosine function to each component of the vector.
///
/// \param T The type of the vector components.
/// \param v The input vector.
/// \return A vector where each component is the cosine of the original
/// component.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector3<T> Cosine(const TVector3<T>& v)
{
    return TVector3<T>(std::cos(v.x), std::cos(v.y), std::cos(v.z));
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Applies the tangent function to each component of the vector.
///
/// \param T The type of the vector components.
/// \param v The input vector.
/// \return A vector where each component is the tangent of the original
/// component.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TVector3<T> Tangent(const TVector3<T>& v)
{
    return TVector3<T>(std::tan(v.x), std::tan(v.y), std::tan(v.z));
}

}   // namespace tkd
