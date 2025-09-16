///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <cmath>
#include <Engine/Config.hpp>
#include <type_traits>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief A collection of common math functions
///
/// \tparam T The type of the values (must be an arithmetic type)
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
class Math
{
public:
    ///////////////////////////////////////////////////////////////////////////
    // Static assertions
    ///////////////////////////////////////////////////////////////////////////
    static_assert(
        std::is_arithmetic<T>::value, "Math only supports arithmetic types"
    );

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief The minimum of two values
    ///
    /// \param a The first value
    /// \param b The second value
    ///
    /// \return The minimum of the two values
    ///
    ///////////////////////////////////////////////////////////////////////////
    static T Min(const T a, const T b) { return (a < b) ? a : b; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief The maximum of two values
    ///
    /// \param a The first value
    /// \param b The second value
    ///
    /// \return The maximum of the two values
    ///
    ///////////////////////////////////////////////////////////////////////////
    static T Max(const T a, const T b) { return (a > b) ? a : b; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Clamps a value between a minimum and maximum value
    ///
    /// \param value The value to clamp
    /// \param min The minimum value
    /// \param max The maximum value
    ///
    /// \return The clamped value
    ///
    ///////////////////////////////////////////////////////////////////////////
    static T Clamp(const T value, const T min, const T max)
    {
        return Math::Min(Math::Max(value, min), max);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Linearly interpolates between two values
    ///
    /// \param a The first value
    /// \param b The second value
    /// \param alpha The interpolation factor (0.0 - x.0)
    ///
    /// \return The interpolated value
    ///
    ///////////////////////////////////////////////////////////////////////////
    static T Lerp(const T a, const T b, const Float32 alpha)
    {
        return a + (b - a) * alpha;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Linearly interpolates between two values with clamping
    ///
    /// \param a The first value
    /// \param b The second value
    /// \param alpha The interpolation factor (0.0 - 1.0)
    ///
    /// \return The interpolated value
    ///
    ///////////////////////////////////////////////////////////////////////////
    static T LerpClamped(const T a, const T b, Float32 alpha)
    {
        alpha = Clamp(alpha, 0.0f, 1.0f);
        return Lerp(a, b, alpha);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Checks if a value is NaN (Not a Number)
    ///
    /// \param value The value to check
    ///
    /// \return True if the value is NaN, false otherwise
    ///
    /// \note This function only works for floating point types. For other
    /// types, it will always return false.
    ///
    ///////////////////////////////////////////////////////////////////////////
    static Bool IsNaN(const T value)
    {
        if constexpr (std::is_floating_point<T>::value)
        {
            return value != value;
        }
        else { return false; }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns the absolute value
    ///
    /// \param value The value
    ///
    /// \return The absolute value
    ///
    ///////////////////////////////////////////////////////////////////////////
    static T Abs(const T value)
    {
        if constexpr (std::is_signed<T>::value)
        {
            return (value < 0) ? -value : value;
        }
        else { return value; }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief The sign of a value
    ///
    /// \param value The value
    ///
    /// \return -1 if negative, 1 if positive, 0 if zero
    ///
    ///////////////////////////////////////////////////////////////////////////
    static T Sign(const T value)
    {
        if (value > 0) { return T(1); }
        if (value < 0) { return T(-1); }
        return T(0);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief The square root of a value
    ///
    /// \param value The value
    ///
    /// \return The square root of the value
    ///
    /// \note This function only works for floating point types.
    ///
    ///////////////////////////////////////////////////////////////////////////
    static T Sqrt(const T value)
    {
        if constexpr (std::is_floating_point<T>::value)
        {
            return std::sqrt(value);
        }
        else { return value; }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief The power of a value
    ///
    /// \param base The base value
    /// \param exponent The exponent value
    ///
    /// \return The power of the value
    ///
    ///////////////////////////////////////////////////////////////////////////
    static T Pow(const T base, const T exponent)
    {
        return std::pow(base, exponent);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Converts degrees to radians
    ///
    /// \param degrees The value in degrees
    ///
    /// \return The value in radians
    ///
    ///////////////////////////////////////////////////////////////////////////
    static T DegreesToRadians(const T degrees)
    {
        return degrees * (3.14159265358979323846 / 180.0);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Converts radians to degrees
    ///
    /// \param radians The value in radians
    ///
    /// \return The value in degrees
    ///
    ///////////////////////////////////////////////////////////////////////////
    static T RadiansToDegrees(const T radians)
    {
        return radians * (180.0 / 3.14159265358979323846);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Checks if an integer is even
    ///
    /// \param value The value to check
    ///
    /// \return True if the value is even, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    static Bool IsEven(const T value)
    {
        if constexpr (std::is_integral<T>::value) { return (value % 2) == 0; }
        else { return false; }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Checks if an integer is odd
    ///
    /// \param value The value to check
    ///
    /// \return True if the value is odd, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    static Bool IsOdd(const T value)
    {
        if constexpr (std::is_integral<T>::value) { return (value % 2) != 0; }
        else { return false; }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Checks if an integer is a power of two
    ///
    /// \param value The integer to check
    ///
    /// \return True if the value is a power of two, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    static Bool IsPowerOfTwo(const T value)
    {
        if constexpr (std::is_integral<T>::value)
        {
            return (value > 0) && ((value & (value - 1)) == 0);
        }
        else { return false; }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Calculates the next power of two
    ///
    /// \param value The integer to check
    ///
    /// \return The next power of two
    ///
    ///////////////////////////////////////////////////////////////////////////
    static T NextPowerOfTwo(T value)
    {
        if constexpr (std::is_integral<T>::value)
        {
            if (value <= 0) { return 1; }
            value--;
            value |= value >> 1;
            value |= value >> 2;
            value |= value >> 4;
            value |= value >> 8;
            value |= value >> 16;
            if constexpr (sizeof(T) > 4) { value |= value >> 32; }
            value++;
            return value;
        }
        else { return value; }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Normalizes a value from a given range to [0, 1]
    ///
    /// \param value The value to normalize
    /// \param min The minimum of the input range
    /// \param max The maximum of the input range
    ///
    /// \return The normalized value
    ///
    ///////////////////////////////////////////////////////////////////////////
    static T Normalize(const T value, const T min, const T max)
    {
        return (value - min) / (max - min);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Smoother step interpolation
    ///
    /// \param edge0 The lower edge of the transition
    /// \param edge1 The upper edge of the transition
    /// \param x The value to interpolate
    ///
    /// \return The interpolated value
    ///
    ///////////////////////////////////////////////////////////////////////////
    static T SmootherStep(const T edge0, const T edge1, const T x)
    {
        T t = Clamp((x - edge0) / (edge1 - edge0), (T)0, (T)1);
        return t * t * t * (t * (t * 6 - 15) + 10);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Step function
    ///
    /// \param edge The threshold value
    /// \param x The value to test
    ///
    /// \return 0 if x < edge, 1 otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    static T Step(const T edge, const T x) { return (x < edge) ? (T)0 : (T)1; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Wraps a value around a range
    ///
    /// \param value The value to wrap
    /// \param min The minimum of the range
    /// \param max The maximum of the range
    ///
    /// \return The wrapped value
    ///
    ///////////////////////////////////////////////////////////////////////////
    static T Wrap(const T value, const T min, const T max)
    {
        const T range = max - min;
        if (range == 0) { return min; }
        return value - range * std::floor((value - min) / range);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Checks if two floating point values are nearly equal
    ///
    /// \param a The first value
    /// \param b The second value
    /// \param epsilon The tolerance
    ///
    /// \return True if the values are nearly equal, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    static Bool IsNearlyEqual(
        const T a,
        const T b,
        const T epsilon = std::numeric_limits<T>::epsilon()
    )
    {
        if constexpr (std::is_floating_point<T>::value)
        {
            return Abs(a - b) <= epsilon;
        }
        else { return a == b; }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief The ceiling of a value
    ///
    /// \param value The value
    ///
    /// \return The smallest integer value not less than the value
    ///
    ///////////////////////////////////////////////////////////////////////////
    static T Ceil(const T value) { return std::ceil(value); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief The floor of a value
    ///
    /// \param value The value
    ///
    /// \return The largest integer value not greater than the value
    ///
    ///////////////////////////////////////////////////////////////////////////
    static T Floor(const T value) { return std::floor(value); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Rounds a value to the nearest integer
    ///
    /// \param value The value to round
    ///
    /// \return The rounded value
    ///
    ///////////////////////////////////////////////////////////////////////////
    static T Round(const T value) { return std::round(value); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Linearly interpolates between two angles
    ///
    /// \param a The first angle in radians
    /// \param b The second angle in radians
    /// \param alpha The interpolation factor (0.0 - 1.0)
    ///
    /// \return The interpolated angle in radians
    ///
    ///////////////////////////////////////////////////////////////////////////
    static T LerpAngle(const T a, const T b, const T alpha)
    {
        T difference = std::fmod(b - a, static_cast<T>(2 * std::numbers::pi));
        if (difference > static_cast<T>(std::numbers::pi))
        {
            difference -= static_cast<T>(2 * std::numbers::pi);
        }
        else if (difference < -static_cast<T>(std::numbers::pi))
        {
            difference += static_cast<T>(2 * std::numbers::pi);
        }
        return a + difference * alpha;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Checks if a value is infinity
    ///
    /// \param value The value to check
    ///
    /// \return True if the value is infinity, false otherwise
    ///
    /// \note This function only works for floating point types.
    ///
    ///////////////////////////////////////////////////////////////////////////
    static Bool IsInfinity(const T value)
    {
        if constexpr (std::is_floating_point<T>::value)
        {
            return std::isinf(value);
        }
        else { return false; }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns the fractional part of a value
    ///
    /// \param value The value
    ///
    /// \return The fractional part
    ///
    ///////////////////////////////////////////////////////////////////////////
    static T Fract(const T value) { return value - std::floor(value); }
};

}   // namespace tkd
