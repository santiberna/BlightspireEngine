#pragma once
#include <common.hpp>

#include <utility/argument_list.hpp>
#include <utility/parameter_list.hpp>

class Value;
class TypeStore;

class Constructor
{
public:
    Constructor(TypeStore& store)
        : store(store)
    {
    }

    virtual ~Constructor() = default;
    NON_COPYABLE(Constructor);
    NON_MOVABLE(Constructor);

    NO_DISCARD virtual Value invoke(const ArgumentList& parameters) const = 0;

protected:
    TypeStore& store;
    ParameterList parameters {};
};
