#pragma once

#include "resource_manager.hpp"
#include "slot_map/container.hpp"
#include "vulkan_fwd.hpp"

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
    float maxLod { 1000.0f };
};

struct Sampler
{
    VkSampler sampler {};
};

class SamplerManager
{
public:
    explicit SamplerManager(const VulkanContext& context);
    ~SamplerManager();

    void Destroy(const ResourceHandle<bb::Sampler>& handle);
    [[nodiscard]] ResourceHandle<bb::Sampler> Create(const bb::SamplerCreation& creation, const char* name);
    [[nodiscard]] const Sampler* Access(const ResourceHandle<bb::Sampler>& handle) const
    {
        return m_storage.get(handle);
    }

    [[nodiscard]] ResourceHandle<bb::Sampler> GetDefaultSampler() const
    {
        return m_default_sampler;
    }

private:
    SlotMap<Sampler> m_storage;
    ResourceHandle<bb::Sampler> m_default_sampler;
    const VulkanContext& m_context;
};

}