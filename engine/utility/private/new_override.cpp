#include "common.hpp"

#if defined TRACY_ENABLE
#include <tracy/Tracy.hpp>

void* operator new(bb::usize size)
{
    if (auto* ptr = std::malloc(size)) // NOLINT
    {
        TracyAlloc(ptr, size);
        return ptr;
    }
    throw std::bad_alloc();
}

void* operator new[](bb::usize size)
{
    if (auto* ptr = std::malloc(size)) // NOLINT
    {
        TracyAlloc(ptr, size);
        return ptr;
    }
    throw std::bad_alloc();
}

void operator delete(void* ptr) noexcept
{
    TracyFree(ptr);
    std::free(ptr); // NOLINT
}

void operator delete(void* ptr, bb::usize) noexcept
{
    TracyFree(ptr);
    std::free(ptr); // NOLINT
}

void operator delete[](void* ptr) noexcept
{
    TracyFree(ptr);
    std::free(ptr); // NOLINT
}

void operator delete[](void* ptr, bb::usize) noexcept
{
    TracyFree(ptr);
    std::free(ptr); // NOLINT
}
#endif