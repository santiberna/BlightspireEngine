#include "resource_management/image_resource_manager.hpp"
#include "resources/texture.hpp"

#include "single_time_commands.hpp"
#include "vulkan_helper.hpp"

#include <file_io.hpp>
#include <spdlog/spdlog.h>
#include <stb_image.h>
#include <tracy/Tracy.hpp>

namespace
{
vk::Format toVkFormat(bb::ImageFormat format)
{
    switch (format)
    {
    // Color sRGB
    case bb::ImageFormat::B8G8R8A8_UNORM:
        return vk::Format::eB8G8R8A8Unorm;
    case bb::ImageFormat::R8G8B8A8_SRGB:
        return vk::Format::eR8G8B8A8Srgb;
    case bb::ImageFormat::R8G8B8_SRGB:
        return vk::Format::eR8G8B8Srgb;
    case bb::ImageFormat::R8G8_SRGB:
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
    case bb::ImageFormat::R16G16B16A16_SFLOAT:
        return vk::Format::eR16G16B16A16Sfloat;
    case bb::ImageFormat::R16G16_SFLOAT:
        return vk::Format::eR16G16Sfloat;
    case bb::ImageFormat::R32_SFLOAT:
        return vk::Format::eR32Sfloat;
    // Depth
    case bb::ImageFormat::D16_UNORM:
        return vk::Format::eD16Unorm;
    case bb::ImageFormat::D32_SFLOAT:
        return vk::Format::eD32Sfloat;

    case bb::ImageFormat::NONE:
        return vk::Format::eUndefined;
    }
}

uint32_t formatStride(bb::ImageFormat format)
{
    switch (format)
    {
    case bb::ImageFormat::R32G32B32A32_SFLOAT:
        return 16;
    case bb::ImageFormat::R16G16B16A16_SFLOAT:
        return 8;
    case bb::ImageFormat::R8G8B8A8_SRGB:
    case bb::ImageFormat::R8G8B8A8_UNORM:
    case bb::ImageFormat::B8G8R8A8_UNORM:
    case bb::ImageFormat::R16G16_SFLOAT:
    case bb::ImageFormat::R32_SFLOAT:
    case bb::ImageFormat::D32_SFLOAT:
        return 4;
    case bb::ImageFormat::R8G8B8_SRGB:
    case bb::ImageFormat::R8G8B8_UNORM:
        return 3;
    case bb::ImageFormat::R8G8_UNORM:
    case bb::ImageFormat::R8G8_SRGB:
    case bb::ImageFormat::D16_UNORM:
        return 2;
    case bb::ImageFormat::R8_SRGB:
    case bb::ImageFormat::R8_UNORM:
        return 1;
    case bb::ImageFormat::NONE:
        return 0;
    }
}
}

