///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Core/Utils/FArgs.hpp>
#include <catch2/catch.hpp>
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
#define DISABLE_STDOUT \
    std::streambuf* oldCoutStreamBuf = std::cout.rdbuf(); \
    std::cout.rdbuf(nullptr); \
    std::streambuf* oldCerrStreamBuf = std::cerr.rdbuf(); \
    std::cerr.rdbuf(nullptr); \
    (void)oldCoutStreamBuf; \
    (void)oldCerrStreamBuf;

#define ENABLE_STDOUT \
    std::cout.rdbuf(oldCoutStreamBuf); \
    std::cerr.rdbuf(oldCerrStreamBuf);

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
using namespace tkd;

// Unit tests for FArgs class

TEST_CASE("FArgs Constructor", "[FArgs]")
{
    FArgs::DestroyInstance();
    FArgs& args = FArgs::GetInstance();
    REQUIRE(args.GetExitCode() == 84);
}

TEST_CASE("AddFlags string optional", "[FArgs]")
{
    FArgs::DestroyInstance();
    FArgs& args = FArgs::GetInstance();
    std::string value = "default";
    args.AddFlags("test", "Test string flag", value, false);

    int argc = 3;
    char* argv[] = { (char*)"program", (char*)"--test", (char*)"hello" };
    DISABLE_STDOUT
    REQUIRE(args.Process(argc, argv));
    ENABLE_STDOUT
    REQUIRE(value == "hello");
    REQUIRE(args.GetExitCode() == 0);
}

TEST_CASE("AddFlags string mandatory", "[FArgs]")
{
    FArgs::DestroyInstance();
    FArgs& args = FArgs::GetInstance();
    std::string value;
    args.AddFlags("test", "Test string flag", value, true);

    int argc = 3;
    char* argv[] = { (char*)"program", (char*)"--test", (char*)"hello" };
    DISABLE_STDOUT
    REQUIRE(args.Process(argc, argv));
    ENABLE_STDOUT
    REQUIRE(value == "hello");
}

TEST_CASE("AddFlags int", "[FArgs]")
{
    FArgs::DestroyInstance();
    FArgs& args = FArgs::GetInstance();
    int value = 0;
    args.AddFlags("number", "Test int flag", value, false);

    int argc = 3;
    char* argv[] = { (char*)"program", (char*)"--number", (char*)"42" };
    DISABLE_STDOUT
    REQUIRE(args.Process(argc, argv));
    ENABLE_STDOUT
    REQUIRE(value == 42);
}

TEST_CASE("AddFlags float", "[FArgs]")
{
    FArgs::DestroyInstance();
    FArgs& args = FArgs::GetInstance();
    float value = 0.0f;
    args.AddFlags("float", "Test float flag", value, false);

    int argc = 3;
    char* argv[] = { (char*)"program", (char*)"--float", (char*)"3.14" };
    DISABLE_STDOUT
    REQUIRE(args.Process(argc, argv));
    ENABLE_STDOUT
    REQUIRE(value == 3.14f);
}

TEST_CASE("AddFlags double", "[FArgs]")
{
    FArgs::DestroyInstance();
    FArgs& args = FArgs::GetInstance();
    double value = 0.0;
    args.AddFlags("double", "Test double flag", value, false);

    int argc = 3;
    char* argv[] = { (char*)"program", (char*)"--double", (char*)"2.718" };
    DISABLE_STDOUT
    REQUIRE(args.Process(argc, argv));
    ENABLE_STDOUT
    REQUIRE(value == 2.718);
}

TEST_CASE("AddFlags bool", "[FArgs]")
{
    FArgs::DestroyInstance();
    FArgs& args = FArgs::GetInstance();
    bool value = false;
    args.AddFlags("bool", "Test bool flag", value, false);

    int argc = 3;
    char* argv[] = { (char*)"program", (char*)"--bool", (char*)"true" };
    DISABLE_STDOUT
    REQUIRE(args.Process(argc, argv));
    ENABLE_STDOUT
    REQUIRE(value == true);
}

TEST_CASE("AddFlags short form", "[FArgs]")
{
    FArgs::DestroyInstance();
    FArgs& args = FArgs::GetInstance();
    std::string value;
    args.AddFlags("test", "Test flag", value, false);

    int argc = 3;
    char* argv[] = { (char*)"program", (char*)"-t", (char*)"short" };
    DISABLE_STDOUT
    REQUIRE(args.Process(argc, argv));
    ENABLE_STDOUT
    REQUIRE(value == "short");
}

TEST_CASE("AddVectorFlags string", "[FArgs]")
{
    FArgs::DestroyInstance();
    FArgs& args = FArgs::GetInstance();
    std::vector<std::string> value;
    args.AddVectorFlags("list", "Test vector flag", value, false, ",");

    int argc = 3;
    char* argv[] = { (char*)"program", (char*)"--list", (char*)"a,b,c" };
    DISABLE_STDOUT
    REQUIRE(args.Process(argc, argv));
    ENABLE_STDOUT
    REQUIRE(value.size() == 3);
    REQUIRE(value[0] == "a");
    REQUIRE(value[1] == "b");
    REQUIRE(value[2] == "c");
}

