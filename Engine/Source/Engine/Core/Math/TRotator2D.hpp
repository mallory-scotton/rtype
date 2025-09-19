///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <type_traits>
#include <cmath>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief 2D rotation class using single angle
///
/// \tparam T Numeric type (float, double)
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
class TRotator2D
{
public:
    static_assert(
        std::is_arithmetic<T>::value, "T must be an arithmetic type"
    );

public:
    ///////////////////////////////////////////////////////////////////////////
    // Static Members
    ///////////////////////////////////////////////////////////////////////////
    static const TRotator2D Identity;   //!< Identity rotator (0)

private:
    ///////////////////////////////////////////////////////////////////////////
    // Member variables
    ///////////////////////////////////////////////////////////////////////////
    T m_angle;    ///< Rotation angle (degrees)

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
            normalized -= static_cast<T>(360);
        else if (normalized <= static_cast<T>(-180))
            normalized += static_cast<T>(360);
        return normalized;
    }

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor - identity rotation
    ///
    ///////////////////////////////////////////////////////////////////////////
    TRotator2D(void)
        : m_angle(static_cast<T>(0))
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor with angle value
    ///
    /// \param angle Rotation angle (degrees)
    ///
    ///////////////////////////////////////////////////////////////////////////
    TRotator2D(const T& angle)
        : m_angle(NormalizeAngle(angle))
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Copy constructor
    ///
    /// \param other The rotator to copy from
    ///
    ///////////////////////////////////////////////////////////////////////////
    TRotator2D(const TRotator2D& other)
        : m_angle(other.m_angle)
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Move constructor
    ///
    /// \param other The rotator to move from
    ///
    ///////////////////////////////////////////////////////////////////////////
    TRotator2D(TRotator2D&& other) noexcept
        : m_angle(other.m_angle)
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor from different type rotator
    ///
    /// \tparam U The type of the other rotator's components
    /// \param other The rotator to copy from
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename U>
    TRotator2D(const TRotator2D<U>& other)
        : m_angle(static_cast<T>(other.GetAngle()))
    {}

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Copy assignment operator
    ///
    /// \param other The rotator to assign from
    /// \return Reference to this rotator
    ///
    ///////////////////////////////////////////////////////////////////////////
    TRotator2D& operator=(const TRotator2D& other)
    {
        if (this != &other)
        {
            m_angle = other.m_angle;
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
    TRotator2D& operator=(TRotator2D&& other) noexcept
    {
        if (this != &other)
        {
            m_angle = other.m_angle;
        }
        return *this;
    }

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the angle component
    ///
    /// \return The angle value in degrees
    ///
    ///////////////////////////////////////////////////////////////////////////
    const T& GetAngle(void) const
    {
        return m_angle;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the angle component
    ///
    /// \param angle The new angle value in degrees
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetAngle(const T& angle)
    {
        m_angle = NormalizeAngle(angle);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Check if this is an identity rotation
    ///
    /// \return True if angle is zero
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool IsIdentity(void) const
    {
        return m_angle == static_cast<T>(0);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Reset to identity rotation
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Reset(void)
    {
        m_angle = static_cast<T>(0);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Check equality with tolerance
    ///
    /// \param other The other rotator to compare with
    /// \param tolerance The tolerance for comparison
    /// \return True if rotators are equal within tolerance
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool Equals(const TRotator2D& other, const T& tolerance = static_cast<T>(1e-6)) const
    {
        return std::abs(m_angle - other.m_angle) <= tolerance;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Rotate by given rotator values
    ///
    /// \param rotator The rotation to add
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Rotate(const TRotator2D& rotator)
    {
        m_angle = NormalizeAngle(m_angle + rotator.m_angle);
    }

};

///////////////////////////////////////////////////////////////////////////////
// Static Member Initialization
///////////////////////////////////////////////////////////////////////////////
template <typename T>
const TRotator2D<T> TRotator2D<T>::Identity = TRotator2D<T>();

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
TRotator2D<T> operator+(const TRotator2D<T>& lhs, const TRotator2D<T>& rhs)
{
    TRotator2D<T> result = lhs;
    result.Rotate(rhs);  // Use the same logic as operator+=
    return result;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Add scalar to rotator
///
/// \tparam T The type of the rotator components
/// \param lhs The left-hand side rotator
/// \param rhs angle value to add
/// \return The combined rotator
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TRotator2D<T> operator+(const TRotator2D<T>& lhs, const T& rhs)
{
    TRotator2D<T> result = lhs;
    TRotator2D<T> righthandside = TRotator2D<T>(rhs);
    result.Rotate(righthandside);  // Use the same logic as operator+=

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
TRotator2D<T>& operator+=(TRotator2D<T>& lhs, const TRotator2D<T>& rhs)
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
bool operator==(const TRotator2D<T>& lhs, const TRotator2D<T>& rhs)
{
    return (lhs.GetAngle() == rhs.GetAngle());
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
bool operator!=(const TRotator2D<T>& lhs, const TRotator2D<T>& rhs)
{
    return !(lhs == rhs);
}

///////////////////////////////////////////////////////////////////////////////
// Type aliases
///////////////////////////////////////////////////////////////////////////////
using Rotator2D = TRotator2D<float>;
using Rotator2Dd = TRotator2D<double>;

}   // namespace tkd
