#pragma once

#include <cassert>
#include <types.hpp>

#include <constructor/constructor.hpp>
#include <destructor/destructor.hpp>
#include <member/field.hpp>
#include <member/method.hpp>

/// Template impls

template <typename T> Instance::Instance(TypeStore& type_store, T val)
{
    this->ptr = new T(std::move(val));
    this->type = type_store.get<T>();
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

template <typename T> Type* TypeStore::get()
{
    std::type_index index = typeid(T);

    auto it = type_map.find(index);
    if (it != type_map.end())
    {
        return it->second.get();
    }

    Type type {};

    type.index = &typeid(T);
    type.name = std::string(typeid(T).name());
    type.destructor = std::make_unique<DestructorImpl<T>>();
    type.size = sizeof(T);

    // Initialize new_type here;

    auto unique = std::make_unique<Type>(std::move(type));
    auto [inserted_it, success] = type_map.emplace(index, std::move(unique));
    return inserted_it->second.get();
}
