#pragma once
#include <common.hpp>
#include <reflection_fwd.hpp>
#include <value/value_ref.hpp>
#include <vector>

namespace reflection
{

class ArgumentList
{
public:
    ArgumentList() = default;

    ArgumentList(std::vector<ValueRef> values)
        : values(std::move(values))
    {
    }

    [[nodiscard]] std::vector<const Type*> asTypes() const;
    [[nodiscard]] ValueRef get(bb::usize i) const;

private:
    std::vector<ValueRef> values;
};
}