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

vk::ImageType ImageTypeConversion(ImageType type)
{
    switch (type)
    {
    case ImageType::e2D:
    case ImageType::eDepth:
    case ImageType::eShadowMap:
    case ImageType::eCubeMap:
        return vk::ImageType::e2D;
    default:
        throw std::runtime_error("Unsupported ImageType!");
    }
}

vk::ImageViewType ImageViewTypeConversion(ImageType type)
{
    switch (type)
    {
    case ImageType::eShadowMap:
    case ImageType::e2D:
        return vk::ImageViewType::e2D;
    case ImageType::eDepth:
        return vk::ImageViewType::e2D;
    case ImageType::eCubeMap:
        return vk::ImageViewType::eCube;
    default:
        throw std::runtime_error("Unsupported ImageType!");
    }
}

GPUImage::GPUImage(const CPUImage& creation, ResourceHandle<Sampler> textureSampler, const std::shared_ptr<VulkanContext>& context, SingleTimeCommands* const commands)
    : _context(context.get())
{
    width = creation.width;
    height = creation.height;
    depth = creation.depth;
    layers = creation.layers;
    flags = creation.flags;
    type = creation.type;
    format = creation.format;
    mips = std::min(creation.mips, static_cast<uint8_t>(floor(log2(std::max(width, height))) + 1));
    name = creation.name;
    isHDR = creation.isHDR;
    sampler = textureSampler;

    vk::ImageCreateInfo imageCreateInfo {};
    imageCreateInfo.imageType = ImageTypeConversion(creation.type);
    imageCreateInfo.extent.width = creation.width;
    imageCreateInfo.extent.height = creation.height;
    imageCreateInfo.extent.depth = creation.depth;
    imageCreateInfo.mipLevels = creation.mips;
    imageCreateInfo.arrayLayers = creation.type == ImageType::eCubeMap ? 6 : creation.layers;
    imageCreateInfo.format = creation.format;
    imageCreateInfo.tiling = vk::ImageTiling::eOptimal;
    imageCreateInfo.initialLayout = vk::ImageLayout::eUndefined;
    imageCreateInfo.sharingMode = vk::SharingMode::eExclusive;
    imageCreateInfo.samples = vk::SampleCountFlagBits::e1;

    imageCreateInfo.usage = creation.flags;

    if (creation.initialData.data())
    {
        imageCreateInfo.usage |= vk::ImageUsageFlagBits::eTransferDst;
        imageCreateInfo.usage |= vk::ImageUsageFlagBits::eTransferSrc;
    }
    if (creation.type == ImageType::eCubeMap)
    {
        imageCreateInfo.flags |= vk::ImageCreateFlagBits::eCubeCompatible;
    }

    {
        ZoneScopedN("VMA Image allocation");
        VmaAllocationCreateInfo allocCreateInfo {};
        allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        util::vmaCreateImage(_context->MemoryAllocator(), reinterpret_cast<VkImageCreateInfo*>(&imageCreateInfo), &allocCreateInfo, reinterpret_cast<VkImage*>(&handle), &allocation, nullptr);
        std::string allocName = creation.name + " handle allocation";
        vmaSetAllocationName(_context->MemoryAllocator(), allocation, allocName.c_str());
    }

    vk::ImageViewCreateInfo viewCreateInfo {};
    viewCreateInfo.image = handle;
    viewCreateInfo.viewType = vk::ImageViewType::e2D;
    viewCreateInfo.format = creation.format;
    viewCreateInfo.subresourceRange.aspectMask = util::GetImageAspectFlags(format);
    viewCreateInfo.subresourceRange.baseMipLevel = 0;
    viewCreateInfo.subresourceRange.levelCount = creation.mips;
    viewCreateInfo.subresourceRange.baseArrayLayer = 0;
    viewCreateInfo.subresourceRange.layerCount = 1;

    vk::Device device = _context->Device();

    for (size_t i = 0; i < imageCreateInfo.arrayLayers; ++i)
    {
        viewCreateInfo.subresourceRange.levelCount = creation.mips;
        viewCreateInfo.subresourceRange.baseMipLevel = 0;
        viewCreateInfo.subresourceRange.baseArrayLayer = i;
        Layer& layer = layerViews.emplace_back();
        layer.view = device.createImageView(viewCreateInfo);

        for (size_t j = 0; j < imageCreateInfo.mipLevels; ++j)
        {
            viewCreateInfo.subresourceRange.levelCount = 1;
            viewCreateInfo.subresourceRange.baseMipLevel = j;
            layer.mipViews.emplace_back(device.createImageView(viewCreateInfo));
        }
    }
    view = layerViews.begin()->view;

    if (creation.type == ImageType::eCubeMap)
    {
        vk::ImageViewCreateInfo cubeViewCreateInfo {};
        cubeViewCreateInfo.image = handle;
        cubeViewCreateInfo.viewType = ImageViewTypeConversion(creation.type);
        cubeViewCreateInfo.format = creation.format;
        cubeViewCreateInfo.subresourceRange.aspectMask = util::GetImageAspectFlags(format);
        cubeViewCreateInfo.subresourceRange.baseMipLevel = 0;
        cubeViewCreateInfo.subresourceRange.levelCount = creation.mips;
        cubeViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        cubeViewCreateInfo.subresourceRange.layerCount = 6;

        util::VK_ASSERT(device.createImageView(&cubeViewCreateInfo, nullptr, &view), "Failed creating image view!");
    }

    if (creation.initialData.data())
    {
        ZoneScopedN("Image data Upload");
        vk::DeviceSize imageSize = width * height * depth * 4;

        if (format == vk::Format::eR8Unorm)
        {
            imageSize = width * height * depth;
        }
        if (isHDR)
        {
            imageSize *= sizeof(float);
        }

        vk::Buffer stagingBuffer;
        VmaAllocation stagingBufferAllocation;

        {
            ZoneScopedN("Image buffer allocation");
            util::CreateBuffer(*_context, imageSize, vk::BufferUsageFlagBits::eTransferSrc, stagingBuffer, true, stagingBufferAllocation, VMA_MEMORY_USAGE_CPU_ONLY, "Texture staging buffer");
            vmaCopyMemoryToAllocation(_context->MemoryAllocator(), creation.initialData.data(), stagingBufferAllocation, 0, imageSize);
        }

        vk::ImageLayout oldLayout = vk::ImageLayout::eTransferDstOptimal;

        if (commands)
        {
            ZoneScopedN("Command upload dispatch");
            const vk::CommandBuffer& commandBuffer = commands->CommandBuffer();

            util::TransitionImageLayout(commandBuffer, handle, format, vk::ImageLayout::eUndefined, oldLayout);

            util::CopyBufferToImage(commandBuffer, stagingBuffer, handle, width, height);

            if (creation.mips > 1)
            {
                ZoneScopedN("Mip creation dispatch");
                util::TransitionImageLayout(commandBuffer, handle, format, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eTransferSrcOptimal, 1, 0, 1);

                for (uint32_t i = 1; i < creation.mips; ++i)
                {
                    vk::ImageBlit blit {};
                    blit.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
                    blit.srcSubresource.layerCount = 1;
                    blit.srcSubresource.mipLevel = i - 1;
                    blit.srcOffsets[1].x = width >> (i - 1);
                    blit.srcOffsets[1].y = height >> (i - 1);
                    blit.srcOffsets[1].z = 1;

                    blit.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
                    blit.dstSubresource.layerCount = 1;
                    blit.dstSubresource.mipLevel = i;
                    blit.dstOffsets[1].x = width >> i;
                    blit.dstOffsets[1].y = height >> i;
                    blit.dstOffsets[1].z = 1;

                    util::TransitionImageLayout(commandBuffer, handle, format, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, 1, i);

                    commandBuffer.blitImage(handle, vk::ImageLayout::eTransferSrcOptimal, handle, vk::ImageLayout::eTransferDstOptimal, 1, &blit, vk::Filter::eLinear);

                    util::TransitionImageLayout(commandBuffer, handle, format, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eTransferSrcOptimal, 1, i);
                }
                oldLayout = vk::ImageLayout::eTransferSrcOptimal;
            }

            util::TransitionImageLayout(commandBuffer, handle, format, oldLayout, vk::ImageLayout::eShaderReadOnlyOptimal, 1, 0, mips);

            commands->TrackAllocation(stagingBufferAllocation, stagingBuffer);
        }
        else
        {
            ZoneScopedN("Command upload dispatch");
            vk::CommandBuffer commandBuffer = util::BeginSingleTimeCommands(_context);

            util::TransitionImageLayout(commandBuffer, handle, format, vk::ImageLayout::eUndefined, oldLayout);

            util::CopyBufferToImage(commandBuffer, stagingBuffer, handle, width, height);

            if (creation.mips > 1)
            {
                ZoneScopedN("Mip creation dispatch");
                util::TransitionImageLayout(commandBuffer, handle, format, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eTransferSrcOptimal, 1, 0, 1);

                for (uint32_t i = 1; i < creation.mips; ++i)
                {
                    vk::ImageBlit blit {};
                    blit.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
                    blit.srcSubresource.layerCount = 1;
                    blit.srcSubresource.mipLevel = i - 1;
                    blit.srcOffsets[1].x = width >> (i - 1);
                    blit.srcOffsets[1].y = height >> (i - 1);
                    blit.srcOffsets[1].z = 1;

                    blit.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
                    blit.dstSubresource.layerCount = 1;
                    blit.dstSubresource.mipLevel = i;
                    blit.dstOffsets[1].x = width >> i;
                    blit.dstOffsets[1].y = height >> i;
                    blit.dstOffsets[1].z = 1;

                    util::TransitionImageLayout(commandBuffer, handle, format, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, 1, i);

                    commandBuffer.blitImage(handle, vk::ImageLayout::eTransferSrcOptimal, handle, vk::ImageLayout::eTransferDstOptimal, 1, &blit, vk::Filter::eLinear);

                    util::TransitionImageLayout(commandBuffer, handle, format, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eTransferSrcOptimal, 1, i);
                }
                oldLayout = vk::ImageLayout::eTransferSrcOptimal;
            }

            util::TransitionImageLayout(commandBuffer, handle, format, oldLayout, vk::ImageLayout::eShaderReadOnlyOptimal, 1, 0, mips);

            {
                ZoneScopedN("Waiting Image upload");
                util::EndSingleTimeCommands(_context, commandBuffer);
            }

            util::vmaDestroyBuffer(_context->MemoryAllocator(), stagingBuffer, stagingBufferAllocation);
        }
    }

    {
        ZoneScopedN("Name Settings");
        if (!creation.name.empty())
        {
            std::stringstream ss {};
            ss << "[IMAGE] ";
            ss << creation.name;
            std::string imageStr = ss.str();

            _context->DebugSetObjectName(handle, imageStr.c_str());
            ss.str("");

            for (size_t i = 0; i < imageCreateInfo.arrayLayers; ++i)
            {
                ss << "[VIEW " << i << "] ";
                ss << creation.name;
                std::string viewStr = ss.str();
                _context->DebugSetObjectName(layerViews[i].view, viewStr.c_str());
                ss.str("");
            }

            ss << "[ALLOCATION] ";
            ss << creation.name;
            std::string str = ss.str();
            vmaSetAllocationName(_context->MemoryAllocator(), allocation, str.c_str());
        }
        else
        {
            spdlog::warn("Creating an unnamed image!");
        }
    }
}

