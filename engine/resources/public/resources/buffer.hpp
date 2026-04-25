#pragma once
#include "common.hpp"
#include "vulkan_fwd.hpp"

#include <string>

class VulkanContext;
namespace bb
{

struct Buffer
{
    Buffer() = default;
    ~Buffer();

    Buffer(Buffer&& other) noexcept;
    Buffer& operator=(Buffer&& other) noexcept;

    NON_COPYABLE(Buffer);

    VkBuffer buffer {};
    VmaAllocation allocation {};

    VkDeviceSize size {};
    void* mappedPtr = nullptr;

    std::string name {};
    VulkanContext* _context;
};

}