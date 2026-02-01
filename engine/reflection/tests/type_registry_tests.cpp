#include <gtest/gtest.h>
#include <reflect.hpp>

struct TestType
{
    int a = 0;
    bool isZero() const { return !(bool)(a); }
    void set(int b) { a = b; }

    void input(int& a) { a = 10; }
};

TEST(TypeTests, MemberMethod)
{
    TypeBuilder builder = reflect::makeBuilder<TestType>();

    builder.addConstructor<>()
        .addConstructor<int>()
        .addMethod("is_zero", &TestType::isZero)
        .addMethod("set", &TestType::set)
        .addMethod("input", &TestType::input)
        .addField("a", &TestType::a)
        .addConstant("Constant", 42)
        .addConstant("Constant2", 69);

    const auto* type = reflect::getType<TestType>();
    EXPECT_EQ(type->getConstant("Constant"), 42);
    EXPECT_EQ(type->getConstant("Constant2"), 69);

    auto value = type->construct(reflect::makeArgumentList(0));

    ASSERT_EQ(value.getType(), reflect::getType<TestType>());
    ASSERT_FALSE(value.is<int>());

    auto& set_result = value.access("a").cast<int>();
    set_result = 0;

    auto ret = value.call("is_zero", {});
    ASSERT_TRUE(ret.is<bool>());
    ASSERT_TRUE(*ret.cast<bool>());

    int test = 0;
    (void)value.call("input", reflect::makeArgumentList(test));
    ASSERT_TRUE(test == 10);
}