///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Config.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief Blend mode enumeration for rendering operations
///
///////////////////////////////////////////////////////////////////////////////
enum class EBlendMode : UInt8
{
    None = 0,   //<! No blending
    Alpha,      //<! Standard alpha blending
    Add,        //<! Additive blending
    Multiply,   //<! Multiplicative blending
    Screen,     //<! Screen blending
    Subtract,   //<! Subtractive blending
    Replace     //<! Replace blending (no blending, opaque)
};

///////////////////////////////////////////////////////////////////////////////
/// \brief Primitive type enumeration
///
///////////////////////////////////////////////////////////////////////////////
enum class EPrimitiveType : UInt8
{
    Points = 0,      //<! Individual points
    Lines,           //<! Line segments
    LineStrip,       //<! Connected line strip
    Triangles,       //<! Individual triangles
    TriangleStrip,   //<! Triangle strip
    TriangleFan,     //<! Triangle fan
    Quads,           //<! Quadrilaterals
    QuadStrip        //<! Quad strip
};

///////////////////////////////////////////////////////////////////////////////
/// \brief Face culling mode enumeration
///
///////////////////////////////////////////////////////////////////////////////
enum class ECullFaceMode : UInt16
{
    Front = 0x0404,   //<! Cull front faces (GL_FRONT)
    Back = 0x0405,    //<! Cull back faces (GL_BACK)
    FrontAndBack =
        0x0408        //<! Cull both front and back faces (GL_FRONT_AND_BACK)
};

///////////////////////////////////////////////////////////////////////////////
/// \brief Front face orientation enumeration
///
///////////////////////////////////////////////////////////////////////////////
enum class EFrontFace : UInt16
{
    Clockwise = 0x0900,         //<! Clockwise winding (GL_CW)
    CounterClockwise = 0x0901   //<! Counter-clockwise winding (GL_CCW)
};

///////////////////////////////////////////////////////////////////////////////
/// \brief Polygon rendering mode enumeration
///
///////////////////////////////////////////////////////////////////////////////
enum class EPolygonMode : UInt16
{
    Point = 0x1B00,   //<! Render as points (GL_POINT)
    Line = 0x1B01,    //<! Render as wireframe (GL_LINE)
    Fill = 0x1B02     //<! Render as filled polygons (GL_FILL)
};

///////////////////////////////////////////////////////////////////////////////
/// \brief Depth comparison function enumeration
///
///////////////////////////////////////////////////////////////////////////////
enum class EDepthFunc : UInt16
{
    Never = 0x0200,          //<! Never passes (GL_NEVER)
    Less = 0x0201,           //<! Passes if less than (GL_LESS)
    Equal = 0x0202,          //<! Passes if equal (GL_EQUAL)
    LessEqual = 0x0203,      //<! Passes if less than or equal (GL_LEQUAL)
    Greater = 0x0204,        //<! Passes if greater than (GL_GREATER)
    NotEqual = 0x0205,       //<! Passes if not equal (GL_NOTEQUAL)
    GreaterEqual = 0x0206,   //<! Passes if greater than or equal (GL_GEQUAL)
    Always = 0x0207          //<! Always passes (GL_ALWAYS)
};

///////////////////////////////////////////////////////////////////////////////
/// \brief Blend factor enumeration
///
///////////////////////////////////////////////////////////////////////////////
enum class EBlendFactor : UInt16
{
    Zero = 0,            //<! Factor is zero (GL_ZERO)
    One = 1,             //<! Factor is one (GL_ONE)
    SrcColor = 0x0300,   //<! Factor is source color (GL_SRC_COLOR)
    OneMinusSrcColor =
        0x0301,   //<! Factor is 1 - source color (GL_ONE_MINUS_SRC_COLOR)
    SrcAlpha = 0x0302,   //<! Factor is source alpha (GL_SRC_ALPHA)
    OneMinusSrcAlpha =
        0x0303,   //<! Factor is 1 - source alpha (GL_ONE_MINUS_SRC_ALPHA)
    DstAlpha = 0x0304,   //<! Factor is destination alpha (GL_DST_ALPHA)
    OneMinusDstAlpha =
        0x0305,   //<! Factor is 1 - destination alpha (GL_ONE_MINUS_DST_ALPHA)
    DstColor = 0x0306,   //<! Factor is destination color (GL_DST_COLOR)
    OneMinusDstColor =
        0x0307,   //<! Factor is 1 - destination color (GL_ONE_MINUS_DST_COLOR)
    SrcAlphaSaturate =
        0x0308,   //<! Factor is source alpha saturated (GL_SRC_ALPHA_SATURATE)
    ConstantColor = 0x8001,   //<! Factor is constant color (GL_CONSTANT_COLOR)
    OneMinusConstantColor = 0x8002,   //<! Factor is 1 - constant color
                                      //(GL_ONE_MINUS_CONSTANT_COLOR)
    ConstantAlpha = 0x8003,   //<! Factor is constant alpha (GL_CONSTANT_ALPHA)
    OneMinusConstantAlpha = 0x8004   //<! Factor is 1 - constant alpha
                                     //(GL_ONE_MINUS_CONSTANT_ALPHA)
};

