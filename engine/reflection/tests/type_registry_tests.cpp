#include <gtest/gtest.h>
#include <impls.hpp>
#include <type/builder.hpp>

struct TestType
{
    int a = 0;
    bool is_zero() const { return !(bool)(a); }
    void set(int b) { a = b; }
};

TEST(TypeTests, MemberMethod)
{
    auto store = TypeStore {};
    TypeBuilder builder = TypeBuilder<TestType>(store);

    builder.addConstructor<>()
        .addConstructor<int>()
        // .addConstMethod(&TestType::is_zero)
        // .addMethod(&TestType::set)
        // .addField(&TestType::a)
        .addConstant("Constant", 42)
        .addConstant("Constant2", 69);

    auto type = store.get<TestType>();
    EXPECT_EQ(type->getConstant("Constant"), 42);
    EXPECT_EQ(type->getConstant("Constant2"), 69);

    // auto constructor = ConstructorImpl<TestType, int>(store);
    // auto a = Field(store, &TestType::a);
    // auto is_zero = MethodImpl(store, &TestType::is_zero);
    // auto set = MethodImpl(store, &TestType::set);

    // ASSERT_EQ(is_zero.parameters.types.size(), 0);
    // ASSERT_EQ(set.parameters.types.size(), 1);

    // Instance in = Instance(store, 3);
    // auto instance = constructor.invoke(store, { std::vector<Instance*> { &in } });

    // *in.cast<int>() = 0;
    // (void)set.invoke(store, instance, { { &in } });

    // auto ret = is_zero.invoke(store, instance, {});
    // bool* result = ret.cast<bool>();

    // ASSERT_NE(result, nullptr);
    // ASSERT_TRUE(*result);

    // auto* value = a.access<int>(instance);
    // ASSERT_NE(value, nullptr);
    // ASSERT_EQ(*value, 0);
}