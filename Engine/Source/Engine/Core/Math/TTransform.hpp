///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <cmath>
#include <Engine/Core/Math/TMatrix4x4.hpp>
#include <Engine/Core/Math/TRotator.hpp>
#include <Engine/Core/Math/TTransform2D.hpp>
#include <Engine/Core/Math/TVector3.hpp>
#include <iostream>
#include <type_traits>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief 3D transformation class using 4x4 matrix representation
///
/// This class uses a 4x4 transformation matrix internally to represent
/// position, rotation, and scale. The matrix is stored in row-major order.
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
    T m_matrix[4][4];   ///< 4x4 transformation matrix (row-major)

public:
    static_assert(
        std::is_arithmetic<T>::value, "T must be an arithmetic type"
    );

private:
    ///////////////////////////////////////////////////////////////////////////
    // Helper Methods
    ///////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Decompose matrix into position, rotation, and scale
    ///
    /// \param outPosition Output position vector
    /// \param outRotation Output rotation angles
    /// \param outScale Output scale vector
    ///
    ///////////////////////////////////////////////////////////////////////////
    void DecomposeMatrix(
        TVector3<T>& outPosition,
        TRotator<T>& outRotation,
        TVector3<T>& outScale
    ) const
    {
        // Extract position (last column)
        outPosition =
            TVector3<T>(m_matrix[0][3], m_matrix[1][3], m_matrix[2][3]);

        // Extract scale from the length of the basis vectors
        TVector3<T> scaleX(m_matrix[0][0], m_matrix[1][0], m_matrix[2][0]);
        TVector3<T> scaleY(m_matrix[0][1], m_matrix[1][1], m_matrix[2][1]);
        TVector3<T> scaleZ(m_matrix[0][2], m_matrix[1][2], m_matrix[2][2]);

        outScale.x = scaleX.Length();
        outScale.y = scaleY.Length();
        outScale.z = scaleZ.Length();

        // Handle zero scale to avoid division by zero
        if (outScale.x == T(0)) { outScale.x = T(1); }
        if (outScale.y == T(0)) { outScale.y = T(1); }
        if (outScale.z == T(0)) { outScale.z = T(1); }

        // Extract rotation by removing scale from the rotation matrix
        T r00 = m_matrix[0][0] / outScale.x;
        T r10 = m_matrix[1][0] / outScale.x;
        T r20 = m_matrix[2][0] / outScale.x;
        T r21 = m_matrix[2][1] / outScale.y;
        T r22 = m_matrix[2][2] / outScale.z;

        // For gimbal lock detection and calculation
        T r11 = m_matrix[1][1] / outScale.y;
        T r12 = m_matrix[1][2] / outScale.z;

        // Convert rotation matrix to Euler angles (ZYX order)
        T pitch, yaw, roll;

        if (r20 < T(1))
        {
            if (r20 > T(-1))
            {
                pitch = std::asin(-r20);
                yaw = std::atan2(r10, r00);
                roll = std::atan2(r21, r22);
            }
            else
            {
                // Gimbal lock case: r20 = -1
                pitch = static_cast<T>(M_PI) / T(2);
                yaw = -std::atan2(-r12, r11);
                roll = T(0);
            }
        }
        else
        {
            // Gimbal lock case: r20 = 1
            pitch = -static_cast<T>(M_PI) / T(2);
            yaw = std::atan2(-r12, r11);
            roll = T(0);
        }

        // Convert from radians to degrees
        pitch = pitch * T(180) / static_cast<T>(M_PI);
        yaw = yaw * T(180) / static_cast<T>(M_PI);
        roll = roll * T(180) / static_cast<T>(M_PI);

        outRotation = TRotator<T>(pitch, yaw, roll);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Build transformation matrix from components
    ///
    /// \param position Position vector
    /// \param rotation Rotation angles
    /// \param scale Scale vector
    ///
    ///////////////////////////////////////////////////////////////////////////
    void BuildMatrix(
        const TVector3<T>& position,
        const TRotator<T>& rotation,
        const TVector3<T>& scale
    )
    {
        // Convert rotation from degrees to radians
        T pitchRad = rotation.GetPitch() * static_cast<T>(M_PI) / T(180);
        T yawRad = rotation.GetYaw() * static_cast<T>(M_PI) / T(180);
        T rollRad = rotation.GetRoll() * static_cast<T>(M_PI) / T(180);

        // Calculate trigonometric values
        T cp = std::cos(pitchRad);
        T sp = std::sin(pitchRad);
        T cy = std::cos(yawRad);
        T sy = std::sin(yawRad);
        T cr = std::cos(rollRad);
        T sr = std::sin(rollRad);

        // Build rotation matrix (ZYX Euler angles)
        // Combined rotation: Rz(yaw) * Ry(pitch) * Rx(roll)
        m_matrix[0][0] = (cy * cp) * scale.x;
        m_matrix[0][1] = (cy * sp * sr - sy * cr) * scale.y;
        m_matrix[0][2] = (cy * sp * cr + sy * sr) * scale.z;
        m_matrix[0][3] = position.x;

        m_matrix[1][0] = (sy * cp) * scale.x;
        m_matrix[1][1] = (sy * sp * sr + cy * cr) * scale.y;
        m_matrix[1][2] = (sy * sp * cr - cy * sr) * scale.z;
        m_matrix[1][3] = position.y;

        m_matrix[2][0] = (-sp) * scale.x;
        m_matrix[2][1] = (cp * sr) * scale.y;
        m_matrix[2][2] = (cp * cr) * scale.z;
        m_matrix[2][3] = position.z;

        m_matrix[3][0] = T(0);
        m_matrix[3][1] = T(0);
        m_matrix[3][2] = T(0);
        m_matrix[3][3] = T(1);
    }

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor - identity transform
    ///
    ///////////////////////////////////////////////////////////////////////////
    TTransform(void)
    {
        // Initialize to identity matrix
        m_matrix[0][0] = T(1);
        m_matrix[0][1] = T(0);
        m_matrix[0][2] = T(0);
        m_matrix[0][3] = T(0);
        m_matrix[1][0] = T(0);
        m_matrix[1][1] = T(1);
        m_matrix[1][2] = T(0);
        m_matrix[1][3] = T(0);
        m_matrix[2][0] = T(0);
        m_matrix[2][1] = T(0);
        m_matrix[2][2] = T(1);
        m_matrix[2][3] = T(0);
        m_matrix[3][0] = T(0);
        m_matrix[3][1] = T(0);
        m_matrix[3][2] = T(0);
        m_matrix[3][3] = T(1);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor with position, rotation, and scale
    ///
    /// \param position The position component
    /// \param rotation The rotation component
    /// \param scale The scale component
    ///
    ///////////////////////////////////////////////////////////////////////////
    TTransform(
        const TVector3<T>& position,
        const TRotator<T>& rotation,
        const TVector3<T>& scale
    )
    {
        BuildMatrix(position, rotation, scale);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor from 4x4 matrix
    ///
    /// \param matrix The 4x4 transformation matrix
    ///
    ///////////////////////////////////////////////////////////////////////////
    explicit TTransform(const TMatrix4x4<T>& matrix)
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++) { m_matrix[i][j] = matrix.data[i][j]; }
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor from raw matrix data
    ///
    /// \param matrixData Raw 4x4 matrix array
    ///
    ///////////////////////////////////////////////////////////////////////////
    explicit TTransform(const T matrixData[4][4])
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++) { m_matrix[i][j] = matrixData[i][j]; }
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Copy constructor
    ///
    /// \param other The transform to copy from
    ///
    ///////////////////////////////////////////////////////////////////////////
    TTransform(const TTransform& other)
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                m_matrix[i][j] = other.m_matrix[i][j];
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Move constructor
    ///
    /// \param other The transform to move from
    ///
    ///////////////////////////////////////////////////////////////////////////
    TTransform(TTransform&& other) noexcept
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                m_matrix[i][j] = other.m_matrix[i][j];
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor from different type transform
    ///
    /// \tparam U The type of the other transform's components
    /// \param other The transform to copy from
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename U>
    TTransform(const TTransform<U>& other)
    {
        TVector3<U> pos = other.GetPosition();
        TRotator<U> rot = other.GetRotation();
        TVector3<U> scl = other.GetScale();

        BuildMatrix(
            TVector3<T>(
                static_cast<T>(pos.x),
                static_cast<T>(pos.y),
                static_cast<T>(pos.z)
            ),
            TRotator<T>(
                static_cast<T>(rot.GetPitch()),
                static_cast<T>(rot.GetYaw()),
                static_cast<T>(rot.GetRoll())
            ),
            TVector3<T>(
                static_cast<T>(scl.x),
                static_cast<T>(scl.y),
                static_cast<T>(scl.z)
            )
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
    TTransform& operator=(const TTransform& other)
    {
        if (this != &other)
        {
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    m_matrix[i][j] = other.m_matrix[i][j];
                }
            }
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
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    m_matrix[i][j] = other.m_matrix[i][j];
                }
            }
        }
        return *this;
    }

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the position component
    ///
    /// \return The position vector
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector3<T> GetPosition(void) const
    {
        return TVector3<T>(m_matrix[0][3], m_matrix[1][3], m_matrix[2][3]);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the rotation component
    ///
    /// \return The rotation rotator
    ///
    ///////////////////////////////////////////////////////////////////////////
    TRotator<T> GetRotation(void) const
    {
        TVector3<T> pos, scale;
        TRotator<T> rot;
        DecomposeMatrix(pos, rot, scale);
        return rot;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the scale component
    ///
    /// \return The scale vector
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector3<T> GetScale(void) const
    {
        TVector3<T> pos, scale;
        TRotator<T> rot;
        DecomposeMatrix(pos, rot, scale);
        return scale;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the position component
    ///
    /// \param position The new position
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetPosition(const TVector3<T>& position)
    {
        m_matrix[0][3] = position.x;
        m_matrix[1][3] = position.y;
        m_matrix[2][3] = position.z;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the rotation component
    ///
    /// \param rotation The new rotation
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetRotation(const TRotator<T>& rotation)
    {
        TVector3<T> pos = GetPosition();
        TVector3<T> scale = GetScale();
        BuildMatrix(pos, rotation, scale);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the scale component
    ///
    /// \param scale The new scale
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetScale(const TVector3<T>& scale)
    {
        TVector3<T> pos = GetPosition();
        TRotator<T> rot = GetRotation();
        BuildMatrix(pos, rot, scale);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set uniform scale
    ///
    /// \param uniformScale The uniform scale value
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetScale(const T& uniformScale)
    {
        SetScale(TVector3<T>(uniformScale, uniformScale, uniformScale));
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the transformation matrix
    ///
    /// \return The 4x4 transformation matrix
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMatrix4x4<T> GetMatrix(void) const
    {
        TMatrix4x4<T> result;
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++) { result.data[i][j] = m_matrix[i][j]; }
        }
        return result;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the transformation matrix directly
    ///
    /// \param matrix The new transformation matrix
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetMatrix(const TMatrix4x4<T>& matrix)
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++) { m_matrix[i][j] = matrix.data[i][j]; }
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Access matrix element (mutable)
    ///
    /// \param row Row index (0-3)
    /// \param col Column index (0-3)
    /// \return Reference to matrix element
    ///
    ///////////////////////////////////////////////////////////////////////////
    T& operator()(int row, int col) { return m_matrix[row][col]; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Access matrix element (const)
    ///
    /// \param row Row index (0-3)
    /// \param col Column index (0-3)
    /// \return Const reference to matrix element
    ///
    ///////////////////////////////////////////////////////////////////////////
    const T& operator()(int row, int col) const { return m_matrix[row][col]; }

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Scale up or down current scale
    ///
    /// \param scaleFactor Multiplier to current scale
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Scale(const T& scaleFactor)
    {
        TVector3<T> currentScale = GetScale();
        SetScale(currentScale * scaleFactor);
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
        TVector3<T> currentScale = GetScale();
        SetScale(currentScale * scaleFactors);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Static method to scale a transform
    ///
    /// \param transform The transform to scale
    /// \param scaleFactor Multiplier to current scale
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void
        Scale(TTransform<T>& transform, const TVector3<T>& scaleFactors)
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
        m_matrix[0][3] += translateFactor;
        m_matrix[1][3] += translateFactor;
        m_matrix[2][3] += translateFactor;
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
        m_matrix[0][3] += translateFactor.x;
        m_matrix[1][3] += translateFactor.y;
        m_matrix[2][3] += translateFactor.z;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Static method to translate a transform by vector
    ///
    /// \param transform The transform to translate
    /// \param translateVector Vector addition to current position
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void
        Translate(TTransform<T>& transform, const TVector3<T>& translateVector)
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
        TRotator<T> currentRotation = GetRotation();
        SetRotation(currentRotation + rotation);
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
        Rotate(TRotator<T>(pitch, yaw, roll));
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
    static void Rotate(
        TTransform<T>& transform, const T& pitch, const T& yaw, const T& roll
    )
    {
        transform.Rotate(pitch, yaw, roll);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Transform a point by this transformation
    ///
    /// \param point The point to transform
    /// \return The transformed point
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector3<T> TransformPoint(const TVector3<T>& point) const
    {
        T x = m_matrix[0][0] * point.x + m_matrix[0][1] * point.y +
              m_matrix[0][2] * point.z + m_matrix[0][3];
        T y = m_matrix[1][0] * point.x + m_matrix[1][1] * point.y +
              m_matrix[1][2] * point.z + m_matrix[1][3];
        T z = m_matrix[2][0] * point.x + m_matrix[2][1] * point.y +
              m_matrix[2][2] * point.z + m_matrix[2][3];

        return TVector3<T>(x, y, z);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Transform a direction by this transformation (ignores
    /// translation)
    ///
    /// \param direction The direction to transform
    /// \return The transformed direction
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector3<T> TransformDirection(const TVector3<T>& direction) const
    {
        T x = m_matrix[0][0] * direction.x + m_matrix[0][1] * direction.y +
              m_matrix[0][2] * direction.z;
        T y = m_matrix[1][0] * direction.x + m_matrix[1][1] * direction.y +
              m_matrix[1][2] * direction.z;
        T z = m_matrix[2][0] * direction.x + m_matrix[2][1] * direction.y +
              m_matrix[2][2] * direction.z;

        return TVector3<T>(x, y, z);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the inverse of this transformation
    ///
    /// \return The inverse transform
    ///
    ///////////////////////////////////////////////////////////////////////////
    TTransform<T> Inverse(void) const
    {
        TMatrix4x4<T> mat = GetMatrix();
        TMatrix4x4<T> invMat = mat.Inverse();
        return TTransform<T>(invMat);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Compute the delta (difference) between two transforms
    ///
    /// This computes transform = this^-1 * other
    ///
    /// \param other The target transform
    /// \return The relative transform from this to other
    ///
    ///////////////////////////////////////////////////////////////////////////
    TTransform<T> GetDelta(const TTransform<T>& other) const
    {
        return Inverse() * other;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Linearly interpolate between two transforms
    ///
    /// \param other The target transform
    /// \param alpha Interpolation factor (0-1)
    /// \return The interpolated transform
    ///
    ///////////////////////////////////////////////////////////////////////////
    TTransform<T> Lerp(const TTransform<T>& other, T alpha) const
    {
        // Decompose both transforms
        TVector3<T> pos1 = GetPosition();
        TRotator<T> rot1 = GetRotation();
        TVector3<T> scale1 = GetScale();

        TVector3<T> pos2 = other.GetPosition();
        TRotator<T> rot2 = other.GetRotation();
        TVector3<T> scale2 = other.GetScale();

        // Interpolate components
        TVector3<T> lerpPos = pos1 + (pos2 - pos1) * alpha;
        TVector3<T> lerpScale = scale1 + (scale2 - scale1) * alpha;

        // Simple lerp for rotation (not slerp)
        T lerpPitch =
            rot1.GetPitch() + (rot2.GetPitch() - rot1.GetPitch()) * alpha;
        T lerpYaw = rot1.GetYaw() + (rot2.GetYaw() - rot1.GetYaw()) * alpha;
        T lerpRoll =
            rot1.GetRoll() + (rot2.GetRoll() - rot1.GetRoll()) * alpha;

        return TTransform<T>(
            lerpPos, TRotator<T>(lerpPitch, lerpYaw, lerpRoll), lerpScale
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Convert to 2D transform (drops Z component)
    ///
    /// \return The resulting 2D transform
    ///
    ///////////////////////////////////////////////////////////////////////////
    TTransform2D<T> To2D(void) const
    {
        TVector3<T> pos = GetPosition();
        TRotator<T> rot = GetRotation();
        TVector3<T> scale = GetScale();

        return TTransform2D<T>(
            TVector2<T>(pos.x, pos.y),
            TRotator2D<T>(rot.GetYaw()),
            TVector2<T>(scale.x, scale.y)
        );
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
/// Matrix multiplication: applies other transform first, then this transform
///
/// \tparam T The type of the transform components
/// \param lhs The left-hand side transform
/// \param rhs The right-hand side transform
///
/// \return The combined transform
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
TTransform<T> operator*(const TTransform<T>& lhs, const TTransform<T>& rhs)
{
    TMatrix4x4<T> lhsMatrix = lhs.GetMatrix();
    TMatrix4x4<T> rhsMatrix = rhs.GetMatrix();
    TMatrix4x4<T> result = lhsMatrix * rhsMatrix;
    return TTransform<T>(result);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Combine this transform with another in place
///
/// \tparam T The type of the transform components
/// \param lhs The transform to modify
/// \param rhs The transform to combine with
///
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
///
/// \return True if transforms are equal
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
bool operator==(const TTransform<T>& lhs, const TTransform<T>& rhs)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (lhs(i, j) != rhs(i, j)) { return false; }
        }
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Check if two transforms are not equal
///
/// \tparam T The type of the transform components
/// \param lhs The left-hand side transform
/// \param rhs The right-hand side transform
///
/// \return True if transforms are not equal
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
bool operator!=(const TTransform<T>& lhs, const TTransform<T>& rhs)
{
    return !(lhs == rhs);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Output stream operator for TTransform
///
/// \tparam T The type of the transform components
/// \param os The output stream
/// \param transform The transform to output
/// \return Reference to the output stream
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
std::ostream& operator<<(std::ostream& os, const TTransform<T>& transform)
{
    TVector3<T> pos = transform.GetPosition();
    TRotator<T> rot = transform.GetRotation();
    TVector3<T> scale = transform.GetScale();

    os << "Transform(Position: (" << pos.x << ", " << pos.y << ", " << pos.z
       << "), ";
    os << "Rotation: (" << rot.GetPitch() << ", " << rot.GetYaw() << ", "
       << rot.GetRoll() << "), ";
    os << "Scale: (" << scale.x << ", " << scale.y << ", " << scale.z << "))";

    return os;
}

///////////////////////////////////////////////////////////////////////////////
// Type aliases
///////////////////////////////////////////////////////////////////////////////
using Transform = TTransform<float>;
using Transformd = TTransform<double>;

}   // namespace tkd
