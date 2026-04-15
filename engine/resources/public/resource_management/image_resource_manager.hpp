#pragma once

#include "resource_manager.hpp"
#include "resources/image.hpp"
#include "resources/texture.hpp"
#include "single_time_commands.hpp"

#include <memory>

class VulkanContext;

class ImageResourceManager final : public ResourceManager<GPUImage>
{
public:
    ImageResourceManager(const std::shared_ptr<VulkanContext>& context, ResourceHandle<Sampler> defaultSampler)
        : _defaultSampler(defaultSampler)
        , _context(context)
    {
    }

    ResourceHandle<GPUImage> Create(
        const CPUImage& cpuImage,
        ResourceHandle<Sampler> sampler,
        SingleTimeCommands* commands = nullptr);

    // TODO remove this
    ResourceHandle<GPUImage> Create(
        const CPUImage& cpuImage,
        SingleTimeCommands* commands = nullptr)
    {
        return Create(cpuImage, _defaultSampler, commands);
    }

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

    ResourceHandle<Sampler> _defaultSampler;

private:
    std::shared_ptr<VulkanContext> _context;
};
