///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Config.hpp>
#include <Engine/Core/Math.hpp>
#include <Engine/Renderer/Enumerations.hpp>
#include <Engine/Renderer/FRenderStates.hpp>
#include <Engine/Renderer/FView.hpp>
#include <Engine/Renderer/IShader.hpp>
#if TKD_ENGINE_CLIENT
    #include <SFML/Graphics.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::SFML
///////////////////////////////////////////////////////////////////////////////
namespace tkd::SFML
{

///////////////////////////////////////////////////////////////////////////////
/// \brief Utility class for converting between TKD and SFML types
///
///////////////////////////////////////////////////////////////////////////////
class Utils
{
public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Convert a TKD color to an SFML color
    ///
    /// \param color The TKD color to convert
    ///
    /// \return The converted SFML color
    ///
    ///////////////////////////////////////////////////////////////////////////
    static sf::Color Convert(const FColor& color);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Convert a TKD linear color to an SFML color
    ///
    /// \param color The TKD linear color to convert
    ///
    /// \return The converted SFML color
    ///
    ///////////////////////////////////////////////////////////////////////////
    static sf::Color Convert(const FLinearColor& color);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Convert a TKD view to an SFML view
    ///
    /// \param view The TKD view to convert
    ///
    /// \return The converted SFML view
    ///
    ///////////////////////////////////////////////////////////////////////////
    static sf::View Convert(const FView& view);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Convert a TKD blend mode to an SFML blend mode
    ///
    /// \param blendMode The TKD blend mode to convert
    ///
    /// \return The converted SFML blend mode
    ///
    ///////////////////////////////////////////////////////////////////////////
    static sf::BlendMode Convert(EBlendMode blendMode);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Convert TKD render states to SFML render states
    ///
    /// \param states The TKD render states to convert
    ///
    /// \return The converted SFML render states
    ///
    ///////////////////////////////////////////////////////////////////////////
    static sf::RenderStates Convert(const FRenderStates& states);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Convert a TKD primitive type to an SFML primitive type
    ///
    /// \param type The TKD primitive type to convert
    ///
    /// \return The converted SFML primitive type
    ///
    ///////////////////////////////////////////////////////////////////////////
    static sf::PrimitiveType Convert(EPrimitiveType type);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Convert a TKD 2D transform to an SFML transform
    ///
    /// \param transform The TKD 2D transform to convert
    ///
    /// \return The converted SFML transform
    ///
    ///////////////////////////////////////////////////////////////////////////
    static sf::Transform Convert(const FTransform2D& transform);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Convert a TKD shader to an SFML shader
    ///
    /// \param shader The TKD shader to convert
    ///
    /// \return The converted SFML shader
    ///
    ///////////////////////////////////////////////////////////////////////////
    static sf::Vertex Convert(const FVertex2D& vertex);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Convert a SFML vector to an TKD vector
    ///
    /// \tparam T The type of the vector components
    ///
    /// \param vector The SFML vector to convert
    ///
    /// \return The converted TKD vector
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    static TVector2<T> Convert(const sf::Vector2<T>& vector)
    {
        return TVector2<T>(vector.x, vector.y);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Convert a TKD vector to an SFML vector
    ///
    /// \tparam T The type of the vector components
    ///
    /// \param vector The TKD vector to convert
    ///
    /// \return The converted SFML vector
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    static sf::Vector2<T> Convert(const TVector2<T>& vector)
    {
        return sf::Vector2<T>(vector.x, vector.y);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Convert a TKD 3D vector to an SFML 3D vector
    ///
    /// \tparam T The type of the vector components
    ///
    /// \param vector The TKD vector to convert
    ///
    /// \return The converted SFML vector
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    static sf::Vector3<T> Convert(const TVector3<T>& vector)
    {
        return sf::Vector3<T>(vector.x, vector.y, vector.z);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Convert a SFML vector to an TKD vector
    ///
    /// \tparam T The type of the vector components
    ///
    /// \param vector The SFML vector to convert
    ///
    /// \return The converted TKD vector
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    static TVector3<T> Convert(const sf::Vector3<T>& vector)
    {
        return TVector3<T>(vector.x, vector.y, vector.z);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Convert a TKD rectangle to an SFML rectangle
    ///
    /// \tparam T The type of the rectangle components
    ///
    /// \param rect The TKD rectangle to convert
    ///
    /// \return The converted SFML rectangle
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    static sf::Rect<T> Convert(const TRectangle<T>& rect)
    {
        return sf::Rect<T>(rect.left, rect.top, rect.width, rect.height);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Convert a TKD 3x3 matrix to an SFML 3x3 matrix
    ///
    /// \param mat The TKD 3x3 matrix to convert
    ///
    /// \return The converted SFML matrix
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    static sf::Glsl::Mat3 Convert(const TMatrix3x3<T>& mat)
    {
        return sf::Glsl::Mat3(
            mat.data[0][0],
            mat.data[0][1],
            mat.data[0][2],
            mat.data[1][0],
            mat.data[1][1],
            mat.data[1][2],
            mat.data[2][0],
            mat.data[2][1],
            mat.data[2][2]
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Convert a TKD 4x4 matrix to an SFML 4x4 matrix
    ///
    /// \param mat The TKD 4x4 matrix to convert
    ///
    /// \return The converted SFML matrix
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    static sf::Glsl::Mat4 Convert(const TMatrix4x4<T>& mat)
    {
        return sf::Glsl::Mat4(
            mat.data[0][0],
            mat.data[0][1],
            mat.data[0][2],
            mat.data[0][3],
            mat.data[1][0],
            mat.data[1][1],
            mat.data[1][2],
            mat.data[1][3],
            mat.data[2][0],
            mat.data[2][1],
            mat.data[2][2],
            mat.data[2][3],
            mat.data[3][0],
            mat.data[3][1],
            mat.data[3][2],
            mat.data[3][3]
        );
    }
};

}   // namespace tkd::SFML
#endif
