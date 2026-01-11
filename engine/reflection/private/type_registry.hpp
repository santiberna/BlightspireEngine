#include <common.hpp>
#include <string>
#include <string_view>
#include <typeindex>
#include <unordered_map>

#include <names.hpp>

struct TypeDescription
{
};

class TypeRegistry
{
public:
    TypeRegistry() = default;

    NON_COPYABLE(TypeRegistry);
    NON_MOVABLE(TypeRegistry);

    template <typename T>
    void newType()
    {
        auto index = std::type_index { typeid(T) };
        auto type_name = sanitizeTypeName(index.name());
        type_to_description[index] = TypeDescription {};
        name_to_type.emplace(std::string(type_name), index);
    }

    template <typename T>
    bool containsType()
    {
        auto index = std::type_index { typeid(T) };
        return type_to_description.contains(index);
    }

    bool containsTypeNamed(std::string_view name)
    {
        return name_to_type.contains(std::string(name));
    }

private:
    std::unordered_map<std::type_index, TypeDescription> type_to_description {};
    std::unordered_map<std::string, std::type_index> name_to_type {};
};