#pragma once
#include <constructor/constructor.hpp>
#include <factory/reflect_factory.hpp>
#include <stdexcept>
#include <utility/traits.hpp>
#include <value/value.hpp>

namespace reflect::detail
{

template <typename Class, typename... Args> class ConstructorImpl : public Constructor
{
public:
    ConstructorImpl(ReflectFactory& type_store);
    NO_DISCARD Value invoke(const ArgumentList& parameters) const override;

private:
    template <std::size_t... Is>
    NO_DISCARD Value invokeHelper(
        const ArgumentList& args, std::index_sequence<Is...> _sequence) const;
};

template <typename Class, typename... Args>
ConstructorImpl<Class, Args...>::ConstructorImpl(ReflectFactory& type_store)
    : Constructor(type_store)
{
    this->parameters = ParameterList { { type_store.get<Args>()... } };
}

template <typename Class, typename... Args>
Value ConstructorImpl<Class, Args...>::invoke(const ArgumentList& args) const
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
NO_DISCARD Value ConstructorImpl<Class, Args...>::invokeHelper(
    const ArgumentList& args, MAYBE_UNUSED std::index_sequence<Is...> _sequence) const
{
    return store.makeValue<Class>(std::forward<Args>(args.get(Is).cast<BareType<Args>>())...);
}

}