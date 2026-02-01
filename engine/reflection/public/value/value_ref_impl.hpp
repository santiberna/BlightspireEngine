#pragma once
#include <stdexcept>
#include <type/type.hpp>
#include <utility/traits.hpp>
#include <value/value_ref.hpp>

namespace reflect
{

template <typename T> const T& ValueRef::cast() const
{
    static_assert(std::is_same_v<T, BareType<T>>,
        "Types used for reflection must not have cv-qualifiers or be refs.");

    if (this->type->is<T>())
    {
        return *static_cast<const T*>(this->value);
    }
    throw std::runtime_error("Casting from incorrect type!");
}

template <typename T> T& ValueRef::cast()
{
    const auto* self = this;
    return const_cast<T&>(self->cast<T>());
}

template <typename T> bool ValueRef::is() const { return getType()->is<T>(); }

}