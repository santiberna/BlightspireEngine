#pragma once

#include "slot_map/container.hpp"

#include "enum_utils.hpp"
#include "resources/image.hpp"
#include "resources/texture.hpp"
#include "single_time_commands.hpp"
#include "slot_map/container.hpp"

#include <memory>

class VulkanContext;

namespace bb
{

enum class TextureFlags : uint8_t
{
    SAMPLED = 1 << 0,
    TRANSFER_SRC = 1 << 1,
    TRANSFER_DST = 1 << 2,
    COLOR_ATTACH = 1 << 3,
    DEPTH_ATTACH = 1 << 4,
    GEN_MIPMAPS = 1 << 5,
    STORAGE_ACCESS = 1 << 6,

    COMMON_FLAGS = SAMPLED | TRANSFER_SRC | TRANSFER_DST | GEN_MIPMAPS,
};

}

class ImageResourceManager
{
public:
    ImageResourceManager(const std::shared_ptr<VulkanContext>& context, ResourceHandle<bb::Sampler> defaultSampler)
        : _defaultSampler(defaultSampler)
        , _context(context)
    {
    }

    ResourceHandle<GPUImage> Create(
        const bb::Image2D& image,
        ResourceHandle<bb::Sampler> sampler,
        bb::Flags<bb::TextureFlags> flags,
        std::string_view name,
        SingleTimeCommands* upload_commands);

    ResourceHandle<GPUImage> Create(
        const bb::Cubemap& cubemap,
        ResourceHandle<bb::Sampler> sampler,
        bb::Flags<bb::TextureFlags> flags,
        std::string_view name);

    // TODO remove this
    ResourceHandle<GPUImage> Create(
        const bb::Image2D& image,
        bb::Flags<bb::TextureFlags> flags,
        std::string_view name,
        SingleTimeCommands* upload_commands)
    {
        return Create(image, _defaultSampler, flags, name, upload_commands);
    }

    // TODO remove this
    ResourceHandle<GPUImage> Create(
        const bb::Image2D& image,
        bb::Flags<bb::TextureFlags> flags,
        std::string_view name)
    {
        SingleTimeCommands commands { *_context };
        return Create(image, _defaultSampler, flags, name, &commands);
    }

    ResourceHandle<bb::Sampler> _defaultSampler;

    const GPUImage* Access(const ResourceHandle<GPUImage>& handle) const
    {
        return m_storage.get(handle);
    }

    void Destroy(const ResourceHandle<GPUImage>& handle)
    {
        m_storage.remove(handle);
    }

    const bb::SlotMap<GPUImage>& Resources() { return m_storage; }

private:
    bb::SlotMap<GPUImage> m_storage {};
    std::shared_ptr<VulkanContext> _context;
};
