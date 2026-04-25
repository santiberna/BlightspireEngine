#include "resource_management/buffer_resource_manager.hpp"

#include "vma_include.hpp"
#include "vulkan_context.hpp"
#include "vulkan_include.hpp"

#include <magic_enum.hpp>
#include <spdlog/spdlog.h>
#include <tracy/Tracy.hpp>

namespace
{

#ifdef TRACY_ENABLE
constexpr const char* BUFFER_MEMORY_LABEL = "GPU Buffer";
#endif

void releaseBuffer(VmaAllocator allocator, const bb::Buffer& buffer)
{
    if (buffer.mappedPtr != nullptr)
    {
        vmaUnmapMemory(allocator, buffer.allocation);
    }

#ifdef TRACY_ENABLE
    TracyFreeN(buffer.allocation, BUFFER_MEMORY_LABEL);
#endif

    vmaDestroyBuffer(allocator, buffer.buffer, buffer.allocation);
}

}

BufferResourceManager::BufferResourceManager(const std::shared_ptr<VulkanContext>& context)
    : _context(context)
{
}

BufferResourceManager::~BufferResourceManager()
{
    for (auto [handle, buffer] : m_storage)
    {
        releaseBuffer(_context->MemoryAllocator(), buffer);
    }
}

void BufferResourceManager::Destroy(ResourceHandle<bb::Buffer> handle)
{
    if (auto* buffer = m_storage.get(handle))
    {
        releaseBuffer(_context->MemoryAllocator(), *buffer);
        m_storage.remove(handle);
    }
}

ResourceHandle<bb::Buffer> BufferResourceManager::Create(VkDeviceSize size, bb::Flags<bb::BufferFlags> flags, const char* name)
{
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
    auto queue_families = _context->QueueFamilies();

    vk::BufferCreateInfo bufferInfo {};
    bufferInfo.size = size;
    bufferInfo.usage = buffer_usage;
    bufferInfo.sharingMode = vk::SharingMode::eExclusive;
    bufferInfo.queueFamilyIndexCount = 1;
    bufferInfo.pQueueFamilyIndices = &queue_families.graphicsFamily.value();

    VmaAllocationCreateInfo allocationCreateInfo {};
    allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;

    if (flags.has(bb::BufferFlags::MAPPABLE))
    {
        allocationCreateInfo.flags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
    }

    VmaAllocationInfo allocInfo {};
    vk::Result result = static_cast<vk::Result>(vmaCreateBuffer(
        _context->MemoryAllocator(),
        bufferInfo,
        &allocationCreateInfo,
        &out.buffer,
        &out.allocation,
        &allocInfo));

    if (result != vk::Result::eSuccess)
    {
        spdlog::warn("Failed to allocate buffer {}: {}", name, magic_enum::enum_name(result));
        return {};
    }

#ifdef TRACY_ENABLE
    TracyAllocN(out.allocation, allocInfo.size, BUFFER_MEMORY_LABEL);
#endif

#if BB_DEVELOPMENT
    vmaSetAllocationName(_context->MemoryAllocator(), out.allocation, name);
    _context->DebugSetObjectName(vk::Buffer(out.buffer), name);
#endif

    out.size = size;
    out.name = name;

    if (flags.has(bb::BufferFlags::MAPPABLE))
    {
        vk::Result result = static_cast<vk::Result>(vmaMapMemory(
            _context->MemoryAllocator(),
            out.allocation,
            &out.mappedPtr));

        if (result != vk::Result::eSuccess)
        {
            spdlog::warn("Failed mapping memory for buffer {}: {}", name, magic_enum::enum_name(result));
        }
    }

    return m_storage.insert(std::move(out));
}