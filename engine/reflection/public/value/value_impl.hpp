#pragma once
#include <cassert>
#include <member/field.hpp>
#include <stdexcept>
#include <type/type.hpp>
#include <utility/traits.hpp>
#include <value/value.hpp>


template <typename T> std::shared_ptr<const T> reflect::Value::cast() const
{
    static_assert(std::is_same_v<T, detail::BareType<T>>,
        "Types used for reflection must not have cv-qualifiers or be refs.");

    if (this->type->is<T>())
    {
        return std::static_pointer_cast<const T>(this->value);
    }
    throw std::runtime_error("Casting from incorrect type!");
}

template <typename T> std::shared_ptr<T> reflect::Value::cast()
{
    const auto* self = this;
    return std::const_pointer_cast<T>(self->cast<T>());
}

template <typename T> bool reflect::Value::is() const { return getType()->is<T>(); }