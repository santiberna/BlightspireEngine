#pragma once

#include "common.hpp"

#include <array>

#include <entt/entity/entity.hpp>
#include <glm/mat4x4.hpp>

struct SkeletonNodeComponent
{
    entt::entity parent { entt::null };
    std::array<entt::entity, 7> children {
        entt::null,
        entt::null,
        entt::null,
        entt::null,
        entt::null,
        entt::null,
        entt::null
    };
};

struct SkeletonComponent
{
    entt::entity root { entt::null };
};

struct JointWorldTransformComponent
{
    glm::mat4 world = glm::mat4(1.0f);
};

struct JointSkinDataComponent
{
    glm::mat4 inverseBindMatrix {};
    bb::u32 jointIndex {};
    entt::entity skeletonEntity {};
};

namespace SkeletonHelpers
{
void AttachChild(entt::registry& registry, entt::entity parent, entt::entity child);
void InitializeSkeletonNode(SkeletonNodeComponent& node);
}
