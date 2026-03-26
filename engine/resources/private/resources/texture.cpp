#include "resources/texture.hpp"

#include "vulkan_helper.hpp"

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
    case bb::ImageFormat::R16_UNORM:
        return vk::Format::eR16Unorm;
    case bb::ImageFormat::R16G16_UNORM:
        return vk::Format::eR16G16Unorm;

    // HDR / floating point
    case bb::ImageFormat::R32G32B32A32_SFLOAT:
        return vk::Format::eR32G32B32A32Sfloat;
    case bb::ImageFormat::R32G32B32_SFLOAT:
        return vk::Format::eR32G32B32Sfloat;
    case bb::ImageFormat::R32G32_SFLOAT:
        return vk::Format::eR32G32Sfloat;
    case bb::ImageFormat::R32_SFLOAT:
        return vk::Format::eR32Sfloat;
    case bb::ImageFormat::R16G16B16A16_SFLOAT:
        return vk::Format::eR16G16B16A16Sfloat;
    case bb::ImageFormat::R16G16B16_SFLOAT:
        return vk::Format::eR16G16B16Sfloat;
    case bb::ImageFormat::R16G16_SFLOAT:
        return vk::Format::eR16G16Sfloat; // likely a typo — no R16G16B

    // Depth
    case bb::ImageFormat::D32_SFLOAT:
        return vk::Format::eD32Sfloat;
    case bb::ImageFormat::D24_UNORM_S8_UINT:
        return vk::Format::eD24UnormS8Uint;
    case bb::ImageFormat::D16_UNORM:
        return vk::Format::eD16Unorm;

    // BC compressed sRGB
    case bb::ImageFormat::BC1_RGB_SRGB:
        return vk::Format::eBc1RgbSrgbBlock;
    case bb::ImageFormat::BC3_SRGB:
        return vk::Format::eBc3SrgbBlock;
    case bb::ImageFormat::BC7_SRGB:
        return vk::Format::eBc7SrgbBlock;

    // BC compressed linear
    case bb::ImageFormat::BC5_UNORM:
        return vk::Format::eBc5UnormBlock;
    case bb::ImageFormat::BC7_UNORM:
        return vk::Format::eBc7UnormBlock;

    case bb::ImageFormat::NONE:
    default:
        assert(false && "Invalid bb::ImageFormat");
        return vk::Format::eUndefined;
    }
}

vk::ImageType toVkImageType(bb::ImageType type)
{
    switch (type)
    {
    case bb::ImageType::IMAGE_2D:
    case bb::ImageType::IMAGE_2D_ARRAY:
    case bb::ImageType::IMAGE_CUBEMAP:
        return vk::ImageType::e2D;
    case bb::ImageType::IMAGE_3D:
        return vk::ImageType::e3D;

    case bb::ImageType::NONE:
    default:
        assert(false && "Invalid bb::ImageType");
        return vk::ImageType::e2D;
    }
}

vk::ImageViewType toVkImageViewType(bb::ImageType type)
{
    switch (type)
    {
    case bb::ImageType::IMAGE_2D:
        return vk::ImageViewType::e2D;
    case bb::ImageType::IMAGE_2D_ARRAY:
        return vk::ImageViewType::e2DArray;
    case bb::ImageType::IMAGE_CUBEMAP:
        return vk::ImageViewType::eCube;
    case bb::ImageType::IMAGE_3D:
        return vk::ImageViewType::e3D;

    case bb::ImageType::NONE:
    default:
        assert(false && "Invalid bb::ImageType");
        return vk::ImageViewType::e2D;
    }
}

