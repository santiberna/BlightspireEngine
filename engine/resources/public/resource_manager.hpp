#pragma once

#include "slot_map/handle.hpp"

namespace bb
{
struct Buffer;
struct Sampler;
}

struct GPUImage;
struct GPUMaterial;
struct GPUModel;
struct GPUMesh;

template <typename T>
using ResourceHandle = bb::SlotHandle<T>;