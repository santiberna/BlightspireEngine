#pragma once
#include <vector>

#include "math.hpp"
#include "resources/material.hpp"

enum class MeshType : bb::u8
{
    eSTATIC,
    eSKINNED,
};

template <typename T>
struct CPUMesh
{
    std::vector<T> vertices;
    std::vector<bb::u32> indices;
    bb::u32 materialIndex { 0 };

    math::Vec3Range boundingBox;
    float boundingRadius;
};

struct GPUMesh
{
    bb::u32 count { 0 };
    bb::u32 vertexOffset { 0 };
    bb::u32 indexOffset { 0 };
    float boundingRadius;
    math::Vec3Range boundingBox;

    MeshType type;
    ResourceHandle<GPUMaterial> material;
};