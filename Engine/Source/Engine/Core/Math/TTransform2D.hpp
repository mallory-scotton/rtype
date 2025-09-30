///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <cmath>
#include <Engine/Core/Math/Geometry/TRectangle.hpp>
#include <Engine/Core/Math/TMatrix3x3.hpp>
#include <Engine/Core/Math/TRotator2D.hpp>
#include <Engine/Core/Math/TVector2.hpp>
#include <iostream>
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
    static_assert(
        std::is_arithmetic<T>::value, "T must be an arithmetic type"
    );

public:
    ///////////////////////////////////////////////////////////////////////////
    // Static Members
    ///////////////////////////////////////////////////////////////////////////
    static const TTransform2D Identity;   //<! Identity transform

private:
    ///////////////////////////////////////////////////////////////////////////
    // Member variables
    ///////////////////////////////////////////////////////////////////////////
    TMatrix3x3<T> m_matrix;   ///<! Transformation matrix

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Update matrix from position, rotation, and scale components
    ///
    /// \param position The position component
    /// \param rotation The rotation component
    /// \param scale The scale component
    ///
    ///////////////////////////////////////////////////////////////////////////
    void UpdateMatrix(
        const TVector2<T>& position,
        const TRotator2D<T>& rotation,
        const TVector2<T>& scale
    )
    {
        // Convert angle from degrees to radians
        T angleRad =
            rotation.GetAngle() * static_cast<T>(M_PI) / static_cast<T>(180.0);
        T cosAngle = std::cos(angleRad);
        T sinAngle = std::sin(angleRad);

        // Build transformation matrix: T * R * S
        // [sx*cos  -sy*sin   tx]
        // [sx*sin   sy*cos   ty]
        // [   0        0      1]
        m_matrix = TMatrix3x3<T>(
            scale.x * cosAngle,
            -scale.y * sinAngle,
            position.x,
            scale.x * sinAngle,
            scale.y * cosAngle,
            position.y,
            static_cast<T>(0),
            static_cast<T>(0),
            static_cast<T>(1)
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Extract position from matrix
    ///
    /// \return The position component
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector2<T> ExtractPosition(void) const
    {
        return TVector2<T>(m_matrix(0, 2), m_matrix(1, 2));
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Extract rotation from matrix
    ///
    /// \return The rotation component
    ///
    ///////////////////////////////////////////////////////////////////////////
    TRotator2D<T> ExtractRotation(void) const
    {
        // Extract scale to normalize rotation components
        TVector2<T> scale = ExtractScale();

        // Avoid division by zero
        if (scale.x == static_cast<T>(0) || scale.y == static_cast<T>(0))
        {
            return TRotator2D<T>();
        }

        // Normalize rotation components
        T cosAngle = m_matrix(0, 0) / scale.x;
        T sinAngle = m_matrix(1, 0) / scale.x;

        // Calculate angle from normalized components
        T angleRad = std::atan2(sinAngle, cosAngle);
        T angleDeg = angleRad * static_cast<T>(180.0) / static_cast<T>(M_PI);

        return TRotator2D<T>(angleDeg);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Extract scale from matrix
    ///
    /// \return The scale component
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector2<T> ExtractScale(void) const
    {
        // Calculate scale as magnitude of transformation vectors
        T scaleX = std::sqrt(
            m_matrix(0, 0) * m_matrix(0, 0) + m_matrix(1, 0) * m_matrix(1, 0)
        );
        T scaleY = std::sqrt(
            m_matrix(0, 1) * m_matrix(0, 1) + m_matrix(1, 1) * m_matrix(1, 1)
        );

        return TVector2<T>(scaleX, scaleY);
    }

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor - identity transform
    ///
    ///////////////////////////////////////////////////////////////////////////
    TTransform2D(void)
        : m_matrix(TMatrix3x3<T>::Identity)
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor with position, rotation, and scale
    ///
    /// \param position The position component
    /// \param rotation The rotation component
    /// \param scale The scale component
    ///
    ///////////////////////////////////////////////////////////////////////////
    TTransform2D(
        const TVector2<T>& position,
        const TRotator2D<T>& rotation,
        const TVector2<T>& scale
    )
    {
        UpdateMatrix(position, rotation, scale);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Copy constructor
    ///
    /// \param other The transform to copy from
    ///
    ///////////////////////////////////////////////////////////////////////////
    TTransform2D(const TTransform2D& other)
        : m_matrix(other.m_matrix)
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Move constructor
    ///
    /// \param other The transform to move from
    ///
    ///////////////////////////////////////////////////////////////////////////
    TTransform2D(TTransform2D&& other) noexcept
        : m_matrix(std::move(other.m_matrix))
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
    {
        UpdateMatrix(
            other.GetPosition(), other.GetRotation(), other.GetScale()
        );
    }

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
        if (this != &other) { m_matrix = other.m_matrix; }
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
        if (this != &other) { m_matrix = std::move(other.m_matrix); }
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Multiply this transform with another transform
    ///
    /// \param other The other transform to multiply with
    /// \return The combined transform
    ///
    ///////////////////////////////////////////////////////////////////////////
    TTransform2D<T> operator*(const TTransform2D<T>& other) const
    {
        TTransform2D<T> result;
        result.m_matrix = m_matrix * other.m_matrix;
        return result;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Multiply this transform with another transform in place
    ///
    /// \param other The other transform to multiply with
    /// \return Reference to this transform
    ///
    ///////////////////////////////////////////////////////////////////////////
    TTransform2D<T>& operator*=(const TTransform2D<T>& other)
    {
        m_matrix = m_matrix * other.m_matrix;
        return *this;
    }

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the position component
    ///
    /// \return The position component
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector2<T> GetPosition(void) const { return ExtractPosition(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the rotation component
    ///
    /// \return The rotation component
    ///
    ///////////////////////////////////////////////////////////////////////////
    TRotator2D<T> GetRotation(void) const { return ExtractRotation(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the scale component
    ///
    /// \return The scale component
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector2<T> GetScale(void) const { return ExtractScale(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the position component
    ///
    /// \param position The new position
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetPosition(const TVector2<T>& position)
    {
        UpdateMatrix(position, ExtractRotation(), ExtractScale());
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the rotation component
    ///
    /// \param rotation The new rotation
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetRotation(const TRotator2D<T>& rotation)
    {
        UpdateMatrix(ExtractPosition(), rotation, ExtractScale());
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the scale component
    ///
    /// \param scale The new scale
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetScale(const TVector2<T>& scale)
    {
        UpdateMatrix(ExtractPosition(), ExtractRotation(), scale);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set uniform scale
    ///
    /// \param uniformScale The uniform scale value
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetScale(const T& uniformScale)
    {
        TVector2<T> scale(uniformScale, uniformScale);
        UpdateMatrix(ExtractPosition(), ExtractRotation(), scale);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the transformation matrix
    ///
    /// \return Const reference to the transformation matrix
    ///
    ///////////////////////////////////////////////////////////////////////////
    const TMatrix3x3<T>& GetMatrix(void) const { return m_matrix; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the transformation matrix directly
    ///
    /// \param matrix The new transformation matrix
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetMatrix(const TMatrix3x3<T>& matrix) { m_matrix = matrix; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Transform a point by this transformation
    ///
    /// \param point The point to transform
    ///
    /// \return The transformed point
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector2<T> TransformPoint(const TVector2<T>& point) const
    {
        T x = m_matrix(0, 0) * point.x + m_matrix(0, 1) * point.y +
              m_matrix(0, 2);
        T y = m_matrix(1, 0) * point.x + m_matrix(1, 1) * point.y +
              m_matrix(1, 2);
        return TVector2<T>(x, y);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Transform a vector by this transformation (ignores translation)
    ///
    /// \param vector The vector to transform
    ///
    /// \return The transformed vector
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector2<T> TransformVector(const TVector2<T>& vector) const
    {
        T x = m_matrix(0, 0) * vector.x + m_matrix(0, 1) * vector.y;
        T y = m_matrix(1, 0) * vector.x + m_matrix(1, 1) * vector.y;
        return TVector2<T>(x, y);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Transform a rectangle by this transformation
    ///
    /// \param rect The rectangle to transform
    ///
    /// \return The transformed rectangle
    ///
    ///////////////////////////////////////////////////////////////////////////
    TRectangle<T> TransformRectangle(const TRectangle<T>& rect) const
    {
        TVector2<T> position = TVector2<T>(rect.left, rect.top);
        TVector2<T> corners[4] = { position,
                                   position + TVector2<T>(rect.width, 0),
                                   position + TVector2<T>(0, rect.height),
                                   position +
                                       TVector2<T>(rect.width, rect.height) };

        TRectangle<T> result;
        for (const auto& corner: corners)
        {
            TVector2<T> transformedCorner = TransformPoint(corner);
            result.Expand(transformedCorner);
        }
        return result;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the inverse of this transformation
    ///
    /// \return The inverse transformation
    ///
    ///////////////////////////////////////////////////////////////////////////
    TTransform2D<T> Inverse(void) const
    {
        TTransform2D<T> result;
        result.m_matrix = m_matrix.Inverse();
        return result;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Scale up or down current scale
    ///
    /// \param scaleFactor Multiplier to current scale
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Scale(const T& scaleFactor)
    {
        TVector2<T> currentScale = ExtractScale();
        currentScale *= scaleFactor;
        UpdateMatrix(ExtractPosition(), ExtractRotation(), currentScale);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Static method to scale a transform by scalar
    ///
    /// \param transform The transform to scale
    /// \param scaleFactor Multiplier to current scale
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void Scale(TTransform2D<T>& transform, const T& scaleFactor)
    {
        transform.Scale(scaleFactor);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief scale up or down current scale
    ///
    /// \param scaleFactors Vector multiplier to current scale
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Scale(const TVector2<T>& scaleFactors)
    {
        TVector2<T> currentScale = ExtractScale();
        currentScale *= scaleFactors;
        UpdateMatrix(ExtractPosition(), ExtractRotation(), currentScale);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Static method to scale a transform by vector
    ///
    /// \param transform The transform to scale
    /// \param scaleFactors Vector multiplier to current scale
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void
        Scale(TTransform2D<T>& transform, const TVector2<T>& scaleFactors)
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
        TVector2<T> currentPosition = ExtractPosition();
        currentPosition += translateFactor;
        UpdateMatrix(currentPosition, ExtractRotation(), ExtractScale());
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Static method to translate a transform by scalar
    ///
    /// \param transform The transform to translate
    /// \param translateFactor Scalar addition to current position
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void Translate(TTransform2D<T>& transform, const T& translateFactor)
    {
        transform.Translate(translateFactor);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Change position based off old position
    ///
    /// \param translateFactor vector Addition to current position
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Translate(const TVector2<T>& translateFactor)
    {
        TVector2<T> currentPosition = ExtractPosition();
        currentPosition += translateFactor;
        UpdateMatrix(currentPosition, ExtractRotation(), ExtractScale());
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Static method to translate a transform by vector
    ///
    /// \param transform The transform to translate
    /// \param translateVector Vector addition to current position
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void Translate(
        TTransform2D<T>& transform, const TVector2<T>& translateVector
    )
    {
        transform.Translate(translateVector);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Rotate by given rotation value
    ///
    /// \param rotation The rotation to add to current rotation
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Rotate(const TRotator2D<T>& rotation)
    {
        TRotator2D<T> currentRotation = ExtractRotation();
        currentRotation += rotation;
        UpdateMatrix(ExtractPosition(), currentRotation, ExtractScale());
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Static method to rotate a transform by rotator
    ///
    /// \param transform The transform to rotate
    /// \param rotation The rotation to add to current rotation
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void
        Rotate(TTransform2D<T>& transform, const TRotator2D<T>& rotation)
    {
        transform.Rotate(rotation);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Rotate by angle
    ///
    /// \param angle Angle rotation to add (degrees)
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Rotate(const T& angle)
    {
        TRotator2D<T> currentRotation = ExtractRotation();
        currentRotation += TRotator2D<T>(angle);
        UpdateMatrix(ExtractPosition(), currentRotation, ExtractScale());
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Static method to rotate a transform by angle
    ///
    /// \param transform The transform to rotate
    /// \param angle Angle rotation to add (degrees)
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void Rotate(TTransform2D<T>& transform, const T& angle)
    {
        transform.Rotate(angle);
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
///
/// \param lhs The left-hand side transform
/// \param rhs The right-hand side transform
///
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
///
/// \param lhs The left-hand side transform
/// \param rhs The right-hand side transform
///
/// \return True if transforms are not equal
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
bool operator!=(const TTransform2D<T>& lhs, const TTransform2D<T>& rhs)
{
    return !(lhs == rhs);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Output stream operator for TTransform2D
///
/// \tparam T The type of the transform components
///
/// \param os The output stream
/// \param transform The transform to output
///
/// \return Reference to the output stream
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
std::ostream& operator<<(std::ostream& os, const TTransform2D<T>& transform)
{
    const auto& pos = transform.GetPosition();
    const auto& rot = transform.GetRotation();
    const auto& scale = transform.GetScale();

    os << "Transform2D(Position: (" << pos.x << ", " << pos.y << "), ";
    os << "Rotation: " << rot.GetAngle() << ", ";
    os << "Scale: (" << scale.x << ", " << scale.y << "))";

    return os;
}

}   // namespace tkd
