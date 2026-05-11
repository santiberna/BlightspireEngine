#pragma once
#include "common.hpp"

#include <glm/glm.hpp>

struct alignas(16) QuadDrawInfo
{
    glm::mat4 matrix {};
    glm::vec4 color = { 1.f, 1.f, 1.f, 1.f };
    glm::vec2 uvMin = { 0.f, 0.f };
    glm::vec2 uvMax = { 1.f, 1.f };
    bb::u32 textureIndex { 0 };
    bb::u32 useRedAsAlpha = false;
};
