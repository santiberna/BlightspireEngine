#pragma once
#include <member/method.hpp>

#include <stdexcept>

template <typename Ret, typename Class, typename... Args>
class MethodImpl : public Method
{
    using Callable = Ret (Class::*)(Args...);

public:
    MethodImpl(TypeStore& type_store, Callable ptr);
    NO_DISCARD Instance invoke(TypeStore& type_store, Instance& object, const ArgumentList& parameters) const override;

private:
    template <std::size_t... Is>
    NO_DISCARD Ret invokeHelper(Class* obj, const ArgumentList& args, std::index_sequence<Is...>) const;

    Callable callable {};
};

template <typename Ret, typename Class, typename... Args>
MethodImpl<Ret, Class, Args...>::MethodImpl(TypeStore& type_store, Callable ptr)
    : Method()
{
    this->name = "Unimplemented!";
    this->callable = ptr;
    this->parameters = ParameterList { { type_store.get<Args>()... } };
}

template <typename Ret, typename Class, typename... Args>
Instance MethodImpl<Ret, Class, Args...>::invoke(TypeStore& type_store, Instance& object, const ArgumentList& args) const
{
    constexpr auto ARG_SIZE = sizeof...(Args);
    assert(this->parameters.types.size() == ARG_SIZE);

    if (!this->parameters.validateArgs(args))
    {
        throw std::runtime_error("Parameter type or count mismatch");
    }

    // Get the object as Class*
    Class* obj = object.cast<Class>();
    if (obj == nullptr)
    {
        throw std::runtime_error("Invalid instance type");
    }

    if constexpr (std::is_void_v<Ret>)
    {
        invokeHelper(obj, args, std::index_sequence_for<Args...> {});
        return {};
    }
    else
    {
        Ret result = invokeHelper(obj, args, std::index_sequence_for<Args...> {});
        return Instance(type_store, std::move(result));
    }
}

template <typename Ret, typename Class, typename... Args>
template <std::size_t... Is>
Ret MethodImpl<Ret, Class, Args...>::invokeHelper(Class* obj, const ArgumentList& args, std::index_sequence<Is...>) const
{
    return (obj->*callable)(*(*args.values[Is]).cast<Args>()...);
}