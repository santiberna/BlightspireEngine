#pragma once

#include "slot_map/container.hpp"

#include "enum_utils.hpp"
#include "resources/image.hpp"
#include "resources/texture.hpp"
#include "single_time_commands.hpp"

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

class ImageResourceManager : public bb::SlotMap<GPUImage>
{
public:
    ImageResourceManager(const std::shared_ptr<VulkanContext>& context, ResourceHandle<Sampler> defaultSampler)
        : _defaultSampler(defaultSampler)
        , _context(context)
    {
    }

    ~ImageResourceManager() override = default;

    ResourceHandle<GPUImage> Create(
        const bb::Image2D& image,
        ResourceHandle<Sampler> sampler,
        bb::Flags<bb::TextureFlags> flags,
        std::string_view name,
        SingleTimeCommands* upload_commands);

    ResourceHandle<GPUImage> Create(
        const bb::Cubemap& cubemap,
        ResourceHandle<Sampler> sampler,
        bb::Flags<bb::TextureFlags> flags,
        std::string_view name);

    // TODO remove this overload at some point
    ResourceHandle<GPUImage> Create(
        const bb::Image2D& image,
        bb::Flags<bb::TextureFlags> flags,
        std::string_view name,
        SingleTimeCommands* upload_commands)
    {
        return Create(image, _defaultSampler, flags, name, upload_commands);
    }

    // TODO remove this overload at some point
    ResourceHandle<GPUImage> Create(
        const bb::Image2D& image,
        bb::Flags<bb::TextureFlags> flags,
        std::string_view name)
    {
        SingleTimeCommands commands { *_context };
        return Create(image, _defaultSampler, flags, name, &commands);
    }

    ResourceHandle<Sampler> _defaultSampler;

private:
    std::shared_ptr<VulkanContext> _context;
};
