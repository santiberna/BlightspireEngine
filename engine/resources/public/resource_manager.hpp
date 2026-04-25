#pragma once

#include "slot_map/handle.hpp"

namespace bb
{
struct Buffer;
}

struct GPUImage;
struct Sampler;
struct GPUMaterial;
struct GPUModel;
struct GPUMesh;

template <typename T>
using ResourceHandle = bb::SlotHandle<T>;