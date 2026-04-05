#pragma once

#include "resources/model.hpp"
#include "thread_pool.hpp"

namespace ModelLoading
{
// Loads a GLTF model from the given path to the file.
[[nodiscard]] CPUModel LoadGLTF(std::string_view path);
[[nodiscard]] CPUModel LoadGLTFFast(ThreadPool& scheduler, std::string_view path, bool genCollision);
}
