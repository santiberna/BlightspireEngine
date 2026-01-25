#pragma once
#include <types.hpp>

class Field
{
public:
    template <typename Class, typename Member>
    Field(TypeStore& type_store, Member Class::* mem_ptr);

    /// Returns nullptr if there is a type mismatch in the passed instance or the accessed type
    template <typename T>
    NO_DISCARD const T* access(const Instance&) const;

    /// Returns nullptr if there is a type mismatch in the passed instance or the accessed type
    template <typename T>
    NO_DISCARD T* access(Instance&);

private:
    std::string name;
    size_t offset {};
    const Type* owner {};
    const Type* type {};
};

#include <member/field_impl.hpp>