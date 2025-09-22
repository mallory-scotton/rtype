///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Network/FBinaryReader.hpp>
#include <catch2/catch.hpp>
#include <Engine/Core/Containers/FString.hpp>
#include <Engine/Network/FBinaryWriter.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
// Test Cases
///////////////////////////////////////////////////////////////////////////////
TEST_CASE("FBinaryReader Tests", "[FBinaryReader]")
{
    SECTION("Read Primitive")
    {
        std::vector<UInt8> buffer;
        FBinaryWriter writer(buffer);
        int original = 42;
        writer.Write(original);

        FBinaryReader reader(buffer.data(), buffer.size());
        int readValue;
        REQUIRE(reader.Read(readValue));
        REQUIRE(readValue == 42);
        REQUIRE(reader.GetOffset() == sizeof(int));
        REQUIRE(reader.GetRemaining() == 0);
    }

    SECTION("Read Multiple Primitives")
    {
        std::vector<UInt8> buffer;
        FBinaryWriter writer(buffer);
        int intVal = 123;
        float floatVal = 45.67f;
        UInt32 uintVal = 890;
        writer.Write(intVal);
        writer.Write(floatVal);
        writer.Write(uintVal);

        FBinaryReader reader(buffer.data(), buffer.size());
        int readInt;
        float readFloat;
        UInt32 readUInt;

        REQUIRE(reader.Read(readInt));
        REQUIRE(reader.Read(readFloat));
        REQUIRE(reader.Read(readUInt));

        REQUIRE(readInt == 123);
        REQUIRE(readFloat == 45.67f);
        REQUIRE(readUInt == 890);
        REQUIRE(reader.GetRemaining() == 0);
    }

    SECTION("Read String")
    {
        std::vector<UInt8> buffer;
        FBinaryWriter writer(buffer);
        std::string original = "Hello World";
        writer.Write(original);

        FBinaryReader reader(buffer.data(), buffer.size());
        std::string readStr;
        REQUIRE(reader.Read(readStr));
        REQUIRE(readStr == "Hello World");
    }

    SECTION("Read Empty String")
    {
        std::vector<UInt8> buffer;
        FBinaryWriter writer(buffer);
        std::string original = "";
        writer.Write(original);

        FBinaryReader reader(buffer.data(), buffer.size());
        std::string readStr;
        REQUIRE(reader.Read(readStr));
        REQUIRE(readStr == "");
    }

    SECTION("Read FString")
    {
        std::vector<UInt8> buffer;
        FBinaryWriter writer(buffer);
        FString original = "Test FString";
        writer.Write(original);

        FBinaryReader reader(buffer.data(), buffer.size());
        FString readStr;
        REQUIRE(reader.Read(readStr));
        REQUIRE(readStr == "Test FString");
    }

    SECTION("Read Vector of Primitives")
    {
        std::vector<UInt8> buffer;
        FBinaryWriter writer(buffer);
        std::vector<int> original = { 1, 2, 3, 4, 5 };
        writer.Write(original);

        FBinaryReader reader(buffer.data(), buffer.size());
        std::vector<int> readVec;
        REQUIRE(reader.Read(readVec));
        REQUIRE(readVec == original);
    }

    SECTION("Read Empty Vector")
    {
        std::vector<UInt8> buffer;
        FBinaryWriter writer(buffer);
        std::vector<int> original;
        writer.Write(original);

        FBinaryReader reader(buffer.data(), buffer.size());
        std::vector<int> readVec;
        REQUIRE(reader.Read(readVec));
        REQUIRE(readVec.empty());
    }

    SECTION("Read Vector of Strings")
    {
        std::vector<UInt8> buffer;
        FBinaryWriter writer(buffer);
        std::vector<std::string> original = { "apple", "banana", "cherry" };
        writer.Write(original);

        FBinaryReader reader(buffer.data(), buffer.size());
        std::vector<std::string> readVec;
        REQUIRE(reader.Read(readVec));
        REQUIRE(readVec == original);
    }

    SECTION("Read Vector of FStrings")
    {
        std::vector<UInt8> buffer;
        FBinaryWriter writer(buffer);
        std::vector<FString> original = { "first", "second" };
        writer.Write(original);

        FBinaryReader reader(buffer.data(), buffer.size());
        std::vector<FString> readVec;
        REQUIRE(reader.Read(readVec));
        REQUIRE(readVec == original);
    }

    SECTION("ReadBytes")
    {
        std::vector<UInt8> buffer;
        FBinaryWriter writer(buffer);
        UInt8 data[] = { 10, 20, 30, 40 };
        writer.WriteBytes(data, 4);

        FBinaryReader reader(buffer.data(), buffer.size());
        UInt8 readData[4];
        REQUIRE(reader.ReadBytes(readData, 4));
        REQUIRE(readData[0] == 10);
        REQUIRE(readData[1] == 20);
        REQUIRE(readData[2] == 30);
        REQUIRE(readData[3] == 40);
    }

    SECTION("Read Beyond Buffer - Primitive")
    {
        std::vector<UInt8> buffer;
        FBinaryWriter writer(buffer);
        int original = 42;
        writer.Write(original);

        FBinaryReader reader(buffer.data(), buffer.size());
        int readValue1, readValue2;
        REQUIRE(reader.Read(readValue1));
        REQUIRE(!reader.Read(readValue2));   // Should fail
    }

    SECTION("Read Beyond Buffer - String")
    {
        std::vector<UInt8> buffer;
        FBinaryWriter writer(buffer);
        std::string original = "test";
        writer.Write(original);

        // Corrupt the size to be larger
        SizeT wrongSize = 100;
        std::memcpy(buffer.data(), &wrongSize, sizeof(SizeT));

        FBinaryReader reader(buffer.data(), buffer.size());
        std::string readStr;
        REQUIRE(!reader.Read(readStr)
        );   // Should fail due to insufficient data
    }

    SECTION("Read Beyond Buffer - ReadBytes")
    {
        std::vector<UInt8> buffer = { 1, 2, 3 };
        FBinaryReader reader(buffer.data(), buffer.size());
        UInt8 readData[5];
        REQUIRE(!reader.ReadBytes(readData, 5));   // Buffer too small
    }

    SECTION("GetOffset and GetRemaining")
    {
        std::vector<UInt8> buffer;
        FBinaryWriter writer(buffer);
        int val1 = 1;
        int val2 = 2;
        writer.Write(val1);
        writer.Write(val2);

        FBinaryReader reader(buffer.data(), buffer.size());
        REQUIRE(reader.GetOffset() == 0);
        REQUIRE(reader.GetRemaining() == buffer.size());

        int readVal1;
        REQUIRE(reader.Read(readVal1));
        REQUIRE(reader.GetOffset() == sizeof(int));
        REQUIRE(reader.GetRemaining() == sizeof(int));

        int readVal2;
        REQUIRE(reader.Read(readVal2));
        REQUIRE(reader.GetOffset() == 2 * sizeof(int));
        REQUIRE(reader.GetRemaining() == 0);
    }

    SECTION("Partial Read")
    {
        std::vector<UInt8> buffer;
        FBinaryWriter writer(buffer);
        int val = 100;
        std::string str = "partial";
        writer.Write(val);
        writer.Write(str);

        FBinaryReader reader(buffer.data(), buffer.size());
        int readVal;
        REQUIRE(reader.Read(readVal));
        REQUIRE(readVal == 100);

        // Skip reading the string, check remaining
        REQUIRE(reader.GetRemaining() > 0);
    }
}

}   // namespace tkd
