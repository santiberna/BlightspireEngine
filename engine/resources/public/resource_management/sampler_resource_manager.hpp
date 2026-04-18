#pragma once

#include "resource_manager.hpp"
#include "resources/sampler.hpp"

#include <memory>

class VulkanContext;

class SamplerResourceManager final : public ResourceManager<Sampler>
{
public:
    explicit SamplerResourceManager(const std::shared_ptr<VulkanContext>& context);
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