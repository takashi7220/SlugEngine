#include "core/object/SObject.hpp"

#include <stdexcept>
#include <gtest/gtest.h>

using namespace slug::core;

namespace
{

class TestObject : public SObject
{
public:
    SLUG_DECLARE_SOBJECT_TYPEINFO(TestObject, SObject)

    TestObject() noexcept = default;
    TestObject(const TestObject&) = delete;
    TestObject(TestObject&&) = delete;
    TestObject& operator=(const TestObject&) = delete;
    TestObject& operator=(TestObject&&) = delete;
    ~TestObject() noexcept override = default;
};

class DerivedObject final : public TestObject
{
public:
    SLUG_DECLARE_SOBJECT_TYPEINFO(DerivedObject, TestObject)

    DerivedObject() noexcept = default;
    DerivedObject(const DerivedObject&) = delete;
    DerivedObject(DerivedObject&&) = delete;
    DerivedObject& operator=(const DerivedObject&) = delete;
    DerivedObject& operator=(DerivedObject&&) = delete;
    ~DerivedObject() noexcept override = default;
};

class OtherObject final : public SObject
{
public:
    SLUG_DECLARE_SOBJECT_TYPEINFO(OtherObject, SObject)

    OtherObject() noexcept = default;
    OtherObject(const OtherObject&) = delete;
    OtherObject(OtherObject&&) = delete;
    OtherObject& operator=(const OtherObject&) = delete;
    OtherObject& operator=(OtherObject&&) = delete;
    ~OtherObject() noexcept override = default;
};

TEST(SObjectTest, StaticTypeIDsAreStableAndMatchTheirTypes)
{
    const SObjectTypeID baseID = SObject::GetStaticTypeID();
    const SObjectTypeID testID = TestObject::GetStaticTypeID();
    const SObjectTypeID derivedID = DerivedObject::GetStaticTypeID();
    const SObjectTypeID otherID = OtherObject::GetStaticTypeID();

    EXPECT_EQ(baseID, SObject::GetStaticTypeID());
    EXPECT_EQ(testID, TestObject::GetStaticTypeID());
    EXPECT_EQ(derivedID, DerivedObject::GetStaticTypeID());
    EXPECT_EQ(otherID, OtherObject::GetStaticTypeID());
    EXPECT_EQ(baseID, GetSObjectTypeID<SObject>());
    EXPECT_EQ(testID, GetSObjectTypeID<TestObject>());
    EXPECT_EQ(derivedID, GetSObjectTypeID<DerivedObject>());
    EXPECT_EQ(otherID, GetSObjectTypeID<OtherObject>());
}

TEST(SObjectTest, StaticTypeIDsAreDistinctAcrossTheHierarchy)
{
    EXPECT_NE(SObject::GetStaticTypeID(), TestObject::GetStaticTypeID());
    EXPECT_NE(SObject::GetStaticTypeID(), DerivedObject::GetStaticTypeID());
    EXPECT_NE(SObject::GetStaticTypeID(), OtherObject::GetStaticTypeID());
    EXPECT_NE(TestObject::GetStaticTypeID(), DerivedObject::GetStaticTypeID());
    EXPECT_NE(TestObject::GetStaticTypeID(), OtherObject::GetStaticTypeID());
    EXPECT_NE(DerivedObject::GetStaticTypeID(), OtherObject::GetStaticTypeID());
}

TEST(SObjectTest, BaseObjectReportsItsOwnDynamicType)
{
    const SObject object;

    EXPECT_EQ(object.GetDynamicTypeID(), SObject::GetStaticTypeID());
}

TEST(SObjectTest, DynamicTypeIDUsesTheMostDerivedTypeThroughBaseReferences)
{
    const TestObject object;
    const DerivedObject derived;
    const OtherObject other;
    const SObject& base = object;
    const SObject& derivedBase = derived;
    const TestObject& intermediateBase = derived;
    const SObject& otherBase = other;

    EXPECT_EQ(base.GetDynamicTypeID(), TestObject::GetStaticTypeID());
    EXPECT_EQ(derived.GetDynamicTypeID(), DerivedObject::GetStaticTypeID());
    EXPECT_EQ(derivedBase.GetDynamicTypeID(), DerivedObject::GetStaticTypeID());
    EXPECT_EQ(intermediateBase.GetDynamicTypeID(), DerivedObject::GetStaticTypeID());
    EXPECT_EQ(otherBase.GetDynamicTypeID(), OtherObject::GetStaticTypeID());
}

TEST(SObjectTest, BaseIsAMatchesOnlyItsOwnType)
{
    const SObject object;

    EXPECT_TRUE(object.IsA(SObject::GetStaticTypeID()));
    EXPECT_FALSE(object.IsA(TestObject::GetStaticTypeID()));
    EXPECT_FALSE(object.IsA(DerivedObject::GetStaticTypeID()));
    EXPECT_FALSE(object.IsA(OtherObject::GetStaticTypeID()));
}

TEST(SObjectTest, DerivedIsAMatchesOnlyItsExactDynamicType)
{
    const DerivedObject object;
    const SObject& base = object;
    const TestObject& intermediateBase = object;

    EXPECT_TRUE(object.IsA(DerivedObject::GetStaticTypeID()));
    EXPECT_FALSE(object.IsA(TestObject::GetStaticTypeID()));
    EXPECT_FALSE(object.IsA(SObject::GetStaticTypeID()));
    EXPECT_FALSE(object.IsA(OtherObject::GetStaticTypeID()));
    EXPECT_TRUE(base.IsA(DerivedObject::GetStaticTypeID()));
    EXPECT_FALSE(base.IsA(SObject::GetStaticTypeID()));
    EXPECT_TRUE(intermediateBase.IsA(DerivedObject::GetStaticTypeID()));
    EXPECT_FALSE(intermediateBase.IsA(TestObject::GetStaticTypeID()));
}

TEST(SObjectTest, IsARejectsDescendantsAndOtherBranches)
{
    const TestObject object;
    const OtherObject other;

    EXPECT_TRUE(object.IsA(TestObject::GetStaticTypeID()));
    EXPECT_FALSE(object.IsA(SObject::GetStaticTypeID()));
    EXPECT_FALSE(object.IsA(DerivedObject::GetStaticTypeID()));
    EXPECT_FALSE(object.IsA(OtherObject::GetStaticTypeID()));
    EXPECT_TRUE(other.IsA(OtherObject::GetStaticTypeID()));
    EXPECT_FALSE(other.IsA(SObject::GetStaticTypeID()));
    EXPECT_FALSE(other.IsA(TestObject::GetStaticTypeID()));
    EXPECT_FALSE(other.IsA(DerivedObject::GetStaticTypeID()));
}

TEST(SObjectTest, TemplatedIsAMatchesTheDynamicTypeThroughTheBaseInterface)
{
    const SObject object;
    const DerivedObject derived;
    const SObject& base = derived;

    EXPECT_TRUE(object.IsA<SObject>());
    EXPECT_FALSE(object.IsA<TestObject>());
    EXPECT_TRUE(base.IsA<DerivedObject>());
    EXPECT_FALSE(base.IsA<TestObject>());
    EXPECT_FALSE(base.IsA<SObject>());
    EXPECT_FALSE(base.IsA<OtherObject>());
}

TEST(SObjectTest, BaseObjectHasOnlyItsOwnAncestorType)
{
    const SObject object;

    EXPECT_TRUE(object.IsAncestorType(SObject::GetStaticTypeID()));
    EXPECT_FALSE(object.IsAncestorType(TestObject::GetStaticTypeID()));
    EXPECT_FALSE(object.IsAncestorType(DerivedObject::GetStaticTypeID()));
    EXPECT_FALSE(object.IsAncestorType(OtherObject::GetStaticTypeID()));
}

TEST(SObjectTest, AncestorTypeIncludesSelfAndEveryBaseType)
{
    const DerivedObject object;
    const SObject& base = object;
    const TestObject& intermediateBase = object;

    EXPECT_TRUE(object.IsAncestorType(DerivedObject::GetStaticTypeID()));
    EXPECT_TRUE(object.IsAncestorType(TestObject::GetStaticTypeID()));
    EXPECT_TRUE(object.IsAncestorType(SObject::GetStaticTypeID()));
    EXPECT_TRUE(base.IsAncestorType(DerivedObject::GetStaticTypeID()));
    EXPECT_TRUE(base.IsAncestorType(TestObject::GetStaticTypeID()));
    EXPECT_TRUE(base.IsAncestorType(SObject::GetStaticTypeID()));
    EXPECT_TRUE(intermediateBase.IsAncestorType(DerivedObject::GetStaticTypeID()));
    EXPECT_FALSE(base.IsAncestorType(OtherObject::GetStaticTypeID()));
}

TEST(SObjectTest, AncestorTypeRejectsDescendantsAndOtherBranches)
{
    const TestObject object;
    const OtherObject other;

    EXPECT_TRUE(object.IsAncestorType(TestObject::GetStaticTypeID()));
    EXPECT_TRUE(object.IsAncestorType(SObject::GetStaticTypeID()));
    EXPECT_FALSE(object.IsAncestorType(DerivedObject::GetStaticTypeID()));
    EXPECT_FALSE(object.IsAncestorType(OtherObject::GetStaticTypeID()));
    EXPECT_TRUE(other.IsAncestorType(OtherObject::GetStaticTypeID()));
    EXPECT_TRUE(other.IsAncestorType(SObject::GetStaticTypeID()));
    EXPECT_FALSE(other.IsAncestorType(TestObject::GetStaticTypeID()));
    EXPECT_FALSE(other.IsAncestorType(DerivedObject::GetStaticTypeID()));
}

TEST(SObjectUtilityTest, CheckedCastDowncastsThroughEveryBaseTypeWithoutAddingReferences)
{
    const auto object = MakeReference<DerivedObject>();
    SObject* base = object.get();
    TestObject* intermediateBase = object.get();
    const unsigned long referenceCount = object->GetReferenceCount();

    EXPECT_EQ(SObjectUtility::CheckedCast<DerivedObject>(base), object.get());
    EXPECT_EQ(SObjectUtility::CheckedCast<TestObject>(base), intermediateBase);
    EXPECT_EQ(SObjectUtility::CheckedCast<DerivedObject>(intermediateBase), object.get());
    EXPECT_EQ(object->GetReferenceCount(), referenceCount);
}

TEST(SObjectUtilityTest, CheckedCastUpcastsToEveryAncestorType)
{
    DerivedObject object;
    TestObject* intermediateBase = &object;
    SObject* base = &object;

    EXPECT_EQ(SObjectUtility::CheckedCast<TestObject>(&object), intermediateBase);
    EXPECT_EQ(SObjectUtility::CheckedCast<SObject>(&object), base);
    EXPECT_EQ(SObjectUtility::CheckedCast<SObject>(intermediateBase), base);
}

TEST(SObjectUtilityTest, CheckedCastPreservesConstness)
{
    const DerivedObject object;
    const SObject* base = &object;
    const TestObject* intermediateBase = &object;

    EXPECT_EQ(SObjectUtility::CheckedCast<const DerivedObject>(base), &object);
    EXPECT_EQ(SObjectUtility::CheckedCast<const TestObject>(base), intermediateBase);
    EXPECT_EQ(SObjectUtility::CheckedCast<const SObject>(&object), base);
}

#if SLUG_DEBUG_MODE

TEST(SObjectUtilityTest, CheckedCastRejectsDowncastingABaseObjectInDebugMode)
{
    SObject object;

    EXPECT_THROW(SObjectUtility::CheckedCast<TestObject>(&object), std::runtime_error);
}

TEST(SObjectUtilityTest, CheckedCastRejectsADerivedTypeThatTheObjectDoesNotHaveInDebugMode)
{
    TestObject object;
    SObject* base = &object;

    EXPECT_THROW(SObjectUtility::CheckedCast<DerivedObject>(base), std::runtime_error);
}

TEST(SObjectUtilityTest, CheckedCastRejectsUnrelatedBranchesInDebugMode)
{
    DerivedObject object;
    OtherObject other;
    SObject* base = &object;
    SObject* otherBase = &other;

    EXPECT_THROW(SObjectUtility::CheckedCast<OtherObject>(base), std::runtime_error);
    EXPECT_THROW(SObjectUtility::CheckedCast<TestObject>(otherBase), std::runtime_error);
}

#endif

}
