///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Core/Math/TVector3.hpp>
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

TEST_CASE("TVector3 Construction", "[TVector3]")
{
    SECTION("Default Constructor")
    {
        TVector3<float> vec;
        REQUIRE(vec.x == 0.0f);
        REQUIRE(vec.y == 0.0f);
        REQUIRE(vec.z == 0.0f);
    }

    SECTION("Constructor with values")
    {
        TVector3<float> vec(1.0f, 2.0f, 3.0f);
        REQUIRE(vec.x == 1.0f);
        REQUIRE(vec.y == 2.0f);
        REQUIRE(vec.z == 3.0f);
    }

    SECTION("Copy Constructor")
    {
        TVector3<float> original(1.0f, 2.0f, 3.0f);
        TVector3<float> copy(original);
        REQUIRE(copy.x == 1.0f);
        REQUIRE(copy.y == 2.0f);
        REQUIRE(copy.z == 3.0f);
    }

    SECTION("Move Constructor")
    {
        TVector3<float> original(1.0f, 2.0f, 3.0f);
        TVector3<float> moved(std::move(original));
        REQUIRE(moved.x == 1.0f);
        REQUIRE(moved.y == 2.0f);
        REQUIRE(moved.z == 3.0f);
    }

    SECTION("Constructor from different type")
    {
        TVector3<float> vec(1, 2, 3);
        REQUIRE(vec.x == 1.0f);
        REQUIRE(vec.y == 2.0f);
        REQUIRE(vec.z == 3.0f);
    }

    SECTION("Constructor from TVector3 of different type")
    {
        TVector3<int> original(1, 2, 3);
        TVector3<float> converted(original);
        REQUIRE(converted.x == 1.0f);
        REQUIRE(converted.y == 2.0f);
        REQUIRE(converted.z == 3.0f);
    }
}

TEST_CASE("TVector3 Assignment", "[TVector3]")
{
    SECTION("Copy Assignment")
    {
        TVector3<float> vec1(1.0f, 2.0f, 3.0f);
        TVector3<float> vec2;
        vec2 = vec1;
        REQUIRE(vec2.x == 1.0f);
        REQUIRE(vec2.y == 2.0f);
        REQUIRE(vec2.z == 3.0f);
    }

    SECTION("Move Assignment")
    {
        TVector3<float> vec1(1.0f, 2.0f, 3.0f);
        TVector3<float> vec2;
        vec2 = std::move(vec1);
        REQUIRE(vec2.x == 1.0f);
        REQUIRE(vec2.y == 2.0f);
        REQUIRE(vec2.z == 3.0f);
    }
}

TEST_CASE("TVector3 Subscript Operator", "[TVector3]")
{
    TVector3<float> vec(1.0f, 2.0f, 3.0f);
    REQUIRE(vec[0] == 1.0f);
    REQUIRE(vec[1] == 2.0f);
    REQUIRE(vec[2] == 3.0f);

    vec[0] = 4.0f;
    vec[1] = 5.0f;
    vec[2] = 6.0f;
    REQUIRE(vec.x == 4.0f);
    REQUIRE(vec.y == 5.0f);
    REQUIRE(vec.z == 6.0f);
}

TEST_CASE("TVector3 Increment/Decrement", "[TVector3]")
{
    TVector3<int> vec(1, 2, 3);

    SECTION("Pre-increment")
    {
        TVector3<int> result = ++vec;
        REQUIRE(result.x == 2);
        REQUIRE(result.y == 3);
        REQUIRE(result.z == 4);
        REQUIRE(vec.x == 2);
        REQUIRE(vec.y == 3);
        REQUIRE(vec.z == 4);
    }

    SECTION("Post-increment")
    {
        TVector3<int> result = vec++;
        REQUIRE(result.x == 1);
        REQUIRE(result.y == 2);
        REQUIRE(result.z == 3);
        REQUIRE(vec.x == 2);
        REQUIRE(vec.y == 3);
        REQUIRE(vec.z == 4);
    }

    SECTION("Pre-decrement")
    {
        TVector3<int> result = --vec;
        REQUIRE(result.x == 0);
        REQUIRE(result.y == 1);
        REQUIRE(result.z == 2);
        REQUIRE(vec.x == 0);
        REQUIRE(vec.y == 1);
        REQUIRE(vec.z == 2);
    }

    SECTION("Post-decrement")
    {
        TVector3<int> result = vec--;
        REQUIRE(result.x == 1);
        REQUIRE(result.y == 2);
        REQUIRE(result.z == 3);
        REQUIRE(vec.x == 0);
        REQUIRE(vec.y == 1);
        REQUIRE(vec.z == 2);
    }
}

