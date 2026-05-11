#include "single_time_commands.hpp"
#include "vulkan_context.hpp"

#include <vulkan_helper.hpp>
#include <vulkan_include.hpp>

SingleTimeCommands::SingleTimeCommands(VulkanContext& context)
    : _context(context)
{
    vk::CommandBufferAllocateInfo allocateInfo {};
    allocateInfo.level = vk::CommandBufferLevel::ePrimary;
    allocateInfo.commandPool = _context.CommandPool();
    allocateInfo.commandBufferCount = 1;

    vk::CommandBuffer buffer {};
    vk::Device device = _context.Device();
    util::VK_ASSERT(device.allocateCommandBuffers(&allocateInfo, &buffer), "Failed allocating one time command buffer!");

    vk::Fence fence {};
    vk::FenceCreateInfo fenceInfo {};
    util::VK_ASSERT(device.createFence(&fenceInfo, nullptr, &fence), "Failed creating single time command fence!");

    vk::CommandBufferBeginInfo beginInfo {};
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

    util::VK_ASSERT(buffer.begin(&beginInfo), "Failed beginning one time command buffer!");

    _commandBuffer = buffer;
    _fence = fence;
}

SingleTimeCommands::~SingleTimeCommands()
{
    vk::CommandBuffer commands = _commandBuffer;
    vk::Fence fence = _fence;
    commands.end();

    vk::SubmitInfo submitInfo {};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commands;

    vk::Device device = _context.Device();
    vk::Queue graphics = _context.GraphicsQueue();

    util::VK_ASSERT(graphics.submit(1, &submitInfo, _fence), "Failed submitting one time buffer to queue!");
    util::VK_ASSERT(device.waitForFences(1, &fence, VK_TRUE, std::numeric_limits<bb::u64>::max()), "Failed waiting for fence!");

    device.free(_context.CommandPool(), commands);
    device.destroy(fence);

    assert(_stagingAllocations.size() == _stagingBuffers.size());
    for (bb::usize i = 0; i < _stagingBuffers.size(); ++i)
    {
        util::vmaDestroyBuffer(_context.MemoryAllocator(), _stagingBuffers[i], _stagingAllocations[i]);
    }
}

void SingleTimeCommands::CopyIntoLocalBuffer(const std::byte* vec, bb::u32 count, bb::u32 offset, VkBuffer buffer)
{
    vk::DeviceSize bufferSize = count;

    vk::Buffer stagingBuffer;
    VmaAllocation stagingBufferAllocation;
    util::CreateBuffer(_context, bufferSize, vk::BufferUsageFlagBits::eTransferSrc, stagingBuffer, true, stagingBufferAllocation, VMA_MEMORY_USAGE_CPU_ONLY, "Staging buffer");

    vmaCopyMemoryToAllocation(_context.MemoryAllocator(), vec, stagingBufferAllocation, 0, bufferSize);

    util::CopyBuffer(_commandBuffer, stagingBuffer, buffer, bufferSize, offset);
    TrackAllocation(stagingBufferAllocation, stagingBuffer);
}
