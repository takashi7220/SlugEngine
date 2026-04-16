#pragma once

#include "core/pack/PackFile.hpp"
#include "core/task/TaskSystem.hpp"

namespace slug::core
{

struct BuildPackItem
{
    PackFilePath path;
    core::TVector<uint8_t> data;
};
using BuildPackItems = core::TVector<BuildPackItem>;

class PackFileUtility
{
public:
    static void AddFromFolder(core::StringView folderPath, TaskSystem& taskSystem, BuildPackItems& items);
    static void AddFromFile(core::StringView filePath, BuildPackItems& items);

    static bool LoadFromFile(core::StringView filePath, TaskSystem& taskSystem, BuildPackItems& items);
    static bool LoadFromMemory(void* data, size_t size, TaskSystem& taskSystem, BuildPackItems& items);

    static bool SaveToFile(core::StringView filePath, TaskSystem& taskSystem, BuildPackItems& items);
    static bool SaveToMemory(core::TVector<uint8_t>& buffer, TaskSystem& taskSystem, BuildPackItems& items);

    static void Extract(core::StringView filePath, BuildPackItems& items);
    static void Clear(BuildPackItems& items);

    static void ReadToPackItem(core::StringView filePath, BuildPackItem& item);
    static void ReadToPackItem(core::StringView filePath, void* data, size_t size, BuildPackItem& item);

    static size_t CalcTotalMemorySize(BuildPackItems& item);
};

}
