///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Core/Math/Geometry/TCircle.hpp>
#include <Engine/Core/Math/Geometry/TCube.hpp>
#include <Engine/Core/Math/Geometry/TRectangle.hpp>
#include <Engine/Core/Math/Geometry/TSphere.hpp>
#include <Engine/Core/Math/Geometry/TSquare.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
// Forward Declarations
///////////////////////////////////////////////////////////////////////////////
using FCircle = TCircle<float>;
using FCube = TCube<float>;
using FRectangle = TRectangle<float>;
using FSphere = TSphere<float>;
using FSquare = TSquare<float>;

///////////////////////////////////////////////////////////////////////////////
// Typed Forward Declarations
///////////////////////////////////////////////////////////////////////////////
using FCirclef = TCircle<float>;
using FCircled = TCircle<double>;
using FCirclei = TCircle<int>;
using FCircleu = TCircle<unsigned int>;

using FCubef = TCube<float>;
using FCubed = TCube<double>;
using FCubei = TCube<int>;
using FCubeu = TCube<unsigned int>;

using FRectanglef = TRectangle<float>;
using FRectangled = TRectangle<double>;
using FRectanglei = TRectangle<int>;
using FRectangleu = TRectangle<unsigned int>;

using FSpheref = TSphere<float>;
using FSphered = TSphere<double>;
using FSpherei = TSphere<int>;
using FSphereu = TSphere<unsigned int>;

using FSquaref = TSquare<float>;
using FSquared = TSquare<double>;
using FSquarei = TSquare<int>;
using FSquareu = TSquare<unsigned int>;

} // !namespace tkd