TEST_CASE("AddVectorFlags int", "[FArgs]")
{
    FArgs::DestroyInstance();
    FArgs& args = FArgs::GetInstance();
    std::vector<int> value;
    args.AddVectorFlags("nums", "Test int vector", value, false, ",");

    int argc = 3;
    char* argv[] = { (char*)"program", (char*)"--nums", (char*)"1,2,3" };
    DISABLE_STDOUT
    REQUIRE(args.Process(argc, argv));
    ENABLE_STDOUT
    REQUIRE(value == std::vector<int>{ 1, 2, 3 });
}

TEST_CASE("Mandatory flag missing", "[FArgs]")
{
    FArgs::DestroyInstance();
    FArgs& args = FArgs::GetInstance();
    std::string value;
    args.AddFlags("required", "Required flag", value, true);

    int argc = 1;
    char* argv[] = { (char*)"program" };
    DISABLE_STDOUT
    REQUIRE_FALSE(args.Process(argc, argv));
    ENABLE_STDOUT
    REQUIRE(args.GetExitCode() == 84);
}

TEST_CASE("Unknown argument", "[FArgs]")
{
    FArgs::DestroyInstance();
    FArgs& args = FArgs::GetInstance();
    std::string value;
    args.AddFlags("known", "Known flag", value, false);

    int argc = 3;
    char* argv[] = { (char*)"program", (char*)"--unknown", (char*)"value" };
    DISABLE_STDOUT
    REQUIRE_FALSE(args.Process(argc, argv));
    ENABLE_STDOUT
}

TEST_CASE("Flag requires value", "[FArgs]")
{
    FArgs::DestroyInstance();
    FArgs& args = FArgs::GetInstance();
    std::string value;
    args.AddFlags("flag", "Test flag", value, false);

    int argc = 2;
    char* argv[] = { (char*)"program", (char*)"--flag" };
    DISABLE_STDOUT
    REQUIRE_FALSE(args.Process(argc, argv));
    ENABLE_STDOUT
}

TEST_CASE("Invalid value for int", "[FArgs]")
{
    FArgs::DestroyInstance();
    FArgs& args = FArgs::GetInstance();
    int value = 0;
    args.AddFlags("num", "Test int", value, false);

    int argc = 3;
    char* argv[] = { (char*)"program", (char*)"--num", (char*)"notanumber" };
    DISABLE_STDOUT
    REQUIRE_FALSE(args.Process(argc, argv));
    ENABLE_STDOUT
}

TEST_CASE("Help flag", "[FArgs]")
{
    FArgs::DestroyInstance();
    FArgs& args = FArgs::GetInstance();
    std::string value;
    args.AddFlags("test", "Test flag", value, false);

    int argc = 2;
    char* argv[] = { (char*)"program", (char*)"--help" };
    DISABLE_STDOUT
    REQUIRE_FALSE(args.Process(argc, argv));
    ENABLE_STDOUT
    REQUIRE(args.GetExitCode() == 0);
}

TEST_CASE("Help short flag", "[FArgs]")
{
    FArgs::DestroyInstance();
    FArgs& args = FArgs::GetInstance();
    std::string value;
    args.AddFlags("test", "Test flag", value, false);

    int argc = 2;
    char* argv[] = { (char*)"program", (char*)"-help" };
    DISABLE_STDOUT
    REQUIRE_FALSE(args.Process(argc, argv));
    ENABLE_STDOUT
    REQUIRE(args.GetExitCode() == 0);
}

TEST_CASE("Multiple flags", "[FArgs]")
{
    FArgs::DestroyInstance();
    FArgs& args = FArgs::GetInstance();
    std::string strVal;
    int intVal = 0;
    args.AddFlags("str", "String flag", strVal, false);
    args.AddFlags("num", "Int flag", intVal, false);

    int argc = 5;
    char* argv[] = { (char*)"program",
                     (char*)"--str",
                     (char*)"hello",
                     (char*)"--num",
                     (char*)"123" };
    DISABLE_STDOUT
    REQUIRE(args.Process(argc, argv));
    ENABLE_STDOUT
    REQUIRE(strVal == "hello");
    REQUIRE(intVal == 123);
}

TEST_CASE("Vector flag with spaces", "[FArgs]")
{
    FArgs::DestroyInstance();
    FArgs& args = FArgs::GetInstance();
    std::vector<std::string> value;
    args.AddVectorFlags("list", "Test vector", value, false, ",");

    int argc = 3;
    char* argv[] = { (char*)"program", (char*)"--list", (char*)"a, b , c" };
    DISABLE_STDOUT
    REQUIRE(args.Process(argc, argv));
    ENABLE_STDOUT
    REQUIRE(value.size() == 3);
    REQUIRE(value[0] == "a");
    REQUIRE(value[1] == "b");
    REQUIRE(value[2] == "c");
}
