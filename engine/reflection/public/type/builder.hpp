#pragma once
#include <constructor/constructor.hpp>
#include <member/field.hpp>
#include <member/method.hpp>

namespace reflect
{

template <typename T> class TypeBuilder
{
    static_assert(std::is_same_v<T, std::remove_cvref_t<T>>,
        "Types used for reflection must not have cv-qualifiers or be refs.");

public:
    TypeBuilder(detail::ReflectFactory& type_store);
    ~TypeBuilder() = default;

    NON_MOVABLE(TypeBuilder);
    NON_COPYABLE(TypeBuilder);

    template <typename U>
    TypeBuilder& addField(std::string_view name, detail::Field::MemberPointer<T, U> ptr);

    template <typename Ret, typename... Args>
    TypeBuilder& addMethod(
        std::string_view name, detail::Method::MemberPointer<T, Ret, Args...> ptr);

    template <typename Ret, typename... Args>
    TypeBuilder& addMethod(
        std::string_view name, detail::Method::ConstMemberPointer<T, Ret, Args...> ptr);

    TypeBuilder& addConstant(std::string_view name, uint64_t value);

    template <typename... Args> TypeBuilder& addConstructor();

private:
    detail::ReflectFactory& type_store;
    Type* for_type {};
};

}

#include <type/builder_impl.hpp>