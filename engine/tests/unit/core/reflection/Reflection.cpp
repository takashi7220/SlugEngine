#include <gtest/gtest.h>
#include "core/reflection/Reflection.hpp"

namespace slug::test::reflection
{

struct Vec2
{
    Vec2() = default;
    Vec2(float _x, float _y) : x(_x), y(_y) {}
    float x = 0.0f;
    float y = 0.0f;
};

class Counter
{
public:
    Counter() = default;

    int m_direct = 0;

    int GetValue() const { return m_value; }
    void SetValue(int v) { m_value = v; }
    void Increment() { ++m_value; }
    int IncrementAndGet() { return ++m_value; }

private:
    int m_value = 0;
};

class Animal
{
public:
    int m_legCount = 4;
};

class Dog : public Animal
{
public:
    int m_tailCount = 1;
};

}

using namespace slug::test::reflection;

SLUG_REGISTER_REFLECTION(int32_t, int32_t)
{
    registry.RegisterType<int32_t>("int32_t");
}

SLUG_REGISTER_REFLECTION(float, float)
{
    registry.RegisterType<float>("float");
}

SLUG_REGISTER_REFLECTION(slug::test::reflection::Vec2, Vec2)
{
    slug::core::Type& t = registry.RegisterType<Vec2>("Vec2");
    slug::core::TypeRegister<Vec2>(t)
        .constructor<float, float>()
        .property("x", &Vec2::x)
        .property("y", &Vec2::y);
}

SLUG_REGISTER_REFLECTION(slug::test::reflection::Counter, Counter)
{
    slug::core::Type& t = registry.RegisterType<Counter>("Counter");
    slug::core::TypeRegister<Counter>(t)
        .constructor<>()
        .property("direct", &Counter::m_direct)
        .property("value", &Counter::GetValue, &Counter::SetValue)
        .property_readonly("valueRo", &Counter::GetValue)
        .method("Increment", &Counter::Increment)
        .method("IncrementAndGet", &Counter::IncrementAndGet)
        .method("GetValue", &Counter::GetValue);
}

SLUG_REGISTER_REFLECTION(slug::test::reflection::Animal, Animal)
{
    slug::core::Type& t = registry.RegisterType<Animal>("Animal");
    slug::core::TypeRegister<Animal>(t)
        .constructor<>()
        .property("legCount", &Animal::m_legCount);
}

SLUG_REGISTER_REFLECTION(slug::test::reflection::Dog, Dog)
{
    slug::core::Type& t = registry.RegisterType<Dog>("Dog");
    slug::core::TypeRegister<Dog>(t)
        .base<Animal>()
        .constructor<>()
        .property("tailCount", &Dog::m_tailCount);
}



namespace
{
using namespace slug::core;
using namespace slug::test::reflection;

class ReflectionTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        provider.AddSingleton<ReflectionService>();
        reflectionService = provider.GetService<ReflectionService>();
    }
private:
    ReflectionServicePtr reflectionService;
    ServiceProvider provider;
};


// ─────────────────────────────────────────────
// Variant
// ─────────────────────────────────────────────

TEST_F(ReflectionTest, Variant_DefaultConstruct_IsInvalid)
{
    Variant v;
    EXPECT_FALSE(v.IsValid());
}

TEST_F(ReflectionTest, Variant_ConstructInt_IsValid)
{
    Variant v(42);
    EXPECT_TRUE(v.IsValid());
}

TEST_F(ReflectionTest, Variant_ConstructInt_IsInt_NotFloat)
{
    Variant v(42);
    EXPECT_TRUE(v.Is<int>());
    EXPECT_FALSE(v.Is<float>());
}

TEST_F(ReflectionTest, Variant_GetInt_ReturnsStoredValue)
{
    Variant v(99);
    ASSERT_TRUE(v.Is<int>());
    EXPECT_EQ(v.Get<int>(), 99);
}

TEST_F(ReflectionTest, Variant_Reset_BecomesInvalid)
{
    Variant v(1);
    ASSERT_TRUE(v.IsValid());
    v.Reset();
    EXPECT_FALSE(v.IsValid());
}