namespace
{
vk::Format toVkFormat(bb::ImageFormat format)
{
    switch (format)
    {
    // Color sRGB
    case bb::ImageFormat::R8G8B8A8_SRGB:
        return vk::Format::eR8G8B8A8Srgb;
    case bb::ImageFormat::R8G8B8_SRGB:
        return vk::Format::eR8G8B8Srgb;
        return vk::Format::eR8G8Srgb;
    case bb::ImageFormat::R8_SRGB:
        return vk::Format::eR8Srgb;
    // Linear UNORM
    case bb::ImageFormat::R8G8B8A8_UNORM:
        return vk::Format::eR8G8B8A8Unorm;
    case bb::ImageFormat::R8G8B8_UNORM:
        return vk::Format::eR8G8B8Unorm;
    case bb::ImageFormat::R8G8_UNORM:
        return vk::Format::eR8G8Unorm;
    case bb::ImageFormat::R8_UNORM:
        return vk::Format::eR8Unorm;
    // HDR / floating point
    case bb::ImageFormat::R32G32B32A32_SFLOAT:
        return vk::Format::eR32G32B32A32Sfloat;
    case bb::ImageFormat::NONE:
    default:
        assert(false && "Invalid bb::ImageFormat");
        return vk::Format::eUndefined;
    }
}

uint32_t formatStride(bb::ImageFormat format)
{
    switch (format)
    {
    case bb::ImageFormat::R32G32B32A32_SFLOAT:
        return 16;
    case bb::ImageFormat::R8G8B8A8_SRGB:
    case bb::ImageFormat::R8G8B8A8_UNORM:
        return 4;
    case bb::ImageFormat::R8G8B8_SRGB:
    case bb::ImageFormat::R8G8B8_UNORM:
        return 3;
    case bb::ImageFormat::R8G8_UNORM:
    case bb::ImageFormat::R8G8_SRGB:
        return 2;
    case bb::ImageFormat::R8_SRGB:
    case bb::ImageFormat::R8_UNORM:
        return 1;
    case bb::ImageFormat::NONE:
        return 0;
    }
}
}

