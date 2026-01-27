#pragma once
#include <types.hpp>

class Method
{
public:
    std::string name {};
    ParameterList parameters {};

    Method() = default;
    virtual ~Method() = default;
    NON_COPYABLE(Method);
    NON_MOVABLE(Method);

    NO_DISCARD virtual Instance invoke(
        TypeStore& type_store, Instance& object, const ArgumentList& parameters) const
        = 0;
};

#include <member/method_impl.hpp>