ResourceHandle<GPUImage> ImageResourceManager::Create(
    const bb::Image2D& image,
    ResourceHandle<Sampler> sampler,
    bb::Flags<bb::TextureFlags> flags,
    std::string_view name,
    SingleTimeCommands* upload_commands)
{
    GPUImage out {};
    out._context = _context.get();
    out.name = name;
    out.width = image.width;
    out.height = image.height;
    out.mips = 1;
    out.depth = 1;
    out.format = toVkFormat(image.format);
    out.type = ImageType::e2D;

    out.isHDR = image.format == bb::ImageFormat::R32G32B32A32_SFLOAT;
    out.sampler = sampler;

    out.flags = {};

    if (flags.has(bb::TextureFlags::SAMPLED))
    {
        out.flags |= vk::ImageUsageFlagBits::eSampled;
    }
    if (flags.has(bb::TextureFlags::TRANSFER_DST))
    {
        out.flags |= vk::ImageUsageFlagBits::eTransferDst;
    }
    if (flags.has(bb::TextureFlags::TRANSFER_SRC))
    {
        out.flags |= vk::ImageUsageFlagBits::eTransferSrc;
    }
    if (flags.has(bb::TextureFlags::COLOR_ATTACH))
    {
        out.flags |= vk::ImageUsageFlagBits::eColorAttachment;
    }
    if (flags.has(bb::TextureFlags::DEPTH_ATTACH))
    {
        out.flags |= vk::ImageUsageFlagBits::eDepthStencilAttachment;
    }
    if (flags.has(bb::TextureFlags::STORAGE_ACCESS))
    {
        out.flags |= vk::ImageUsageFlagBits::eStorage;
    }
    if (flags.has(bb::TextureFlags::GEN_MIPMAPS))
    {
        out.mips = static_cast<uint8_t>(std::floor(std::log2(std::max(out.width, out.height)))) + 1;
    }

    vk::ImageCreateInfo imageCreateInfo {};
    imageCreateInfo.imageType = vk::ImageType::e2D;
    imageCreateInfo.extent.width = out.width;
    imageCreateInfo.extent.height = out.height;
    imageCreateInfo.extent.depth = out.depth;
    imageCreateInfo.mipLevels = out.mips;
    imageCreateInfo.arrayLayers = out.layers;
    imageCreateInfo.format = out.format;
    imageCreateInfo.tiling = vk::ImageTiling::eOptimal;
    imageCreateInfo.initialLayout = vk::ImageLayout::eUndefined;
    imageCreateInfo.sharingMode = vk::SharingMode::eExclusive;
    imageCreateInfo.samples = vk::SampleCountFlagBits::e1;
    imageCreateInfo.usage = out.flags;

    {
        ZoneScopedN("VMA Image allocation");
        VmaAllocationCreateInfo allocCreateInfo {};
        allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        auto result = util::vmaCreateImage(
            _context->MemoryAllocator(),
            reinterpret_cast<VkImageCreateInfo*>(&imageCreateInfo),
            &allocCreateInfo,
            reinterpret_cast<VkImage*>(&out.handle),
            &out.allocation,
            nullptr);

        util::VK_ASSERT(result, "Failed to create handle");

        std::string allocName = std::string(name) + " handle allocation";
        // vmaSetAllocationName(_context->MemoryAllocator(), allocation, allocName.c_str());
    }

    vk::ImageViewCreateInfo viewCreateInfo {};
    viewCreateInfo.image = out.handle;
    viewCreateInfo.viewType = vk::ImageViewType::e2D;
    viewCreateInfo.format = out.format;
    viewCreateInfo.subresourceRange.aspectMask = util::GetImageAspectFlags(out.format);
    viewCreateInfo.subresourceRange.baseMipLevel = 0;
    viewCreateInfo.subresourceRange.levelCount = out.mips;
    viewCreateInfo.subresourceRange.baseArrayLayer = 0;
    viewCreateInfo.subresourceRange.layerCount = 1;

    vk::Device device = _context->Device();

    for (size_t i = 0; i < imageCreateInfo.arrayLayers; ++i)
    {
        viewCreateInfo.subresourceRange.levelCount = out.mips;
        viewCreateInfo.subresourceRange.baseMipLevel = 0;
        viewCreateInfo.subresourceRange.baseArrayLayer = i;
        GPUImage::Layer& layer = out.layerViews.emplace_back();
        layer.view = device.createImageView(viewCreateInfo).value;

        for (size_t j = 0; j < imageCreateInfo.mipLevels; ++j)
        {
            viewCreateInfo.subresourceRange.levelCount = 1;
            viewCreateInfo.subresourceRange.baseMipLevel = j;
            layer.mipViews.emplace_back(device.createImageView(viewCreateInfo).value);
        }
    }
    out.view = out.layerViews.begin()->view;

    if (image.data != nullptr && upload_commands)
    {
        ZoneScopedN("Image data Upload");
        vk::DeviceSize imageSize = out.width * out.height * out.depth * formatStride(image.format);

        vk::Buffer stagingBuffer;
        VmaAllocation stagingBufferAllocation;

        {
            ZoneScopedN("Image buffer allocation");
            util::CreateBuffer(*_context, imageSize, vk::BufferUsageFlagBits::eTransferSrc, stagingBuffer, true, stagingBufferAllocation, VMA_MEMORY_USAGE_CPU_ONLY, "Texture staging buffer");
            vmaCopyMemoryToAllocation(_context->MemoryAllocator(), image.data.get(), stagingBufferAllocation, 0, imageSize);
        }

        vk::ImageLayout oldLayout = vk::ImageLayout::eTransferDstOptimal;
        vk::CommandBuffer commandBuffer = upload_commands->CommandBuffer();

        util::TransitionImageLayout(commandBuffer, out.handle, out.format, vk::ImageLayout::eUndefined, oldLayout);
        util::CopyBufferToImage(commandBuffer, stagingBuffer, out.handle, out.width, out.height);

        if (out.mips > 1)
        {
            ZoneScopedN("Mip creation dispatch");
            util::TransitionImageLayout(commandBuffer, out.handle, out.format, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eTransferSrcOptimal, 1, 0, 1);

            for (uint32_t i = 1; i < out.mips; ++i)
            {
                vk::ImageBlit blit {};
                blit.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
                blit.srcSubresource.layerCount = 1;
                blit.srcSubresource.mipLevel = i - 1;
                blit.srcOffsets[1].x = out.width >> (i - 1);
                blit.srcOffsets[1].y = out.height >> (i - 1);
                blit.srcOffsets[1].z = 1;

                blit.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
                blit.dstSubresource.layerCount = 1;
                blit.dstSubresource.mipLevel = i;
                blit.dstOffsets[1].x = out.width >> i;
                blit.dstOffsets[1].y = out.height >> i;
                blit.dstOffsets[1].z = 1;

                util::TransitionImageLayout(commandBuffer, out.handle, out.format, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, 1, i);

                commandBuffer.blitImage(out.handle, vk::ImageLayout::eTransferSrcOptimal, out.handle, vk::ImageLayout::eTransferDstOptimal, 1, &blit, vk::Filter::eLinear);

                util::TransitionImageLayout(commandBuffer, out.handle, out.format, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eTransferSrcOptimal, 1, i);
            }
            oldLayout = vk::ImageLayout::eTransferSrcOptimal;
        }

        util::TransitionImageLayout(commandBuffer, out.handle, out.format, oldLayout, vk::ImageLayout::eShaderReadOnlyOptimal, 1, 0, out.mips);
        upload_commands->TrackAllocation(stagingBufferAllocation, stagingBuffer);
    }

    {
        ZoneScopedN("Name Settings");
        if (!name.empty())
        {
            std::stringstream ss {};
            ss << "[IMAGE] ";
            ss << name;
            std::string imageStr = ss.str();

            _context->DebugSetObjectName(out.handle, imageStr.c_str());
            ss.str("");

            for (size_t i = 0; i < imageCreateInfo.arrayLayers; ++i)
            {
                ss << "[VIEW " << i << "] ";
                ss << name;
                std::string viewStr = ss.str();
                _context->DebugSetObjectName(out.layerViews[i].view, viewStr.c_str());
                ss.str("");
            }

            ss << "[ALLOCATION] ";
            ss << name;
            std::string str = ss.str();
            vmaSetAllocationName(_context->MemoryAllocator(), out.allocation, str.c_str());
        }
        else
        {
            spdlog::warn("Creating an unnamed image!");
        }
    }

    return ResourceManager::Create(std::move(out));
}

