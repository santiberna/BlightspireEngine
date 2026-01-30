#pragma once
#include <common.hpp>

#include <cstddef>

class TypeStore;
class Instance;
class Type;

/// TODO: rework this class, apparently we only need offset here.
class Field
{
public:
    template <typename Class, typename Member> using MemberPointer = Member Class::*;

    template <typename Class, typename Member>
    Field(TypeStore& type_store, MemberPointer<Class, Member> mem_ptr);

    ~Field() = default;

    DEFAULT_MOVABLE(Field);
    NON_COPYABLE(Field);

    /// Returns nullptr if there is a type mismatch in the passed instance or the accessed type
    // template <typename T> NO_DISCARD const T* access(const Instance& instance) const;

    // /// Returns nullptr if there is a type mismatch in the passed instance or the accessed type
    // template <typename T> NO_DISCARD T* access(Instance& instance);

    void* apply_raw_offset(void* ptr) const { return (std::byte*)ptr + offset; };
    NO_DISCARD const Type* getType() const { return type; }

private:
    size_t offset {};
    const Type* owner {};
    const Type* type {};
};

#include <member/field_impl.hpp>