///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Core/Math/TVector2.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
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
TEST_CASE("TVector2<int> Tests", "[TVector2]")
{
    using Vec2i = TVector2<int>;

    SECTION("Default Constructor")
    {
        Vec2i v;
        REQUIRE(v.x == 0);
        REQUIRE(v.y == 0);
    }

    SECTION("Constructor with x, y")
    {
        Vec2i v(3, 4);
        REQUIRE(v.x == 3);
        REQUIRE(v.y == 4);
    }

    SECTION("Copy Constructor")
    {
        Vec2i original(1, 2);
        Vec2i copy(original);
        REQUIRE(copy.x == 1);
        REQUIRE(copy.y == 2);
    }

    SECTION("Move Constructor")
    {
        Vec2i original(5, 6);
        Vec2i moved(std::move(original));
        REQUIRE(moved.x == 5);
        REQUIRE(moved.y == 6);
    }

    SECTION("Constructor from different type")
    {
        Vec2i v(3.5f, 4.7f);   // Should cast to int
        REQUIRE(v.x == 3);
        REQUIRE(v.y == 4);
    }

    SECTION("Copy Assignment")
    {
        Vec2i v1(1, 2);
        Vec2i v2;
        v2 = v1;
        REQUIRE(v2.x == 1);
        REQUIRE(v2.y == 2);
    }

    SECTION("Move Assignment")
    {
        Vec2i v1(7, 8);
        Vec2i v2;
        v2 = std::move(v1);
        REQUIRE(v2.x == 7);
        REQUIRE(v2.y == 8);
    }

    SECTION("Array Access")
    {
        Vec2i v(10, 20);
        REQUIRE(v[0] == 10);
        REQUIRE(v[1] == 20);
        REQUIRE(v.data[0] == 10);
        REQUIRE(v.data[1] == 20);

        v[0] = 30;
        REQUIRE(v.x == 30);
    }

    SECTION("Increment Operators")
    {
        Vec2i v(1, 2);
        ++v;
        REQUIRE(v.x == 2);
        REQUIRE(v.y == 3);

        Vec2i post = v++;
        REQUIRE(post.x == 2);
        REQUIRE(post.y == 3);
        REQUIRE(v.x == 3);
        REQUIRE(v.y == 4);
    }

    SECTION("Decrement Operators")
    {
        Vec2i v(3, 4);
        --v;
        REQUIRE(v.x == 2);
        REQUIRE(v.y == 3);

        Vec2i post = v--;
        REQUIRE(post.x == 2);
        REQUIRE(post.y == 3);
        REQUIRE(v.x == 1);
        REQUIRE(v.y == 2);
    }

    SECTION("Arithmetic Operators")
    {
        Vec2i a(1, 2);
        Vec2i b(3, 4);

        // Vector + Vector
        Vec2i sum = a + b;
        REQUIRE(sum.x == 4);
        REQUIRE(sum.y == 6);

        // Vector + Scalar
        Vec2i sumS = a + 5;
        REQUIRE(sumS.x == 6);
        REQUIRE(sumS.y == 7);

        // Scalar + Vector
        Vec2i sumS2 = 5 + a;
        REQUIRE(sumS2.x == 6);
        REQUIRE(sumS2.y == 7);

        // Vector - Vector
        Vec2i diff = b - a;
        REQUIRE(diff.x == 2);
        REQUIRE(diff.y == 2);

        // Vector - Scalar
        Vec2i diffS = b - 1;
        REQUIRE(diffS.x == 2);
        REQUIRE(diffS.y == 3);

        // Scalar - Vector
        Vec2i diffS2 = 10 - a;
        REQUIRE(diffS2.x == 9);
        REQUIRE(diffS2.y == 8);

        // Vector * Vector
        Vec2i prod = a * b;
        REQUIRE(prod.x == 3);
        REQUIRE(prod.y == 8);

        // Vector * Scalar
        Vec2i prodS = a * 2;
        REQUIRE(prodS.x == 2);
        REQUIRE(prodS.y == 4);

        // Scalar * Vector
        Vec2i prodS2 = 2 * a;
        REQUIRE(prodS2.x == 2);
        REQUIRE(prodS2.y == 4);

        // Vector / Vector
        Vec2i quot = b / a;
        REQUIRE(quot.x == 3);
        REQUIRE(quot.y == 2);

        // Vector / Scalar
        Vec2i quotS = b / 2;
        REQUIRE(quotS.x == 1);
        REQUIRE(quotS.y == 2);

        // Scalar / Vector
        Vec2i quotS2 = 10 / a;
        REQUIRE(quotS2.x == 10);
        REQUIRE(quotS2.y == 5);
    }

    SECTION("Compound Assignment Operators")
    {
        Vec2i v(1, 2);

        v += Vec2i(3, 4);
        REQUIRE(v.x == 4);
        REQUIRE(v.y == 6);

        v += 1;
        REQUIRE(v.x == 5);
        REQUIRE(v.y == 7);

        v -= Vec2i(2, 3);
        REQUIRE(v.x == 3);
        REQUIRE(v.y == 4);

        v -= 1;
        REQUIRE(v.x == 2);
        REQUIRE(v.y == 3);

        v *= Vec2i(2, 3);
        REQUIRE(v.x == 4);
        REQUIRE(v.y == 9);

        v *= 2;
        REQUIRE(v.x == 8);
        REQUIRE(v.y == 18);

        v /= Vec2i(2, 3);
        REQUIRE(v.x == 4);
        REQUIRE(v.y == 6);

        v /= 2;
        REQUIRE(v.x == 2);
        REQUIRE(v.y == 3);
    }

    SECTION("Unary Operators")
    {
        Vec2i v(1, -2);
        Vec2i pos = +v;
        REQUIRE(pos.x == 1);
        REQUIRE(pos.y == -2);

        Vec2i neg = -v;
        REQUIRE(neg.x == -1);
        REQUIRE(neg.y == 2);
    }

    SECTION("Comparison Operators")
    {
        Vec2i a(1, 2);
        Vec2i b(1, 2);
        Vec2i c(3, 4);

        REQUIRE(a == b);
        REQUIRE(a != c);

        REQUIRE(a != 1);

        REQUIRE(Vec2i(2, 2) == 2);

        REQUIRE(a != 3);

        REQUIRE(c >= a);
        REQUIRE(a <= c);
        REQUIRE(c > a);
        REQUIRE(a < c);

        REQUIRE(c >= 3);
        REQUIRE(a <= 2);
        REQUIRE(c > 2);
        REQUIRE(a < 3);
    }

    SECTION("Static Members")
    {
        REQUIRE(Vec2i::Zero.x == 0);
        REQUIRE(Vec2i::Zero.y == 0);

        REQUIRE(Vec2i::One.x == 1);
        REQUIRE(Vec2i::One.y == 1);

        REQUIRE(Vec2i::UnitX.x == 1);
        REQUIRE(Vec2i::UnitX.y == 0);

        REQUIRE(Vec2i::UnitY.x == 0);
        REQUIRE(Vec2i::UnitY.y == 1);
    }

    SECTION("Output Stream")
    {
        Vec2i v(5, 6);
        std::stringstream ss;
        ss << v;
        REQUIRE(ss.str() == "(5, 6)");
    }
}

TEST_CASE("TVector2<float> Tests", "[TVector2]")
{
    using Vec2f = TVector2<float>;

    SECTION("Floating Point Arithmetic")
    {
        Vec2f a(1.5f, 2.5f);
        Vec2f b(2.0f, 3.0f);

        Vec2f sum = a + b;
        REQUIRE(sum.x == Approx(3.5f));
        REQUIRE(sum.y == Approx(5.5f));

        Vec2f prod = a * b;
        REQUIRE(prod.x == Approx(3.0f));
        REQUIRE(prod.y == Approx(7.5f));

        Vec2f quot = b / a;
        REQUIRE(quot.x == Approx(1.333333f));
        REQUIRE(quot.y == Approx(1.2f));
    }

    SECTION("Static Members")
    {
        REQUIRE(Vec2f::Zero.x == Approx(0.0f));
        REQUIRE(Vec2f::One.x == Approx(1.0f));
        REQUIRE(Vec2f::UnitX.x == Approx(1.0f));
        REQUIRE(Vec2f::UnitY.y == Approx(1.0f));
    }
}

}   // namespace tkd
