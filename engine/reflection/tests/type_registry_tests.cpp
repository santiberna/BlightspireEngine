#include <gtest/gtest.h>
#include <type_registry.hpp>

struct TestItem
{
    std::string name;
};

namespace TestNamespace
{
struct TestItem2
{
    std::string name;
};
}

TEST(TypeRegistryTests, NewType)
{
    auto registry = TypeRegistry();
    registry.newType<TestItem>();

    EXPECT_TRUE(registry.containsType<TestItem>());
}

TEST(TypeRegistryTests, NewTypeCheckName)
{
    auto registry = TypeRegistry();
    registry.newType<TestItem>();
    registry.newType<TestNamespace::TestItem2>();

    EXPECT_TRUE(registry.containsTypeNamed("TestItem"));
    EXPECT_TRUE(registry.containsTypeNamed("TestNamespace::TestItem2"));
}

// TEST(TypeRegistryTests, GetTypeNames)
// {
//     auto registry = TypeRegistry();
//     registry.newType<TestItem>();

//     EXPECT_TRUE(condition)
// }
