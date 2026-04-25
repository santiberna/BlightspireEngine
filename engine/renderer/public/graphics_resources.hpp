#pragma once

#include "common.hpp"
#include <memory>

class VulkanContext;
class MeshResourceManager;
class MaterialResourceManager;
class BufferResourceManager;
class SamplerResourceManager;
class ModelResourceManager;
class ImageResourceManager;

class GraphicsResources
{
public:
    GraphicsResources(const std::shared_ptr<VulkanContext>& vulkanContext);
    ~GraphicsResources();

    NON_COPYABLE(GraphicsResources);
    NON_MOVABLE(GraphicsResources);

    MeshResourceManager& GetMeshResourceManager() { return *_meshResourceManager; }
    MaterialResourceManager& GetMaterialResourceManager() { return *_materialResourceManager; }
    ImageResourceManager& GetImageResourceManager() { return *_imageResourceManager; }
    BufferResourceManager& GetBufferResourceManager() { return *_bufferResourceManager; }
    SamplerResourceManager& GetSamplerResourceManager() { return *_samplerResourceManager; }
    ModelResourceManager& GetModelResourceManager() { return *_modelResourceManager; }

private:
    std::shared_ptr<VulkanContext> _vulkanContext;

    std::shared_ptr<ModelResourceManager> _modelResourceManager;
    std::shared_ptr<MeshResourceManager> _meshResourceManager;
    std::shared_ptr<MaterialResourceManager> _materialResourceManager;
    std::shared_ptr<ImageResourceManager> _imageResourceManager;
    std::shared_ptr<BufferResourceManager> _bufferResourceManager;
    std::shared_ptr<SamplerResourceManager> _samplerResourceManager;
};