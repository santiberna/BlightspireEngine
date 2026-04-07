#pragma once
#include "common.hpp"
#include "resources/image.hpp"
#include "resources/sampler.hpp"
#include "vulkan_fwd.hpp"

#include "enum_utils.hpp"
#include "resource_manager.hpp"
#include "vulkan_include.hpp"


#include <optional>

class SingleTimeCommands;
class VulkanContext;

// namespace bb
// {

// struct Texture
// {
//     static std::optional<Texture> fromImage(
//         SingleTimeCommands& upload_commands,
//         const Image2D& image,
//         ResourceHandle<Sampler> sampler,
//         VulkanFlags usage_flags,
//         const char* name);

//     // void destroy(VulkanContext& ctx);

//     struct LayerView
//     {
//         VkImageView view {};
//         std::vector<VkImageView> mips {};
//     };

//     ResourceHandle<Sampler> sampler {};
//     VmaAllocation allocation {};
//     VkImage image {};
//     VkImageView full_view {};
//     ImageFormat format {};
//     std::vector<LayerView> layers {};
//     ImageType type {};
//     uint32_t width {};
//     uint32_t height {};
//     uint32_t depth {};
//     uint32_t mips {};
// };

// }

struct GPUImage
{
public:
    GPUImage(const CPUImage& data, ResourceHandle<Sampler> textureSampler, const std::shared_ptr<VulkanContext>& context, SingleTimeCommands* const commands = nullptr);
    GPUImage(SingleTimeCommands& upload_commands, const bb::Image2D& image, ResourceHandle<Sampler> textureSampler, bb::Flags<bb::TextureFlags> flags, std::string_view name);

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
    ResourceHandle<Sampler> sampler {};

    uint16_t width { 1 };
    uint16_t height { 1 };
    uint16_t depth { 1 };
    uint16_t layers { 1 };
    uint8_t mips { 1 };
    vk::ImageUsageFlags flags { 0 };
    bool isHDR;
    ImageType type;

    vk::Format format { vk::Format::eUndefined };
    vk::ImageType vkType { vk::ImageType::e2D };

    std::string name;

private:
    VulkanContext* _context;
};