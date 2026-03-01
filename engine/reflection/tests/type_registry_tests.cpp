#include <gtest/gtest.h>
#include <reflect_macros.hpp>

struct TestType
{
    int a = 0;
    bool isZero() const { return !(bool)(a); }
    void set(int b) { a = b; }

    constexpr static uint64_t FLAGS = 0;
    void input(int& a) { a = 10; }
};

REFLECT_TYPE(TestType)
{
    NEW_TYPE(TestType)
    ADD_CONSTRUCTOR(int)
    ADD_METHOD(&TestType::isZero)
    ADD_METHOD(&TestType::input)
    ADD_METHOD(&TestType::set)
    ADD_FIELD(&TestType::a)
    ADD_CONSTANT(TestType::FLAGS);
}

TEST(TypeTests, MemberMethod)
{
    const auto* type = reflect::getType<TestType>();
    EXPECT_EQ(type->getConstant("FLAGS"), 0);

    auto value = type->construct(reflect::makeArgumentList(0));

    ASSERT_EQ(value.getType(), reflect::getType<TestType>());
    ASSERT_FALSE(value.is<int>());

    auto& set_result = value.access("a").cast<int>();
    set_result = 0;

    auto ret = value.call("isZero", {});
    ASSERT_TRUE(ret.is<bool>());
    ASSERT_TRUE(*ret.cast<bool>());

    int test = 0;
    (void)value.call("input", reflect::makeArgumentList(test));
    ASSERT_TRUE(test == 10);
}