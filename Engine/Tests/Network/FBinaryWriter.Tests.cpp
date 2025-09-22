///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Network/FBinaryWriter.hpp>
#include <catch2/catch.hpp>
#include <Engine/Core/Containers/FString.hpp>
#include <Engine/Network/FBinaryReader.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
// Test Cases
///////////////////////////////////////////////////////////////////////////////
TEST_CASE("FBinaryWriter Tests", "[FBinaryWriter]")
{
    SECTION("Write and Read Primitive")
    {
        std::vector<UInt8> buffer;
        FBinaryWriter writer(buffer);

        int value = 42;
        writer.Write(value);

        REQUIRE(buffer.size() == sizeof(int));

        FBinaryReader reader(buffer.data(), buffer.size());
        int readValue;
        REQUIRE(reader.Read(readValue));
        REQUIRE(readValue == 42);
    }

    SECTION("Write and Read Multiple Primitives")
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
    }

    SECTION("Write and Read String")
    {
        std::vector<UInt8> buffer;
        FBinaryWriter writer(buffer);

        std::string str = "Hello World";
        writer.Write(str);

        FBinaryReader reader(buffer.data(), buffer.size());
        std::string readStr;
        REQUIRE(reader.Read(readStr));
        REQUIRE(readStr == "Hello World");
    }

    SECTION("Write and Read Empty String")
    {
        std::vector<UInt8> buffer;
        FBinaryWriter writer(buffer);

        std::string str = "";
        writer.Write(str);

        FBinaryReader reader(buffer.data(), buffer.size());
        std::string readStr;
        REQUIRE(reader.Read(readStr));
        REQUIRE(readStr == "");
    }

    SECTION("Write and Read FString")
    {
        std::vector<UInt8> buffer;
        FBinaryWriter writer(buffer);

        FString str = "Test FString";
        writer.Write(str);

        FBinaryReader reader(buffer.data(), buffer.size());
        FString readStr;
        REQUIRE(reader.Read(readStr));
        REQUIRE(readStr == "Test FString");
    }

    SECTION("Write and Read Vector of Primitives")
    {
        std::vector<UInt8> buffer;
        FBinaryWriter writer(buffer);

        std::vector<int> vec = { 1, 2, 3, 4, 5 };
        writer.Write(vec);

        FBinaryReader reader(buffer.data(), buffer.size());
        std::vector<int> readVec;
        REQUIRE(reader.Read(readVec));
        REQUIRE(readVec == vec);
    }

    SECTION("Write and Read Empty Vector")
    {
        std::vector<UInt8> buffer;
        FBinaryWriter writer(buffer);

        std::vector<int> vec;
        writer.Write(vec);

        FBinaryReader reader(buffer.data(), buffer.size());
        std::vector<int> readVec;
        REQUIRE(reader.Read(readVec));
        REQUIRE(readVec.empty());
    }

    SECTION("Write and Read Vector of Strings")
    {
        std::vector<UInt8> buffer;
        FBinaryWriter writer(buffer);

        std::vector<std::string> vec = { "apple", "banana", "cherry" };
        writer.Write(vec);

        FBinaryReader reader(buffer.data(), buffer.size());
        std::vector<std::string> readVec;
        REQUIRE(reader.Read(readVec));
        REQUIRE(readVec == vec);
    }

    SECTION("Write and Read Vector of FStrings")
    {
        std::vector<UInt8> buffer;
        FBinaryWriter writer(buffer);

        std::vector<FString> vec = { "first", "second" };
        writer.Write(vec);

        FBinaryReader reader(buffer.data(), buffer.size());
        std::vector<FString> readVec;
        REQUIRE(reader.Read(readVec));
        REQUIRE(readVec == vec);
    }

    SECTION("WriteBytes")
    {
        std::vector<UInt8> buffer;
        FBinaryWriter writer(buffer);

        UInt8 data[] = { 10, 20, 30, 40 };
        writer.WriteBytes(data, 4);

        REQUIRE(buffer.size() == 4);
        REQUIRE(buffer[0] == 10);
        REQUIRE(buffer[1] == 20);
        REQUIRE(buffer[2] == 30);
        REQUIRE(buffer[3] == 40);
    }

    SECTION("GetOffset and SetOffset")
    {
        std::vector<UInt8> buffer;
        FBinaryWriter writer(buffer);

        REQUIRE(writer.GetOffset() == 0);

        int value1 = 100;
        writer.Write(value1);
        REQUIRE(writer.GetOffset() == sizeof(int));

        writer.SetOffset(0);
        REQUIRE(writer.GetOffset() == 0);

        int value2 = 200;
        writer.Write(value2);

        FBinaryReader reader(buffer.data(), buffer.size());
        int readValue;
        REQUIRE(reader.Read(readValue));
        REQUIRE(readValue == 200);
    }

    SECTION("Buffer Resizing")
    {
        std::vector<UInt8> buffer;
        FBinaryWriter writer(buffer);

        // Initially empty
        REQUIRE(buffer.empty());

        // Write something
        long long bigValue = 123456789012345LL;
        writer.Write(bigValue);

        REQUIRE(buffer.size() == sizeof(long long));

        // Write more
        std::string str = "additional data";
        writer.Write(str);

        REQUIRE(
            buffer.size() == sizeof(long long) + sizeof(SizeT) + str.size()
        );
    }
}

}   // namespace tkd
