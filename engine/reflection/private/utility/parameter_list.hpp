#pragma once

#include <common.hpp>
#include <vector>

class Type;
class ArgumentList;

class ParameterList
{
public:
    ParameterList() = default;

    ParameterList(std::initializer_list<const Type*> init)
        : types(init)
    {
    }

    explicit ParameterList(std::vector<const Type*> init)
        : types(std::move(init))
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
template <> struct std::hash<ParameterList>
{
    std::size_t operator()(const ParameterList& params) const noexcept;
};