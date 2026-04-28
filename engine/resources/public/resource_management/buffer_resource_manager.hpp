#pragma once

#include "resource_manager.hpp"
#include "resources/buffer.hpp"
#include "slot_map/container.hpp"

#include <memory>

class VulkanContext;

class BufferResourceManager
{
public:
    explicit BufferResourceManager(const std::shared_ptr<VulkanContext>& context);

    ResourceHandle<Buffer> Create(const BufferCreation& creation);

    const Buffer* Access(const ResourceHandle<Buffer>& handle) const
    {
        return m_storage.get(handle);
    }

    void Destroy(const ResourceHandle<Buffer>& handle)
    {
        m_storage.remove(handle);
    }

private:
    bb::SlotMap<Buffer> m_storage {};
    std::shared_ptr<VulkanContext> _context;
};