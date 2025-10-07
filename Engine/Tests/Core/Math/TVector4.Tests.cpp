///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Core/Math/TVector4.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cmath>
#include <sstream>

using Catch::Approx;

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
// Test Cases
///////////////////////////////////////////////////////////////////////////////

TEST_CASE("TVector4 Construction", "[TVector4]")
{
    SECTION("Default Constructor")
    {
        TVector4<float> vec;
        REQUIRE(vec.x == 0.0f);
        REQUIRE(vec.y == 0.0f);
        REQUIRE(vec.z == 0.0f);
        REQUIRE(vec.w == 0.0f);
    }

    SECTION("Constructor with values")
    {
        TVector4<float> vec(1.0f, 2.0f, 3.0f, 4.0f);
        REQUIRE(vec.x == 1.0f);
        REQUIRE(vec.y == 2.0f);
        REQUIRE(vec.z == 3.0f);
        REQUIRE(vec.w == 4.0f);
    }

    SECTION("Copy Constructor")
    {
        TVector4<float> original(1.0f, 2.0f, 3.0f, 4.0f);
        TVector4<float> copy(original);
        REQUIRE(copy.x == 1.0f);
        REQUIRE(copy.y == 2.0f);
        REQUIRE(copy.z == 3.0f);
        REQUIRE(copy.w == 4.0f);
    }

    SECTION("Move Constructor")
    {
        TVector4<float> original(1.0f, 2.0f, 3.0f, 4.0f);
        TVector4<float> moved(std::move(original));
        REQUIRE(moved.x == 1.0f);
        REQUIRE(moved.y == 2.0f);
        REQUIRE(moved.z == 3.0f);
        REQUIRE(moved.w == 4.0f);
    }

    SECTION("Constructor from different type")
    {
        TVector4<float> vec(1, 2, 3, 4);
        REQUIRE(vec.x == 1.0f);
        REQUIRE(vec.y == 2.0f);
        REQUIRE(vec.z == 3.0f);
        REQUIRE(vec.w == 4.0f);
    }

    SECTION("Constructor from TVector4 of different type")
    {
        TVector4<int> original(1, 2, 3, 4);
        TVector4<float> converted(original);
        REQUIRE(converted.x == 1.0f);
        REQUIRE(converted.y == 2.0f);
        REQUIRE(converted.z == 3.0f);
        REQUIRE(converted.w == 4.0f);
    }
}

TEST_CASE("TVector4 Assignment", "[TVector4]")
{
    SECTION("Copy Assignment")
    {
        TVector4<float> vec1(1.0f, 2.0f, 3.0f, 4.0f);
        TVector4<float> vec2;
        vec2 = vec1;
        REQUIRE(vec2.x == 1.0f);
        REQUIRE(vec2.y == 2.0f);
        REQUIRE(vec2.z == 3.0f);
        REQUIRE(vec2.w == 4.0f);
    }

    SECTION("Move Assignment")
    {
        TVector4<float> vec1(1.0f, 2.0f, 3.0f, 4.0f);
        TVector4<float> vec2;
        vec2 = std::move(vec1);
        REQUIRE(vec2.x == 1.0f);
        REQUIRE(vec2.y == 2.0f);
        REQUIRE(vec2.z == 3.0f);
        REQUIRE(vec2.w == 4.0f);
    }
}

TEST_CASE("TVector4 Subscript Operator", "[TVector4]")
{
    TVector4<float> vec(1.0f, 2.0f, 3.0f, 4.0f);
    REQUIRE(vec[0] == 1.0f);
    REQUIRE(vec[1] == 2.0f);
    REQUIRE(vec[2] == 3.0f);
    REQUIRE(vec[3] == 4.0f);

    vec[0] = 5.0f;
    vec[1] = 6.0f;
    vec[2] = 7.0f;
    vec[3] = 8.0f;
    REQUIRE(vec.x == 5.0f);
    REQUIRE(vec.y == 6.0f);
    REQUIRE(vec.z == 7.0f);
    REQUIRE(vec.w == 8.0f);
}