TEST_F(ReflectionTest, Variant_Copy_PreservesValue)
{
    Variant a(7);
    Variant b = a;
    ASSERT_TRUE(b.Is<int>());
    EXPECT_EQ(b.Get<int>(), 7);
}

TEST_F(ReflectionTest, Variant_Move_LeavesSourceInvalid)
{
    Variant a(5);
    Variant b = std::move(a);
    EXPECT_FALSE(a.IsValid());
    ASSERT_TRUE(b.Is<int>());
    EXPECT_EQ(b.Get<int>(), 5);
}

// ─────────────────────────────────────────────
// Instance
// ─────────────────────────────────────────────

TEST_F(ReflectionTest, Instance_DefaultConstruct_IsInvalid)
{
    Instance inst;
    EXPECT_FALSE(inst.IsValid());
}

TEST_F(ReflectionTest, Instance_MutableObject_IsNotConst)
{
    Counter c;
    Instance inst(c);
    EXPECT_TRUE(inst.IsValid());
    EXPECT_FALSE(inst.IsConst());
}

TEST_F(ReflectionTest, Instance_ConstObject_IsConst)
{
    const Counter c;
    Instance inst(c);
    EXPECT_TRUE(inst.IsValid());
    EXPECT_TRUE(inst.IsConst());
}

TEST_F(ReflectionTest, Instance_TryAs_CorrectType_ReturnsPtr)
{
    Counter c;
    c.m_direct = 3;
    Instance inst(c);
    Counter* ptr = inst.TryAs<Counter>();
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(ptr->m_direct, 3);
}

TEST_F(ReflectionTest, Instance_TryAs_WrongType_ReturnsNull)
{
    Counter c;
    Instance inst(c);
    EXPECT_EQ(inst.TryAs<Animal>(), nullptr);
}

TEST_F(ReflectionTest, Instance_MutableTryAs_OnConstInstance_ReturnsNull)
{
    const Counter c;
    Instance inst(c);
    EXPECT_EQ(inst.TryAs<Counter>(), nullptr);
}

TEST_F(ReflectionTest, Instance_ConstTryAs_OnConstInstance_ReturnsPtr)
{
    const Counter c;
    Instance inst(c);
    const Instance& cinst = inst;
    const Counter* ptr = cinst.TryAs<Counter>();
    EXPECT_NE(ptr, nullptr);
}

TEST_F(ReflectionTest, Instance_FromMutable_Factory)
{
    Counter c;
    c.m_direct = 9;
    TypeId id = TypeOf<Counter>().GetId();
    Instance inst = Instance::FromMutable(&c, id);
    EXPECT_FALSE(inst.IsConst());
    Counter* ptr = inst.TryAs<Counter>();
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(ptr->m_direct, 9);
}

TEST_F(ReflectionTest, Instance_FromConst_Factory)
{
    const Counter c;
    TypeId id = TypeOf<Counter>().GetId();
    Instance inst = Instance::FromConst(&c, id);
    EXPECT_TRUE(inst.IsConst());

    EXPECT_EQ(inst.TryAs<Counter>(), nullptr);

    const Instance& cinst = inst;
    EXPECT_NE(cinst.TryAs<Counter>(), nullptr);
}

// ─────────────────────────────────────────────
// ReflectionRegistry
// ─────────────────────────────────────────────

TEST_F(ReflectionTest, Registry_FindUnregisteredType_ReturnsNull)
{
    const Type* t = ReflectionService::GetStaticRegistry()->FindType("NonExistentType12345");
    EXPECT_EQ(t, nullptr);
}

TEST_F(ReflectionTest, Registry_FindRegisteredType_ByName)
{
    const Type* t = ReflectionService::GetStaticRegistry()->FindType("Counter");
    ASSERT_NE(t, nullptr);
    EXPECT_EQ(t->GetName(), "Counter");
}

