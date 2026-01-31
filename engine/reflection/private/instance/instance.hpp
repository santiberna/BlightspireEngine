#pragma once
#include <common.hpp>

#include <memory>
#include <string_view>
#include <utility/argument_list.hpp>

class Type;
class InstanceRef;
class VoidInstanceType;

class Instance
{
public:
    Instance(VoidInstanceType);
    bool operator==(VoidInstanceType) const;

    template <typename T> NO_DISCARD bool is() const;
    template <typename T> NO_DISCARD std::shared_ptr<const T> cast() const;
    template <typename T> NO_DISCARD std::shared_ptr<T> cast();

    NO_DISCARD Instance call(std::string_view name, const ArgumentList& args) const;
    NO_DISCARD InstanceRef asRef() const;
    NO_DISCARD InstanceRef access(std::string_view name) const;

    NO_DISCARD const Type* getType() const;

private:
    friend class TypeStore; // For access to private constructor
    Instance(std::shared_ptr<void> value, const Type* type);

    std::shared_ptr<void> value {};
    const Type* type {};
};

class VoidInstanceType
{
};
constexpr VoidInstanceType VOID_INSTANCE = {};

#include <instance/instance_impl.hpp>