TEST_CASE("TVector3 Arithmetic Operators", "[TVector3]")
{
    TVector3<float> a(1.0f, 2.0f, 3.0f);
    TVector3<float> b(4.0f, 5.0f, 6.0f);
    float scalar = 2.0f;

    SECTION("Unary Plus")
    {
        TVector3<float> result = +a;
        REQUIRE(result.x == 1.0f);
        REQUIRE(result.y == 2.0f);
        REQUIRE(result.z == 3.0f);
    }

    SECTION("Unary Minus")
    {
        TVector3<float> result = -a;
        REQUIRE(result.x == -1.0f);
        REQUIRE(result.y == -2.0f);
        REQUIRE(result.z == -3.0f);
    }

    SECTION("Vector Addition")
    {
        TVector3<float> result = a + b;
        REQUIRE(result.x == 5.0f);
        REQUIRE(result.y == 7.0f);
        REQUIRE(result.z == 9.0f);
    }

    SECTION("Scalar Addition (vector + scalar)")
    {
        TVector3<float> result = a + scalar;
        REQUIRE(result.x == 3.0f);
        REQUIRE(result.y == 4.0f);
        REQUIRE(result.z == 5.0f);
    }

    SECTION("Scalar Addition (scalar + vector)")
    {
        TVector3<float> result = scalar + a;
        REQUIRE(result.x == 3.0f);
        REQUIRE(result.y == 4.0f);
        REQUIRE(result.z == 5.0f);
    }

    SECTION("Vector Subtraction")
    {
        TVector3<float> result = b - a;
        REQUIRE(result.x == 3.0f);
        REQUIRE(result.y == 3.0f);
        REQUIRE(result.z == 3.0f);
    }

    SECTION("Scalar Subtraction (vector - scalar)")
    {
        TVector3<float> result = b - scalar;
        REQUIRE(result.x == 2.0f);
        REQUIRE(result.y == 3.0f);
        REQUIRE(result.z == 4.0f);
    }

    SECTION("Scalar Subtraction (scalar - vector)")
    {
        TVector3<float> result = scalar - a;
        REQUIRE(result.x == 1.0f);
        REQUIRE(result.y == 0.0f);
        REQUIRE(result.z == -1.0f);
    }

    SECTION("Vector Multiplication")
    {
        TVector3<float> result = a * b;
        REQUIRE(result.x == 4.0f);
        REQUIRE(result.y == 10.0f);
        REQUIRE(result.z == 18.0f);
    }

    SECTION("Scalar Multiplication (vector * scalar)")
    {
        TVector3<float> result = a * scalar;
        REQUIRE(result.x == 2.0f);
        REQUIRE(result.y == 4.0f);
        REQUIRE(result.z == 6.0f);
    }

    SECTION("Scalar Multiplication (scalar * vector)")
    {
        TVector3<float> result = scalar * a;
        REQUIRE(result.x == 2.0f);
        REQUIRE(result.y == 4.0f);
        REQUIRE(result.z == 6.0f);
    }

    SECTION("Vector Division")
    {
        TVector3<float> result = b / a;
        REQUIRE(result.x == 4.0f);
        REQUIRE(result.y == 2.5f);
        REQUIRE(result.z == 2.0f);
    }

    SECTION("Scalar Division (vector / scalar)")
    {
        TVector3<float> result = a / scalar;
        REQUIRE(result.x == 0.5f);
        REQUIRE(result.y == 1.0f);
        REQUIRE(result.z == 1.5f);
    }

    SECTION("Scalar Division (scalar / vector)")
    {
        TVector3<float> result = scalar / a;
        REQUIRE(result.x == 2.0f);
        REQUIRE(result.y == 1.0f);
        REQUIRE(result.z == Approx(2.0f / 3.0f));
    }
}

