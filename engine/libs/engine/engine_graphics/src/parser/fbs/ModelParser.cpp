#include "engine_graphics/parser/ModelParser.hpp"
#include "engine_graphics/asset/MaterialAsset.hpp"
#include "engine_core/asset/AssetUtility.hpp"
#include "render_core/utility/Utility.hpp"
#include <srtf/Model_fbs.h>

namespace slug::engine_graphics
{

struct VertexFormatMapping
{
    render_core::FormatType format;
    fbs::srtf::VertexFormat vertexFormat;
};

struct VertexAttributeTypeMapping
{
    render_core::VertexAttributeType type;
    fbs::srtf::SemanticType semanticType;
};

static VertexFormatMapping VERTEX_FORMAT_MAPPING[] =
{
    {render_core::FormatType::UNKNOWN, fbs::srtf::VertexFormat::VertexFormat_NONE},
    {render_core::FormatType::R8_SINT, fbs::srtf::VertexFormat::VertexFormat_BYTE1},
    {render_core::FormatType::RG8_SINT, fbs::srtf::VertexFormat::VertexFormat_BYTE2},
    {render_core::FormatType::RGBA8_SINT, fbs::srtf::VertexFormat::VertexFormat_BYTE4},
    {render_core::FormatType::RGBA8_SNORM, fbs::srtf::VertexFormat::VertexFormat_BYTE4N},
    {render_core::FormatType::R8_UINT, fbs::srtf::VertexFormat::VertexFormat_UBYTE1},
    {render_core::FormatType::RG8_UINT, fbs::srtf::VertexFormat::VertexFormat_UBYTE2},
    {render_core::FormatType::RGBA8_UINT, fbs::srtf::VertexFormat::VertexFormat_UBYTE4},
    {render_core::FormatType::RGBA8_UNORM, fbs::srtf::VertexFormat::VertexFormat_UBYTE4N},
    {render_core::FormatType::R16_SINT, fbs::srtf::VertexFormat::VertexFormat_SHORT1},
    {render_core::FormatType::RG16_SINT, fbs::srtf::VertexFormat::VertexFormat_SHORT2},
    {render_core::FormatType::RGBA16_SINT, fbs::srtf::VertexFormat::VertexFormat_SHORT4},
    {render_core::FormatType::RGBA16_SNORM, fbs::srtf::VertexFormat::VertexFormat_SHORT4N},
    {render_core::FormatType::R16_UINT, fbs::srtf::VertexFormat::VertexFormat_USHORT1},
    {render_core::FormatType::RG16_UINT, fbs::srtf::VertexFormat::VertexFormat_USHORT2},
    {render_core::FormatType::RGBA16_UINT, fbs::srtf::VertexFormat::VertexFormat_USHORT4},
    {render_core::FormatType::RGBA16_UNORM, fbs::srtf::VertexFormat::VertexFormat_USHORT4N},
    {render_core::FormatType::R32_SINT, fbs::srtf::VertexFormat::VertexFormat_INT1},
    {render_core::FormatType::RG32_SINT, fbs::srtf::VertexFormat::VertexFormat_INT2},
    {render_core::FormatType::RGBA32_SINT, fbs::srtf::VertexFormat::VertexFormat_INT4},
    {render_core::FormatType::R32_UINT, fbs::srtf::VertexFormat::VertexFormat_UINT1},
    {render_core::FormatType::RG32_UINT, fbs::srtf::VertexFormat::VertexFormat_UINT2},
    {render_core::FormatType::RGBA32_UINT, fbs::srtf::VertexFormat::VertexFormat_UINT4},
    {render_core::FormatType::R16_FLOAT, fbs::srtf::VertexFormat::VertexFormat_FLOAT1},
    {render_core::FormatType::RG16_FLOAT, fbs::srtf::VertexFormat::VertexFormat_FLOAT2},
    {render_core::FormatType::RGBA16_FLOAT, fbs::srtf::VertexFormat::VertexFormat_FLOAT4},
    {render_core::FormatType::R11G11B10_FLOAT, fbs::srtf::VertexFormat::VertexFormat_R11G11B10Float},
    {render_core::FormatType::R10G10B10A2_UNORM, fbs::srtf::VertexFormat::VertexFormat_R10G10B10A2Unorm}
};

static VertexAttributeTypeMapping VERTEX_ATTRIBUTE_TYPE_MAPPING[] =
{
    {render_core::VertexAttributeType::Position, fbs::srtf::SemanticType::SemanticType_Position },
    {render_core::VertexAttributeType::Normal, fbs::srtf::SemanticType::SemanticType_Normal },
    {render_core::VertexAttributeType::Texcoord, fbs::srtf::SemanticType::SemanticType_Texcoord },
    {render_core::VertexAttributeType::Tangent, fbs::srtf::SemanticType::SemanticType_Tangent },
    {render_core::VertexAttributeType::Binormal, fbs::srtf::SemanticType::SemanticType_Binormal },
    {render_core::VertexAttributeType::BoneIndex, fbs::srtf::SemanticType::SemanticType_BoneIndex },
    {render_core::VertexAttributeType::BoneWeight, fbs::srtf::SemanticType::SemanticType_BoneWeight },
};

render_core::FormatType ConvertFormat(fbs::srtf::VertexFormat format)
{
    for (const VertexFormatMapping& vertexFormat : VERTEX_FORMAT_MAPPING)
    {
        if (vertexFormat.vertexFormat == format)
        {
            return vertexFormat.format;
        }
    }
    return render_core::FormatType::UNKNOWN;
}

render_core::VertexAttributeType ConvertVertexAttributeType(fbs::srtf::SemanticType semanticType)
{
    for (const VertexAttributeTypeMapping& attributeType : VERTEX_ATTRIBUTE_TYPE_MAPPING)
    {
        if (attributeType.semanticType == semanticType)
        {
            return attributeType.type;
        }
    }
    return render_core::VertexAttributeType::Position;
}

render_core::MeshPtr ParseFbxMesh(render_core::RenderService& service, const fbs::srtf::OptimizeMesh* srcMesh, const fbs::srtf::SubModel* srcSubModel)
{
    render_core::Mesh::SetupParameter param = {};
    param.write = true;

    // vertex accessor
    {
        for (uint32_t i = 0; i < srcMesh->vertex_accessors()->size(); i++)
        {
            auto& dstVertexAccessor = param.vertexAccessor.attributeAccessors.emplace_back();
            auto srcVertexAccessor = srcMesh->vertex_accessors()->Get(i);
            dstVertexAccessor.attributeIndex = srcVertexAccessor->semantic_index();
            dstVertexAccessor.attributeName = render_core::SlotUtility::GetVertexAttributeName(ConvertVertexAttributeType(srcVertexAccessor->semantic_type()));
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
        dstIndexAccessor.attributeName = render_core::SlotUtility::GetVertexAttributeName(render_core::VertexAttributeType::Index);
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
    return render_core::Mesh::Create(context, param);
}

render_core::SubModelPtr ParseFbsSubModel(render_core::RenderService& service, const fbs::srtf::SubModel* srcSubModel)
{
    render_core::SubModelPtr dstSubModel = core::MakeReference<render_core::SubModel>();
    for (uint32_t i = 0; i < srcSubModel->optimize_meshes()->size(); i++)
    {
        dstSubModel->meshes.push_back(ParseFbxMesh(context, srcSubModel->optimize_meshes()->Get(i), srcSubModel));
    }
    return dstSubModel;
}

render_core::LodModelPtr ParseFbsLodModel(render_core::RenderService& service, const fbs::srtf::LodModel* srcLodModel)
{
    render_core::LodModelPtr dstLodModel = core::MakeReference<render_core::LodModel>();
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
    out.model = core::MakeReference<render_core::Model>();
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
