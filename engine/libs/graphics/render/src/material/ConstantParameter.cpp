#include "render/material/ConstantParameter.hpp"

namespace slug::render
{
bool ConstantParameter::SetParameter(core::StringView name, bool value)
{
    return SetParameterInternal(name, &value, sizeof(bool));
}

bool ConstantParameter::SetParameter(core::StringView name, float value)
{
    return SetParameterInternal(name, &value, sizeof(float));
}

bool ConstantParameter::SetParameter(core::StringView name, const float* values, size_t count)
{
    return SetParameterInternal(name, values, sizeof(float) * count);
}

bool ConstantParameter::SetParameter(core::StringView name, const math::Vector2& value)
{
    return SetParameterInternal(name, &value, sizeof(math::Vector2));
}

bool ConstantParameter::SetParameter(core::StringView name, const math::Vector3& value)
{
    return SetParameterInternal(name, &value, sizeof(math::Vector3));
}

bool ConstantParameter::SetParameter(core::StringView name, const math::Vector4& value)
{
    return SetParameterInternal(name, &value, sizeof(math::Vector4));
}

bool ConstantParameter::SetParameter(core::StringView name, const math::Matrix3x3& value)
{
    return SetParameterInternal(name, &value, sizeof(math::Matrix3x3));
}

bool ConstantParameter::SetParameter(core::StringView name, const math::Matrix4x4& value)
{
    return SetParameterInternal(name, &value, sizeof(math::Matrix4x4));
}

void ConstantParameter::AddParameter(core::StringView name, bool value)
{
    AddParameterInternal(name, ParameterType::Bool, 1, sizeof(bool), &value);
}

void ConstantParameter::AddParameter(core::StringView name, float value)
{
    AddParameterInternal(name, ParameterType::Float, 1, sizeof(float), &value);
}

void ConstantParameter::AddParameter(core::StringView name, const float* values, size_t count)
{
    AddParameterInternal(name, ParameterType::FloatArray, count, sizeof(float) * count, values);
}

void ConstantParameter::AddParameter(core::StringView name, const math::Vector2& value)
{
    AddParameterInternal(name, ParameterType::Float2, 1, sizeof(math::Vector2), &value);
}

void ConstantParameter::AddParameter(core::StringView name, const math::Vector3& value)
{
    AddParameterInternal(name, ParameterType::Float3, 1, sizeof(math::Vector3), &value);
}

void ConstantParameter::AddParameter(core::StringView name, const math::Vector4& value)
{
    AddParameterInternal(name, ParameterType::Float4, 1, sizeof(math::Vector4), &value);
}

void ConstantParameter::AddParameter(core::StringView name, const math::Matrix3x3& value)
{
    AddParameterInternal(name, ParameterType::Float3x3, 1, sizeof(math::Matrix3x3), &value);
}

void ConstantParameter::AddParameter(core::StringView name, const math::Matrix4x4& value)
{
    AddParameterInternal(name, ParameterType::Float4x4, 1, sizeof(math::Matrix4x4), &value);
}

void ConstantParameter::AddParameter(core::StringView name, ParameterType type, size_t count, size_t size, const void* data)
{
    AddParameterInternal(name, type, count, size, data);
}

bool ConstantParameter::GetParameter(core::StringView name, bool& value) const
{
    return GetParameterInternal(name, &value, sizeof(bool));
}

bool ConstantParameter::GetParameter(core::StringView name, float& value) const
{
    return GetParameterInternal(name, &value, sizeof(float));
}

bool ConstantParameter::GetParameter(core::StringView name, core::TVector<float>& values) const
{
    auto accessor = GetParameterAccessor(name);
    values.resize(accessor->count);
    return GetParameterInternal(name, &values.front(), sizeof(float) * values.size());
}

bool ConstantParameter::GetParameter(core::StringView name, math::Vector2& value) const
{
    return GetParameterInternal(name, &value, sizeof(math::Vector2));
}

bool ConstantParameter::GetParameter(core::StringView name, math::Vector3& value) const
{
    return GetParameterInternal(name, &value, sizeof(math::Vector3));
}

bool ConstantParameter::GetParameter(core::StringView name, math::Vector4& value) const
{
    return GetParameterInternal(name, &value, sizeof(math::Vector4));
}

bool ConstantParameter::GetParameter(core::StringView name, math::Matrix3x3& value) const
{
    return GetParameterInternal(name, &value, sizeof(math::Matrix3x3));
}

bool ConstantParameter::GetParameter(core::StringView name, math::Matrix4x4& value) const
{
    return GetParameterInternal(name, &value, sizeof(math::Matrix4x4));
}

bool ConstantParameter::HasParameter(core::StringView name) const
{
    for (auto& itr : m_accessors)
    {
        if (itr.name == name)
        {
            return true;
        }
    }
    return false;
}

const core::TVector<uint8_t>& ConstantParameter::GetConstantBufferPtr() const
{
    return m_constantBuffer;
}

size_t ConstantParameter::GetCostantBufferSize() const
{
    return m_constantBuffer.size();
}

const core::TVector<ParameterAccessor>& ConstantParameter::GetAccessors() const
{
    return m_accessors;
}

void ConstantParameter::GetParameterNames(core::TVector<core::StringView>& names) const
{
    for (auto itr : m_accessors)
    {
        names.push_back(itr.name);
    }
}

bool ConstantParameter::SetParameterInternal(size_t size, size_t offset, const void* value)
{
    if (value)
    {
        core::MemoryUtilities::Memcpy(m_constantBuffer.data() + offset, size, value, size);
        return true;
    }
    return false;
}

bool ConstantParameter::SetParameterInternal(core::StringView name, const void* value, size_t valueSize)
{
    auto accessor = GetParameterAccessor(name);
    if (accessor && accessor->size == valueSize)
    {
        m_needUpdateBuffer = true;
        return SetParameterInternal(accessor->size, accessor->offset, &value);
    }
    return false;
}

bool ConstantParameter::GetParameterInternal(size_t size, size_t offset, void* value) const
{
    if (value)
    {
        core::MemoryUtilities::Memcpy(value, size, m_constantBuffer.data() + offset, size);
        return true;
    }
    return false;
}

bool ConstantParameter::GetParameterInternal(core::StringView name, void* value, size_t valueSize) const
{
    auto accessor = GetParameterAccessor(name);
    if (accessor && accessor->size == valueSize)
    {
        return GetParameterInternal(accessor->size, accessor->offset, value);
    }
    return false;
}

void ConstantParameter::AddParameterInternal(core::StringView name, ParameterType type, size_t count, size_t size, const void* data)
{
    m_constantBuffer.resize(m_constantBuffer.size() + size);
    AddParameterAcessor(name, type, count, size);
    SetParameterInternal(name, data, size);
    m_needCreateBuffer = true;
}

void ConstantParameter::AddParameterAcessor(core::StringView name, ParameterType type, size_t count, size_t size)
{
    if (GetParameterAccessor(name) != nullptr)
    {
        return;
    }

    size_t offset = 0;
    for (const auto& itr : m_accessors)
    {
        offset += itr.size;
    }

    ParameterAccessor& accessor = m_accessors.emplace_back();
    accessor.name = name;
    accessor.type = type;
    accessor.count = count;
    accessor.size = size;
    accessor.offset = offset;
}

const ParameterAccessor* ConstantParameter::GetParameterAccessor(core::StringView name) const
{
    for (const auto& itr : m_accessors)
    {
        if (itr.name == name)
        {
            return &itr;
        }
    }
    return nullptr;
}

}
