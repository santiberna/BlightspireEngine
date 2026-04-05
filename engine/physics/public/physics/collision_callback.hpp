#pragma once

#include "wren_entity.hpp"
#include "wren_include.hpp"

#include <functional>
#include <variant>

struct CollisionCallback
{
    CollisionCallback() = default;

    CollisionCallback(std::function<void(WrenEntity, WrenEntity)> func)
    {
        fn = func;
    }

    CollisionCallback(wren::Variable wrenLambda)
    {
        fn = wrenLambda.func("call(_,_)");
    }

private:
    struct CallVisitor
    {
        void operator()(std::monostate) const { };
        void operator()(std::function<void(WrenEntity, WrenEntity)>& fn) const { fn(a, b); };

        void operator()(wren::Method& fn) const
        {
            fn(a, b);
        };

        WrenEntity a {};
        WrenEntity b {};
    };

public:
    void operator()(WrenEntity a, WrenEntity b)
    {
        std::visit(CallVisitor { a, b }, fn);
    }

private:
    using Fn = std::variant<std::monostate, std::function<void(WrenEntity, WrenEntity)>, wren::Method>;
    Fn fn {};
};