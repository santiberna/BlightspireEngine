#pragma once
#include <type/builder.hpp>

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
    auto field = Field(type_store, ptr);
    for_type->fields.emplace_back(std::move(field));
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