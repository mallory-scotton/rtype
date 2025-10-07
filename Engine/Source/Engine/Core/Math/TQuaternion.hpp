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
    TVector4<T> m_quaternion;   ///< Quaternion components (x, y, z, w)

public:
    ///////////////////////////////////////////////////////////////////////////
    // Static Members
    ///////////////////////////////////////////////////////////////////////////
    static const TQuaternion Identity;   //!< Identity quaternion (0, 0, 0, 1)

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
    ///////////////////////////////////////////////////////////////////////////
    // Component Access
    ///////////////////////////////////////////////////////////////////////////

    T GetX() const { return m_quaternion.x; }

    T GetY() const { return m_quaternion.y; }

    T GetZ() const { return m_quaternion.z; }

    T GetW() const { return m_quaternion.w; }

    void SetX(const T& value) { m_quaternion.x = value; }

    void SetY(const T& value) { m_quaternion.y = value; }

    void SetZ(const T& value) { m_quaternion.z = value; }

    void SetW(const T& value) { m_quaternion.w = value; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get vector part as TVector3
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector3<T> GetVectorPart() const
    {
        return TVector3<T>(m_quaternion.x, m_quaternion.y, m_quaternion.z);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get scalar part
    ///
    ///////////////////////////////////////////////////////////////////////////
    T GetScalarPart() const { return m_quaternion.w; }

public:
    ///////////////////////////////////////////////////////////////////////////
    // Quaternion Operations
    ///////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Calculate magnitude/length of quaternion
    ///
    ///////////////////////////////////////////////////////////////////////////
    T Magnitude() const { return m_quaternion.Magnitude(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Calculate squared magnitude (more efficient)
    ///
    ///////////////////////////////////////////////////////////////////////////
    T MagnitudeSquared() const { return m_quaternion.MagnitudeSquared(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Normalize quaternion to unit length
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Normalize() { m_quaternion.Normalize(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get normalized copy of quaternion
    ///
    ///////////////////////////////////////////////////////////////////////////
    TQuaternion Normalized() const
    {
        TQuaternion result = *this;
        result.Normalize();
        return result;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get conjugate of quaternion (negates vector part)
    ///
    ///////////////////////////////////////////////////////////////////////////
    TQuaternion Conjugate() const
    {
        return TQuaternion(
            -m_quaternion.x, -m_quaternion.y, -m_quaternion.z, m_quaternion.w
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get inverse of quaternion
    ///
    ///////////////////////////////////////////////////////////////////////////
    TQuaternion Inverse() const
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
/// \brief Stream output operator
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
std::ostream& operator<<(std::ostream& os, const TQuaternion<T>& quat)
{
    os << "Quaternion(X: " << quat.GetX() << ", Y: " << quat.GetY()
       << ", Z: " << quat.GetZ() << ", W: " << quat.GetW() << ")";
    return os;
}

///////////////////////////////////////////////////////////////////////////////
// Type aliases
///////////////////////////////////////////////////////////////////////////////
using Quaternion = TQuaternion<float>;
using Quaterniond = TQuaternion<double>;

}   // namespace tkd
