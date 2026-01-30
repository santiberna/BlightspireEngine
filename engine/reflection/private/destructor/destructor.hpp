#pragma once
#include <common.hpp>

/// TODO: currently unused, possibly useful for "fake" custom types.
class Destructor
{
public:
    Destructor() = default;
    virtual ~Destructor() = default;

    NON_COPYABLE(Destructor);
    NON_MOVABLE(Destructor);

    virtual void operator()(void* mem) const = 0;
};