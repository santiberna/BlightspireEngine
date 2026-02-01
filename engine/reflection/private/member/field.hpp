#pragma once
#include <common.hpp>

#include <cstddef>

class ReflectFactory;
class Value;
class Type;

/// TODO: rework this class, apparently we only need offset here.
class Field
{
public:
    template <typename Class, typename Member> using MemberPointer = Member Class::*;
    template <typename Class, typename Member>
    Field(ReflectFactory& type_store, MemberPointer<Class, Member> mem_ptr);

    ~Field() = default;
    DEFAULT_MOVABLE(Field);
    NON_COPYABLE(Field);

    NO_DISCARD void* rawAccess(void* ptr) const;
    NO_DISCARD const Type* getType() const;

private:
    size_t offset {};
    const Type* type {};
};

#include <member/field_impl.hpp>