///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Network/FPacketManager.hpp>
#include <catch2/catch_test_macros.hpp>
#include <Engine/Network/FPacketHeader.hpp>
#include <Engine/Network/TPacket.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
// Test Packet
///////////////////////////////////////////////////////////////////////////////
struct TestPacket : public TPacket<TestPacket>
{
    int value;

    TestPacket() = default;

    TestPacket(int v)
        : value(v)
    {}

    bool Serialize(FBinaryWriter& writer) const override
    {
        writer.Write(value);
        return true;
    }

    bool Deserialize(FBinaryReader& reader) override
    {
        return reader.Read(value);
    }

    SizeT GetSize() const override { return sizeof(int); }
};

///////////////////////////////////////////////////////////////////////////////
// Test Cases
///////////////////////////////////////////////////////////////////////////////
TEST_CASE("FPacketManager Tests", "[FPacketManager]")
{
    FPacketManager manager;

    SECTION("Register Packet")
    {
        manager.RegisterPacket<TestPacket>();
        // Registration is implicit, test by trying to serialize/deserialize
    }

    SECTION("Serialize and Deserialize Packet")
    {
        manager.RegisterPacket<TestPacket>();

        TestPacket original(42);
        auto buffer = manager.SerializePacket(original);

        REQUIRE(!buffer.empty());

        FPacketHeader header;
        auto deserialized =
            manager.DeserializePacket(buffer.data(), buffer.size(), header);

        REQUIRE(deserialized != nullptr);
        auto* testPacket = dynamic_cast<TestPacket*>(deserialized.get());
        REQUIRE(testPacket != nullptr);
        REQUIRE(testPacket->value == 42);
        REQUIRE(header.packetType == TestPacket::GetStaticType());
        REQUIRE(header.protocolVersion == PROTOCOL_VERSION);
        REQUIRE(header.magic == MAGIC_NUMBER);
    }

    SECTION("Sequence Number Increment")
    {
        manager.RegisterPacket<TestPacket>();

        TestPacket packet1(1);
        TestPacket packet2(2);

        auto buffer1 = manager.SerializePacket(packet1);
        auto buffer2 = manager.SerializePacket(packet2);

        FPacketHeader header1, header2;
        manager.DeserializePacket(buffer1.data(), buffer1.size(), header1);
        manager.DeserializePacket(buffer2.data(), buffer2.size(), header2);

        REQUIRE(header2.sequenceNumber == header1.sequenceNumber + 1);
    }

    SECTION("Deserialize with Invalid Magic")
    {
        manager.RegisterPacket<TestPacket>();

        TestPacket original(42);
        auto buffer = manager.SerializePacket(original);

        // Corrupt magic number
        buffer[0] = 0xFF;

        FPacketHeader header;
        auto deserialized =
            manager.DeserializePacket(buffer.data(), buffer.size(), header);
        REQUIRE(deserialized == nullptr);
    }

    SECTION("Deserialize with Invalid Protocol Version")
    {
        manager.RegisterPacket<TestPacket>();

        TestPacket original(42);
        auto buffer = manager.SerializePacket(original);

        // Corrupt protocol version (assuming it's at offset 4)
        buffer[4] = 0xFF;

        FPacketHeader header;
        auto deserialized =
            manager.DeserializePacket(buffer.data(), buffer.size(), header);
        REQUIRE(deserialized == nullptr);
    }

    SECTION("Deserialize with Invalid Packet Size")
    {
        manager.RegisterPacket<TestPacket>();

        TestPacket original(42);
        auto buffer = manager.SerializePacket(original);

        // Corrupt packet size (at offset 8)
        UInt16 wrongSize = static_cast<UInt16>(buffer.size() + 1);
        std::memcpy(buffer.data() + 8, &wrongSize, sizeof(UInt16));

        FPacketHeader header;
        auto deserialized =
            manager.DeserializePacket(buffer.data(), buffer.size(), header);
        REQUIRE(deserialized == nullptr);
    }

    SECTION("Deserialize with Invalid Checksum")
    {
        manager.RegisterPacket<TestPacket>();

        TestPacket original(42);
        auto buffer = manager.SerializePacket(original);

        // Corrupt some data after header to invalidate checksum
        buffer[FPacketHeader::SIZE] = ~buffer[FPacketHeader::SIZE];

        FPacketHeader header;
        auto deserialized =
            manager.DeserializePacket(buffer.data(), buffer.size(), header);
        REQUIRE(deserialized == nullptr);
    }

    SECTION("Deserialize Unregistered Packet Type")
    {
        manager.RegisterPacket<TestPacket>();

        TestPacket original(42);
        auto buffer = manager.SerializePacket(original);

        // Change packet type to unregistered (e.g., 9999)
        UInt16 unregisteredType = 9999;
        std::memcpy(buffer.data() + 6, &unregisteredType, sizeof(UInt16));

        FPacketHeader header;
        auto deserialized =
            manager.DeserializePacket(buffer.data(), buffer.size(), header);
        REQUIRE(deserialized == nullptr);
    }

    SECTION("Serialize Packet with Empty Data")
    {
        manager.RegisterPacket<TestPacket>();

        TestPacket original(0);
        auto buffer = manager.SerializePacket(original);

        REQUIRE(!buffer.empty());

        FPacketHeader header;
        auto deserialized =
            manager.DeserializePacket(buffer.data(), buffer.size(), header);
        REQUIRE(deserialized != nullptr);
        auto* testPacket = dynamic_cast<TestPacket*>(deserialized.get());
        REQUIRE(testPacket->value == 0);
    }
}

}   // namespace tkd
