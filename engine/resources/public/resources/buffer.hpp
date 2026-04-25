#pragma once
#include "enum_utils.hpp"
#include "resource_manager.hpp"
#include "slot_map/container.hpp"
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

enum class BufferFlags
{
    NONE = 0,
    TRANSFER_DST = 1 << 0,
    INDEX_USAGE = 1 << 1,
    VERTEX_USAGE = 1 << 2,
    UNIFORM_USAGE = 1 << 3,
    STORAGE_USAGE = 1 << 4,
    INDIRECT_USAGE = 1 << 5,
    MAPPABLE = 1 << 6,
};

class BufferManager
{
public:
    explicit BufferManager(const VulkanContext& context);
    ~BufferManager();

    [[nodiscard]] ResourceHandle<bb::Buffer> Create(VkDeviceSize size, bb::Flags<bb::BufferFlags> flags, const char* name);
    [[nodiscard]] const bb::Buffer* Access(const ResourceHandle<bb::Buffer>& handle) const
    {
        return m_storage.get(handle);
    }

    void Destroy(ResourceHandle<bb::Buffer> handle);

private:
    bb::SlotMap<bb::Buffer> m_storage {};
    const VulkanContext& m_context;
};

}