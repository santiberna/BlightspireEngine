#pragma once

#include <common.hpp>
#include <value/value_ref.hpp>
#include <vector>

class Type;
class ValueRef;

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