TEST_CASE("TVector3 Compound Assignment", "[TVector3]")
{
    TVector3<float> a(1.0f, 2.0f, 3.0f);
    TVector3<float> b(4.0f, 5.0f, 6.0f);
    float scalar = 2.0f;

    SECTION("+= Vector")
    {
        a += b;
        REQUIRE(a.x == 5.0f);
        REQUIRE(a.y == 7.0f);
        REQUIRE(a.z == 9.0f);
    }

    SECTION("+= Scalar")
    {
        a += scalar;
        REQUIRE(a.x == 3.0f);
        REQUIRE(a.y == 4.0f);
        REQUIRE(a.z == 5.0f);
    }

    SECTION("-= Vector")
    {
        b -= a;
        REQUIRE(b.x == 3.0f);
        REQUIRE(b.y == 3.0f);
        REQUIRE(b.z == 3.0f);
    }

    SECTION("-= Scalar")
    {
        b -= scalar;
        REQUIRE(b.x == 2.0f);
        REQUIRE(b.y == 3.0f);
        REQUIRE(b.z == 4.0f);
    }

    SECTION("*= Vector")
    {
        a *= b;
        REQUIRE(a.x == 4.0f);
        REQUIRE(a.y == 10.0f);
        REQUIRE(a.z == 18.0f);
    }

    SECTION("*= Scalar")
    {
        a *= scalar;
        REQUIRE(a.x == 2.0f);
        REQUIRE(a.y == 4.0f);
        REQUIRE(a.z == 6.0f);
    }

    SECTION("/= Vector")
    {
        b /= a;
        REQUIRE(b.x == 4.0f);
        REQUIRE(b.y == 2.5f);
        REQUIRE(b.z == 2.0f);
    }

    SECTION("/= Scalar")
    {
        a /= scalar;
        REQUIRE(a.x == 0.5f);
        REQUIRE(a.y == 1.0f);
        REQUIRE(a.z == 1.5f);
    }
}

