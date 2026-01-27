#include <gtest/gtest.h>
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
        .addConstMethod(&TestType::is_zero)
        .addMethod(&TestType::set)
        .addField(&TestType::a)
        .addConstant("Constant", 42)
        .addConstant("Constant2", 69);

    auto* type = store.get<TestType>();
    EXPECT_EQ(type->getConstant("Constant"), 42);
    EXPECT_EQ(type->getConstant("Constant2"), 69);

    Instance params = store.makeInstance(0);
    auto instance = type->construct({ { &params } });

    ASSERT_EQ(instance.getType(), store.get<TestType>());

    auto* failed_cast = instance.cast<int>();
    ASSERT_TRUE(failed_cast == nullptr);
}