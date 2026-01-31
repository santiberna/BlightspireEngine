#pragma once
#include <member/field.hpp>

#include <instance/instance.hpp>
#include <type/store.hpp>

template <typename Class, typename Member>
Field::Field(TypeStore& type_store, Member Class::* mem_ptr)
{
    this->type = type_store.get<Member>();

    Class* null = nullptr;
    auto* address = std::addressof(null->*mem_ptr);
    this->offset = reinterpret_cast<size_t>(address);
}