TEST_F(ReflectionTest, Registry_FindRegisteredType_ById)
{
    TypeId id = TypeOf<Counter>().GetId();
    const Type* t = ReflectionService::GetStaticRegistry()->FindType(id);
    ASSERT_NE(t, nullptr);
    EXPECT_EQ(t->GetId(), id);
}

TEST_F(ReflectionTest, Registry_RegisterSameName_ReturnsSameType)
{
    auto reg = ReflectionService::GetStaticRegistry();
    Type& a = reg->RegisterType<int>("int");
    Type& b = reg->RegisterType<int>("int");
    EXPECT_EQ(&a, &b);
}

// ─────────────────────────────────────────────
// TypeOf / TypeResolver
// ─────────────────────────────────────────────

TEST_F(ReflectionTest, TypeOf_MatchesRegisteredTypeId)
{
    const Type* t = ReflectionService::GetStaticRegistry()->FindType("Counter");
    ASSERT_NE(t, nullptr);
    EXPECT_EQ(TypeOf<Counter>().GetId(), t->GetId());
}

TEST_F(ReflectionTest, TypeOf_DifferentTypes_DifferentIds)
{
    TypeId intId     = TypeOf<int>().GetId();
    TypeId floatId   = TypeOf<float>().GetId();
    TypeId counterId = TypeOf<Counter>().GetId();
    EXPECT_NE(intId, floatId);
    EXPECT_NE(intId, counterId);
    EXPECT_NE(floatId, counterId);
}

// ─────────────────────────────────────────────
// Type メタデータ
// ─────────────────────────────────────────────

TEST_F(ReflectionTest, Type_GetName)
{
    EXPECT_EQ(TypeOf<Counter>().GetName(), "Counter");
}

TEST_F(ReflectionTest, Type_FindProperty_Existing)
{
    const Type* t = ReflectionService::GetStaticRegistry()->FindType("Counter");
    ASSERT_NE(t, nullptr);
    auto prop = t->FindProperty("direct");
    EXPECT_NE(prop, nullptr);
}

TEST_F(ReflectionTest, Type_FindProperty_NonExisting_ReturnsNull)
{
    const Type* t = ReflectionService::GetStaticRegistry()->FindType("Counter");
    ASSERT_NE(t, nullptr);
    auto prop = t->FindProperty("nonExistentProp");
    EXPECT_EQ(prop, nullptr);
}

TEST_F(ReflectionTest, Type_FindMethod_Existing)
{
    const Type* t = ReflectionService::GetStaticRegistry()->FindType("Counter");
    ASSERT_NE(t, nullptr);
    auto method = t->FindMethod("Increment");
    EXPECT_NE(method, nullptr);
}

TEST_F(ReflectionTest, Type_FindMethod_NonExisting_ReturnsNull)
{
    const Type* t = ReflectionService::GetStaticRegistry()->FindType("Counter");
    ASSERT_NE(t, nullptr);
    auto method = t->FindMethod("NonExistentMethod");
    EXPECT_EQ(method, nullptr);
}

TEST_F(ReflectionTest, Type_GetProperties_ContainsRegistered)
{
    const Type* t = ReflectionService::GetStaticRegistry()->FindType("Counter");
    ASSERT_NE(t, nullptr);
    const auto& props = t->GetProperties();
    EXPECT_GE(props.size(), 3u); // direct, value, valueRo
}

TEST_F(ReflectionTest, Type_GetMethods_ContainsRegistered)
{
    const Type* t = ReflectionService::GetStaticRegistry()->FindType("Counter");
    ASSERT_NE(t, nullptr);
    const auto& methods = t->GetMethods();
    EXPECT_GE(methods.size(), 3u); // Increment, IncrementAndGet, GetValue
}

TEST_F(ReflectionTest, Type_GetConstructors_ContainsRegistered)
{
    const Type* t = ReflectionService::GetStaticRegistry()->FindType("Counter");
    ASSERT_NE(t, nullptr);
    EXPECT_GE(t->GetConstructors().size(), 1u);
}

// ─────────────────────────────────────────────
// Property — メンバポインタ方式
// ─────────────────────────────────────────────

