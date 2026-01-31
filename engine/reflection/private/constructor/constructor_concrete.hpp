#pragma once
#include <constructor/constructor.hpp>

#include <instance/instance.hpp>
#include <stdexcept>
#include <type/store.hpp>
#include <utility/traits.hpp>

template <typename Class, typename... Args> class ConstructorImpl : public Constructor
{
public:
    ConstructorImpl(TypeStore& type_store);
    NO_DISCARD Instance invoke(const ArgumentList& parameters) const override;

private:
    template <std::size_t... Is>
    NO_DISCARD Instance invokeHelper(
        const ArgumentList& args, std::index_sequence<Is...> _sequence) const;
};

template <typename Class, typename... Args>
ConstructorImpl<Class, Args...>::ConstructorImpl(TypeStore& type_store)
    : Constructor(type_store)
{
    this->parameters = ParameterList { { type_store.get<Args>()... } };
}

template <typename Class, typename... Args>
Instance ConstructorImpl<Class, Args...>::invoke(const ArgumentList& args) const
{
    constexpr auto ARG_SIZE = sizeof...(Args);
    assert(this->parameters.length() == ARG_SIZE);

    if (!this->parameters.validateArgs(args))
    {
        throw std::runtime_error("Parameter type or count mismatch");
    }

    return invokeHelper(args, std::index_sequence_for<Args...> {});
}

template <typename Class, typename... Args>
template <std::size_t... Is>
NO_DISCARD Instance ConstructorImpl<Class, Args...>::invokeHelper(
    const ArgumentList& args, MAYBE_UNUSED std::index_sequence<Is...> _sequence) const
{
    return store.makeInstance<Class>(std::forward<Args>(args.get(Is).cast<BareType<Args>>())...);
}