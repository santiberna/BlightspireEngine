#pragma once

#include <common.hpp>
#include <vector>

class Type;
class Instance;

class ArgumentList
{
public:
    ArgumentList(std::initializer_list<Instance*> init)
        : values(init)
    {
    }

    explicit ArgumentList(std::vector<Instance*> init)
        : values(std::move(init))
    {
    }

    NO_DISCARD std::vector<const Type*> asTypes() const;
    NO_DISCARD Instance* get(size_t i) const;

private:
    std::vector<Instance*> values;
};