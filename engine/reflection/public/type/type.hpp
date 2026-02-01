#pragma once

#include <common.hpp>

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <typeindex>
#include <unordered_map>
#include <utility/parameter_list.hpp>

namespace reflect
{
class Value;
class ArgumentList;
namespace detail
{
    class Constructor;
    class ReflectFactory;
    class Destructor;
    class Field;
    class Method;
}

class Type
{
public:
    template <typename T> NO_DISCARD bool is() const;

    NON_COPYABLE(Type);
    DEFAULT_MOVABLE(Type);
    ~Type();

    NO_DISCARD Value construct(const ArgumentList& args) const;
    NO_DISCARD std::optional<uint64_t> getConstant(std::string_view name) const;
    NO_DISCARD bool hasField(std::string_view name) const;
    NO_DISCARD bool hasMethod(std::string_view name) const;
    NO_DISCARD std::type_index getIndex() const;

private:
    Type();

    NO_DISCARD const detail::Field* getField(std::string_view name) const;
    NO_DISCARD const detail::Method* getMethod(std::string_view name) const;

    std::string name {};
    size_t size {};
    const type_info* index {}; // Make into type_index

    // Factory set
    std::optional<std::string> alias {};

    std::unordered_map<std::string, uint64_t> constants {};
    std::unordered_map<std::string, std::unique_ptr<detail::Field>> fields {};
    std::unordered_map<std::string, std::unique_ptr<detail::Method>> methods {};
    std::unordered_map<ParameterList, std::unique_ptr<detail::Constructor>> constructors {};

    friend Value;
    friend detail::ReflectFactory;
    template <typename T> friend class TypeBuilder;
};

template <typename T> bool Type::is() const
{
    static_assert(std::is_same_v<T, std::remove_cvref_t<T>>,
        "Types used for reflection must not have cv-qualifiers or be refs.");
    return this->getIndex() == typeid(T);
}

}