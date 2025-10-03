///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Renderer/FVertex2DArray.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
FVertex2DArray::FVertex2DArray(EPrimitiveType type)
    : TVector<FVertex2D>()
    , m_primitiveType(type)
{}

///////////////////////////////////////////////////////////////////////////////
FRectanglef FVertex2DArray::GetBounds(void) const
{
    if (!Empty())
    {
        float left = m_data[0].position.x;
        float top = m_data[0].position.y;
        float right = m_data[0].position.x;
        float bottom = m_data[0].position.y;

        for (const auto& vertex: m_data)
        {
            if (vertex.position.x < left) { left = vertex.position.x; }
            else if (vertex.position.x > right) { right = vertex.position.x; }
            if (vertex.position.y < top) { top = vertex.position.y; }
            else if (vertex.position.y > bottom)
            {
                bottom = vertex.position.y;
            }
        }

        return FRectanglef(left, top, right - left, bottom - top);
    }

    return FRectanglef::Zero;
}

///////////////////////////////////////////////////////////////////////////////
EPrimitiveType FVertex2DArray::GetPrimitiveType(void) const
{
    return m_primitiveType;
}

}   // namespace tkd
