#include "resources/texture.hpp"

#include "vulkan_helper.hpp"

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