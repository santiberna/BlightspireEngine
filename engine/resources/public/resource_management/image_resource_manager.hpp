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
    explicit ImageResourceManager(const std::shared_ptr<VulkanContext>& context, ResourceHandle<Sampler> defaultSampler);

    ResourceHandle<GPUImage> Create(const CPUImage& cpuImage, ResourceHandle<Sampler> sampler, SingleTimeCommands* const commands = nullptr);

    // Create an image with the default sampler
    ResourceHandle<GPUImage> Create(const CPUImage& cpuImage, SingleTimeCommands* commands = nullptr)
    {
        return Create(cpuImage, _defaultSampler, commands);
    }

    // NEW API
    ResourceHandle<GPUImage> Create(SingleTimeCommands& upload_commands, const bb::Image2D& image, ResourceHandle<Sampler> textureSampler, bb::Flags<bb::TextureFlags> flags, std::string_view name)
    {
        auto texture = GPUImage { upload_commands, image, textureSampler, flags, name };
        return ResourceManager::Create(std::move(texture));
    }

    // NEW API
    ResourceHandle<GPUImage> Create(const bb::Image2D& image, bb::Flags<bb::TextureFlags> flags, std::string_view name)
    {
        SingleTimeCommands commands { *_context };
        auto texture = GPUImage { commands, image, _defaultSampler, flags, name };
        return ResourceManager::Create(std::move(texture));
    }

    ResourceHandle<Sampler> _defaultSampler;

private:
    std::shared_ptr<VulkanContext> _context;

    vk::ImageType ImageTypeConversion(ImageType type);
    vk::ImageViewType ImageViewTypeConversion(ImageType type);
};
