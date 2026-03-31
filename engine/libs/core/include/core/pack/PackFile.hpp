#pragma once

#include "core/container/String.hpp"
#include "core/container/Vector.hpp"
#include "core/filesystem/FileSystem.hpp"
#include "core/filesystem/FileMapping.hpp"
#include "core/utility/Hash.hpp"
namespace slug
{
namespace core
{

using PackFilePath = core::FixedString<1024>;

struct PackFilePathKeyHash
{
    size_t operator()(const PackFilePath& path) const
    {
        return core::HashUtility::ConvertStringToHash64(path.view());
    }
};

struct PackFilePathKeyCompare
{
    bool operator()(const PackFilePath& v0, const PackFilePath& v1) const
    {
        return v0 == v1.view();
    }
};

struct PackHeader
{
    uint32_t count = 0;
};

struct PackItem
{
    PackFilePath path = {};
    uint32_t size = 0;
    void* data = nullptr;
};

class PackFile : public ReferenceObject
{
public:
    PackFile(core::StringView path);
    virtual ~PackFile();
    const core::TVector<PackItem>& GetItems() const;

    void Open();
    void Close();

private:
    void BuildPackItems();

private:
    core::FileMappingObjectPtr m_fileMappingObject;
    core::PackFilePath m_path;
    core::TVector<PackItem> m_items;
};
using PackFilePtr = core::TReferencePtr<PackFile>;

}
}
