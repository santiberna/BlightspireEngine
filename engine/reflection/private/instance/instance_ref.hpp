#pragma once
#include <common.hpp>

#include <string_view>

class Type;
class Instance;
class TypeStore;
class ArgumentList;

class InstanceRef
{
public:
    template <typename T> NO_DISCARD bool is() const;
    template <typename T> NO_DISCARD const T& cast() const;
    template <typename T> NO_DISCARD T& cast();

    NO_DISCARD Instance call(std::string_view name, const ArgumentList& args);
    NO_DISCARD InstanceRef access(std::string_view name) const;

    NO_DISCARD const Type* getType() const;

private:
    friend TypeStore;
    friend Instance;

    InstanceRef(void* value, const Type* type);

    void* value {};
    const Type* type;
};

#include <instance/instance_ref_impl.hpp>