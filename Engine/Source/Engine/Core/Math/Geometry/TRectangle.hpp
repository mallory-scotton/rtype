///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <cassert>
#include <Engine/Config.hpp>
#include <Engine/Core/Math/TVector2.hpp>
#include <ostream>
#include <type_traits>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief A 2D rectangle template class for arithmetic types.
///
/// This class represents a rectangle defined by its position (left, top) and
/// size (width, height). It provides methods for geometric operations like
/// intersection, containment checking, and transformations similar to SFML's
/// sf::Rect.
///
/// \tparam T The arithmetic type for the rectangle components.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
class TRectangle
{
public:
    static_assert(
        std::is_arithmetic<T>::value, "T must be an arithmetic type"
    );

public:
    ///////////////////////////////////////////////////////////////////////////
    // Class Members
    ///////////////////////////////////////////////////////////////////////////
    T left;     //<! Left coordinate of the rectangle.
    T top;      //<! Top coordinate of the rectangle.
    T width;    //<! Width of the rectangle.
    T height;   //<! Height of the rectangle.

public:
    ///////////////////////////////////////////////////////////////////////////
    // Static Members
    ///////////////////////////////////////////////////////////////////////////
    static const TRectangle Zero;   //<! A rectangle at origin with zero size.
    static const TRectangle Unit;   //<! A unit rectangle (0, 0, 1, 1).

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor. Initializes the rectangle to (0, 0, 0, 0).
    ///
    ///////////////////////////////////////////////////////////////////////////
    TRectangle(void)
        : left(0)
        , top(0)
        , width(0)
        , height(0)
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructs a rectangle with the specified position and size.
    ///
    /// \param left The left coordinate.
    /// \param top The top coordinate.
    /// \param width The width of the rectangle.
    /// \param height The height of the rectangle.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TRectangle(const T& left, const T& top, const T& width, const T& height)
        : left(left)
        , top(top)
        , width(width)
        , height(height)
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructs a rectangle from position and size vectors.
    ///
    /// \param position The position (left, top) as a vector.
    /// \param size The size (width, height) as a vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TRectangle(const TVector2<T>& position, const TVector2<T>& size)
        : left(position.x)
        , top(position.y)
        , width(size.x)
        , height(size.y)
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Copy constructor. Creates a copy of another rectangle.
    ///
    /// \param other The rectangle to copy from.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TRectangle(const TRectangle& other)
        : left(other.left)
        , top(other.top)
        , width(other.width)
        , height(other.height)
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Template copy constructor for different types.
    ///
    /// \tparam U The source type.
    /// \param other The rectangle to copy from.
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename U>
    explicit TRectangle(const TRectangle<U>& other)
        : left(static_cast<T>(other.left))
        , top(static_cast<T>(other.top))
        , width(static_cast<T>(other.width))
        , height(static_cast<T>(other.height))
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Destructor.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ~TRectangle(void) = default;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Assignment operator.
    ///
    /// \param other The rectangle to assign from.
    /// \return Reference to this rectangle.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TRectangle& operator=(const TRectangle& other)
    {
        if (this != &other)
        {
            left = other.left;
            top = other.top;
            width = other.width;
            height = other.height;
        }
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Template assignment operator for different types.
    ///
    /// \tparam U The source type.
    /// \param other The rectangle to assign from.
    /// \return Reference to this rectangle.
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename U>
    TRectangle& operator=(const TRectangle<U>& other)
    {
        left = static_cast<T>(other.left);
        top = static_cast<T>(other.top);
        width = static_cast<T>(other.width);
        height = static_cast<T>(other.height);
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Equality operator.
    ///
    /// \param other The rectangle to compare with.
    /// \return True if rectangles are equal, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool operator==(const TRectangle& other) const
    {
        return left == other.left && top == other.top &&
               width == other.width && height == other.height;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Inequality operator.
    ///
    /// \param other The rectangle to compare with.
    /// \return True if rectangles are not equal, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool operator!=(const TRectangle& other) const
    {
        return !(*this == other);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Checks if a point is contained within the rectangle.
    ///
    /// \param x The x-coordinate of the point.
    /// \param y The y-coordinate of the point.
    /// \return True if the point is inside the rectangle, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool Contains(const T& x, const T& y) const
    {
        return (x >= left) && (x < left + width) && (y >= top) &&
               (y < top + height);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Checks if a point is contained within the rectangle.
    ///
    /// \param point The point as a vector.
    /// \return True if the point is inside the rectangle, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool Contains(const TVector2<T>& point) const
    {
        return Contains(point.x, point.y);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Checks if another rectangle intersects with this one.
    ///
    /// \param other The rectangle to check intersection with.
    /// \return True if rectangles intersect, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool Intersects(const TRectangle& other) const
    {
        return !(
            left >= other.left + other.width || left + width <= other.left ||
            top >= other.top + other.height || top + height <= other.top
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Checks if another rectangle intersects with this one and
    ///        returns the intersection area.
    ///
    /// \param other The rectangle to check intersection with.
    /// \param intersection Reference to store the intersection rectangle.
    /// \return True if rectangles intersect, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool Intersects(const TRectangle& other, TRectangle& intersection) const
    {
        if (!Intersects(other))
        {
            intersection = TRectangle();
            return false;
        }

        T intersectLeft = (left > other.left) ? left : other.left;
        T intersectTop = (top > other.top) ? top : other.top;
        T intersectRight = (left + width < other.left + other.width)
                               ? left + width
                               : other.left + other.width;
        T intersectBottom = (top + height < other.top + other.height)
                                ? top + height
                                : other.top + other.height;

        intersection.left = intersectLeft;
        intersection.top = intersectTop;
        intersection.width = intersectRight - intersectLeft;
        intersection.height = intersectBottom - intersectTop;

        return true;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets the position of the rectangle.
    ///
    /// \return The position as a vector (left, top).
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector2<T> GetPosition(void) const { return TVector2<T>(left, top); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets the size of the rectangle.
    ///
    /// \return The size as a vector (width, height).
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector2<T> GetSize(void) const { return TVector2<T>(width, height); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets the center point of the rectangle.
    ///
    /// \return The center point as a vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector2<T> GetCenter(void) const
    {
        return TVector2<T>(left + width / 2, top + height / 2);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets the right coordinate of the rectangle.
    ///
    /// \return The right coordinate (left + width).
    ///
    ///////////////////////////////////////////////////////////////////////////
    T GetRight(void) const { return left + width; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets the bottom coordinate of the rectangle.
    ///
    /// \return The bottom coordinate (top + height).
    ///
    ///////////////////////////////////////////////////////////////////////////
    T GetBottom(void) const { return top + height; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets the area of the rectangle.
    ///
    /// \return The area (width * height).
    ///
    ///////////////////////////////////////////////////////////////////////////
    T GetArea(void) const { return width * height; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets the perimeter of the rectangle.
    ///
    /// \return The perimeter (2 * (width + height)).
    ///
    ///////////////////////////////////////////////////////////////////////////
    T GetPerimeter(void) const { return 2 * (width + height); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Checks if the rectangle is empty (zero or negative size).
    ///
    /// \return True if the rectangle is empty, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool IsEmpty(void) const { return width <= 0 || height <= 0; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Sets the position of the rectangle.
    ///
    /// \param x The new left coordinate.
    /// \param y The new top coordinate.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetPosition(const T& x, const T& y)
    {
        left = x;
        top = y;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Sets the position of the rectangle.
    ///
    /// \param position The new position as a vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetPosition(const TVector2<T>& position)
    {
        left = position.x;
        top = position.y;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Sets the size of the rectangle.
    ///
    /// \param width The new width.
    /// \param height The new height.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetSize(const T& width, const T& height)
    {
        this->width = width;
        this->height = height;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Sets the size of the rectangle.
    ///
    /// \param size The new size as a vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetSize(const TVector2<T>& size)
    {
        width = size.x;
        height = size.y;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Moves the rectangle by the specified offset.
    ///
    /// \param offsetX The horizontal offset.
    /// \param offsetY The vertical offset.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Move(const T& offsetX, const T& offsetY)
    {
        left += offsetX;
        top += offsetY;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Moves the rectangle by the specified offset.
    ///
    /// \param offset The offset as a vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Move(const TVector2<T>& offset)
    {
        left += offset.x;
        top += offset.y;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Scales the rectangle from its center.
    ///
    /// \param scaleX The horizontal scale factor.
    /// \param scaleY The vertical scale factor.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Scale(const T& scaleX, const T& scaleY)
    {
        TVector2<T> center = GetCenter();
        width *= scaleX;
        height *= scaleY;
        left = center.x - width / 2;
        top = center.y - height / 2;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Scales the rectangle from its center.
    ///
    /// \param scale The scale factor as a vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Scale(const TVector2<T>& scale) { Scale(scale.x, scale.y); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Inflates the rectangle by the specified amounts.
    ///
    /// \param deltaX The horizontal expansion (positive = expand, negative =
    /// shrink).
    /// \param deltaY The vertical expansion (positive = expand, negative =
    /// shrink).
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Inflate(const T& deltaX, const T& deltaY)
    {
        left -= deltaX;
        top -= deltaY;
        width += 2 * deltaX;
        height += 2 * deltaY;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Inflates the rectangle by the specified amounts.
    ///
    /// \param delta The expansion as a vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Inflate(const TVector2<T>& delta) { Inflate(delta.x, delta.y); }

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Creates a rectangle from two corner points.
    ///
    /// \param point1 The first corner point.
    /// \param point2 The opposite corner point.
    /// \return A rectangle encompassing both points.
    ///
    ///////////////////////////////////////////////////////////////////////////
    static TRectangle
        FromPoints(const TVector2<T>& point1, const TVector2<T>& point2)
    {
        T minX = (point1.x < point2.x) ? point1.x : point2.x;
        T minY = (point1.y < point2.y) ? point1.y : point2.y;
        T maxX = (point1.x > point2.x) ? point1.x : point2.x;
        T maxY = (point1.y > point2.y) ? point1.y : point2.y;

        return TRectangle(minX, minY, maxX - minX, maxY - minY);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Creates a rectangle centered at the specified position.
    ///
    /// \param center The center point.
    /// \param size The size of the rectangle.
    /// \return A rectangle centered at the given position.
    ///
    ///////////////////////////////////////////////////////////////////////////
    static TRectangle
        FromCenter(const TVector2<T>& center, const TVector2<T>& size)
    {
        return TRectangle(
            center.x - size.x / 2, center.y - size.y / 2, size.x, size.y
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Creates the union of two rectangles (smallest rectangle
    /// containing both).
    ///
    /// \param rect1 The first rectangle.
    /// \param rect2 The second rectangle.
    /// \return A rectangle that encompasses both input rectangles.
    ///
    ///////////////////////////////////////////////////////////////////////////
    static TRectangle Union(const TRectangle& rect1, const TRectangle& rect2)
    {
        if (rect1.IsEmpty()) { return rect2; }
        if (rect2.IsEmpty()) { return rect1; }

        T minX = (rect1.left < rect2.left) ? rect1.left : rect2.left;
        T minY = (rect1.top < rect2.top) ? rect1.top : rect2.top;
        T maxX = (rect1.GetRight() > rect2.GetRight()) ? rect1.GetRight()
                                                       : rect2.GetRight();
        T maxY = (rect1.GetBottom() > rect2.GetBottom()) ? rect1.GetBottom()
                                                         : rect2.GetBottom();

        return TRectangle(minX, minY, maxX - minX, maxY - minY);
    }
};

///////////////////////////////////////////////////////////////////////////////
// Static Member Definitions
///////////////////////////////////////////////////////////////////////////////
template <typename T>
const TRectangle<T> TRectangle<T>::Zero = TRectangle<T>(0, 0, 0, 0);

template <typename T>
const TRectangle<T> TRectangle<T>::Unit = TRectangle<T>(0, 0, 1, 1);

///////////////////////////////////////////////////////////////////////////////
/// \brief Stream output operator for TRectangle.
///
/// \param os The output stream.
/// \param rect The rectangle to output.
///
/// \return The output stream.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
std::ostream& operator<<(std::ostream& os, const TRectangle<T>& rect)
{
    os << "TRectangle(" << rect.left << ", " << rect.top << ", " << rect.width
       << ", " << rect.height << ")";
    return os;
}

}   // namespace tkd
