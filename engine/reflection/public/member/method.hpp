#pragma once
#include <common.hpp>

#include <utility/argument_list.hpp>
#include <utility/parameter_list.hpp>

namespace reflect
{
class Value;
class ValueRef;

namespace detail
{
    class ReflectFactory;

    class Method
    {
    public:
        Method(ReflectFactory& store)
            : store(store)
        {
        }

        virtual ~Method() = default;
        NON_COPYABLE(Method);
        NON_MOVABLE(Method);

        template <typename Class, typename Ret, typename... Args>
        using MemberPointer = Ret (Class::*)(Args...);

        template <typename Class, typename Ret, typename... Args>
        using ConstMemberPointer = Ret (Class::*)(Args...) const;

        NO_DISCARD virtual Value invoke(ValueRef object, const ArgumentList& parameters) const = 0;

    protected:
        ReflectFactory& store;
        ParameterList parameters {};
    };
}
}