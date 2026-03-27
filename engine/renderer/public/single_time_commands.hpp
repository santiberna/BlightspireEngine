#pragma once

#include "common.hpp"
#include "vulkan_fwd.hpp"

#include <vector>

class GraphicsContext;
struct Texture;
class VulkanContext;

class SingleTimeCommands
{
public:
    SingleTimeCommands(VulkanContext& context);
    ~SingleTimeCommands();

    void TrackAllocation(VmaAllocation allocation, VkBuffer buffer)
    {
        _stagingAllocations.push_back(allocation);
        _stagingBuffers.push_back(buffer);
    }

    template <typename T>
    void CopyIntoLocalBuffer(const std::vector<T>& vec, uint32_t offset, VkBuffer buffer)
    {
        CopyIntoLocalBuffer(reinterpret_cast<const std::byte*>(vec.data()), vec.size() * sizeof(T), offset * sizeof(T), buffer);
    }
    void CopyIntoLocalBuffer(const std::byte* vec, uint32_t count, uint32_t offset, VkBuffer buffer);

    VkCommandBuffer CommandBuffer() const { return _commandBuffer; }
    VulkanContext& GetContext() const { return _context; }

    NON_MOVABLE(SingleTimeCommands);
    NON_COPYABLE(SingleTimeCommands);

private:
    VulkanContext& _context;
    VkFence _fence;
    VkCommandBuffer _commandBuffer;
    std::vector<VkBuffer> _stagingBuffers;
    std::vector<VmaAllocation> _stagingAllocations;
};