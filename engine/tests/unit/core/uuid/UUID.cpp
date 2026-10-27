#include <gtest/gtest.h>
#include "core/uuid/UUID.hpp"

#include <algorithm>

#include "core/container/UnorderedSet.hpp"
#include "core/container/Vector.hpp"
#include "core/serialize/archiver/ArchiverSerializer.hpp"

using namespace slug::core;

namespace
{

TEST(UUIDTest, DefaultUUIDIsInvalid)
{
    const UUID uuid;

    EXPECT_FALSE(uuid.IsValid());
    EXPECT_EQ(uuid.ToString(), "00000000-0000-0000-0000-000000000000");
    EXPECT_EQ(sizeof(UUID), 16u);
}

TEST(UUIDTest, CopiedUUIDComparesEqual)
{
    UUID uuid = UUID::Generate();
    UUID copy = uuid;

    EXPECT_TRUE(uuid == copy);
    EXPECT_EQ(uuid.GetHash(), copy.GetHash());
    EXPECT_EQ(uuid.Get(), copy.Get());
}

TEST(UUIDTest, GetReturnsSixteenByteView)
{
    UUID uuid = UUID::Generate();

    EXPECT_EQ(uuid.Get().size(), 16u);
}

TEST(UUIDTest, HashFunctorUsesUUIDHash)
{
    UUID uuid = UUID::Generate();
    UUIDHash hash;

    EXPECT_EQ(hash(uuid), uuid.GetHash());
}

TEST(UUIDTest, GeneratedUUIDsAreUsableAsUnorderedSetKeys)
{
    TUnorderedSet<UUID, UUIDHash> values;

    for (int i = 0; i < 32; ++i)
    {
        values.insert(UUID::Generate());
    }

    EXPECT_EQ(values.size(), 32u);
}

TEST(UUIDTest, GeneratedUUIDsHaveVersionFourAndRfcVariant)
{
    for (int i = 0; i < 256; ++i)
    {
        const UUID uuid = UUID::Generate();
        const StringView bytes = uuid.Get();

        EXPECT_EQ(static_cast<unsigned char>(bytes[6]) & 0xF0u, 0x40u);
        EXPECT_EQ(static_cast<unsigned char>(bytes[8]) & 0xC0u, 0x80u);
    }
}

TEST(UUIDTest, GeneratedUUIDsRemainUniqueInLargeBatch)
{
    constexpr size_t Count = 1'000'000;
    TVector<UUID> values;
    values.reserve(Count);
    for (size_t i = 0; i < Count; ++i)
    {
        values.push_back(UUID::Generate());
    }

    std::sort(values.begin(), values.end(), [](const UUID& lhs, const UUID& rhs) noexcept
    {
        return lhs.Get() < rhs.Get();
    });

    EXPECT_EQ(std::adjacent_find(values.begin(), values.end()), values.end());
}

TEST(UUIDTest, ByteAndStringRepresentationsUseTheSameOrder)
{
    const UUID::Bytes bytes = {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
    };
    const UUID uuid(bytes);

    EXPECT_TRUE(uuid.IsValid());
    EXPECT_EQ(uuid.GetBytes(), bytes);
    EXPECT_EQ(uuid.ToString(), "00112233-4455-6677-8899-aabbccddeeff");

    UUID restored;
    ASSERT_TRUE(restored.TrySetString(uuid.ToString()));
    EXPECT_EQ(restored, uuid);
    EXPECT_EQ(restored.GetHash(), uuid.GetHash());

    restored.SetBytes({});
    EXPECT_FALSE(restored.IsValid());
    restored.SetBytes(bytes);
    EXPECT_EQ(restored, uuid);
}

TEST(UUIDTest, StringParsingAcceptsUppercaseAndNilUUIDs)
{
    UUID uuid;
    ASSERT_TRUE(uuid.TrySetString("00112233-4455-6677-8899-AABBCCDDEEFF"));
    EXPECT_EQ(uuid.ToString(), "00112233-4455-6677-8899-aabbccddeeff");

    ASSERT_TRUE(uuid.TrySetString("00000000-0000-0000-0000-000000000000"));
    EXPECT_FALSE(uuid.IsValid());
}

TEST(UUIDTest, InvalidStringDoesNotChangeUUID)
{
    UUID uuid = UUID::Generate();
    const UUID original = uuid;
    const StringView invalidValues[] = {
        "",
        "00112233-4455-6677-8899-aabbccddeef",
        "00112233-4455-6677-8899-aabbccddeeff0",
        "00112233_4455-6677-8899-aabbccddeeff",
        "00112233-4455-6677-8899-aabbccddeefg"
    };
    for (StringView invalid : invalidValues)
    {
        EXPECT_FALSE(uuid.TrySetString(invalid));
        EXPECT_EQ(uuid, original);
    }
}

TEST(UUIDTest, BinaryDataRoundTripsIncludingEmbeddedZeroBytes)
{
    const UUID::Bytes bytes = { 0x00, 0xFF, 0x80, 0x01 };
    const UUID original(bytes);
    const String saved(original.Get());
    UUID restored;

    ASSERT_TRUE(restored.TrySetBytes(saved));
    EXPECT_EQ(restored, original);
    ASSERT_TRUE(restored.TrySetBytes(restored.Get()));
    EXPECT_EQ(restored, original);

    EXPECT_FALSE(restored.TrySetBytes(StringView(saved.data(), saved.size() - 1)));
    EXPECT_FALSE(restored.TrySetBytes(String(17, 'x')));
    EXPECT_EQ(restored, original);
}

TEST(UUIDTest, EqualityUsesAllSixteenBytes)
{
    UUID::Bytes bytes = {};
    bytes[0] = 1;
    const UUID first(bytes);
    bytes[15] = 1;
    const UUID second(bytes);

    EXPECT_NE(first, second);
}

TEST(UUIDTest, SavedStringRoundTripsThroughBinaryAndJsonArchives)
{
    const UUID original = UUID::Generate();
    const String saved = original.ToString();
    TVector<uint8_t> binary;
    BinaryOutputArchive binaryOutput(binary);
    binaryOutput.Value(saved);

    String binaryText;
    BinaryInputArchive binaryInput(binary);
    binaryInput.Value(binaryText);
    UUID fromBinary;
    ASSERT_TRUE(fromBinary.TrySetString(binaryText));
    EXPECT_EQ(fromBinary, original);

    String json;
    {
        JsonOutputArchive output(json);
        output.Value(saved);
    }
    String jsonText;
    JsonInputArchive jsonInput(json);
    jsonInput.Value(jsonText);
    UUID fromJson;
    ASSERT_TRUE(fromJson.TrySetString(jsonText));
    EXPECT_EQ(fromJson, original);
}

}
