///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "TVector3.hpp"
#include "TRotator.hpp"
#include <type_traits>
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief 3D transformation class containing position, rotation, and scale
///
/// \tparam T Numeric type (float, double)
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
class TTransform
{

public:
    ///////////////////////////////////////////////////////////////////////////
    // Static Members
    ///////////////////////////////////////////////////////////////////////////
    static const TTransform Identity;   //!< Identity transform

private:
    ///////////////////////////////////////////////////////////////////////////
    // Member variables
    ///////////////////////////////////////////////////////////////////////////
    TVector3<T> m_position;    ///< Position in 3D space
    TRotator<T> m_rotation;    ///< Rotation component
    TVector3<T> m_scale;       ///< Scale component

public:
    static_assert(
        std::is_arithmetic<T>::value, "T must be an arithmetic type"
    );

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor - identity transform
    ///
    ///////////////////////////////////////////////////////////////////////////
    TTransform(void)
        : m_position(TVector3<T>::Zero)
        , m_rotation()
        , m_scale(TVector3<T>::One)
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor with position, rotation, and scale
    ///
    /// \param position The position component
    /// \param rotation The rotation component
    /// \param scale The scale component
    ///
    ///////////////////////////////////////////////////////////////////////////
    TTransform(const TVector3<T>& position, const TRotator<T>& rotation, const TVector3<T>& scale)
        : m_position(position)
        , m_rotation(rotation)
        , m_scale(scale)
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Copy constructor
    ///
    /// \param other The transform to copy from
    ///
    ///////////////////////////////////////////////////////////////////////////
    TTransform(const TTransform& other)
        : m_position(other.m_position)
        , m_rotation(other.m_rotation)
        , m_scale(other.m_scale)
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Move constructor
    ///
    /// \param other The transform to move from
    ///
    ///////////////////////////////////////////////////////////////////////////
    TTransform(TTransform&& other) noexcept
        : m_position(std::move(other.m_position))
        , m_rotation(std::move(other.m_rotation))
        , m_scale(std::move(other.m_scale))
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor from different type transform
    ///
    /// \tparam U The type of the other transform's components
    /// \param other The transform to copy from
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename U>
    TTransform(const TTransform<U>& other)
        : m_position(other.GetPosition())
        , m_rotation(other.GetRotation())
        , m_scale(other.GetScale())
    {}