TEST_CASE("TVector4 Increment/Decrement", "[TVector4]")
{
    TVector4<int> vec(1, 2, 3, 4);

    SECTION("Pre-increment")
    {
        TVector4<int> result = ++vec;
        REQUIRE(result.x == 2);
        REQUIRE(result.y == 3);
        REQUIRE(result.z == 4);
        REQUIRE(result.w == 5);
        REQUIRE(vec.x == 2);
        REQUIRE(vec.y == 3);
        REQUIRE(vec.z == 4);
        REQUIRE(vec.w == 5);
    }

    SECTION("Post-increment")
    {
        TVector4<int> result = vec++;
        REQUIRE(result.x == 1);
        REQUIRE(result.y == 2);
        REQUIRE(result.z == 3);
        REQUIRE(result.w == 4);
        REQUIRE(vec.x == 2);
        REQUIRE(vec.y == 3);
        REQUIRE(vec.z == 4);
        REQUIRE(vec.w == 5);
    }

    SECTION("Pre-decrement")
    {
        TVector4<int> result = --vec;
        REQUIRE(result.x == 0);
        REQUIRE(result.y == 1);
        REQUIRE(result.z == 2);
        REQUIRE(result.w == 3);
        REQUIRE(vec.x == 0);
        REQUIRE(vec.y == 1);
        REQUIRE(vec.z == 2);
        REQUIRE(vec.w == 3);
    }

    SECTION("Post-decrement")
    {
        TVector4<int> result = vec--;
        REQUIRE(result.x == 1);
        REQUIRE(result.y == 2);
        REQUIRE(result.z == 3);
        REQUIRE(result.w == 4);
        REQUIRE(vec.x == 0);
        REQUIRE(vec.y == 1);
        REQUIRE(vec.z == 2);
        REQUIRE(vec.w == 3);
    }
}

