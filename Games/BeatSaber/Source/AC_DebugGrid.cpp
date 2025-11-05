///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <AC_DebugGrid.hpp>
#if TKD_ENGINE_CLIENT
    #include <GL/glew.h>
    #include <GL/glu.h>
#endif

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
AC_DebugGrid::AC_DebugGrid(const FString& name)
    : UActorComponent(name)
    , m_vertices()
{
    // Setup the vertex for the beat saber debug grid
    // The grid is a simple XY grid on the Z=0 plane
    // Each grid tiles is 0.6 units wide and tall
    // y=2  [ ] [ ] [ ] [ ]
    // y=1  [ ] [ ] [ ] [ ]
    // y=0  [ ] [ ] [ ] [ ]
    //      x=0 x=1 x=2 x=3

    const Float32 tileSize = 0.6f;
    const Int32 columns = 4;
    const Int32 rows = 3;
    const Float32 gridWidth = columns * tileSize;
    const Float32 gridHeight = rows * tileSize;
    const Float32 offsetX = -gridWidth / 2.0f;
    const Float32 offsetY = 0.3f;
    const FColor gridColor = FColor::White;

    // Reserve space for vertices (5 horizontal lines + 5 vertical lines) * 2
    // vertices per line
    m_vertices.reserve((rows + 1 + columns + 1) * 2);

    // Create horizontal lines (from left to right)
    for (Int32 i = 0; i <= rows; ++i)
    {
        Float32 y = i * tileSize + offsetY;
        m_vertices.push_back(FVertex(FVector3(offsetX, y, 0.0f), gridColor));
        m_vertices.push_back(
            FVertex(FVector3(offsetX + gridWidth, y, 0.0f), gridColor)
        );
    }

    // Create vertical lines (from bottom to top)
    for (Int32 i = 0; i <= columns; ++i)
    {
        Float32 x = i * tileSize + offsetX;
        m_vertices.push_back(FVertex(FVector3(x, offsetY, 0.0f), gridColor));
        m_vertices.push_back(
            FVertex(FVector3(x, offsetY + gridHeight, 0.0f), gridColor)
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
void AC_DebugGrid::Render(IRenderer& renderer) const
{
    renderer.Draw(m_vertices, EPrimitiveType::Lines);
}

}   // namespace tkd
