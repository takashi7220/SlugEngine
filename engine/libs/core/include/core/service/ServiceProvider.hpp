#pragma once

#include "core/Function.hpp"
#include "core/container/Vector.hpp"
#include "core/container/UnorderedSet.hpp"
#include "core/utility/Hash.hpp"
#include "core/memory/ReferencePtr.hpp"   
#include "core/memory/MemoryUtilities.hpp"
#include "core/uuid/NameID.hpp"
#include "core/container/Queue.hpp"

#define SLUG_SERVICE_STATIC_TYPE_ID(type)                                               \
static slug::core::ServiceTypeID GetStaticServiceTypeID()                               \
{                                                                                       \
    return slug::core::ServiceTypeID(#type);                                            \
}                                                                                       \

#define SLUG_INTERFACE_SERVICE_REGISTER(type)                                           \
SLUG_SERVICE_STATIC_TYPE_ID(type)                                                       \

#define SLUG_SERVICE_CUSTOM_REGISTER(type)                                              \
SLUG_SERVICE_STATIC_TYPE_ID(type)                                                       \

#define SLUG_SERVICE_REGISTER(type)                                                     \
SLUG_SERVICE_STATIC_TYPE_ID(type)                                                       \
static slug::core::IServicePtr Create(slug::core::ServiceProvider& provider)            \
{                                                                                       \
    return slug::core::CreateStaticService<type>(provider);                             \
}                                                                                       \

#define SLUG_SERVICE_REGISTER_DEPENDENTS(type, ...)                                     \
SLUG_SERVICE_STATIC_TYPE_ID(type)                                                       \
static slug::core::IServicePtr Create(slug::core::ServiceProvider& provider)            \
{                                                                                       \
    return slug::core::CreateStaticService<type, __VA_ARGS__>(provider);                \
}                                                                                       \

namespace slug::core
{

using ServiceTypeID = NameID;
using ServiceTypeIDHash = NameIDHash;

class IService : public ReferenceObject
{
public:
    ServiceTypeID GetServiceTypeId()
    {
        return m_serviceTypeId;
    }

    void SetServiceTypeId(ServiceTypeID id)
    {
        m_serviceTypeId = id;
    }

private:
    ServiceTypeID m_serviceTypeId;
};
using IServicePtr = core::TReferencePtr<IService>;

class ServiceProvider
{
public:
    using ServiceFactory = core::TFunctionObject<IServicePtr(ServiceProvider&)>;
    enum class Lifetime
    {
        Singleton,
        Transient
    };

    struct Descriptor
    {
        Lifetime lifetime = Lifetime::Singleton;
        ServiceFactory factory = nullptr;
    };

public:
    void AddSingleton(ServiceTypeID id, ServiceFactory factory)
    {
        if (m_descriptors.find(id) == m_descriptors.end())
        {
            Descriptor& descriptor = m_descriptors[id];
            descriptor.lifetime = Lifetime::Singleton;
            descriptor.factory = factory;
        }
    }

    void AddTransient(ServiceTypeID id, ServiceFactory factory)
    {
        if (m_descriptors.find(id) == m_descriptors.end())
        {
            Descriptor& descriptor = m_descriptors[id];
            descriptor.lifetime = Lifetime::Transient;
            descriptor.factory = factory;
        }
    }

    IServicePtr GetService(ServiceTypeID id)
    {
        if (m_descriptors.find(id) == m_descriptors.end())
        {
            return nullptr;
        }

        Descriptor& descriptor = m_descriptors[id];
        if (descriptor.lifetime == Lifetime::Singleton)
        {
            if (m_singletons.find(id) != m_singletons.end())
            {
                return m_singletons[id];
            }
            else
            {
                m_singletons[id] = CreateService(id, descriptor);
                return m_singletons[id];
            }
        }
        else
        {
            return CreateService(id, descriptor);
        }
    }

    IServicePtr GetRequiredService(ServiceTypeID id)
    {
        IServicePtr service = GetService(id);
        if (service == nullptr)
        {
            SLUG_THROW_EXCEPTION("Error required service is not registered.");
        }
        return service;
    }

    void CleanSingletons()
    {
        auto itr = m_singletons.begin();
        while (itr != m_singletons.end())
        {
            if (itr->second->Control()->strong == 1)
            {
                itr = m_singletons.erase(itr);
            }
            else
            {
                itr++;
            }
        }
    }

    void CleanAllSingletons()
    {
        while (m_singletons.size() != 0)
        {
            CleanSingletons();
        }
    }

    void DeleteDescriptor(ServiceTypeID id)
    {
        auto itr = m_descriptors.find(id);
        if (itr != m_descriptors.end())
        {
            m_descriptors.erase(itr);
        }
    }

public:
    template<typename ServiceType>
    void AddSingleton()
    {
        AddSingleton(ServiceType::GetStaticServiceTypeID(), ServiceType::Create);
    }

    template<typename ServiceType>
    void AddSingleton(ServiceFactory factory)
    {
        AddSingleton(ServiceType::GetStaticServiceTypeID(), factory);
    }

    template<typename ServiceTypeBase, typename ServiceType>
    void AddSingleton()
    {
        AddSingleton(ServiceTypeBase::GetStaticServiceTypeID(), ServiceType::Create);
    }

    template<typename ServiceType>
    void AddTransient()
    {
        AddTransient(ServiceType::GetStaticServiceTypeID(), ServiceType::Create);
    }

    template<typename ServiceType>
    void AddTransient(ServiceFactory factory)
    {
        AddTransient(ServiceType::GetStaticServiceTypeID(), factory);
    }

    template<typename ServiceTypeBase, typename ServiceType>
    void AddTransient()
    {
        AddTransient(ServiceTypeBase::GetStaticServiceTypeID(), ServiceType::Create);
    }

    template<typename ServiceType>
    void DeleteDescriptor()
    {
        DeleteDescriptor(ServiceType::GetStaticServiceTypeID());
    }

    template<typename ServiceType>
    TReferencePtr<ServiceType> GetService()
    {
        return MemoryUtilities::CheckedCast<ServiceType*, IService*>(GetService(ServiceType::GetStaticServiceTypeID()).get());
    }

    template<typename ServiceType>
    TReferencePtr<ServiceType> GetRequiredService()
    {
        return MemoryUtilities::CheckedCast<ServiceType*, IService*>(GetRequiredService(ServiceType::GetStaticServiceTypeID()).get());
    }
private:
    IServicePtr CreateService(ServiceTypeID serviceTypeId, Descriptor& descriptor)
    {
        IServicePtr service = nullptr;
        if (CheckCreatableService(serviceTypeId))
        {
            m_creatingServiceHash.insert(serviceTypeId);
            service = descriptor.factory(*this);
            m_creatingServiceHash.erase(serviceTypeId);
        }
        else
        {
            SLUG_THROW_EXCEPTION("The target service cannot be registered because it has a circular reference.");
        }
        return service;
    }

    bool CheckCreatableService(ServiceTypeID serviceTypeId)
    {
        // 循環参照による無限ループを防ぐため、対象のサービスが生成中かチェック
        return !m_creatingServiceHash.contains(serviceTypeId);
    }

private:
    core::TUnorderedMap<ServiceTypeID, Descriptor, ServiceTypeIDHash> m_descriptors;
    core::TUnorderedMap<ServiceTypeID, IServicePtr, ServiceTypeIDHash> m_singletons;
    core::TUnorderedSet<ServiceTypeID, ServiceTypeIDHash> m_creatingServiceHash;
};

template<typename ServiceType, class... Dependends>
IServicePtr CreateStaticService(ServiceProvider& provider)
{
    IServicePtr service = new ServiceType(provider.GetRequiredService<Dependends>()...);
    service->SetServiceTypeId(ServiceType::GetStaticServiceTypeID());
    return service;
}
}
