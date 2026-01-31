#pragma once
#include <instance/instance_ref.hpp>

#include <stdexcept>
#include <type/type.hpp>
#include <utility/traits.hpp>

template <typename T> T& InstanceRef::cast()
{
    static_assert(std::is_same_v<T, BareType<T>>,
        "Types used for reflection must not have cv-qualifiers or be refs.");

    if (this->type->is<T>())
    {
        return *static_cast<T*>(this->value);
    }
    throw std::runtime_error("Casting from incorrect type!");
}

template <typename T> const T& InstanceRef::cast() const
{
    return const_cast<InstanceRef*>(this)->cast<T>();
}

template <typename T> bool InstanceRef::is() const { return getType()->is<T>(); }