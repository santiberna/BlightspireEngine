#include <gtest/gtest.h>
#include <reflection_macros.hpp>

struct TestType
{
    int a = 0;
    bool isZero() const { return !(bool)(a); }
    void set(int b) { a = b; }

    constexpr static bb::u64 FLAGS = 0;
    void out(int& b) { b = a; }
};

STATIC_BLOCK
{
    NEW_TYPE(TestType)
    ADD_CONSTRUCTOR(int)
    ADD_METHOD(&TestType::isZero)
    ADD_METHOD(&TestType::out)
    ADD_METHOD(&TestType::set)
    ADD_FIELD(&TestType::a)
    ADD_CONSTANT(TestType::FLAGS);
}

TEST(TypeTests, MemberMethod)
{
    const auto* type = reflection::getType<TestType>();
    EXPECT_EQ(type->getConstant("FLAGS"), 0);

    auto value = type->construct(reflection::makeArgumentList(0));

    ASSERT_EQ(value.getType(), reflection::getType<TestType>());
    ASSERT_FALSE(value.is<int>());

    auto& set_result = value.access("a").cast<int>();
    set_result = 0;

    auto ret = value.call("isZero", {});
    ASSERT_TRUE(ret.is<bool>());
    ASSERT_TRUE(*ret.cast<bool>());

    set_result = 10;
    int test = 0;
    (void)value.call("out", reflection::makeArgumentList(test));
    ASSERT_TRUE(test == 10);
}