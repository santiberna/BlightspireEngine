#pragma once
#include <string>
#include <vector>

#include "enum_utils.hpp"
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

    // HDR / floating point
    R32G32B32A32_SFLOAT,
};

struct Image2D
{
    static std::optional<Image2D> fromFile(std::string_view path);
    static std::optional<Image2D> fromMemory(std::span<std::byte> data);

    std::shared_ptr<std::byte[]> data {}; // Leave empty to create empty textures
    ImageFormat format {};
    uint32_t width {};
    uint32_t height {};
};

enum class TextureFlags : uint8_t
{
    SAMPLED = 1 << 0,
    TRANSFER_SRC = 1 << 1,
    TRANSFER_DST = 1 << 2,
    COLOR_ATTACH = 1 << 3,
    DEPTH_ATTACH = 1 << 4,
    GEN_MIPMAPS = 1 << 5,

    COMMON_FLAGS = SAMPLED | TRANSFER_SRC | TRANSFER_DST | GEN_MIPMAPS,
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