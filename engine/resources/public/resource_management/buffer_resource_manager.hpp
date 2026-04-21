#pragma once

#include "common.hpp"
#include "resource_manager.hpp"
#include "resources/buffer.hpp"

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

class BufferResourceManager final : public ResourceManager<bb::Buffer>
{
public:
    explicit BufferResourceManager(const std::shared_ptr<VulkanContext>& context);
    ~BufferResourceManager() final = default;

    ResourceHandle<bb::Buffer> Create(VkDeviceSize size, bb::Flags<bb::BufferFlags> flags, const char* name);

private:
    std::shared_ptr<VulkanContext> _context;
};