#pragma once

#include "core/container/String.hpp"
#include "core/debug/Assert.hpp"

namespace slug
{
namespace engine_core
{

#define RESOURCE_HEADER_SIZE 1168

struct AssetHeader
{
    uint16_t version = 0;           // 2byte
    uint16_t headerSize = 0;        // 2byte
    uint32_t metaSize = 0;          // 4byte
    uint64_t dataSize = 0;          // 8byte
    char filePath[1024] = {};       // 1024byte
    char assetType[128] = {};       // 128byte

    static size_t GetSize()
    {
        return RESOURCE_HEADER_SIZE;
    }
};

SLUG_STATIC_ASSERT(sizeof(AssetHeader) == RESOURCE_HEADER_SIZE);

}
}
