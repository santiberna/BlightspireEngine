#pragma once
#include <constructor/constructor.hpp>
#include <stdexcept>

template <typename Class, typename... Args>
class ConstructorImpl : public Constructor
{
public:
    ConstructorImpl(TypeStore& type_store);
    NO_DISCARD Instance invoke(TypeStore& type_store, const ArgumentList& parameters) const override;
    NO_DISCARD Instance emplace(TypeStore& type_store, void* mem, const ArgumentList& parameters) const override;

private:
    template <std::size_t... Is>
    NO_DISCARD Instance invokeHelper(TypeStore& type_store, const ArgumentList& args, std::index_sequence<Is...>) const;

    template <std::size_t... Is>
    NO_DISCARD Instance emplaceHelper(TypeStore& type_store, void* mem, const ArgumentList& args, std::index_sequence<Is...>) const;
};

template <typename Class, typename... Args>
ConstructorImpl<Class, Args...>::ConstructorImpl(TypeStore& type_store)
    : Constructor()
{
    this->parameters = ParameterList { { type_store.get<Args>()... } };
}

template <typename Class, typename... Args>
Instance ConstructorImpl<Class, Args...>::invoke(TypeStore& type_store, const ArgumentList& args) const
{
    constexpr auto ARG_SIZE = sizeof...(Args);
    assert(this->parameters.types.size() == ARG_SIZE);

    if (!this->parameters.validateArgs(args))
    {
        throw std::runtime_error("Parameter type or count mismatch");
    }

    return invokeHelper(type_store, args, std::index_sequence_for<Args...> {});
}

template <typename Class, typename... Args>
template <std::size_t... Is>
NO_DISCARD Instance ConstructorImpl<Class, Args...>::invokeHelper(TypeStore& type_store, const ArgumentList& args, std::index_sequence<Is...>) const
{
    Class val = Class { *(*args.values[Is]).cast<Args>()... };
    return Instance(type_store, std::move(val));
}

template <typename Class, typename... Args>
Instance ConstructorImpl<Class, Args...>::emplace(TypeStore& type_store, void* mem, const ArgumentList& args) const
{
    constexpr auto ARG_SIZE = sizeof...(Args);
    assert(this->parameters.types.size() == ARG_SIZE);

    if (!this->parameters.validateArgs(args))
    {
        throw std::runtime_error("Parameter type or count mismatch");
    }

    return emplaceHelper(type_store, mem, args, std::index_sequence_for<Args...> {});
}

template <typename Class, typename... Args>
template <std::size_t... Is>
NO_DISCARD Instance ConstructorImpl<Class, Args...>::emplaceHelper(TypeStore& type_store, void* mem, const ArgumentList& args, std::index_sequence<Is...>) const
{
    new (mem) Class { *(*args.values[Is]).cast<Args>()... };
    auto type = type_store.get<Class>();
    return Instance(mem, type);
}