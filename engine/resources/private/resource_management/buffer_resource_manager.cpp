#include "resource_management/buffer_resource_manager.hpp"

#include "vulkan_context.hpp"

BufferResourceManager::BufferResourceManager(const std::shared_ptr<VulkanContext>& context)
    : _context(context)
{
}

ResourceHandle<Buffer> BufferResourceManager::Create(const BufferCreation& creation)
{
    return m_storage.insert(Buffer { creation, _context });
}