#pragma once

#include <common.hpp>
#include <vector>

class Type;
class Instance;

class ArgumentList
{
public:
    NO_DISCARD std::vector<const Type*> asTypes() const;
    std::vector<Instance*> values;
};