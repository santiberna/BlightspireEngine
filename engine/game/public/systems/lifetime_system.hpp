#pragma once

#include "system_interface.hpp"

class LifetimeSystem final : public SystemInterface
{
public:
    LifetimeSystem() = default;
    ~LifetimeSystem() override = default;
    NON_COPYABLE(LifetimeSystem);
    NON_MOVABLE(LifetimeSystem);

    void Update(ECSModule& ecs, [[maybe_unused]] float dt) override;
    void Render([[maybe_unused]] const ECSModule& ecs) const override { }
    void Inspect() override;

    std::string_view GetName() override { return "LifetimeSystem"; }
};