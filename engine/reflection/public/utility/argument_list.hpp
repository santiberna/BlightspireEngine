#pragma once

#include <common.hpp>
#include <reflect_fwd.hpp>
#include <value/value_ref.hpp>
#include <vector>

namespace reflect
{

class ArgumentList
{
public:
    ArgumentList() = default;

    ArgumentList(std::vector<ValueRef> values)
        : values(std::move(values))
    {
    }

    NO_DISCARD std::vector<const Type*> asTypes() const;
    NO_DISCARD ValueRef get(size_t i) const;

private:
    std::vector<ValueRef> values;
};
}