#pragma once
#include "common.hpp"
#include "resources/image.hpp"
#include "vulkan_fwd.hpp"

#include <optional>

class SingleTimeCommands;

namespace bb
{

struct Texture
{
    static std::optional<Texture> fromImage(
        SingleTimeCommands& upload_commands,
        const Image2D& image,
        ResourceHandle<Sampler> sampler,
        VulkanFlags usage_flags,
        const char* name);

    // void destroy(VulkanContext& ctx);

    struct LayerView
    {
        VkImageView view {};
        std::vector<VkImageView> mips {};
    };

    ResourceHandle<Sampler> sampler {};
    VmaAllocation allocation {};
    VkImage image {};
    VkImageView full_view {};
    ImageFormat format {};
    std::vector<LayerView> layers {};
    ImageType type {};
    uint32_t width {};
    uint32_t height {};
    uint32_t depth {};
    uint32_t mips {};
};

}