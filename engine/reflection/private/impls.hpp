#pragma once

#include <cassert>
#include <stdexcept>
#include <types.hpp>

/// Template impls

template <typename T>
Instance::Instance(TypeStore& type_store, T val)
{
    this->ptr = new T(std::move(val));
    this->type = type_store.get<T>();
}

template <typename T>
T* Instance::cast()
{
    if (this->type->is<T>())
    {
        return static_cast<T*>(this->ptr);
    }
    return nullptr;
}

template <typename T>
const T* Instance::cast() const
{
}

template <typename T>
class DestructorImpl : public Destructor
{
public:
    void invoke(void* mem) const override
    {
        delete static_cast<T*>(mem);
    }
};

template <typename T>
bool Type::is() const
{
    return this->getIndex() == typeid(T);
}

template <typename Ret, typename Class, typename... Args>
class MethodImpl : public MemberMethod
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
    : MemberMethod()
{
    this->name = "Unimplemented!";
    this->callable = ptr;
    this->parameters = ParameterList { { type_store.get<Args>()... } };
}

template <typename Ret, typename Class, typename... Args>
Instance MethodImpl<Ret, Class, Args...>::invoke(TypeStore& type_store, Instance& object, const ArgumentList& args) const
{
    if (args.values.size() != this->parameters.types.size())
    {
        throw std::runtime_error("Parameter count mismatch");
    }

    // Get the object as Class*
    Class* obj = object.cast<Class>();
    if (obj == nullptr)
    {
        throw std::runtime_error("Invalid object type");
    }

    for (size_t i = 0; i < args.values.size(); i++)
    {
        auto* arg = args.values[i];
        auto* param = this->parameters.types[i];

        if (!arg->isType(*param))
        {
            throw std::runtime_error("Invalid argument type");
        }
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

template <typename T>
Type* TypeStore::get()
{
    std::type_index index = typeid(T);

    auto it = type_map.find(index);
    if (it != type_map.end())
    {
        return it->second.get();
    }

    Type type {};

    type.index = &typeid(T);
    type.name = std::string(typeid(T).name());
    type.destructor = std::make_unique<DestructorImpl<T>>();
    type.size = sizeof(T);

    // Initialize new_type here;

    auto unique = std::make_unique<Type>(std::move(type));
    auto [inserted_it, success] = type_map.emplace(index, std::move(unique));
    return inserted_it->second.get();
}

template <typename Class, typename... Args>
class ConstructorImpl : public Constructor
{
public:
    ConstructorImpl(TypeStore& type_store);
    NO_DISCARD Instance invoke(TypeStore& type_store, const ArgumentList& parameters) const override;

private:
    template <std::size_t... Is>
    NO_DISCARD Instance invokeHelper(TypeStore& type_store, const ArgumentList& args, std::index_sequence<Is...>) const;
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

    if (args.values.size() != ARG_SIZE)
    {
        throw std::runtime_error("Parameter count mismatch");
    }

    for (size_t i = 0; i < args.values.size(); i++)
    {
        auto* arg = args.values[i];
        auto* param = this->parameters.types[i];

        if (!arg->isType(*param))
        {
            throw std::runtime_error("Invalid argument type");
        }
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