TEST_F(ReflectionTest, Property_MemberPtr_GetValue)
{
    const Type* t = ReflectionService::GetStaticRegistry()->FindType("Counter");
    auto prop = t->FindProperty("direct");
    ASSERT_NE(prop, nullptr);

    Counter c;
    c.m_direct = 42;
    Instance inst(c);

    Variant val = prop->GetValue(inst);
    ASSERT_TRUE(val.Is<int>());
    EXPECT_EQ(val.Get<int>(), 42);
}

TEST_F(ReflectionTest, Property_MemberPtr_SetValue)
{
    const Type* t = ReflectionService::GetStaticRegistry()->FindType("Counter");
    auto prop = t->FindProperty("direct");
    ASSERT_NE(prop, nullptr);

    Counter c;
    Instance inst(c);

    bool ok = prop->SetValue(inst, Variant(77));
    EXPECT_TRUE(ok);
    EXPECT_EQ(c.m_direct, 77);
}

TEST_F(ReflectionTest, Property_MemberPtr_SetValue_OnConstInstance_ReturnsFalse)
{
    const Type* t = ReflectionService::GetStaticRegistry()->FindType("Counter");
    auto prop = t->FindProperty("direct");
    ASSERT_NE(prop, nullptr);

    const Counter c;
    Instance inst(c);

    bool ok = prop->SetValue(inst, Variant(1));
    EXPECT_FALSE(ok);
}

// ─────────────────────────────────────────────
// Property — getter / setter 方式
// ─────────────────────────────────────────────

TEST_F(ReflectionTest, Property_GetterSetter_GetValue)
{
    const Type* t = ReflectionService::GetStaticRegistry()->FindType("Counter");
    auto prop = t->FindProperty("value");
    ASSERT_NE(prop, nullptr);

    Counter c;
    c.SetValue(55);
    Instance inst(c);

    Variant val = prop->GetValue(inst);
    ASSERT_TRUE(val.Is<int>());
    EXPECT_EQ(val.Get<int>(), 55);
}

TEST_F(ReflectionTest, Property_GetterSetter_SetValue)
{
    const Type* t = ReflectionService::GetStaticRegistry()->FindType("Counter");
    auto prop = t->FindProperty("value");
    ASSERT_NE(prop, nullptr);

    Counter c;
    Instance inst(c);

    bool ok = prop->SetValue(inst, Variant(33));
    EXPECT_TRUE(ok);
    EXPECT_EQ(c.GetValue(), 33);
}

TEST_F(ReflectionTest, Property_GetterSetter_SetValue_OnConstInstance_ReturnsFalse)
{
    const Type* t = ReflectionService::GetStaticRegistry()->FindType("Counter");
    auto prop = t->FindProperty("value");
    ASSERT_NE(prop, nullptr);

    const Counter c;
    Instance inst(c);

    bool ok = prop->SetValue(inst, Variant(1));
    EXPECT_FALSE(ok);
}

// ─────────────────────────────────────────────
// Property — 読み取り専用
// ─────────────────────────────────────────────

TEST_F(ReflectionTest, Property_Readonly_GetValue)
{
    const Type* t = ReflectionService::GetStaticRegistry()->FindType("Counter");
    auto prop = t->FindProperty("valueRo");
    ASSERT_NE(prop, nullptr);

    Counter c;
    c.SetValue(11);
    Instance inst(c);

    Variant val = prop->GetValue(inst);
    ASSERT_TRUE(val.Is<int>());
    EXPECT_EQ(val.Get<int>(), 11);
}

TEST_F(ReflectionTest, Property_Readonly_SetValue_ReturnsFalse)
{
    const Type* t = ReflectionService::GetStaticRegistry()->FindType("Counter");
    auto prop = t->FindProperty("valueRo");
    ASSERT_NE(prop, nullptr);

    Counter c;
    Instance inst(c);

    EXPECT_FALSE(prop->IsWritable());
    bool ok = prop->SetValue(inst, Variant(99));
    EXPECT_FALSE(ok);
}

// ─────────────────────────────────────────────
// Method
// ─────────────────────────────────────────────

