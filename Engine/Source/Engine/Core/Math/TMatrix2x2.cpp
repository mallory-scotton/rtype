///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Core/Math/TMatrix2x2.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
TMatrix2x2<float>::TMatrix2x2(void)
    : m{
        {1.0f, 0.0f},
        {0.0f, 1.0f}
    }
{}

///////////////////////////////////////////////////////////////////////////////
TMatrix2x2<float>::TMatrix2x2(
    float m00, float m01,
    float m10, float m11
)
    : m{
        {m00, m01},
        {m10, m11}
    }
{}

///////////////////////////////////////////////////////////////////////////////
float& TMatrix2x2<float>::operator()(std::size_t row, std::size_t col)
{
    return m[row][col];
}

///////////////////////////////////////////////////////////////////////////////
const float& TMatrix2x2<float>::operator()(std::size_t row, std::size_t col) const
{
    return m[row][col];
}

///////////////////////////////////////////////////////////////////////////////
void TMatrix2x2<float>::SetIdentity()
{
    m[0][0] = 1.0f; m[0][1] = 0.0f;
    m[1][0] = 0.0f; m[1][1] = 1.0f;
}

///////////////////////////////////////////////////////////////////////////////
void TMatrix2x2<float>::SetZero()
{
    m[0][0] = 0.0f; m[0][1] = 0.0f;
    m[1][0] = 0.0f; m[1][1] = 0.0f;
}

///////////////////////////////////////////////////////////////////////////////
float TMatrix2x2<float>::Determinant() const
{
    return m[0][0] * m[1][1] - m[0][1] * m[1][0];
}

///////////////////////////////////////////////////////////////////////////////
TMatrix2x2<float> TMatrix2x2<float>::Transpose() const
{
    return TMatrix2x2<float>(
        m[0][0], m[1][0],
        m[0][1], m[1][1]
    );
}

///////////////////////////////////////////////////////////////////////////////
TMatrix2x2<float> TMatrix2x2<float>::Inverse() const
{
    float det = Determinant();
    if (det == 0)
    {
        throw std::runtime_error("Matrix is not invertible");
    }

    float invDet = 1.0f / det;
    return TMatrix2x2<float>(
        m[1][1] * invDet, -m[0][1] * invDet,
        -m[1][0] * invDet, m[0][0] * invDet
    );
}

///////////////////////////////////////////////////////////////////////////////
TMatrix2x2<float> TMatrix2x2<float>::operator+(const TMatrix2x2<float>& other) const
{
    return TMatrix2x2<float>(
        m[0][0] + other.m[0][0], m[0][1] + other.m[0][1],
        m[1][0] + other.m[1][0], m[1][1] + other.m[1][1]
    );
}

///////////////////////////////////////////////////////////////////////////////
TMatrix2x2<float> TMatrix2x2<float>::operator-(const TMatrix2x2<float>& other) const
{
    return TMatrix2x2<float>(
        m[0][0] - other.m[0][0], m[0][1] - other.m[0][1],
        m[1][0] - other.m[1][0], m[1][1] - other.m[1][1]
    );
}

///////////////////////////////////////////////////////////////////////////////
TMatrix2x2<float> TMatrix2x2<float>::operator*(const TMatrix2x2<float>& other) const
{
    return TMatrix2x2<float>(
        m[0][0] * other.m[0][0] + m[0][1] * other.m[1][0],
        m[0][0] * other.m[0][1] + m[0][1] * other.m[1][1],
        m[1][0] * other.m[0][0] + m[1][1] * other.m[1][0],
        m[1][0] * other.m[0][1] + m[1][1] * other.m[1][1]
    );
}

///////////////////////////////////////////////////////////////////////////////
TMatrix2x2<float> TMatrix2x2<float>::operator*(float scalar) const
{
    return TMatrix2x2<float>(
        m[0][0] * scalar, m[0][1] * scalar,
        m[1][0] * scalar, m[1][1] * scalar
    );
}

///////////////////////////////////////////////////////////////////////////////
bool TMatrix2x2<float>::operator==(const TMatrix2x2<float>& other) const
{
    return m[0][0] == other.m[0][0] && m[0][1] == other.m[0][1] &&
           m[1][0] == other.m[1][0] && m[1][1] == other.m[1][1];
}

///////////////////////////////////////////////////////////////////////////////
bool TMatrix2x2<float>::operator!=(const TMatrix2x2<float>& other) const
{
    return !(*this == other);
}

///////////////////////////////////////////////////////////////////////////////
float TMatrix2x2<float>::Trace() const
{
    return m[0][0] + m[1][1];
}

}   // namespace tkd