TEST_CASE("TVector3 Comparison Operators", "[TVector3]")
{
    TVector3<float> a(1.0f, 2.0f, 3.0f);
    TVector3<float> b(1.0f, 2.0f, 3.0f);
    TVector3<float> c(4.0f, 5.0f, 6.0f);
    float scalar = 2.0f;

    SECTION("== Vector")
    {
        REQUIRE(a == b);
        REQUIRE(!(a == c));
    }

    SECTION("== Scalar")
    {
        REQUIRE(!(a == scalar));
        TVector3<float> d(2.0f, 2.0f, 2.0f);
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
        TVector3<float> d(2.0f, 2.0f, 2.0f);
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

TEST_CASE("TVector3 Static Constants", "[TVector3]")
{
    REQUIRE(TVector3<float>::Zero.x == 0.0f);
    REQUIRE(TVector3<float>::Zero.y == 0.0f);
    REQUIRE(TVector3<float>::Zero.z == 0.0f);

    REQUIRE(TVector3<float>::One.x == 1.0f);
    REQUIRE(TVector3<float>::One.y == 1.0f);
    REQUIRE(TVector3<float>::One.z == 1.0f);

    REQUIRE(TVector3<float>::UnitX.x == 1.0f);
    REQUIRE(TVector3<float>::UnitX.y == 0.0f);
    REQUIRE(TVector3<float>::UnitX.z == 0.0f);

    REQUIRE(TVector3<float>::UnitY.x == 0.0f);
    REQUIRE(TVector3<float>::UnitY.y == 1.0f);
    REQUIRE(TVector3<float>::UnitY.z == 0.0f);

    REQUIRE(TVector3<float>::UnitZ.x == 0.0f);
    REQUIRE(TVector3<float>::UnitZ.y == 0.0f);
    REQUIRE(TVector3<float>::UnitZ.z == 1.0f);
}

TEST_CASE("TVector3 Output Operator", "[TVector3]")
{
    TVector3<float> vec(1.0f, 2.0f, 3.0f);
    std::stringstream ss;
    ss << vec;
    REQUIRE(ss.str() == "(1, 2, 3)");
}

TEST_CASE("TVector3 Free Functions", "[TVector3]")
{
    TVector3<float> a(1.0f, 2.0f, 3.0f);
    TVector3<float> b(4.0f, 5.0f, 6.0f);
    TVector3<float> c(0.5f, 1.5f, 2.5f);

    SECTION("Add")
    {
        auto result = Add(a, b);
        REQUIRE(result.x == 5.0f);
        REQUIRE(result.y == 7.0f);
        REQUIRE(result.z == 9.0f);
    }

    SECTION("Subtract")
    {
        auto result = Subtract(b, a);
        REQUIRE(result.x == 3.0f);
        REQUIRE(result.y == 3.0f);
        REQUIRE(result.z == 3.0f);
    }

    SECTION("Multiply")
    {
        auto result = Multiply(a, b);
        REQUIRE(result.x == 4.0f);
        REQUIRE(result.y == 10.0f);
        REQUIRE(result.z == 18.0f);
    }

    SECTION("Divide")
    {
        auto result = Divide(b, a);
        REQUIRE(result.x == 4.0f);
        REQUIRE(result.y == 2.5f);
        REQUIRE(result.z == 2.0f);
    }

    SECTION("MultiplyAdd")
    {
        auto result = MultiplyAdd(a, b, c);
        REQUIRE(result.x == 4.5f);
        REQUIRE(result.y == 11.5f);
        REQUIRE(result.z == 20.5f);
    }

    SECTION("Cross")
    {
        auto result = Cross(a, b);
        REQUIRE(result.x == -3.0f);
        REQUIRE(result.y == 6.0f);
        REQUIRE(result.z == -3.0f);
    }

    SECTION("Project")
    {
        TVector3<float> proj = Project(a, b);
        // Projection of a onto b
        float dot = 1 * 4 + 2 * 5 + 3 * 6;     // 32
        float lenSq = 4 * 4 + 5 * 5 + 6 * 6;   // 77
        float scale = dot / lenSq;
        REQUIRE(proj.x == Approx(4 * scale));
        REQUIRE(proj.y == Approx(5 * scale));
        REQUIRE(proj.z == Approx(6 * scale));
    }

    SECTION("Reflect")
    {
        TVector3<float> normal(0.0f, 1.0f, 0.0f);   // Unit Y
        auto result = Reflect(a, normal);
        // Reflection formula
        REQUIRE(result.x == 1.0f - 2 * 2 * 0.0f);
        REQUIRE(result.y == 2.0f - 2 * 2 * 1.0f);
        REQUIRE(result.z == 3.0f - 2 * 2 * 0.0f);
    }

    SECTION("Distance")
    {
        auto dist = Distance(a, b);
        REQUIRE(dist == Approx(std::sqrt(3 * 3 + 3 * 3 + 3 * 3)));
    }

    SECTION("Length")
    {
        auto len = Length(a);
        REQUIRE(len == Approx(std::sqrt(1 + 4 + 9)));
    }

    SECTION("Absolute")
    {
        TVector3<float> neg(-1.0f, -2.0f, 3.0f);
        auto result = Absolute(neg);
        REQUIRE(result.x == 1.0f);
        REQUIRE(result.y == 2.0f);
        REQUIRE(result.z == 3.0f);
    }

    SECTION("Normalize")
    {
        auto result = Normalize(a);
        float len = Length(a);
        REQUIRE(result.x == Approx(1.0f / len));
        REQUIRE(result.y == Approx(2.0f / len));
        REQUIRE(result.z == Approx(3.0f / len));
    }

    SECTION("Minimum")
    {
        auto result = Minimum(a, b);
        REQUIRE(result.x == 1.0f);
        REQUIRE(result.y == 2.0f);
        REQUIRE(result.z == 3.0f);
    }

    SECTION("Maximum")
    {
        auto result = Maximum(a, b);
        REQUIRE(result.x == 4.0f);
        REQUIRE(result.y == 5.0f);
        REQUIRE(result.z == 6.0f);
    }

    SECTION("Floor")
    {
        TVector3<float> vec(1.7f, 2.3f, 3.9f);
        auto result = Floor(vec);
        REQUIRE(result.x == 1.0f);
        REQUIRE(result.y == 2.0f);
        REQUIRE(result.z == 3.0f);
    }

    SECTION("Ceil")
    {
        TVector3<float> vec(1.1f, 2.9f, 3.0f);
        auto result = Ceil(vec);
        REQUIRE(result.x == 2.0f);
        REQUIRE(result.y == 3.0f);
        REQUIRE(result.z == 3.0f);
    }

    SECTION("Fraction")
    {
        TVector3<float> vec(1.7f, 2.3f, 3.9f);
        auto result = Fraction(vec);
        REQUIRE(result.x == Approx(0.7f));
        REQUIRE(result.y == Approx(0.3f));
        REQUIRE(result.z == Approx(0.9f));
    }

    SECTION("Modulo")
    {
        TVector3<float> vec(5.5f, 6.7f, 7.2f);
        auto result = Modulo(vec, 3.0f);
        REQUIRE(result.x == Approx(2.5f));
        REQUIRE(result.y == Approx(0.7f));
        REQUIRE(result.z == Approx(1.2f));
    }

    SECTION("Wrap")
    {
        TVector3<float> vec(5.0f, -1.0f, 3.5f);
        auto result = Wrap(vec, 0.0f, 4.0f);
        REQUIRE(result.x == Approx(1.0f));
        REQUIRE(result.y == Approx(3.0f));
        REQUIRE(result.z == Approx(3.5f));
    }

    SECTION("Snap")
    {
        TVector3<float> vec(1.3f, 2.7f, 3.9f);
        auto result = Snap(vec, 2.0f);
        REQUIRE(result.x == 2.0f);
        REQUIRE(result.y == 2.0f);
        REQUIRE(result.z == 4.0f);
    }

    SECTION("Sine")
    {
        TVector3<float> vec(0.0f, 1.570796f, 3.14159f);   // 0, pi/2, pi
        auto result = Sine(vec);
        REQUIRE(result.x == Approx(0.0f).margin(1e-5f));
        REQUIRE(result.y == Approx(1.0f));
        REQUIRE(result.z == Approx(0.0f).margin(1e-5f));
    }

    SECTION("Cosine")
    {
        TVector3<float> vec(0.0f, 1.570796f, 3.14159f);
        auto result = Cosine(vec);
        REQUIRE(result.x == Approx(1.0f));
        REQUIRE(result.y == Approx(0.0f).margin(1e-5f));
        REQUIRE(result.z == Approx(-1.0f));
    }

    SECTION("Tangent")
    {
        TVector3<float> vec(0.0f, 0.785398f, 1.570796f);   // 0, pi/4, pi/2
        auto result = Tangent(vec);
        REQUIRE(result.x == Approx(0.0f).margin(1e-5f));
        REQUIRE(result.y == Approx(1.0f));
        REQUIRE(result.z == Approx(std::tan(1.570796f)));   // Large number
    }
}

}   // namespace tkd