uint32_t formatStride(bb::ImageFormat format)
{
    switch (format)
    {
    // Color sRGB
    case bb::ImageFormat::R8G8B8A8_SRGB:
        return 4;
    case bb::ImageFormat::R8G8B8_SRGB:
        return 3;
    case bb::ImageFormat::R8G8_SRGB:
        return 2;
    case bb::ImageFormat::R8_SRGB:
        return 1;

    // Linear UNORM
    case bb::ImageFormat::R8G8B8A8_UNORM:
        return 4;
    case bb::ImageFormat::R8G8B8_UNORM:
        return 3;
    case bb::ImageFormat::R8G8_UNORM:
        return 2;
    case bb::ImageFormat::R8_UNORM:
        return 1;
    case bb::ImageFormat::R16_UNORM:
        return 2;
    case bb::ImageFormat::R16G16_UNORM:
        return 4;

    // HDR / floating point
    case bb::ImageFormat::R32G32B32A32_SFLOAT:
        return 16;
    case bb::ImageFormat::R32G32B32_SFLOAT:
        return 12;
    case bb::ImageFormat::R32G32_SFLOAT:
        return 8;
    case bb::ImageFormat::R32_SFLOAT:
        return 4;
    case bb::ImageFormat::R16G16B16A16_SFLOAT:
        return 8;
    case bb::ImageFormat::R16G16B16_SFLOAT:
        return 6;
    case bb::ImageFormat::R16G16_SFLOAT:
        return 4;

    // Depth
    case bb::ImageFormat::D32_SFLOAT:
        return 4;
    case bb::ImageFormat::D24_UNORM_S8_UINT:
        return 4;
    case bb::ImageFormat::D16_UNORM:
        return 2;

    // BC compressed — these are block formats, stride is per block not per pixel
    // each block covers 4x4 pixels
    case bb::ImageFormat::BC1_RGB_SRGB:
        return 8; // 8 bytes per 4x4 block
    case bb::ImageFormat::BC3_SRGB:
        return 16; // 16 bytes per 4x4 block
    case bb::ImageFormat::BC7_SRGB:
        return 16; // 16 bytes per 4x4 block
    case bb::ImageFormat::BC5_UNORM:
        return 16; // 16 bytes per 4x4 block
    case bb::ImageFormat::BC7_UNORM:
        return 16; // 16 bytes per 4x4 block

    case bb::ImageFormat::NONE:
    default:
        assert(false && "Invalid bb::ImageFormat");
        return 0;
    }
}
}

// std::optional<bb::Texture> bb::Texture::fromImage(SingleTimeCommands& upload_commands, const Image& image, ResourceHandle<Sampler> sampler, VulkanFlags usage_flags, const char* name)
// {
//     Texture texture {};
//     texture.sampler = sampler;
//     texture.format = image.format;
//     texture.type = image.type;
//     texture.width = image.width;
//     texture.height = image.height;
//     texture.depth = image.depth;

//     if (image.mips == 0)
//     {
//         uint32_t max_dim = std::max({ image.width, image.height, image.type == ImageType::IMAGE_3D ? image.depth : 1u });
//         texture.mips = static_cast<uint32_t>(std::floor(std::log2(max_dim)) + 1);
//     }
//     else
//     {
//         texture.mips = image.mips;
//     }

//     // --- VkImage creation ---
//     vk::ImageCreateInfo texture_info {};
//     texture_info.imageType = toVkImageType(image.type);

//     texture_info.extent
//         .setWidth(image.width)
//         .setHeight(image.height)
//         .setDepth(image.type == ImageType::IMAGE_3D ? image.depth : 1);

//     texture_info.mipLevels = texture.mips;

//     if (image.type == ImageType::IMAGE_3D)
//     {
//         texture_info.arrayLayers = 1;
//     }
//     else if (image.type == ImageType::IMAGE_CUBEMAP)
//     {
//         texture_info.arrayLayers = 6;
//     }
//     else
//     {
//         texture_info.arrayLayers = image.depth;
//     }

