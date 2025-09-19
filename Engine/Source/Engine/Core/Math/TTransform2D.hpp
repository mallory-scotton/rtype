///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "TVector2.hpp"
#include "TRotator2D.hpp"
#include <type_traits>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief 2D transformation class containing position, rotation, and scale
///
/// \tparam T Numeric type (float, double)
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
class TTransform2D
{

public:
    ///////////////////////////////////////////////////////////////////////////
    // Static Members
    ///////////////////////////////////////////////////////////////////////////
    static const TTransform2D Identity;   //!< Identity transform

private:
    ///////////////////////////////////////////////////////////////////////////
    // Member variables
    ///////////////////////////////////////////////////////////////////////////
    TVector2<T> m_position;    ///< Position in 2D space
    TRotator2D<T> m_rotation;  ///< Rotation component
    TVector2<T> m_scale;       ///< Scale component

public:
    static_assert(
        std::is_arithmetic<T>::value, "T must be an arithmetic type"
    );

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor - identity transform
    ///
    ///////////////////////////////////////////////////////////////////////////
    TTransform2D(void)
        : m_position(TVector2<T>::Zero)
        , m_rotation()
        , m_scale(TVector2<T>::One)
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor with position, rotation, and scale
    ///
    /// \param position The position component
    /// \param rotation The rotation component
    /// \param scale The scale component
    ///
    ///////////////////////////////////////////////////////////////////////////
    TTransform2D(const TVector2<T>& position, const TRotator2D<T>& rotation, const TVector2<T>& scale)
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
    TTransform2D(const TTransform2D& other)
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
    TTransform2D(TTransform2D&& other) noexcept
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
    TTransform2D(const TTransform2D<U>& other)
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
    TTransform2D& operator=(const TTransform2D& other)
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
    TTransform2D& operator=(TTransform2D&& other) noexcept
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
    const TVector2<T>& GetPosition(void) const
    {
        return m_position;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the rotation component
    ///
    /// \return Const reference to the rotation
    ///
    ///////////////////////////////////////////////////////////////////////////
    const TRotator2D<T>& GetRotation(void) const
    {
        return m_rotation;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the scale component
    ///
    /// \return Const reference to the scale
    ///
    ///////////////////////////////////////////////////////////////////////////
    const TVector2<T>& GetScale(void) const
    {
        return m_scale;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the position component
    ///
    /// \param position The new position
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetPosition(const TVector2<T>& position)
    {
        m_position = position;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the rotation component
    ///
    /// \param rotation The new rotation
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetRotation(const TRotator2D<T>& rotation)
    {
        m_rotation = rotation;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the scale component
    ///
    /// \param scale The new scale
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetScale(const TVector2<T>& scale)
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
        m_scale = TVector2<T>(uniformScale, uniformScale);
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
    /// \brief scale up or down current scale
    ///
    /// \param scaleFactor Vector multiplier to current scale
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Scale(const TVector2<T>& scaleFactors)
    {
        m_scale *= scaleFactors;
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
    /// \brief Change position based off old position
    ///
    /// \param translateFactor vector Addition to current position
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Translate(const TVector2<T>& translateFactor)
    {
        m_position += translateFactor;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Rotate by given rotation value
    ///
    /// \param rotation The rotation to add to current rotation
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Rotate(const TRotator2D<T>& rotation)
    {
        m_rotation += rotation;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Rotate by angle
    ///
    /// \param angle Angle rotation to add (degrees)
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Rotate(const T& angle)
    {
        m_rotation += TRotator2D<T>(angle);
    }
};

///////////////////////////////////////////////////////////////////////////////
// Static Member Initialization
///////////////////////////////////////////////////////////////////////////////
template <typename T>
const TTransform2D<T> TTransform2D<T>::Identity = TTransform2D<T>();

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
bool operator==(const TTransform2D<T>& lhs, const TTransform2D<T>& rhs)
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
bool operator!=(const TTransform2D<T>& lhs, const TTransform2D<T>& rhs)
{
    return !(lhs == rhs);
}

///////////////////////////////////////////////////////////////////////////////
// Type aliases
///////////////////////////////////////////////////////////////////////////////
using Transform2D = TTransform2D<float>;
using Transform2Dd = TTransform2D<double>;

}   // namespace tkd
