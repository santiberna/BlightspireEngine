#include "resources/buffer.hpp"
#include "vulkan_helper.hpp"

bb::Buffer::~Buffer()
{
    if (!_context)
    {
        return;
    }

    if (mappedPtr)
    {
        vmaUnmapMemory(_context->MemoryAllocator(), allocation);
    }

    util::vmaDestroyBuffer(_context->MemoryAllocator(), buffer, allocation);
}

bb::Buffer::Buffer(bb::Buffer&& other) noexcept
    : buffer(other.buffer)
    , allocation(other.allocation)
    , mappedPtr(other.mappedPtr)
    , size(other.size)
    , name(std::move(other.name))
    , _context(other._context)
{
    other.buffer = nullptr;
    other.allocation = nullptr;
    other.mappedPtr = nullptr;
    other._context = nullptr;
}

bb::Buffer& bb::Buffer::operator=(bb::Buffer&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    buffer = other.buffer;
    allocation = other.allocation;
    mappedPtr = other.mappedPtr;
    size = other.size;
    name = std::move(other.name);
    _context = other._context;

    other.buffer = nullptr;
    other.allocation = nullptr;
    other.mappedPtr = nullptr;
    other._context = nullptr;

    return *this;
}
