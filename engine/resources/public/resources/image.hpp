#pragma once
#include <string>
#include <vector>

#include "resource_manager.hpp"
#include "resources/sampler.hpp"
#include "single_time_commands.hpp"
#include "vulkan_context.hpp"
#include "vulkan_include.hpp"

namespace bb
{

enum class ImageFormat
{
    NONE,

    // Color sRGB
    R8G8B8A8_SRGB,
    R8G8B8_SRGB,
    R8G8_SRGB,
    R8_SRGB,

    // Linear UNORM
    R8G8B8A8_UNORM,
    R8G8B8_UNORM,
    R8G8_UNORM,
    R8_UNORM,

    R16_UNORM,
    R16G16_UNORM,

    // HDR / floating point
    R32G32B32A32_SFLOAT,
    R32G32B32_SFLOAT,
    R32G32_SFLOAT,
    R32_SFLOAT,
    R16G16B16A16_SFLOAT,
    R16G16B16_SFLOAT,
    R16G16_SFLOAT,

    // Depth
    D32_SFLOAT,
    D24_UNORM_S8_UINT,
    D16_UNORM,

    // BC compressed sRGB
    BC1_RGB_SRGB,
    BC3_SRGB,
    BC7_SRGB,

    // BC compressed linear
    BC5_UNORM,
    BC7_UNORM,
};

enum class ImageType
{
    NONE,
    IMAGE_2D,
    IMAGE_2D_ARRAY,
    IMAGE_CUBEMAP,
    IMAGE_3D,
};

struct Image
{
    static std::optional<Image> fromFile(std::string_view path, bool is_srgb);
    static std::optional<Image> fromMemory(std::span<std::byte> data, bool is_srgb);

    std::shared_ptr<std::byte[]> data {}; // Leave empty to create empty textures
    ImageFormat format {};
    ImageType type {};
    uint32_t width {};
    uint32_t height {};
    uint32_t depth {};
    uint32_t mips {}; // Leave as 0 to generate max amount of possible mips
};

}

enum class ImageType
{
    e2D,
    eDepth,
    eCubeMap,
    eShadowMap
};

struct CPUImage
{
    std::vector<std::byte> initialData;
    uint16_t width { 1 };
    uint16_t height { 1 };
    uint16_t depth { 1 };
    uint16_t layers { 1 };
    uint8_t mips { 1 };
    vk::ImageUsageFlags flags { 0 };
    bool isHDR = false;

    vk::Format format { vk::Format::eUndefined };
    ImageType type { ImageType::e2D };

    std::string name;

    /**
     * loads the pixel data, width, height and format from a specifed png file.
     * assigned format can be R8G8B8Unorm or vk::Format::eR8G8B8A8Unorm.
     * @param path filepath for the png file, .png extention required.
     */
    CPUImage& FromPNG(std::string_view path);
    CPUImage& SetData(std::vector<std::byte> data);
    CPUImage& SetSize(uint16_t width, uint16_t height, uint16_t depth = 1);
    CPUImage& SetMips(uint8_t mips);
    CPUImage& SetFlags(vk::ImageUsageFlags flags);
    CPUImage& SetFormat(vk::Format format);
    CPUImage& SetName(std::string_view name);
    CPUImage& SetType(ImageType type);
};

struct GPUImage
{
    GPUImage(const CPUImage& data, ResourceHandle<Sampler> textureSampler, const std::shared_ptr<VulkanContext>& context, SingleTimeCommands* const commands = nullptr);
    ~GPUImage();

    GPUImage(GPUImage&& other) noexcept;
    GPUImage& operator=(GPUImage&& other) noexcept;

    NON_COPYABLE(GPUImage);

    struct Layer
    {
        vk::ImageView view;
        std::vector<vk::ImageView> mipViews {};
    };

    vk::Image image {};
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
    std::shared_ptr<VulkanContext> _context;
};