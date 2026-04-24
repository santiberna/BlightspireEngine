#pragma once

#include "resource_manager.hpp"
#include "resources/sampler.hpp"
#include "slot_map/container.hpp"

#include <memory>

class VulkanContext;

class SamplerResourceManager final : public bb::SlotMap<Sampler>
{
public:
    explicit SamplerResourceManager(const std::shared_ptr<VulkanContext>& context);
    ~SamplerResourceManager() override = default;

    ResourceHandle<Sampler> Create(const bb::SamplerCreation& creation);

    // create default fallback sampler
    void CreateDefaultSampler()
    {
        bb::SamplerCreation info;
        info.name = "Default sampler";
        info.minLod = 0;
        info.maxLod = 32;
        _defaultSampler = Create(info);
    }

    ResourceHandle<Sampler> GetDefaultSamplerHandle() const noexcept { return _defaultSampler; };

private:
    std::shared_ptr<VulkanContext> _context;

    ResourceHandle<Sampler> _defaultSampler;
};