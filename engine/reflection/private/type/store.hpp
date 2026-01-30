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

    DEFAULT_MOVABLE(TypeStore);
    NON_COPYABLE(TypeStore);

    template <typename T> NO_DISCARD Type* get();
    template <typename T, typename... Args> NO_DISCARD Instance makeInstance(Args&&... args);

private:
    std::unordered_map<std::type_index, std::unique_ptr<Type>> type_map;
};

#include <type/store_impl.hpp>