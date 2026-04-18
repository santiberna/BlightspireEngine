#include "resources/image.hpp"

#include "vulkan_helper.hpp"
#include <file_io.hpp>
#include <fstream>
#include <spdlog/spdlog.h>
#include <stb_image.h>
#include <tracy/Tracy.hpp>

std::optional<bb::Image2D> bb::Image2D::fromFile(std::string_view path)
{
    auto data = fileIO::OpenReadStream(std::string(path));
    if (!data)
    {
        spdlog::error("Failed loading image data at path: {}", path);
        return std::nullopt;
    }
    auto mem = fileIO::DumpStreamIntoBytes(data.value());
    return fromMemory(mem);
}

std::optional<bb::Image2D> bb::Image2D::fromMemory(std::span<const std::byte> data)
{
    stbi_uc* mem_start = std::bit_cast<stbi_uc*>(data.data());
    int mem_size = static_cast<int>(data.size());

    int w;
    int h;
    int channels;
    ImageFormat format;
    std::byte* bytes = nullptr;

    bool is_float = stbi_is_hdr_from_memory(mem_start, mem_size) != 0;
    if (is_float)
    {
        // HDR maps with less than 4 channels are poorly supported, hence we force 4 channels in stbi
        bytes = std::bit_cast<std::byte*>(
            stbi_loadf_from_memory(mem_start, mem_size, &w, &h, &channels, 4));

        if (bytes == nullptr)
        {
            return std::nullopt;
        }

        format = ImageFormat::R32G32B32A32_SFLOAT;
    }
    else
    {
        bytes = std::bit_cast<std::byte*>(
            stbi_load_from_memory(mem_start, mem_size, &w, &h, &channels, 4));

        if (bytes == nullptr)
        {
            return std::nullopt;
        }

        format = ImageFormat::R8G8B8A8_UNORM;
    }

    bb::Image2D out;
    out.format = format;
    out.height = static_cast<uint32_t>(h);
    out.width = static_cast<uint32_t>(w);
    out.data = std::shared_ptr<std::byte[]>(bytes, stbi_image_free);

    return out;
}
