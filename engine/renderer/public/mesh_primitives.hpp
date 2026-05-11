#pragma once

#include "resources/mesh.hpp"
#include "vertex.hpp"

CPUMesh<Vertex> GenerateUVSphere(bb::u32 slices, bb::u32 stacks, float radius = 1.0f);