GPUImage::GPUImage(SingleTimeCommands& upload_commands, const bb::Image2D& image, ResourceHandle<Sampler> textureSampler, bb::Flags<bb::TextureFlags> input_flags, std::string_view given_name)
    : name(given_name)
    , _context(&upload_commands.GetContext())
{
    width = image.width;
    height = image.height;
    mips = 1;
    depth = 1;
    format = toVkFormat(image.format);
    type = ImageType::e2D;

    isHDR = image.format == bb::ImageFormat::R32G32B32A32_SFLOAT;
    sampler = textureSampler;

    flags = {};

    if (input_flags.has(bb::TextureFlags::SAMPLED))
    {
        flags |= vk::ImageUsageFlagBits::eSampled;
    }
    if (input_flags.has(bb::TextureFlags::TRANSFER_DST))
    {
        flags |= vk::ImageUsageFlagBits::eTransferDst;
    }
    if (input_flags.has(bb::TextureFlags::TRANSFER_SRC))
    {
        flags |= vk::ImageUsageFlagBits::eTransferSrc;
    }
    if (input_flags.has(bb::TextureFlags::COLOR_ATTACH))
    {
        flags |= vk::ImageUsageFlagBits::eColorAttachment;
    }
    if (input_flags.has(bb::TextureFlags::DEPTH_ATTACH))
    {
        flags |= vk::ImageUsageFlagBits::eDepthStencilAttachment;
    }
    if (input_flags.has(bb::TextureFlags::GEN_MIPMAPS))
    {
        mips = static_cast<uint8_t>(std::floor(std::log2(std::max(width, height)))) + 1;
    }

    vk::ImageCreateInfo imageCreateInfo {};
    imageCreateInfo.imageType = vk::ImageType::e2D;
    imageCreateInfo.extent.width = width;
    imageCreateInfo.extent.height = height;
    imageCreateInfo.extent.depth = depth;
    imageCreateInfo.mipLevels = mips;
    imageCreateInfo.arrayLayers = layers;
    imageCreateInfo.format = format;
    imageCreateInfo.tiling = vk::ImageTiling::eOptimal;
    imageCreateInfo.initialLayout = vk::ImageLayout::eUndefined;
    imageCreateInfo.sharingMode = vk::SharingMode::eExclusive;
    imageCreateInfo.samples = vk::SampleCountFlagBits::e1;
    imageCreateInfo.usage = flags;

    {
        ZoneScopedN("VMA Image allocation");
        VmaAllocationCreateInfo allocCreateInfo {};
        allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        auto result = util::vmaCreateImage(
            _context->MemoryAllocator(),
            reinterpret_cast<VkImageCreateInfo*>(&imageCreateInfo),
            &allocCreateInfo,
            reinterpret_cast<VkImage*>(&handle),
            &allocation,
            nullptr);

        util::VK_ASSERT(result, "Failed to create handle");

        std::string allocName = std::string(name) + " handle allocation";
        // vmaSetAllocationName(_context->MemoryAllocator(), allocation, allocName.c_str());
    }

    vk::ImageViewCreateInfo viewCreateInfo {};
    viewCreateInfo.image = handle;
    viewCreateInfo.viewType = vk::ImageViewType::e2D;
    viewCreateInfo.format = format;
    viewCreateInfo.subresourceRange.aspectMask = util::GetImageAspectFlags(format);
    viewCreateInfo.subresourceRange.baseMipLevel = 0;
    viewCreateInfo.subresourceRange.levelCount = mips;
    viewCreateInfo.subresourceRange.baseArrayLayer = 0;
    viewCreateInfo.subresourceRange.layerCount = 1;

    vk::Device device = _context->Device();

    for (size_t i = 0; i < imageCreateInfo.arrayLayers; ++i)
    {
        viewCreateInfo.subresourceRange.levelCount = mips;
        viewCreateInfo.subresourceRange.baseMipLevel = 0;
        viewCreateInfo.subresourceRange.baseArrayLayer = i;
        Layer& layer = layerViews.emplace_back();
        layer.view = device.createImageView(viewCreateInfo);

        for (size_t j = 0; j < imageCreateInfo.mipLevels; ++j)
        {
            viewCreateInfo.subresourceRange.levelCount = 1;
            viewCreateInfo.subresourceRange.baseMipLevel = j;
            layer.mipViews.emplace_back(device.createImageView(viewCreateInfo));
        }
    }
    view = layerViews.begin()->view;

    if (image.data != nullptr)
    {
        ZoneScopedN("Image data Upload");
        vk::DeviceSize imageSize = width * height * depth * formatStride(image.format);

        vk::Buffer stagingBuffer;
        VmaAllocation stagingBufferAllocation;

        {
            ZoneScopedN("Image buffer allocation");
            util::CreateBuffer(*_context, imageSize, vk::BufferUsageFlagBits::eTransferSrc, stagingBuffer, true, stagingBufferAllocation, VMA_MEMORY_USAGE_CPU_ONLY, "Texture staging buffer");
            vmaCopyMemoryToAllocation(_context->MemoryAllocator(), image.data.get(), stagingBufferAllocation, 0, imageSize);
        }

        vk::ImageLayout oldLayout = vk::ImageLayout::eTransferDstOptimal;
        vk::CommandBuffer commandBuffer = upload_commands.CommandBuffer();

        util::TransitionImageLayout(commandBuffer, handle, format, vk::ImageLayout::eUndefined, oldLayout);
        util::CopyBufferToImage(commandBuffer, stagingBuffer, handle, width, height);

        if (mips > 1)
        {
            ZoneScopedN("Mip creation dispatch");
            util::TransitionImageLayout(commandBuffer, handle, format, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eTransferSrcOptimal, 1, 0, 1);

            for (uint32_t i = 1; i < mips; ++i)
            {
                vk::ImageBlit blit {};
                blit.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
                blit.srcSubresource.layerCount = 1;
                blit.srcSubresource.mipLevel = i - 1;
                blit.srcOffsets[1].x = width >> (i - 1);
                blit.srcOffsets[1].y = height >> (i - 1);
                blit.srcOffsets[1].z = 1;

                blit.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
                blit.dstSubresource.layerCount = 1;
                blit.dstSubresource.mipLevel = i;
                blit.dstOffsets[1].x = width >> i;
                blit.dstOffsets[1].y = height >> i;
                blit.dstOffsets[1].z = 1;

                util::TransitionImageLayout(commandBuffer, handle, format, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, 1, i);

                commandBuffer.blitImage(handle, vk::ImageLayout::eTransferSrcOptimal, handle, vk::ImageLayout::eTransferDstOptimal, 1, &blit, vk::Filter::eLinear);

                util::TransitionImageLayout(commandBuffer, handle, format, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eTransferSrcOptimal, 1, i);
            }
            oldLayout = vk::ImageLayout::eTransferSrcOptimal;
        }

        util::TransitionImageLayout(commandBuffer, handle, format, oldLayout, vk::ImageLayout::eShaderReadOnlyOptimal, 1, 0, mips);
        upload_commands.TrackAllocation(stagingBufferAllocation, stagingBuffer);
    }

    {
        ZoneScopedN("Name Settings");
        if (!name.empty())
        {
            std::stringstream ss {};
            ss << "[IMAGE] ";
            ss << name;
            std::string imageStr = ss.str();

            _context->DebugSetObjectName(handle, imageStr.c_str());
            ss.str("");

            for (size_t i = 0; i < imageCreateInfo.arrayLayers; ++i)
            {
                ss << "[VIEW " << i << "] ";
                ss << name;
                std::string viewStr = ss.str();
                _context->DebugSetObjectName(layerViews[i].view, viewStr.c_str());
                ss.str("");
            }

            ss << "[ALLOCATION] ";
            ss << name;
            std::string str = ss.str();
            vmaSetAllocationName(_context->MemoryAllocator(), allocation, str.c_str());
        }
        else
        {
            spdlog::warn("Creating an unnamed image!");
        }
    }
}

