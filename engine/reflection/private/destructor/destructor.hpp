#pragma once
#include <common.hpp>

class Destructor
{
public:
    Destructor() = default;
    virtual ~Destructor() = default;

    NON_COPYABLE(Destructor);
    NON_MOVABLE(Destructor);

    virtual void invokeDelete(void* mem) const = 0;
};

#include <destructor/destructor_impl.hpp>