///////////////////////////////////////////////////////////////////////////////
/// \brief Matrix mode enumeration
///
///////////////////////////////////////////////////////////////////////////////
enum class EMatrixMode : UInt16
{
    ModelView = 0x1700,    //<! Model-view matrix (GL_MODELVIEW)
    Projection = 0x1701,   //<! Projection matrix (GL_PROJECTION)
    Texture = 0x1702       //<! Texture matrix (GL_TEXTURE)
};

///////////////////////////////////////////////////////////////////////////////
/// \brief Shading model enumeration
///
///////////////////////////////////////////////////////////////////////////////
enum class EShadeModel : UInt16
{
    Flat = 0x1D00,    //<! Flat shading (GL_FLAT)
    Smooth = 0x1D01   //<! Smooth shading (GL_SMOOTH)
};

///////////////////////////////////////////////////////////////////////////////
/// \brief OpenGL capability enumeration for enable/disable operations
///
///////////////////////////////////////////////////////////////////////////////
enum class EGLCapability : UInt16
{
    Blend = 0x0BE2,           //<! Blending (GL_BLEND)
    CullFace = 0x0B44,        //<! Face culling (GL_CULL_FACE)
    DepthTest = 0x0B71,       //<! Depth testing (GL_DEPTH_TEST)
    Lighting = 0x0B50,        //<! Lighting (GL_LIGHTING)
    Texture2D = 0x0DE1,       //<! 2D texturing (GL_TEXTURE_2D)
    ColorMaterial = 0x0B57,   //<! Color material (GL_COLOR_MATERIAL)
    Fog = 0x0B60,             //<! Fog (GL_FOG)
    AlphaTest = 0x0BC0,       //<! Alpha testing (GL_ALPHA_TEST)
    StencilTest = 0x0B90,     //<! Stencil testing (GL_STENCIL_TEST)
    Normalize = 0x0BA1,       //<! Normal normalization (GL_NORMALIZE)
    LineSmooth = 0x0B20,      //<! Line antialiasing (GL_LINE_SMOOTH)
    PolygonSmooth = 0x0B41,   //<! Polygon antialiasing (GL_POLYGON_SMOOTH)
    ScissorTest = 0x0C11      //<! Scissor test (GL_SCISSOR_TEST)
};

///////////////////////////////////////////////////////////////////////////////
/// \brief OpenGL attribute bit flags for glPushAttrib/glPopAttrib
///
///////////////////////////////////////////////////////////////////////////////
enum class EGLAttributeBit : UInt32
{
    CurrentBit = 0x00000001,   //<! Current attribute group (GL_CURRENT_BIT)
    PointBit = 0x00000002,     //<! Point attribute group (GL_POINT_BIT)
    LineBit = 0x00000004,      //<! Line attribute group (GL_LINE_BIT)
    PolygonBit = 0x00000008,   //<! Polygon attribute group (GL_POLYGON_BIT)
    PolygonStippleBit =
        0x00000010,   //<! Polygon stipple group (GL_POLYGON_STIPPLE_BIT)
    PixelModeBit = 0x00000020,   //<! Pixel mode group (GL_PIXEL_MODE_BIT)
    LightingBit = 0x00000040,   //<! Lighting attribute group (GL_LIGHTING_BIT)
    FogBit = 0x00000080,        //<! Fog attribute group (GL_FOG_BIT)
    DepthBufferBit =
        0x00000100,             //<! Depth buffer group (GL_DEPTH_BUFFER_BIT)
    AccumBufferBit =
        0x00000200,   //<! Accumulation buffer group (GL_ACCUM_BUFFER_BIT)
    StencilBufferBit =
        0x00000400,   //<! Stencil buffer group (GL_STENCIL_BUFFER_BIT)
    ViewportBit = 0x00000800,   //<! Viewport attribute group (GL_VIEWPORT_BIT)
    TransformBit =
        0x00001000,           //<! Transform attribute group (GL_TRANSFORM_BIT)
    EnableBit = 0x00002000,   //<! Enable attribute group (GL_ENABLE_BIT)
    ColorBufferBit =
        0x00004000,           //<! Color buffer group (GL_COLOR_BUFFER_BIT)
    HintBit = 0x00008000,     //<! Hint attribute group (GL_HINT_BIT)
    EvalBit = 0x00010000,     //<! Evaluator group (GL_EVAL_BIT)
    ListBit = 0x00020000,     //<! Display list group (GL_LIST_BIT)
    TextureBit = 0x00040000,     //<! Texture attribute group (GL_TEXTURE_BIT)
    ScissorBit = 0x00080000,     //<! Scissor attribute group (GL_SCISSOR_BIT)
    AllAttribBits = 0x000FFFFF   //<! All attribute groups (GL_ALL_ATTRIB_BITS)
};

}   // namespace tkd