//     texture_info.format = toVkFormat(image.format);
//     texture_info.tiling = vk::ImageTiling::eOptimal;
//     texture_info.initialLayout = vk::ImageLayout::eUndefined;
//     texture_info.sharingMode = vk::SharingMode::eExclusive;
//     texture_info.samples = vk::SampleCountFlagBits::e1;
//     texture_info.usage = vk::ImageUsageFlags(usage_flags);

//     if (image.data)
//     {
//         texture_info.usage |= vk::ImageUsageFlagBits::eTransferDst;
//         texture_info.usage |= vk::ImageUsageFlagBits::eTransferSrc;
//     }
//     if (image.type == bb::ImageType::IMAGE_CUBEMAP)
//     {
//         texture_info.flags |= vk::ImageCreateFlagBits::eCubeCompatible;
//     }

//     // --- VMA allocation ---
//     VmaAllocationCreateInfo alloc_info {};
//     alloc_info.usage = VMA_MEMORY_USAGE_AUTO;

//     VkResult result = util::vmaCreateImage(
//         upload_commands.GetContext().MemoryAllocator(),
//         reinterpret_cast<VkImageCreateInfo*>(&texture_info),
//         &alloc_info,
//         &texture.image,
//         &texture.allocation,
//         nullptr);

//     if (result != VK_SUCCESS)
//     {
//         spdlog::error("Failed to allocate image: {}", name ? name : "unnamed");
//         return std::nullopt;
//     }

//     // --- Image views ---
//     vk::ImageViewCreateInfo view_info {};
//     view_info.image = texture.image;
//     view_info.viewType = vk::ImageViewType::e2D;
//     view_info.format = texture_info.format;
//     view_info.subresourceRange.aspectMask = util::GetImageAspectFlags(texture_info.format);
//     view_info.subresourceRange.baseMipLevel = 0;
//     view_info.subresourceRange.levelCount = texture.mips;
//     view_info.subresourceRange.baseArrayLayer = 0;
//     view_info.subresourceRange.layerCount = 1;

//     vk::Device device = upload_commands.GetContext().Device();

//     for (uint32_t i = 0; i < texture_info.arrayLayers; ++i)
//     {
//         view_info.subresourceRange.baseMipLevel = 0;
//         view_info.subresourceRange.levelCount = texture.mips;
//         view_info.subresourceRange.baseArrayLayer = i;

//         LayerView& layer_view = texture.layers.emplace_back();
//         layer_view.view = device.createImageView(view_info);

//         for (uint32_t j = 0; j < texture.mips; ++j)
//         {
//             view_info.subresourceRange.baseMipLevel = j;
//             view_info.subresourceRange.levelCount = 1;
//             layer_view.mips.emplace_back(device.createImageView(view_info));
//         }
//     }

//     vk::ImageViewCreateInfo full_view {};
//     full_view.image = texture.image;
//     full_view.viewType = toVkImageViewType(image.type);
//     full_view.format = toVkFormat(image.format);
//     full_view.subresourceRange.aspectMask = util::GetImageAspectFlags(texture_info.format);
//     full_view.subresourceRange.baseMipLevel = 0;
//     full_view.subresourceRange.levelCount = texture.mips;
//     full_view.subresourceRange.baseArrayLayer = 0;
//     full_view.subresourceRange.layerCount = texture_info.arrayLayers;

//     texture.full_view = device.createImageView(full_view);

//     // --- Upload ---
//     if (image.data)
//     {
//         vk::DeviceSize imageSize = image.width * image.height * image.depth * formatStride(image.format);

//         vk::Buffer stagingBuffer {};
//         VmaAllocation stagingAllocation {};

//         util::CreateBuffer(
//             upload_commands.GetContext(),
//             imageSize,
//             vk::BufferUsageFlagBits::eTransferSrc,
//             stagingBuffer,
//             true,
//             stagingAllocation,
//             VMA_MEMORY_USAGE_CPU_ONLY,
//             "Texture staging buffer");

//         vmaCopyMemoryToAllocation(
//             upload_commands.GetContext().MemoryAllocator(),
//             image.data.get(),
//             stagingAllocation,
//             0,
//             imageSize);