    public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Copy assignment operator
    ///
    /// \param other The transform to assign from
    /// \return Reference to this transform
    ///
    ///////////////////////////////////////////////////////////////////////////
    TTransform& operator=(const TTransform& other)
    {
        if (this != &other)
        {
            m_position = other.m_position;
            m_rotation = other.m_rotation;
            m_scale = other.m_scale;
        }
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Move assignment operator
    ///
    /// \param other The transform to move from
    /// \return Reference to this transform
    ///
    ///////////////////////////////////////////////////////////////////////////
    TTransform& operator=(TTransform&& other) noexcept
    {
        if (this != &other)
        {
            m_position = std::move(other.m_position);
            m_rotation = std::move(other.m_rotation);
            m_scale = std::move(other.m_scale);
        }
        return *this;
    }

    public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the position component
    ///
    /// \return Const reference to the position
    ///
    ///////////////////////////////////////////////////////////////////////////
    const TVector3<T>& GetPosition(void) const
    {
        return m_position;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the rotation component
    ///
    /// \return Const reference to the rotation
    ///
    ///////////////////////////////////////////////////////////////////////////
    const TRotator<T>& GetRotation(void) const
    {
        return m_rotation;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the scale component
    ///
    /// \return Const reference to the scale
    ///
    ///////////////////////////////////////////////////////////////////////////
    const TVector3<T>& GetScale(void) const
    {
        return m_scale;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the position component
    ///
    /// \param position The new position
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetPosition(const TVector3<T>& position)
    {
        m_position = position;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the rotation component
    ///
    /// \param rotation The new rotation
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetRotation(const TRotator<T>& rotation)
    {
        m_rotation = rotation;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the scale component
    ///
    /// \param scale The new scale
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetScale(const TVector3<T>& scale)
    {
        m_scale = scale;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set uniform scale
    ///
    /// \param uniformScale The uniform scale value
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetScale(const T& uniformScale)
    {
        m_scale = TVector3<T>(uniformScale, uniformScale, uniformScale);
    }

public:
    ///////////////////////////////////////////////////////////////////////////
    // Static utility functions
    ///////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Scale up or down current scale
    ///
    /// \param scaleFactor Multiplier to current scale
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Scale(const T& scaleFactor)
    {
        m_scale *= scaleFactor;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Static method to scale a transform
    ///
    /// \param transform The transform to scale
    /// \param scaleFactor Multiplier to current scale
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void Scale(TTransform<T>& transform, const T& scaleFactor)
    {
        transform.Scale(scaleFactor);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief scale up or down current scale
    ///
    /// \param scaleFactor Vector multiplier to current scale
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Scale(const TVector3<T>& scaleFactors)
    {
        m_scale *= scaleFactors;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Static method to scale a transform
    ///
    /// \param transform The transform to scale
    /// \param scaleFactor Multiplier to current scale
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void Scale(TTransform<T>& transform, const TVector3<T>& scaleFactors)
    {
        transform.Scale(scaleFactors);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Change position based off old position
    ///
    /// \param translateFactor Addition to current position
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Translate(const T& translateFactor)
    {
        m_position += translateFactor;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Static method to translate a transform by scalar
    ///
    /// \param transform The transform to translate
    /// \param translateFactor Scalar addition to current position
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void Translate(TTransform<T>& transform, const T& translateFactor)
    {
        transform.Translate(translateFactor);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Change position based off old position
    ///
    /// \param translateFactor vector Addition to current position
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Translate(const TVector3<T>& translateFactor)
    {
        m_position += translateFactor;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Static method to translate a transform by vector
    ///
    /// \param transform The transform to translate
    /// \param translateVector Vector addition to current position
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void Translate(TTransform<T>& transform, const TVector3<T>& translateVector)
    {
        transform.Translate(translateVector);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Rotate by given rotation values
    ///
    /// \param rotation The rotation to add to current rotation
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Rotate(const TRotator<T>& rotation)
    {
        m_rotation += rotation;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Static method to rotate a transform by rotator
    ///
    /// \param transform The transform to rotate
    /// \param rotation The rotation to add to current rotation
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void Rotate(TTransform<T>& transform, const TRotator<T>& rotation)
    {
        transform.Rotate(rotation);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Rotate by individual Euler angles
    ///
    /// \param pitch Pitch rotation to add (degrees)
    /// \param yaw Yaw rotation to add (degrees)
    /// \param roll Roll rotation to add (degrees)
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Rotate(const T& pitch, const T& yaw, const T& roll)
    {
        m_rotation += TRotator<T>(pitch, yaw, roll);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Static method to rotate a transform by individual angles
    ///
    /// \param transform The transform to rotate
    /// \param pitch Pitch rotation to add (degrees)
    /// \param yaw Yaw rotation to add (degrees)
    /// \param roll Roll rotation to add (degrees)
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void Rotate(TTransform<T>& transform, const T& pitch, const T& yaw, const T& roll)
    {
        transform.Rotate(pitch, yaw, roll);
    }
};

///////////////////////////////////////////////////////////////////////////////
// Static Member Initialization
///////////////////////////////////////////////////////////////////////////////
template <typename T>
const TTransform<T> TTransform<T>::Identity = TTransform<T>();

///////////////////////////////////////////////////////////////////////////////
/// \brief Combine two transforms (this * other)
///
/// \tparam T The type of the transform components
/// \param lhs The left-hand side transform
/// \param rhs The right-hand side transform
/// \return The combined transform
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TTransform<T> operator*(const TTransform<T>& lhs, const TTransform<T>& rhs)
{
    // Transform composition: first apply rhs, then lhs
    TVector3<T> newPosition = lhs.GetPosition() +
        lhs.GetRotation().RotateVector(rhs.GetPosition() *
        lhs.GetScale());
    TRotator<T> newRotation = lhs.GetRotation() * rhs.GetRotation();
    TVector3<T> newScale = lhs.GetScale() * rhs.GetScale();

    return (TTransform<T>(newPosition, newRotation, newScale));
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Combine this transform with another in place
///
/// \tparam T The type of the transform components
/// \param lhs The transform to modify
/// \param rhs The transform to combine with
/// \return Reference to the modified transform
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TTransform<T>& operator*=(TTransform<T>& lhs, const TTransform<T>& rhs)
{
    lhs = lhs * rhs;
    return lhs;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Check if two transforms are equal
///
/// \tparam T The type of the transform components
/// \param lhs The left-hand side transform
/// \param rhs The right-hand side transform
/// \return True if transforms are equal
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
bool operator==(const TTransform<T>& lhs, const TTransform<T>& rhs)
{
    return lhs.GetPosition() == rhs.GetPosition() &&
           lhs.GetRotation() == rhs.GetRotation() &&
           lhs.GetScale() == rhs.GetScale();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Check if two transforms are not equal
///
/// \tparam T The type of the transform components
/// \param lhs The left-hand side transform
/// \param rhs The right-hand side transform
/// \return True if transforms are not equal
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
bool operator!=(const TTransform<T>& lhs, const TTransform<T>& rhs)
{
    return !(lhs == rhs);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Output stream operator for TTransform position
///
/// \tparam T The type of the transform components
/// \param os The output stream
/// \param transform The transform to output position from
/// \return Reference to the output stream
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
std::ostream& operator<<(std::ostream& os, const TTransform<T>& transform)
{
    const auto& pos = transform.GetPosition();
    const auto& rot = transform.GetRotation();
    const auto& scale = transform.GetScale();
    
    os << "Transform(Position: (" << pos.x << ", " << pos.y << ", " << pos.z << "), ";
    os << "Rotation: (" << rot.GetPitch() << ", " << rot.GetYaw() << ", " << rot.GetRoll() << "), ";
    os << "Scale: (" << scale.x << ", " << scale.y << ", " << scale.z << "))";
    
    return os;
}

///////////////////////////////////////////////////////////////////////////////
// Type aliases
///////////////////////////////////////////////////////////////////////////////
using Transform = TTransform<float>;
using Transformd = TTransform<double>;

}   // namespace tkd
