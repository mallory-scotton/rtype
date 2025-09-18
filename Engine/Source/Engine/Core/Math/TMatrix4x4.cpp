///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Core/Math/TMatrix4x4.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{
///////////////////////////////////////////////////////////////////////////////
TMatrix4x4<float>::TMatrix4x4(void)
    : m{
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    }
{};

///////////////////////////////////////////////////////////////////////////////
TMatrix4x4<float>::TMatrix4x4(
    float m00, float m01, float m02, float m03,
    float m10, float m11, float m12, float m13,
    float m20, float m21, float m22, float m23,
    float m30, float m31, float m32, float m33
)
    : m{
        {m00, m01, m02, m03},
        {m10, m11, m12, m13},
        {m20, m21, m22, m23},
        {m30, m31, m32, m33}
    }
{};

///////////////////////////////////////////////////////////////////////////////
float& TMatrix4x4<float>::operator()(std::size_t row, std::size_t col)
{
    return m[row][col];
}

///////////////////////////////////////////////////////////////////////////////
const float& TMatrix4x4<float>::operator()(std::size_t row, std::size_t col) const
{
    return m[row][col];
}

///////////////////////////////////////////////////////////////////////////////
void TMatrix4x4<float>::SetIdentity()
{
    m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
    m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = 0.0f;
    m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = 0.0f;
    m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
}

///////////////////////////////////////////////////////////////////////////////
void TMatrix4x4<float>::SetZero()
{
    m[0][0] = 0.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
    m[1][0] = 0.0f; m[1][1] = 0.0f; m[1][2] = 0.0f; m[1][3] = 0.0f;
    m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 0.0f; m[2][3] = 0.0f;
    m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 0.0f;
}

///////////////////////////////////////////////////////////////////////////////
float TMatrix4x4<float>::Determinant() const
{
    float det = 0.0f;

    // Using cofactor expansion along the first row
    det += m[0][0] * (
        m[1][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) -
        m[1][2] * (m[2][1] * m[3][3] - m[2][3] * m[3][1]) +
        m[1][3] * (m[2][1] * m[3][2] - m[2][2] * m[3][1])
    );

    det -= m[0][1] * (
        m[1][0] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) -
        m[1][2] * (m[2][0] * m[3][3] - m[2][3] * m[3][0]) +
        m[1][3] * (m[2][0] * m[3][2] - m[2][2] * m[3][0])
    );

    det += m[0][2] * (
        m[1][0] * (m[2][1] * m[3][3] - m[2][3] * m[3][1]) -
        m[1][1] * (m[2][0] * m[3][3] - m[2][3] * m[3][0]) +
        m[1][3] * (m[2][0] * m[3][1] - m[2][1] * m[3][0])
    );

    det -= m[0][3] * (
        m[1][0] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]) -
        m[1][1] * (m[2][0] * m[3][2] - m[2][2] * m[3][0]) +
        m[1][2] * (m[2][0] * m[3][1] - m[2][1] * m[3][0])
    );

    return det;
}

///////////////////////////////////////////////////////////////////////////////
TMatrix4x4<float> TMatrix4x4<float>::Transpose() const
{
    return TMatrix4x4<float>(
        m[0][0], m[1][0], m[2][0], m[3][0],
        m[0][1], m[1][1], m[2][1], m[3][1],
        m[0][2], m[1][2], m[2][2], m[3][2],
        m[0][3], m[1][3], m[2][3], m[3][3]
    );
}

