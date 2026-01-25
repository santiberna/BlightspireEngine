#pragma once
#include <destructor/destructor.hpp>

template <typename T>
class DestructorImpl : public Destructor
{
public:
    void invoke_delete(void* mem) const override
    {
        delete static_cast<T*>(mem);
    }
};