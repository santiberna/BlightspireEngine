#include "resource_management/buffer_resource_manager.hpp"

#include "vulkan_context.hpp"
#include "vulkan_helper.hpp"

BufferResourceManager::BufferResourceManager(const std::shared_ptr<VulkanContext>& context)
    : _context(context)
{
}

ResourceHandle<bb::Buffer> BufferResourceManager::Create(VkDeviceSize size, bb::Flags<bb::BufferFlags> flags, const char* name)
{
    bool mappable = flags.has(bb::BufferFlags::MAPPABLE);

    vk::BufferUsageFlags buffer_usage {};

    if (flags.has(bb::BufferFlags::INDEX_USAGE))
    {
        buffer_usage |= vk::BufferUsageFlagBits::eIndexBuffer;
    }
    if (flags.has(bb::BufferFlags::VERTEX_USAGE))
    {
        buffer_usage |= vk::BufferUsageFlagBits::eVertexBuffer;
    }
    if (flags.has(bb::BufferFlags::STORAGE_USAGE))
    {
        buffer_usage |= vk::BufferUsageFlagBits::eStorageBuffer;
    }
    if (flags.has(bb::BufferFlags::UNIFORM_USAGE))
    {
        buffer_usage |= vk::BufferUsageFlagBits::eUniformBuffer;
    }
    if (flags.has(bb::BufferFlags::TRANSFER_DST))
    {
        buffer_usage |= vk::BufferUsageFlagBits::eTransferDst;
    }
    if (flags.has(bb::BufferFlags::INDIRECT_USAGE))
    {
        buffer_usage |= vk::BufferUsageFlagBits::eIndirectBuffer;
    }

    bb::Buffer out {};
    vk::Buffer vk_type {};

    util::CreateBuffer(*_context,
        size,
        buffer_usage,
        vk_type,
        mappable,
        out.allocation,
        VMA_MEMORY_USAGE_AUTO,
        out.name.c_str());

    out.buffer = vk_type;
    out.size = size;
    out._context = _context.get();
    out.name = name;

    if (mappable)
    {
        util::VK_ASSERT(vmaMapMemory(_context->MemoryAllocator(), out.allocation, &out.mappedPtr),
            "Failed mapping memory for buffer: " + std::string(name));
    }

    return ResourceManager<bb::Buffer>::Create(std::move(out));
}