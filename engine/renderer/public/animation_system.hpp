#pragma once

#include "system_interface.hpp"

#include <entt/entt.hpp>
#include <glm/mat4x4.hpp>

struct SkeletonComponent;
class RendererModule;

class AnimationSystem final : public SystemInterface
{
public:
    AnimationSystem() = default;
    ~AnimationSystem() override;

    void Update(ECSModule& ecs, float dt) override;
    void Render(const ECSModule& ecs) const override;
    void Inspect() override;

    std::string_view GetName() override { return "AnimationSystem"; }

private:
    void RecursiveCalculateMatrix(entt::entity entity, const glm::mat4& parentMatrix, ECSModule& ecs, const SkeletonComponent& skeleton);
};