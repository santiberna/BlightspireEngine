#pragma once
#include "common.hpp"
#include "resources/image.hpp"
#include "resources/sampler.hpp"
#include "vulkan_fwd.hpp"

#include "resource_manager.hpp"
#include "vulkan_include.hpp"

class SingleTimeCommands;
class VulkanContext;

struct GPUImage
{
    GPUImage() = default;
    ~GPUImage();

    GPUImage(GPUImage&& other) noexcept;
    GPUImage& operator=(GPUImage&& other) noexcept;

    NON_COPYABLE(GPUImage);

    struct Layer
    {
        vk::ImageView view;
        std::vector<vk::ImageView> mipViews {};
    };

    vk::Image handle {};
    std::vector<Layer> layerViews {};
    vk::ImageView view; // Same as first view in view, or refers to a cubemap view
    VmaAllocation allocation {};
    ResourceHandle<bb::Sampler> sampler {};

    bb::u16 width { 1 };
    bb::u16 height { 1 };
    bb::u16 depth { 1 };
    bb::u16 layers { 1 };
    bb::u8 mips { 1 };
    vk::ImageUsageFlags flags { 0 };
    bool isHDR;
    ImageType type;

    vk::Format format { vk::Format::eUndefined };
    vk::ImageType vkType { vk::ImageType::e2D };

    std::string name;
    VulkanContext* _context;
};