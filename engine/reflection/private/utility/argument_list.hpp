#pragma once

#include <common.hpp>
#include <instance/instance_ref.hpp>
#include <vector>

class Type;
class InstanceRef;

class ArgumentList
{
public:
    ArgumentList() = default;

    ArgumentList(std::vector<InstanceRef> values)
        : values(std::move(values))
    {
    }

    NO_DISCARD std::vector<const Type*> asTypes() const;
    NO_DISCARD InstanceRef get(size_t i) const;

private:
    std::vector<InstanceRef> values;
};