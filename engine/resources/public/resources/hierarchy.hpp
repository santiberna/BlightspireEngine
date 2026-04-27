#pragma once
#include "components/animation_channel_component.hpp"
#include "resources/mesh.hpp"

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

enum class NodeLightType
{
    Directional,
    Point,
    Spot
};

struct NodeLightData
{
    static constexpr float DEFAULT_LIGHT_RANGE = 16.f;

    glm::vec3 color {};
    NodeLightType type {};
    float range {};
    float intensity {};
};

struct NodeMeshData
{
    MeshType type {};
    bb::u32 index {};
};

struct NodeJointData
{
    bb::u32 index {};
    glm::mat4 inverseBind = glm::mat4(1.0f);
};

struct NodePhysicsData
{
    bb::u32 colliderIndex {};
};

struct Node
{
    std::string name {};
    glm::mat4 transform { 1.0f };
    std::vector<bb::u32> childrenIndices {};

    std::optional<NodeMeshData> mesh {};
    std::optional<NodeLightData> light {};
    std::optional<NodeJointData> joint {};
    std::optional<NodePhysicsData> physics {};

    std::unordered_map<bb::u32, TransformAnimationSpline> animationSplines {};
    std::optional<bb::u32> skeletonNode {};
};

struct Hierarchy
{
    bb::u32 root {};
    std::vector<Node> nodes {};

    std::optional<bb::u32> skeletonRoot = std::nullopt;
};
