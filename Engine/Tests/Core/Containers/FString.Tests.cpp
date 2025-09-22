///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Core/Containers/FString.hpp>
#include <catch2/catch_test_macros.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
using namespace tkd;

TEST_CASE("FString Default Constructor", "[FString][Constructor]")
{
    FString str;

    REQUIRE(str.Length() == 0);
    REQUIRE(str.Size() == 0);
    REQUIRE(str.IsEmpty());
    REQUIRE(str.CStr() != nullptr);
    REQUIRE(std::string(str.CStr()) == "");
}

TEST_CASE("FString Copy Constructor", "[FString][Constructor]")
{
    FString original("Hello World");
    FString copy(original);

    REQUIRE(copy.Length() == 11);
    REQUIRE(std::string(copy.CStr()) == "Hello World");
    REQUIRE(copy == original);
}

TEST_CASE("FString Move Constructor", "[FString][Constructor]")
{
    FString original("Hello World");
    FString moved(std::move(original));

    REQUIRE(moved.Length() == 11);
    REQUIRE(std::string(moved.CStr()) == "Hello World");
}

TEST_CASE("FString C-string Constructor", "[FString][Constructor]")
{
    const char* cstr = "Test String";
    FString str(cstr);

    REQUIRE(str.Length() == 11);
    REQUIRE(std::string(str.CStr()) == "Test String");
}

TEST_CASE("FString C-string with Length Constructor", "[FString][Constructor]")
{
    const char* cstr = "Test String";
    FString str(cstr, 4);

    REQUIRE(str.Length() == 4);
    REQUIRE(std::string(str.CStr()) == "Test");
}

TEST_CASE("FString Fill Constructor", "[FString][Constructor]")
{
    FString str(5, 'A');

    REQUIRE(str.Length() == 5);
    REQUIRE(std::string(str.CStr()) == "AAAAA");
}

TEST_CASE("FString Substring Constructor", "[FString][Constructor]")
{
    FString original("Hello World");
    FString substr(original, 6, 5);

    REQUIRE(substr.Length() == 5);
    REQUIRE(std::string(substr.CStr()) == "World");
}

TEST_CASE("FString Assignment Operators", "[FString][Assignment]")
{
    SECTION("Copy Assignment")
    {
        FString str1("Hello");
        FString str2("World");
        str1 = str2;

        REQUIRE(str1 == str2);
        REQUIRE(std::string(str1.CStr()) == "World");
    }

    SECTION("Move Assignment")
    {
        FString str1("Hello");
        FString str2("World");
        str1 = std::move(str2);

        REQUIRE(std::string(str1.CStr()) == "World");
    }
}

TEST_CASE("FString Comparison Operators", "[FString][Comparison]")
{
    FString str1("Apple");
    FString str2("Apple");
    FString str3("Banana");
    FString str4("Aardvark");

    SECTION("Equality")
    {
        REQUIRE(str1 == str2);
        REQUIRE_FALSE(str1 == str3);
    }

    SECTION("Inequality")
    {
        REQUIRE(str1 != str3);
        REQUIRE_FALSE(str1 != str2);
    }

    SECTION("Less Than")
    {
        REQUIRE(str4 < str1);   // Aardvark < Apple
        REQUIRE(str1 < str3);   // Apple < Banana
    }

    SECTION("Greater Than")
    {
        REQUIRE(str3 > str1);   // Banana > Apple
        REQUIRE(str1 > str4);   // Apple > Aardvark
    }

    SECTION("Less Than or Equal")
    {
        REQUIRE(str1 <= str2);   // Apple <= Apple
        REQUIRE(str4 <= str1);   // Aardvark <= Apple
    }

    SECTION("Greater Than or Equal")
    {
        REQUIRE(str1 >= str2);   // Apple >= Apple
        REQUIRE(str3 >= str1);   // Banana >= Apple
    }
}

