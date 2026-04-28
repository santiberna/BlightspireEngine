#pragma once
#include <common.hpp>
#include <reflection_fwd.hpp>

namespace reflection
{
class Field
{
public:
    template <typename Class, typename Member>
    using MemberPointer = Member Class::*;
    template <typename Class, typename Member>
    Field(ReflectFactory& type_store, MemberPointer<Class, Member> mem_ptr);
    ~Field();

    NON_MOVABLE(Field);
    NON_COPYABLE(Field);

    [[nodiscard]] void* rawAccess(void* ptr) const;
    [[nodiscard]] const Type* getType() const;

private:
    bb::usize offset {};
    const Type* type {};
};
}

#include <member/field_impl.hpp>