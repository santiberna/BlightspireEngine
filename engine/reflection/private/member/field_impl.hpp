#pragma once
#include <member/field.hpp>

#include <instance/instance.hpp>
#include <type/store.hpp>


template <typename Class, typename Member>
Field::Field(TypeStore& type_store, Member Class::* mem_ptr)
{
    this->type = type_store.get<Member>();
    this->owner = type_store.get<Class>();
    this->name = "Unimplemented";

    Class* null = nullptr;
    auto* address = std::addressof(null->*mem_ptr);
    this->offset = reinterpret_cast<size_t>(address);
}

template <typename T> NO_DISCARD const T* Field::access(const Instance& instance) const
{
    return const_cast<Field*>(this)->access<T>(const_cast<Instance&>(instance));
}

template <typename T> NO_DISCARD T* Field::access(Instance& instance)
{
    if (!(instance.getType() == this->owner))
    {
        return nullptr;
    }

    if (!this->type->is<T>())
    {
        return nullptr;
    }

    void* address = const_cast<void*>(instance.getAddress());
    auto* base = static_cast<std::byte*>(address);
    auto* field = base + this->offset;

    return reinterpret_cast<T*>(field);
}