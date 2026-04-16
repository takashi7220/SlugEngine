#include <gtest/gtest.h>
#include "core/serialize/archiver/ArchiverSerializer.hpp"
#include "ArchiverSerializeClass.hpp"

#if 0
namespace
{
using namespace slug::core;
using namespace slug::test;

template<typename T>
T round_trip_json(const T& src)
{
    JsonOutputArchive oar;
    oar.value(src);

    T dst {};
    {
        JsonInputArchive iar(oar.root());
        iar.value(dst);
    }

    return dst;
}
}

TEST(SerializerTest, JsonRoundTrip_SimpleObject)
{
    SimpleData src;
    src.id = 7;
    src.value = 1.25f;
    src.name = "json_test";

    const SimpleData dst = round_trip_json(src);
    EXPECT_EQ(src, dst);
}

TEST(SerializerTest, JsonRoundTrip_VectorAndString)
{
    Inventory src;
    src.itemIds = { 100, 200, 300 };
    src.owner = "player_b";

    const Inventory dst = round_trip_json(src);
    EXPECT_EQ(src, dst);
}

TEST(SerializerTest, JsonRoundTrip_NestedObject)
{
    Player src;
    src.id = 100;
    src.name = "Mage";
    src.hp = 65.25f;
    src.inventoryIds = { 7, 8, 9 };
    src.weapon.name = "Staff";
    src.weapon.attack = 55;

    const Player dst = round_trip_json(src);
    EXPECT_EQ(src, dst);
}

TEST(SerializerTest, JsonRoundTrip_InheritanceBaseClass)
{
    Character src;
    src.entityId = 777;
    src.name = "Archer";
    src.level = 21;
    src.skillIds = { 11, 22 };

    const Character dst = round_trip_json(src);
    EXPECT_EQ(src, dst);
}

TEST(SerializerTest, JsonOutput_ContainsExpectedFields)
{
    Player src;
    src.id = 1;
    src.name = "Hero";
    src.hp = 50.0f;
    src.inventoryIds = { 1, 2 };
    src.weapon.name = "Dagger";
    src.weapon.attack = 9;

    JsonOutputArchive oar;
    oar.value(src);

    const String json = oar.str();

    EXPECT_NE(json.find("\"id\":1"), String::npos);
    EXPECT_NE(json.find("\"name\":\"Hero\""), String::npos);
    EXPECT_NE(json.find("\"hp\":50"), String::npos);
    EXPECT_NE(json.find("\"inventoryIds\":[1,2]"), String::npos);
    EXPECT_NE(json.find("\"weapon\""), String::npos);
    EXPECT_NE(json.find("\"attack\":9"), String::npos);
}

TEST(SerializerTest, JsonOutput_InheritanceContainsBaseNode)
{
    Character src;
    src.entityId = 314;
    src.name = "Rogue";
    src.level = 8;
    src.skillIds = { 2, 4 };

    JsonOutputArchive oar;
    oar.value(src);

    const String json = oar.str();

    EXPECT_NE(json.find("\"base\""), String::npos);
    EXPECT_NE(json.find("\"entityId\":314"), String::npos);
    EXPECT_NE(json.find("\"name\":\"Rogue\""), String::npos);
}

TEST(SerializerTest, JsonInput_ThrowsWhenFieldMissing)
{
    JsonValue::Object obj;
    obj["id"] = JsonValue(static_cast<std::int64_t>(10));
    JsonValue root(obj);

    JsonInputArchive iar(root);

    SimpleData dst;
    EXPECT_THROW(iar.value(dst), mini_cereal3::Exception);
}

TEST(SerializerTest, JsonInput_ThrowsOnTypeMismatch)
{
    JsonValue::Object obj;
    obj["id"] = JsonValue(String("not_integer"));
    obj["value"] = JsonValue(1.0);
    obj["name"] = JsonValue(String("abc"));

    JsonValue root(obj);
    JsonInputArchive iar(root);

    SimpleData dst;
    EXPECT_THROW(iar.value(dst), mini_cereal3::Exception);
}
#endif


