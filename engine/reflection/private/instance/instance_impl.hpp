#pragma once
#include <instance/instance.hpp>

#include <cassert>
#include <member/field.hpp>
#include <stdexcept>
#include <type/type.hpp>
#include <utility/traits.hpp>

template <typename T> std::shared_ptr<T> Instance::cast()
{
    static_assert(std::is_same_v<T, BareType<T>>,
        "Types used for reflection must not have cv-qualifiers or be refs.");

    if (this->type->is<T>())
    {
        return std::static_pointer_cast<T>(this->value);
    }
    throw std::runtime_error("Casting from incorrect type!");
}

template <typename T> std::shared_ptr<const T> Instance::cast() const
{
    auto* this_mut = const_cast<Instance*>(this);
    return std::const_pointer_cast<const T>(this_mut->cast<T>());
}

template <typename T> bool Instance::is() const { return getType()->is<T>(); }