TEST_F(ReflectionTest, Method_VoidMethod_SideEffectApplied)
{
    const Type* t = ReflectionService::GetStaticRegistry()->FindType("Counter");
    auto prop   = t->FindProperty("value");
    auto method = t->FindMethod("Increment");
    ASSERT_NE(prop, nullptr);
    ASSERT_NE(method, nullptr);

    Counter c;
    Instance inst(c);

    prop->SetValue(inst, Variant(0));
    method->Invoke(inst, {});
    method->Invoke(inst, {});

    Variant val = prop->GetValue(inst);
    ASSERT_TRUE(val.Is<int>());
    EXPECT_EQ(val.Get<int>(), 2);
}

TEST_F(ReflectionTest, Method_ReturnValue_ReturnsCorrectValue)
{
    const Type* t = ReflectionService::GetStaticRegistry()->FindType("Counter");
    auto prop   = t->FindProperty("value");
    auto method = t->FindMethod("IncrementAndGet");
    ASSERT_NE(prop, nullptr);
    ASSERT_NE(method, nullptr);

    Counter c;
    Instance inst(c);
    prop->SetValue(inst, Variant(9));

    Variant result = method->Invoke(inst, {});
    ASSERT_TRUE(result.Is<int>());
    EXPECT_EQ(result.Get<int>(), 10);
}

TEST_F(ReflectionTest, Method_ConstMethod_OnConstInstance_Succeeds)
{
    const Type* t = ReflectionService::GetStaticRegistry()->FindType("Counter");
    auto prop   = t->FindProperty("value");
    auto method = t->FindMethod("GetValue");
    ASSERT_NE(method, nullptr);
    EXPECT_TRUE(method->IsConst());

    Counter c;
    Instance minst(c);
    prop->SetValue(minst, Variant(7));

    const Counter& cc = c;
    Instance cinst(cc);

    Variant result = method->Invoke(cinst, {});
    ASSERT_TRUE(result.Is<int>());
    EXPECT_EQ(result.Get<int>(), 7);
}

TEST_F(ReflectionTest, Method_NonConstMethod_OnConstInstance_ReturnsInvalid)
{
    const Type* t  = ReflectionService::GetStaticRegistry()->FindType("Counter");
    auto method    = t->FindMethod("IncrementAndGet");
    ASSERT_NE(method, nullptr);
    EXPECT_FALSE(method->IsConst());

    const Counter c;
    Instance cinst(c);

    Variant result = method->Invoke(cinst, {});
    EXPECT_FALSE(result.IsValid());
}

TEST_F(ReflectionTest, Method_WrongArgCount_ReturnsInvalid)
{
    const Type* t = ReflectionService::GetStaticRegistry()->FindType("Counter");
    auto method   = t->FindMethod("GetValue");
    ASSERT_NE(method, nullptr);

    Counter c;
    Instance inst(c);

    slug::core::TVector<Variant> extraArgs = { Variant(1), Variant(2) };
    Variant result = method->Invoke(inst, extraArgs);
    EXPECT_FALSE(result.IsValid());
}

// ─────────────────────────────────────────────
// Constructor
// ─────────────────────────────────────────────

TEST_F(ReflectionTest, Constructor_NoArgs_CreatesObject)
{
    const Type* t = ReflectionService::GetStaticRegistry()->FindType("Counter");
    ASSERT_NE(t, nullptr);
    ASSERT_FALSE(t->GetConstructors().empty());

    const auto& ctor = t->GetConstructors()[0];
    Variant result = ctor->Invoke({});

    ASSERT_TRUE(result.Is<Counter>());
    EXPECT_EQ(result.Get<Counter>().GetValue(), 0);
}

