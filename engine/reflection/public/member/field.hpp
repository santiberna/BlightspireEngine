#pragma once
#include <common.hpp>
#include <cstddef>
#include <reflection_fwd.hpp>

namespace reflection
{
class Field
{
public:
    template <typename Class, typename Member> using MemberPointer = Member Class::*;
    template <typename Class, typename Member>
    Field(ReflectFactory& type_store, MemberPointer<Class, Member> mem_ptr);
    ~Field();

    NON_MOVABLE(Field);
    NON_COPYABLE(Field);

    NO_DISCARD void* rawAccess(void* ptr) const;
    NO_DISCARD const Type* getType() const;

private:
    size_t offset {};
    const Type* type {};
};
}

#include <member/field_impl.hpp>