///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <catch2/catch.hpp>
#include <Engine/Core/Utils/Singleton.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
using namespace tkd;

///////////////////////////////////////////////////////////////////////////////
// Test Singleton Class
///////////////////////////////////////////////////////////////////////////////
class TestSingleton : public Singleton<TestSingleton>
{
public:
    int value = 0;

    TestSingleton() = default;

    TestSingleton(int v)
        : value(v)
    {}
};

///////////////////////////////////////////////////////////////////////////////
// Test Cases
///////////////////////////////////////////////////////////////////////////////

TEST_CASE("Singleton GetInstance creates instance", "[Singleton]")
{
    // Ensure no instance exists initially
    REQUIRE_FALSE(TestSingleton::HasInstance());

    // Get instance
    TestSingleton& instance = TestSingleton::GetInstance();

    // Check instance exists
    REQUIRE(TestSingleton::HasInstance());

    // Check it's the same instance
    REQUIRE(&instance == &TestSingleton::GetInstance());

    // Clean up
    TestSingleton::DestroyInstance();
}

TEST_CASE("Singleton GetInstance with arguments", "[Singleton]")
{
    // Ensure no instance exists initially
    REQUIRE_FALSE(TestSingleton::HasInstance());

    // Get instance with argument
    TestSingleton& instance = TestSingleton::GetInstance(42);

    // Check instance exists
    REQUIRE(TestSingleton::HasInstance());

    // Check value is set
    REQUIRE(instance.value == 42);

    // Get instance again without args, should return same instance
    TestSingleton& instance2 = TestSingleton::GetInstance();
    REQUIRE(&instance == &instance2);
    REQUIRE(instance2.value == 42);

    // Clean up
    TestSingleton::DestroyInstance();
}

TEST_CASE("Singleton CreateInstance", "[Singleton]")
{
    // Ensure no instance exists initially
    REQUIRE_FALSE(TestSingleton::HasInstance());

    // Create instance with argument
    TestSingleton& instance = TestSingleton::CreateInstance(100);

    // Check instance exists
    REQUIRE(TestSingleton::HasInstance());

    // Check value is set
    REQUIRE(instance.value == 100);

    // Clean up
    TestSingleton::DestroyInstance();
}

TEST_CASE("Singleton HasInstance", "[Singleton]")
{
    // Initially no instance
    REQUIRE_FALSE(TestSingleton::HasInstance());

    // After getting instance
    TestSingleton::GetInstance();
    REQUIRE(TestSingleton::HasInstance());

    // After destroying
    TestSingleton::DestroyInstance();
    REQUIRE_FALSE(TestSingleton::HasInstance());
}

TEST_CASE("Singleton GetInstancePtr", "[Singleton]")
{
    // Initially null
    REQUIRE(TestSingleton::GetInstancePtr() == nullptr);

    // After creating
    TestSingleton::GetInstance();
    REQUIRE(TestSingleton::GetInstancePtr() != nullptr);

    // Check it's the same as GetInstance
    REQUIRE(TestSingleton::GetInstancePtr() == &TestSingleton::GetInstance());

    // After destroying
    TestSingleton::DestroyInstance();
    REQUIRE(TestSingleton::GetInstancePtr() == nullptr);
}

TEST_CASE("Singleton DestroyInstance", "[Singleton]")
{
    // Create instance
    TestSingleton::GetInstance(50);
    REQUIRE(TestSingleton::HasInstance());

    // Destroy
    TestSingleton::DestroyInstance();
    REQUIRE_FALSE(TestSingleton::HasInstance());

    // Can create new instance
    TestSingleton& instance = TestSingleton::GetInstance(75);
    REQUIRE(TestSingleton::HasInstance());
    REQUIRE(instance.value == 75);

    // Clean up
    TestSingleton::DestroyInstance();
}

TEST_CASE(
    "Singleton multiple GetInstance calls return same instance", "[Singleton]"
)
{
    TestSingleton& instance1 = TestSingleton::GetInstance();
    TestSingleton& instance2 = TestSingleton::GetInstance();
    TestSingleton& instance3 = TestSingleton::GetInstance();

    REQUIRE(&instance1 == &instance2);
    REQUIRE(&instance2 == &instance3);

    // Clean up
    TestSingleton::DestroyInstance();
}