TEST_F(ReflectionTest, Constructor_WithArgs_CreatesObjectWithValues)
{
    const Type* t = ReflectionService::GetStaticRegistry()->FindType("Vec2");
    ASSERT_NE(t, nullptr);
    ASSERT_FALSE(t->GetConstructors().empty());

    const auto& ctor = t->GetConstructors()[0];
    slug::core::TVector<Variant> args = { Variant(3.0f), Variant(4.0f) };
    Variant result = ctor->Invoke(args);

    ASSERT_TRUE(result.Is<Vec2>());
    const Vec2& v = result.Get<Vec2>();
    EXPECT_FLOAT_EQ(v.x, 3.0f);
    EXPECT_FLOAT_EQ(v.y, 4.0f);
}

TEST_F(ReflectionTest, Constructor_WrongArgCount_ReturnsInvalid)
{
    const Type* t = ReflectionService::GetStaticRegistry()->FindType("Vec2");
    ASSERT_NE(t, nullptr);
    const auto& ctor = t->GetConstructors()[0];

    slug::core::TVector<Variant> wrongArgs = { Variant(1.0f) };
    Variant result = ctor->Invoke(wrongArgs);
    EXPECT_FALSE(result.IsValid());
}

// ─────────────────────────────────────────────
// 継承
// ─────────────────────────────────────────────

TEST_F(ReflectionTest, Inheritance_BaseType_IsSet)
{
    const Type* dogType    = ReflectionService::GetStaticRegistry()->FindType("Dog");
    const Type* animalType = ReflectionService::GetStaticRegistry()->FindType("Animal");
    ASSERT_NE(dogType, nullptr);
    ASSERT_NE(animalType, nullptr);
    EXPECT_EQ(dogType->GetBaseType(), animalType);
}

TEST_F(ReflectionTest, Inheritance_IsDerivedFrom_Self)
{
    const Type* t = ReflectionService::GetStaticRegistry()->FindType("Dog");
    ASSERT_NE(t, nullptr);
    EXPECT_TRUE(t->IsDerivedFrom(t));
}

TEST_F(ReflectionTest, Inheritance_IsDerivedFrom_Parent)
{
    const Type* dogType    = ReflectionService::GetStaticRegistry()->FindType("Dog");
    const Type* animalType = ReflectionService::GetStaticRegistry()->FindType("Animal");
    ASSERT_NE(dogType, nullptr);
    ASSERT_NE(animalType, nullptr);
    EXPECT_TRUE(dogType->IsDerivedFrom(animalType));
}

TEST_F(ReflectionTest, Inheritance_IsDerivedFrom_Unrelated_ReturnsFalse)
{
    const Type* dogType     = ReflectionService::GetStaticRegistry()->FindType("Dog");
    const Type* counterType = ReflectionService::GetStaticRegistry()->FindType("Counter");
    ASSERT_NE(dogType, nullptr);
    ASSERT_NE(counterType, nullptr);
    EXPECT_FALSE(dogType->IsDerivedFrom(counterType));
}

TEST_F(ReflectionTest, Inheritance_IsDerivedFrom_Null_ReturnsFalse)
{
    const Type* t = ReflectionService::GetStaticRegistry()->FindType("Dog");
    ASSERT_NE(t, nullptr);
    EXPECT_FALSE(t->IsDerivedFrom(nullptr));
}

TEST_F(ReflectionTest, Inheritance_Parent_IsNotDerivedFrom_Child)
{
    const Type* dogType    = ReflectionService::GetStaticRegistry()->FindType("Dog");
    const Type* animalType = ReflectionService::GetStaticRegistry()->FindType("Animal");
    ASSERT_NE(dogType, nullptr);
    ASSERT_NE(animalType, nullptr);
    EXPECT_FALSE(animalType->IsDerivedFrom(dogType));
}

TEST_F(ReflectionTest, Inheritance_DogProperty_AccessibleOnDogInstance)
{
    const Type* dogType = ReflectionService::GetStaticRegistry()->FindType("Dog");
    ASSERT_NE(dogType, nullptr);

    auto prop = dogType->FindProperty("tailCount");
    ASSERT_NE(prop, nullptr);

    Dog d;
    d.m_tailCount = 3;
    Instance inst(d);

    Variant val = prop->GetValue(inst);
    ASSERT_TRUE(val.Is<int>());
    EXPECT_EQ(val.Get<int>(), 3);
}
}
