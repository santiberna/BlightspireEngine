#pragma once
#include <common.hpp>
#include <reflect_fwd.hpp>
#include <string_view>

namespace reflect
{

class ValueRef
{
public:
    template <typename T> NO_DISCARD bool is() const;
    template <typename T> NO_DISCARD const T& cast() const;
    template <typename T> NO_DISCARD T& cast();

    NO_DISCARD Value call(std::string_view name, const ArgumentList& args);
    NO_DISCARD ValueRef access(std::string_view name) const;

    NO_DISCARD const Type* getType() const;

private:
    friend ReflectFactory;
    friend Value;

    ValueRef(void* value, const Type* type);

    void* value {};
    const Type* type;
};

}

#include <value/value_ref_impl.hpp>