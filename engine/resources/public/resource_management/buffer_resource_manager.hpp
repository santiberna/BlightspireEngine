#pragma once

#include "common.hpp"
#include "enum_utils.hpp"
#include "resource_manager.hpp"
#include "resources/buffer.hpp"

#include <memory>

class VulkanContext;

namespace bb
{

enum class BufferFlags
{
    TRANSFER_DST = 1 << 0,
    INDEX_USAGE = 1 << 1,
    VERTEX_USAGE = 1 << 2,
    UNIFORM_USAGE = 1 << 3,
    STORAGE_USAGE = 1 << 4,
    INDIRECT_USAGE = 1 << 5,
    MAPPABLE = 1 << 6,
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