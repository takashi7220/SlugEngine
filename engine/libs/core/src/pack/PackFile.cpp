#include "core/pack/PackFile.hpp"
#include "core/memory/MemoryUtilities.hpp"
#include "core/debug/Assert.hpp"

namespace slug::core
{

PackFile::PackFile(core::StringView path)
    : m_path(path)
{
    Open();
}

PackFile::~PackFile()
{
    Close();
}

const core::TVector<PackItem>& PackFile::GetItems() const
{
    return m_items;
}

void PackFile::Open()
{
    if (m_fileMappingObject != nullptr)
    {
        m_fileMappingObject = FileMappingObject::Create(m_path.view());
    }
    BuildPackItems();
}

void PackFile::Close()
{
    m_fileMappingObject.reset();
}

void PackFile::BuildPackItems()
{
    m_items.clear();
    void* top = m_fileMappingObject->GetData();
    size_t totalSize = m_fileMappingObject->GetSize();

    const char* basePtr = reinterpret_cast<const char*>(top);
    const char* current = basePtr;
    const char* end = basePtr + totalSize;


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

    PackHeader header = {};
    read(&header, sizeof(header));
    m_items.resize(header.count);

    for (size_t i = 0; i < m_items.size(); i++)
    {
        auto& item = m_items.at(i);
        read(&item.path, sizeof(item.path));
        read(&item.size, sizeof(item.size));
        item.data = (void*)current;
        current += item.size;
    }
}

}
