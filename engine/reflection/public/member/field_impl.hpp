#pragma once
#include <factory/reflect_factory.hpp>
#include <member/field.hpp>
#include <value/value.hpp>

template <typename Class, typename Member>
reflect::Field::Field(ReflectFactory& type_store, Member Class::* mem_ptr)
{
    this->type = type_store.get<Member>();

    Class* null = nullptr;
    auto* address = std::addressof(null->*mem_ptr);
    this->offset = reinterpret_cast<size_t>(address);
}