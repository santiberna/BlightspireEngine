#pragma once

#include <member/field.hpp>
#include <types.hpp>

class TypeBuilder
{
public:
    template <typename T> TypeBuilder(TypeStore& type_store);
    DEFAULT_MOVABLE(TypeBuilder);
    NON_COPYABLE(TypeBuilder);
    ~TypeBuilder() = default;

    template <typename Class, typename Member>
    TypeBuilder& addField(Field::PointerType<Class, Member> ptr)
    {
        auto field = std::make_unique<Field>(type_store, ptr);
        for_type->fields.emplace(std::move(field));
    }

private:
    TypeStore& type_store;
    Type* for_type {};
};

/// IMPL

template <typename T>
TypeBuilder::TypeBuilder(TypeStore& type_store)
    : type_store(type_store)
    , for_type(type_store.get<T>())
{
}