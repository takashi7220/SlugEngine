#include "core/pack/PackUtility.hpp"
#include "core/filesystem/FileSystem.hpp"
#include "core/memory/MemoryUtilities.hpp"
#include "core/debug/Assert.hpp"

namespace slug::core
{

struct PackItemMeta
{
    PackFilePath path = {};
    uint32_t size = 0;
    std::streampos dataPos = {};
};

void PackFileUtility::AddFromFolder(core::StringView folderPath, TaskSystem& taskSystem, BuildPackItems& items)
{
    core::TVector<core::String> filePaths = {};
    FileSystem::EnumrateFiles(folderPath, filePaths);
    size_t offset = items.size();
    items.resize(offset + filePaths.size());

    taskSystem.ParallelFor(0, filePaths.size(), ParallelForOptions(), [&](size_t index)
    {
        BuildPackItem& item = items.at(index);
        String& filePath = filePaths.at(index);
        ReadToPackItem(filePath, item);
    });
}

void PackFileUtility::AddFromFile(core::StringView filePath, BuildPackItems& items)
{
    auto blob = FileSystem::Read(filePath);
    ReadToPackItem(filePath, items.emplace_back());
}

bool PackFileUtility::LoadFromFile(core::StringView filePath, TaskSystem& taskSystem, BuildPackItems& items)
{
    auto blob = FileSystem::Read(filePath);
    return LoadFromMemory(blob->data(), blob->size(), taskSystem, items);
}

bool PackFileUtility::LoadFromMemory(void* data, size_t size, TaskSystem& taskSystem, BuildPackItems& items)
{
    Clear(items);

    const char* basePtr = reinterpret_cast<const char*>(data);
    const char* current = basePtr;
    const char* end = basePtr + size;

    // 簡易read関数
    auto read = [&](auto* out, size_t sz)
        {
            if (current + sz > end)
            {
                SLUG_ASSERT_MSG(false, "Out of bounds");
                return;
            }
            core::MemoryUtilities::Memcpy(out, sz, current, sz);
            current += sz;
        };

    // headerを取得
    PackHeader header = {};
    read(&header, sizeof(header));

    // 領域確保
    core::TVector<PackItemMeta> packItemMetas(header.count);
    items.resize(header.count);

    // 各ファイルのmeta情報を取得
    for (size_t i = 0; i < packItemMetas.size(); i++)
    {
        auto& itemMeta = packItemMetas.at(i);
        read(&itemMeta.path, sizeof(itemMeta.path));
        read(&itemMeta.size, sizeof(itemMeta.size));
        itemMeta.dataPos = current - basePtr;
        current += itemMeta.size;
    }

    taskSystem.ParallelFor(0, header.count, ParallelForOptions(), [&](size_t index)
    {
        auto& meta = packItemMetas.at(index);
        auto& item = items.at(index);
        void* dataStart = (void*)(basePtr + meta.dataPos);
        ReadToPackItem(meta.path.view(), dataStart, meta.size, item);
    });

    return true;
}

bool PackFileUtility::SaveToFile(core::StringView filePath, TaskSystem& taskSystem, BuildPackItems& items)
{
    core::TVector<uint8_t> buffer = {};
    SaveToMemory(buffer, taskSystem, items);

    FileBlobPtr blob(new FileBlob(buffer.data(), buffer.size()));
    FileSystem::Write(filePath, blob);
    return true;
}

bool PackFileUtility::SaveToMemory(core::TVector<uint8_t>& buffer, TaskSystem& taskSystem, BuildPackItems& items)
{
    size_t fileCount = items.size();
    size_t totalSize = CalcTotalMemorySize(items);
    buffer.resize(totalSize);

    size_t offset = 0;

    PackHeader header = {};
    header.count = (uint32_t)fileCount;
    core::MemoryUtilities::Memcpy(buffer.data() + offset, sizeof(PackHeader), &header, sizeof(PackHeader));
    offset += sizeof(PackHeader);

    struct BufferMeta
    {
        size_t offset;
        size_t size;
    };

    core::TVector<BufferMeta> bufferMetas = {};
    bufferMetas.reserve(fileCount);
    size_t bufferOffset = 0;
    for (auto& item : items)
    {
        bufferMetas.push_back(BufferMeta { bufferOffset, item.data.size() });
        bufferOffset += sizeof(PackFilePath);
        bufferOffset += sizeof(uint32_t);
        bufferOffset += item.data.size();
    }

    taskSystem.ParallelFor(0, fileCount, ParallelForOptions(), [&](size_t index)
    {
        auto& bufferMeta = bufferMetas.at(index);
        auto& item = items.at(index);

        size_t currentOffset = offset + bufferMeta.offset;

        core::MemoryUtilities::Memcpy((void*)(buffer.data() + currentOffset), sizeof(uint32_t), &bufferMeta.size, sizeof(uint32_t));
        currentOffset += sizeof(uint32_t);

        core::MemoryUtilities::Memcpy((void*)(buffer.data() + currentOffset), sizeof(PackFilePath), &item.path, sizeof(PackFilePath));
        currentOffset += sizeof(PackFilePath);

        core::MemoryUtilities::Memcpy((void*)(buffer.data() + currentOffset), bufferMeta.size, item.data.data(), bufferMeta.size);
    });
    return true;
}

void PackFileUtility::Extract(core::StringView filePath, BuildPackItems& items)
{
    for (auto& item : items)
    {
        FileBlobPtr blob(new FileBlob(item.data.data(), item.data.size()));
        FileSystem::Write(item.path.view(), blob);
    }
}

void PackFileUtility::Clear(BuildPackItems& items)
{
    items.clear();
}

void PackFileUtility::ReadToPackItem(core::StringView filePath, BuildPackItem& item)
{
    auto blob = FileSystem::Read(filePath);
    ReadToPackItem(filePath, blob->data(), blob->size(), item);
}

void PackFileUtility::ReadToPackItem(core::StringView filePath, void* data, size_t size, BuildPackItem& item)
{
    item.path = filePath;
    item.data.resize(size);
    core::MemoryUtilities::Memcpy(item.data.data(), item.data.size(), data, size);
}

size_t PackFileUtility::CalcTotalMemorySize(BuildPackItems& items)
{
    size_t totalSize = 0;
    for (auto& item : items)
    {
        totalSize += sizeof(FixedString<1024>);
        totalSize += sizeof(uint32_t);
        totalSize += item.data.size();
    }
    totalSize += sizeof(PackHeader);
    return totalSize;
}

}
