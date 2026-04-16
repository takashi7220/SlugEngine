#pragma once

#include "math/Vector2.hpp"
#include "math/Vector3.hpp"
#include "math/Vector4.hpp"
#include "math/Matrix3x3.hpp"
#include "math/Matrix4x4.hpp"
#include "core/container/Vector.hpp"
#include "core/container/String.hpp"

#include "core/object/SObject.hpp"
#include "shader/Define.h"

namespace slug::render
{
enum class ParameterType : uint8_t
{
    Bool = shader::ParameterType::Bool,
    Float = shader::ParameterType::Float,
    Float2 = shader::ParameterType::Float2,
    Float3 = shader::ParameterType::Float3,
    Float4 = shader::ParameterType::Float4,
    Float3x3 = shader::ParameterType::Float3x3,
    Float4x4 = shader::ParameterType::Float4x4,
    FloatArray = shader::ParameterType::FloatArray,
};

struct ParameterAccessor
{
    core::String name = {};
    ParameterType type = ParameterType::Float;
    size_t count = 1;
    size_t offset = 0;
    size_t size = 0;
};

class ConstantParameter
{
public:
    virtual bool SetParameter(core::StringView name, bool values);
    virtual bool SetParameter(core::StringView name, float value);
    virtual bool SetParameter(core::StringView name, const float* values, size_t count);
    virtual bool SetParameter(core::StringView name, const math::Vector2& value);
    virtual bool SetParameter(core::StringView name, const math::Vector3& value);
    virtual bool SetParameter(core::StringView name, const math::Vector4& value);
    virtual bool SetParameter(core::StringView name, const math::Matrix3x3& value);
    virtual bool SetParameter(core::StringView name, const math::Matrix4x4& value);

    virtual void AddParameter(core::StringView name, bool value);
    virtual void AddParameter(core::StringView name, float value);
    virtual void AddParameter(core::StringView name, const float* values, size_t count);
    virtual void AddParameter(core::StringView name, const math::Vector2& value);
    virtual void AddParameter(core::StringView name, const math::Vector3& value);
    virtual void AddParameter(core::StringView name, const math::Vector4& value);
    virtual void AddParameter(core::StringView name, const math::Matrix3x3& value);
    virtual void AddParameter(core::StringView name, const math::Matrix4x4& value);
    virtual void AddParameter(core::StringView name, ParameterType type, size_t count, size_t size, const void* data);

    virtual bool GetParameter(core::StringView name, bool& value) const;
    virtual bool GetParameter(core::StringView name, float& value) const;
    virtual bool GetParameter(core::StringView name, core::TVector<float>& values) const;
    virtual bool GetParameter(core::StringView name, math::Vector2& value) const;
    virtual bool GetParameter(core::StringView name, math::Vector3& value) const;
    virtual bool GetParameter(core::StringView name, math::Vector4& value) const;
    virtual bool GetParameter(core::StringView name, math::Matrix3x3& value) const;
    virtual bool GetParameter(core::StringView name, math::Matrix4x4& value) const;

    virtual bool HasParameter(core::StringView name) const;

    const core::TVector<uint8_t>& GetConstantBufferPtr() const;
    size_t GetCostantBufferSize() const;

    const core::TVector<ParameterAccessor>& GetAccessors() const;
    void GetParameterNames(core::TVector<core::StringView>& names) const;
protected:
    bool SetParameterInternal(size_t size, size_t offset, const void* value);
    bool SetParameterInternal(core::StringView name, const void* value, size_t valueSize);

    bool GetParameterInternal(size_t size, size_t offset, void* value) const;
    bool GetParameterInternal(core::StringView name, void* value, size_t valueSize) const;

    void AddParameterInternal(core::StringView name, ParameterType type, size_t count, size_t size, const void* data);

    void AddParameterAcessor(core::StringView name, ParameterType type, size_t count, size_t size);
    const ParameterAccessor* GetParameterAccessor(core::StringView name) const;

protected:
    core::TVector<ParameterAccessor> m_accessors = {};
    core::TVector<uint8_t> m_constantBuffer = {};
    bool m_needCreateBuffer = false;
    bool m_needUpdateBuffer = false;

};

}
