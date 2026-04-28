#pragma once

#include "slot_map/handle.hpp"

struct GPUImage;
struct Sampler;
struct Buffer;
struct GPUMaterial;
struct GPUModel;
struct GPUMesh;

template <typename T>
using ResourceHandle = bb::SlotHandle<T>;