///////////////////////////////////////////////////////////////////////////////
TMatrix4x4<float> TMatrix4x4<float>::Inverse() const
{
    float det = Determinant();
    if (det == 0)
    {
        throw std::runtime_error("Matrix is not invertible");
    }

    float invDet = 1.0f / det;
    TMatrix4x4<float> result;

    // Calculate cofactors and adjugate matrix
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            // Get minor by excluding row i and column j
            float minor = 0.0f;
            int sign = ((i + j) % 2 == 0) ? 1 : -1;

            // 3x3 determinant for each minor
            int r[3], c[3];
            for (int k = 0, kr = 0; k < 4; k++) {
                if (k != i) r[kr++] = k;
            }
            for (int k = 0, kc = 0; k < 4; k++) {
                if (k != j) c[kc++] = k;
            }

            minor =
                m[r[0]][c[0]] * (m[r[1]][c[1]] * m[r[2]][c[2]] - m[r[1]][c[2]] * m[r[2]][c[1]]) -
                m[r[0]][c[1]] * (m[r[1]][c[0]] * m[r[2]][c[2]] - m[r[1]][c[2]] * m[r[2]][c[0]]) +
                m[r[0]][c[2]] * (m[r[1]][c[0]] * m[r[2]][c[1]] - m[r[1]][c[1]] * m[r[2]][c[0]]);

            // Adjugate has transposed indices
            result.m[j][i] = sign * minor * invDet;
        }
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
TMatrix4x4<float> TMatrix4x4<float>::operator+(const TMatrix4x4<float>& other) const
{
    return TMatrix4x4<float>(
        m[0][0] + other.m[0][0], m[0][1] + other.m[0][1], m[0][2] + other.m[0][2], m[0][3] + other.m[0][3],
        m[1][0] + other.m[1][0], m[1][1] + other.m[1][1], m[1][2] + other.m[1][2], m[1][3] + other.m[1][3],
        m[2][0] + other.m[2][0], m[2][1] + other.m[2][1], m[2][2] + other.m[2][2], m[2][3] + other.m[2][3],
        m[3][0] + other.m[3][0], m[3][1] + other.m[3][1], m[3][2] + other.m[3][2], m[3][3] + other.m[3][3]
    );
}

///////////////////////////////////////////////////////////////////////////////
TMatrix4x4<float> TMatrix4x4<float>::operator-(const TMatrix4x4<float>& other) const
{
    return TMatrix4x4<float>(
        m[0][0] - other.m[0][0], m[0][1] - other.m[0][1], m[0][2] - other.m[0][2], m[0][3] - other.m[0][3],
        m[1][0] - other.m[1][0], m[1][1] - other.m[1][1], m[1][2] - other.m[1][2], m[1][3] - other.m[1][3],
        m[2][0] - other.m[2][0], m[2][1] - other.m[2][1], m[2][2] - other.m[2][2], m[2][3] - other.m[2][3],
        m[3][0] - other.m[3][0], m[3][1] - other.m[3][1], m[3][2] - other.m[3][2], m[3][3] - other.m[3][3]
    );
}

///////////////////////////////////////////////////////////////////////////////
TMatrix4x4<float> TMatrix4x4<float>::operator*(const TMatrix4x4<float>& other) const
{
    TMatrix4x4<float> result;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result.m[i][j] =
                m[i][0] * other.m[0][j] +
                m[i][1] * other.m[1][j] +
                m[i][2] * other.m[2][j] +
                m[i][3] * other.m[3][j];
        }
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
TMatrix4x4<float> TMatrix4x4<float>::operator*(float scalar) const
{
    return TMatrix4x4<float>(
        m[0][0] * scalar, m[0][1] * scalar, m[0][2] * scalar, m[0][3] * scalar,
        m[1][0] * scalar, m[1][1] * scalar, m[1][2] * scalar, m[1][3] * scalar,
        m[2][0] * scalar, m[2][1] * scalar, m[2][2] * scalar, m[2][3] * scalar,
        m[3][0] * scalar, m[3][1] * scalar, m[3][2] * scalar, m[3][3] * scalar
    );
}

///////////////////////////////////////////////////////////////////////////////
bool TMatrix4x4<float>::operator==(const TMatrix4x4<float>& other) const
{
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (m[i][j] != other.m[i][j]) {
                return false;
            }
        }
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool TMatrix4x4<float>::operator!=(const TMatrix4x4<float>& other) const
{
    return !(*this == other);
}

///////////////////////////////////////////////////////////////////////////////
float TMatrix4x4<float>::Trace() const
{
    return m[0][0] + m[1][1] + m[2][2] + m[3][3];
}

}   // namespace tkd