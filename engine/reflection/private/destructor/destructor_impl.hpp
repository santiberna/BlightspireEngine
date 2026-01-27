#pragma once
#include <destructor/destructor.hpp>

template <typename T>
class DestructorImpl : public Destructor
{
public:
    void invokeDelete(void* mem) const override
    {
        delete static_cast<T*>(mem);
    }
};