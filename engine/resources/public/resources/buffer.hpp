#pragma once
#include "common.hpp"
#include "vulkan_fwd.hpp"

#include <string>

class VulkanContext;
namespace bb
{

struct Buffer
{
    VkBuffer buffer {};
    VmaAllocation allocation {};
    VkDeviceSize size {};
    std::string name {};
    void* mappedPtr = nullptr;
};

}