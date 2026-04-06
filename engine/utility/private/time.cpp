#include "time.hpp"

#include <chrono>

bb::Stopwatch::Stopwatch()
{
    auto val = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    nano_start = { val };
}

bb::NanosecondsI64 bb::Stopwatch::getElapsed() const
{
    auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    return { now - nano_start.value };
}