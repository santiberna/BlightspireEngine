#pragma once
#include <common.hpp>

#include <reflect_fwd.hpp>
#include <utility/argument_list.hpp>
#include <utility/parameter_list.hpp>

namespace reflect::detail
{
class Constructor
{
public:
    Constructor(ReflectFactory& store)
        : store(store)
    {
    }

    virtual ~Constructor() = default;
    NON_COPYABLE(Constructor);
    NON_MOVABLE(Constructor);

    NO_DISCARD virtual Value invoke(const ArgumentList& parameters) const = 0;

protected:
    ReflectFactory& store;
    ParameterList parameters {};
};
}