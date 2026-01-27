#pragma once

#include <constructor/constructor.hpp>
#include <member/field.hpp>
#include <member/method.hpp>

template <typename T> class TypeBuilder
{
public:
    TypeBuilder(TypeStore& type_store);
    NON_MOVABLE(TypeBuilder);
    NON_COPYABLE(TypeBuilder);
    ~TypeBuilder() = default;

    template <typename U> TypeBuilder& addField(Field::MemberPointer<T, U> ptr);

    template <typename Ret, typename... Args>
    TypeBuilder& addMethod(Method::MemberPointer<T, Ret, Args...> ptr);

    template <typename Ret, typename... Args>
    TypeBuilder& addConstMethod(Method::ConstMemberPointer<T, Ret, Args...> ptr);

    TypeBuilder& addConstant(std::string_view name, uint64_t value);

    template <typename... Args> TypeBuilder& addConstructor();

private:
    TypeStore& type_store;
    Type* for_type {};
};

#include <type/builder_impl.hpp>