ResourceHandle<GPUImage> ImageResourceManager::Create(
    const bb::Cubemap& cubemap,
    ResourceHandle<Sampler> sampler,
    bb::Flags<bb::TextureFlags> flags,
    std::string_view name)
{
    GPUImage out {};
    out._context = _context.get();
    out.name = name;
    out.width = cubemap.width;
    out.height = cubemap.height;
    out.layers = 6;
    out.mips = 1;
    out.depth = 1;
    out.format = toVkFormat(cubemap.format);
    out.type = ImageType::eCubeMap;

    out.isHDR = cubemap.format == bb::ImageFormat::R32G32B32A32_SFLOAT;
    out.sampler = sampler;

    out.flags = {};

    if (flags.has(bb::TextureFlags::SAMPLED))
    {
        out.flags |= vk::ImageUsageFlagBits::eSampled;
    }
    if (flags.has(bb::TextureFlags::TRANSFER_DST))
    {
        out.flags |= vk::ImageUsageFlagBits::eTransferDst;
    }
    if (flags.has(bb::TextureFlags::TRANSFER_SRC))
    {
        out.flags |= vk::ImageUsageFlagBits::eTransferSrc;
    }
    if (flags.has(bb::TextureFlags::COLOR_ATTACH))
    {
        out.flags |= vk::ImageUsageFlagBits::eColorAttachment;
    }
    if (flags.has(bb::TextureFlags::DEPTH_ATTACH))
    {
        out.flags |= vk::ImageUsageFlagBits::eDepthStencilAttachment;
    }
    if (flags.has(bb::TextureFlags::STORAGE_ACCESS))
    {
        out.flags |= vk::ImageUsageFlagBits::eStorage;
    }
    if (flags.has(bb::TextureFlags::GEN_MIPMAPS))
    {
        out.mips = static_cast<uint8_t>(std::floor(std::log2(std::max(out.width, out.height)))) + 1;
    }

    vk::ImageCreateInfo imageCreateInfo {};
    imageCreateInfo.imageType = vk::ImageType::e2D;
    imageCreateInfo.extent.width = out.width;
    imageCreateInfo.extent.height = out.height;
    imageCreateInfo.extent.depth = out.depth;
    imageCreateInfo.mipLevels = out.mips;
    imageCreateInfo.arrayLayers = out.layers;
    imageCreateInfo.format = out.format;
    imageCreateInfo.tiling = vk::ImageTiling::eOptimal;
    imageCreateInfo.initialLayout = vk::ImageLayout::eUndefined;
    imageCreateInfo.sharingMode = vk::SharingMode::eExclusive;
    imageCreateInfo.samples = vk::SampleCountFlagBits::e1;
    imageCreateInfo.usage = out.flags;
    imageCreateInfo.flags = vk::ImageCreateFlagBits::eCubeCompatible;

    {
        ZoneScopedN("VMA Image allocation");
        VmaAllocationCreateInfo allocCreateInfo {};
        allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        auto result = util::vmaCreateImage(
            _context->MemoryAllocator(),
            reinterpret_cast<VkImageCreateInfo*>(&imageCreateInfo),
            &allocCreateInfo,
            reinterpret_cast<VkImage*>(&out.handle),
            &out.allocation,
            nullptr);

        util::VK_ASSERT(result, "Failed to create handle");

        std::string allocName = std::string(name) + " handle allocation";
        // TODO: add this again?
        // vmaSetAllocationName(_context->MemoryAllocator(), allocation, allocName.c_str());
    }

    vk::ImageViewCreateInfo viewCreateInfo {};
    viewCreateInfo.image = out.handle;
    viewCreateInfo.viewType = vk::ImageViewType::e2D;
    viewCreateInfo.format = out.format;
    viewCreateInfo.subresourceRange.aspectMask = util::GetImageAspectFlags(out.format);
    viewCreateInfo.subresourceRange.baseMipLevel = 0;
    viewCreateInfo.subresourceRange.levelCount = out.mips;
    viewCreateInfo.subresourceRange.baseArrayLayer = 0;
    viewCreateInfo.subresourceRange.layerCount = 1;

    vk::Device device = _context->Device();

    for (size_t i = 0; i < imageCreateInfo.arrayLayers; ++i)
    {
        viewCreateInfo.subresourceRange.levelCount = out.mips;
        viewCreateInfo.subresourceRange.baseMipLevel = 0;
        viewCreateInfo.subresourceRange.baseArrayLayer = i;
        GPUImage::Layer& layer = out.layerViews.emplace_back();
        layer.view = device.createImageView(viewCreateInfo).value;

        for (size_t j = 0; j < imageCreateInfo.mipLevels; ++j)
        {
            viewCreateInfo.subresourceRange.levelCount = 1;
            viewCreateInfo.subresourceRange.baseMipLevel = j;
            layer.mipViews.emplace_back(device.createImageView(viewCreateInfo).value);
        }
    }

    vk::ImageViewCreateInfo cubeViewCreateInfo {};
    cubeViewCreateInfo.image = out.handle;
    cubeViewCreateInfo.viewType = vk::ImageViewType::eCube;
    cubeViewCreateInfo.format = out.format;
    cubeViewCreateInfo.subresourceRange.aspectMask = util::GetImageAspectFlags(out.format);
    cubeViewCreateInfo.subresourceRange.baseMipLevel = 0;
    cubeViewCreateInfo.subresourceRange.levelCount = out.mips;
    cubeViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    cubeViewCreateInfo.subresourceRange.layerCount = 6;

    util::VK_ASSERT(device.createImageView(&cubeViewCreateInfo, nullptr, &out.view), "Failed creating image view!");

    {
        ZoneScopedN("Name Settings");
        if (!name.empty())
        {
            std::stringstream ss {};
            ss << "[IMAGE] ";
            ss << name;
            std::string imageStr = ss.str();

            _context->DebugSetObjectName(out.handle, imageStr.c_str());
            ss.str("");

            for (size_t i = 0; i < imageCreateInfo.arrayLayers; ++i)
            {
                ss << "[VIEW " << i << "] ";
                ss << name;
                std::string viewStr = ss.str();
                _context->DebugSetObjectName(out.layerViews[i].view, viewStr.c_str());
                ss.str("");
            }

            ss << "[ALLOCATION] ";
            ss << name;
            std::string str = ss.str();
            vmaSetAllocationName(_context->MemoryAllocator(), out.allocation, str.c_str());
        }
        else
        {
            spdlog::warn("Creating an unnamed image!");
        }
    }

    return ResourceManager::Create(std::move(out));
}