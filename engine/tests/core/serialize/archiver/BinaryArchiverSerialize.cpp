#include <gtest/gtest.h>
#include "core/serialize/archiver/ArchiverSerializer.hpp"
#include "ArchiverSerializeClass.hpp"

namespace
{
using namespace slug::core;
using namespace slug::test::archiver;

template<typename T>
T round_trip_binary(const T& src)
{
    TVector<uint8_t> buffer;
    {
        BinaryOutputArchive oar(buffer);
        oar.Value(src);
    }

    T dst {};
    {
        BinaryInputArchive iar(buffer);
        iar.Value(dst);
    }

    return dst;
}
}

TEST(ArchiverSerializerTest, BinaryRoundTrip_SimpleObject)
{
    SimpleData src;
    src.id = 42;
    src.value = 3.5f;
    src.name = "alpha";

    const SimpleData dst = round_trip_binary(src);
    EXPECT_EQ(src, dst);
}

TEST(ArchiverSerializerTest, BinaryRoundTrip_VectorAndString)
{
    Inventory src;
    src.itemIds = { 1, 2, 3, 10, 20 };
    src.owner = "player_a";

    const Inventory dst = round_trip_binary(src);
    EXPECT_EQ(src, dst);
}

TEST(ArchiverSerializerTest, BinaryRoundTrip_NestedObject)
{
    Player src;
    src.id = 99;
    src.name = "Hero";
    src.hp = 87.5f;
    src.inventoryIds = { 10, 20, 30, 40 };
    src.weapon.name = "Sword";
    src.weapon.attack = 123;

    const Player dst = round_trip_binary(src);
    EXPECT_EQ(src, dst);
}

TEST(ArchiverSerializerTest, BinaryRoundTrip_InheritanceBaseClass)
{
    Character src;
    src.entityId = 500;
    src.name = "Knight";
    src.level = 12;
    src.skillIds = { 3, 6, 9 };

    const Character dst = round_trip_binary(src);
    EXPECT_EQ(src, dst);
}

TEST(ArchiverSerializerTest, BinaryRoundTrip_UnorderedMap_IntToInt)
{
    TUnorderedMap<int, int> src = { { 1, 10 }, { 2, 20 }, { 3, 30 } };

    const auto dst = round_trip_binary(src);

    EXPECT_EQ(src, dst);
}

TEST(ArchiverSerializerTest, BinaryRoundTrip_UnorderedMap_StringToInt)
{
    TUnorderedMap<String, int> src = { { "hp", 100 }, { "mp", 50 }, { "atk", 30 } };

    const auto dst = round_trip_binary(src);

    EXPECT_EQ(src, dst);
}

TEST(ArchiverSerializerTest, BinaryRoundTrip_UnorderedMap_IntToString)
{
    TUnorderedMap<int, String> src = { { 1, "sword" }, { 2, "shield" }, { 3, "bow" } };

    const auto dst = round_trip_binary(src);

    EXPECT_EQ(src, dst);
}

TEST(ArchiverSerializerTest, BinaryRoundTrip_UnorderedMap_Empty)
{
    TUnorderedMap<int, int> src;

    const auto dst = round_trip_binary(src);

    EXPECT_TRUE(dst.empty());
}

TEST(ArchiverSerializerTest, BinaryRoundTrip_UnorderedMap_StructValue)
{
    TUnorderedMap<String, Weapon> src;
    src["main"] = Weapon { "Sword", 120 };
    src["sub"]  = Weapon { "Dagger", 45 };

    const auto dst = round_trip_binary(src);

    EXPECT_EQ(src, dst);
}

TEST(ArchiverSerializerTest, BinaryRoundTrip_UnorderedMap_NestedInStruct)
{
    ItemTable src;
    src.items = { { 101, "Potion" }, { 202, "Ether" }, { 303, "Elixir" } };

    const ItemTable dst = round_trip_binary(src);

    EXPECT_EQ(src, dst);
}

TEST(ArchiverSerializerTest, BinaryRoundTrip_UnorderedMap_SingleEntry)
{
    TUnorderedMap<String, int> src = { { "only", 99 } };

    const auto dst = round_trip_binary(src);

    EXPECT_EQ(dst.size(), 1u);
    EXPECT_EQ(dst.at("only"), 99);
}

TEST(ArchiverSerializerTest, BinaryRoundTrip_UnorderedMap_StructKeyToInt)
{
    TUnorderedMap<ItemId, int, std::hash<ItemId>> src;
    src[ItemId { 1 }] = 100;
    src[ItemId { 2 }] = 200;
    src[ItemId { 3 }] = 300;

    const auto dst = round_trip_binary(src);

    EXPECT_EQ(src, dst);
}

TEST(ArchiverSerializerTest, BinaryRoundTrip_UnorderedMap_StructKeyToStructValue)
{
    TUnorderedMap<ItemId, Weapon, std::hash<ItemId>> src;
    src[ItemId { 10 }] = Weapon { "Sword", 120 };
    src[ItemId { 20 }] = Weapon { "Dagger", 45 };
    src[ItemId { 30 }] = Weapon { "Staff", 80 };

    const auto dst = round_trip_binary(src);

    EXPECT_EQ(src, dst);
}

TEST(ArchiverSerializerTest, BinaryInput_ThrowsOnBufferOverrun)
{
    TVector<uint8_t> invalidBuffer = { 0x01, 0x02 };

    BinaryInputArchive iar(invalidBuffer);

    int value = 0;
    EXPECT_THROW(iar.Value(value), std::exception);
}

TEST(ArchiverSerializerTest, BinaryCanSerializeMultipleValuesSequentially)
{
    TVector<uint8_t> buffer;

    SimpleData a;
    a.id = 1;
    a.value = 2.0f;
    a.name = "A";

    SimpleData b;
    b.id = 2;
    b.value = 4.0f;
    b.name = "B";

    {
        BinaryOutputArchive oar(buffer);
        oar.Value(a);
        oar.Value(b);
    }

    SimpleData outA;
    SimpleData outB;
    {
        BinaryInputArchive iar(buffer);
        iar.Value(outA);
        iar.Value(outB);
    }

    EXPECT_EQ(a, outA);
    EXPECT_EQ(b, outB);
}
