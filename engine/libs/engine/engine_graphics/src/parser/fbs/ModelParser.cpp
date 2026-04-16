#include "engine_graphics/parser/ModelParser.hpp"
#include "engine_graphics/asset/MaterialAsset.hpp"
#include "engine_core/asset/AssetUtility.hpp"
#include "render/utility/Utility.hpp"
#include <srtf/Model_fbs.h>

namespace slug::engine_graphics
{

struct VertexFormatMapping
{
    render::FormatType format;
    fbs::srtf::VertexFormat vertexFormat;
};

struct VertexAttributeTypeMapping
{
    render::VertexAttributeType type;
    fbs::srtf::SemanticType semanticType;
};

static VertexFormatMapping VERTEX_FORMAT_MAPPING[] =
{
    {render::FormatType::UNKNOWN, fbs::srtf::VertexFormat::VertexFormat_NONE},
    {render::FormatType::R8_SINT, fbs::srtf::VertexFormat::VertexFormat_BYTE1},
    {render::FormatType::RG8_SINT, fbs::srtf::VertexFormat::VertexFormat_BYTE2},
    {render::FormatType::RGBA8_SINT, fbs::srtf::VertexFormat::VertexFormat_BYTE4},
    {render::FormatType::RGBA8_SNORM, fbs::srtf::VertexFormat::VertexFormat_BYTE4N},
    {render::FormatType::R8_UINT, fbs::srtf::VertexFormat::VertexFormat_UBYTE1},
    {render::FormatType::RG8_UINT, fbs::srtf::VertexFormat::VertexFormat_UBYTE2},
    {render::FormatType::RGBA8_UINT, fbs::srtf::VertexFormat::VertexFormat_UBYTE4},
    {render::FormatType::RGBA8_UNORM, fbs::srtf::VertexFormat::VertexFormat_UBYTE4N},
    {render::FormatType::R16_SINT, fbs::srtf::VertexFormat::VertexFormat_SHORT1},
    {render::FormatType::RG16_SINT, fbs::srtf::VertexFormat::VertexFormat_SHORT2},
    {render::FormatType::RGBA16_SINT, fbs::srtf::VertexFormat::VertexFormat_SHORT4},
    {render::FormatType::RGBA16_SNORM, fbs::srtf::VertexFormat::VertexFormat_SHORT4N},
    {render::FormatType::R16_UINT, fbs::srtf::VertexFormat::VertexFormat_USHORT1},
    {render::FormatType::RG16_UINT, fbs::srtf::VertexFormat::VertexFormat_USHORT2},
    {render::FormatType::RGBA16_UINT, fbs::srtf::VertexFormat::VertexFormat_USHORT4},
    {render::FormatType::RGBA16_UNORM, fbs::srtf::VertexFormat::VertexFormat_USHORT4N},
    {render::FormatType::R32_SINT, fbs::srtf::VertexFormat::VertexFormat_INT1},
    {render::FormatType::RG32_SINT, fbs::srtf::VertexFormat::VertexFormat_INT2},
    {render::FormatType::RGBA32_SINT, fbs::srtf::VertexFormat::VertexFormat_INT4},
    {render::FormatType::R32_UINT, fbs::srtf::VertexFormat::VertexFormat_UINT1},
    {render::FormatType::RG32_UINT, fbs::srtf::VertexFormat::VertexFormat_UINT2},
    {render::FormatType::RGBA32_UINT, fbs::srtf::VertexFormat::VertexFormat_UINT4},
    {render::FormatType::R16_FLOAT, fbs::srtf::VertexFormat::VertexFormat_FLOAT1},
    {render::FormatType::RG16_FLOAT, fbs::srtf::VertexFormat::VertexFormat_FLOAT2},
    {render::FormatType::RGBA16_FLOAT, fbs::srtf::VertexFormat::VertexFormat_FLOAT4},
    {render::FormatType::R11G11B10_FLOAT, fbs::srtf::VertexFormat::VertexFormat_R11G11B10Float},
    {render::FormatType::R10G10B10A2_UNORM, fbs::srtf::VertexFormat::VertexFormat_R10G10B10A2Unorm}
};

static VertexAttributeTypeMapping VERTEX_ATTRIBUTE_TYPE_MAPPING[] =
{
    {render::VertexAttributeType::Position, fbs::srtf::SemanticType::SemanticType_Position },
    {render::VertexAttributeType::Normal, fbs::srtf::SemanticType::SemanticType_Normal },
    {render::VertexAttributeType::Texcoord, fbs::srtf::SemanticType::SemanticType_Texcoord },
    {render::VertexAttributeType::Tangent, fbs::srtf::SemanticType::SemanticType_Tangent },
    {render::VertexAttributeType::Binormal, fbs::srtf::SemanticType::SemanticType_Binormal },
    {render::VertexAttributeType::BoneIndex, fbs::srtf::SemanticType::SemanticType_BoneIndex },
    {render::VertexAttributeType::BoneWeight, fbs::srtf::SemanticType::SemanticType_BoneWeight },
};

render::FormatType ConvertFormat(fbs::srtf::VertexFormat format)
{
    for (const VertexFormatMapping& vertexFormat : VERTEX_FORMAT_MAPPING)
    {
        if (vertexFormat.vertexFormat == format)
        {
            return vertexFormat.format;
        }
    }
    return render::FormatType::UNKNOWN;
}

render::VertexAttributeType ConvertVertexAttributeType(fbs::srtf::SemanticType semanticType)
{
    for (const VertexAttributeTypeMapping& attributeType : VERTEX_ATTRIBUTE_TYPE_MAPPING)
    {
        if (attributeType.semanticType == semanticType)
        {
            return attributeType.type;
        }
    }
    return render::VertexAttributeType::Position;
}

render::MeshPtr ParseFbxMesh(render::DrawContext& context, const fbs::srtf::OptimizeMesh* srcMesh, const fbs::srtf::SubModel* srcSubModel)
{
    render::Mesh::SetupParameter param = {};
    param.write = true;

    // vertex accessor
    {
        for (uint32_t i = 0; i < srcMesh->vertex_accessors()->size(); i++)
        {
            auto& dstVertexAccessor = param.vertexAccessor.attributeAccessors.emplace_back();
            auto srcVertexAccessor = srcMesh->vertex_accessors()->Get(i);
            dstVertexAccessor.attributeIndex = srcVertexAccessor->semantic_index();
            dstVertexAccessor.attributeName = render::SlotUtility::GetVertexAttributeName(ConvertVertexAttributeType(srcVertexAccessor->semantic_type()));
            dstVertexAccessor.format = ConvertFormat(srcVertexAccessor->foramt());
            dstVertexAccessor.offset = srcVertexAccessor->offset();
            dstVertexAccessor.size = srcVertexAccessor->size();
            param.vertexAccessor.attributeAccessors.push_back(dstVertexAccessor);
        }
        param.vertexAccessor.bufferIndex = 0;
        param.vertexAccessor.count = srcMesh->vertex_count();
    }

    // index accessor
    {
        auto srcIndexAccessor = srcMesh->index_accessor();
        auto& dstIndexAccessor = param.indexAccessor;
        dstIndexAccessor.attributeName = render::SlotUtility::GetVertexAttributeName(render::VertexAttributeType::Index);
        dstIndexAccessor.format = ConvertFormat(srcIndexAccessor->foramt());
        dstIndexAccessor.count = srcMesh->index_count();
        dstIndexAccessor.size = srcIndexAccessor->size();
        dstIndexAccessor.bufferIndex = 0;
    }

    // buffer
    {
        auto& dstBuffer = param.buffers.emplace_back();
        dstBuffer.name = srcSubModel->buffer()->name()->c_str();
        dstBuffer.size = srcSubModel->buffer()->source()->size();
        dstBuffer.data = (void*)srcSubModel->buffer()->source()->data();
    }

    param.materialSlotName = srcSubModel->materials()->Get(srcMesh->material())->slot()->c_str();
    return render::Mesh::Create(context, param);
}

render::SubModelPtr ParseFbsSubModel(render::DrawContext& context, const fbs::srtf::SubModel* srcSubModel)
{
    render::SubModelPtr dstSubModel = core::MakeReference<render::SubModel>();
    for (uint32_t i = 0; i < srcSubModel->optimize_meshes()->size(); i++)
    {
        dstSubModel->meshes.push_back(ParseFbxMesh(context, srcSubModel->optimize_meshes()->Get(i), srcSubModel));
    }
    return dstSubModel;
}

render::LodModelPtr ParseFbsLodModel(render::DrawContext& context, const fbs::srtf::LodModel* srcLodModel)
{
    render::LodModelPtr dstLodModel = core::MakeReference<render::LodModel>();
    for (uint32_t i = 0; i < srcLodModel->sub_models()->size(); i++)
    {
        dstLodModel->subModels.push_back(ParseFbsSubModel(context, srcLodModel->sub_models()->Get(i)));
    }

    auto srcLodSettings = srcLodModel->lod_settings();
    dstLodModel->lodSettings.offsteLodIndex = srcLodSettings->offset_lod_index();
    for (uint32_t i = 0; i < srcLodSettings->items()->size(); i++)
    {
        auto srcLodItemSettings = srcLodSettings->items()->Get(i);
        auto& dstLodItemSettigns = dstLodModel->lodSettings.items.emplace_back();
        dstLodItemSettigns.drawPixelAreaRatio = srcLodItemSettings->draw_pixel_area_ratio();
    }
    return dstLodModel;
}

void ParseMaterialSlot(ModelParser::MaterialSlot& dstMaterialSlot, const fbs::srtf::MaterialSlot* srcMaterialSlot)
{
    dstMaterialSlot.slotName = srcMaterialSlot->slot()->c_str();
    dstMaterialSlot.materialId.path = engine_core::AssetPath(srcMaterialSlot->material()->c_str());
    dstMaterialSlot.materialId.type = engine_core::AssetType(MaterialAsset::GetStaticAssetType());
}

bool ModelParser::Parse(const InputParam& input, OutParam& out)
{
    if (!fbs::srtf::ModelBufferHasIdentifier(input.data))
    {
        return false;
    }

    auto fbsModel = fbs::srtf::GetModel(input.data);
    if (fbsModel == nullptr)
    {
        return false;
    }

    // model
    out.model.reset();
    out.model = core::MakeReference<render::Model>();
    out.model->mainModel = ParseFbsSubModel(*input.context, fbsModel->main_model());
    out.model->lodModel = ParseFbsLodModel(*input.context, fbsModel->lod_model());

    // material
    for (uint32_t i = 0; i < fbsModel->main_model()->materials()->size(); i++)
    {
        ParseMaterialSlot(out.materialSlots.emplace_back(), fbsModel->main_model()->materials()->Get(i));
    }

    return true;
}

}
