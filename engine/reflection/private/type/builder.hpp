#pragma once

#include <member/field.hpp>

#include <constructor/constructor.hpp>
#include <member/method.hpp>
#include <types.hpp>

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

/// IMPL

template <typename T>
TypeBuilder<T>::TypeBuilder(TypeStore& type_store)
    : type_store(type_store)
    , for_type(type_store.get<T>())
{
}

template <typename T>
template <typename U>
TypeBuilder<T>& TypeBuilder<T>::addField(Field::MemberPointer<T, U> ptr)
{
    auto field = std::make_unique<Field>(type_store, ptr);
    for_type->fields.emplace(std::move(field));
    return *this;
}

template <typename T>
template <typename Ret, typename... Args>
TypeBuilder<T>& TypeBuilder<T>::addMethod(Method::MemberPointer<T, Ret, Args...> ptr)
{
    using Impl = MethodImpl<Ret, T, void, Args...>;
    auto method = std::make_unique<Impl>(type_store, ptr);
    for_type->methods.emplace_back(std::move(method));
    return *this;
}

template <typename T>
template <typename Ret, typename... Args>
TypeBuilder<T>& TypeBuilder<T>::addConstMethod(Method::ConstMemberPointer<T, Ret, Args...> ptr)
{
    using Impl = MethodImpl<Ret, T, const void, Args...>;
    auto field = std::make_unique<Impl>(type_store, ptr);
    for_type->methods.emplace_back(std::move(field));
    return *this;
}

template <typename T>
TypeBuilder<T>& TypeBuilder<T>::addConstant(std::string_view name, uint64_t value)
{
    for_type->constants.emplace(std::string(name), value);
    return *this;
}

template <typename T> template <typename... Args> TypeBuilder<T>& TypeBuilder<T>::addConstructor()
{
    using Impl = ConstructorImpl<T, Args...>;

    auto constructor = std::make_unique<Impl>(type_store);
    auto params = ParameterList { { type_store.get<Args>()... } };

    for_type->constructors.emplace(params, std::move(constructor));
    return *this;
}