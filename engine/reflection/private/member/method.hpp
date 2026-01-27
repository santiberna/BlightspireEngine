#pragma once
#include <common.hpp>
#include <string>

#include <utility/argument_list.hpp>
#include <utility/parameter_list.hpp>

class Instance;
class TypeStore;

class Method
{
public:
    template <typename Class, typename Ret, typename... Args>
    using MemberPointer = Ret (Class::*)(Args...);

    template <typename Class, typename Ret, typename... Args>
    using ConstMemberPointer = Ret (Class::*)(Args...) const;

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