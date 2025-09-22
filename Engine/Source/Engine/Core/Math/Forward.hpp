///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Core/Math/TMatrix2x2.hpp>
#include <Engine/Core/Math/TMatrix3x3.hpp>
#include <Engine/Core/Math/TMatrix4x4.hpp>
#include <Engine/Core/Math/TQuaternion.hpp>
#include <Engine/Core/Math/TRotator.hpp>
#include <Engine/Core/Math/TRotator2D.hpp>
#include <Engine/Core/Math/TTransform.hpp>
#include <Engine/Core/Math/TTransform2D.hpp>
#include <Engine/Core/Math/TVector2.hpp>
#include <Engine/Core/Math/TVector3.hpp>
#include <Engine/Core/Math/TVector4.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
// Forward Declarations
///////////////////////////////////////////////////////////////////////////////
using FMatrix2x2 = TMatrix2x2<float>;
using FMatrix3x3 = TMatrix3x3<float>;
using FMatrix4x4 = TMatrix4x4<float>;
using FQuaternion = TQuaternion<float>;
using FRotator = TRotator<float>;
using FRotator2D = TRotator2D<float>;
using FTransform = TTransform<float>;
using FTransform2D = TTransform2D<float>;
using FVector2 = TVector2<float>;
using FVector3 = TVector3<float>;
using FVector4 = TVector4<float>;

///////////////////////////////////////////////////////////////////////////////
// Typed Forward Declarations
///////////////////////////////////////////////////////////////////////////////
using FMatrix2x2f = TMatrix2x2<float>;
using FMatrix2x2d = TMatrix2x2<double>;
using FMatrix2x2i = TMatrix2x2<int>;
using FMatrix2x2u = TMatrix2x2<unsigned int>;

using FMatrix3x3f = TMatrix3x3<float>;
using FMatrix3x3d = TMatrix3x3<double>;
using FMatrix3x3i = TMatrix3x3<int>;
using FMatrix3x3u = TMatrix3x3<unsigned int>;

using FMatrix4x4f = TMatrix4x4<float>;
using FMatrix4x4d = TMatrix4x4<double>;
using FMatrix4x4i = TMatrix4x4<int>;
using FMatrix4x4u = TMatrix4x4<unsigned int>;

using FQuaternionf = TQuaternion<float>;
using FQuaterniond = TQuaternion<double>;
using FQuaternioni = TQuaternion<int>;
using FQuaternionu = TQuaternion<unsigned int>;

using FRotatorf = TRotator<float>;
using FRotatord = TRotator<double>;
using FRotatori = TRotator<int>;
using FRotatoru = TRotator<unsigned int>;

using FRotator2Df = TRotator2D<float>;
using FRotator2Dd = TRotator2D<double>;
using FRotator2Di = TRotator2D<int>;
using FRotator2Du = TRotator2D<unsigned int>;

using FTransformf = TTransform<float>;
using FTransformd = TTransform<double>;
using FTransformi = TTransform<int>;
using FTransformu = TTransform<unsigned int>;

using FTransform2Df = TTransform2D<float>;
using FTransform2Dd = TTransform2D<double>;
using FTransform2Di = TTransform2D<int>;
using FTransform2Du = TTransform2D<unsigned int>;

using FVector2f = TVector2<float>;
using FVector2d = TVector2<double>;
using FVector2i = TVector2<int>;
using FVector2u = TVector2<unsigned int>;

using FVector3f = TVector3<float>;
using FVector3d = TVector3<double>;
using FVector3i = TVector3<int>;
using FVector3u = TVector3<unsigned int>;

using FVector4f = TVector4<float>;
using FVector4d = TVector4<double>;
using FVector4i = TVector4<int>;
using FVector4u = TVector4<unsigned int>;

}   // namespace tkd
