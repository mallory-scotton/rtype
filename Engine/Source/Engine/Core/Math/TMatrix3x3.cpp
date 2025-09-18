///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Core/Math/TMatrix3x3.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{
///////////////////////////////////////////////////////////////////////////////
TMatrix3x3<float>::TMatrix3x3(void)
    : m{
        {1.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f}
    }
{};

///////////////////////////////////////////////////////////////////////////////
TMatrix3x3<float>::TMatrix3x3(
    float m00, float m01, float m02,
    float m10, float m11, float m12,
    float m20, float m21, float m22
)
    : m{
        {m00, m01, m02},
        {m10, m11, m12},
        {m20, m21, m22}
    }
{};

///////////////////////////////////////////////////////////////////////////////
float& TMatrix3x3<float>::operator()(std::size_t row, std::size_t col)
{
    return m[row][col];
}

///////////////////////////////////////////////////////////////////////////////
const float& TMatrix3x3<float>::operator()(std::size_t row, std::size_t col) const
{
    return m[row][col];
}

///////////////////////////////////////////////////////////////////////////////
void TMatrix3x3<float>::SetIdentity()
{
    m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f;
    m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f;
    m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f;
}

///////////////////////////////////////////////////////////////////////////////
void TMatrix3x3<float>::SetZero()
{
    m[0][0] = 0.0f; m[0][1] = 0.0f; m[0][2] = 0.0f;
    m[1][0] = 0.0f; m[1][1] = 0.0f; m[1][2] = 0.0f;
    m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 0.0f;
}

///////////////////////////////////////////////////////////////////////////////
float TMatrix3x3<float>::Determinant() const
{
    return
        m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
        m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
        m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
}

///////////////////////////////////////////////////////////////////////////////
TMatrix3x3<float> TMatrix3x3<float>::Transpose() const
{
    return TMatrix3x3<float>(
        m[0][0], m[1][0], m[2][0],
        m[0][1], m[1][1], m[2][1],
        m[0][2], m[1][2], m[2][2]
    );
}

///////////////////////////////////////////////////////////////////////////////
TMatrix3x3<float> TMatrix3x3<float>::Inverse() const
{
    float det = Determinant();
    if (det == 0)
    {
        throw std::runtime_error("Matrix is not invertible");
    }

    float invDet = 1.0f / det;

    return TMatrix3x3<float>(
        (m[1][1] * m[2][2] - m[1][2] * m[2][1]) * invDet,
        (m[0][2] * m[2][1] - m[0][1] * m[2][2]) * invDet,
        (m[0][1] * m[1][2] - m[0][2] * m[1][1]) * invDet,

        (m[1][2] * m[2][0] - m[1][0] * m[2][2]) * invDet,
        (m[0][0] * m[2][2] - m[0][2] * m[2][0]) * invDet,
        (m[0][2] * m[1][0] - m[0][0] * m[1][2]) * invDet,

        (m[1][0] * m[2][1] - m[1][1] * m[2][0]) * invDet,
        (m[0][1] * m[2][0] - m[0][0] * m[2][1]) * invDet,
        (m[0][0] * m[1][1] - m[0][1] * m[1][0]) * invDet
    );
}

///////////////////////////////////////////////////////////////////////////////
TMatrix3x3<float> TMatrix3x3<float>::operator+(const TMatrix3x3<float>& other) const
{
    return TMatrix3x3<float>(
        m[0][0] + other.m[0][0], m[0][1] + other.m[0][1], m[0][2] + other.m[0][2],
        m[1][0] + other.m[1][0], m[1][1] + other.m[1][1], m[1][2] + other.m[1][2],
        m[2][0] + other.m[2][0], m[2][1] + other.m[2][1], m[2][2] + other.m[2][2]
    );
}

///////////////////////////////////////////////////////////////////////////////
TMatrix3x3<float> TMatrix3x3<float>::operator-(const TMatrix3x3<float>& other) const
{
    return TMatrix3x3<float>(
        m[0][0] - other.m[0][0], m[0][1] - other.m[0][1], m[0][2] - other.m[0][2],
        m[1][0] - other.m[1][0], m[1][1] - other.m[1][1], m[1][2] - other.m[1][2],
        m[2][0] - other.m[2][0], m[2][1] - other.m[2][1], m[2][2] - other.m[2][2]
    );
}

///////////////////////////////////////////////////////////////////////////////
TMatrix3x3<float> TMatrix3x3<float>::operator*(const TMatrix3x3<float>& other) const
{
    return TMatrix3x3<float>(
        m[0][0] * other.m[0][0] + m[0][1] * other.m[1][0] + m[0][2] * other.m[2][0],
        m[0][0] * other.m[0][1] + m[0][1] * other.m[1][1] + m[0][2] * other.m[2][1],
        m[0][0] * other.m[0][2] + m[0][1] * other.m[1][2] + m[0][2] * other.m[2][2],

        m[1][0] * other.m[0][0] + m[1][1] * other.m[1][0] + m[1][2] * other.m[2][0],
        m[1][0] * other.m[0][1] + m[1][1] * other.m[1][1] + m[1][2] * other.m[2][1],
        m[1][0] * other.m[0][2] + m[1][1] * other.m[1][2] + m[1][2] * other.m[2][2],

        m[2][0] * other.m[0][0] + m[2][1] * other.m[1][0] + m[2][2] * other.m[2][0],
        m[2][0] * other.m[0][1] + m[2][1] * other.m[1][1] + m[2][2] * other.m[2][1],
        m[2][0] * other.m[0][2] + m[2][1] * other.m[1][2] + m[2][2] * other.m[2][2]
    );
}

///////////////////////////////////////////////////////////////////////////////
TMatrix3x3<float> TMatrix3x3<float>::operator*(float scalar) const
{
    return TMatrix3x3<float>(
        m[0][0] * scalar, m[0][1] * scalar, m[0][2] * scalar,
        m[1][0] * scalar, m[1][1] * scalar, m[1][2] * scalar,
        m[2][0] * scalar, m[2][1] * scalar, m[2][2] * scalar
    );
}

///////////////////////////////////////////////////////////////////////////////
bool TMatrix3x3<float>::operator==(const TMatrix3x3<float>& other) const
{
    return m[0][0] == other.m[0][0] && m[0][1] == other.m[0][1] && m[0][2] == other.m[0][2] &&
           m[1][0] == other.m[1][0] && m[1][1] == other.m[1][1] && m[1][2] == other.m[1][2] &&
           m[2][0] == other.m[2][0] && m[2][1] == other.m[2][1] && m[2][2] == other.m[2][2];
}

///////////////////////////////////////////////////////////////////////////////
bool TMatrix3x3<float>::operator!=(const TMatrix3x3<float>& other) const
{
    return !(*this == other);
}

///////////////////////////////////////////////////////////////////////////////
float TMatrix3x3<float>::Trace() const
{
    return m[0][0] + m[1][1] + m[2][2];
}



}   // namespace tkd