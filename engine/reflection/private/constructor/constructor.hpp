#pragma once
#include <common.hpp>

#include <utility/argument_list.hpp>
#include <utility/parameter_list.hpp>

class Instance;
class TypeStore;
class Constructor
{
public:
    Constructor() = default;
    virtual ~Constructor() = default;
    NON_COPYABLE(Constructor);
    NON_MOVABLE(Constructor);

    NO_DISCARD virtual Instance invoke(TypeStore& type_store, const ArgumentList& parameters) const
        = 0;
    NO_DISCARD virtual Instance emplace(
        TypeStore& type_store, void* mem, const ArgumentList& parameters) const
        = 0;

protected:
    ParameterList parameters {};
};

#include <constructor/constructor_impl.hpp>