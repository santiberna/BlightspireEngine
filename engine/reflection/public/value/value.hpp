#pragma once
#include <common.hpp>

#include <memory>
#include <string_view>
#include <utility/argument_list.hpp>

class Type;
class ValueRef;
class VoidValueType;

class Value
{
public:
    Value(VoidValueType);
    bool operator==(VoidValueType) const;

    template <typename T> NO_DISCARD bool is() const;
    template <typename T> NO_DISCARD std::shared_ptr<const T> cast() const;
    template <typename T> NO_DISCARD std::shared_ptr<T> cast();

    NO_DISCARD Value call(std::string_view name, const ArgumentList& args) const;
    NO_DISCARD ValueRef asRef() const;
    NO_DISCARD ValueRef access(std::string_view name) const;

    NO_DISCARD const Type* getType() const;

private:
    friend class ReflectFactory; // For access to private constructor
    Value(std::shared_ptr<void> value, const Type* type);

    std::shared_ptr<void> value {};
    const Type* type {};
};

class VoidValueType
{
};
constexpr VoidValueType VOID_INSTANCE = {};

#include <value/value_impl.hpp>