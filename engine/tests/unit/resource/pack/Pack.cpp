#include <gtest/gtest.h>
#include <resource/pack/PackFileSystem.hpp>
#include <resource/builder/Builder.hpp>
#include <resource/asset/IAsset.hpp>

using namespace slug;

struct Player
{
    int id = 0;
    core::String name;
    float hp = 0.0f;
    core::TVector<int> inventoryIds;

    template<typename Archive>
    void Serialize(Archive& ar) const
    {
        ar.BeginObject();
        ar.Field("id", id);
        ar.Field("name", name);
        ar.Field("hp", hp);
        ar.Field("inventoryIds", inventoryIds);
        ar.EndObject();
    }

    template<typename Archive>
    void Deserialize(Archive& ar)
    {
        ar.BeginObject();
        ar.Field("id", id);
        ar.Field("name", name);
        ar.Field("hp", hp);
        ar.Field("inventoryIds", inventoryIds);
        ar.EndObject();
    }
};

class PlayerAsset : public resource::IAsset
{
public:
    SLUG_DECLARE_SOBJECT_TYPEINFO(PlayerAsset, resource::IAsset)

    template<typename Archive>
    void Serialize(Archive& ar) const
    {
        ar.BeginObject();
        ar.BaseClass(core::base_class<resource::IAsset>(*this));
        ar.Field("playerData", playerData);
        ar.EndObject();
    }
    template<typename Archive>
    void Deserialize(Archive& ar)
    {
        ar.BeginObject();
        ar.BaseClass(core::base_class<resource::IAsset>(*this));
        ar.Field("playerData", playerData);
        ar.EndObject();
    }

private:
    Player playerData;
};

TEST(ResourceConstants, Test)
{
    PlayerAsset playerAssets[256];

    resource::Builder builder;
    for (const auto& playerAsset : playerAssets)
    {
        core::TVector<uint8_t> buffer;
        core::BinaryOutputArchive oar(buffer);
        oar.Value(playerAsset);
        resource::ChunkId chunkId = core::HashUtility::CreateHash64();

        resource::BuilderChunkDesc chunkDesc;
        chunkDesc.chunkId = chunkId;
        chunkDesc.data = buffer.data();
        chunkDesc.size = static_cast<uint32_t>(buffer.size());
        chunkDesc.compression = resource::CompressionType::None;
        builder.AddChunk(chunkDesc);

        resource::BuilderAssetDesc assetDesc;
        assetDesc.assetId = playerAsset.header.assetId;
        assetDesc.metadataChunkId = chunkId;
        assetDesc.version = playerAsset.header.version;
        assetDesc.name = playerAsset.info.name.c_str();
        builder.AddAsset(assetDesc);
    }

    core::TVector<uint8_t> tocBuffer;
    core::TVector<uint8_t > dataBuffer;
    builder.Build(tocBuffer, dataBuffer);

}
