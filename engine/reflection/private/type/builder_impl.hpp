#pragma once
#include <type/builder.hpp>

#include <constructor/constructor_concrete.hpp>
#include <member/method_concrete.hpp>

template <typename T>
TypeBuilder<T>::TypeBuilder(TypeStore& type_store)
    : type_store(type_store)
    , for_type(type_store.get_mut<T>())
{
}

template <typename T>
template <typename U>
TypeBuilder<T>& TypeBuilder<T>::addField(std::string_view name, Field::MemberPointer<T, U> ptr)
{
    auto field = Field(type_store, ptr);
    auto [ret, ok] = for_type->fields.emplace(std::string(name), std::move(field));
    assert(ok && "Adding field with the same name! Check your type definition!");
    return *this;
}

template <typename T>
template <typename Ret, typename... Args>
TypeBuilder<T>& TypeBuilder<T>::addMethod(
    std::string_view name, Method::MemberPointer<T, Ret, Args...> ptr)
{
    using Impl = MethodImpl<Ret, T, void, Args...>;
    auto method = std::make_unique<Impl>(type_store, ptr);
    auto [ret, ok] = for_type->methods.emplace(std::string(name), std::move(method));
    assert(ok && "Adding field with the same name! Check your type definition!");
    return *this;
}

template <typename T>
template <typename Ret, typename... Args>
TypeBuilder<T>& TypeBuilder<T>::addMethod(
    std::string_view name, Method::ConstMemberPointer<T, Ret, Args...> ptr)
{
    using Impl = MethodImpl<Ret, T, const void, Args...>;
    auto method = std::make_unique<Impl>(type_store, ptr);
    auto [ret, ok] = for_type->methods.emplace(std::string(name), std::move(method));
    assert(ok && "Adding field with the same name! Check your type definition!");
    return *this;
}

template <typename T>
TypeBuilder<T>& TypeBuilder<T>::addConstant(std::string_view name, uint64_t value)
{
    auto [ret, ok] = for_type->constants.emplace(std::string(name), value);
    assert(ok && "Adding field with the same name! Check your type definition!");
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