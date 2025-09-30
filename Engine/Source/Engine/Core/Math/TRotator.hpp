///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "TVector3.hpp"
#include <cmath>
#include <iostream>
#include <type_traits>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief 3D rotation class using Euler angles (Pitch, Yaw, Roll)
///
/// \tparam T Numeric type (float, double)
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
class TRotator
{
public:
    static_assert(
        std::is_arithmetic<T>::value, "T must be an arithmetic type"
    );

public:
    ///////////////////////////////////////////////////////////////////////////
    // Static Members
    ///////////////////////////////////////////////////////////////////////////
    static const TRotator Identity;   //!< Identity rotator (0, 0, 0)

private:
    ///////////////////////////////////////////////////////////////////////////
    // Member variables
    ///////////////////////////////////////////////////////////////////////////
    T m_pitch;   ///< Rotation around Y-axis (degrees)
    T m_yaw;     ///< Rotation around Z-axis (degrees)
    T m_roll;    ///< Rotation around X-axis (degrees)

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Normalize angle to [-180, 180] range
    ///
    /// \param angle The angle to normalize in degrees
    /// \return Normalized angle
    ///
    ///////////////////////////////////////////////////////////////////////////
    static T NormalizeAngle(const T& angle)
    {
        T normalized = std::fmod(angle, static_cast<T>(360));
        if (normalized > static_cast<T>(180))
        {
            normalized -= static_cast<T>(360);
        }
        else if (normalized <= static_cast<T>(-180))
        {
            normalized += static_cast<T>(360);
        }
        return normalized;
    }

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor - identity rotation
    ///
    ///////////////////////////////////////////////////////////////////////////
    TRotator(void)
        : m_pitch(static_cast<T>(0))
        , m_yaw(static_cast<T>(0))
        , m_roll(static_cast<T>(0))
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor with pitch, yaw, and roll values
    ///
    /// \param pitch Rotation around Y-axis (degrees)
    /// \param yaw Rotation around Z-axis (degrees)
    /// \param roll Rotation around X-axis (degrees)
    ///
    ///////////////////////////////////////////////////////////////////////////
    TRotator(const T& pitch, const T& yaw, const T& roll)
        : m_pitch(NormalizeAngle(pitch))
        , m_yaw(NormalizeAngle(yaw))
        , m_roll(NormalizeAngle(roll))
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Copy constructor
    ///
    /// \param other The rotator to copy from
    ///
    ///////////////////////////////////////////////////////////////////////////
    TRotator(const TRotator& other)
        : m_pitch(other.m_pitch)
        , m_yaw(other.m_yaw)
        , m_roll(other.m_roll)
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Move constructor
    ///
    /// \param other The rotator to move from
    ///
    ///////////////////////////////////////////////////////////////////////////
    TRotator(TRotator&& other) noexcept
        : m_pitch(other.m_pitch)
        , m_yaw(other.m_yaw)
        , m_roll(other.m_roll)
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor from different type rotator
    ///
    /// \tparam U The type of the other rotator's components
    /// \param other The rotator to copy from
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename U>
    TRotator(const TRotator<U>& other)
        : m_pitch(static_cast<T>(other.GetPitch()))
        , m_yaw(static_cast<T>(other.GetYaw()))
        , m_roll(static_cast<T>(other.GetRoll()))
    {}

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Copy assignment operator
    ///
    /// \param other The rotator to assign from
    /// \return Reference to this rotator
    ///
    ///////////////////////////////////////////////////////////////////////////
    TRotator& operator=(const TRotator& other)
    {
        if (this != &other)
        {
            m_pitch = other.m_pitch;
            m_yaw = other.m_yaw;
            m_roll = other.m_roll;
        }
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Move assignment operator
    ///
    /// \param other The rotator to move from
    /// \return Reference to this rotator
    ///
    ///////////////////////////////////////////////////////////////////////////
    TRotator& operator=(TRotator&& other) noexcept
    {
        if (this != &other)
        {
            m_pitch = other.m_pitch;
            m_yaw = other.m_yaw;
            m_roll = other.m_roll;
        }
        return *this;
    }

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the pitch component
    ///
    /// \return The pitch value in degrees
    ///
    ///////////////////////////////////////////////////////////////////////////
    const T& GetPitch(void) const { return m_pitch; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the yaw component
    ///
    /// \return The yaw value in degrees
    ///
    ///////////////////////////////////////////////////////////////////////////
    const T& GetYaw(void) const { return m_yaw; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the roll component
    ///
    /// \return The roll value in degrees
    ///
    ///////////////////////////////////////////////////////////////////////////
    const T& GetRoll(void) const { return m_roll; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the pitch component
    ///
    /// \param pitch The new pitch value in degrees
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetPitch(const T& pitch) { m_pitch = NormalizeAngle(pitch); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the yaw component
    ///
    /// \param yaw The new yaw value in degrees
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetYaw(const T& yaw) { m_yaw = NormalizeAngle(yaw); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the roll component
    ///
    /// \param roll The new roll value in degrees
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetRoll(const T& roll) { m_roll = NormalizeAngle(roll); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Check if this is an identity rotation
    ///
    /// \return True if all components are zero
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool IsIdentity(void) const
    {
        return m_pitch == static_cast<T>(0) && m_yaw == static_cast<T>(0) &&
               m_roll == static_cast<T>(0);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Reset to identity rotation
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Reset(void)
    {
        m_pitch = static_cast<T>(0);
        m_yaw = static_cast<T>(0);
        m_roll = static_cast<T>(0);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Check equality with tolerance
    ///
    /// \param other The other rotator to compare with
    /// \param tolerance The tolerance for comparison
    /// \return True if rotators are equal within tolerance
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool Equals(
        const TRotator& other, const T& tolerance = static_cast<T>(1e-6)
    ) const
    {
        return std::abs(m_pitch - other.m_pitch) <= tolerance &&
               std::abs(m_yaw - other.m_yaw) <= tolerance &&
               std::abs(m_roll - other.m_roll) <= tolerance;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Rotate by given rotator values
    ///
    /// \param rotator The rotation to add
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Rotate(const TRotator& rotator)
    {
        m_pitch = NormalizeAngle(m_pitch + rotator.m_pitch);
        m_yaw = NormalizeAngle(m_yaw + rotator.m_yaw);
        m_roll = NormalizeAngle(m_roll + rotator.m_roll);
    }

    ///////////////////////////////////////////////////////////////////////////
    // Static utility functions
    ///////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Static method to rotate a rotator by another rotator
    ///
    /// \param rotator The rotator to rotate
    /// \param rotation The rotation to add
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void Rotate(TRotator<T>& rotator, const TRotator<T>& rotation)
    {
        rotator.Rotate(rotation);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Static method to rotate a rotator by individual angles
    ///
    /// \param rotator The rotator to rotate
    /// \param pitch Pitch rotation to add (degrees)
    /// \param yaw Yaw rotation to add (degrees)
    /// \param roll Roll rotation to add (degrees)
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void Rotate(
        TRotator<T>& rotator, const T& pitch, const T& yaw, const T& roll
    )
    {
        rotator.Rotate(TRotator<T>(pitch, yaw, roll));
    }
};

///////////////////////////////////////////////////////////////////////////////
// Static Member Initialization
///////////////////////////////////////////////////////////////////////////////
template <typename T>
const TRotator<T> TRotator<T>::Identity = TRotator<T>();

///////////////////////////////////////////////////////////////////////////////
/// \brief Add two rotators
///
/// \tparam T The type of the rotator components
/// \param lhs The left-hand side rotator
/// \param rhs The right-hand side rotator
/// \return The combined rotator
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TRotator<T> operator+(const TRotator<T>& lhs, const TRotator<T>& rhs)
{
    TRotator<T> result = lhs;
    result.Rotate(rhs);   // Use the same logic as operator+

    return result;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Add two rotators
///
/// \tparam T The type of the rotator components
/// \param lhs The left-hand side rotator
/// \param rhs roteate
/// \return The combined rotator
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TRotator<T> operator+(const TRotator<T>& lhs, const T& rhs)
{
    TRotator<T> result = lhs;
    TRotator<T> righthandside = TRotator(rhs, rhs, rhs);
    result.Rotate(righthandside);   // Use the same logic as operator+=

    return result;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Add rotator in place
///
/// \tparam T The type of the rotator components
/// \param lhs The rotator to modify
/// \param rhs The rotator to add
/// \return Reference to the modified rotator
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TRotator<T>& operator+=(TRotator<T>& lhs, const TRotator<T>& rhs)
{
    lhs.Rotate(rhs);

    return lhs;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Check if two rotators are equal
///
/// \tparam T The type of the rotator components
/// \param lhs The left-hand side rotator
/// \param rhs The right-hand side rotator
/// \return True if rotators are equal
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
bool operator==(const TRotator<T>& lhs, const TRotator<T>& rhs)
{
    return lhs.GetPitch() == rhs.GetPitch() && lhs.GetYaw() == rhs.GetYaw() &&
           lhs.GetRoll() == rhs.GetRoll();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Check if two rotators are not equal
///
/// \tparam T The type of the rotator components
/// \param lhs The left-hand side rotator
/// \param rhs The right-hand side rotator
/// \return True if rotators are not equal
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
bool operator!=(const TRotator<T>& lhs, const TRotator<T>& rhs)
{
    return !(lhs == rhs);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Output stream operator for TRotator
///
/// \tparam T The type of the rotator components
/// \param os The output stream
/// \param rotator The rotator to output
/// \return Reference to the output stream
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
std::ostream& operator<<(std::ostream& os, const TRotator<T>& rotator)
{
    os << "Rotator(Pitch: " << rotator.GetPitch()
       << ", Yaw: " << rotator.GetYaw() << ", Roll: " << rotator.GetRoll()
       << ")";

    return os;
}

///////////////////////////////////////////////////////////////////////////////
// Type aliases
///////////////////////////////////////////////////////////////////////////////
using Rotator = TRotator<float>;
using Rotatord = TRotator<double>;

}   // namespace tkd