TEST_CASE("TVector4 Arithmetic Operators", "[TVector4]")
{
    TVector4<float> a(1.0f, 2.0f, 3.0f, 4.0f);
    TVector4<float> b(5.0f, 6.0f, 7.0f, 8.0f);
    float scalar = 2.0f;

    SECTION("Unary Plus")
    {
        TVector4<float> result = +a;
        REQUIRE(result.x == 1.0f);
        REQUIRE(result.y == 2.0f);
        REQUIRE(result.z == 3.0f);
        REQUIRE(result.w == 4.0f);
    }

    SECTION("Unary Minus")
    {
        TVector4<float> result = -a;
        REQUIRE(result.x == -1.0f);
        REQUIRE(result.y == -2.0f);
        REQUIRE(result.z == -3.0f);
        REQUIRE(result.w == -4.0f);
    }

    SECTION("Vector Addition")
    {
        TVector4<float> result = a + b;
        REQUIRE(result.x == 6.0f);
        REQUIRE(result.y == 8.0f);
        REQUIRE(result.z == 10.0f);
        REQUIRE(result.w == 12.0f);
    }

    SECTION("Scalar Addition (vector + scalar)")
    {
        TVector4<float> result = a + scalar;
        REQUIRE(result.x == 3.0f);
        REQUIRE(result.y == 4.0f);
        REQUIRE(result.z == 5.0f);
        REQUIRE(result.w == 6.0f);
    }

    SECTION("Scalar Addition (scalar + vector)")
    {
        TVector4<float> result = scalar + a;
        REQUIRE(result.x == 3.0f);
        REQUIRE(result.y == 4.0f);
        REQUIRE(result.z == 5.0f);
        REQUIRE(result.w == 6.0f);
    }

    SECTION("Vector Subtraction")
    {
        TVector4<float> result = b - a;
        REQUIRE(result.x == 4.0f);
        REQUIRE(result.y == 4.0f);
        REQUIRE(result.z == 4.0f);
        REQUIRE(result.w == 4.0f);
    }

    SECTION("Scalar Subtraction (vector - scalar)")
    {
        TVector4<float> result = b - scalar;
        REQUIRE(result.x == 3.0f);
        REQUIRE(result.y == 4.0f);
        REQUIRE(result.z == 5.0f);
        REQUIRE(result.w == 6.0f);
    }

    SECTION("Scalar Subtraction (scalar - vector)")
    {
        TVector4<float> result = scalar - a;
        REQUIRE(result.x == 1.0f);
        REQUIRE(result.y == 0.0f);
        REQUIRE(result.z == -1.0f);
        REQUIRE(result.w == -2.0f);
    }

    SECTION("Vector Multiplication")
    {
        TVector4<float> result = a * b;
        REQUIRE(result.x == 5.0f);
        REQUIRE(result.y == 12.0f);
        REQUIRE(result.z == 21.0f);
        REQUIRE(result.w == 32.0f);
    }

    SECTION("Scalar Multiplication (vector * scalar)")
    {
        TVector4<float> result = a * scalar;
        REQUIRE(result.x == 2.0f);
        REQUIRE(result.y == 4.0f);
        REQUIRE(result.z == 6.0f);
        REQUIRE(result.w == 8.0f);
    }

    SECTION("Scalar Multiplication (scalar * vector)")
    {
        TVector4<float> result = scalar * a;
        REQUIRE(result.x == 2.0f);
        REQUIRE(result.y == 4.0f);
        REQUIRE(result.z == 6.0f);
        REQUIRE(result.w == 8.0f);
    }

    SECTION("Vector Division")
    {
        TVector4<float> result = b / a;
        REQUIRE(result.x == 5.0f);
        REQUIRE(result.y == 3.0f);
        REQUIRE(result.z == 7.0f / 3.0f);
        REQUIRE(result.w == 2.0f);
    }

    SECTION("Scalar Division (vector / scalar)")
    {
        TVector4<float> result = a / scalar;
        REQUIRE(result.x == 0.5f);
        REQUIRE(result.y == 1.0f);
        REQUIRE(result.z == 1.5f);
        REQUIRE(result.w == 2.0f);
    }

    SECTION("Scalar Division (scalar / vector)")
    {
        TVector4<float> result = scalar / a;
        REQUIRE(result.x == 2.0f);
        REQUIRE(result.y == 1.0f);
        REQUIRE(result.z == 2.0f / 3.0f);
        REQUIRE(result.w == 0.5f);
    }
}

TEST_CASE("TVector4 Compound Assignment", "[TVector4]")
{
    TVector4<float> a(1.0f, 2.0f, 3.0f, 4.0f);
    TVector4<float> b(5.0f, 6.0f, 7.0f, 8.0f);
    float scalar = 2.0f;

    SECTION("+= Vector")
    {
        a += b;
        REQUIRE(a.x == 6.0f);
        REQUIRE(a.y == 8.0f);
        REQUIRE(a.z == 10.0f);
        REQUIRE(a.w == 12.0f);
    }

    SECTION("+= Scalar")
    {
        a += scalar;
        REQUIRE(a.x == 3.0f);
        REQUIRE(a.y == 4.0f);
        REQUIRE(a.z == 5.0f);
        REQUIRE(a.w == 6.0f);
    }

    SECTION("-= Vector")
    {
        b -= a;
        REQUIRE(b.x == 4.0f);
        REQUIRE(b.y == 4.0f);
        REQUIRE(b.z == 4.0f);
        REQUIRE(b.w == 4.0f);
    }

    SECTION("-= Scalar")
    {
        b -= scalar;
        REQUIRE(b.x == 3.0f);
        REQUIRE(b.y == 4.0f);
        REQUIRE(b.z == 5.0f);
        REQUIRE(b.w == 6.0f);
    }

    SECTION("*= Vector")
    {
        a *= b;
        REQUIRE(a.x == 5.0f);
        REQUIRE(a.y == 12.0f);
        REQUIRE(a.z == 21.0f);
        REQUIRE(a.w == 32.0f);
    }

    SECTION("*= Scalar")
    {
        a *= scalar;
        REQUIRE(a.x == 2.0f);
        REQUIRE(a.y == 4.0f);
        REQUIRE(a.z == 6.0f);
        REQUIRE(a.w == 8.0f);
    }

    SECTION("/= Vector")
    {
        b /= a;
        REQUIRE(b.x == 5.0f);
        REQUIRE(b.y == 3.0f);
        REQUIRE(b.z == 7.0f / 3.0f);
        REQUIRE(b.w == 2.0f);
    }

    SECTION("/= Scalar")
    {
        a /= scalar;
        REQUIRE(a.x == 0.5f);
        REQUIRE(a.y == 1.0f);
        REQUIRE(a.z == 1.5f);
        REQUIRE(a.w == 2.0f);
    }
}

