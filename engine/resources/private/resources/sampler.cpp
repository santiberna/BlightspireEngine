#include "resources/sampler.hpp"
#include "vulkan_context.hpp"
#include "vulkan_include.hpp"

namespace
{

vk::Filter toVkFilter(bb::SamplerFilter filter)
{
    switch (filter)
    {
    case bb::SamplerFilter::LINEAR:
        return vk::Filter::eLinear;
    case bb::SamplerFilter::NEAREST:
        return vk::Filter::eNearest;
    }
}

vk::SamplerMipmapMode toVkMipmapFilter(bb::SamplerFilter filter)
{
    switch (filter)
    {
    case bb::SamplerFilter::LINEAR:
        return vk::SamplerMipmapMode::eLinear;
    case bb::SamplerFilter::NEAREST:
        return vk::SamplerMipmapMode::eNearest;
    }
}

vk::SamplerAddressMode toAddressMode(bb::SamplerAddressMode mode)
{
    switch (mode)
    {
    case bb::SamplerAddressMode::REPEAT:
        return vk::SamplerAddressMode::eRepeat;
    case bb::SamplerAddressMode::CLAMP_TO_BORDER:
        return vk::SamplerAddressMode::eClampToBorder;
    case bb::SamplerAddressMode::CLAMP_TO_EDGE:
        return vk::SamplerAddressMode::eClampToEdge;
    }
}

vk::SamplerReductionMode toVkReductionMode(bb::SamplerReductionMode mode)
{
    switch (mode)
    {
    case bb::SamplerReductionMode::WEIGHTED_AVERAGE:
        return vk::SamplerReductionMode::eWeightedAverage;
    case bb::SamplerReductionMode::MAX:
        return vk::SamplerReductionMode::eMax;
    case bb::SamplerReductionMode::MIN:
        return vk::SamplerReductionMode::eMin;
    }
}

vk::CompareOp toVkCompareOp(bb::SamplerCompareOp op)
{
    switch (op)
    {
    case bb::SamplerCompareOp::ALWAYS:
        return vk::CompareOp::eAlways;
    case bb::SamplerCompareOp::LESS_OR_EQUAL:
        return vk::CompareOp::eLessOrEqual;
    default:
        return {};
    }
}

vk::BorderColor toVkBorderColor(bb::SamplerBorderColor color)
{
    switch (color)
    {
    case bb::SamplerBorderColor::OPAQUE_BLACK_INT:
        return vk::BorderColor::eIntOpaqueBlack;
    case bb::SamplerBorderColor::OPAQUE_BLACK_FLOAT:
        return vk::BorderColor::eFloatOpaqueBlack;
    case bb::SamplerBorderColor::OPAQUE_WHITE_FLOAT:
        return vk::BorderColor::eFloatOpaqueWhite;
    }
}

}

Sampler::Sampler(const bb::SamplerCreation& creation, const VulkanContext* context)
    : _context(context)
{
    vk::StructureChain<vk::SamplerCreateInfo, vk::SamplerReductionModeCreateInfo> structureChain {};

    vk::SamplerCreateInfo& createInfo { structureChain.get<vk::SamplerCreateInfo>() };
    if (creation.useMaxAnisotropy)
    {
        vk::PhysicalDevice physical = _context->PhysicalDevice();
        auto properties = physical.getProperties();
        createInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    }

    vk::SamplerReductionModeCreateInfo& reductionModeCreateInfo { structureChain.get<vk::SamplerReductionModeCreateInfo>() };
    reductionModeCreateInfo.reductionMode = toVkReductionMode(creation.reductionMode);

    createInfo.addressModeU = toAddressMode(creation.addressModeU);
    createInfo.addressModeV = toAddressMode(creation.addressModeV);
    createInfo.addressModeW = toAddressMode(creation.addressModeW);

    createInfo.minFilter = toVkFilter(creation.minFilter);
    createInfo.magFilter = toVkFilter(creation.magFilter);
    createInfo.mipmapMode = toVkMipmapFilter(creation.mipmapMode);
    createInfo.borderColor = toVkBorderColor(creation.borderColor);

    createInfo.compareEnable = vk::False;
    if (creation.compareOp != bb::SamplerCompareOp::NONE)
    {
        createInfo.compareEnable = vk::True;
        createInfo.compareOp = toVkCompareOp(creation.compareOp);
    }

    createInfo.unnormalizedCoordinates = creation.unnormalizedCoordinates;
    createInfo.mipLodBias = creation.mipLodBias;
    createInfo.minLod = creation.minLod;
    createInfo.maxLod = creation.maxLod;

    vk::Device device = _context->Device();
    sampler = device.createSampler(createInfo).value;

    _context->DebugSetObjectName(vk::Sampler(sampler), creation.name.c_str());
}

Sampler::~Sampler()
{
    if (!_context)
    {
        return;
    }

    vk::Device device = _context->Device();
    device.destroy(sampler);
}

Sampler::Sampler(Sampler&& other) noexcept
    : sampler(other.sampler)
    , _context(other._context)
{
    other.sampler = nullptr;
    other._context = nullptr;
}

Sampler& Sampler::operator=(Sampler&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    sampler = other.sampler;
    _context = other._context;

    other.sampler = nullptr;
    other._context = nullptr;

    return *this;
}