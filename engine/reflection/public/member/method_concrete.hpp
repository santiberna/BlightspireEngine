#pragma once
#include <factory/reflect_factory.hpp>
#include <member/method.hpp>
#include <stdexcept>
#include <utility/traits.hpp>
#include <value/value_ref.hpp>

namespace reflection
{

// Qualifiers can only be void of const void
template <typename Ret, typename Class, typename Qualifiers, typename... Args>
class MethodImpl : public Method
{
    // Static assert to validate Qualifiers
    static_assert(std::is_same_v<Qualifiers, void> || std::is_same_v<Qualifiers, const void>,
        "Qualifiers must be either void (non-const member function) or const void (const member "
        "function)");

public:
    // Conditional type based on Qualifiers
    using Callable = std::conditional_t<std::is_same_v<Qualifiers, const void>,
        ConstMemberPointer<Class, Ret, Args...>, MemberPointer<Class, Ret, Args...>>;

    MethodImpl(ReflectFactory& type_store, Callable ptr);
    NO_DISCARD Value invoke(ValueRef object, const ArgumentList& parameters) const override;

private:
    template <std::size_t... Is>
    NO_DISCARD Ret invokeHelper(
        Class& obj, const ArgumentList& args, std::index_sequence<Is...> sequence) const;

    Callable callable {};
};

template <typename Ret, typename Class, typename Qualifiers, typename... Args>
MethodImpl<Ret, Class, Qualifiers, Args...>::MethodImpl(ReflectFactory& type_store, Callable ptr)
    : Method(type_store)
{
    this->callable = ptr;
    this->parameters = ParameterList { { type_store.get<Args>()... } };
}

template <typename Ret, typename Class, typename Qualifiers, typename... Args>
Value MethodImpl<Ret, Class, Qualifiers, Args...>::invoke(
    ValueRef object, const ArgumentList& args) const
{
    constexpr auto ARG_SIZE = sizeof...(Args);
    assert(this->parameters.length() == ARG_SIZE);

    if (!this->parameters.validateArgs(args))
    {
        throw std::runtime_error("Parameter type or count mismatch");
    }

    // Get the object as Class*
    Class& obj = object.cast<Class>();
    if constexpr (std::is_void_v<Ret>)
    {
        invokeHelper(obj, args, std::index_sequence_for<Args...> {});
        return VOID_INSTANCE;
    }
    else
    {
        Ret result = invokeHelper(obj, args, std::index_sequence_for<Args...> {});
        return store.makeValue<Ret>(std::move(result));
    }
}

template <typename Ret, typename Class, typename Qualifiers, typename... Args>
template <std::size_t... Is>
Ret MethodImpl<Ret, Class, Qualifiers, Args...>::invokeHelper(
    Class& obj, const ArgumentList& args, MAYBE_UNUSED std::index_sequence<Is...> sequence) const
{
    return (obj.*callable)(args.get(Is).cast<detail::BareType<Args>>()...);
}

}