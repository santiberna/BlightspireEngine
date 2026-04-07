#pragma once

#include "common.hpp"
#include "vulkan_fwd.hpp"

#include <string>

class VulkanContext;

namespace bb
{

enum class SamplerAddressMode
{
    REPEAT,
    CLAMP_TO_EDGE,
    CLAMP_TO_BORDER,
};

enum class SamplerFilter
{
    NEAREST,
    LINEAR,
};

enum class SamplerCompareOp
{
    NONE,
    ALWAYS,
    LESS_OR_EQUAL,
};

enum class SamplerBorderColor
{
    OPAQUE_BLACK_INT,
    OPAQUE_BLACK_FLOAT,
    OPAQUE_WHITE_FLOAT,
};

enum class SamplerReductionMode
{
    WEIGHTED_AVERAGE,
    MIN,
    MAX,
};

struct SamplerCreation
{
    std::string name {};

    SamplerAddressMode addressModeU { SamplerAddressMode::REPEAT };
    SamplerAddressMode addressModeW { SamplerAddressMode::REPEAT };
    SamplerAddressMode addressModeV { SamplerAddressMode::REPEAT };

    SamplerFilter minFilter { SamplerFilter::LINEAR };
    SamplerFilter magFilter { SamplerFilter::LINEAR };
    SamplerFilter mipmapMode { SamplerFilter::LINEAR };

    SamplerCompareOp compareOp { SamplerCompareOp::NONE };
    SamplerBorderColor borderColor { SamplerBorderColor::OPAQUE_BLACK_INT };
    SamplerReductionMode reductionMode { SamplerReductionMode::WEIGHTED_AVERAGE };

    bool useMaxAnisotropy { true };
    bool anisotropyEnable { true };
    bool unnormalizedCoordinates { false };

    float mipLodBias { 0.0f };
    float minLod { 0.0f };
    float maxLod { 1.0f };
};

}

struct Sampler
{
    Sampler(const bb::SamplerCreation& creation, const VulkanContext* context);
    ~Sampler();

    Sampler(Sampler&& other) noexcept;
    Sampler& operator=(Sampler&& other) noexcept;

    NON_COPYABLE(Sampler);

    VkSampler sampler;

private:
    const VulkanContext* _context;
};