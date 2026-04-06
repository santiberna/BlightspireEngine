#pragma once

#include "wren_include.hpp"

#include <variant>

struct Callback
{
    Callback() = default;

    Callback(std::function<void()> func)
    {
        fn = func;
    }

    Callback(wren::Variable wrenLambda)
    {
        fn = wrenLambda.func("call()");
    }

private:
    struct CallVisitor
    {
        void operator()(std::monostate) const { };
        void operator()(std::function<void()>& fn) const { fn(); };

        void operator()(wren::Method& fn) const
        {
            fn();
        };
    };

public:
    void operator()()
    {
        std::visit(CallVisitor {}, fn);
    }

private:
    using Fn = std::variant<std::monostate, std::function<void()>, wren::Method>;
    Fn fn {};
};