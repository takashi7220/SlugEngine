#include <gtest/gtest.h>
#include "core/service/ServiceProvider.hpp"

using namespace slug::core;

namespace
{

class TestServiceA : public IService
{
public:
    SLUG_SERVICE_REGISTER(TestServiceA)

        TestServiceA()
    {
    }

    uint32_t GetValue()
    {
        return 1;
    }
};


class TestServiceB : public IService
{
public:
    SLUG_SERVICE_REGISTER_DEPENDENTS(TestServiceB, TestServiceA)

        TestServiceB(TestServiceA* serviceA)
        : m_serviceA(serviceA)
    {
    }

    uint32_t GetValue()
    {
        return 2 + m_serviceA->GetValue();
    }

    TestServiceA* GetServiceA()
    {
        return m_serviceA;
    }

private:
    TReferencePtr<TestServiceA> m_serviceA;
};

class TestServiceC : public IService
{
public:
    SLUG_SERVICE_REGISTER(TestServiceC)

        TestServiceC()
    {
    }

    uint32_t GetValue()
    {
        return 3;
    }
};

class TestServiceD : public IService
{
public:
    SLUG_SERVICE_REGISTER_DEPENDENTS(TestServiceD, TestServiceA, TestServiceC)

        TestServiceD(TestServiceA* serviceA, TestServiceC* serviceC)
        : m_serviceA(serviceA)
        , m_serviceC(serviceC)
    {
    }

    uint32_t GetValue()
    {
        return 4 + m_serviceA->GetValue() + m_serviceC->GetValue();
    }

private:
    TReferencePtr<TestServiceA> m_serviceA;
    TReferencePtr<TestServiceC> m_serviceC;
};

class TestServiceE : public IService
{
public:
    struct Desc
    {
        uint32_t value = 0;
    };

public:
    SLUG_INTERFACE_SERVICE_REGISTER(TestServiceE)

    TestServiceE(const Desc& desc)
    : m_desc(desc)
    {
    }

    uint32_t GetValue()
    {
        return m_desc.value;
    }

private:
    Desc m_desc;
};

class TestServiceF : public IService
{
public:
    struct Desc
    {
        uint32_t value = 0;
    };

public:
    SLUG_INTERFACE_SERVICE_REGISTER(TestServiceF)

    TestServiceF(const Desc& desc, TestServiceE* serviceE, TestServiceA* serviceA)
    : m_desc(desc)
    , m_serviceE(serviceE)
    , m_serviceA(serviceA)
    {
    }

    uint32_t GetValue()
    {
        return m_desc.value + m_serviceE->GetValue() + m_serviceA->GetValue();
    }

private:
    Desc m_desc;
    TReferencePtr<TestServiceE> m_serviceE;
    TReferencePtr<TestServiceA> m_serviceA;
};

class TestServiceBase : public IService
{
public:
    SLUG_INTERFACE_SERVICE_REGISTER(TestServiceBase)

    TestServiceBase()
    {
    }

    virtual uint32_t GetValue() = 0;
};

class TestServiceG : public TestServiceBase
{
public:
    SLUG_INTERFACE_SERVICE_REGISTER(TestServiceH)

    struct Desc
    {
        uint32_t value = 0;
    };

    TestServiceG(const Desc& desc)
        : m_desc(desc)
    {

    }

    uint32_t GetValue() override
    {
        return m_desc.value;
    }
private:
    Desc m_desc;
};

class TestServiceI;

class TestServiceH : public IService
{
public:
    SLUG_SERVICE_REGISTER_DEPENDENTS(TestServiceH, TestServiceI)
    TestServiceH(TestServiceI* serviceI)
    {
    }
};

class TestServiceI : public IService
{
public:
    SLUG_SERVICE_REGISTER_DEPENDENTS(TestServiceI, TestServiceH)
    TestServiceI(TestServiceH* serviceH)
    {}
};

TEST(ServiceProvider, ServiceProviderRegister)
{
    ServiceProvider provider;
    provider.AddSingleton<TestServiceA>();
    TReferencePtr<TestServiceA> serviceA = provider.GetService<TestServiceA>();
    EXPECT_EQ(serviceA->GetValue(), 1);
}

TEST(ServiceProvider, ServiceProviderDependents)
{
    ServiceProvider provider;
    provider.AddSingleton<TestServiceA>();
    provider.AddSingleton<TestServiceB>();
    provider.AddSingleton<TestServiceC>();
    provider.AddSingleton<TestServiceD>();

    TReferencePtr<TestServiceB> serviceB = provider.GetService<TestServiceB>();
    TReferencePtr<TestServiceD> serviceD = provider.GetService<TestServiceD>();

    EXPECT_EQ(serviceB->GetValue(), 3);
    EXPECT_EQ(serviceD->GetValue(), 8);
}

TEST(ServiceProvider, ServiceProviderCustomFactory)
{
    ServiceProvider provider;
    provider.AddSingleton<TestServiceA>();
    provider.AddSingleton<TestServiceE>([](ServiceProvider& provider)
    {
        TestServiceE::Desc descE;
        descE.value = 10;
        IServicePtr service(new TestServiceE(descE));
        return service;
    });

    provider.AddTransient<TestServiceF>([](ServiceProvider& provider)
    {
        TestServiceF::Desc descF;
        descF.value = 20;
        IServicePtr service(new TestServiceF(descF, provider.GetService<TestServiceE>(), provider.GetService<TestServiceA>()));
        return service;
    });

    TReferencePtr<TestServiceF> serviceF = provider.GetService<TestServiceF>();
    EXPECT_EQ(serviceF->GetValue(), 31);
}

TEST(ServiceProvider, ServiceProviderInterface)
{
    ServiceProvider provider;
    provider.AddSingleton<TestServiceBase>([](ServiceProvider& provider)
    {
        TestServiceG::Desc descG;
        descG.value = 21;
        IServicePtr service(new TestServiceG(descG));
        return service;
    });

    TReferencePtr<TestServiceBase> serviceG = provider.GetService<TestServiceBase>();
    EXPECT_EQ(serviceG->GetValue(), 21);
}

TEST(ServiceProvider, ServiceProviderCircularRefference)
{
    ServiceProvider provider;
    provider.AddSingleton<TestServiceH>();
    provider.AddSingleton<TestServiceI>();

    bool exception = false;
    try
    {
        provider.GetService<TestServiceI>();
    }
    catch (...)
    {
        exception = true;
    }
    EXPECT_TRUE(exception);
}

}
