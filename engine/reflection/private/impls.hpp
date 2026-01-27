#pragma once

#include <cassert>
#include <types.hpp>

#include <constructor/constructor.hpp>
#include <destructor/destructor.hpp>
#include <member/field.hpp>
#include <member/method.hpp>

/// Template impls

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

template <typename T> bool Type::is() const { return this->getIndex() == typeid(T); }

template <typename T> Instance TypeStore::makeInstance(T&& val)
{
    return Instance(this->get<T>(), std::forward<T>(val));
}

template <typename T> Type* TypeStore::get()
{
    std::type_index index = typeid(T);

    auto it = type_map.find(index);
    if (it != type_map.end())
    {
        return it->second.get();
    }

    Type type { this };

    type.index = &typeid(T);
    type.name = std::string(typeid(T).name());
    type.destructor = std::make_unique<DestructorImpl<T>>();
    type.size = sizeof(T);

    // Initialize new_type here;

    auto unique = std::make_unique<Type>(std::move(type));
    auto [inserted_it, success] = type_map.emplace(index, std::move(unique));
    return inserted_it->second.get();
}