TEST_CASE("TVector4 Comparison Operators", "[TVector4]")
{
    TVector4<float> a(1.0f, 2.0f, 3.0f, 4.0f);
    TVector4<float> b(1.0f, 2.0f, 3.0f, 4.0f);
    TVector4<float> c(5.0f, 6.0f, 7.0f, 8.0f);
    float scalar = 2.0f;

    SECTION("== Vector")
    {
        REQUIRE(a == b);
        REQUIRE(!(a == c));
    }

    SECTION("== Scalar")
    {
        REQUIRE(!(a == scalar));
        TVector4<float> d(2.0f, 2.0f, 2.0f, 2.0f);
        REQUIRE(d == scalar);
    }

    SECTION("!= Vector")
    {
        REQUIRE(!(a != b));
        REQUIRE(a != c);
    }

    SECTION("!= Scalar")
    {
        REQUIRE(a != scalar);
        TVector4<float> d(2.0f, 2.0f, 2.0f, 2.0f);
        REQUIRE(!(d != scalar));
    }

    SECTION(">= Vector")
    {
        REQUIRE(a >= b);
        REQUIRE(c >= a);
        REQUIRE(!(a >= c));
    }

    SECTION(">= Scalar")
    {
        REQUIRE(c >= scalar);
        REQUIRE(!(a >= scalar));
    }

    SECTION("> Vector")
    {
        REQUIRE(c > a);
        REQUIRE(!(a > c));
        REQUIRE(!(a > b));
    }

    SECTION("> Scalar")
    {
        REQUIRE(c > scalar);
        REQUIRE(!(a > scalar));
    }

    SECTION("<= Vector")
    {
        REQUIRE(a <= b);
        REQUIRE(a <= c);
        REQUIRE(!(c <= a));
    }

    SECTION("<= Scalar")
    {
        REQUIRE(!(a <= scalar));
        REQUIRE(!(c <= scalar));
    }

    SECTION("< Vector")
    {
        REQUIRE(a < c);
        REQUIRE(!(c < a));
        REQUIRE(!(a < b));
    }

    SECTION("< Scalar")
    {
        REQUIRE(!(a < scalar));
        REQUIRE(!(c < scalar));
    }
}

TEST_CASE("TVector4 Static Constants", "[TVector4]")
{
    REQUIRE(TVector4<float>::Zero.x == 0.0f);
    REQUIRE(TVector4<float>::Zero.y == 0.0f);
    REQUIRE(TVector4<float>::Zero.z == 0.0f);
    REQUIRE(TVector4<float>::Zero.w == 0.0f);

    REQUIRE(TVector4<float>::One.x == 1.0f);
    REQUIRE(TVector4<float>::One.y == 1.0f);
    REQUIRE(TVector4<float>::One.z == 1.0f);
    REQUIRE(TVector4<float>::One.w == 1.0f);

    REQUIRE(TVector4<float>::UnitX.x == 1.0f);
    REQUIRE(TVector4<float>::UnitX.y == 0.0f);
    REQUIRE(TVector4<float>::UnitX.z == 0.0f);
    REQUIRE(TVector4<float>::UnitX.w == 0.0f);

    REQUIRE(TVector4<float>::UnitY.x == 0.0f);
    REQUIRE(TVector4<float>::UnitY.y == 1.0f);
    REQUIRE(TVector4<float>::UnitY.z == 0.0f);
    REQUIRE(TVector4<float>::UnitY.w == 0.0f);

    REQUIRE(TVector4<float>::UnitZ.x == 0.0f);
    REQUIRE(TVector4<float>::UnitZ.y == 0.0f);
    REQUIRE(TVector4<float>::UnitZ.z == 1.0f);
    REQUIRE(TVector4<float>::UnitZ.w == 0.0f);

    REQUIRE(TVector4<float>::UnitW.x == 0.0f);
    REQUIRE(TVector4<float>::UnitW.y == 0.0f);
    REQUIRE(TVector4<float>::UnitW.z == 0.0f);
    REQUIRE(TVector4<float>::UnitW.w == 1.0f);
}

