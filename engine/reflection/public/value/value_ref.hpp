#pragma once
#include <common.hpp>
#include <reflection_fwd.hpp>
#include <string_view>

namespace reflection
{

class ValueRef
{
public:
    template <typename T>
    [[nodiscard]] bool is() const;
    template <typename T>
    [[nodiscard]] const T& cast() const;
    template <typename T>
    [[nodiscard]] T& cast();

    [[nodiscard]] Value call(std::string_view name, const ArgumentList& args);
    [[nodiscard]] ValueRef access(std::string_view name) const;

    [[nodiscard]] const Type* getType() const;

private:
    friend ReflectFactory;
    friend Value;

    ValueRef(void* value, const Type* type);

    void* value {};
    const Type* type;
};

}

#include <value/value_ref_impl.hpp>