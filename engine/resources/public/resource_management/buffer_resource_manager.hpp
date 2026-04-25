#pragma once

#include "resource_manager.hpp"
#include "resources/buffer.hpp"
#include "slot_map/container.hpp"

#include "enum_utils.hpp"
#include "vma_include.hpp"
#include "vulkan_include.hpp"

#include <memory>

class VulkanContext;

namespace bb
{

enum class BufferFlags
{
    TRANSFER_DST,
    INDEX_USAGE,
    VERTEX_USAGE,
    UNIFORM_USAGE,
    STORAGE_USAGE,
    INDIRECT_USAGE,
    MAPPABLE,
};

}

class BufferResourceManager
{
public:
    explicit BufferResourceManager(const std::shared_ptr<VulkanContext>& context);

    ResourceHandle<bb::Buffer> Create(VkDeviceSize size, bb::Flags<bb::BufferFlags> flags, const char* name);

    const bb::Buffer* Access(const ResourceHandle<bb::Buffer>& handle) const
    {
        return m_storage.get(handle);
    }

    void Destroy(const ResourceHandle<bb::Buffer>& handle)
    {
        m_storage.remove(handle);
    }

private:
    bb::SlotMap<bb::Buffer> m_storage {};
    std::shared_ptr<VulkanContext> _context;
};