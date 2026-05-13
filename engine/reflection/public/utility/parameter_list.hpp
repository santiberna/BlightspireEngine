#pragma once
#include <common.hpp>
#include <reflection_fwd.hpp>
#include <vector>

namespace reflection
{
class ParameterList
{
public:
    ParameterList() = default;

    ParameterList(std::vector<const Type*> types)
        : types(std::move(types))
    {
    }

    [[nodiscard]] bb::usize length() const { return types.size(); }
    [[nodiscard]] bool validateArgs(const ArgumentList& args) const;
    bool operator==(const ParameterList& other) const { return types == other.types; }

private:
    std::vector<const Type*> types;
    friend class std::hash<ParameterList>;
};

// Specialize std::hash for ParameterList

}

namespace std
{
template <>
struct hash<reflection::ParameterList>
{
    bb::usize operator()(const reflection::ParameterList& params) const noexcept;
};
}