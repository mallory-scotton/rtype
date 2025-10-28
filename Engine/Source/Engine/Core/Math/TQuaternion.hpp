///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <cmath>
#include <Engine/Core/Math/TVector3.hpp>
#include <Engine/Core/Math/TVector4.hpp>
#include <iostream>
#include <type_traits>

///////////////////////////////////////////////////////////////////////////////
// Forward declarations
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{
template <typename T>
class TMatrix4x4;
}   // namespace tkd

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief Quaternion class for 3D rotations
///
/// \tparam T Numeric type (float, double)
///
/// Quaternions represent rotations using 4 components: x, y, z, w
/// where w is the scalar part and (x, y, z) is the vector part.
/// They avoid gimbal lock and provide smooth interpolation.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
class TQuaternion
{
public:
    static_assert(
        std::is_arithmetic<T>::value, "T must be an arithmetic type"
    );

private:
    ///////////////////////////////////////////////////////////////////////////
    // Member variables
    ///////////////////////////////////////////////////////////////////////////
    TVector4<T> m_quaternion;   //<! Quaternion components (x, y, z, w)

public:
    ///////////////////////////////////////////////////////////////////////////
    // Static Members
    ///////////////////////////////////////////////////////////////////////////
    static const TQuaternion Identity;   //<! Identity quaternion (0, 0, 0, 1)

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor - creates identity quaternion
    ///
    ///////////////////////////////////////////////////////////////////////////
    TQuaternion()
        : m_quaternion(
              static_cast<T>(0),
              static_cast<T>(0),
              static_cast<T>(0),
              static_cast<T>(1)
          )
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor with explicit components
    ///
    /// \param x X component (vector part)
    /// \param y Y component (vector part)
    /// \param z Z component (vector part)
    /// \param w W component (scalar part)
    ///
    ///////////////////////////////////////////////////////////////////////////
    TQuaternion(const T& x, const T& y, const T& z, const T& w)
        : m_quaternion(x, y, z, w)
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor from axis-angle representation
    ///
    /// \param axis Rotation axis (must be normalized)
    /// \param angle Rotation angle in degrees
    ///
    ///////////////////////////////////////////////////////////////////////////
    TQuaternion(const TVector3<T>& axis, const T& angle)
    {
        FromAxisAngle(axis, angle);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor from Euler angles
    ///
    /// \param pitch Rotation around X-axis (degrees)
    /// \param yaw Rotation around Y-axis (degrees)
    /// \param roll Rotation around Z-axis (degrees)
    ///
    ///////////////////////////////////////////////////////////////////////////
    TQuaternion(const T& pitch, const T& yaw, const T& roll)
    {
        FromEulerAngles(pitch, yaw, roll);
    }

public:
    T GetX(void) const { return m_quaternion.x; }

    T GetY(void) const { return m_quaternion.y; }

    T GetZ(void) const { return m_quaternion.z; }

    T GetW(void) const { return m_quaternion.w; }

    void SetX(const T& value) { m_quaternion.x = value; }

    void SetY(const T& value) { m_quaternion.y = value; }

    void SetZ(const T& value) { m_quaternion.z = value; }

    void SetW(const T& value) { m_quaternion.w = value; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get vector part as TVector3
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector3<T> GetVectorPart(void) const
    {
        return TVector3<T>(m_quaternion.x, m_quaternion.y, m_quaternion.z);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get scalar part
    ///
    ///////////////////////////////////////////////////////////////////////////
    T GetScalarPart(void) const { return m_quaternion.w; }

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Calculate magnitude/length of quaternion
    ///
    ///////////////////////////////////////////////////////////////////////////
    T Magnitude(void) const { return m_quaternion.Magnitude(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Calculate squared magnitude (more efficient)
    ///
    ///////////////////////////////////////////////////////////////////////////
    T MagnitudeSquared(void) const { return m_quaternion.MagnitudeSquared(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Normalize quaternion to unit length
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Normalize(void) { m_quaternion.Normalize(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get normalized copy of quaternion
    ///
    ///////////////////////////////////////////////////////////////////////////
    TQuaternion Normalized(void) const
    {
        TQuaternion result = *this;
        result.Normalize();
        return result;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get conjugate of quaternion (negates vector part)
    ///
    ///////////////////////////////////////////////////////////////////////////
    TQuaternion Conjugate(void) const
    {
        return TQuaternion(
            -m_quaternion.x, -m_quaternion.y, -m_quaternion.z, m_quaternion.w
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get inverse of quaternion
    ///
    ///////////////////////////////////////////////////////////////////////////
    TQuaternion Inverse(void) const
    {
        T magSq = MagnitudeSquared();
        if (magSq > static_cast<T>(0))
        {
            TQuaternion conj = Conjugate();
            return TQuaternion(
                conj.GetX() / magSq,
                conj.GetY() / magSq,
                conj.GetZ() / magSq,
                conj.GetW() / magSq
            );
        }
        return TQuaternion();
    }

public:
    ///////////////////////////////////////////////////////////////////////////
    // Rotation Operations
    ///////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Create from axis-angle representation
    ///
    ///////////////////////////////////////////////////////////////////////////
    void FromAxisAngle(const TVector3<T>& axis, const T& angleDegrees)
    {
        T angleRad = angleDegrees * static_cast<T>(M_PI) / static_cast<T>(180);
        T halfAngle = angleRad * static_cast<T>(0.5);
        T sinHalf = std::sin(halfAngle);

        m_quaternion.x = axis.x * sinHalf;
        m_quaternion.y = axis.y * sinHalf;
        m_quaternion.z = axis.z * sinHalf;
        m_quaternion.w = std::cos(halfAngle);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Create from Euler angles (ZYX order)
    ///
    ///////////////////////////////////////////////////////////////////////////
    void FromEulerAngles(const T& pitch, const T& yaw, const T& roll)
    {
        T pitchRad = pitch * static_cast<T>(M_PI) / static_cast<T>(180) *
                     static_cast<T>(0.5);
        T yawRad = yaw * static_cast<T>(M_PI) / static_cast<T>(180) *
                   static_cast<T>(0.5);
        T rollRad = roll * static_cast<T>(M_PI) / static_cast<T>(180) *
                    static_cast<T>(0.5);

        T cp = std::cos(pitchRad);
        T sp = std::sin(pitchRad);
        T cy = std::cos(yawRad);
        T sy = std::sin(yawRad);
        T cr = std::cos(rollRad);
        T sr = std::sin(rollRad);

        m_quaternion.w = cr * cp * cy + sr * sp * sy;
        m_quaternion.x = sr * cp * cy - cr * sp * sy;
        m_quaternion.y = cr * sp * cy + sr * cp * sy;
        m_quaternion.z = cr * cp * sy - sr * sp * cy;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Convert to Euler angles (pitch, yaw, roll)
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector3<T> ToEulerAngles() const
    {
        T pitch, yaw, roll;

        // Roll (x-axis rotation)
        T sinr_cosp = 2 * (m_quaternion.w * m_quaternion.x +
                           m_quaternion.y * m_quaternion.z);
        T cosr_cosp = 1 - 2 * (m_quaternion.x * m_quaternion.x +
                               m_quaternion.y * m_quaternion.y);
        roll = std::atan2(sinr_cosp, cosr_cosp);

        // Pitch (y-axis rotation)
        T sinp = 2 * (m_quaternion.w * m_quaternion.y -
                      m_quaternion.z * m_quaternion.x);
        if (std::abs(sinp) >= 1)
        {
            pitch = std::copysign(static_cast<T>(M_PI) / 2, sinp);
        }
        else { pitch = std::asin(sinp); }

        // Yaw (z-axis rotation)
        T siny_cosp = 2 * (m_quaternion.w * m_quaternion.z +
                           m_quaternion.x * m_quaternion.y);
        T cosy_cosp = 1 - 2 * (m_quaternion.y * m_quaternion.y +
                               m_quaternion.z * m_quaternion.z);
        yaw = std::atan2(siny_cosp, cosy_cosp);

        // Convert to degrees
        pitch *= static_cast<T>(180) / static_cast<T>(M_PI);
        yaw *= static_cast<T>(180) / static_cast<T>(M_PI);
        roll *= static_cast<T>(180) / static_cast<T>(M_PI);

        return TVector3<T>(pitch, yaw, roll);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Convert to 4x4 rotation matrix
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMatrix4x4<T> ToMatrix4x4() const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Create from 4x4 rotation matrix
    ///
    /// \param matrix The rotation matrix to convert from
    ///
    ///////////////////////////////////////////////////////////////////////////
    void FromMatrix4x4(const TMatrix4x4<T>& matrix);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get rotation angle in degrees
    ///
    ///////////////////////////////////////////////////////////////////////////
    T GetAngle() const
    {
        // w = cos(angle/2), so angle = 2 * acos(w)
        T w = m_quaternion.w;
        // Clamp to avoid numerical issues with acos
        if (w > static_cast<T>(1)) { w = static_cast<T>(1); }
        if (w < static_cast<T>(-1)) { w = static_cast<T>(-1); }

        T angleRad = static_cast<T>(2) * std::acos(w);
        return angleRad * static_cast<T>(180) / static_cast<T>(M_PI);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get rotation axis
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector3<T> GetAxis() const
    {
        T sinHalfAngle =
            std::sqrt(static_cast<T>(1) - m_quaternion.w * m_quaternion.w);

        // Avoid division by zero for identity quaternion
        if (sinHalfAngle < static_cast<T>(0.0001))
        {
            return TVector3<T>(
                static_cast<T>(1), static_cast<T>(0), static_cast<T>(0)
            );
        }

        return TVector3<T>(
            m_quaternion.x / sinHalfAngle,
            m_quaternion.y / sinHalfAngle,
            m_quaternion.z / sinHalfAngle
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Rotate a vector by this quaternion
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector3<T> RotateVector(const TVector3<T>& vector) const
    {
        // v' = q * v * q^(-1)
        // More efficient formula: v' = v + 2 * cross(qv, cross(qv, v) + w*v)
        TVector3<T> qv(m_quaternion.x, m_quaternion.y, m_quaternion.z);
        TVector3<T> cross1 = qv.Cross(vector);
        TVector3<T> cross2 = qv.Cross(cross1 + vector * m_quaternion.w);
        return vector + cross2 * static_cast<T>(2);
    }

public:
    ///////////////////////////////////////////////////////////////////////////
    // Static Utility Functions
    ///////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Calculate dot product between two quaternions
    ///
    ///////////////////////////////////////////////////////////////////////////
    static T Dot(const TQuaternion& a, const TQuaternion& b)
    {
        return a.GetX() * b.GetX() + a.GetY() * b.GetY() +
               a.GetZ() * b.GetZ() + a.GetW() * b.GetW();
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Calculate angle between two quaternions in degrees
    ///
    ///////////////////////////////////////////////////////////////////////////
    static T AngleBetween(const TQuaternion& a, const TQuaternion& b)
    {
        T dot = Dot(a, b);
        // Clamp to avoid numerical issues with acos
        if (dot > static_cast<T>(1)) { dot = static_cast<T>(1); }
        if (dot < static_cast<T>(-1)) { dot = static_cast<T>(-1); }

        T angleRad = std::acos(std::abs(dot)) * static_cast<T>(2);
        return angleRad * static_cast<T>(180) / static_cast<T>(M_PI);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Create rotation from direction vector (look at)
    ///
    /// \param forward The forward direction (will be normalized)
    /// \param up The up direction (default is world up)
    ///
    ///////////////////////////////////////////////////////////////////////////
    static TQuaternion LookRotation(
        const TVector3<T>& forward,
        const TVector3<T>& up = TVector3<T>(
            static_cast<T>(0), static_cast<T>(1), static_cast<T>(0)
        )
    )
    {
        TVector3<T> f = forward;
        f.Normalize();

        TVector3<T> u = up;
        u.Normalize();

        // Calculate right vector
        TVector3<T> r = u.Cross(f);
        r.Normalize();

        // Recalculate up to ensure orthogonality
        u = f.Cross(r);

        // Build rotation matrix and convert to quaternion
        T trace = r.x + u.y + f.z;

        TQuaternion result;

        if (trace > static_cast<T>(0))
        {
            T s = std::sqrt(trace + static_cast<T>(1)) * static_cast<T>(2);
            result.SetW(static_cast<T>(0.25) * s);
            result.SetX((u.z - f.y) / s);
            result.SetY((f.x - r.z) / s);
            result.SetZ((r.y - u.x) / s);
        }
        else if (r.x > u.y && r.x > f.z)
        {
            T s = std::sqrt(static_cast<T>(1) + r.x - u.y - f.z) *
                  static_cast<T>(2);
            result.SetW((u.z - f.y) / s);
            result.SetX(static_cast<T>(0.25) * s);
            result.SetY((u.x + r.y) / s);
            result.SetZ((f.x + r.z) / s);
        }
        else if (u.y > f.z)
        {
            T s = std::sqrt(static_cast<T>(1) + u.y - r.x - f.z) *
                  static_cast<T>(2);
            result.SetW((f.x - r.z) / s);
            result.SetX((u.x + r.y) / s);
            result.SetY(static_cast<T>(0.25) * s);
            result.SetZ((f.y + u.z) / s);
        }
        else
        {
            T s = std::sqrt(static_cast<T>(1) + f.z - r.x - u.y) *
                  static_cast<T>(2);
            result.SetW((r.y - u.x) / s);
            result.SetX((f.x + r.z) / s);
            result.SetY((f.y + u.z) / s);
            result.SetZ(static_cast<T>(0.25) * s);
        }

        return result;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Create rotation between two vectors
    ///
    /// \param from Starting direction
    /// \param to Target direction
    ///
    ///////////////////////////////////////////////////////////////////////////
    static TQuaternion
        FromToRotation(const TVector3<T>& from, const TVector3<T>& to)
    {
        TVector3<T> f = from;
        f.Normalize();
        TVector3<T> t = to;
        t.Normalize();

        T dot = f.Dot(t);

        // Vectors are parallel
        if (dot >= static_cast<T>(0.999999)) { return TQuaternion(); }

        // Vectors are opposite
        if (dot <= static_cast<T>(-0.999999))
        {
            // Find an axis perpendicular to from
            TVector3<T> axis(
                static_cast<T>(1), static_cast<T>(0), static_cast<T>(0)
            );
            if (std::abs(f.x) > static_cast<T>(0.9))
            {
                axis = TVector3<T>(
                    static_cast<T>(0), static_cast<T>(1), static_cast<T>(0)
                );
            }
            axis = axis.Cross(f);
            axis.Normalize();
            return TQuaternion(axis, static_cast<T>(180));
        }

        // General case
        TVector3<T> axis = f.Cross(t);
        T s = std::sqrt((static_cast<T>(1) + dot) * static_cast<T>(2));
        T invs = static_cast<T>(1) / s;

        return TQuaternion(
            axis.x * invs,
            axis.y * invs,
            axis.z * invs,
            s * static_cast<T>(0.5)
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Spherical linear interpolation between two quaternions
    ///
    ///////////////////////////////////////////////////////////////////////////
    static TQuaternion
        Slerp(const TQuaternion& from, const TQuaternion& to, const T& t)
    {
        T dot = from.GetX() * to.GetX() + from.GetY() * to.GetY() +
                from.GetZ() * to.GetZ() + from.GetW() * to.GetW();

        // If dot product is negative, negate one quaternion to take shorter
        // path
        TQuaternion to_use =
            (dot < 0)
                ? TQuaternion(-to.GetX(), -to.GetY(), -to.GetZ(), -to.GetW())
                : to;
        if (dot < 0) { dot = -dot; }

        const T threshold = static_cast<T>(0.9995);

        if (dot > threshold)
        {
            // Linear interpolation for very close quaternions
            TQuaternion result(
                from.GetX() + t * (to_use.GetX() - from.GetX()),
                from.GetY() + t * (to_use.GetY() - from.GetY()),
                from.GetZ() + t * (to_use.GetZ() - from.GetZ()),
                from.GetW() + t * (to_use.GetW() - from.GetW())
            );
            result.Normalize();
            return result;
        }

        T theta_0 = std::acos(dot);
        T theta = theta_0 * t;
        T sin_theta = std::sin(theta);
        T sin_theta_0 = std::sin(theta_0);

        T s0 = std::cos(theta) - dot * sin_theta / sin_theta_0;
        T s1 = sin_theta / sin_theta_0;

        return TQuaternion(
            s0 * from.GetX() + s1 * to_use.GetX(),
            s0 * from.GetY() + s1 * to_use.GetY(),
            s0 * from.GetZ() + s1 * to_use.GetZ(),
            s0 * from.GetW() + s1 * to_use.GetW()
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Linear interpolation between two quaternions (faster but less
    /// smooth)
    ///
    ///////////////////////////////////////////////////////////////////////////
    static TQuaternion
        Lerp(const TQuaternion& from, const TQuaternion& to, const T& t)
    {
        T dot = from.GetX() * to.GetX() + from.GetY() * to.GetY() +
                from.GetZ() * to.GetZ() + from.GetW() * to.GetW();
        TQuaternion to_use =
            (dot < 0)
                ? TQuaternion(-to.GetX(), -to.GetY(), -to.GetZ(), -to.GetW())
                : to;

        TQuaternion result(
            from.GetX() + t * (to_use.GetX() - from.GetX()),
            from.GetY() + t * (to_use.GetY() - from.GetY()),
            from.GetZ() + t * (to_use.GetZ() - from.GetZ()),
            from.GetW() + t * (to_use.GetW() - from.GetW())
        );
        result.Normalize();
        return result;
    }
};

///////////////////////////////////////////////////////////////////////////////
// Static Member Initialization
///////////////////////////////////////////////////////////////////////////////
template <typename T>
const TQuaternion<T> TQuaternion<T>::Identity = TQuaternion<T>();

///////////////////////////////////////////////////////////////////////////////
// Operators
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// \brief Quaternion multiplication (rotation composition)
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TQuaternion<T> operator*(const TQuaternion<T>& lhs, const TQuaternion<T>& rhs)
{
    return TQuaternion<T>(
        lhs.GetW() * rhs.GetX() + lhs.GetX() * rhs.GetW() +
            lhs.GetY() * rhs.GetZ() - lhs.GetZ() * rhs.GetY(),
        lhs.GetW() * rhs.GetY() - lhs.GetX() * rhs.GetZ() +
            lhs.GetY() * rhs.GetW() + lhs.GetZ() * rhs.GetX(),
        lhs.GetW() * rhs.GetZ() + lhs.GetX() * rhs.GetY() -
            lhs.GetY() * rhs.GetX() + lhs.GetZ() * rhs.GetW(),
        lhs.GetW() * rhs.GetW() - lhs.GetX() * rhs.GetX() -
            lhs.GetY() * rhs.GetY() - lhs.GetZ() * rhs.GetZ()
    );
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Quaternion multiplication assignment
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TQuaternion<T>& operator*=(TQuaternion<T>& lhs, const TQuaternion<T>& rhs)
{
    lhs = lhs * rhs;
    return lhs;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Scalar multiplication
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TQuaternion<T> operator*(const TQuaternion<T>& quat, T scalar)
{
    return TQuaternion<T>(
        quat.GetX() * scalar,
        quat.GetY() * scalar,
        quat.GetZ() * scalar,
        quat.GetW() * scalar
    );
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Scalar multiplication (reversed)
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TQuaternion<T> operator*(T scalar, const TQuaternion<T>& quat)
{
    return quat * scalar;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Scalar multiplication assignment
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TQuaternion<T>& operator*=(TQuaternion<T>& quat, T scalar)
{
    quat.SetX(quat.GetX() * scalar);
    quat.SetY(quat.GetY() * scalar);
    quat.SetZ(quat.GetZ() * scalar);
    quat.SetW(quat.GetW() * scalar);
    return quat;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Quaternion addition
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TQuaternion<T> operator+(const TQuaternion<T>& lhs, const TQuaternion<T>& rhs)
{
    return TQuaternion<T>(
        lhs.GetX() + rhs.GetX(),
        lhs.GetY() + rhs.GetY(),
        lhs.GetZ() + rhs.GetZ(),
        lhs.GetW() + rhs.GetW()
    );
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Quaternion addition assignment
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TQuaternion<T>& operator+=(TQuaternion<T>& lhs, const TQuaternion<T>& rhs)
{
    lhs.SetX(lhs.GetX() + rhs.GetX());
    lhs.SetY(lhs.GetY() + rhs.GetY());
    lhs.SetZ(lhs.GetZ() + rhs.GetZ());
    lhs.SetW(lhs.GetW() + rhs.GetW());
    return lhs;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Quaternion negation
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TQuaternion<T> operator-(const TQuaternion<T>& quat)
{
    return TQuaternion<T>(
        -quat.GetX(), -quat.GetY(), -quat.GetZ(), -quat.GetW()
    );
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Quaternion subtraction
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TQuaternion<T> operator-(const TQuaternion<T>& lhs, const TQuaternion<T>& rhs)
{
    return TQuaternion<T>(
        lhs.GetX() - rhs.GetX(),
        lhs.GetY() - rhs.GetY(),
        lhs.GetZ() - rhs.GetZ(),
        lhs.GetW() - rhs.GetW()
    );
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Quaternion subtraction assignment
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TQuaternion<T>& operator-=(TQuaternion<T>& lhs, const TQuaternion<T>& rhs)
{
    lhs.SetX(lhs.GetX() - rhs.GetX());
    lhs.SetY(lhs.GetY() - rhs.GetY());
    lhs.SetZ(lhs.GetZ() - rhs.GetZ());
    lhs.SetW(lhs.GetW() - rhs.GetW());
    return lhs;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Equality comparison
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
bool operator==(const TQuaternion<T>& lhs, const TQuaternion<T>& rhs)
{
    return lhs.GetX() == rhs.GetX() && lhs.GetY() == rhs.GetY() &&
           lhs.GetZ() == rhs.GetZ() && lhs.GetW() == rhs.GetW();
}

///////////////////////////////////////////////////////////////////////////////
// Type aliases
///////////////////////////////////////////////////////////////////////////////
using Quaternion = TQuaternion<float>;
using Quaterniond = TQuaternion<double>;

}   // namespace tkd

///////////////////////////////////////////////////////////////////////////////
/// \brief Stream output operator
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
std::ostream& operator<<(std::ostream& os, const tkd::TQuaternion<T>& quat)
{
    os << "Quaternion(X: " << quat.GetX() << ", Y: " << quat.GetY()
       << ", Z: " << quat.GetZ() << ", W: " << quat.GetW() << ")";
    return os;
}

///////////////////////////////////////////////////////////////////////////////
// Include matrix header for implementation
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Core/Math/TMatrix4x4.hpp>

namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
// Template method implementations requiring TMatrix4x4
///////////////////////////////////////////////////////////////////////////////

template <typename T>
TMatrix4x4<T> TQuaternion<T>::ToMatrix4x4() const
{
    T xx = m_quaternion.x * m_quaternion.x;
    T yy = m_quaternion.y * m_quaternion.y;
    T zz = m_quaternion.z * m_quaternion.z;
    T xy = m_quaternion.x * m_quaternion.y;
    T xz = m_quaternion.x * m_quaternion.z;
    T yz = m_quaternion.y * m_quaternion.z;
    T wx = m_quaternion.w * m_quaternion.x;
    T wy = m_quaternion.w * m_quaternion.y;
    T wz = m_quaternion.w * m_quaternion.z;

    TMatrix4x4<T> result;

    result(0, 0) = static_cast<T>(1) - static_cast<T>(2) * (yy + zz);
    result(0, 1) = static_cast<T>(2) * (xy - wz);
    result(0, 2) = static_cast<T>(2) * (xz + wy);
    result(0, 3) = static_cast<T>(0);

    result(1, 0) = static_cast<T>(2) * (xy + wz);
    result(1, 1) = static_cast<T>(1) - static_cast<T>(2) * (xx + zz);
    result(1, 2) = static_cast<T>(2) * (yz - wx);
    result(1, 3) = static_cast<T>(0);

    result(2, 0) = static_cast<T>(2) * (xz - wy);
    result(2, 1) = static_cast<T>(2) * (yz + wx);
    result(2, 2) = static_cast<T>(1) - static_cast<T>(2) * (xx + yy);
    result(2, 3) = static_cast<T>(0);

    result(3, 0) = static_cast<T>(0);
    result(3, 1) = static_cast<T>(0);
    result(3, 2) = static_cast<T>(0);
    result(3, 3) = static_cast<T>(1);

    return result;
}

template <typename T>
void TQuaternion<T>::FromMatrix4x4(const TMatrix4x4<T>& matrix)
{
    T trace = matrix(0, 0) + matrix(1, 1) + matrix(2, 2);

    if (trace > static_cast<T>(0))
    {
        T s = std::sqrt(trace + static_cast<T>(1)) * static_cast<T>(2);
        m_quaternion.w = static_cast<T>(0.25) * s;
        m_quaternion.x = (matrix(2, 1) - matrix(1, 2)) / s;
        m_quaternion.y = (matrix(0, 2) - matrix(2, 0)) / s;
        m_quaternion.z = (matrix(1, 0) - matrix(0, 1)) / s;
    }
    else if (matrix(0, 0) > matrix(1, 1) && matrix(0, 0) > matrix(2, 2))
    {
        T s =
            std::sqrt(
                static_cast<T>(1) + matrix(0, 0) - matrix(1, 1) - matrix(2, 2)
            ) *
            static_cast<T>(2);
        m_quaternion.w = (matrix(2, 1) - matrix(1, 2)) / s;
        m_quaternion.x = static_cast<T>(0.25) * s;
        m_quaternion.y = (matrix(0, 1) + matrix(1, 0)) / s;
        m_quaternion.z = (matrix(0, 2) + matrix(2, 0)) / s;
    }
    else if (matrix(1, 1) > matrix(2, 2))
    {
        T s =
            std::sqrt(
                static_cast<T>(1) + matrix(1, 1) - matrix(0, 0) - matrix(2, 2)
            ) *
            static_cast<T>(2);
        m_quaternion.w = (matrix(0, 2) - matrix(2, 0)) / s;
        m_quaternion.x = (matrix(0, 1) + matrix(1, 0)) / s;
        m_quaternion.y = static_cast<T>(0.25) * s;
        m_quaternion.z = (matrix(1, 2) + matrix(2, 1)) / s;
    }
    else
    {
        T s =
            std::sqrt(
                static_cast<T>(1) + matrix(2, 2) - matrix(0, 0) - matrix(1, 1)
            ) *
            static_cast<T>(2);
        m_quaternion.w = (matrix(1, 0) - matrix(0, 1)) / s;
        m_quaternion.x = (matrix(0, 2) + matrix(2, 0)) / s;
        m_quaternion.y = (matrix(1, 2) + matrix(2, 1)) / s;
        m_quaternion.z = static_cast<T>(0.25) * s;
    }
}

}   // namespace tkd

