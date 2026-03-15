#pragma once
#include <common.hpp>
#include <memory>
#include <reflection_fwd.hpp>
#include <typeindex>
#include <unordered_map>
#include <utility/argument_list.hpp>
#include <utility/parameter_list.hpp>

namespace reflection
{
class ReflectFactory
{
public:
    ReflectFactory() = default;
    ~ReflectFactory() = default;

    DEFAULT_MOVABLE(ReflectFactory);
    NON_COPYABLE(ReflectFactory);

    template <typename T>
    [[nodiscard]] const Type* get();
    template <typename T, typename... Args>
    [[nodiscard]] Value makeValue(Args&&... args);
    template <typename T>
    [[nodiscard]] ValueRef makeRef(T&& value);
    template <typename... Args>
    [[nodiscard]] ParameterList asParamaters();
    template <typename... Args>
    [[nodiscard]] ArgumentList makeArgs(Args&&... args);

private:
    template <typename T>
    friend class ::reflection::TypeBuilder;
    template <typename T>
    [[nodiscard]] Type* getMut() { return const_cast<Type*>(this->get<T>()); };

    std::unordered_map<std::type_index, std::unique_ptr<Type>> type_map;
};
}

#include <factory/reflect_factory_impl.hpp>