#pragma once

#include "resource_manager.hpp"
#include "resources/sampler.hpp"
#include "slot_map/container.hpp"

#include <memory>

class VulkanContext;

class SamplerResourceManager
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

    const Sampler* Access(const ResourceHandle<Sampler>& handle) const
    {
        return m_storage.get(handle);
    }

    void Destroy(const ResourceHandle<Sampler>& handle)
    {
        m_storage.remove(handle);
    }

private:
    bb::SlotMap<Sampler> m_storage {};
    std::shared_ptr<VulkanContext> _context;

    ResourceHandle<Sampler> _defaultSampler;
};