#include <gtest/gtest.h>
#include "core/serialize/archiver/ArchiverSerializer.hpp"
#include "ArchiverSerializeClass.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace
{ 
using namespace slug::core;
using namespace slug::test::archiver;

template<typename T>
T round_trip_json(const T& src)
{
    String buffer;
    {
        JsonOutputArchive oar(buffer);
        oar.Value(src);
    }

    T dst {};
    {
        JsonInputArchive iar(buffer);
        iar.Value(dst);
    }

    return dst;
}
}

// ──────────────────────────────────────────────────────────
// ラウンドトリップ
// ──────────────────────────────────────────────────────────

TEST(JsonArchiverSerializerTest, JsonRoundTrip_SimpleObject)
{
    SimpleData src;
    src.id = 42;
    src.value = 3.5f;
    src.name = "alpha";

    const SimpleData dst = round_trip_json(src);
    EXPECT_EQ(src, dst);
}

TEST(JsonArchiverSerializerTest, JsonRoundTrip_VectorAndString)
{
    Inventory src;
    src.itemIds = { 1, 2, 3, 10, 20 };
    src.owner = "player_a";

    const Inventory dst = round_trip_json(src);
    EXPECT_EQ(src, dst);
}

TEST(JsonArchiverSerializerTest, JsonRoundTrip_NestedObject)
{
    Player src;
    src.id = 99;
    src.name = "Hero";
    src.hp = 87.5f;
    src.inventoryIds = { 10, 20, 30 };
    src.weapon.name = "Sword";
    src.weapon.attack = 123;

    const Player dst = round_trip_json(src);
    EXPECT_EQ(src, dst);
}

TEST(JsonArchiverSerializerTest, JsonRoundTrip_InheritanceBaseClass)
{
    Character src;
    src.entityId = 500;
    src.name = "Knight";
    src.level = 12;
    src.skillIds = { 3, 6, 9 };

    const Character dst = round_trip_json(src);
    EXPECT_EQ(src, dst);
}

TEST(JsonArchiverSerializerTest, JsonRoundTrip_UnorderedMap_IntToInt)
{
    TUnorderedMap<int, int> src = { { 1, 10 }, { 2, 20 }, { 3, 30 } };

    const auto dst = round_trip_json(src);
    EXPECT_EQ(src, dst);
}

TEST(JsonArchiverSerializerTest, JsonRoundTrip_UnorderedMap_StringToInt)
{
    TUnorderedMap<String, int> src = { { "hp", 100 }, { "mp", 50 }, { "atk", 30 } };

    const auto dst = round_trip_json(src);
    EXPECT_EQ(src, dst);
}

TEST(JsonArchiverSerializerTest, JsonRoundTrip_UnorderedMap_Empty)
{
    TUnorderedMap<int, int> src;

    const auto dst = round_trip_json(src);
    EXPECT_TRUE(dst.empty());
}

TEST(JsonArchiverSerializerTest, JsonRoundTrip_UnorderedMap_StructValue)
{
    TUnorderedMap<String, Weapon> src;
    src["main"] = Weapon { "Sword", 120 };
    src["sub"]  = Weapon { "Dagger", 45 };

    const auto dst = round_trip_json(src);
    EXPECT_EQ(src, dst);
}

TEST(JsonArchiverSerializerTest, JsonRoundTrip_UnorderedMap_StructKeyToStructValue)
{
    TUnorderedMap<ItemId, Weapon, std::hash<ItemId>> src;
    src[ItemId { 10 }] = Weapon { "Sword", 120 };
    src[ItemId { 20 }] = Weapon { "Dagger", 45 };

    const auto dst = round_trip_json(src);
    EXPECT_EQ(src, dst);
}

TEST(JsonArchiverSerializerTest, JsonRoundTrip_UnorderedMap_NestedInStruct)
{
    ItemTable src;
    src.items = { { 101, "Potion" }, { 202, "Ether" }, { 303, "Elixir" } };

    const ItemTable dst = round_trip_json(src);
    EXPECT_EQ(src, dst);
}

// ──────────────────────────────────────────────────────────
// JSON 出力の内容検証
// ──────────────────────────────────────────────────────────

TEST(JsonArchiverSerializerTest, JsonOutput_ContainsExpectedFields)
{
    Player src;
    src.id = 1;
    src.name = "Hero";
    src.hp = 50.0f;
    src.inventoryIds = { 1, 2 };
    src.weapon.name = "Dagger";
    src.weapon.attack = 9;

    String buffer;
    {
        JsonOutputArchive oar(buffer);
        oar.Value(src);
    }

    EXPECT_NE(buffer.find("\"id\": 1"),          String::npos);
    EXPECT_NE(buffer.find("\"name\": \"Hero\""), String::npos);
    EXPECT_NE(buffer.find("\"hp\": 50"),         String::npos);
    EXPECT_NE(buffer.find("\"attack\": 9"),      String::npos);
    EXPECT_NE(buffer.find("\"weapon\":"),        String::npos);
    EXPECT_NE(buffer.find("\"Dagger\""),         String::npos);
}

TEST(JsonArchiverSerializerTest, JsonOutput_ArrayFormat)
{
    Inventory src;
    src.itemIds = { 10, 20, 30 };
    src.owner = "owner";

    String buffer;
    {
        JsonOutputArchive oar(buffer);
        oar.Value(src);
    }

    EXPECT_NE(buffer.find("\"itemIds\":"),  String::npos);
    EXPECT_NE(buffer.find("10"),            String::npos);
    EXPECT_NE(buffer.find("20"),            String::npos);
    EXPECT_NE(buffer.find("30"),            String::npos);
}

