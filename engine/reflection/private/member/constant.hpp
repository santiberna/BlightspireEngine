#pragma once

#include <common.hpp>
#include <string>

class Constant
{
private:
    std::string name {};
    uint64_t value {};

public:
    ~Constant() = default;

    Constant(std::string name, uint64_t value)
        : name(std::move(name))
        , value(value)
    {
    }

    NO_DISCARD uint64_t get() const { return value; }
};