#pragma once

#include "resource_manager.hpp"
#include "resources/material.hpp"
#include "slot_map/container.hpp"

#include <memory>

class ImageResourceManager;

class MaterialResourceManager
{
public:
    explicit MaterialResourceManager(const std::shared_ptr<ImageResourceManager>& ImageResourceManager);
    ResourceHandle<GPUMaterial> Create(const MaterialCreation& creation);

    const GPUMaterial* Access(const ResourceHandle<GPUMaterial>& handle) const
    {
        return m_storage.get(handle);
    }

    void Destroy(const ResourceHandle<GPUMaterial>& handle)
    {
        m_storage.remove(handle);
    }

    const bb::SlotMap<GPUMaterial>& Resources() { return m_storage; }

private:
    bb::SlotMap<GPUMaterial> m_storage {};
    std::shared_ptr<ImageResourceManager> _imageResourceManager;
};