///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Renderer/Primitives/UCubePrimitive.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
UCubePrimitive::UCubePrimitive(const FColor& color)
    : UPrimitive(EPrimitiveType::Quads, color)
{
    // FRONT FACE
    m_vertices.push_back(FVertex(FVector3(-0.5f, -0.5f, 0.5f), color));
    m_vertices.push_back(FVertex(FVector3(0.5f, -0.5f, 0.5f), color));
    m_vertices.push_back(FVertex(FVector3(0.5f, 0.5f, 0.5f), color));
    m_vertices.push_back(FVertex(FVector3(-0.5f, 0.5f, 0.5f), color));

    // BACK FACE
    m_vertices.push_back(FVertex(FVector3(-0.5f, -0.5f, -0.5f), color));
    m_vertices.push_back(FVertex(FVector3(-0.5f, 0.5f, -0.5f), color));
    m_vertices.push_back(FVertex(FVector3(0.5f, 0.5f, -0.5f), color));
    m_vertices.push_back(FVertex(FVector3(0.5f, -0.5f, -0.5f), color));

    // TOP FACE
    m_vertices.push_back(FVertex(FVector3(-0.5f, 0.5f, -0.5f), color));
    m_vertices.push_back(FVertex(FVector3(-0.5f, 0.5f, 0.5f), color));
    m_vertices.push_back(FVertex(FVector3(0.5f, 0.5f, 0.5f), color));
    m_vertices.push_back(FVertex(FVector3(0.5f, 0.5f, -0.5f), color));

    // BOTTOM FACE
    m_vertices.push_back(FVertex(FVector3(-0.5f, -0.5f, -0.5f), color));
    m_vertices.push_back(FVertex(FVector3(0.5f, -0.5f, -0.5f), color));
    m_vertices.push_back(FVertex(FVector3(0.5f, -0.5f, 0.5f), color));
    m_vertices.push_back(FVertex(FVector3(-0.5f, -0.5f, 0.5f), color));

    // RIGHT FACE
    m_vertices.push_back(FVertex(FVector3(0.5f, -0.5f, -0.5f), color));
    m_vertices.push_back(FVertex(FVector3(0.5f, 0.5f, -0.5f), color));
    m_vertices.push_back(FVertex(FVector3(0.5f, 0.5f, 0.5f), color));
    m_vertices.push_back(FVertex(FVector3(0.5f, -0.5f, 0.5f), color));

    // LEFT FACE
    m_vertices.push_back(FVertex(FVector3(-0.5f, -0.5f, -0.5f), color));
    m_vertices.push_back(FVertex(FVector3(-0.5f, -0.5f, 0.5f), color));
    m_vertices.push_back(FVertex(FVector3(-0.5f, 0.5f, 0.5f), color));
    m_vertices.push_back(FVertex(FVector3(-0.5f, 0.5f, -0.5f), color));
}

}   // namespace tkd
