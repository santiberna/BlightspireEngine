#include "resource_management/sampler_resource_manager.hpp"

SamplerResourceManager::SamplerResourceManager(const std::shared_ptr<VulkanContext>& context)
    : _context(context)
{
}

ResourceHandle<Sampler> SamplerResourceManager::Create(const bb::SamplerCreation& creation)
{
    return insert(Sampler { creation, _context.get() });
}
