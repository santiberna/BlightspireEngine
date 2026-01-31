#include <gtest/gtest.h>
#include <instance/instance_ref.hpp>
#include <type/builder.hpp>

struct TestType
{
    int a = 0;
    bool isZero() const { return !(bool)(a); }
    void set(int b) { a = b; }

    void input(int& a) { a = 10; }
};

TEST(TypeTests, MemberMethod)
{
    auto store = TypeStore {};
    TypeBuilder builder = TypeBuilder<TestType>(store);

    builder.addConstructor<>()
        .addConstructor<int>()
        .addMethod("is_zero", &TestType::isZero)
        .addMethod("set", &TestType::set)
        .addMethod("input", &TestType::input)
        .addField("a", &TestType::a)
        .addConstant("Constant", 42)
        .addConstant("Constant2", 69);

    auto* type = store.get<TestType>();
    EXPECT_EQ(type->getConstant("Constant"), 42);
    EXPECT_EQ(type->getConstant("Constant2"), 69);

    Instance params = store.makeInstance<int>(0);
    ArgumentList args = { { params.asRef() } };

    auto instance = type->construct(args);

    ASSERT_EQ(instance.getType(), store.get<TestType>());
    ASSERT_FALSE(instance.is<int>());

    auto& set_result = instance.access("a").cast<int>();
    set_result = 0;

    auto ret = instance.call("is_zero", {});
    ASSERT_TRUE(ret.is<bool>());
    ASSERT_TRUE(*ret.cast<bool>());

    auto test_in = store.makeInstance<int>(0);
    ArgumentList args2 = { { test_in.asRef() } };

    (void)instance.call("input", args2);
    ASSERT_TRUE(*test_in.cast<int>() == 10);
}