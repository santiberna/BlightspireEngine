#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

struct LineVertex
{
    glm::vec3 position;
};

struct Vertex
{
    glm::vec3 position {};
    glm::vec3 normal {};
    glm::vec4 tangent {};
    glm::vec2 texCoord {};
};

struct SkinnedVertex
{
    glm::vec3 position {};
    glm::vec3 normal {};
    glm::vec4 tangent {};
    glm::vec2 texCoord {};
    glm::vec4 joints {};
    glm::vec4 weights {};
};
