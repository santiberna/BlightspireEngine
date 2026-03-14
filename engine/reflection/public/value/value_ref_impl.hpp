#pragma once
#include <stdexcept>
#include <type/type.hpp>
#include <utility/traits.hpp>
#include <value/value_ref.hpp>

namespace reflection
{

template <typename T>
const T& ValueRef::cast() const
{
    static_assert(std::is_same_v<T, detail::BareType<T>>,
        "Types used for reflectionion must not have cv-qualifiers or be refs.");

    if (this->type->is<T>())
    {
        return *static_cast<const T*>(this->value);
    }
    throw std::runtime_error("Casting from incorrect type!");
}

template <typename T>
T& ValueRef::cast()
{
    const auto* self = this;
    return const_cast<T&>(self->cast<T>());
}

template <typename T>
bool ValueRef::is() const { return getType()->is<T>(); }

}