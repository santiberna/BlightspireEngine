#pragma once
#include <common.hpp>
#include <memory>
#include <reflection_fwd.hpp>
#include <string_view>
#include <utility/argument_list.hpp>

namespace reflection
{
class VoidValueType
{
};
constexpr VoidValueType VOID_INSTANCE = {};

class Value
{
public:
    Value(VoidValueType);
    bool operator==(VoidValueType) const;

    template <typename T>
    [[nodiscard]] bool is() const;
    template <typename T>
    [[nodiscard]] std::shared_ptr<const T> cast() const;
    template <typename T>
    [[nodiscard]] std::shared_ptr<T> cast();

    [[nodiscard]] Value call(std::string_view name, const ArgumentList& args) const;
    [[nodiscard]] ValueRef asRef() const;
    [[nodiscard]] ValueRef access(std::string_view name) const;

    [[nodiscard]] const Type* getType() const;

private:
    friend ReflectFactory; // For access to private constructor
    Value(std::shared_ptr<void> value, const Type* type);

    std::shared_ptr<void> value {};
    const Type* type {};
};

}

#include <value/value_impl.hpp>