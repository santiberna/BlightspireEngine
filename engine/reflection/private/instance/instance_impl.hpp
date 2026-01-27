#pragma once
#include <instance/instance.hpp>

#include <cassert>
#include <stdexcept>
#include <type/type.hpp>

template <typename T> Instance::Instance(const Type* type, T val)
{
    if (type->getIndex() != typeid(T))
    {
        throw std::runtime_error("Creating an instance from incorrect type!");
    }

    this->ptr = new T(std::move(val));
    this->type = type;
}

template <typename T> T* Instance::cast()
{
    if (this->type->is<T>())
    {
        return static_cast<T*>(this->ptr);
    }
    return nullptr;
}

template <typename T> const T* Instance::cast() const
{
    return const_cast<Instance*>(this)->cast<T>();
}
