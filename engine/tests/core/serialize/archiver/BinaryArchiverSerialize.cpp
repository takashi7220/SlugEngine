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
