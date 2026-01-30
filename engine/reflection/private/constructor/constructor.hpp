#pragma once
#include <common.hpp>

#include <utility/argument_list.hpp>
#include <utility/parameter_list.hpp>

class Instance;
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

    NO_DISCARD virtual Instance invoke(const ArgumentList& parameters) const = 0;

protected:
    TypeStore& store;
    ParameterList parameters {};
};

#include <constructor/constructor_impl.hpp>