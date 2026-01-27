#pragma once

#include <utility>

#include <type/store.hpp>

#include <destructor/destructor.hpp>
#include <instance/instance.hpp>
#include <type/type.hpp>

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

    Type type {};

    type.owner_store = this;
    type.index = &typeid(T);
    type.name = std::string(typeid(T).name());
    type.destructor = std::make_unique<DestructorImpl<T>>();
    type.size = sizeof(T);

    // Initialize new_type here;

    auto unique = std::make_unique<Type>(std::move(type));
    auto [inserted_it, success] = type_map.emplace(index, std::move(unique));
    return inserted_it->second.get();
}
