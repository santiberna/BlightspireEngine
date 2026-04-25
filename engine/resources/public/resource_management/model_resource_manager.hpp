#pragma once

#include "resource_manager.hpp"
#include "resources/model.hpp"
#include "slot_map/container.hpp"

#include <memory>

class ImageResourceManager;
class MaterialResourceManager;
class MeshResourceManager;
class BatchBuffer;

class ModelResourceManager
{
public:
    ModelResourceManager(std::shared_ptr<VulkanContext> vkContext,
        std::shared_ptr<ImageResourceManager> imageResourceManager,
        std::shared_ptr<MaterialResourceManager> materialResourceManager,
        std::shared_ptr<MeshResourceManager> meshResourceManager);

    ResourceHandle<GPUModel> Create(const CPUModel& data, BatchBuffer& staticBatchBuffer, BatchBuffer& skinnedBatchBuffer);

    const GPUModel* Access(const ResourceHandle<GPUModel>& handle) const
    {
        return m_storage.get(handle);
    }

    void Destroy(const ResourceHandle<GPUModel>& handle)
    {
        m_storage.remove(handle);
    }

private:
    bb::SlotMap<GPUModel> m_storage {};
    std::shared_ptr<ImageResourceManager> _imageResourceManager;
    std::shared_ptr<MaterialResourceManager> _materialResourceManager;
    std::shared_ptr<MeshResourceManager> _meshResourceManager;
    std::shared_ptr<VulkanContext> _vkContext;
};
