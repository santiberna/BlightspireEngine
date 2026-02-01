#pragma once

#include <common.hpp>
#include <vector>

namespace reflect
{

class Type;
class ArgumentList;

class ParameterList
{
public:
    ParameterList() = default;

    ParameterList(std::vector<const Type*> types)
        : types(std::move(types))
    {
    }

    NO_DISCARD size_t length() const { return types.size(); }
    NO_DISCARD bool validateArgs(const ArgumentList& args) const;
    bool operator==(const ParameterList& other) const { return types == other.types; }

private:
    std::vector<const Type*> types;
    friend class std::hash<ParameterList>;
};

// Specialize std::hash for ParameterList

}

namespace std
{
template <> struct hash<reflect::ParameterList>
{
    std::size_t operator()(const reflect::ParameterList& params) const noexcept;
};
}