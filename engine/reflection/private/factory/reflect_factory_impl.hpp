#pragma once
#include <factory/reflect_factory.hpp>

#include <destructor/destructor.hpp>
#include <type/type.hpp>
#include <value/value.hpp>

template <typename T, typename... Args> Value ReflectFactory::makeValue(Args&&... args)
{
    static_assert(std::is_same_v<T, std::remove_cvref_t<T>>,
        "Types used for reflection must not have cv-qualifiers or be refs.");

    auto value = std::make_shared<T>(std::forward<Args>(args)...);
    return Value { value, get<T>() };
}

template <typename T> const Type* ReflectFactory::get()
{
    // We want const and ref variations of a type to map to the same one
    std::type_index index = typeid(std::remove_cvref_t<T>);

    auto it = type_map.find(index);
    if (it != type_map.end())
    {
        return it->second.get();
    }

    Type type {};

    type.index = &typeid(T);
    type.name = std::string(typeid(T).name());
    type.size = sizeof(T);

    // Initialize new_type here;

    auto unique = std::make_unique<Type>(std::move(type));
    auto [inserted_it, success] = type_map.emplace(index, std::move(unique));
    return inserted_it->second.get();
}

template <typename... Args> NO_DISCARD ParameterList ReflectFactory::asParamaters()
{
    return { { this->get<Args>()... } };
}

template <typename T> NO_DISCARD ValueRef ReflectFactory::makeRef(T&& value)
{
    if constexpr (std::is_same_v<BareType<T>, ValueRef>)
    {
        return value;
    }
    else if constexpr (std::is_same_v<BareType<T>, Value>)
    {
        return value.asRef();
    }
    else
    {
        return ValueRef { std::addressof(value), get<T>() };
    }
}

template <typename... Args> NO_DISCARD ArgumentList ReflectFactory::makeArgs(Args&&... args)
{
    return ArgumentList { { this->makeRef(std::forward<Args>(args))... } };
}