TEST_CASE("FString Element Access", "[FString][Access]")
{
    FString str("Hello");

    SECTION("Subscript Operator")
    {
        REQUIRE(str[0] == 'H');
        REQUIRE(str[4] == 'o');

        str[0] = 'h';
        REQUIRE(str[0] == 'h');
    }

    SECTION("At Method")
    {
        REQUIRE(str.At(1) == 'e');
        REQUIRE(str.At(3) == 'l');
    }

    SECTION("Front and Back")
    {
        REQUIRE(str.Front() == 'H');
        REQUIRE(str.Back() == 'o');

        str.Front() = 'J';
        str.Back() = 'y';
        REQUIRE(std::string(str.CStr()) == "Jelly");
    }
}

TEST_CASE("FString Append Operations", "[FString][Append]")
{
    SECTION("Append FString")
    {
        FString str1("Hello");
        FString str2(" World");
        str1.Append(str2);

        REQUIRE(std::string(str1.CStr()) == "Hello World");
    }

    SECTION("Append C-string")
    {
        FString str("Hello");
        str.Append(" World");

        REQUIRE(std::string(str.CStr()) == "Hello World");
    }

    SECTION("Append Character")
    {
        FString str("Hell");
        str.PushBack('o');

        REQUIRE(std::string(str.CStr()) == "Hello");
    }

    SECTION("Append Operator +=")
    {
        FString str("Hello");
        str += " ";
        str += "World";
        str += '!';

        REQUIRE(std::string(str.CStr()) == "Hello World!");
    }
}

TEST_CASE("FString Insert Operations", "[FString][Insert]")
{
    SECTION("Insert FString")
    {
        FString str("Hello World");
        FString insert(" Beautiful");
        str.Insert(5, insert);

        REQUIRE(std::string(str.CStr()) == "Hello Beautiful World");
    }

    SECTION("Insert C-string")
    {
        FString str("Hello World");
        str.Insert(6, "Big ");

        REQUIRE(std::string(str.CStr()) == "Hello Big World");
    }

    SECTION("Insert Character")
    {
        FString str("Hell");
        str.Insert(4, 1, 'o');

        REQUIRE(std::string(str.CStr()) == "Hello");
    }
}

TEST_CASE("FString Erase Operations", "[FString][Erase]")
{
    SECTION("Erase by Position and Length")
    {
        FString str("Hello Beautiful World");
        str.Erase(5, 10);   // Remove " Beautiful"

        REQUIRE(std::string(str.CStr()) == "Hello World");
    }

    SECTION("Pop Back")
    {
        FString str("Hello!");
        str.PopBack();

        REQUIRE(std::string(str.CStr()) == "Hello");
    }
}

TEST_CASE("FString Replace Operations", "[FString][Replace]")
{
    FString str("Hello World");
    str.Replace(6, 5, "Universe");

    REQUIRE(std::string(str.CStr()) == "Hello Universe");
}

TEST_CASE("FString Find Operations", "[FString][Find]")
{
    FString str("Hello World Hello");

    SECTION("Find Substring")
    {
        REQUIRE(str.Find("Hello") == 0);
        REQUIRE(str.Find("World") == 6);
        REQUIRE(str.Find("Hello", 1) == 12);
        REQUIRE(str.Find("NotFound") == FString::NPOS);
    }

    SECTION("Find Character")
    {
        REQUIRE(str.Find('l') == 2);
        REQUIRE(str.Find('o', 5) == 7);
        REQUIRE(str.Find('z') == FString::NPOS);
    }

    SECTION("Reverse Find")
    {
        REQUIRE(str.RFind("Hello") == 12);
        REQUIRE(str.RFind("World") == 6);
        REQUIRE(str.RFind('l') == 15);
    }

    SECTION("Find First Of")
    {
        REQUIRE(str.FindFirstOf("aeiou") == 1);   // 'e' in Hello
        REQUIRE(str.FindFirstOf("xyz") == FString::NPOS);
    }

    SECTION("Find Last Of")
    {
        REQUIRE(str.FindLastOf("aeiou") == 16);   // 'o' in second Hello
    }

    SECTION("Find First Not Of")
    {
        REQUIRE(str.FindFirstNotOf("H") == 1);
        REQUIRE(str.FindFirstNotOf("Hel o") == 6);   // 'W' in World
    }

    SECTION("Find Last Not Of")
    {
        REQUIRE(str.FindLastNotOf("o") == 15);   // Last 'l'
    }
}