//         const vk::CommandBuffer& cmd = upload_commands.CommandBuffer();

//         util::TransitionImageLayout(cmd, texture.image, toVkFormat(image.format),
//             vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);

//         util::CopyBufferToImage(cmd, stagingBuffer, texture.image, image.width, image.height);

//         vk::ImageLayout finalSrcLayout = vk::ImageLayout::eTransferDstOptimal;

//         if (texture.mips > 1)
//         {
//             util::TransitionImageLayout(cmd, texture.image, toVkFormat(image.format),
//                 vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eTransferSrcOptimal, 1, 0, 1);

//             for (uint32_t i = 1; i < texture.mips; ++i)
//             {
//                 uint32_t src = i - 1;
//                 uint32_t dst = i;

//                 auto mipSize = [](uint32_t v, uint32_t s)
//                 {
//                     return std::max(static_cast<int32_t>(v >> s), 1);
//                 };

//                 vk::ImageBlit blit {};
//                 blit.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
//                 blit.srcSubresource.layerCount = 1;
//                 blit.srcSubresource.mipLevel = src;
//                 blit.srcOffsets[1] = {
//                     .x = mipSize(image.width, src),
//                     .y = mipSize(image.height, src),
//                     .z = texture.type == ImageType::IMAGE_3D ? mipSize(image.depth, src) : 1
//                 };

//                 blit.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
//                 blit.dstSubresource.layerCount = 1;
//                 blit.dstSubresource.mipLevel = i;
//                 blit.dstOffsets[1] = {
//                     .x = mipSize(image.width, dst),
//                     .y = mipSize(image.height, dst),
//                     .z = texture.type == ImageType::IMAGE_3D ? mipSize(image.depth, dst) : 1
//                 };

//                 util::TransitionImageLayout(cmd, texture.image, toVkFormat(image.format),
//                     vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, 1, i);

//                 cmd.blitImage(
//                     texture.image, vk::ImageLayout::eTransferSrcOptimal,
//                     texture.image, vk::ImageLayout::eTransferDstOptimal,
//                     1, &blit, vk::Filter::eLinear);

//                 util::TransitionImageLayout(cmd, texture.image, toVkFormat(image.format),
//                     vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eTransferSrcOptimal, 1, i);
//             }

//             finalSrcLayout = vk::ImageLayout::eTransferSrcOptimal;
//         }

//         util::TransitionImageLayout(cmd, texture.image, toVkFormat(image.format),
//             finalSrcLayout, vk::ImageLayout::eShaderReadOnlyOptimal, 1, 0, texture.mips);

//         upload_commands.TrackAllocation(stagingAllocation, stagingBuffer);
//     }

//     auto* context = &upload_commands.GetContext();

//     std::string n = name;
//     context->DebugSetObjectName<vk::Image>(texture.image, ("[IMAGE] " + n).c_str());
//     context->DebugSetObjectName<vk::ImageView>(texture.full_view, ("[VIEW] " + n).c_str());

//     for (size_t i = 0; i < texture.layers.size(); ++i)
//     {
//         context->DebugSetObjectName<vk::ImageView>(texture.layers[i].view,
//             ("[VIEW " + std::to_string(i) + "] " + n).c_str());
//     }

//     vmaSetAllocationName(
//         context->MemoryAllocator(),
//         texture.allocation,
//         ("[ALLOCATION] " + n).c_str());

//     return texture;
// }

// void bb::Texture::destroy(VulkanContext& ctx)
// {
//     vk::Device device = ctx.Device();
//     for (auto& layer : layers)
//     {
//         for (auto& mip : layer.mips)
//             device.destroyImageView(mip);

//         device.destroyImageView(layer.view);
//     }

//     device.destroyImageView(full_view);
//     util::vmaDestroyImage(ctx.MemoryAllocator(), image, allocation);
// }
