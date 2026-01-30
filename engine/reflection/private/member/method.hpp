#pragma once
#include <common.hpp>

#include <utility/argument_list.hpp>
#include <utility/parameter_list.hpp>

class Instance;
class TypeStore;

/// TODO: const methods and methods should have a different base
// So they can be stored separately and make use of const in Instance
class Method
{
public:
    Method(TypeStore& store)
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

    NO_DISCARD virtual Instance invoke(Instance& object, const ArgumentList& parameters) const = 0;

protected:
    TypeStore& store;
    ParameterList parameters {};
};

#include <member/method_impl.hpp>