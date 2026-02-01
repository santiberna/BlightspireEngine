#pragma once
#include <common.hpp>

#include <memory>
#include <typeindex>
#include <unordered_map>

#include <reflect_fwd.hpp>
#include <utility/argument_list.hpp>
#include <utility/parameter_list.hpp>

namespace reflect::detail
{
class ReflectFactory
{
public:
    ReflectFactory() = default;
    ~ReflectFactory() = default;

    DEFAULT_MOVABLE(ReflectFactory);
    NON_COPYABLE(ReflectFactory);

    template <typename T> NO_DISCARD const Type* get();
    template <typename T, typename... Args> NO_DISCARD Value makeValue(Args&&... args);
    template <typename T> NO_DISCARD ValueRef makeRef(T&& value);
    template <typename... Args> NO_DISCARD ParameterList asParamaters();
    template <typename... Args> NO_DISCARD ArgumentList makeArgs(Args&&... args);

private:
    template <typename T> friend class ::reflect::TypeBuilder;
    template <typename T> NO_DISCARD Type* getMut() { return const_cast<Type*>(this->get<T>()); };

    std::unordered_map<std::type_index, std::unique_ptr<Type>> type_map;
};
}

#include <factory/reflect_factory_impl.hpp>