TEST_CASE("FString Substring", "[FString][Substring]")
{
    FString str("Hello World");

    SECTION("Full Substring")
    {
        FString sub = str.SubStr(6);
        REQUIRE(std::string(sub.CStr()) == "World");
    }

    SECTION("Partial Substring")
    {
        FString sub = str.SubStr(0, 5);
        REQUIRE(std::string(sub.CStr()) == "Hello");
    }

    SECTION("Middle Substring")
    {
        FString sub = str.SubStr(6, 5);
        REQUIRE(std::string(sub.CStr()) == "World");
    }
}

TEST_CASE("FString Size and Capacity", "[FString][Size]")
{
    FString str("Hello");

    SECTION("Basic Size Operations")
    {
        REQUIRE(str.Length() == 5);
        REQUIRE(str.Size() == 5);
        REQUIRE_FALSE(str.IsEmpty());
    }

    SECTION("Resize Operations")
    {
        str.Resize(10);
        REQUIRE(str.Length() == 10);

        str.Resize(3);
        REQUIRE(str.Length() == 3);
        REQUIRE(std::string(str.CStr()) == "Hel");

        str.Resize(7, 'x');
        REQUIRE(str.Length() == 7);
        REQUIRE(std::string(str.CStr()) == "Helxxxx");
    }

    SECTION("Capacity Operations")
    {
        str.Reserve(100);
        REQUIRE(str.Capacity() >= 100);
        REQUIRE(str.Length() == 5);   // Length shouldn't change
    }

    SECTION("Clear Operation")
    {
        str.Clear();
        REQUIRE(str.IsEmpty());
        REQUIRE(str.Length() == 0);
    }
}

TEST_CASE("FString Case Conversion", "[FString][Case]")
{
    SECTION("To Lower Case")
    {
        FString str("Hello WORLD");
        str.ToLowerCase();
        REQUIRE(std::string(str.CStr()) == "hello world");
    }

    SECTION("To Upper Case")
    {
        FString str("Hello world");
        str.ToUpperCase();
        REQUIRE(std::string(str.CStr()) == "HELLO WORLD");
    }
}

TEST_CASE("FString Trim", "[FString][Trim]")
{
    FString str("  Hello World  ");
    str.Trim();
    REQUIRE(std::string(str.CStr()) == "Hello World");

    FString str2("\t\n  Trimmed  \t\n");
    str2.Trim();
    REQUIRE(std::string(str2.CStr()) == "Trimmed");
}

TEST_CASE("FString Copy Method", "[FString][Copy]")
{
    FString str("Hello World");
    char buffer[20];

    SizeT copied = str.Copy(buffer, 5, 0);
    buffer[copied] = '\0';   // Null terminate

    REQUIRE(copied == 5);
    REQUIRE(std::string(buffer) == "Hello");

    copied = str.Copy(buffer, 5, 6);
    buffer[copied] = '\0';

    REQUIRE(copied == 5);
    REQUIRE(std::string(buffer) == "World");
}

TEST_CASE("FString Swap", "[FString][Swap]")
{
    FString str1("Hello");
    FString str2("World");

    str1.Swap(str2);

    REQUIRE(std::string(str1.CStr()) == "World");
    REQUIRE(std::string(str2.CStr()) == "Hello");

    // Test static swap
    FString::Swap(str1, str2);

    REQUIRE(std::string(str1.CStr()) == "Hello");
    REQUIRE(std::string(str2.CStr()) == "World");
}