TEST(JsonArchiverSerializerTest, JsonOutput_StringEscaping)
{
    SimpleData src;
    src.id = 0;
    src.value = 0.0f;
    src.name = "say \"hello\"";

    String buffer;
    {
        JsonOutputArchive oar(buffer);
        oar.Value(src);
    }

    EXPECT_NE(buffer.find("\\\"hello\\\""), String::npos);
}

// ──────────────────────────────────────────────────────────
// エラーハンドリング
// ──────────────────────────────────────────────────────────

TEST(JsonArchiverSerializerTest, JsonInput_ThrowsWhenFieldMissing)
{
    // "name" フィールドが欠けている JSON
    const String json = R"({"id":1,"value":1.0})";
    JsonInputArchive iar(json);

    SimpleData dst;
    EXPECT_THROW(iar.Value(dst), std::exception);
}

TEST(JsonArchiverSerializerTest, JsonInput_ThrowsOnInvalidJson)
{
    const String json = "not valid json {{{";
    EXPECT_THROW({ JsonInputArchive iar(json); }, std::exception);
}

// ──────────────────────────────────────────────────────────
// 複雑階層クラス (Hero: 3段継承 + ネスト構造体 + コンテナ)
// ──────────────────────────────────────────────────────────

TEST(JsonArchiverSerializerTest, JsonRoundTrip_Hero)
{
    Hero src;
    src.objectId = 1;
    src.tag      = "player";
    src.name     = "Arthas";
    src.stats    = { 500, 200, 1.5f, 0.8f };
    src.level    = 25;
    src.equipment.mainWeapon  = { "Frostmourne", 300 };
    src.equipment.subWeapon   = { "Shield", 0 };
    src.equipment.accessories = { "Ring of Power", "Amulet of Speed" };
    src.activeEffects = { { "Freeze", 50, 3 }, { "Bless", 20, 10 } };
    src.attributes    = { { "strength", 80 }, { "dexterity", 60 }, { "intelligence", 40 } };
    src.inventory[ItemId { 1 }] = 5;
    src.inventory[ItemId { 2 }] = 3;

    const Hero dst = round_trip_json(src);
    EXPECT_EQ(src, dst);
}

TEST(JsonArchiverSerializerTest, JsonRoundTrip_Hero_Empty)
{
    Hero src;

    const Hero dst = round_trip_json(src);
    EXPECT_EQ(src, dst);
}

// ──────────────────────────────────────────────────────────
// ファイル入出力
// ──────────────────────────────────────────────────────────

TEST(JsonArchiverSerializerTest, JsonFileRoundTrip_Character)
{
    Character src;
    src.entityId  = 500;
    src.name      = "Knight";
    src.level     = 12;
    src.skillIds  = { 3, 6, 9 };

    // シリアライズ
    String json;
    {
        JsonOutputArchive oar(json);
        oar.Value(src);
    }

    // ファイル出力
    const std::filesystem::path filePath = ".tmp/test_core_serialize_archiver_json.json";
    std::filesystem::create_directories(filePath.parent_path());

    {
        std::ofstream ofs(filePath);
        ASSERT_TRUE(ofs.is_open());
        ofs << json.c_str();
    }

    // ファイル読み込み
    String loaded;
    {
        std::ifstream ifs(filePath);
        ASSERT_TRUE(ifs.is_open());
        std::ostringstream ss;
        ss << ifs.rdbuf();
        loaded = String(ss.str().c_str());
    }

    // デシリアライズして比較
    Character dst{};
    {
        JsonInputArchive iar(loaded);
        iar.Value(dst);
    }

    EXPECT_EQ(src, dst);
}

TEST(JsonArchiverSerializerTest, JsonFileRoundTrip_Hero)
{
    Hero src;
    src.objectId = 1;
    src.tag      = "player";
    src.name     = "Arthas";
    src.stats    = { 500, 200, 1.5f, 0.8f };
    src.level    = 25;
    src.equipment.mainWeapon  = { "Frostmourne", 300 };
    src.equipment.subWeapon   = { "Shield", 0 };
    src.equipment.accessories = { "Ring of Power", "Amulet of Speed" };
    src.activeEffects = { { "Freeze", 50, 3 }, { "Bless", 20, 10 } };
    src.attributes    = { { "strength", 80 }, { "dexterity", 60 }, { "intelligence", 40 } };
    src.inventory[ItemId { 1 }] = 5;
    src.inventory[ItemId { 2 }] = 3;

    // シリアライズ
    String json;
    {
        JsonOutputArchive oar(json);
        oar.Value(src);
    }

    // ファイル出力
    const std::filesystem::path filePath = ".tmp/test_core_serialize_archiver_json.json";
    std::filesystem::create_directories(filePath.parent_path());

    {
        std::ofstream ofs(filePath);
        ASSERT_TRUE(ofs.is_open());
        ofs << json.c_str();
    }

    // ファイル読み込み
    String loaded;
    {
        std::ifstream ifs(filePath);
        ASSERT_TRUE(ifs.is_open());
        std::ostringstream ss;
        ss << ifs.rdbuf();
        loaded = String(ss.str().c_str());
    }

    // デシリアライズして比較
    Hero dst{};
    {
        JsonInputArchive iar(loaded);
        iar.Value(dst);
    }

    EXPECT_EQ(src, dst);
}
