#include "resources/image.hpp"

#include "vulkan_helper.hpp"
#include <file_io.hpp>
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

std::optional<bb::Image2D> bb::Image2D::fromMemory(std::span<std::byte> data)
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
            stbi_load_from_memory(mem_start, mem_size, &w, &h, &channels, 0));

        if (bytes == nullptr)
        {
            return std::nullopt;
        }

        switch (channels)
        {
        case 4:
            format = ImageFormat::R8G8B8A8_UNORM;
            break;
        case 3:
            format = ImageFormat::R8G8B8_UNORM;
            break;
        case 2:
            format = ImageFormat::R8G8_UNORM;
            break;
        case 1:
            format = ImageFormat::R8_UNORM;
            break;
        default:
            return std::nullopt;
        }
    }

    bb::Image2D out;
    out.format = format;
    out.height = static_cast<uint32_t>(h);
    out.width = static_cast<uint32_t>(w);
    out.data = std::shared_ptr<std::byte[]>(bytes, stbi_image_free);

    return out;
}

CPUImage& CPUImage::FromPNG(std::string_view path)
{
    int width;
    int height;
    int nrChannels;

    auto stream1 = fileIO::OpenReadStream(std::string(path));
    auto stream2 = std::ifstream { std::string { path } };

    std::byte* data = nullptr;
    if (path.find("Steam") > path.size())
    {
        data = reinterpret_cast<std::byte*>(fileIO::LoadImageFromIfstream(stream1.value(),
            &width, &height, &nrChannels,
            4));
    }
    else
    {
        data = reinterpret_cast<std::byte*>(stbi_load(path.data(),
            &width, &height, &nrChannels,
            4));
    }

    if (data == nullptr)
    {
        throw std::runtime_error("Failed to load image!");
    }

    if (width > UINT16_MAX || height > UINT16_MAX)
    {
        throw std::runtime_error("Image size is too large!");
    }

    SetFormat(vk::Format::eR8G8B8A8Unorm);
    SetSize(static_cast<uint16_t>(width), static_cast<uint16_t>(height));
    SetName(path);
    initialData.assign(data, data + static_cast<ptrdiff_t>(width * height * 4));
    stbi_image_free(data);

    return *this;
}
CPUImage& CPUImage::SetData(std::vector<std::byte> data)
{
    initialData = std::move(data);
    return *this;
}

CPUImage& CPUImage::SetSize(uint16_t width, uint16_t height, uint16_t depth)
{
    this->width = width;
    this->height = height;
    this->depth = depth;
    return *this;
}

CPUImage& CPUImage::SetMips(uint8_t mips)
{
    this->mips = mips;
    return *this;
}

CPUImage& CPUImage::SetFlags(vk::ImageUsageFlags flags)
{
    this->flags = flags;
    return *this;
}

CPUImage& CPUImage::SetFormat(vk::Format format)
{
    this->format = format;
    return *this;
}

CPUImage& CPUImage::SetName(std::string_view name)
{
    this->name = name;
    return *this;
}

CPUImage& CPUImage::SetType(ImageType type)
{
    this->type = type;
    return *this;
}