TEST_CASE("FString Iterators", "[FString][Iterators]")
{
    FString str("Hello");

    SECTION("Forward Iterators")
    {
        auto it = str.Begin();
        REQUIRE(*it == 'H');

        ++it;
        REQUIRE(*it == 'e');

        std::string reconstructed;
        for (auto iter = str.Begin(); iter != str.End(); ++iter)
        {
            reconstructed += *iter;
        }
        REQUIRE(reconstructed == "Hello");
    }

    SECTION("Const Iterators")
    {
        const FString constStr("World");

        std::string reconstructed;
        for (auto iter = constStr.CBegin(); iter != constStr.CEnd(); ++iter)
        {
            reconstructed += *iter;
        }
        REQUIRE(reconstructed == "World");
    }

    SECTION("Reverse Iterators")
    {
        std::string reversed;
        for (auto iter = str.RBegin(); iter != str.REnd(); ++iter)
        {
            reversed += *iter;
        }
        REQUIRE(reversed == "olleH");
    }
}

TEST_CASE("FString Concatenation Operators", "[FString][Concatenation]")
{
    FString str1("Hello");
    FString str2(" World");

    SECTION("FString + FString")
    {
        FString result = str1 + str2;
        REQUIRE(std::string(result.CStr()) == "Hello World");
    }

    SECTION("FString + C-string")
    {
        FString result = str1 + " C++";
        REQUIRE(std::string(result.CStr()) == "Hello C++");
    }

    SECTION("C-string + FString")
    {
        FString result = "Hi " + str2;
        REQUIRE(std::string(result.CStr()) == "Hi  World");
    }

    SECTION("FString + char")
    {
        FString result = str1 + '!';
        REQUIRE(std::string(result.CStr()) == "Hello!");
    }

    SECTION("char + FString")
    {
        FString result = '>' + str1;
        REQUIRE(std::string(result.CStr()) == ">Hello");
    }
}

TEST_CASE("FString Edge Cases", "[FString][EdgeCases]")
{
    SECTION("Empty String Operations")
    {
        FString empty;

        REQUIRE(empty.Find("test") == FString::NPOS);
        REQUIRE(empty.SubStr() == empty);

        empty += "test";
        REQUIRE(std::string(empty.CStr()) == "test");
    }

    SECTION("Single Character String")
    {
        FString single("A");

        REQUIRE(single.Length() == 1);
        REQUIRE(single.Front() == 'A');
        REQUIRE(single.Back() == 'A');
        REQUIRE(single[0] == 'A');
    }

    SECTION("Large String Operations")
    {
        std::string largeStr(1000, 'X');
        FString fstr(largeStr.c_str());

        REQUIRE(fstr.Length() == 1000);
        REQUIRE(fstr.Find('X') == 0);
        REQUIRE(fstr.Find('Y') == FString::NPOS);
    }
}

TEST_CASE("FString std::string Conversion", "[FString][Conversion]")
{
    FString fstr("Hello World");
    std::string stdstr = fstr;

    REQUIRE(stdstr == "Hello World");
}

// Test with complex iterator operations
TEST_CASE(
    "FString Iterator Range Constructor", "[FString][Iterator][Constructor]"
)
{
    FString original("Hello World");
    auto begin = original.Begin();
    auto end = original.Begin();

    // Move end iterator to position 5
    for (int i = 0; i < 5; ++i) { ++end; }

    FString substring(begin, end);
    REQUIRE(std::string(substring.CStr()) == "Hello");
}

TEST_CASE("FString Performance Characteristics", "[FString][Performance]")
{
    SECTION("Capacity Growth")
    {
        FString str;
        SizeT prevCapacity = str.Capacity();

        // Add characters and check that capacity grows appropriately
        for (int i = 0; i < 100; ++i)
        {
            str += 'A';
            if (str.Capacity() > prevCapacity)
            {
                // Capacity should have grown
                REQUIRE(str.Capacity() > str.Length());
                prevCapacity = str.Capacity();
            }
        }

        REQUIRE(str.Length() == 100);
    }

    SECTION("Shrink to Fit")
    {
        FString str(100, 'A');
        str.Resize(10);

        str.ShrinkToFit();

        REQUIRE(str.Length() == 10);
        // Capacity should be reduced (though exact behavior may vary)
        REQUIRE(str.Capacity() >= str.Length());
    }
}
