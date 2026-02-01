#pragma once
#include <common.hpp>
#include <memory>
#include <optional>
#include <reflect_fwd.hpp>
#include <string>
#include <string_view>
#include <typeindex>
#include <unordered_map>
#include <utility/parameter_list.hpp>
#include <utility/string_hashmap.hpp>
namespace reflect
{

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

    detail::StringHashmap<uint64_t> constants {};
    detail::StringHashmap<std::unique_ptr<detail::Field>> fields {};
    detail::StringHashmap<std::unique_ptr<detail::Method>> methods {};
    std::unordered_map<ParameterList, std::unique_ptr<detail::Constructor>> constructors {};

    friend Value;
    friend ValueRef;
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