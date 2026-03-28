#pragma once
#include "common.hpp"

#include <string_view>

class ECSModule;

class SystemInterface
{
public:
    SystemInterface() = default;
    virtual ~SystemInterface() = default;

    NON_MOVABLE(SystemInterface);
    NON_COPYABLE(SystemInterface);

    virtual void Update([[maybe_unused]] ECSModule& ecs, [[maybe_unused]] float dt) { };
    virtual void Render([[maybe_unused]] const ECSModule& ecs) const { };
    virtual void Inspect() { };
    virtual std::string_view GetName() = 0;
};