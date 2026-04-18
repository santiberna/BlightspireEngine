#pragma once

#include "resources/model.hpp"

class ThreadPool;

namespace ModelLoading
{
[[nodiscard]] CPUModel LoadGLTF(ThreadPool* scheduler, std::string_view path, bool genCollision);
}
