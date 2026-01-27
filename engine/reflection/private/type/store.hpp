#pragma once
#include <common.hpp>

#include <memory>
#include <typeindex>
#include <unordered_map>

class Type;
class Instance;

class TypeStore
{
public:
    TypeStore() = default;
    ~TypeStore() = default;

    NON_COPYABLE(TypeStore);

    template <typename T> NO_DISCARD Type* get();
    template <typename T> NO_DISCARD Instance makeInstance(T&& val);

private:
    std::unordered_map<std::type_index, std::unique_ptr<Type>> type_map;
};

#include <type/store_impl.hpp>