TEST_CASE("TVector4 Output Operator", "[TVector4]")
{
    TVector4<float> vec(1.0f, 2.0f, 3.0f, 4.0f);
    std::stringstream ss;
    ss << vec;
    REQUIRE(ss.str() == "(1, 2, 3, 4)");
}

TEST_CASE("TVector4 Free Functions", "[TVector4]")
{
    TVector4<float> a(1.0f, 2.0f, 3.0f, 4.0f);
    TVector4<float> b(5.0f, 6.0f, 7.0f, 8.0f);
    TVector4<float> c(0.5f, 1.5f, 2.5f, 3.5f);

    SECTION("Add")
    {
        auto result = Add(a, b);
        REQUIRE(result.x == 6.0f);
        REQUIRE(result.y == 8.0f);
        REQUIRE(result.z == 10.0f);
        REQUIRE(result.w == 12.0f);
    }

    SECTION("Subtract")
    {
        auto result = Subtract(b, a);
        REQUIRE(result.x == 4.0f);
        REQUIRE(result.y == 4.0f);
        REQUIRE(result.z == 4.0f);
        REQUIRE(result.w == 4.0f);
    }

    SECTION("Multiply")
    {
        auto result = Multiply(a, b);
        REQUIRE(result.x == 5.0f);
        REQUIRE(result.y == 12.0f);
        REQUIRE(result.z == 21.0f);
        REQUIRE(result.w == 32.0f);
    }

    SECTION("Divide")
    {
        auto result = Divide(b, a);
        REQUIRE(result.x == 5.0f);
        REQUIRE(result.y == 3.0f);
        REQUIRE(result.z == 7.0f / 3.0f);
        REQUIRE(result.w == 2.0f);
    }

    SECTION("MultiplyAdd")
    {
        auto result = MultiplyAdd(a, b, c);
        REQUIRE(result.x == 5.5f);
        REQUIRE(result.y == 13.5f);
        REQUIRE(result.z == 23.5f);
        REQUIRE(result.w == 35.5f);
    }

    SECTION("Distance")
    {
        auto dist = Distance(a, b);
        REQUIRE(dist == Approx(std::sqrt(4 * 4 + 4 * 4 + 4 * 4 + 4 * 4)));
    }

    SECTION("Length")
    {
        auto len = Length(a);
        REQUIRE(len == Approx(std::sqrt(1 + 4 + 9 + 16)));
    }

    SECTION("Absolute")
    {
        TVector4<float> neg(-1.0f, -2.0f, 3.0f, -4.0f);
        auto result = Absolute(neg);
        REQUIRE(result.x == 1.0f);
        REQUIRE(result.y == 2.0f);
        REQUIRE(result.z == 3.0f);
        REQUIRE(result.w == 4.0f);
    }

    SECTION("Normalize")
    {
        auto result = Normalize(a);
        float len = Length(a);
        REQUIRE(result.x == Approx(1.0f / len));
        REQUIRE(result.y == Approx(2.0f / len));
        REQUIRE(result.z == Approx(3.0f / len));
        REQUIRE(result.w == Approx(4.0f / len));
    }

    SECTION("Minimum")
    {
        auto result = Minimum(a, b);
        REQUIRE(result.x == 1.0f);
        REQUIRE(result.y == 2.0f);
        REQUIRE(result.z == 3.0f);
        REQUIRE(result.w == 4.0f);
    }

    SECTION("Maximum")
    {
        auto result = Maximum(a, b);
        REQUIRE(result.x == 5.0f);
        REQUIRE(result.y == 6.0f);
        REQUIRE(result.z == 7.0f);
        REQUIRE(result.w == 8.0f);
    }

    SECTION("Floor")
    {
        TVector4<float> vec(1.7f, 2.3f, 3.9f, 4.1f);
        auto result = Floor(vec);
        REQUIRE(result.x == 1.0f);
        REQUIRE(result.y == 2.0f);
        REQUIRE(result.z == 3.0f);
        REQUIRE(result.w == 4.0f);
    }

    SECTION("Ceil")
    {
        TVector4<float> vec(1.1f, 2.9f, 3.0f, 4.8f);
        auto result = Ceil(vec);
        REQUIRE(result.x == 2.0f);
        REQUIRE(result.y == 3.0f);
        REQUIRE(result.z == 3.0f);
        REQUIRE(result.w == 5.0f);
    }

    SECTION("Fraction")
    {
        TVector4<float> vec(1.7f, 2.3f, 3.9f, 4.1f);
        auto result = Fraction(vec);
        REQUIRE(result.x == Approx(0.7f));
        REQUIRE(result.y == Approx(0.3f));
        REQUIRE(result.z == Approx(0.9f));
        REQUIRE(result.w == Approx(0.1f));
    }

    SECTION("Modulo")
    {
        TVector4<float> vec(5.5f, 6.7f, 7.2f, 8.3f);
        auto result = Modulo(vec, 3.0f);
        REQUIRE(result.x == Approx(2.5f));
        REQUIRE(result.y == Approx(0.7f));
        REQUIRE(result.z == Approx(1.2f));
        REQUIRE(result.w == Approx(2.3f));
    }

    SECTION("Wrap")
    {
        TVector4<float> vec(5.0f, -1.0f, 3.5f, 6.0f);
        auto result = Wrap(vec, 0.0f, 4.0f);
        REQUIRE(result.x == Approx(1.0f));
        REQUIRE(result.y == Approx(3.0f));
        REQUIRE(result.z == Approx(3.5f));
        REQUIRE(result.w == Approx(2.0f));
    }

    SECTION("Snap")
    {
        TVector4<float> vec(1.3f, 2.7f, 3.9f, 4.1f);
        auto result = Snap(vec, 2.0f);
        REQUIRE(result.x == 2.0f);
        REQUIRE(result.y == 2.0f);
        REQUIRE(result.z == 4.0f);
        REQUIRE(result.w == 4.0f);
    }

    SECTION("Sine")
    {
        TVector4<float> vec(
            0.0f, 1.570796f, 3.14159f, 4.71239f
        );   // 0, pi/2, pi, 3pi/2
        auto result = Sine(vec);
        REQUIRE(result.x == Approx(0.0f).margin(1e-5f));
        REQUIRE(result.y == Approx(1.0f));
        REQUIRE(result.z == Approx(0.0f).margin(1e-5f));
        REQUIRE(result.w == Approx(-1.0f));
    }

    SECTION("Cosine")
    {
        TVector4<float> vec(0.0f, 1.570796f, 3.14159f, 4.71239f);
        auto result = Cosine(vec);
        REQUIRE(result.x == Approx(1.0f));
        REQUIRE(result.y == Approx(0.0f).margin(1e-5f));
        REQUIRE(result.z == Approx(-1.0f));
        REQUIRE(result.w == Approx(0.0f).margin(1e-5f));
    }

    SECTION("Tangent")
    {
        TVector4<float> vec(
            0.0f, 0.785398f, 1.570796f, 2.35619f
        );   // 0, pi/4, pi/2, 3pi/4
        auto result = Tangent(vec);
        REQUIRE(result.x == Approx(0.0f).margin(1e-5f));
        REQUIRE(result.y == Approx(1.0f));
        REQUIRE(result.z == Approx(std::tan(1.570796f)));   // Large number
        REQUIRE(result.w == Approx(-1.0f));
    }
}

}   // namespace tkd