GPUImage::~GPUImage()
{
    if (!_context)
    {
        return;
    }

    vk::Device device = _context->Device();
    util::vmaDestroyImage(_context->MemoryAllocator(), handle, allocation);

    for (auto& layer : layerViews)
    {
        device.destroy(layer.view);
        for (auto& mipView : layer.mipViews)
        {
            device.destroy(mipView);
        }
    }
    if (type == ImageType::eCubeMap)
    {
        device.destroy(view);
    }
}

GPUImage::GPUImage(GPUImage&& other) noexcept
    : handle(other.handle)
    , layerViews(std::move(other.layerViews))
    , view(other.view)
    , allocation(other.allocation)
    , sampler(other.sampler)
    , width(other.width)
    , height(other.height)
    , depth(other.depth)
    , layers(other.layers)
    , mips(other.mips)
    , flags(other.flags)
    , isHDR(other.isHDR)
    , type(other.type)
    , format(other.format)
    , vkType(other.vkType)
    , name(std::move(other.name))
    , _context(other._context)
{
    other.handle = nullptr;
    other.view = nullptr;
    other.allocation = nullptr;
    other._context = nullptr;
}

GPUImage& GPUImage::operator=(GPUImage&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    handle = other.handle;
    layerViews = std::move(other.layerViews);
    view = other.view;
    allocation = other.allocation;
    sampler = other.sampler;
    width = other.width;
    height = other.height;
    depth = other.depth;
    layers = other.layers;
    mips = other.mips;
    flags = other.flags;
    isHDR = other.isHDR;
    type = other.type;
    format = other.format;
    vkType = other.vkType;
    name = std::move(other.name);
    _context = other._context;

    other.handle = nullptr;
    other.view = nullptr;
    other.allocation = nullptr;
    other._context = nullptr;

    return *this;
}